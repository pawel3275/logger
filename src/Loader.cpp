#include "../include/Loader.h"
#include "../include/Config.h"

/*********************************/
/*Description:
/*	Constructor for loader, which represents parser here. Since
/*  It's main logic relies on smart loading and then parsing data
/*	To proper maps, vectors etc. hence the name loader.
/*Input:
/*	None
/*Output:
/*	None, constructor
/*********************************/
Loader::Loader()
{
	getDateFormatFromConfig();
	getDataFormatPatterns();

	// DEBUG
	Config &conf = conf.getInstance();
	if (conf.flags & conf.OpDEBUG)
	{
		cout << "DEBUG: Loader()" << endl;
	}
}

/*********************************/
/*Description:
/*	Performs clear on a file making it empty.
/*Input:
/*	None.
/*Output:
/*	None
/*********************************/
void clearFile(string filename)
{
	fstream file;
	file.open(filename, ios::trunc | ios::out);
	file.close();
}

/*********************************/
/*Description:
/*	The main function of this class. It loades the lines from a given file.
/*	If the dates are specified from which it should be loaded, then it loads
/*	only those lines. It takes a line, cuts it into smaller pieces and moves it
/*	to different containers. At the end the data is emmited outside.
/*Input:
/*	None.
/*Output:
/*	Status, 0 for full success
/*********************************/
int Loader::loadFromFileToMap()
{
	ifstream inFile;
	
	Config &conf = Config::getInstance();
	string logFilename = conf.m_logFilename;

	inFile.open(logFilename.c_str(), ios::in);
	if (!inFile.is_open())
	{
		cerr << "ERROR: Unable to open log file: " << logFilename << endl;
	}

	string logLine;
	string lastProcessedLine;
	string startDate;
	string endDate;
	int lineBuffer = conf.m_lineBuffer;
	int numberOfLine = 0;
	bool loading = false;
	bool loadAll = false;
	bool requiresMerge = false;

	if (conf.m_dateEnd != "all" || conf.m_dateStart != "all")
	{
		if (conf.m_dateEnd != "all")
		{
			endDate = conf.m_dateEnd;
		}

		if (conf.m_dateStart != "all")
		{
			startDate = conf.m_dateStart;
		}
	}
	else
	{
		loadAll = true;
		loading = true;
	}

	// DEBUG
	if (conf.flags & conf.OpDEBUG)
	{
		cout << "DEBUG: loadFromFileToMap()" << endl;
		cout << "loadAll->" << loadAll << endl;
		cout << "loading->" << loading << endl;
		cout << "startDate->" << startDate << endl;
		cout << "endDate->" << endDate << endl;
	}

	size_t position;
	// load until:
	// 1. You reach end of file 
	// 2. Buffer taken from config is not exceeded
	while (getline(inFile, logLine))
	{
		numberOfLine++;
		lastProcessedLine = logLine;

		if (conf.flags & conf.OpDEBUG)
		{
			cout << "DEBUG processing line: " << logLine << endl;
		}
		// if:
		// 1. You are not loading all
		// 2. Not loading now
		if (!loadAll && !loading)
		{
			// Start date specified in config is present at this line
			position = logLine.find(startDate);
			if (position != string::npos)
			{
				// start loading from file
				loading = true;
			}
		}

		// if:
		// 1. You are not loading all
		// 2. You are loading now
		if (!loadAll && loading)
		{
			// End date specified in config is present at this line
			position = logLine.find(endDate);
			if (position != string::npos)
			{
				// Stop loading
				loading = false;
			}
		}

		if (loading)
		{
			parseToCorrectContainer(logLine);
		}

		// Below if is for cases when we exceed line buffer number specified
		// inside config file. For each exceeding lineBuffer, we need to emit
		// gathered data to tmp file, and merge it to already created output file
		if (numberOfLine == lineBuffer)
		{
			emitLineBuffer();
			numberOfLine = 0;
			requiresMerge = true;
		}
	}

	if (numberOfLine > 0 && requiresMerge)
	{
		emitLineBuffer();
	}
	else
	{
		emitAllDataToFile();
	}

	inFile.close();
	
	if (conf.flags & conf.OpDEBUG)
	{
		cout << "Done" << endl;
	}
	return 0;
}

