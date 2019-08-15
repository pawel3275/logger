#include <iostream>
#include <stdio.h>
#include <chrono>
#include <experimental/filesystem>
#include <algorithm>
#include <cctype>
#include <string>

#include "../include/Config.h"
#include "../include/Loader.h"
#include "../include/Merger.h"

using namespace std;

/*********************************/
/*Description:
/*	Show general help
/*Input:
/*	None
/*Output:
/*	None
/*********************************/
void showHelp()
{
	cout << "type: --help for general help." << endl;
	cout << "type: --hh ARG for help regarding <ARG>." << endl;
	cout << "Please note: passed arguments can be either lowercase or uppercase." << endl;
		cout << "\tPossible ARG:" << endl;
			cout << "\t\tparser\t\"Info about parser of logs from txt to json format\"" << endl;
			cout << "\t\tmerger\t\"Info about merger of json files\"" << endl;
			cout << "\t\tconfig\t\"Info about confguration file\"" << endl;
		cout << "\t\tusage \t\"Info about example usage\"" << endl;
		cout << "\tFlags:" << endl;
			cout << "\t\t--debug\t\"Run in debug mode. Additional informations will be printed to output\"" << endl;
			cout << "\t\t--perfo\t\"Run in performance mode to check time measurement\"" << endl;
			cout << "\t\t--noOut\t\"No Output files will be generated.\"" << endl;
			cout << "\t\t--conf \t\"Use own config file.\"" << endl;

	cout << endl;
	cout << "To generate default config file please invoke:" << endl;
		cout << "\t../logger --config --genNew" << endl;

}

/*********************************/
/*Description:
/*	Show specific help about parser
/*Input:
/*	None
/*Output:
/*	None
/*********************************/
void showHelpParser()
{
	cout << "Parser is a mode, which takes as input the TXT file" << endl;
	cout << "specified in a config file, and parses the lines into a JSON file." << endl;
	cout << "Running \"../logger parser\" without any options makes parser to run in default mode." << endl;
	
	cout << endl;
	cout << "Run:" << endl;
		cout << "\t../logger --parser" << endl;
		cout << "\t../logger --parser -i inputfile.txt -o output.txt [...]" << endl;
		cout << "\t../logger --parser [OPTIONS]" << endl;
	cout << "Parser options:" << endl;
		cout << "\t../logger --parser -i [...] \t\"-i specifies input log filename and overrides the one from config\"" << endl;
		cout << "\t../logger --parser -o [...] \t\"-o specifies output filename and overrides the one from config\"" << endl;
	
	cout << endl;
	cout << "To parse logs within whole folder use:" << endl;
		cout << "\t../logger --parser -f <folderWithLogs>" << endl;
	cout << endl;
	cout << "If you would like also to merge them into one big json run:" << endl;
		cout << "\t../logger --parser -f <folderWithLogs> --mergeoutput" << endl;
	cout << endl;
	cout << "Additional options you can provide while merging whole folder are:" << endl;
		cout << "\t-s <logExtension> - uses extension for logs provided by user. For example:" << endl;
		cout << "\t\t../logger --parser -f <folderWithLogs> --mergeoutput -s json" << endl;
		cout << "\t--clear - leaves only one big json. For example:" << endl;
		cout << "\t\t../logger --parser -f <folderWithLogs> --mergeoutput --clear" << endl;
}

/*********************************/
/*Description:
/*	Show specific help about merger
/*Input:
/*	None
/*Output:
/*	None
/*********************************/
void showHelpMerger()
{
	cout << "Merger is a mode, which takes as input multiple" << endl;
	cout << "input JSON files, and merges them together into one." << endl;
	cout << "Output file is specified inside the config file." << endl;
	cout << "Running \"../logger merger\" without any options makes merger to run in default mode." << endl;
	cout << "Run:" << endl;
		cout << "\t../logger --merger" << endl;
		cout << "\t../logger --merger file1 file2 file3 [...] [OPTIONS]" << endl;
}

/*********************************/
/*Description:
/*	Show specific help about config
/*Input:
/*	None
/*Output:
/*	None
/*********************************/
void showHelpConfig()
{
	cout << "Config is a mandatory file used during program execution." << endl;
	cout << "Presence of this file is obligatory. As arguments are loaded from it." << endl;
	cout << "Run:" << endl;
		cout << "\t../logger --config [OPTIONS] ..." << endl;
	cout << "Config options:" << endl;
		cout << "\t--genNew\t\"Generate new default config file\"" << endl;
}

/*********************************/
/*Description:
/*	Show specific help about usage and examples
/*Input:
/*	None
/*Output:
/*	None
/*********************************/
void showHelpUsage()
{
	cout << "Examples:" << endl;
	cout << "../logger [MODE] [OPTIONS] ..." << endl;
	cout << "Possible main modes: " << endl;
		cout << "\tparser" << endl;
			cout << "\t\t../logger --parser [OPTIONS]" << endl;
		cout << "\tmerger" << endl;
			cout << "\t\t../logger --merger [OPTIONS]" << endl;
	cout << "Please refer to specific modes via \"--hh ARG\" to get more details." << endl;
}

