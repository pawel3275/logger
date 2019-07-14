#include "../include/Merger.h"

/*********************************/
/*Description:
/*	Constructor of a merger. It;s sub module for merging two or more files into one file.
/*Input:
/*	None
/*Output:
/*	None
/*********************************/
Merger::Merger()
{
	Config &conf = Config::getInstance();
	m_sourceOutputFile = conf.GetOutputFilename();
	if (conf.flags & conf.OpDEBUG)
	{
		cout << "DEBUG: Merger()" << endl;
		cout << "m_sourceOutputFile->" << m_sourceOutputFile << endl;
	}
}

/*********************************/
/*Description:
/*	Add up value given as input to a map, if this value for given key exists
/*	then increment it by one, if not, create new pair and insert it to map
/*Input:
/*	map <KeyType, ValueType> &data - the map to which key and value is inserted/updated
/*	KeyType key                    - key to check and update in map
/*Output:
/*	status - 0 for full success
/*********************************/
template<typename KeyType, typename ValueType>
int Merger::addToMap(map <KeyType, ValueType> &data, KeyType key)
{
	typename map <KeyType, ValueType>::iterator iter;
	iter = data.find(key);
	Config &conf = conf.getInstance();
	// DEBUG
	if (conf.flags & conf.OpDEBUG)
	{
		cout << "DEBUG: addToMap()" << endl;
		cout << "key->" << key << endl;
		cout << "iter->" << iter << endl;
	}

	if (iter != data.end())
	{
		// key does exist, increment value by one
		iter->second++;
	}
	else
	{
		// key doesn't exist
		data.insert(pair <KeyType, ValueType>(key, 1));
	}

	return 0;
}

