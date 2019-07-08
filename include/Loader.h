#pragma once
#ifndef _Loader_H
#define _Loader_H

#include <map>
#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "Config.h"
#include "JsonOutput.h"
#include "Shared.h"
#include "Merger.h"

using namespace std;

namespace defaultNginxFormats
{

	enum  defaultNginxFormatsTypes
	{
		e_IPADDRESS = 1,
		e_RESERVED0 = 2, 
		e_DATE = 3,
		e_REQUESTTYPE = 4,
		e_STATUS = 5,
		e_BYTESSENT = 6,
		e_HTTPREFERER = 7,
		e_HTTPUSERAGENT = 8
	};

	enum  extendedNginxFormatsTypes
	{
		e_IPADDRESSEXT = 1,
		e_RESERVED0EXT = 2,
		e_DATEEXT = 3,
		e_REQUESTTYPEEXT = 4,
		e_STATUSEXT = 5,
		e_BYTESSENTEXT = 6,
		e_HTTPREFEREREXT = 7,
		e_HTTPUSERAGENTEXT = 8,
		e_OPERATIONSYSYEMEXT = 9,
		e_WEBBROWSEREXT = 10,
		e_REQUESTURLEXT = 11,
	};

	//To iterate through
	static const defaultNginxFormatsTypes allDefaultNginxFormatsTypes[] =
	{
		e_IPADDRESS,
		e_RESERVED0,
		e_DATE,
		e_REQUESTTYPE,
		e_STATUS,
		e_BYTESSENT,
		e_HTTPREFERER,
		e_HTTPUSERAGENT
	};

	//To iterate through
	static const extendedNginxFormatsTypes allExtendedNginxFormatsTypes[] =
	{
		e_IPADDRESSEXT,
		e_RESERVED0EXT,
		e_DATEEXT,
		e_REQUESTTYPEEXT,
		e_STATUSEXT,
		e_BYTESSENTEXT,
		//e_RESERVED1,
		e_HTTPREFEREREXT,
		e_HTTPUSERAGENTEXT,
		e_OPERATIONSYSYEMEXT,
		e_WEBBROWSEREXT
	};
}

enum  defaultNginxDate
{
	e_DAY = 1,
	e_MONTH = 2,
	e_YEAR = 3,
	e_HOUR = 4,
	e_MINUTE = 5,
	e_SECOND = 6,
	e_TIMEZONE = 7
};

class Loader : public Config
{
public:
	//*****************************************//
	//Singleton specific below
	static Loader& getInstance()
	{
		static Loader instance; // Guaranteed to be destroyed.
							   // Instantiated on first use.
		return instance;
	}
	Loader(Loader const&) = delete;
	void operator=(Loader const&) = delete;
	//*****************************************//
	//*****************************************//
	template<typename KeyType, typename ValueType>
	int emitProcessedDataToFile(defaultNginxFormats::extendedNginxFormatsTypes format, map <KeyType, ValueType> data);
	
	template <typename mapType, typename keyType>
	int insertToCorrectMap(mapType format, keyType key);
	
	template<typename KeyType, typename ValueType>
	int mapInsertion(map <KeyType, ValueType> &data, KeyType key);

	int parseToCorrectContainer(string logLine);
	int loadFromFileToMap();
	//*****************************************//
	//*****************************************//
	void setCreateOperatingSystem(string httpUserAgent);
	void setCreateWebBrowser(string httpUserAgent);
	void setDate(vector <string> pattern, vector <string> date);
	void getDataFormatPatterns();
	void splitRequest(string requestLine);
	void parseDateFromFileToMap(string logLine);
	void emitAllDataToFile();
	void emptyLocalCache();
	void emitLineBuffer();
	//*****************************************//
	//*****************************************//
	string getDateFormatFromConfig();
	//*****************************************//
	//*****************************************//

	~Loader();

	struct time
	{
		map <string, int> hour;
		map <string, int> minute;
		map <string, int> second;
		map <string, int> timeZone;
	}timeInstance;

	vector <shared_ptr<date>> m_dateInstancesPTR;

private:
	Loader();

	vector <string> m_dateFormatPatterns;
	vector <char> m_dateDelimeters;

	map <string, string> m_date;
	map <string, int> m_ipAddress;
	map <string, int> m_requestType;
	map <string, int> m_requestURL;
	map <string, int> m_status;
	map <string, int> m_bytesSent;
	map <string, int> m_httpReferer;
	map <string, int> m_httpUserAgent;
	map <string, int> m_operatingSystem;
	map <string, int> m_webBrowser;

	map <int, string> positions;

	map <string, string> dataPatterns
	{
		{ "DAY", "dd " },
		{ "MONTH", "mm " },
		{ "YEAR", "yyyy " },
		{ "HOUR", "hh " },
		{ "MINUTE", "min " },
		{ "SECOND", "ss " },
		{ "TIMEZONE", "timezone " }
	};
		
	map < string, string > m_logLineValues
	{
		{ "e_IPADDRESS", "" },
		{ "e_RESERVED0", "" },
		{ "e_DATE", "" },
		{ "e_REQUESTTYPE", "" },
		{ "e_STATUS", "" },
		{ "e_BYTESSENT", "" },
		{ "e_HTTPREFERER", ""},
		{ "e_HTTPUSERAGENT", "" }
	};

	string m_dataPattern;
};

#endif
