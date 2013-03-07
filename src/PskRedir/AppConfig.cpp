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
		pskutils::trim(readbuff);

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

	vector<string> tokens;
	pskutils::splitInWhitespaces(readbuff, tokens);

	if (tokens.size() < 5 || tokens.size() > 6) {
		cerr << "Invalid configuration data: " << readbuff << endl;
		return false;
	}

	try {
		this->mode = parseMode(tokens[0]);
	} catch (runtime_error& err) {
		cerr << err.what() << endl;
		return false;
	}
	this->firstIp = tokens[1];
	this->firstPort = pskutils::parseNum<short>(tokens[2]);
	this->secondIp = tokens[3];
	this->secondPort = pskutils::parseNum<short>(tokens[4]);
	if (tokens.size() == 6) {
		this->notification = tokens[5];
	}

	return true;
}

string AppConfig::prettyFormat()
{
	stringstream ss;
	ss << "From: " << firstIp;
	ss << ", Port= " << firstPort;
	ss << " >> ";
	ss << "To: " << secondIp;
	ss << ", Port= " << secondPort;
	if (!notification.empty()) {
		ss << " ;Notification= " << notification;
	}
	return ss.str();
}

AppConfig::OperationMode AppConfig::parseMode(std::string modeStr)
{
	if (string("LISTEN_CONN") == modeStr) {
		return LISTEN_CONNECT;
	} else if (string("LISTEN_LISTEN") == modeStr) {
		return LISTEN_LISTEN;
	} else if (string("CONN_CONN") == modeStr) {
		return CONNECT_CONNECT;
	} else {
		throw runtime_error(string("Invalid operation mode: ").append(modeStr));
	}
}