import sys
import subprocess
import os
from datetime import datetime, timedelta


def show_help():
    print("This tool takes logs from day before, and logs from present day, \n")
    print("parses them and merges to specified file: \n")
    print("./combinedMode.py <configFile> <loggerBinary> <firstLogFile> <SecondLogFile> <outputJsonFile> \n")
    print("./configModifier.py /../config.txt /../logger /../configModifier /../log1.txt /../log2.txt example.json \n")


if len(sys.argv) <= 1 or len(sys.argv) > 7:
    show_help()
    exit(-1)
else:
    configFile = sys.argv[1]
    loggerBinaryFile = sys.argv[2]
    configModifierFile = sys.argv[3]
    firstLogFile = sys.argv[4]
    secondLogFile = sys.argv[5]
    outputJsonFile = sys.argv[6]

    presentDayDate = datetime.now()
    presentDayDate = presentDayDate.strftime("%d/%b/%Y")

    yesterdayDate = datetime.now() - timedelta(days=1)
    yesterdayDate = yesterdayDate.strftime("%d/%b/%Y")

    print("Starting parsing first file")
    # Setup properly config to parse logs from day before
    # Set start date in config
    process = subprocess.Popen([sys.executable, configModifierFile, configFile, "Date_start", yesterdayDate])
    # Set end date in config
    process = subprocess.Popen([sys.executable, configModifierFile, configFile, "Date_end", presentDayDate])
    process.wait()

    # Parse the log from day before
    firstOutputParsedFile = os.getcwd() + "\ParsedLog1.json"
    process = subprocess.Popen([loggerBinaryFile, "--parser", "-o", firstOutputParsedFile, "-i", firstLogFile,
                     "--conf", configFile])
    process.wait()

    print("Parsing of first file done")
    print("Starting parsing second file")

    # Setup properly config to parse logs from present day (take all)
    # Set start date in config
    process = subprocess.Popen([sys.executable, configModifierFile, configFile, "Date_start", "all"])
    # Set end date in config
    process = subprocess.Popen([sys.executable, configModifierFile, configFile, "Date_end", "all"])
    process.wait()

    # Parse log from today
    secondOutputParsedFile = os.getcwd() + "\ParsedLog2.json"
    process = subprocess.Popen([loggerBinaryFile, "--parser", "-o", secondOutputParsedFile, "-i", secondLogFile,
                     "--conf", configFile])
    process.wait()

    print("Parsing of second file done.")
    print("Starting Merging")
    # Set output filename for merged files inside config
    process = subprocess.Popen([sys.executable, configModifierFile, configFile, "output_filename", outputJsonFile])
    process.wait()
    # Merge two logs into one
    process = subprocess.Popen([loggerBinaryFile, "--merger", firstOutputParsedFile, secondOutputParsedFile,
                                "--conf", configFile])
    process.wait()
    os.remove(firstOutputParsedFile)
    os.remove(secondOutputParsedFile)

    print("Done! \n")