/*********************************/
/*Description:
/*	Function takes string of a log line inside, and cut's it by given separator 
/*	from config file declared. At the en the data is moved to container, and packed up.
/*Input:
/*	string logLine - line from the log file which we currently parse.
/*Output:
/*	Status, 0 for full success
/*********************************/
int Loader::parseToCorrectContainer(string logLine)
{
	Config &conf = conf.getInstance();
	size_t position = 0;
	char separator = conf.m_separator;
	string logLineEntryData;
	
	if (conf.flags & conf.OpDEBUG)
	{
		cout << "parseToCorrectContainer()" << endl;
		cout << "logLine->" << "logLine" << endl;
	}

	// Split line and add to correct maps
	for (auto format : conf.m_LogFormat)
	{
		position = logLine.find(separator);
		logLineEntryData = logLine.substr(0, position);
		transform(format.begin(), format.end(), format.begin(), ::toupper);

		if (format == "REMOTE_ADDR")
		{
			m_logLineValues["e_IPADDRESS"] = logLineEntryData;
		}
		else if (format == "REMOTE_USER")
		{
			m_logLineValues["e_REMOTEUSER"] = logLineEntryData;
		}
		else if (format == "TIME_LOCAL")
		{
			m_logLineValues["e_DATE"] = logLineEntryData;
		}
		else if (format == "REQUEST")
		{
			m_logLineValues["e_REQUESTTYPE"] = logLineEntryData;
		}
		else if (format == "STATUS")
		{
			m_logLineValues["e_STATUS"] = logLineEntryData;
		}
		else if (format == "BODY_BYTES_SENT")
		{
			m_logLineValues["e_BYTESSENT"] = logLineEntryData;
		}
		else if (format == "HTTP_REFERER")
		{
			m_logLineValues["e_HTTPREFERER"] = logLineEntryData;
		}
		else if (format == "HTTP_USER_AGENT")
		{
			m_logLineValues["e_HTTPUSERAGENT"] = logLineEntryData;
		}

		logLine.erase(0, position + 1); // 1 stands for separator length
	}

	
	map<string, string>::iterator it;
	for (it = m_logLineValues.begin(); it != m_logLineValues.end(); it++)
	{
		insertToCorrectMap<string, string>(it->first, it->second);
	}
	
	
	return 0;
}

/*********************************/
/*Description:
/*	Function trims up the data pattern given inside the config file.
/*Input:
/*	None
/*Output:
/*	None
/*********************************/
void Loader::getDataFormatPatterns()
{
	Config &conf = conf.getInstance();
	if (conf.flags & conf.OpDEBUG)
	{
		cout << "getDataFormatPatterns()" << endl;
	}

	string trimmedString;
	transform(m_dataPattern.begin(), m_dataPattern.end(), m_dataPattern.begin(), ::toupper);
	for (unsigned int i = 0; i < m_dataPattern.length(); i++)
	{
		if (m_dataPattern[i] != ' ')
		{
			trimmedString += m_dataPattern[i];
		}

		if (m_dataPattern[i] == ' ')
		{
			auto status = remove(trimmedString.begin(), trimmedString.end(), ' ');
			m_dateFormatPatterns.push_back(trimmedString);
			trimmedString = "";
		}
	}
}

/*********************************/
/*Description:
/*	When lineBuffer counter reaches it's point there is need to empty it.
/*	alas, this function performs emit of all gathered data to tmp, and
/*	merges it to already created output file.
/*Input:
/*	None
/*Output:
/*	None
/*********************************/
void Loader::emitLineBuffer()
{
	Merger mergerObj;
	Config &conf = Config::getInstance();
	string originalOutputfilename = conf.m_outputFilename;

	ofstream tmpFile;
	tmpFile.open("tmpFile", ios::out);
	conf.setOutputFilename("tmpFile");
	emitAllDataToFile();
	mergerObj.loadFromProcessedLog("tmpFile");
	conf.m_outputFilename = originalOutputfilename;
	mergerObj.loadFromProcessedLog(originalOutputfilename);
	clearFile(originalOutputfilename);
	mergerObj.SaveProcessedLog(originalOutputfilename);

	tmpFile.close();
	remove("tmpFile");

	emptyLocalCache();
}

