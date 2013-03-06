#include "AppConfig.h"
#include "BaseRedirEngine.h"

#include "SimpleSocket.h"

/**
* The implementation of ListenConnRedirEngine for the Listen-Listen type of redirection.
*/
class ListenListenRedirEngine : public BaseRedirEngine {

protected:
	virtual void prepareEngine();
	virtual void handleRedirection();
	virtual void interruptCurrentActivity();

private:
	SimpleSocket port1;
	SimpleSocket::ActiveConnection port1Conn;

	SimpleSocket port2;
	SimpleSocket::ActiveConnection port2Conn;

	/**
	* Handles the main daemon loop.
	* - Accept a client connection on port1 (if not already connected)
	* - Accept a client connection on port2 (if not already connected)
	*
	* - Exchange packages until some of the nodes closes the connection.
	* - start over...
	*/
	void handleRedirection(SimpleSocket& server);
};