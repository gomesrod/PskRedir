#include "SimpleSocket.h"
#include "utils.h"

#include "debug.h"

#if defined(WIN32)
#include <ws2tcpip.h>
#include <winerror.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#endif

using namespace std;

const int SimpleSocket::LISTEN_BACKLOG_SIZE = 2;

#ifdef PSKDEBUG
const int SimpleSocket::RECEIVE_TIMEOUT_MICROSECS = 2000000; // Work with bigger time intervals when debugging.
#else
const int SimpleSocket::RECEIVE_TIMEOUT_MICROSECS = 1000;
#endif

#ifdef WIN32
#define ERROR_CODE_CONN_RESET WSAECONNRESET
#define ERROR_CODE_CONN_ABORTED WSAECONNABORTED
#define ERROR_CODE_BAD_SOCKET WSAEBADF
#else
#define ERROR_CODE_CONN_RESET ECONNRESET
#define ERROR_CODE_CONN_ABORTED ECONNABORTED
#define ERROR_CODE_BAD_SOCKET EBADF

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#endif

/* ********************************************************************
*   SimpleSoclet class Implementation
**********************************************************************/
SimpleSocket::SimpleSocket() :
							_listenSocketHandle(INVALID_SOCKET)
{
	///////
}

SimpleSocket::~SimpleSocket()
{
	if (_listenSocketHandle != INVALID_SOCKET) {
		closeSock(_listenSocketHandle);
	}
}

void SimpleSocket::initSocketLib()
{
#ifdef WIN32
	WSADATA winsock_info;
	if (int retcode = WSAStartup(MAKEWORD(2,0), (LPWSADATA)&winsock_info) != 0) {
		throw SocketException(pskutils::buildErrorMessage("Error initializing Socket library!", retcode));
	}
#endif
}

void SimpleSocket::cleanupSocketLib()
{
#ifdef WIN32
	WSACleanup();
#endif
}

void SimpleSocket::listen(std::string listenIp, short port) 
{
	_listenSocketHandle = socket(AF_INET, SOCK_STREAM, 0);
	if (_listenSocketHandle == INVALID_SOCKET) {
		throw SocketException(pskutils::buildErrorMessage("socket() err:", socketLastError()));
	}

	sockaddr_in listenAddr = {};
	listenAddr.sin_family = AF_INET;
	listenAddr.sin_port = htons(port);

	if (listenIp == "127.0.0.1") {
		listenAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	} else if (listenIp == "0.0.0.0") {
		listenAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	} else {
		throw SocketException(string("The ListenIP parameter currently accepts one of the following values: 0.0.0.0 or 127.0.0.1"));
	}

	int ret; // general-purpose return code.

	ret = bind(_listenSocketHandle, (const sockaddr *)&listenAddr, sizeof(sockaddr_in));
	if (ret == SOCKET_ERROR) {
		throw SocketException(pskutils::buildErrorMessage("bind() err:", socketLastError()));
	}

	::listen(_listenSocketHandle, LISTEN_BACKLOG_SIZE);
}

void SimpleSocket::stopListening()
{
	if (_listenSocketHandle != INVALID_SOCKET) {
		closeSock(_listenSocketHandle);
		_listenSocketHandle = INVALID_SOCKET;
	}
}

SimpleSocket::ActiveConnection SimpleSocket::acceptConnection() {
	sockaddr_in clientAddress;
	socklen_t clientAddressSize = sizeof clientAddress;

	SOCKET serverSocket = accept(_listenSocketHandle, reinterpret_cast<sockaddr *>(&clientAddress), &clientAddressSize);

	if (serverSocket == INVALID_SOCKET) {
	    int lastErr = socketLastError();
	    if (lastErr == ERROR_CODE_BAD_SOCKET) {
	      throw SocketException("Accept connect failed. Invalid socket");
	    } else {
	      throw SocketException(pskutils::buildErrorMessage("acceptConnection() err:", 
			lastErr));
	    }
	}

	return ActiveConnection(serverSocket);
}

SimpleSocket::ActiveConnection SimpleSocket::connectToHost(std::string addr, short port) {
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET) {
		throw SocketException(pskutils::buildErrorMessage("SimpleSocket::connectToHost socket() err:", socketLastError()));
	}

	struct addrinfo hints = {};
	hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

	struct addrinfo * servHostPtr;
    
	int addrinforetcode = getaddrinfo(addr.c_str(), pskutils::numToString<short>(port).c_str(), &hints, &servHostPtr);
	if (addrinforetcode != 0) {
		closeSock(clientSocket);
		throw SocketException(pskutils::buildErrorMessage("SimpleSocket::connectToHost getaddrinfo() err:", socketLastError()));		
	}

	int connretcode = connect(clientSocket, servHostPtr->ai_addr, sizeof(sockaddr));
	freeaddrinfo(servHostPtr);
	if (connretcode != 0) {
		closeSock(clientSocket);
		throw SocketException(pskutils::buildErrorMessage("SimpleSocket::connectToHost connect() err:", socketLastError()));
	}

	return ActiveConnection(clientSocket);
}

