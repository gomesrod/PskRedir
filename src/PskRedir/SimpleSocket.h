#include <string>
#include <exception>
#include <stdexcept>

#if defined(WIN32)
#include "winsock2.h"
#else
#include "sys/types.h"
#include "sys/socket.h"
#endif

#ifndef POWERREDIR_SIMPLESOCKET_H
#define POWERREDIR_SIMPLESOCKET_H

#ifndef WIN32
typedef int SOCKET;
#endif

/**
* Represents network errors.
*/
class SocketException : public std::runtime_error {
public:
	SocketException (const std::string& msg);
};

/**
* SimpleSocket: An abstraction over the most common socket operations.
*
* IMPORTANT: This class contains platform-specific code.
*/
class SimpleSocket
{
public:
	SimpleSocket();
	~SimpleSocket();

	/**
	* Initializes the socket library on the current platform.
	* Must be called prior to the creation of any socket connection.
	*/
	static void initSocketLib();

	/**
	* Do the socket library cleanup.
	*/
	static void cleanupSocketLib();

	/** 
	* Prepares this socket to work in server mode 
	*/
	void listen(std::string listenIp, short port);

	/**
	 * Forces the listening socket to close.
	 */
	void stopListening();
	
	// Forward declaration
	class ActiveConnection;

	/**
	* Waits for a new connection from a client.
	* Before this, the listen() method must have been called on this instance.
	*/
	ActiveConnection acceptConnection();

	/**
	* Starts a connection as a client.
	*/
	static ActiveConnection connectToHost(std::string address, short port);

	/*************************
	* ActiveConnection
	*
	* The nested class represents an active and usable connection.
	* Contains the read and write operations.
	*/
	class ActiveConnection {
		typedef unsigned char byte;

		friend ActiveConnection SimpleSocket::acceptConnection();
		friend ActiveConnection SimpleSocket::connectToHost(std::string address, short port);

	public:
		/**
		* Copy constructor transfers the socket ownership from the other instance.
		*
		* Important:
		* After copy-constructing, the former instance will NOT be in a valid state;
		* it no longer will hold an active network socket.
		*/
		ActiveConnection(const ActiveConnection&);

		/**
		* Destroys this object BY CLOSING THE ASSOCIATED SOCKET CONNECTION.
		*
		* (does not do anything if the ownership was transfered to other
		*  object through the copy-constructor)
		*/
		~ActiveConnection();
		
		/**
		* Tries to read from the socket.
		* Can return either of the following:
		* 
		* ->The amount of bytes read.
		* -> 0 when no data is available for reading. (this methods is "semi-non-blocking",
		*    which means, it blocks for a few milliseconds only)
		* -> -1 if the peer is gone.
		*
		* A SocketException is thrown on other errors.
		*/
		int read(byte* databuffer, int buffsize);

		/**
		* Write data to the remote host.
		* Return true if the write is sucessfull, false otherwise.
		*
		* On errors a SocketException is thrown.
		*/
		bool write(byte * data, int len);
		
		/**
		* This status is set to false when any function detects that the remote host 
		* has closed the connection
		*/
		inline bool isPeerActive();

	private:
		SOCKET connsocket;
		bool peerActive;

		/**
		* Constructable only by the friend SimpleSocket methods.
		*/
		ActiveConnection(SOCKET sock);

		// Assignment Forbidden
		ActiveConnection& operator=(ActiveConnection&);
	};
	/* **** End of ActiveConnection class *****************************/

private:
	static const int LISTEN_BACKLOG_SIZE;
	static const int RECEIVE_TIMEOUT_MICROSECS;

	SOCKET _listenSocketHandle;

	static int socketLastError();
	static void closeSock(SOCKET sock);
};

inline bool SimpleSocket::ActiveConnection::isPeerActive() {
	return peerActive;
}

#endif
