#include <string>

#include "winsock2.h"

#ifndef POWERREDIR_SIMPLESOCKET_H
#define POWERREDIR_SIMPLESOCKET_H

/**
* Abstraction over the simplest socket operations.
*/
class SimpleSocket
{
public:
	SimpleSocket();
	~SimpleSocket();

	static int initSocketLib();
	static void cleanupSocketLib();

	/** Prepares this socket to act in server mode */
	bool listen(std::string listenIp, short port);
	
	const std::string& lastError();

	class ActiveConnection;

	/**
	* Must be called in an instance with SS_SERVER status.
	*/
	ActiveConnection acceptConnection();

	/**
	*
	*/
	static ActiveConnection connectToHost(std::string address, short port, std::string* o_errMsg);

	/**
	* Contains the methods that are valid in a active, usable connection.
	*/
	class ActiveConnection {
		//typedef unsigned char byte;

		friend ActiveConnection SimpleSocket::acceptConnection();
		friend ActiveConnection connectToHost(std::string address, short port, std::string* o_errMsg);
	public:
		~ActiveConnection();
		bool isNull();
		
		/** Tries to read from the socket. Returns the amount of bytes read,
		    or -1 if the data cannot be sent (other side is gone) */
		int read(byte** o_data);

		bool write(byte * data, int len);
		
	private:
		ActiveConnection(SOCKET sock);
		SOCKET connsocket;
	};

private:
	std::string _lastError;
	SOCKET _listenSocketHandle;
};

#endif