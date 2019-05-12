#pragma once
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include "Shared.h"
#include "Config.h"
#include "JsonOutput.h"

using namespace std;

class Merger
{
public:
	Merger();
	~Merger();

	void loadFromProcessedLog(string sourceOutputFile);
	void SaveProcessedLog(string sourceOutputFile);

	template<typename KeyType, typename ValueType>
	int addToMap(map <KeyType, ValueType> &data, KeyType key);

	int updateDateStruct(string year, string month, string day, string dayValue);

private:
	//Vector with pointers to date nodes.
	vector <date*> m_dateInstancesPTR;

	//Taken from config
	string m_sourceOutputFile;

	//Key to hold var name
	//Value to hold var data key and value as value
	map <string, map <string, string> > m_varData;
	
	template<typename Type1, typename Type2>
	int UpdateMapKeyValue(map<Type1, Type2>& data, Type1 key, Type2 value);

};