/*********************************/
/*Description:
/*	If user has specified flags to be used during execution,
/*	this function is setting them up.
/*Input:
/*	vector <string> options - options provided via *argv
/*Output:
/*	None
/*********************************/
void globalFlagSetup(vector <string> options)
{
	Config &conf = Config::getInstance();
	if (find(options.begin(), options.end(), "--GENNEW") != options.end())
	{
		cout << "Generating new default config file." << endl;
		Config::genDefaultConfigFile();
		exit(0);
	}
	if (find(options.begin(), options.end(), "--DEBUG") != options.end())
	{
		cout << "Running with debug flag." << endl;
		conf.flags = conf.OpDEBUG;
	}
	if (find(options.begin(), options.end(), "--PERFO") != options.end())
	{
		cout << "Running with performance flag." << endl;
		conf.flags = conf.OpPERFO;
	}
	if (find(options.begin(), options.end(), "--NOOUT") != options.end())
	{
		cout << "Running without output flag." << endl;
		conf.flags = conf.OpNOOUT;
	}
}

/*********************************/
/*Description:
/*	If user has specified config filename, this function
/*	sets it up to provided one
/*Input:
/*	vector <string> options - options provided via *argv
/*Output:
/*	None
/*********************************/
void setConfigFilename(vector <string> options)
{
	Config &conf = Config::getInstance();
	if (find(options.begin(), options.end(), "--CONF") != options.end())
	{
		size_t pos = find(options.begin(), options.end(), "--CONF") - options.begin();
		if (pos <= options.size())
		{
			conf.setConfigFilename(options[pos + 1]);
		}
		else
		{
			cerr << "ERROR: Provide Config file! Example:" << endl;
			cerr << "\t../logger <ARG> --conf myconfig.cfg ..." << endl;
			exit(-1);
		}
	}
}

/*********************************/
/*Description:
/*	If user has specified output or input log filename
/*	we set it up for config here
/*Input:
/*	vector <string> options - options provided via *argv
/*Output:
/*	None
/*********************************/
void setParserInputOutputFilename(vector <string> options)
{
	Config &conf = Config::getInstance();
	if (find(options.begin(), options.end(), "-i") != options.end()) // i for in
	{
		size_t pos = find(options.begin(), options.end(), "-i") - options.begin();
		if (pos <= options.size())
		{
			conf.setLogFilename(options[pos + 1]);
		}
		else
		{
			cerr << "ERROR: Provide input file! Example:" << endl;
			cerr << "\t../logger <ARG [PARSER]> -f mylog.txt ..." << endl;
			exit(-1);
		}
	}

	if (find(options.begin(), options.end(), "-o") != options.end()) //  o for out
	{
		size_t pos = find(options.begin(), options.end(), "-o") - options.begin();
		if (pos <= options.size())
		{
			conf.setOutputFilename(options[pos + 1]);
		}
		else
		{
			cerr << "ERROR: Provide output filename! Example:" << endl;
			cerr << "\t../logger <ARG [PARSER]> -o myoutput.txt ..." << endl;
			exit(-1);
		}
	}
}

/*********************************/
/*Description:
/*	If user has specified to parse whole content of a directory
/*	we go through each log file and parse it. At the very end we can emrge it all
/*	to one big filename or not, depending on '--clear' option.
/*Input:
/*	vector <string> options - options provided via *argv
/*Output:
/*	None
/*********************************/
void parseWholeFolderOption(vector <string> options)
{
	Config &conf = Config::getInstance();

	size_t pos = find(options.begin(), options.end(), "-f") - options.begin();
	size_t suffixPos = find(options.begin(), options.end(), "-s") - options.begin(); //for user extension of file
	size_t shouldMerge = find(options.begin(), options.end(), "--MERGEOUTPUT") - options.begin();
	vector <string> filesToMerge;
	if (pos <= options.size())
	{
		vector <string> filesToParse;
		string path = options[pos + 1];

		for (const auto & entry : std::experimental::filesystem::directory_iterator(path))
		{
			filesToParse.push_back(entry.path().string());
		}

		for (auto & file : filesToParse)
		{
			string outputLogFilename;
			if (suffixPos < options.size())
			{
				string extension = options[suffixPos + 1];
				transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
				outputLogFilename = file + "." + extension;
			}
			else
			{
				outputLogFilename = file + ".json";
			}
			conf.setLogFilename(file);
			conf.setOutputFilename(outputLogFilename);
			Loader &loaderInst = Loader::getInstance();
			loaderInst.loadFromFileToMap();
			loaderInst.emptyLocalCache();
			filesToMerge.push_back(outputLogFilename);
		}
		if (shouldMerge <= options.size())
		{
			Merger mergerObj;
			for (auto & file : filesToMerge)
			{
				mergerObj.loadFromProcessedLog(file);
			}
			if (find(options.begin(), options.end(), "-o") != options.end())
			{
				size_t pos = find(options.begin(), options.end(), "-o") - options.begin();
				if (pos <= options.size())
				{
					conf.setOutputFilename(options[pos + 1]);
					mergerObj.SaveProcessedLog(options[pos + 1]);
				}
				else
				{
					cerr << "ERROR: No filename for output provided for argument -o" << endl;
					cerr << "Using default output filename: whole_default_merge.json" << endl;
				}
			}
			else
			{
				conf.setOutputFilename("whole_default_merge.json");
				mergerObj.SaveProcessedLog("whole_default_merge.json");
			}
		}

		//If option clear is specified then logger will produce only one output file with whole merge.
		if (find(options.begin(), options.end(), "--CLEAR") != options.end())
		{
			for (auto & file : filesToMerge)
			{
				remove(file.c_str());
			}
		}
	}
	else
	{
		cerr << "ERROR: Provide path to folder! Example:" << endl;
		cerr << "\t../logger <ARG [PARSER]> -f C:\\logs\\" << endl;
		exit(-1);
	}
}