/*********************************/
/*Description:
/*	Load all lines from a JSON file and cut out informations to process it later.
/*	Depending on nesting, trim the line to get the value and key from this JSON,
/*	correlated with var name, and move it to proper data structures.
/*	Date is treated differently as it's format is different, but logic 
/*	stays the same as for the other vars.
/*Input:
/*	string sourceOutputFile - optional source for the file to merge and process
/*							  if this is not present then the value will be taken
/*							  from the configuration file.
/*Output:
/*	None
/*********************************/
void Merger::loadFromProcessedLog(string sourceOutputFile)
{
	ifstream inFile;

	// DEBUG
	Config &conf = conf.getInstance();
	if (conf.flags & conf.OpDEBUG)
	{
		cout << "DEBUG loadFromProcessedLog()" << endl;
		cout << "sourceOutputFile->" << sourceOutputFile << endl;
	}

	if (sourceOutputFile.empty())
	{
		inFile.open(m_sourceOutputFile.c_str(), ios::in);
	}
	else
	{
		inFile.open(sourceOutputFile.c_str(), ios::in);
	}

	if (!inFile.is_open())
	{
		cerr << "Error: Unable to open file during merge: " << &inFile << endl;
	}
	string inLine;
	string currentVar;
	bool loading = false;
	bool insideVar = false;
	bool insideData = false;
	bool insideDate = false;
	int nestingLevel = 0;

	string year;
	string month;
	string day;
	string dayValue;

	while (getline(inFile, inLine))
	{
		// DEBUG
		if (conf.flags & conf.OpDEBUG)
		{
			cout << "DEBUG processing line: " << inLine << endl;
		}
		// Check if we are inside date
		if (insideDate)
		{
			string parsingLine = inLine;
			parsingLine.erase(remove(parsingLine.begin(), parsingLine.end(), '{'), parsingLine.end());
			parsingLine.erase(remove(parsingLine.begin(), parsingLine.end(), '\t'), parsingLine.end());
			parsingLine.erase(remove(parsingLine.begin(), parsingLine.end(), ','), parsingLine.end());
			parsingLine.erase(remove(parsingLine.begin(), parsingLine.end(), ' '), parsingLine.end());
			if (nestingLevel != 3)
			{
				parsingLine.erase(remove(parsingLine.begin(), parsingLine.end(), ':'), parsingLine.end());

			}
			// For year
			if (nestingLevel == 1)
			{
				year = parsingLine;
			}
			// For month
			else if (nestingLevel == 2)
			{
				month = parsingLine;
			}
			// For day
			else if (nestingLevel == 3)
			{
				if (parsingLine.find(':') != string::npos)
				{
					day = parsingLine.substr(0, parsingLine.find(':'));
					dayValue = parsingLine.substr(parsingLine.find(':') + 1, parsingLine.length());
					if (dayValue != "0")
					{
						updateDateStruct(year, month, day, dayValue);
					}
				}
			}


			if (inLine.find("{") != string::npos)
			{
				nestingLevel++;
			}
			if (inLine.find("}") != string::npos || inLine.find("]") != string::npos)
			{
				nestingLevel--;
			}
			if (nestingLevel == 0)
			{
				insideDate = false;
			}
		}
		// check if currently inside var and data
		if (insideVar && insideData)
		{
			// If we are inside and data is being processed but we ecnountered data stop char as below 
			// then state that loading should stop
			if (inLine == "\t]")
			{
				insideVar = false;
				insideData = false;
			}
			// Process and get the Key:
			// Find the key value between chars
			string key;
			if (inLine.find("', ") != string::npos && inLine.find("['") != string::npos)
			{
				size_t posStart = inLine.find("['") + 2;
				size_t posEnd = inLine.find("', ");
				key = inLine.substr(posStart, posEnd - posStart);
				string concat = "['" + key + "', ";
				// Cut it out to get the value between different chars
				posStart = inLine.find(concat);
				inLine.erase(posStart, concat.length());
			}

			// Process and get Value
			// Cut out the value
			string value;
			if (inLine.find("]") != string::npos && inLine.find("[") != string::npos)
			{
				size_t posStart = inLine.find("[") + 1;
				size_t posEnd = inLine.find("]");
				value = inLine.substr(posStart, posEnd - posStart);
			}
			else
			{
				value = "1";
			}


			// Check if the value for key is present already in map for current var
			if (m_varData[currentVar][key] == "")
			{
				// Value for key does not exist
				int PrevValue = 0;
				int CurrValue = stoi(value);
				value = to_string(PrevValue + CurrValue);
			}
			else
			{
				// Value for key exists, increment it
				int PrevValue = stoi(m_varData[currentVar][key]);
				int CurrValue = stoi(value);
				value = to_string(PrevValue + CurrValue);
			}

			// Add/update map key value
			m_varData[currentVar][key] = value;
		}
		// This is for merging the date wiyh year -> month -> day 
		if (inLine.find("var dates = [") != string::npos)
		{
			insideDate = true;
			nestingLevel = 1;
		}

		// If we are currently not loading
		// And we are not loading unless we found the var
		// Code below is actually to just save the var in which we currently are
		if (!loading)
		{
			size_t posStart = inLine.find("var ");
			size_t posEnd = inLine.find(" = {");
			if (posStart == 0 && posStart != string::npos && inLine.find(" = [") == string::npos)
			{
				posStart += 4;
				insideVar = true;
				currentVar = inLine.substr(posStart, posEnd - posStart);
			}
		}

		// If we have encountered the 'meat' of the var which is the data (keys and values)
		// then let's start loading this up to the map
		if (insideVar && inLine.find("\tdata: [") != string::npos)
		{
			insideData = true;
		}

	}

	// Close the file afterwards
	inFile.close();
}

