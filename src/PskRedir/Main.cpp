// Main.cpp : Defines the entry point for the console application.
//
#include "AppConfig.h"
#include "SimpleSocket.h"
#include "ListenConnRedirEngine.h"
#include "ListenListenRedirEngine.h"

#include <iostream>
#include <csignal>

using namespace std;

static const char* STANDARD_CONFIG_LOCATION = "pskredir.conf";

/**
* Loads the configuration file, based on the command-line argument.
*/
int loadConfiguration(int argc, char** argv, AppConfig* config);

/**
* Called by the signal handler to interrupt the daemon.
*/
void interruptEngine(int);

/**
*
*/
BaseRedirEngine* engine;

/**
*
*/
int main(int argc, char** argv)
{
	cout << ">>>> PskRedir - The Personal Socket Redirection service >>>>" << endl;

	AppConfig config;
	int loadConfigRet = loadConfiguration(argc, argv, &config);

	if (loadConfigRet != 0) {
		return loadConfigRet;
	}

	if (config.getMode() == AppConfig::LISTEN_CONNECT) {
		engine = new ListenConnRedirEngine;
	} else if (config.getMode() == AppConfig::LISTEN_LISTEN) {
		engine = new ListenListenRedirEngine;
	} else {
		// This is not possible,the mode was validated by the configuration parser
		throw logic_error("Unknown operation mode");
	}
	engine->setConfig(config);

	signal(SIGINT, &interruptEngine);
	signal(SIGTERM, &interruptEngine);

	int engineRet = engine->run();
	
	delete engine;
	return engineRet;
}

int loadConfiguration(int argc, char** argv, AppConfig* config)
{
	string configPath;
	if (argc == 2 && string("/?") != argv[1] && string("--help") != argv[1]) {
		configPath = argv[1];
	} else if (argc == 1) {
		configPath = STANDARD_CONFIG_LOCATION;
	} else {
		cout << "Usage: " 
			<< endl
			<< "    " << argv[0] << " <configuration_file>" 
			<< endl
			<< "        or:" << endl
			<< "    " << argv[0] << "  # no-args call: use standard file " << STANDARD_CONFIG_LOCATION
			<< endl;
		return 1;
	}

	cout << "Loading config from " << configPath << endl;
	if (!config->load(configPath)) {
		return 1;
	}
	cout << config->prettyFormat() << endl;
	return 0;
}

void interruptEngine(int sig) {
	cout << "Signal received: " << sig << endl;
	engine->interrupt();
}