/*********************************/
/*Description:
/*	Because date comes as string which needs additional trimming
/*	for the day month year etc. this whole trimming operation is done
/*	here in this function.
/*Input:
/*	string logLine - line from the log file which we currently parse with date.
/*Output:
/*	None
/*********************************/
void Loader::parseDateFromFileToMap(string logLine)
{
	vector <string> values;
	string trimmedString;
	Config &conf = conf.getInstance();

	if (conf.flags & conf.OpDEBUG)
	{
		cout << "parseDateFromFileToMap()" << endl;
		cout << "logLine->" << logLine << endl;
	}
	for (unsigned int i = 0; i <= logLine.length(); i++)
	{
		if ((logLine[i] < 65 || logLine[i] > 90) && // check for small letters
			(logLine[i] < 97 || logLine[i] > 122) && // check for big letters
			(logLine[i] < 48 || logLine[i] > 57) && // check for numbers
			(logLine[i] != '+' && logLine[i] != '-') // check for + and - (timezone)
			)
		{
			if (find(m_dateDelimeters.begin(), m_dateDelimeters.end(), logLine[i]) == m_dateDelimeters.end() &&
				logLine[i] != '\0' && logLine[i] != 32 // since you cant do != '' then literal 32 for empty char is used
				)
			{
				// save separator to the separators list if it wasn't there already
				m_dateDelimeters.push_back(logLine[i]);
			}

			values.push_back(trimmedString); // since we reached separator push back string
			replace(logLine.begin(), logLine.end(), logLine[i], ' '); // replace separator with whitespace
			trimmedString = ""; // reset trimmed string value to 0
		}
		if (logLine[i] != ' ')
		{
			trimmedString += logLine[i]; // append value to string
		}
	}

	// DEBUG
	if (conf.flags & conf.OpDEBUG)
	{
		cout << "trimmedString->" << trimmedString << endl;
	}

	setDate(m_dateFormatPatterns, values);
}

/*********************************/
/*Description:
/*	Emits all gathered data to the output file, specified within 
/*	the config file.
/*Input:
/*	None
/*Output:
/*	None
/*********************************/
void Loader::emitAllDataToFile()
{
	emitProcessedDataToFile(defaultNginxFormats::extendedNginxFormatsTypes::e_IPADDRESSEXT, m_ipAddress);
	emitProcessedDataToFile(defaultNginxFormats::extendedNginxFormatsTypes::e_DATEEXT, m_ipAddress);
	emitProcessedDataToFile(defaultNginxFormats::extendedNginxFormatsTypes::e_REQUESTTYPEEXT, m_requestType);
	emitProcessedDataToFile(defaultNginxFormats::extendedNginxFormatsTypes::e_REQUESTURLEXT, m_requestURL);
	emitProcessedDataToFile(defaultNginxFormats::extendedNginxFormatsTypes::e_STATUSEXT, m_status);
	emitProcessedDataToFile(defaultNginxFormats::extendedNginxFormatsTypes::e_BYTESSENTEXT, m_bytesSent);
	emitProcessedDataToFile(defaultNginxFormats::extendedNginxFormatsTypes::e_OPERATIONSYSYEMEXT, m_operatingSystem);
	emitProcessedDataToFile(defaultNginxFormats::extendedNginxFormatsTypes::e_WEBBROWSEREXT, m_webBrowser);
}

/*********************************/
/*Description:
/*	Clears local cache and all maps within the object of loader.
/*	all maps and containers used for emit, shall be emptied.
/*Input:
/*	None
/*Output:
/*	None
/*********************************/
void Loader::emptyLocalCache()
{
	m_date.clear();
	m_ipAddress.clear();
	m_requestType.clear();
	m_requestURL.clear();
	m_status.clear();
	m_bytesSent.clear();
	m_httpReferer.clear();
	m_httpUserAgent.clear();
	m_operatingSystem.clear();
	m_webBrowser.clear();
}

