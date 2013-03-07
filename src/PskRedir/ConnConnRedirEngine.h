#include "AppConfig.h"
#include "BaseRedirEngine.h"

#include "SimpleSocket.h"

/**
* The implementation of RedirEngine for the Connect-Connect operating mode.
*/
class ConnConnRedirEngine : public BaseRedirEngine {

protected:
	virtual void prepareEngine();

	/**
	* Handles the main daemon loop.
	* - Receives the packets from the primary connection.
	* - When it gets a WAKEUP_STRING , start the secondary connection
	*
	* - Exchange packages until some of the nodes closes the connection.
	*   If the secondary host is the one that interrupted communication,
	*   just start over the main loop
	*   If the primary host ended connection, terminate.
	*/
	virtual void handleRedirection();
	virtual void interruptCurrentActivity();

private:
	SimpleSocket::ActiveConnection primaryConnection;
};