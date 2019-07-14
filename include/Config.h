#pragma once

#ifndef _Config_H
#define _Config_H

#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <regex>
#include <sstream>
#include <filesystem>

using namespace std;

//singleton class of config.
class Config
{
public:
	//*****************************************//
	//Singleton specific below
	static Config& getInstance();
	Config(Config const&) = delete;
	void operator=(Config const&) = delete;
	//*****************************************//

	friend class Loader;
	friend class JsonOutput;

	template <typename keyType, typename variableType>
	int assaignConfigToVariables(keyType key, variableType value);
	int loadConfigFromFile();
	
	string GetOutputFilename() { return m_outputFilename; }
	void configSetup();
	void setOutputFilename(string newOutputFilename);
	void setLogFilename(string newLogFilename);
	void setConfigFilename(string newConfigGilename);
	static void genDefaultConfigFile();
	static void split(const string &line, char delim, vector<string> &elements);

	unsigned char flags;
	enum Flags
	{
		//8 bits max 0x80 - 128
		OpDEBUG = 0x01, //"00000001"
		OpPERFO = 0x02, //"00000010"
		OpNOOUT = 0x04  //"00000100"
	};

	~Config();
protected:
	string m_configFilename;
	string m_logFilename;
	string m_dateFormat;
	string m_dateStart;
	string m_dateEnd;
	string m_outputFilename;

	int m_lineBuffer;
	char m_separator;

private:
	Config();
	
};

#endif