int SimpleSocket::socketLastError()
{
#ifdef WIN32
	return WSAGetLastError();
#else
	return errno;
#endif
}

void SimpleSocket::closeSock(SOCKET sock)
{
  #ifdef WIN32
	closesocket(sock);
#else
	close(sock);
#endif
}

/* ********************************************************************
*   SimpleSoclet::ActiveConnection class Implementation
**********************************************************************/
SimpleSocket::ActiveConnection::ActiveConnection(SOCKET sock) :
	connsocket(sock)
{
	DEBUGMSG("New ActiveConnection constructed. Sock=");
	DEBUGNUM(sock);
	if (sock != 0) {
		peerActive = true;
	}
}

SimpleSocket::ActiveConnection::ActiveConnection(const ActiveConnection& other) :
	connsocket(other.connsocket),
	peerActive(other.peerActive)
{
	DEBUGMSG("Copying connection. Getting ownership of socket:");
	DEBUGNUM(connsocket);

	// This constness break is expected, as the API for this constructor explicit says that
	// after copy-constructing the original object will be no longer
	// valid.
	(const_cast<ActiveConnection&>(other)).connsocket = INVALID_SOCKET;
}

SimpleSocket::ActiveConnection::ActiveConnection() :
	connsocket(0)
{
}

SimpleSocket::ActiveConnection& SimpleSocket::ActiveConnection::operator=(const ActiveConnection& other) 
{
	connsocket = other.connsocket;
	peerActive = other.peerActive;

	DEBUGMSG("Assigning connection. Getting ownership of socket:");
	DEBUGNUM(connsocket);

	// This constness break is expected, as the API for this method explicit says that
	// after the assignment the original object will be no longer valid.
	(const_cast<ActiveConnection&>(other)).connsocket = INVALID_SOCKET;

	return *this;
}
SimpleSocket::ActiveConnection::~ActiveConnection() {
	DEBUGMSG("~ActiveConnection destructed. CLOSE Sock=");
	DEBUGNUM(connsocket);
	if (connsocket > 0) {
		closeSock(connsocket);
	}
}

int SimpleSocket::ActiveConnection::read(byte* databuffer, int buffsize) {
	if (hasData()) {
		int size = recv(connsocket, reinterpret_cast<char *>(databuffer), buffsize, 0);
		
		DEBUGNUM(size);

		if (size < 0) {
			int errcode = socketLastError();
			if (errcode == ERROR_CODE_CONN_RESET || errcode == ERROR_CODE_CONN_ABORTED) {
				peerActive = false; // The remote size has closed the connection
				return -1;
			}

			throw SocketException(pskutils::buildErrorMessage("recv() err:", socketLastError()));
		} else if (size == 0) {
			peerActive = false;
			return -1; // The remote size has closed the connection
		} else {
			// Ok
			return size;
		}
	} else {
		DEBUGMSG("read: no data available");
		return 0; // No data
	}
}

bool SimpleSocket::ActiveConnection::hasData() {
	fd_set readfds = {0};
	FD_ZERO(&readfds);

// warning C4127: conditional expression is constant   - na macro FD_SET (VS 2010)
#pragma warning(suppress : 4127)
	FD_SET(connsocket, &readfds);

	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = RECEIVE_TIMEOUT_MICROSECS;
	
	if (select(connsocket+1, &readfds, 0, 0, &timeout) < 0) {
	  throw SocketException(pskutils::buildErrorMessage("select() err:", socketLastError()));
	}

	if (FD_ISSET(connsocket, &readfds)) {
		return true;
	} else {
		return false;
	}
}

bool SimpleSocket::ActiveConnection::sendData(const byte * data, int len) {
	// Call the send function as many times as needed, in order to
	// send the whole message.
	int offset = 0;
	int bytesToSend = len;

	while (bytesToSend > 0) {
		// The amount of bytes already sent is used as an offset to send
		// the rest of the message
		int bytesSent = send(connsocket, reinterpret_cast<const char *>(data + offset), len, 0);
		if (bytesSent < 0) {
			int errcode = socketLastError();
			if (errcode == ERROR_CODE_CONN_RESET || errcode == ERROR_CODE_CONN_ABORTED) {
				peerActive = false; // The remote size has closed the connection
				return false;
			}

			throw SocketException(pskutils::buildErrorMessage("send() err:", socketLastError()));
		}

		bytesToSend -= bytesSent;
		offset += bytesSent;
	}

	return true;
}

bool SimpleSocket::ActiveConnection::sendData(string msg) {
	return sendData(reinterpret_cast<const byte*>(msg.c_str()), msg.size()+1);
}

void SimpleSocket::ActiveConnection::destroy()
{
	if (connsocket > 0) {
		closeSock(connsocket);
		connsocket = 0;
		peerActive = false;
	}
}

SimpleSocket::ActiveConnection::operator bool()
{
	return connsocket != 0;
}

/* ********************************************************************
*   SocketException class Implementation
**********************************************************************/
SocketException::SocketException (const std::string& msg) 
	: std::runtime_error(msg)
{
}
