#include <string>
#include <exception>

#include "winsock2.h"

#ifndef POWERREDIR_SIMPLESOCKET_H
#define POWERREDIR_SIMPLESOCKET_H

class SocketException : public std::runtime_error {
public:
	SocketException (const std::string& msg);
};

/**
* Abstraction over the simplest socket operations.
*/
class SimpleSocket
{
public:
	SimpleSocket();
	~SimpleSocket();

	static void initSocketLib();
	static void cleanupSocketLib();

	/** Prepares this socket to act in server mode */
	void listen(std::string listenIp, short port);

	/**
	* Must be called in an instance with SS_SERVER status.
	*/
	SOCKET acceptConnection();

	/**
	*
	*/
	static SOCKET connectToHost(std::string address, short port);

	/*************************
	* Contains the operations that are valid in a active, usable connection.
	*/
	class ActiveConnection {
		typedef unsigned char byte;

	public:
		ActiveConnection(SOCKET sock);
		~ActiveConnection();
		
		/** Tries to read from the socket.
			Returns the amount of bytes read, 
			     -1 if the other side is gone
		*/
		int read(byte* databuffer, int buffsize);
		void write(byte * data, int len);
		
		/** Tells if any function has detected that the remote host has closed connection */
		inline bool isStillActive();

	private:
		SOCKET connsocket;
		bool stillActive;

		ActiveConnection& operator=(ActiveConnection&); // Forbidden
		ActiveConnection(const ActiveConnection&);
	};
	/* **** End of ActiveConnection class *****************************/

private:
	static const int LISTEN_BACKLOG_SIZE;
	static const int RECEIVE_TIMEOUT_MICROSECS;

	SOCKET _listenSocketHandle;
};

inline bool SimpleSocket::ActiveConnection::isStillActive() {
	return stillActive;
}

#endif