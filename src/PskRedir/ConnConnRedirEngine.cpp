#include "ConnConnRedirEngine.h"
#include "debug.h"

#include <memory>

using namespace std;

void ConnConnRedirEngine::prepareEngine()
{
	DEBUGMSG("Attempting to connect to primary host...");
	primaryConnection = SimpleSocket::connectToHost(config.getFirstIp(), config.getFirstPort());
	DEBUGMSG("Connected");
}

void ConnConnRedirEngine::handleRedirection()
{
	while(!primaryConnection.hasData()) 
	{
		DEBUGMSG("Waiting for wakeup message...");
	}
	DEBUGMSG("Wakeup message received");

	SimpleSocket::ActiveConnection secConnection(SimpleSocket::connectToHost(config.getSecondIp(), config.getSecondPort()));

	DEBUGMSG("Connection established. Start package forwarding...");

	exchangePackagesUntilDisconnect(primaryConnection, secConnection);
	
	// If the primary connection is gone we can't continue to operate
	if (!primaryConnection.isPeerActive()) {
		DEBUGMSG("Primary Connection closed by peer.");
		this->interrupt();
	} else {
		// Exchange terminated by secondary host disconnection
		if (!config.getNotification().empty()) {
			DEBUGMSG("Sending notification message");
			primaryConnection.sendData(config.getNotification());
		}
	}
}

void ConnConnRedirEngine::interruptCurrentActivity()
{
	// Nothing to do
}