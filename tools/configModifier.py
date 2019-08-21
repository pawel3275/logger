import sys


def show_help():
    print("To modify or generate config values please provide as first \n")
    print("config path, then key to modify and second the value, for example: \n")
    print("./configModifier.py <path> <key> <value> \n")
    print("./configModifier.py /../config.cfg lineBuffer 100 \n")


def load_config_file(path):
    file = open(path, "r")
    content = file.readlines()
    file.close()
    return content


def override_config(path, content):
    file = open(path, "w")
    file.write(content)
    file.close()


if len(sys.argv) <= 1 or len(sys.argv) > 4:
    show_help()
    exit(-1)
else:
    configPath = str(sys.argv[1])
    keyToChange = sys.argv[2]
    valueToSet = sys.argv[3]
    print("Config path: ", configPath)
    print("Key: ", keyToChange)
    print("Value path: ", valueToSet)

configContent = load_config_file(configPath)

newContent = ""
for line in configContent:
    if(keyToChange in line) and not (line.startswith("#")):
        line = keyToChange + " = " + valueToSet + "\n"
        print("New line is: ", line)
    newContent += line

override_config(configPath, newContent)






