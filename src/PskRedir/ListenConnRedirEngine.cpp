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

	exchangePackagesUntilDisconnect(cliConn, forwardConn);
}

void ListenConnRedirEngine::interruptCurrentActivity() {
	server.stopListening();
}