/*********************************/
/*Description:
/*	Default parser invoking
/*Input:
/*	None
/*Output:
/*	None
/*********************************/
void useDefaultParserMode()
{
	Loader &loaderInst = Loader::getInstance();
	loaderInst.loadFromFileToMap();
}

/*********************************/
/*Description:
/*	Default merger invoking, we take a list of files to emrge and merge
/*	them up to one file specified insdie config
/*Input:
/*	vector <string> options - options provided via *argv
/*Output:
/*	None
/*********************************/
void useDefaultMergerMode(vector <string> options)
{
	Merger mergerObj;
	vector <string> filesToMerge;
	string empty;
	for (unsigned int i = 2; i < options.size(); i++)
	{
		string option = options[i];
		if (option.find("--") == string::npos)
		{
			// Save up files to merge
			filesToMerge.push_back(option);
		}
		else
		{
			//stop saving files to merge, this can occur when user is specifying own config file.
			break;
		}
	}

	for (unsigned int i = 0; i < filesToMerge.size(); i++)
	{
		mergerObj.loadFromProcessedLog(filesToMerge[i]);
	}
	//No need to provide output filename as it will be taken from config file.
	mergerObj.SaveProcessedLog("");
}

/*********************************/
/*Description:
/*	If user has specified --hh or --help trigger adequate info.
/*Input:
/*	vector <string> options - options provided via *argv
/*Output:
/*	None
/*********************************/
void invokeHelpCommunicates(vector <string> options)
{
	if (options[1] == "--HELP" && options.size() <= 2)
	{
		showHelp();
		exit(0);
	}
	else if (options[1] == "--HH" && options.size() > 2)
	{
		if (options[2] == "parser")
		{
			showHelpParser();
			exit(0);
		}
		else if (options[2] == "merger")
		{
			showHelpMerger();
			exit(0);
		}
		else if (options[2] == "config")
		{
			showHelpConfig();
			exit(0);
		}
		else if (options[2] == "usage")
		{
			showHelpUsage();
			exit(0);
		}
		else
		{
			cout << "Unknown option: " << options[2] << endl;
			showHelp();
			exit(0);
		}
	}
	else if (options[1] == "--HH" && options.size() == 2)
	{
		cout << "No specified option!" << endl;
		showHelp();
		exit(0);
	}
}

/*********************************/
/*Description:
/*	Main function.
*/
int main(int argc, char *argv[])
{
	//For performance stats
	auto startClock = chrono::steady_clock::now();
	
	Config &conf = Config::getInstance();
	vector <string> options;

	if (argc > 1)
	{
		for (int i = 0; i < argc; ++i)
		{
			options.push_back(argv[i]);
			if (options[i].find("--") != string::npos)
			{
				//transform everything to upper case, giving more freedom to choose input format.
				transform(options[i].begin(), options[i].end(), options[i].begin(), ::toupper);
			}
		}

		//Invoke them if they are provided via cmd.
		invokeHelpCommunicates(options);
	}
	else
	{
		cerr << "ERROR: Too few arguments! Type --help for options." << endl;
		exit(-1);
	}

	//Setup all necessary global flags like: performance, debug
	globalFlagSetup(options);
	
	//If provided setup the config filename to use isntead of the default one
	setConfigFilename(options);
	
	//Main configuration setup befor any mode is invoked.
	conf.configSetup();

	if (argc > 1 && options[1] == "--PARSER")
	{
		if (find(options.begin(), options.end(), "-i") != options.end() || find(options.begin(), options.end(), "-o") != options.end())
		{
			//Set properly input log filename to use and/or the output filename for json log
			setParserInputOutputFilename(options);
		}

		if (find(options.begin(), options.end(), "-f") != options.end()) //  f for folder
		{
			parseWholeFolderOption(options);
			exit(0);
		}

		// Parse the file taken from configuration file.
		useDefaultParserMode();
	}
	else if (argc > 1 && options[1] == "--MERGER")
	{
		useDefaultMergerMode(options);
	}
	else
	{
		cerr << "ERROR: No mode specified!" << endl;
		showHelp();
		exit(-1);
	}

	if (conf.flags & conf.OpNOOUT)
	{
		auto endClock = chrono::steady_clock::now();
		cout << "Time in miliseconds:" << chrono::duration_cast<chrono::milliseconds>(endClock - startClock).count() << endl;
	}

	return 0;
}
