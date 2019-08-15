#include "../include/Config.h"

using namespace std;

/*********************************/
/*Description:
/*	Constructor of the singleton configuration object.
/*Input:
/*	None
/*Output:
/*	None
/*********************************/
Config::Config()
{
	m_defaultLogFormat = "$remote_addr\"$remote_user\"$time_local\"$request\"$status\"$body_bytes_sent\"$http_referer\"$http_user_agent;";
}

/*********************************/
/*Description:
/*	Split the 'line' by the 'delimeter' and move trimmed data to vector with 'elements'
/*Input:
/*	const string &line       - line to split.
/*	char delim			     - delimeter by which split is done
/*	vector<string> &elements - vector with trimmed elements
/*Output:
/*	None
/*********************************/
void Config::split(const string &line, char delim, vector<string> &elements)
{
	size_t DelimPosition = line.find(delim);

	if (DelimPosition == 0 || DelimPosition == string::npos)
	{
		return;
	}
	elements.push_back(line.substr(0, DelimPosition - 1));
	elements.push_back(line.substr(DelimPosition + 2));
}

/*********************************/
/*Description:
/*	Returns the instance of singleton object.
/*Input:
/*	None
/*Output:
/*	None
/*********************************/
Config & Config::getInstance()
{
	static Config instance;
	return instance;
}

/*********************************/
/*Description:
/*	Opens up config file and reads up the lines setting up variables.
/*	If some varaible is uninitialised then the default ones are used.
/*Input:
/*	None
/*Output:
/*	status, 0 for full success
/*********************************/
int Config::loadConfigFromFile()
{
	ifstream inFile;
	inFile.open(m_configFilename.c_str());

	if (!inFile.is_open())
	{
		if (flags & OpDEBUG)
		{
			cout << "DEBUG: searching for config... " << endl;
		}
		// Check for default lower case filename...
		transform(m_configFilename.begin(), m_configFilename.end(), m_configFilename.begin(), ::tolower);
		inFile.open(m_configFilename.c_str());
		if (!inFile.is_open())
		{
			// Check for default config...
			setConfigFilename("config.cfg");
			inFile.open(m_configFilename.c_str());
			if (!inFile.is_open())
			{
				cerr << "ERROR: Unable to open config file: " << m_configFilename << endl;
				return 1;
			}
		}
	}
	string configLine;

	while (getline(inFile, configLine))
	{
		if (configLine[0] == '#' || configLine.empty())
		{
			//skip comments inside config file
			continue;
		}
		vector<string> lineElements;
		split(configLine, '=', lineElements);

		if (!lineElements[0].empty() && !lineElements[1].empty())
		{
			assaignConfigToVariables(lineElements[0], lineElements[1]);
		}
	}
	return 0;
}

/*********************************/
/*Description:
/*	Takes up read values from file and assaigns them to proper values
/*	inside the config object.
/*Input:
/*	keyType key        - key from config file
/*	variableType value - value from config file
/*Output:
/*	status, 0 for full success
/*********************************/
template <typename keyType, typename variableType>
int Config::assaignConfigToVariables(keyType key, variableType value)
{
	// make loaded value from key uppercase
	transform(key.begin(), key.end(), key.begin(), ::toupper);

	if (key == "SEPARATOR" && value.length() > 0)
	{
		m_separator = value[0]; // Take upon consideration only one char
	}

	if (key == "LINEBUFFER" && value.length() > 0)
	{
		m_lineBuffer = stoi(value); // convert int to string with stoi
	}

	if (key == "LOG_FILENAME" && value.length() > 0 && m_logFilename.empty())
	{
		m_logFilename = value; // can be provided via cmd
	}
	
	if (key == "DATE_FORMAT" && value.length() > 0)
	{
		m_dateFormat = value; //  keep in mind date format still needs to be parsed.
	}

	if (key == "DATE_START" && value.length() > 0)
	{
		m_dateStart = value;
	}
	if (key == "DATE_END" && value.length() > 0)
	{
		m_dateEnd = value;
	}
	if (key == "OUTPUT_FILENAME" && value.length() > 0 && m_outputFilename.empty())
	{
		try
		{
			using namespace std::experimental::filesystem;
			{
				path directory(value);
				path p(value);
				path dir = p.parent_path();
				if (!exists(dir))
				{
					create_directories(dir);
				}
			}
		}
		catch (...)
		{
			cerr << "ERROR: ENABLE TO CREATE DIRECTORY FOR OUTPUT FILE" << endl;
		}

		m_outputFilename = value; // can be provided via cmd
	}

	if (key == "NGINX_LOG_FORMAT" && value.length() > 0)
	{
		verifyAndSetLogFormat(value);
	}

	if (flags & OpDEBUG)
	{
		cout << "Config: \"" << key << "\" equals to: \"" << value << "\"" << endl;
	}
	return 0;
}

