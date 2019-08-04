import os
from os import path
import sys
import subprocess
import difflib
import filecmp

referenceFiles = [
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
    "whole_directory_merge.json"
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

    # with open(referencefile) as reference:
    #    reference_text = reference.read()
    # with open(testfile) as testfile:
    #    testfile_text = testfile.read()
    # Find and print the diff:
    # for line in difflib.unified_diff(reference_text, testfile_text, fromfile=referencefile, tofile=testfile, lineterm=''):
    #    print(line)

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


statusGeneral = True
prepare_to_test()
currentPath = path.dirname(__file__)
loggerbinaryfile = sys.argv[1]

counter = 0
for x in testFiles:
    testCaseStatus = False
    print("Test Case:", counter)
    outputFilename = "PARSED_" + str(counter) + "_" + x
    testFilepath = path.abspath(path.join(currentPath, "..", "..", "example_logs", x))

    if not os.path.exists(testFilepath):
        print("No test file found:", testFilepath)
        continue
    # Parse file
    process = subprocess.Popen([loggerbinaryfile, "--parser", "-i", testFilepath, "-o", outputFilename])
    process.wait()

    outputFilepath = os.path.dirname(loggerbinaryfile)
    fullOutputFilepath = outputFilename
    referenceFilepath = currentPath + "/reference data/" + referenceFiles[counter]

    testCaseStatus = compare_two_files(referenceFilepath, fullOutputFilepath)

    if testCaseStatus:
        os.remove(fullOutputFilepath)

    if testCaseStatus:
        print("Test Case: \n", counter)
        print("\033[1;32;40m PASSED")
    else:
        print("\033[1;31;40m FAILED")
    counter += 1



