#include "ListenListenRedirEngine.h"
#include "debug.h"
#include <cstring>

using namespace std;

void ListenListenRedirEngine::prepareEngine()
{
	port1.listen(config.getFirstIp(), config.getFirstPort());
	DEBUGMSG("PORT1 socket listening");

	port2.listen(config.getSecondIp(), config.getSecondPort());
	DEBUGMSG("PORT2 socket listening");

	hasDisconnectNotification = !config.getNotification().empty();
}

void ListenListenRedirEngine::handleRedirection()
{
	if (!port1Conn) {
		DEBUGMSG("Accepting connections on Port 1...");
		port1Conn = port1.acceptConnection();
		DEBUGMSG("Connection received on Port 1.");	
	}

	if (!port2Conn) {
		DEBUGMSG("Accepting connections on Port 2...");
		port2Conn = port2.acceptConnection();
		DEBUGMSG("Connection received on Port 2.");	
	}

	DEBUGMSG("Connection established. Start package forwarding...");

	exchangePackagesUntilDisconnect(port1Conn, port2Conn);
	
	// Destroys the connection whose peer is gone
	if (!port1Conn.isPeerActive()) {
		DEBUGMSG("Destroying connection on Port 1.");
		port1Conn.destroy();
	}
	if (!port2Conn.isPeerActive()) {
		DEBUGMSG("Destroying connection on Port 2.");
		port2Conn.destroy();
	}
}

void ListenListenRedirEngine::interruptCurrentActivity()
{
	port1.stopListening();
	port2.stopListening();
}

bool ListenListenRedirEngine::handleSpecialDataOnForward(
	SimpleSocket::ActiveConnection&, SimpleSocket::ActiveConnection& dest, 
	byte* data, int dataLen)
{
	if (!hasDisconnectNotification) return true;

	if (strncmp(reinterpret_cast<const char *>(data), config.getNotification().c_str(), dataLen) == 0) {
		DEBUGMSG("DISCONNECT NOTIFICATION RECEIVED. DISCONNECTING RECEIVER...");
		dest.destroy();
		return false;
	} else {
		return true;
	}
}
