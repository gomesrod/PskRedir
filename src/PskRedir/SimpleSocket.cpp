#include "SimpleSocket.h"
#include "utils.h"

#include <ws2tcpip.h>
#include <winerror.h>

using namespace std;

static const int LISTEN_BACKLOG_SIZE = 2;

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

int SimpleSocket::initSocketLib()
{
	WSADATA winsock_info;
	return WSAStartup(MAKEWORD(2,0), (LPWSADATA)&winsock_info);
}

void SimpleSocket::cleanupSocketLib()
{
	WSACleanup();
}

bool SimpleSocket::listen(std::string listenIp, short port)
{
	_listenSocketHandle = socket(AF_INET, SOCK_STREAM, 0);
	if (_listenSocketHandle == INVALID_SOCKET) {
		_lastError = myutils::buildErrorMessage("socket() err:", WSAGetLastError());
		return false;
	}

	sockaddr_in listenAddr = {};
	listenAddr.sin_family = AF_INET;
	listenAddr.sin_port = htons(port);

	if (listenIp == "127.0.0.1") {
		listenAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	} else if (listenIp == "0.0.0.0") {
		listenAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	} else {
		_lastError = "The ListenIP parameter currently accepts one of the following values: 0.0.0.0 or 127.0.0.1";
		return false;
	}

	int ret; // general-purpose return code.

	ret = bind(_listenSocketHandle, (const sockaddr *)&listenAddr, sizeof(sockaddr_in));
	if (ret == SOCKET_ERROR) {
		_lastError = myutils::buildErrorMessage("bind() err:", WSAGetLastError());
		return false;
	}

	::listen(_listenSocketHandle, LISTEN_BACKLOG_SIZE);

	return true;
}

const string& SimpleSocket::lastError()
{
	return _lastError;
}

SimpleSocket::ActiveConnection SimpleSocket::acceptConnection() {
	sockaddr_in clientAddress;
	int clientAddressSize = sizeof clientAddress;

	SOCKET serverSocket = accept(_listenSocketHandle, reinterpret_cast<sockaddr *>(&clientAddress), &clientAddressSize);

	if (serverSocket == INVALID_SOCKET) {
		_lastError = myutils::buildErrorMessage("acceptConnection() err:", WSAGetLastError());
		return ActiveConnection(0);
	}

	ActiveConnection conn(serverSocket);
	return conn;
}

SimpleSocket::ActiveConnection SimpleSocket::connectToHost(std::string addr, short port, std::string* o_errMsg) {
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET) {
		*o_errMsg = myutils::buildErrorMessage("SimpleSocket::connectToHost socket() err:", WSAGetLastError());
		return ActiveConnection(0);
	}

	struct addrinfo hints = {};
	hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

	struct addrinfo * servHostPtr;
    
	int addrinforetcode = getaddrinfo(addr.c_str(), myutils::numToString<short>(port).c_str(), &hints, &servHostPtr);
	if (addrinforetcode != 0) {
		*o_errMsg = myutils::buildErrorMessage("SimpleSocket::connectToHost getaddrinfo() err:", WSAGetLastError());
		closesocket(clientSocket);
		return ActiveConnection(0);
	}

	int connretcode = connect(clientSocket, servHostPtr->ai_addr, sizeof(sockaddr));
	freeaddrinfo(servHostPtr);
	if (connretcode != 0) {
		*o_errMsg = myutils::buildErrorMessage("SimpleSocket::connectToHost connect() err:", WSAGetLastError());
		closesocket(clientSocket);
		return ActiveConnection(0);
	}

	ActiveConnection conn(clientSocket);
	return conn;
}

SimpleSocket::ActiveConnection::ActiveConnection(SOCKET sock) :
															connsocket(sock){
	/////////
}

SimpleSocket::ActiveConnection::~ActiveConnection() {
	if (connsocket > 0) {
		closesocket(connsocket);
	}
}

bool SimpleSocket::ActiveConnection::isNull() {
	return (connsocket == 0);
}

int SimpleSocket::ActiveConnection::read(byte** o_data) {
	return 0; //TODO
}

bool SimpleSocket::ActiveConnection::write(byte * data, int len) {
	return 0; //TODO
}