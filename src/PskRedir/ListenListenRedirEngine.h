#include "AppConfig.h"
#include "BaseRedirEngine.h"

#include "SimpleSocket.h"

/**
* The implementation of RedirEngine for the Listen-Listen type of redirection.
*/
class ListenListenRedirEngine : public BaseRedirEngine {

protected:
	virtual void prepareEngine();

	/**
	* Handles the main daemon loop.
	* - Accept a client connection on port1 (if not already connected)
	* - Accept a client connection on port2 (if not already connected)
	*
	* - Exchange packages until some of the nodes closes the connection.
	* - start over...
	*/
	virtual void handleRedirection();

	virtual void interruptCurrentActivity();

	//@Override
	virtual bool handleSpecialDataOnForward(SimpleSocket::ActiveConnection& origin, SimpleSocket::ActiveConnection& dest, 
								byte* data, int dataLen);
private:
	SimpleSocket port1;
	SimpleSocket::ActiveConnection port1Conn;

	SimpleSocket port2;
	SimpleSocket::ActiveConnection port2Conn;

	bool hasDisconnectNotification;
};