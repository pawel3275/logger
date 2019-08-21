import os
from os import path
import sys
import subprocess
from colorama import Fore, Back, Style


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
        sys.exit(-1)
    loggerbinaryfile = sys.argv[1]
    # Create default config file
    configprocess = subprocess.Popen([loggerbinaryfile, "--config", "--genNew"])
    configprocess.wait()


def TEST_parse_all_files():
    counter = 0
    FailedTests = 0
    PassedTests = 0
    currentPath = os.getcwd()
    for x in testFiles:
        print("Test Case", counter, ":")
        outputFilename = "PARSED_" + str(counter) + "_" + x
        testFilepath = path.abspath(path.join(currentPath, "..", "..", "example_logs", x))

        if not os.path.exists(testFilepath):
            print("No test file found:", testFilepath)
            continue
        # Parse file
        print("Invoke command:", loggerbinaryfile, "--parser", "-i", testFilepath, "-o", outputFilename)
        process = subprocess.Popen([loggerbinaryfile, "--parser", "-i", testFilepath, "-o", outputFilename])
        process.wait()

        fullOutputFilepath = outputFilename
        referenceFilepath = currentPath + "/reference data/parser/" + referenceParseFiles[counter]

        testCaseStatus = compare_two_files(referenceFilepath, fullOutputFilepath)

        if testCaseStatus:
            os.remove(fullOutputFilepath)

        print(Style.RESET_ALL)
        if testCaseStatus:
            PassedTests +=1
            print(Fore.GREEN)
            print("Test Case: ", counter, "PASSED")
        else:
            FailedTests += 1
            print(Fore.RED)
            print("Test Case: ", counter, "FAILED")
        print(Style.RESET_ALL)
        print("Passed: ", PassedTests, " Failed: ", FailedTests)
        counter += 1


def TEST_merge_files():
    loggerbinaryfile = sys.argv[1]
    currentPath = os.getcwd()
    goldenFilepath = currentPath + "/reference data/merger/"
    referenceFilepath = currentPath + "/reference data/parser/"

    print("Invoke command:", loggerbinaryfile, "--merger", referenceFilepath + referenceParseFiles[0],
          referenceFilepath + referenceParseFiles[1])
    process = subprocess.Popen([loggerbinaryfile, "--merger", referenceFilepath + referenceParseFiles[0],
                                referenceFilepath + referenceParseFiles[1]])
    process.wait()
    testCaseOne = compare_two_files("testoutput.txt", goldenFilepath + referenceMergeFiles[0])
    os.remove("testoutput.txt")

    print("Invoke command:", loggerbinaryfile, "--merger", referenceFilepath + referenceParseFiles[0],
          referenceFilepath + referenceParseFiles[1], referenceFilepath + referenceParseFiles[18])
    process = subprocess.Popen([loggerbinaryfile, "--merger", referenceFilepath + referenceParseFiles[0],
                                referenceFilepath + referenceParseFiles[1], referenceFilepath + referenceParseFiles[18]])
    process.wait()
    testCaseTwo = compare_two_files("testoutput.txt", goldenFilepath + referenceMergeFiles[1])
    os.remove("testoutput.txt")

    print("Invoke command:", loggerbinaryfile, "--merger", referenceFilepath + referenceParseFiles[5],
          referenceFilepath + referenceParseFiles[9], referenceFilepath + referenceParseFiles[19])
    process = subprocess.Popen([loggerbinaryfile, "--merger", referenceFilepath + referenceParseFiles[5],
                                referenceFilepath + referenceParseFiles[9], referenceFilepath + referenceParseFiles[19]])
    process.wait()
    testCaseThree = compare_two_files("testoutput.txt", goldenFilepath + referenceMergeFiles[2])
    os.remove("testoutput.txt")

    print(Style.RESET_ALL)
    if testCaseOne and testCaseTwo and testCaseThree:
        print(Fore.GREEN)
        print("ALL MERGER TEST CASES PASSED")
    else:
        print(Fore.RED)
        print("FAILED:")
        print(referenceMergeFiles[0], testCaseOne)
        print(referenceMergeFiles[1], testCaseTwo)
        print(referenceMergeFiles[2], testCaseThree)
    print(Style.RESET_ALL)



# Do all necessary test preparations
prepare_to_test()
loggerbinaryfile = sys.argv[1]

TEST_parse_all_files()
TEST_merge_files()

os.remove("config.cfg")






