#include "AppConfig.h"

#include "SimpleSocket.h"

/**
* The application core
*/
class RedirEngine {
private:
	AppConfig config;
	bool interrupted;

	void handleRedir(SimpleSocket& server);

public:
	void setConfig(const AppConfig &conf);

	/*
	* Starts a socket server, and wait for clients.
	* Then we open our own connection to the forward address, and
	* start exchanging the packages
	* 
	*      client <---> this_app <---> forward_server
	*/
	int run();

	void interrupt();
};