/*********************************/
/*Description:
/*	Take format which we use and check for givent format entry in map
/*	based on a given key. Depending on format different procedures
/*	are executed for each key.
/*Input:
/*	mapType format - Format of the map, for which operations around 
					 the key are performed
/*	keyType key    - The key istelf, can be parsed value from log.
/*Output:
/*	Status, 0 for full success
/*********************************/
template <typename mapType, typename keyType> 
int Loader::insertToCorrectMap(mapType format, keyType key)
{
	// DEBUG
	Config &conf = conf.getInstance();
	if (conf.flags & conf.OpDEBUG)
	{
		cout << "insertToCorrectMap()" << endl;
		cout << "format->" << format << endl;
		cout << "key->" << key << endl;
	}

	if (format == "e_IPADDRESS")
	{
		mapInsertion(m_ipAddress, key);
		return 0;
	}
	else if (format == "e_RESERVED0")
	{
		return 0;
	}
	else if (format == "e_DATE")
	{
		parseDateFromFileToMap(key);
		return 0;
	}
	else if (format == "e_REQUESTTYPE")
	{
		splitRequest(key);
		return 0;
	}
	else if (format == "e_STATUS")
	{
		mapInsertion(m_status, key);
		return 0;
	}
	else if (format == "e_BYTESSENT")
	{
		mapInsertion(m_bytesSent, key);
		return 0;
	}
	else if (format == "e_HTTPREFERER")
	{
		mapInsertion(m_httpReferer, key);
		return 0;
	}
	else if (format == "e_HTTPUSERAGENT")
	{
		setCreateWebBrowser(key);
		setCreateOperatingSystem(key);
		mapInsertion(m_httpUserAgent, key);
		return 0;
	}

	if (conf.flags & conf.OpDEBUG)
	{
		cout << "Inside insertToCorrectMap() no format found for:";
		cout << " format->" << format;
		cout << " key->" << key << endl;
	}

	return 0;
}

/*********************************/
/*Description:
/*	Split the line with the 'REQUEST' into two values. The 'type' and the 'URL'.
/*Input:
/*	string requestLine - line with request
/*Output:
/*	None
/*********************************/
void Loader::splitRequest(string requestLine)
{
	// DEBUG
	Config &conf = conf.getInstance();
	if (conf.flags & conf.OpDEBUG)
	{
		cout << "splitRequest()" << endl;
		cout << "requestLine->" << requestLine << endl;
	}

	size_t position = requestLine.find(' ');
	string requestType = requestLine.substr(0, position);
	requestLine.erase(0, position + 1);
	string requestURL = requestLine;

	mapInsertion(m_requestType, requestType);
	mapInsertion(m_requestURL, requestURL);
}

/*********************************/
/*Description:
/*	Take the format from the config file for a date. And save it up
/*	for a parser to know, which value stands in which position.
/*Input:
/*	None
/*Output:
/*	string pattern - patter for a date format
/*********************************/
string Loader::getDateFormatFromConfig()
{
	// DEBUG
	Config &conf = conf.getInstance();
	if (conf.flags & conf.OpDEBUG)
	{
		cout << "getDateFormatFromConfig()" << endl;
	}
	string dateFormat = conf.m_dateFormat;
	if (dateFormat.empty())
	{
		return m_dataPattern = "";
	}
	transform(dateFormat.begin(), dateFormat.end(), dateFormat.begin(), ::toupper);
	map <string, string>::iterator iter;

	int position;
	// find positions of each pattern
	for (iter = dataPatterns.begin(); iter != dataPatterns.end(); iter++)
	{
		position = static_cast<int>(dateFormat.find(iter->first));
		positions.insert(pair <int, string>(position, iter->first)); // save it to map in structure
	}

	// merge pattern into whole one;
	string pattern;
	map <int, string>::iterator iterForPattern;
	for (iterForPattern = positions.begin(); iterForPattern != positions.end(); iterForPattern++)
	{
		pattern += dataPatterns[iterForPattern->second];
	}
	m_dataPattern = pattern;

	// DEBUG
	if (conf.flags & conf.OpDEBUG)
	{
		cout << "pattern->" << pattern << endl;
	}
	return pattern;
}

