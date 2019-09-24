import sys


def show_help():
    print("To modify or generate config values please provide as first \n")
    print("config path, then key to modify and second the value, for example: \n")
    print("./configModifier.py <path> <key> <value> \n")
    print("./configModifier.py /../config.cfg lineBuffer 100 \n")


def load_config_file(path):
    with open(path, 'r') as file:
        content = file.readlines()
    return content


def override_config(path, content):
    with open(path, 'w') as file:
        file.write(content)


if len(sys.argv) <= 1 or len(sys.argv) > 4:
    show_help()
    exit(-1)
else:
    config_path = str(sys.argv[1])
    key_to_change = sys.argv[2]
    value_to_set = sys.argv[3]
    print("Config path: ", config_path)
    print("Key: ", key_to_change)
    print("Value path: ", value_to_set)


config_content = load_config_file(config_path)
new_content = ""
for line in config_content:
    if (key_to_change in line) and not (line.startswith("#")):
        line = "{}{}{}{}".format(key_to_change, " = ", value_to_set, "\n")
        print("New line is: ", line)
    new_content += line

override_config(config_path, new_content)
