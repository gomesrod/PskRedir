#include "AppConfig.h"
#include "BaseRedirEngine.h"

#include "SimpleSocket.h"

/**
* The implementation of ListenConnRedirEngine for the Listen-Connect type of redirection.
*/
class ListenConnRedirEngine : public BaseRedirEngine {

protected:
	virtual void prepareEngine();
	virtual void handleRedirection();
	virtual void interruptCurrentActivity();

private:
	SimpleSocket server;

	/**
	* Handles the main daemon loop.
	* - Accept a client connection.
	* - Open a connection to the forward host.
	* - Exchange packages until some of the nodes closes the connection.
	* - Waits for a new client connection and start over...
	*/
	void handleRedirection(SimpleSocket& server);
};