/*********************************/
/*Description:
/*	Set up Config object initialising all neccesary values and load whole file.
/*Input:
/*	None
/*Output:
/*	None
/*********************************/
void Config::configSetup()
{
	int status;
	if (m_configFilename.empty())
	{
		cout << "No config file specified, using default one: ";
		cout << "config.cfg" << endl;
		m_configFilename = "config.cfg";

		status = loadConfigFromFile();
	}
	else
	{
		cout << "Debug: Config filename has been specified. Using:" << m_configFilename << endl;
		status = loadConfigFromFile();
	}

	if (status != 0)
	{
		cerr << "ERROR: Load of the Config file has ecountered Error!" << endl;
		exit(-1);
	}
}

/*********************************/
/*Description:
/*	Setter for filename variable, which is used during write to file operations
/*Input:
/*	string newOutputFilename - new filename that is set up.
/*Output:
/*	None
/*********************************/
void Config::setOutputFilename(string newOutputFilename)
{
	if (!newOutputFilename.empty())
	{
		Config &conf = Config::getInstance();
		conf.m_outputFilename = newOutputFilename;
	}
}

/*********************************/
/*Description:
/*	Setter for log filename variable, which is used during load from log operations
/*Input:
/*	string newLogFilename - new filename.
/*Output:
/*	None
/*********************************/
void Config::setLogFilename(string newLogFilename)
{
	if (!newLogFilename.empty())
	{
		Config &conf = Config::getInstance();
		conf.m_logFilename = newLogFilename;
	}
}

/*********************************/
/*Description:
/*	Setter for configuration filename.
/*Input:
/*	string newConfigGilename - new filename that is set up.
/*Output:
/*	None
/*********************************/
void Config::setConfigFilename(string newConfigGilename)
{
	if (!newConfigGilename.empty())
	{
		m_configFilename = newConfigGilename;
	}
}

void Config::verifyAndSetLogFormat(string value)
{
	value.erase(remove(value.begin(), value.end(), '$'), value.end());
	value.erase(remove(value.begin(), value.end(), ';'), value.end());

	size_t position = value.find(this->m_separator);
	if (position == string::npos)
	{
		cout << "INFO: Unknown log format using default one: " << m_defaultLogFormat << endl;
		value = m_defaultLogFormat;
		value.erase(remove(value.begin(), value.end(), '$'), value.end());
		value.erase(remove(value.begin(), value.end(), ';'), value.end());
		position = value.find(this->m_separator);
	}

	while (position != string::npos)
	{
		m_LogFormat.push_back(value.substr(0, position));
		&value.erase(0, position + 1);
		position = value.find(this->m_separator);
	}

	m_LogFormat.push_back(value);
}

/*********************************/
/*Description:
/*	When one of the arguments as input is --genNew for config
/*	then create new default config file
/*Input:
/*	None
/*Output:
/*	None
/*********************************/
void Config::genDefaultConfigFile()
{
	ofstream outfile("config.cfg", ios::trunc | ios::out);
	{
		outfile << "# This is example Config file of the Merger "                                                                                       << endl;
		outfile                                                                                                                                         << endl;
		outfile << "# State how many lines should be processed in one batch, useful for memory save up,"												<< endl;
		outfile << "# but increases the I/O number of operations needed"																				<< endl;
		outfile << "LineBuffer = 20000"																													<< endl;
		outfile																																			<< endl;
		outfile << "# State The separator char in logs"																									<< endl;
		outfile << "Separator = \""																														<< endl;
		outfile																																			<< endl;
		outfile << "# PutYour nGinnx log format from your setting format here"																		    << endl;
		outfile << "# if empty then default one shall be taken. Mandatory fields are listed below."														<< endl;
		outfile << "nginx_log_format = $remote_addr\"$remote_user\"$time_local\"$request\"$status\"$body_bytes_sent\"$http_referer\"$http_user_agent;"  << endl;
		outfile																																			<< endl;
		outfile << "# state the default log filename from which data should be processed"																<< endl;
		outfile << "Log_filename = C:\\Users\\<user>\\logs\\example.log"																				<< endl;
		outfile																																			<< endl;
		outfile << "# format of the date"																												<< endl;
		outfile << "Date_format = Day/Month/Year:hour:minute:second timezone"																			<< endl;
		outfile																																			<< endl;
		outfile << "# State from which date period the data should be processed"																		<< endl;
		outfile << "# Possible values : \"all\" | date in format \"DD/MM/YYYY\""																		<< endl;
		outfile << "Date_start = all"																													<< endl;
		outfile << "Date_end = all"																														<< endl;
		outfile																																			<< endl;
		outfile << "# filename of the output"																											<< endl;
		outfile << "output_filename = testoutput.txt"																									<< endl;
	}
	outfile.close();
}

/*********************************/
/*Description:
/*	Destructor
/*Input:
/*	None
/*Output:
/*	None
/*********************************/
Config::~Config()
{
}
