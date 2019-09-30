import sys
import subprocess
import os
from datetime import datetime, timedelta


def show_help():
    print("This tool takes logs from day before, and logs from present day, \n")
    print("parses them and merges to specified file: \n")
    print("./combinedMode.py <config_file> <loggerBinary> <first_log_file> <second_log_file> <output_json_file> \n")
    print("./configModifier.py /../config.txt /../logger /../configModifier /../log1.log /../log2.log example.json \n")


if len(sys.argv) <= 1 or len(sys.argv) > 7:
    show_help()
    exit(-1)

else:
    config_file = sys.argv[1]
    logger_binary_file = sys.argv[2]
    config_modifier_file = sys.argv[3]
    first_log_file = sys.argv[4]
    second_log_file = sys.argv[5]
    output_json_file = sys.argv[6]

    present_day_date = datetime.now()
    present_day_date = present_day_date.strftime("%d/%b/%Y")

    yesterday_date = datetime.now() - timedelta(days=1)
    yesterday_date = yesterday_date.strftime("%d/%b/%Y")

    print("Starting parsing first file")
    # Setup properly config to parse logs from day before
    # Set start date in config
    process = subprocess.Popen([sys.executable, config_modifier_file, config_file, "Date_start", yesterday_date])
    # Set end date in config
    process = subprocess.Popen([sys.executable, config_modifier_file, config_file, "Date_end", present_day_date])
    process.wait()

    # Parse the log from day before
    first_output_parsed_file = "{}\ParsedLog1.json".format(os.getcwd())
    process = subprocess.Popen([logger_binary_file, "--parser", "-o", first_output_parsed_file, "-i", first_log_file,
                     "--conf", config_file])
    process.wait()

    print("Parsing of first file done")
    print("Starting parsing second file")

    # Setup properly config to parse logs from present day (take all)
    # Set start date in config
    process = subprocess.Popen([sys.executable, config_modifier_file, config_file, "Date_start", "all"])

    # Set end date in config
    process = subprocess.Popen([sys.executable, config_modifier_file, config_file, "Date_end", "all"])
    process.wait()

    # Parse log from today
    second_output_parsed_file = "{}\ParsedLog2.json".format(os.getcwd())
    process = subprocess.Popen([logger_binary_file, "--parser", "-o", second_output_parsed_file, "-i", second_log_file,
                     "--conf", config_file])
    process.wait()

    print("Parsing of second file done.")
    print("Starting Merging")
    # Set output filename for merged files inside config
    process = subprocess.Popen([sys.executable, config_modifier_file, config_file, "output_filename", output_json_file])
    process.wait()
    # Merge two logs into one
    process = subprocess.Popen([logger_binary_file, "--merger", first_output_parsed_file, second_output_parsed_file,
                                "--conf", config_file])
    process.wait()
    os.remove(first_output_parsed_file)
    os.remove(second_output_parsed_file)

    print("Done! \n")
