import os
import sys
import subprocess
from os import path
from colorama import Back, Fore, Style


referenceParseFiles = [
    "2017_05_piknik_blog1.access.log.json",
    "2017_06_piknik_blog1.access.log.json",
    "2017_07_piknik_blog1.access.log.json",
    "2017_08_piknik_blog1.access.log.json",
    "2017_09_piknik_blog1.access.log.json",
    "2017_10_piknik_blog1.access.log.json",
    "2017_11_piknik_blog1.access.log.json",
    "2017_12_piknik_blog1.access.log.json",
    "2018_01_piknik_blog1.access.log.json",
    "2018_02_piknik_blog1.access.log.json",
    "2018_04_piknik_blog1.access.log.json",
    "2018_05_piknik_blog1.access.log.json",
    "2018_06_piknik_blog1.access.log.json",
    "2018_07_piknik_blog1.access.log.json",
    "2018_08_piknik_blog1.access.log.json",
    "2018_09_piknik_blog1.access.log.json",
    "2018_10_piknik_blog1.access.log.json",
    "2018_11_piknik_blog1.access.log.json",
    "2018_12_piknik_blog1.access.log.json",
    "2019_01_piknik_blog1.access.log.json",
    "2019_02_piknik_blog1.access.log.json",
    "AllInOne.log.json"
]

referenceMergeFiles = [
    "Merge_2017_05_to_2017_06.json",
    "Merge_2017_05_to_2017_06_to_2018_12 .json",
    "Merge_2017_10_to_2018_02_to_2019_01.json"
]

testFiles = [
    "2017_05_piknik_blog1.access.log",
    "2017_06_piknik_blog1.access.log",
    "2017_07_piknik_blog1.access.log",
    "2017_08_piknik_blog1.access.log",
    "2017_09_piknik_blog1.access.log",
    "2017_10_piknik_blog1.access.log",
    "2017_11_piknik_blog1.access.log",
    "2017_12_piknik_blog1.access.log",
    "2018_01_piknik_blog1.access.log",
    "2018_02_piknik_blog1.access.log",
    "2018_04_piknik_blog1.access.log",
    "2018_05_piknik_blog1.access.log",
    "2018_06_piknik_blog1.access.log",
    "2018_07_piknik_blog1.access.log",
    "2018_08_piknik_blog1.access.log",
    "2018_09_piknik_blog1.access.log",
    "2018_10_piknik_blog1.access.log",
    "2018_11_piknik_blog1.access.log",
    "2018_12_piknik_blog1.access.log",
    "2019_01_piknik_blog1.access.log",
    "2019_02_piknik_blog1.access.log",
    "AllInOne.log"
]


def compare_two_files(referencefile, testfile):
    # Function compares two files line by line returning status
    status = True

    with open(referencefile) as reference, open(testfile) as testfile:
        for line1, line2 in zip(reference, testfile):
            if not line1 == line2:
                status = False

    if not status:
        print("Mismatch for File:", referencefile, "-->", testfile)

    return status


def prepare_to_test():
    if not len(sys.argv) == 2:
        print("Error! Provide logger binary file to test!")
        show_help()
        sys.exit(-1)

    logger_binary_file = sys.argv[1]
    # Create default config file
    configprocess = subprocess.Popen([logger_binary_file, "--config", "--genNew"])
    configprocess.wait()


def test_parse_all_files():
    counter = 0
    failed_tests = 0
    passed_tests = 0
    current_path = os.getcwd()

    for x in testFiles:
        print("Test Case", counter, ":")
        output_filename = "PARSED_{}_{}".format(str(counter), x)
        test_filepath = path.abspath(path.join(current_path, "..", "..", "example_logs", x))

        if not os.path.exists(test_filepath):
            print("No test file found:", test_filepath)
            continue
        # Parse file
        print("Invoke command:", logger_binary_file, "--parser", "-i", test_filepath, "-o", output_filename)
        process = subprocess.Popen([logger_binary_file, "--parser", "-i", test_filepath, "-o", output_filename])
        process.wait()

        full_output_filepath = output_filename
        reference_filepath = "{}/reference data/parser/{}".format(current_path, referenceParseFiles[counter])
        test_case_status = compare_two_files(reference_filepath, full_output_filepath)

        if test_case_status:
            os.remove(full_output_filepath)

        print(Style.RESET_ALL)

        if test_case_status:
            passed_tests += 1
            print(Fore.GREEN)
            print("Test Case: ", counter, "PASSED")
        else:
            failed_tests += 1
            print(Fore.RED)
            print("Test Case: ", counter, "FAILED")

        print(Style.RESET_ALL)
        print("Passed: ", passed_tests, " Failed: ", failed_tests)
        counter += 1


def test_merge_files():
    logger_binary_file = sys.argv[1]
    current_path = os.getcwd()
    golden_filepath = "{}/reference data/merger/".format(current_path)
    reference_filepath = "{}/reference data/parser/".format(current_path)

    print("Invoke command:", logger_binary_file, "--merger", reference_filepath + referenceParseFiles[0],
          reference_filepath + referenceParseFiles[1])

    process = subprocess.Popen([logger_binary_file, "--merger", reference_filepath + referenceParseFiles[0],
                                reference_filepath + referenceParseFiles[1]])
    process.wait()
    test_case_one = compare_two_files("testoutput.txt", golden_filepath + referenceMergeFiles[0])
    os.remove("testoutput.txt")

    print("Invoke command:", logger_binary_file, "--merger", reference_filepath + referenceParseFiles[0],
          reference_filepath + referenceParseFiles[1], reference_filepath + referenceParseFiles[18])

    process = subprocess.Popen([logger_binary_file, "--merger", reference_filepath + referenceParseFiles[0],
                                reference_filepath + referenceParseFiles[1], reference_filepath + referenceParseFiles[18]])
    process.wait()
    test_case_two = compare_two_files("testoutput.txt", golden_filepath + referenceMergeFiles[1])
    os.remove("testoutput.txt")

    print("Invoke command:", logger_binary_file, "--merger", reference_filepath + referenceParseFiles[5],
          reference_filepath + referenceParseFiles[9], reference_filepath + referenceParseFiles[19])

    process = subprocess.Popen([logger_binary_file, "--merger", reference_filepath + referenceParseFiles[5],
                                reference_filepath + referenceParseFiles[9], reference_filepath + referenceParseFiles[19]])
    process.wait()
    test_case_three = compare_two_files("testoutput.txt", golden_filepath + referenceMergeFiles[2])
    os.remove("testoutput.txt")

    print(Style.RESET_ALL)

    if test_case_one and test_case_two and test_case_three:
        print(Fore.GREEN)
        print("ALL MERGER TEST CASES PASSED")

    else:
        print(Fore.RED)
        print("FAILED:")
        print(referenceMergeFiles[0], test_case_one)
        print(referenceMergeFiles[1], test_case_two)
        print(referenceMergeFiles[2], test_case_three)

    print(Style.RESET_ALL)

def show_help():
    print("Tool to run basic tests for logger binary \n")
    print("./Test_Parse_Each_File.py <logger_binary_file>\n")
    print("./Test_Parse_Each_File.py /../logger.exe\n")

# Do all necessary test preparations
prepare_to_test()
logger_binary_file = sys.argv[1]

test_parse_all_files()
test_merge_files()

os.remove("config.cfg")
