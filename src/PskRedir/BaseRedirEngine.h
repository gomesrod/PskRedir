#include "AppConfig.h"
#include "SimpleSocket.h"

#ifndef BASE_REDIR_ENGINE_H
#define BASE_REDIR_ENGINE_H

/**
* Contains the application core logic.
*/
class BaseRedirEngine
{
public:
	typedef unsigned char byte;

	/**
	* Injects the application configuration.
	*/
	void setConfig(const AppConfig &conf);
	
	/*
	* Starts the redirection work.
	*/
	int run();

	/**
	* Request the main loop to interrupt.
	*/
	void interrupt();

protected:

	AppConfig config;
	bool interrupted;

	/**
	* Allow implementations to prepare for the forwarding job. For example, by 
	* starting server sockets.
	*/
	virtual void prepareEngine() = 0;

	/**
	* Do the next forwarding between peers. This may include wait for the next client connection,
	* or actively connecting to the appropriate nodes.
	*
	* This method will be called in loop while the job is not interrupted.
	*/
	virtual void handleRedirection() = 0;

	/**
	* Called when the processing is interrupted, allowing the subclasses to deal with
	* any possible pendent status (for example, a listening-blocked socket)
	*/
	virtual void interruptCurrentActivity() = 0;

	/**
	* Exchange packages between the sockets, until une of them disconnects.
	*/
	void exchangePackagesUntilDisconnect(SimpleSocket::ActiveConnection& conn1, SimpleSocket::ActiveConnection& conn2);

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
};

#endif