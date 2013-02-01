// Main.cpp : Defines the entry point for the console application.
//
#include "AppConfig.h"
#include "SimpleSocket.h"
#include "RedirEngine.h"

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
RedirEngine engine;

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

	engine.setConfig(config);

	signal(SIGINT, &interruptEngine);
	signal(SIGTERM, &interruptEngine);

	int engineRet = engine.run();
	
	return engineRet;
}

int loadConfiguration(int argc, char** argv, AppConfig* config)
{
	string configPath;
	if (argc > 1) {
		configPath = argv[1];
	} else {
		configPath = STANDARD_CONFIG_LOCATION;
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
	engine.interrupt();
}
