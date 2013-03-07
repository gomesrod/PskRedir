#include <string>

#ifndef POWERREDIR_APPCONFIG_H
#define POWERREDIR_APPCONFIG_H

/**
* Responsible for loading the engine configuration, and making
* this data available to the application.
*/
class AppConfig
{
public:
	enum OperationMode {LISTEN_CONNECT, LISTEN_LISTEN, CONNECT_CONNECT};

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
	
	inline OperationMode getMode();
	inline std::string getFirstIp();
	inline short getFirstPort();
	inline std::string getSecondIp();
	inline short getSecondPort();
	inline std::string getNotification();

	/**
	* Format the configuration data for display.
	*/
	std::string prettyFormat();

private:
	OperationMode mode;
	std::string firstIp;
	short firstPort;
	std::string secondIp;
	short secondPort;
	std::string notification;
	
	/**
	* Parses the string-representation of the operation mode.
	*/
	OperationMode parseMode(std::string modeStr);
};

inline AppConfig::OperationMode AppConfig::getMode()
{
	return mode;
}

inline std::string AppConfig::getFirstIp()
{
	return firstIp;
}

inline short AppConfig::getFirstPort()
{
	return firstPort;
}

inline std::string AppConfig::getSecondIp()
{
	return secondIp;
}

inline short AppConfig::getSecondPort()
{
	return secondPort;
}

inline std::string AppConfig::getNotification()
{
	return notification;
}

#endif