/*********************************/
/*Description:
/*	Because we have 'raw' data about the user agent we have to deduct, which user agent
/*	occurs over here, so given the string with the agent itself we check for the presence of each 
/*	agent and increment map value with this.
/*Input:
/*	string httpUserAgent - Raw line with user agent itself
/*Output:
/*	None
/*********************************/
void Loader::setCreateOperatingSystem(string httpUserAgent)
{
	// DEBUG
	Config &conf = conf.getInstance();
	if (conf.flags & conf.OpDEBUG)
	{
		cout << "setCreateOperatingSystem()" << endl;
		cout << "httpUserAgent->" << httpUserAgent << endl;
	}

	if (httpUserAgent.find("(Win16;") != string::npos)
	{
		m_operatingSystem["Windows 3.11"]++;
	}
	else if (httpUserAgent.find("(Windows 95;") != string::npos ||
		     httpUserAgent.find("(Win95;") != string::npos || 
		     httpUserAgent.find("(Windows_95;") != string::npos
		    )
	{
		m_operatingSystem["Windows 95"]++;
	}
	else if (httpUserAgent.find("(Windows 98;") != string::npos || 
		     httpUserAgent.find("(Win98;") != string::npos
		    )
	{
		m_operatingSystem["Windows 98"]++;
	}
	else if (httpUserAgent.find("(Windows NT 5.0;") != string::npos || 
		     httpUserAgent.find("(Windows 2000;") != string::npos
		    )
	{
		m_operatingSystem["Windows 2000"]++;
	}
	else if (httpUserAgent.find("(Windows NT 5.1;") != string::npos || 
		     httpUserAgent.find("(Windows XP;") != string::npos
		    )
	{
		m_operatingSystem["Windows XP"]++;
	}
	else if (httpUserAgent.find("(Windows NT 5.2;") != string::npos)
	{
		m_operatingSystem["Windows Server 2003"]++;
	}
	else if (httpUserAgent.find("(Win16;") != string::npos)
	{
		m_operatingSystem["Windows 3.11"]++;
	}
	else if (httpUserAgent.find("(Windows NT 6.0;") != string::npos)
	{
		m_operatingSystem["Windows Vista"]++;
	}
	else if (httpUserAgent.find("(Windows NT 6.1;") != string::npos)
	{
		m_operatingSystem["Windows 7"]++;
	}
	else if (httpUserAgent.find("(Windows NT 6.2;") != string::npos)
	{
		m_operatingSystem["Windows 8"]++;
	}
	else if (httpUserAgent.find("(Windows NT 10.0;") != string::npos)
	{
		m_operatingSystem["Windows 10"]++;
	}
	else if (httpUserAgent.find("(Windows NT 4.0;") != string::npos ||
		     httpUserAgent.find("(WinNT4.0;") != string::npos       ||
		     httpUserAgent.find("(WinNT;") != string::npos          || 
		     httpUserAgent.find("(Windows NT;") != string::npos
		    )
	{
		m_operatingSystem["Windows NT 4.0"]++;
	}
	else if (httpUserAgent.find("(Windows ME;") != string::npos)
	{
		m_operatingSystem["Windows ME"]++;
	}
	else if (httpUserAgent.find("(OpenBSD;") != string::npos)
	{
		m_operatingSystem["Open BSD"]++;
	}
	else if (httpUserAgent.find("(SunOS;") != string::npos)
	{
		m_operatingSystem["Sun OS"]++;
	}
	else if (httpUserAgent.find("(Linux;") != string::npos || 
		     httpUserAgent.find("(X11;") != string::npos
		    )
	{
		m_operatingSystem["Linux"]++;
	}
	else if (httpUserAgent.find("(Mac_PowerPC;") != string::npos || 
		     httpUserAgent.find("(Macintosh;") != string::npos
		    )
	{
		m_operatingSystem["Mac OS"]++;
	}
	else if (httpUserAgent.find("(QNX;") != string::npos)
	{
		m_operatingSystem["QNX"]++;
	}
	else if (httpUserAgent.find("(BeOS;") != string::npos)
	{
		m_operatingSystem["BeOS"]++;
	}
	else if (httpUserAgent.find("(OS/2;") != string::npos)
	{
		m_operatingSystem["OS/2"]++;
	}
	else if (httpUserAgent.find("nuhk") != string::npos        || 
		httpUserAgent.find("; Googlebot/") != string::npos     || 
		httpUserAgent.find("Yammybot") != string::npos         || 
		httpUserAgent.find("Openbot") != string::npos          || 
		httpUserAgent.find("! Slurp") != string::npos          || 
		httpUserAgent.find("/msnbot") != string::npos          || 
		httpUserAgent.find("Ask Jeeves/Teoma") != string::npos || 
		httpUserAgent.find("DotBot") != string::npos           ||
		httpUserAgent.find("ia_archiver") != string::npos
	   )
	{
		m_operatingSystem["Search Bot"]++;
	}


}
/*********************************/
/*Description:
/*	Because we have 'raw' data about the user agent we have to deduct, which web browser
/*	occurs over here, so given the string with the agent itself we check for the presence of each
/*	browser and increment map value with this.
/*Input:
/*	string httpUserAgent - Raw line with user agent itself
/*Output:
/*	None
/*********************************/
void Loader::setCreateWebBrowser(string httpUserAgent)
{
	// DEBUG
	Config &conf = conf.getInstance();
	if (conf.flags & conf.OpDEBUG)
	{
		cout << "setCreateWebBrowser()" << endl;
		cout << "httpUserAgent->" << httpUserAgent << endl;
	}

	if (httpUserAgent.find("Firefox/") != string::npos && 
		httpUserAgent.find("Seamonkey/") == string::npos
	   )
	{
		m_webBrowser["Firefox"]++;
	}
	else if (httpUserAgent.find("Seamonkey/") != string::npos)
	{
		m_webBrowser["Seamonkey"]++;
	}
	else if (httpUserAgent.find("Chrome/") != string::npos && 
		     httpUserAgent.find("Chromium/") == string::npos
		    )
	{
		m_webBrowser["Chromium"]++;
	}
	else if (httpUserAgent.find("Safari/") != string::npos && 
		    (httpUserAgent.find("Chromium/") == string::npos && httpUserAgent.find("Chrome/") == string::npos)
		    )
	{
		m_webBrowser["Chromium"]++;
	}
	else if (httpUserAgent.find("OPR/") != string::npos || 
		     httpUserAgent.find("Opera/") != string::npos
		    )
	{
		m_webBrowser["Opera"]++;
	}
	else if (httpUserAgent.find("; MSIE ") != string::npos)
	{
		m_webBrowser["Internet Explorer"]++;
	}
	else if (httpUserAgent.find("Edge/") != string::npos)
	{
		m_webBrowser["Edge"]++;
	}
	else
	{
		m_webBrowser["Others"]++;
	}

}

