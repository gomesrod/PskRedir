#include "BaseRedirEngine.h"
#include "debug.h"

#include <iostream>
using namespace std;

int BaseRedirEngine::run() {
	try {
		SimpleSocket::initSocketLib();

		this->prepareEngine();

		this->interrupted = false;
		while (!this->interrupted) {
			this->handleRedirection();
		}

	} catch (SocketException& se) {
		cerr << se.what() << endl;
		return 1;
	}

	DEBUGMSG("Interrupting services...");

	SimpleSocket::cleanupSocketLib();
	return 0;
}


void BaseRedirEngine::setConfig(const AppConfig& conf) {
	config = conf;
}

void BaseRedirEngine::interrupt() {
	this->interrupted = true;
	this->interruptCurrentActivity();
}

void BaseRedirEngine::exchangePackagesUntilDisconnect(SimpleSocket::ActiveConnection& conn1, SimpleSocket::ActiveConnection& conn2)
{
	int databuffLen = 4096; // Grows if needed.
	BaseRedirEngine::byte* databuff = new BaseRedirEngine::byte[databuffLen];

	try {
		/* 
		* Forward data from one side to the other, until some of them closes the connection.
		*/ 
		while (conn1.isPeerActive() && conn2.isPeerActive()) {
			DEBUGMSG("Polling message from cliConn");
			forwardDataIfAvailable(conn1, conn2, &databuff, &databuffLen);

			if (conn1.isPeerActive()) {
				DEBUGMSG("Polling message from forwardConn");
				forwardDataIfAvailable(conn2, conn1, &databuff, &databuffLen);
			}
		}
		DEBUGMSG("Connection ended");
		delete databuff;
	} catch (...) {
		delete databuff;
		throw;
	}
}

void BaseRedirEngine::forwardDataIfAvailable(SimpleSocket::ActiveConnection& origin, SimpleSocket::ActiveConnection& dest, byte** databuff, int* databuffLen) 
{
	bool retry = true;
	while(retry) {
		int bytesRead = origin.read(*databuff, *databuffLen);
		DEBUGNUM(bytesRead);

		if (bytesRead == -1) {
			// The connection is gone. Do nothing, the socket is already flagged (peer inactive) in order to stop processing.
			DEBUGMSG("Connection closed by remote host");
			retry = false;
		} else if (bytesRead == 0) {
			retry = false; // No data. Just wait for the next iteration
		} else if (bytesRead > *databuffLen) {
			// Alloc a larger buffer and try again
			delete[] *databuff;
			*databuffLen *= 2;
			DEBUGMSG("Reallocating buffer. NewLen:");
			DEBUGNUM(*databuffLen);
			*databuff = new byte[*databuffLen];
		} else {
			// Ok. Send the message to the other side.
			DEBUGSTR(*databuff);

			dest.write(*databuff, bytesRead);
			retry = false;
		}
	}
}