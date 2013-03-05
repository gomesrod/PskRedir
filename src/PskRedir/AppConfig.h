#include <string>

#ifndef POWERREDIR_APPCONFIG_H
#define POWERREDIR_APPCONFIG_H

/**
* Responsible for loading the engine configuration, and making
* this data available to the application.
*/
class AppConfig
{
private:
	std::string clientIp;
	short clientPort;
	std::string forwardHost;
	short forwardPort;
	
public:
	AppConfig();
	~AppConfig();

	/**
	* Loads the configuration from the file.
	*
	* Returns true if the file could be read and the data is valid.
	* Otherwise returns false.
	*
	* Any error will be reported through the standard error stream.
	*/
	bool load(std::string configFilePath);

	inline std::string getClientIp();
	inline short getClientPort();
	inline std::string getForwardHost();
	inline short getForwardPort();

	/**
	* Format the configuration data for display.
	*/
	std::string prettyFormat();
};

inline std::string AppConfig::getClientIp()
{
	return clientIp;
}

inline short AppConfig::getClientPort()
{
	return clientPort;
}

inline std::string AppConfig::getForwardHost()
{
	return forwardHost;
}

inline short AppConfig::getForwardPort()
{
	return forwardPort;
}

#endif