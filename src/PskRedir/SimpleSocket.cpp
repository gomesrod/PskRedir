#include "SimpleSocket.h"
#include "utils.h"

#include "debug.h"

#include <ws2tcpip.h>
#include <winerror.h>

using namespace std;

const int SimpleSocket::LISTEN_BACKLOG_SIZE = 2;

#ifdef PSKDEBUG
const int SimpleSocket::RECEIVE_TIMEOUT_MICROSECS = 2000000; // Work with bigger time intervals when debugging.
#else
const int SimpleSocket::RECEIVE_TIMEOUT_MICROSECS = 1000;
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
		closesocket(_listenSocketHandle);
	}
}

void SimpleSocket::initSocketLib()
{
	WSADATA winsock_info;
	if (int retcode = WSAStartup(MAKEWORD(2,0), (LPWSADATA)&winsock_info) != 0) {
		throw SocketException(pskutils::buildErrorMessage("Error initializing Socket library!", retcode));
	}
}

void SimpleSocket::cleanupSocketLib()
{
	WSACleanup();
}

void SimpleSocket::listen(std::string listenIp, short port) 
{
	_listenSocketHandle = socket(AF_INET, SOCK_STREAM, 0);
	if (_listenSocketHandle == INVALID_SOCKET) {
		throw SocketException(pskutils::buildErrorMessage("socket() err:", WSAGetLastError()));
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
		throw SocketException(pskutils::buildErrorMessage("bind() err:", WSAGetLastError()));
	}

	::listen(_listenSocketHandle, LISTEN_BACKLOG_SIZE);
}

SimpleSocket::ActiveConnection SimpleSocket::acceptConnection() {
	sockaddr_in clientAddress;
	int clientAddressSize = sizeof clientAddress;

	SOCKET serverSocket = accept(_listenSocketHandle, reinterpret_cast<sockaddr *>(&clientAddress), &clientAddressSize);

	if (serverSocket == INVALID_SOCKET) {
		throw SocketException(pskutils::buildErrorMessage("acceptConnection() err:", WSAGetLastError()));
	}

	return ActiveConnection(serverSocket);
}

SimpleSocket::ActiveConnection SimpleSocket::connectToHost(std::string addr, short port) {
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET) {
		throw SocketException(pskutils::buildErrorMessage("SimpleSocket::connectToHost socket() err:", WSAGetLastError()));
	}

	struct addrinfo hints = {};
	hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

	struct addrinfo * servHostPtr;
    
	int addrinforetcode = getaddrinfo(addr.c_str(), pskutils::numToString<short>(port).c_str(), &hints, &servHostPtr);
	if (addrinforetcode != 0) {
		closesocket(clientSocket);
		throw SocketException(pskutils::buildErrorMessage("SimpleSocket::connectToHost getaddrinfo() err:", WSAGetLastError()));		
	}

	int connretcode = connect(clientSocket, servHostPtr->ai_addr, sizeof(sockaddr));
	freeaddrinfo(servHostPtr);
	if (connretcode != 0) {
		closesocket(clientSocket);
		throw SocketException(pskutils::buildErrorMessage("SimpleSocket::connectToHost connect() err:", WSAGetLastError()));
	}

	return ActiveConnection(clientSocket);
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
	connsocket(other.connsocket)
{
	DEBUGMSG("Copying connection. Getting ownership of socket:");
	DEBUGNUM(connsocket);

	// This constness break is expected, as the API explicit says that
	// after copy-constructing the original object will be no longer
	// valid.
	(const_cast<ActiveConnection&>(other)).connsocket = INVALID_SOCKET;
}

SimpleSocket::ActiveConnection::~ActiveConnection() {
	DEBUGMSG("~ActiveConnection destructed. CLOSE Sock=");
	DEBUGNUM(connsocket);
	if (connsocket > 0) {
		closesocket(connsocket);
	}
}

int SimpleSocket::ActiveConnection::read(byte* databuffer, int buffsize) {
	fd_set readfds = {0};
	readfds.fd_count = 1;
	readfds.fd_array[0] = connsocket;

	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = RECEIVE_TIMEOUT_MICROSECS;

	select(0, &readfds, 0, 0, &timeout);
	if (FD_ISSET(connsocket, &readfds)) {
		int size = recv(connsocket, reinterpret_cast<char *>(databuffer), buffsize, 0);
		
		DEBUGNUM(size);

		if (size < 0) {
			throw SocketException(pskutils::buildErrorMessage("recv() err:", WSAGetLastError()));
		} else if (size == 0) {
			peerActive = false;
			return -1; // The remote size has closed the connection
		} else {
			// Ok
			return size;
		}
	} else {
		DEBUGMSG("select(): no data available");
		return 0; // No data
	}
}

bool SimpleSocket::ActiveConnection::write(byte * data, int len) {
	// Call the send function as many times as needed, in order to
	// send the whole message.
	int offset = 0;
	int bytesToSend = len;

	while (bytesToSend > 0) {
		// The amount of bytes already sent is used as an offset to send
		// the rest of the message
		int bytesSent = send(connsocket, reinterpret_cast<char *>(data + offset), len, 0);
		if (bytesSent < 0) {
			int errcode = WSAGetLastError();
			if (errcode == WSAECONNRESET) {
				peerActive = false; // The remote size has closed the connection
				return false;
			}

			throw SocketException(pskutils::buildErrorMessage("send() err:", WSAGetLastError()));
		}

		bytesToSend -= bytesSent;
		offset += bytesSent;
	}

	return true;
}


/* ********************************************************************
*   SocketException class Implementation
**********************************************************************/
SocketException::SocketException (const std::string& msg) 
	: runtime_error(msg)
{
}
