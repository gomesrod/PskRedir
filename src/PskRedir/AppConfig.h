
#include <string>

#ifndef POWERREDIR_APPCONFIG_H
#define POWERREDIR_APPCONFIG_H

class AppConfig
{
private:
	std::string listenIp;
	short listenPort;
	std::string forwardHost;
	short forwardPort;
	
public:
	AppConfig();
	~AppConfig();

	bool load(std::string configFilePath);

	std::string getListenIp();
	short getListenPort();
	std::string getForwardHost();
	short getForwardPort();

	std::string prettyFormat();
};

#endif