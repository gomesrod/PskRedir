#include "ListenConnRedirEngine.h"
#include "SimpleSocket.h"

#include "debug.h"

#include <iostream>
#include <fstream>

using namespace std;

void ListenConnRedirEngine::prepareEngine() {
	server.listen(config.getClientIp(), config.getClientPort());

	DEBUGMSG("IN socket listening");
}

void ListenConnRedirEngine::handleRedirection() {
	// Waits from a connection from the client.
	DEBUGMSG("Accepting connections...");
	SimpleSocket::ActiveConnection cliConn(server.acceptConnection());
	DEBUGMSG("Client connection received");

	// Start a new forward connection.
	DEBUGMSG("Connecting to forward host...");
	SimpleSocket::ActiveConnection forwardConn(SimpleSocket::connectToHost(
			config.getForwardHost(), config.getForwardPort()));

	DEBUGMSG("Connection established. Start package forwarding...");

	int databuffLen = 4096; // Grows if needed.
	BaseRedirEngine::byte* databuff = new BaseRedirEngine::byte[databuffLen];

	try {
		/* 
		* Forward data from one side to the other, until some of them closes the connection.
		*/ 
		while (cliConn.isPeerActive() && forwardConn.isPeerActive()) {
			DEBUGMSG("Polling message from cliConn");
			forwardDataIfAvailable(cliConn, forwardConn, &databuff, &databuffLen);

			if (cliConn.isPeerActive()) {
				DEBUGMSG("Polling message from forwardConn");
				forwardDataIfAvailable(forwardConn, cliConn, &databuff, &databuffLen);
			}
		}
		DEBUGMSG("Connection ended");
		delete databuff;
	} catch (...) {
		delete databuff;
		throw;
	}

}

void ListenConnRedirEngine::interruptCurrentActivity() {
	server.stopListening();
}