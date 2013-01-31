// PowerRedir.cpp : Defines the entry point for the console application.
//
#include "AppConfig.h"
#include "SimpleSocket.h"
#include "RedirEngine.h"

#include <iostream>

#include <csignal>

#define STANDARD_CONFIG_LOCATION "pskredir.conf";

using namespace std;

int loadConfiguration(int argc, char** argv, AppConfig* config);
void interruptEngine(int);

RedirEngine engine;

int main(int argc, char** argv)
{
	cout << ">>>> PskRedir - The Personal Socket Redirection service >>>>" << endl;

	int ret; // General-purpose return value

	AppConfig config;
	ret = loadConfiguration(argc, argv, &config);

	if (ret != 0) {
		return ret;
	}

	engine.setConfig(config);

	signal(SIGINT, &interruptEngine);
	signal(SIGTERM, &interruptEngine);

	ret = engine.run();
	
	return ret;
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
