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
	cout << "To parse logs within whole folder use:" << endl;
		cout << "\t../logger --parser -f <folderWithLogs>" << endl;
	cout << "If you would like also to merge them into one big json run:" << endl;
		cout << "\t../logger --parser -f <folderWithLogs> --mergeoutput" << endl;
	cout << "Additional options you can provide while merging whole fodler are:" << endl;
		cout << "\t-s <logExtension> - uses extension for logs provided by user. For example:" << endl;
			cout << "\t\t../logger --parser -f <folderWithLogs> --mergeoutput -s json" << endl;
		cout << "\t--clear - leaves only one big json. For example:" << endl;
			cout << "\t\t../logger --parser -f <folderWithLogs> --mergeoutput --clear" << endl;

}

void showHelpParser()
{
	cout << "Parser is a mode, which takes as input the TXT file" << endl;
	cout << "specified in a config file, and parses the lines into a JSON file." << endl;
	cout << "Running \"../logger parser\" without any options makes parser to run in default mode." << endl;
	cout << "Run:" << endl;
		cout << "\t../logger --parser" << endl;
		cout << "\t../logger --parser -i inputfile.txt -o output.txt [...]" << endl;
		cout << "\t../logger --parser [OPTIONS]" << endl;
	cout << "Parser options:" << endl;
		cout << "\t../logger --parser -i [...] \t\"-i specifies input log filename and overrides the one from config\"" << endl;
		cout << "\t../logger --parser -o [...] \t\"-o specifies output filename and overrides the one from config\"" << endl;
}

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

void showHelpConfig()
{
	cout << "Config is a mandatory file used during program executions." << endl;
	cout << "Presence of this file is obligatory. As arguments are loaded from it." << endl;
	cout << "Run:" << endl;
		cout << "\t../logger --config [OPTIONS] ..." << endl;
	cout << "Config options:" << endl;
		cout << "\t--genNew\t\"Generate new default config file\"" << endl;
}

void showHelpUsage()
{
	cout << "Examples:" << endl;
	cout << "../logger [MODE] [OPTIONS] ..." << endl;
	cout << "Possible main modes: " << endl;
		cout << "\tparser" << endl;
			cout << "\t\t../logger --parser [OPTIONS]" << endl;
		cout << "\tmerger" << endl;
			cout << "\t\t../logger --merger [OPTIONS]" << endl;
}



int main(int argc, char *argv[])
{
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
				transform(options[i].begin(), options[i].end(), options[i].begin(), ::toupper);
			}
		}

		if (options[1] == "--HELP" && argc <= 2)
		{
			showHelp();
			return 0;
		}
		else if (options[1] == "--HH" && argc > 2)
		{
			if (options[2] == "parser")
			{
				showHelpParser();
				return 0;
			}
			else if (options[2] == "merger")
			{
				showHelpMerger();
				return 0;
			}
			else if (options[2] == "config")
			{
				showHelpConfig();
				return 0;
			}
			else if (options[2] == "usage")
			{
				showHelpUsage();
				return 0;
			}
			else
			{
				cout << "Unknown option: " << argv[2] << endl;
				showHelp();
				return 0;
			}
		}
		else if(options[1] == "--HH" && argc == 2)
		{
			cout << "No specified option!" << endl;
			showHelp();
			return 0;
		}
	}
	else
	{
		cout << "Too few arguments! Type --help for options." << endl;
		return 0;
	}

	if (find(options.begin(), options.end(), "--GENNEW") != options.end())
	{
		cout << "Generating new default config file." << endl;
		Config::genDefaultConfigFile();
		return 0;
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

	if (find(options.begin(), options.end(), "--CONF") != options.end())
	{
		size_t pos = find(options.begin(), options.end(), "--CONF") - options.begin();
		if (pos <= options.size())
		{
			conf.setConfigFilename(options[pos+1]);
		}
		else
		{
			cerr << "ERROR: Provide Config file! Example:" << endl;
			cerr << "\t../logger <ARG> --conf myconfig.cfg ..." << endl;
			return 0;
		}
	}

	conf.configSetup();

	if (argc > 1 && options[1] == "--PARSER")
	{
		if (find(options.begin(), options.end(), "-i") != options.end()) // i for in
		{
			ptrdiff_t pos = find(options.begin(), options.end(), "-i") - options.begin();
			if (pos <= options.size())
			{
				conf.setLogFilename(options[pos + 1]);
			}
			else
			{
				cerr << "ERROR: Provide input file! Example:" << endl;
				cerr << "\t../logger <ARG [PARSER]> -f mylog.txt ..." << endl;
				return 0;
			}
		}

		if (find(options.begin(), options.end(), "-o") != options.end()) //  o for out
		{
			ptrdiff_t pos = find(options.begin(), options.end(), "-o") - options.begin();
			if (pos <= options.size())
			{
				conf.setOutputFilename(options[pos + 1]);
			}
			else
			{
				cerr << "ERROR: Provide output filename! Example:" << endl;
				cerr << "\t../logger <ARG [PARSER]> -o myoutput.txt ..." << endl;
				return 0;
			}
		}

		if (find(options.begin(), options.end(), "-f") != options.end()) //  f for folder
		{
			ptrdiff_t pos = find(options.begin(), options.end(), "-f") - options.begin();
			ptrdiff_t suffixPos = find(options.begin(), options.end(), "-s") - options.begin(); //for user extension of file
			ptrdiff_t shouldMerge = find(options.begin(), options.end(), "--MERGEOUTPUT") - options.begin();
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
						ptrdiff_t pos = find(options.begin(), options.end(), "-o") - options.begin();
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

				return 0;
			}
			else
			{
				cerr << "ERROR: Provide path to folder! Example:" << endl;
				cerr << "\t../logger <ARG [PARSER]> -f C:\\logs\\" << endl;
				return 0;
			}
		}

		// Parse the file taken from configuration file.
		Loader &loaderInst = Loader::getInstance();
		loaderInst.loadFromFileToMap();
	}
	else if (argc > 1 && options[1] == "--MERGER")
	{
		Merger mergerObj;
		vector <string> filesToMerge;
		string empty;
		for (int i = 2; i < argc; i++)
		{
			string option = argv[i];
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
		mergerObj.SaveProcessedLog("");
	}
	else
	{
		cerr << "No mode specified!" << endl;
		showHelp();
		return 0;
	}

	if (conf.flags & conf.OpNOOUT)
	{
		auto endClock = chrono::steady_clock::now();
		cout << "Time in miliseconds:" << chrono::duration_cast<chrono::milliseconds>(endClock - startClock).count() << endl;
	}

	return 0;
}
