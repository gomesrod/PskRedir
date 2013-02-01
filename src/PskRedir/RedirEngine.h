#include "AppConfig.h"

#include "SimpleSocket.h"

/**
* Contains the application core logic.
*/
class RedirEngine {
private:
	AppConfig config;
	bool interrupted;

	/**
	* Handles the main daemon loop.
	* - Accept a client connection.
	* - Open a connection to the forward host.
	* - Exchange packages until some of the nodes closes the connection.
	* - Waits for a new client connection and start over...
	*/
	void handleRedirection(SimpleSocket& server);

	/**
	* If the origin socket has any data available for reading, send it to the destination socket.
	*
	* The caller is responsible for providing a data buffer. This allows the buffer to be reused
	* among subsequent calls.
	* When it's done, the caller must deallocate the buffer.
    *
	* IMPORTANT: This method can reallocate the buffer if it is not enough to hold the data. In this
	*            case the pointers databuff and databuffLen will point to the updated values.
	*            When this happens, the old buffer is automatically deallocated.
	*/
	void forwardDataIfAvailable(SimpleSocket::ActiveConnection& origin, SimpleSocket::ActiveConnection& dest,
								byte** databuff, int* databuffLen);

public:
	/**
	* Injects the application configuration.
	*/
	void setConfig(const AppConfig &conf);

	/*
	* Starts the socket server, and wait for clients.
	*/
	int run();

	/**
	* Request the main loop to interrupt.
	*/
	void interrupt();
};