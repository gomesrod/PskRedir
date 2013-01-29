#include "RedirEngine.h"

#include "SimpleSocket.h"

#include <iostream>
#include <fstream>

using namespace std;

int RedirEngine::run() {
	int initSocketLibRetCode = SimpleSocket::initSocketLib();
	if (initSocketLibRetCode != 0) {
		cerr << "Error initializing socket Library! " << initSocketLibRetCode << endl;
		return 1;
	}

	SimpleSocket server;
	if (!server.listen(config.getListenIp(), config.getListenPort())) {
		cerr << "Error opening socket. " << server.lastError() << endl;
		return 1;
	}

	cout << "IN socket listening...." << endl;

	this->interrupted = false;
	while (!this->interrupted) {
		handleRedir(server);
	}

	cout << "Interrupting services..." << endl;

	SimpleSocket::cleanupSocketLib();
	return 0;
}

void RedirEngine::handleRedir(SimpleSocket& server) {
	cout << "Accepting...";
	SimpleSocket::ActiveConnection cliConn = server.acceptConnection();
	cout << "Connection Received" << endl;

	if (cliConn.isNull()) {
		cerr << "Error accepting connection. " << server.lastError() << endl;
		return;
	}

	// Start a new forward connection.
	string errMsg;

	SimpleSocket::ActiveConnection forwardConn = SimpleSocket::connectToHost(
											config.getForwardHost(), config.getForwardPort(), &errMsg);

	if (forwardConn.isNull()) {
		cerr << "Error connecting to forward host. " << errMsg << endl;
		return;
	}

	cout << "Connection established. Start package forwarding..." << endl;

	bool connectionActive = true;
	while (connectionActive) {
		
	}
}

void RedirEngine::setConfig(const AppConfig& conf) {
	config = conf;
}

void RedirEngine::interrupt() {
	this->interrupted = true;
}