/*********************************/
/*Description:
/*	Save the vars and date, which was already processed to file.
/*	This is the last step for merger process.
/*Input:
/*	string sourceOutputFile - optional source for the file to merge and process
/*							  This variable is now not needed.
/*Output:
/*	None
/*********************************/
void Merger::SaveProcessedLog(string sourceOutputFile)
{
	//  DEBUG
	Config &conf = conf.getInstance();
	if (conf.flags & conf.OpDEBUG)
	{
		cout << "DEBUG: SaveProcessedLog()" << endl;
	}
	/*********************/
	// Movie Vars to output
	/*********************/
	JsonOutput::writeVarsToFile(m_varData);
	/*********************/
	//  Movie Date to output
	/*********************/
	for (vector <shared_ptr<date>>::iterator pObj = m_dateInstancesPTR.begin(); pObj != m_dateInstancesPTR.end(); pObj++)
	{
		int month = stoi((*pObj)->month);
		(*pObj)->month = JsonOutput::monthDecTomonthStr(month);
	}
	JsonOutput::writeDateToFile(m_dateInstancesPTR);
}

/*********************************/
/*Description:
/*	Function similar to addToMap(). here we are updating the structure of a date for given
/*  day in a month in a given year. If there was some entry for a day in a month in a year
/*	then update this value or create new one.
/*Input:
/*	string s_year - year within a structure to update
/*	string s_month - month within a structure to update
/*	string s_day - dat within a structure to update
/*	string s_dayValue - value for a given day to update or to create new entry
/*Output:
/*	Status, 0 for full success
/*********************************/
int Merger::updateDateStruct(string s_year, string s_month, string s_day, string s_dayValue)
{
	vector <shared_ptr<date>>::iterator foundPosition;
	shared_ptr <date> ptr(new date());
	ptr->year = s_year;
	ptr->month = s_month;

	if (stoi(s_day) < 10)
	{
		s_day = "0" + s_day;
	}

	foundPosition = find_if(m_dateInstancesPTR.begin(), m_dateInstancesPTR.end(), [ptr](shared_ptr <date> const &arg) 
							{ 
								return ((ptr->year == arg->year) && (ptr->month == arg->month));
							});

	//  DEBUG
	Config &conf = conf.getInstance();
	if (conf.flags & conf.OpDEBUG)
	{
		cout << "DEBUG: updateDateStruct()" << endl;
		cout << "s_year->" << s_year << endl;
		cout << "s_month->" << s_month << endl;
		cout << "s_day->" << s_day << endl;
		cout << "foundPosition->" << *foundPosition << endl;
	}
	
	if (foundPosition != m_dateInstancesPTR.end())
	{
		int index = distance(m_dateInstancesPTR.begin(), move(foundPosition));
		((*m_dateInstancesPTR[index]).day[s_day] += stoi(s_dayValue));
	}
	else
	{
		ptr->day[s_day] = stoi(s_dayValue);
		m_dateInstancesPTR.push_back(move(ptr));
	}
	
	return 0;
}

/*********************************/
/*Description:
/*	Take as input map, key and value, check the map if value for given key
/*	exists, and if it does then update it with value from input, if not
/*	then create new entry in the map with key and value from input.
/*Input:
/*	map<Type1, Type2>& data - map to search and update/add key and value
/*	Type1 key               - key to search for
/*	Type2 value             - value to add or update for existing key
/*Output:
/*	Status, 0 for full success
/*********************************/
template<typename Type1, typename Type2>
int Merger::UpdateMapKeyValue(map<Type1, Type2>& data, Type1 key, Type2 value)
{
	typename map <Type1, Type2>::iterator iter;
	iter = data.find(key);

	// DEBUG
	Config &conf = conf.getInstance();
	if (conf.flags & conf.OpDEBUG)
	{
		cout << "DEBUG: UpdateMapKeyValue()" << endl;
		cout << "key->" << key << endl;
		cout << "value->" << value << endl;
		cout << "iter->" << iter << endl;
	}

	if (iter != data.end())
	{
		// key does exist, increment value by one
		auto originalValue = iter->second;
		originalValue += value;
		iter->second = originalValue;
	}
	else
	{
		// key doesn't exist
		data.insert(pair <Type1, Type2>(key, value));
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
Merger::~Merger()
{
}