/*********************************/
/*Description:
/*	Using the pattern we sort out the date regarding the format from congif file.
/*	After this action is performed we create new pointer to a structure with a date
/*	based on the presence of such structure with year and month, we add it up to
/*	vector with pointers to such structures or not.
/*Input:
/*	vector <string> pattern - Sorted and trimmed pattern given inside the config file.
/*	vector <string> dateDt  - The date vector in which each element represents day month year time etc.
/*Output:
/*	None
/*********************************/
void Loader::setDate(vector <string> pattern, vector <string> dateDt)
{
	// DEBUG
	Config &conf = conf.getInstance();
	if (conf.flags & conf.OpDEBUG)
	{
		cout << "setDate()" << endl;
	}

	if (!pattern.empty())
	{
		if (pattern.size() == dateDt.size())
		{
			for (unsigned int i = 0; i < pattern.size(); i++)
			{
				m_date[pattern[i]] = dateDt[i];
			}
		}
	}
	
	shared_ptr<date> ptr(new date());
	ptr->year = m_date["YYYY"];
	ptr->month = m_date["MM"];
	mapInsertion(ptr->day, m_date["DD"]);

	vector<shared_ptr<date>>::iterator foundPosition;
	foundPosition = find_if(m_dateInstancesPTR.begin(), m_dateInstancesPTR.end(), [ptr](shared_ptr <date> const &arg) 
							{ 
								return ((ptr->year == arg->year) && (ptr->month == arg->month));
							});

	if (foundPosition != m_dateInstancesPTR.end())
	{
		int index = distance(m_dateInstancesPTR.begin(), foundPosition);
		mapInsertion(m_dateInstancesPTR[index]->day, m_date["DD"]);
	}
	else
	{
		m_dateInstancesPTR.push_back(move(ptr));
	}
}

