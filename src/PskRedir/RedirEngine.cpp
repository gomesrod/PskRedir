#include "RedirEngine.h"
#include "SimpleSocket.h"

#include "debug.h"

#include <iostream>
#include <fstream>

using namespace std;

int RedirEngine::run() {
	try {
		SimpleSocket::initSocketLib();

		SimpleSocket server;
		server.listen(config.getListenIp(), config.getListenPort());

		cout << "IN socket listening...." << endl;

		this->interrupted = false;
		while (!this->interrupted) {
			handleRedir(server);
		}

	} catch (SocketException& se) {
		cerr << se.what() << endl;
		return 1;
	}

	cout << "Interrupting services..." << endl;
	SimpleSocket::cleanupSocketLib();
	return 0;
}

void RedirEngine::handleRedir(SimpleSocket& server) {
	DEBUGMSG("Accepting connections...");
	SimpleSocket::ActiveConnection cliConn(server.acceptConnection());
	DEBUGMSG("Client connection received");

	// Start a new forward connection.

	DEBUGMSG("Connecting to forward host...");
	SimpleSocket::ActiveConnection forwardConn(SimpleSocket::connectToHost(
											config.getForwardHost(), config.getForwardPort()));

	cout << "Connection established. Start package forwarding..." << endl;

	int databuffLen = 2048; // Grows as needed.
	byte* databuff = new byte[databuffLen];

	try {
		/* 
		* Forward data from one side to the other, until some of them closes the connection.
		*/ 
		while (cliConn.isStillActive() && forwardConn.isStillActive()) {
			DEBUGNUM(cliConn.isStillActive());
			DEBUGNUM(forwardConn.isStillActive());

			memset(databuff, 0, databuffLen);

			DEBUGMSG("Polling message from cliConn");
			forwardDataIfAvailable(cliConn, forwardConn, &databuff, &databuffLen);

			if (cliConn.isStillActive()) {
				DEBUGMSG("Polling message from forwardConn");
				forwardDataIfAvailable(forwardConn, cliConn, &databuff, &databuffLen);
			}
		}
		delete databuff;
	} catch (...) {
		delete databuff;
		throw;
	}
}

void RedirEngine::forwardDataIfAvailable(SimpleSocket::ActiveConnection& origin, SimpleSocket::ActiveConnection& dest, byte** databuff, int* databuffLen) {
	bool retry = true;
	while(retry) {
		int bytesRead = origin.read(*databuff, *databuffLen);
		DEBUGNUM(bytesRead);

		if (bytesRead == -1) {
			// The connection is gone. Do nothing, the connection object should be flagged in order to stop processing.
			DEBUGMSG("Connection closed by remote host");
			retry = false;
		} else if (bytesRead == 0) {
			retry = false; // No data. Just wait for the next iteration
		} else if (bytesRead > *databuffLen) {
			// Alloc a larger buffer and try again
			delete[] *databuff;
			*databuffLen *= 2;
			DEBUGMSG("Reallocating buffer. NewLen:");
			DEBUGNUM(databuffLen);
			*databuff = new byte[*databuffLen];
		} else {
			// Ok. Send the message to the other side.
			DEBUGSTR(*databuff);

			dest.write(*databuff, bytesRead);
			retry = false;
		}
	}
}

void RedirEngine::setConfig(const AppConfig& conf) {
	config = conf;
}

void RedirEngine::interrupt() {
	this->interrupted = true;
}