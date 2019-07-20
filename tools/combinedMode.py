import sys
from datetime import datetime, timedelta
import subprocess


def show_help():
    print("This tool takes logs from day before, and logs from present day, \n")
    print("parses them and merges to specified file: \n")
    print("./combinedMode.py <configFile> <loggerBinary> <firstLogFile> <SecondLogFile> <outputJsonFile> \n")
    print("./configModifier.py /../log1.txt /../log2.txt /../example.json \n")


if len(sys.argv) <= 1 or len(sys.argv) > 6:
    show_help()
    exit(-1)
else:
    configFile = sys.argv[1]
    loggerBinaryFile = sys.argv[2]
    firstLogFile = sys.argv[3]
    secondLogFile = sys.argv[4]
    outputJsonFile = sys.argv[5]

    presentDayDate = datetime.datetime.now()
    presentDayDate.strftime("%d/%m/%Y")

    yesterdayDate = datetime.now() - timedelta(days=1)
    yesterdayDate.strftime("%d/%m/%Y")

    print("firstLogFile: ", firstLogFile)
    print("secondLogFile: ", secondLogFile)
    print("outputJsonFile: ", outputJsonFile)

    # Setup properly config to parse logs from day before
    # Set start date
    subprocess.call()
    # Set end date
    subprocess.call()
    # Parse the log from day before
    subprocess.call()

    # Set start date in config
    subprocess.call()
    # Parse log from today
    subprocess.call()

    # Merge two logs into one
    subprocess.call()