/*********************************/
/*Description:
/*	Add to a given map as first parameter to function the 'key', if this key already exists,
/*	then increment it's value by one, if it does not occur inside the 'data' map then create
/*	new entry for given 'key' with value '1'.
/*Input:
/*	map <Type1, Type2> &data - map to which we add key or update it.
/*	Type1 key                - key istelf, can be anything.
/*Output:
/*	Status, 0 for full success
/*********************************/
template<typename Type1, typename Type2>
int Loader::mapInsertion(map <Type1, Type2> &data, Type1 key)
{
	// DEBUG
	Config &conf = conf.getInstance();
	if (conf.flags & conf.OpDEBUG)
	{
		cout << "mapInsertion()" << endl;
		cout << "key->" << key << endl;
	}

	typename map <Type1, Type2>::iterator iter;
	iter = data.find(key);
	if (iter != data.end())
	{
		// key does exist, increment value by one
		iter->second++;
	}
	else
	{
		// key doesn't exist
		data.insert(pair <Type1, Type2>(key, 1));
	}

	return 0;
}

/*********************************/
/*Description:
/*	After whole data for given format is parsed, it's time to emit this to a File.
/*	This operation is done by invoking the JSON object that handles writing data to file.
/*	Each format can be handeled differently by JSON.
/*Input:
/*	defaultNginxFormats::extendedNginxFormatsTypes format - the format which we want to write.
/*	map <KeyType, ValueType> data                         - map with data that is written to file.
/*Output:
/*	Status, 0 for full success
/*********************************/
template<typename KeyType, typename ValueType>
int Loader::emitProcessedDataToFile(defaultNginxFormats::extendedNginxFormatsTypes format, map <KeyType, ValueType> data)
{
	// DEBUG
	Config &conf = conf.getInstance();
	if (conf.flags & conf.OpDEBUG)
	{
		cout << "emitProcessedDataToFile()" << endl;
		cout << "format->" << format << endl;
	}

	switch (format)
	{
	case defaultNginxFormats::e_IPADDRESS:
	{
		JsonOutput json1("ip", "IP", "IP");
		json1.writeDataToFile(data);
		break;
	}
	case defaultNginxFormats::e_REMOTEUSER:
	{
		break;
	}
	case defaultNginxFormats::e_DATE:
	{
		JsonOutput json("date", true);
		JsonOutput::writeDateToFile(move(m_dateInstancesPTR));
		break;
	}
	case defaultNginxFormats::e_REQUESTTYPE:
	{
		JsonOutput json1("request_type", "request_type", "request_type");
		json1.writeDataToFile(data);
		break;
	}

	case defaultNginxFormats::e_REQUESTURLEXT:
	{
		JsonOutput json1("request", "request", "request");
		json1.writeDataToFile(data);
		break;
	}
	
	case defaultNginxFormats::e_STATUS:
	{
		JsonOutput json1("status", "status", "status");
		json1.writeDataToFile(data);
		break;
	}
	case defaultNginxFormats::e_BYTESSENT:
	{
		JsonOutput json1("bytes_sent", "bytes_sent", "bytes_sent");
		json1.writeDataToFile(data);
		break;
	}
	case defaultNginxFormats::e_HTTPREFERER:
	{
		break;
	}
	case defaultNginxFormats::e_HTTPUSERAGENT:
	{
		break;
	}
	case defaultNginxFormats::e_OPERATIONSYSYEMEXT:
	{
		JsonOutput json1("OS", "OS", "OS");
		json1.writeDataToFile(data);
		break;
	}
	case defaultNginxFormats::e_WEBBROWSEREXT:
	{
		JsonOutput json1("web_browser", "web_browser", "web_browser");
		json1.writeDataToFile(data);
		break;
	}
	default:
		break;
	}
	return 0;
}

/*********************************/
/*Description:
/*	Destructor
/*Input:
/*	None
/*Output:
/*	None
/*********************************/
Loader::~Loader()
{
	// DEBUG
	Config &conf = conf.getInstance();
	if (conf.flags & conf.OpDEBUG)
	{
		cout << "~Loader()" << endl;
	}
}
