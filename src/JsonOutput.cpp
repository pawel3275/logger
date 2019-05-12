//#include "../stdafx.h"
#include "../include/JsonOutput.h"

using namespace std;

/*********************************/
/*Description:
/*	Constructor for processing output of JSON file
/*Input:
/*	string varName - name for a var to put it into the file
/*	string name - name which will be put into file
/*	string label - label which will be put into file
/*	string dataStart - specifies char that starts up the data
/*	string dataEnd  - specifies char that ends up the data
/*	string varEnd - ending of a var to put in a json file
/*Output:
/*	None, this is constructor
/*********************************/
JsonOutput::JsonOutput(
	string varName, 
	string name, 
	string label,
	string dataStart,
	string dataEnd,
	string varEnd)
{
	m_varName   = "var "        + varName + " = {\n";
	m_name      = "\tname: '"   + name    + "',\n";
	m_label     = "\tlabel: '"  + label   + "',\n";
	m_dataStart = "\tdata: [\n";
	m_dataEnd   = "\t]\n";
	m_varEnd    = "};\n";

	//DEBUG
	Config &conf = Config::getInstance();
	if (conf.flags & conf.OpDEBUG)
	{
		cout << "DEBUG: JsonOutput() " << endl;
		cout << "m_varName->" << m_varName << endl;
		cout << "m_name->" << m_name << endl;
		cout << "m_label->" << m_label << endl;
		cout << "m_dataStart->" << m_dataStart << endl;
		cout << "m_dataEnd->" << m_dataEnd << endl;
		cout << "m_varEnd->" << m_varEnd << endl;
	}
}

/*********************************/
/*Description:
/*	Constructor for processing output of JSON file
/*Input:
/*	string varName - name for a var to put it into the file 
/*	bool format - option is unused for now
/*Output:
/*	None, this is constructor
/*********************************/
JsonOutput::JsonOutput(
	string varName,
	bool format)
{
	m_varName = "var " + varName + " = {\n";
	Loader &loadedData = Loader::getInstance();
	vector <date*> *m_dateInstancesPTR = loadedData.getDateInstancesPTR();
	
	//DEBUG
	Config &conf = Config::getInstance();
	if (conf.flags & conf.OpDEBUG)
	{
		cout << "DEBUG: JsonOutput() " << endl;
		cout << "m_varName->" << m_varName << endl;
		cout << "loadedData->" << &loadedData << endl;
		cout << "m_dateInstancesPTR->" << m_dateInstancesPTR << endl;
	}
}

/*********************************/
/*Description:
/*	Function takes string as a month and changes it to it's
/*  Int value, for example: January->1, May->5
/*Input:
/*	string month - month to process as a string value. 
/*Output:
/*	int month - month processed as it's int value.
/*********************************/
int JsonOutput::monthStrTomonthDec(string month)
{
	//DEBUG
	Config &conf = Config::getInstance();
	if (conf.flags & conf.OpDEBUG)
	{
		cout << "DEBUG: monthStrTomonthDec() " << endl;
		cout << "month->" << month << endl;
	}

	transform(month.begin(), month.end(), month.begin(), ::toupper);
	map<string, int> m = strMonthDec();
	return m[month];
}

/*********************************/
/*Description:
/*	Takes month as int value and changes it to it's string equivalent
/*Input:
/*	int month - month as integer
/*Output:
/*	string month - month as string value for example: "January", "May", "April" etc.
/*********************************/
string JsonOutput::monthDecTomonthStr(int month)
{
	//DEBUG
	Config &conf = Config::getInstance();
	if (conf.flags & conf.OpDEBUG)
	{
		cout << "DEBUG: monthDecTomonthStr() " << endl;
		cout << "month->" << month << endl;
	}

	map<int, string> m = DecMonthstr();
	return m[month];
}

/*********************************/
/*Description:
/*	Function returns number of days in given month.
/*Input:
/*	int month - month as integer value
/*Output:
/* int number - number of days in a given month
/*********************************/
int JsonOutput::getNumOfDays(int month)
{
	//DEBUG
	Config &conf = Config::getInstance();
	if (conf.flags & conf.OpDEBUG)
	{
		cout << "DEBUG: getNumOfDays() " << endl;
		cout << "month->" << month << endl;
	}

	map<int, int>::iterator it;
	map<int, int> m = monthNumDays();
	return m[month];
}

