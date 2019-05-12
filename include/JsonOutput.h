#pragma once
#ifndef _JsonOutput_H
#define _JsonOutput_H

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "Loader.h"
#include "Shared.h"

using namespace std;

class JsonOutput
{
public:
	//For usual vars
	JsonOutput(
		string varName = "Not Specified \n",
		string name = "Not Specified \n",
		string label = "Not Specified \n",
		string dataStart = "\t data: [\n",
		string dataEnd = "\t]\n",
		string varEnd = "};");

	//For date
	JsonOutput(
		string varName = "Not Specified \n",
		bool format = true);
	
	//Return key for given STR month
	static map <string, int> strMonthDec()
	{
		map <string, int> m
		{
			{ "JAN", 1 },
			{ "FEB", 2 },
			{ "MAR", 3 },
			{ "APR", 4 },
			{ "MAY", 5 },
			{ "JUN", 6 },
			{ "JUL", 7 },
			{ "AUG", 8 },
			{ "SEP", 9 },
			{ "OCT", 10 },
			{ "NOV", 11 },
			{ "DEC", 12 }
		};
		return m;
	};

	//Return key for given DEC month
	static map <int, string> DecMonthstr()
	{
		map <int, string> m
		{
			{ 1, "JAN" },
			{ 2, "FEB" },
			{ 3, "MAR" },
			{ 4, "APR" },
			{ 5, "MAY" },
			{ 6, "JUN" },
			{ 7, "JUL" },
			{ 8, "AUG" },
			{ 9, "SEP" },
			{ 10, "OCT" },
			{ 11, "NOV" },
			{ 12, "DEC" }
		};
		return m;
	};

	//Return number of days for each month
	static map <int, int> monthNumDays()
	{
		map <int, int> m
		{
			{ 1, 31 },
			{ 2, 28},
			{ 3, 31},
			{ 4, 30},
			{ 5, 31},
			{ 6, 30},
			{ 7, 31},
			{ 8, 31},
			{ 9, 30},
			{ 10, 31},
			{ 11, 30},
			{ 12, 31}
		};
		return m;
	};

	static int writeDateToFile(vector <date*> *m_dateInstancesPTR);
	static int writeVarsToFile(map <string, map <string, string> > varData);
	static int monthStrTomonthDec(string month);
	static int getNumOfDays(int month);
	static string monthDecTomonthStr(int month);
	
	//defined here due to lnk2019 external symbol error with compiler
	template<typename keyType, typename valueType>
	inline int writeDataToFile(map<keyType, valueType> &data)
	{
		Config &conf = Config::getInstance();
		string outputFilename = conf.m_outputFilename;
		if (conf.flags & conf.OpDEBUG)
		{
			cout << "DEBUG: writeDataToFile()" << endl;
			cout << "outputFilename->" << outputFilename << endl;
		}

		if (outputFilename.empty())
		{
			cout << "Error: No outputFilename specified in config" << endl;
			return 1;
		}

		ofstream file(outputFilename, ios::app | ios::out);

		if (file.is_open())
		{
			if (conf.flags & conf.OpNOOUT)
			{
				return 0;
			}

			typename map <keyType, valueType>::iterator dataIterator;
			file << this->m_varName;
			file << this->m_name;
			file << this->m_label;
			file << this->m_dataStart;
			for (dataIterator = data.begin(); dataIterator != data.end(); dataIterator++)
			{
				file << "\t\t['" << dataIterator->first << "', [" << dataIterator->second << "]],\n";
			}
			file << this->m_dataEnd;
			file << this->m_varEnd;

		}

		file.close();
		return 0;
	}

	~JsonOutput();

private:
	//Variables specified in constructor - refer there
	string m_varName;
	string m_name;
	string m_label;
	string m_dataStart;
	string m_dataEnd;
	string m_varEnd;
};

#endif

