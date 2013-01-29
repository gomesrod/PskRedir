#include "AppConfig.h"
#include "utils.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

AppConfig::AppConfig()
{
	
}

AppConfig::~AppConfig()
{
	
}

bool AppConfig::load(string configFilePath)
{
	ifstream infile(configFilePath.c_str(), ios::in);

	if (infile.fail()) {
		cerr << "Error opening the configuration file. Program will be aborted." << endl;
		return false;
	}

	string readbuff;
	bool configFound = false;

	// Looks in the file for a line properly formatted
	// LOCAL_BIND_IP LOCAL_PORT REMOTE_HOST REMOTE_PORT
	// Empty lines and comments (#) are discarded
	while (!infile.eof()) {
		getline(infile, readbuff);
		myutils::trim(readbuff);

		if (readbuff.length() != 0
				&& readbuff[0] != '#') {
			configFound = true;
			break;
		}
	}
	infile.close();

	if (!configFound) {
		cerr << "The configuration file contains no data." << endl;
		return false;
	}

	stringvector tokens;
	myutils::splitInWhitespaces(readbuff, tokens);

	if (tokens.size() != 4) {
		cerr << "Invalid configuration data: " << readbuff << endl;
		return false;
	}

	this->listenIp = tokens[0];
	this->listenPort = myutils::parseNum<short>(tokens[1]);
	this->forwardHost = tokens[2];
	this->forwardPort = myutils::parseNum<short>(tokens[3]);

	return true;
}

string AppConfig::getListenIp()
{
	return listenIp;
}

short AppConfig::getListenPort()
{
	return listenPort;
}

string AppConfig::getForwardHost()
{
	return forwardHost;
}

short AppConfig::getForwardPort()
{
	return forwardPort;
}

string AppConfig::prettyFormat()
{
	stringstream ss;
	ss << "Listen IP = " << listenIp;
	ss << ", Listen Port = " << listenPort;
	ss << ", Forward Host = " << forwardHost;
	ss << ", Forward Port = " << forwardPort;
	return ss.str();
}