/*********************************/
/*Description:
/*	Use the vector with date structures and move it to a json file. 
/*	Function first checks in vector how many of unique years is present,
/*	then it checks for unique month corresnpondent with those unique years to process
/*	after that whole data is movied to json file with unique values
/*Input:
/*	vector <date*> *m_dateInstancesPTR - pointer to vector with date structures
/*Output:
/*	int status - returns status, 0 if full success
/*********************************/
int JsonOutput::writeDateToFile(vector <date*> *m_dateInstancesPTR)
{
	Config &conf = Config::getInstance();
	string outputFilename = conf.m_outputFilename;

	if (outputFilename.empty())
	{
		cout << "Error: No outputFilename specified in config" << endl;
		return 1;
	}

	//NO OUTPUT
	if (conf.flags & conf.OpNOOUT)
	{
		return 0;
	}

	ofstream file(outputFilename, ios::app | ios::out);

	if (file.is_open())
	{
		vector <string> uniqueYearsToProcess;
		vector<string>::iterator iterato;
		//Find unique values of years in dateInstances.
		for (vector<date*>::iterator pObj = m_dateInstancesPTR->begin(); pObj != m_dateInstancesPTR->end(); pObj++)
		{
			iterato = find(uniqueYearsToProcess.begin(), uniqueYearsToProcess.end(), (*pObj)->year);
			if (iterato == uniqueYearsToProcess.end())
			{
				uniqueYearsToProcess.push_back((*pObj)->year);
			}
		}
		
		file << "var dates = [" << endl;
		for (vector<string>::iterator vec = uniqueYearsToProcess.begin(); vec != uniqueYearsToProcess.end(); vec++)
		{
			vector<date*> pStructsToProcess;
			vector<int> uniqueMonths;
			//Find unique values of months in dateInstances for current processed year.
			for (vector<date*>::iterator pObj = m_dateInstancesPTR->begin(); pObj != m_dateInstancesPTR->end(); pObj++)
			{
				if ((*pObj)->year == *vec)
				{
					uniqueMonths.push_back(monthStrTomonthDec((*pObj)->month));
					pStructsToProcess.push_back(*pObj);
				}
			}

			//File output operation starts here:
			file << "\t" << *vec << ": {" << endl;
			for (unsigned short i = 1; i <= 12; i++)
			{
				file << "\t\t" << i << ": {" << endl;
				if (find(uniqueMonths.begin(), uniqueMonths.end(), i) != uniqueMonths.end())
				{
					//Scenario for month occuring in struct table.
					//FindStruct with correct month
					date* pDateStruct = nullptr;
					for (vector<date*>::iterator Obj = pStructsToProcess.begin(); Obj != pStructsToProcess.end(); Obj++)
					{
						//go through the unique months in unique objects pointer to find month-day values
						int month = monthStrTomonthDec((*Obj)->month);
						if (month == i)
						{
							pDateStruct = *Obj;
							break;
						}
					}

					//if such is found fill it out with values from Loader::date::day
					if (pDateStruct)
					{
						int month = monthStrTomonthDec((*pDateStruct).month);
						int numofDays = getNumOfDays(month);
						for (unsigned short j = 1; j <= numofDays; j++)
						{
							string strMonth = to_string(j);
							file << "\t\t\t" << j << ":" << (*pDateStruct).day[strMonth] ;
							if (j != numofDays)
							{
								file << ",";
							}
							file << endl;
						}
					}
				}
				else 
				{
					//No month present, fill out whole month with zeros.
					int numofDays = getNumOfDays(i);
					for (unsigned short j = 1; j <= numofDays; j++)
					{
						file << "\t\t\t" << j << ":" << "0";
						if (j != numofDays)
						{
							file << ",";
						}
						file << endl;
					}
				}
				file << "\t\t" << "}," << endl;
			}
			file << "\t" << "}," << endl;
		}
		file << "]" << endl;

		file.close();
	}
	return 0;
}

/*********************************/
/*Description:
/*	Take as input map with vars and process it to a JSON file.
/*Input:
/*	map <string, map <string, string> > varData - map where key is var name, and it's key is another
/*	map with value name of the var and it's count presence
/*Output:
/*	int status - returns status, 0 if full success
/*********************************/
int JsonOutput::writeVarsToFile(map <string, map <string, string> > varData)
{
	Config &conf = Config::getInstance();
	string outputFilename = conf.m_outputFilename;
	
	if (conf.flags & conf.OpDEBUG)
	{
		cout << "DEBUG: writeVarsToFile()" << endl;
		cout << "outputFilename->" << outputFilename << endl;
	}

	if (outputFilename.empty())
	{
		cout << "Error: No outputFilename specified in config" << endl;
		return 1;
	}

	//NO OUTPUT
	ofstream file(outputFilename, ios::app | ios::out);

	if (file.is_open())
	{
		if (conf.flags & conf.OpNOOUT)
		{
			return 0;
		}

		for (map<string, map<string, string> >::const_iterator ptr = varData.begin(); ptr != varData.end(); ptr++)
		{
			file << "var " << ptr->first << " = {\n";
			file << "\tname: '" << ptr->first << "',\n";
			file << "\tlabel: '" << ptr->first << "',\n";
			file << "\tdata: [\n";
			for (map<string, string>::const_iterator eptr = ptr->second.begin(); eptr != ptr->second.end(); eptr++)
			{	
				if (!eptr->first.empty())
				{
					file << "\t\t['" << eptr->first << "', [" << eptr->second << "]],\n";
				}
			}
			file << "\t]\n";
			file << "};\n";
		}
	}
	else
	{
		cerr << "ERROR: Unable to open file during Json output: " << outputFilename << endl;
	}
	return 0;
}

/*********************************/
/*Description:
/*	Destructor for JsonOutput
/*Input:
/*	None, destructor
/*Output:
/*	None, destructor
/*********************************/
JsonOutput::~JsonOutput()
{
}
