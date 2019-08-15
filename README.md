**About Logger**:

Logger is set of two tools combined together:
  - *parser*
  - *merger*

Parser takes as an input log generated from server and parses it into more viewable version of JSON file. Merger on the other hand takes multiple JSON files and merges them into one JSON file. To view more about usage and examples please refer to the "Usage" section. Example logs that can be parsed can be found inside "examples" directory within the project.

Logger supports both Windows and Linux.

**Requirements**:

- *cmake*
- *gcc*
- *Visual studio 2015+*
- *python 3* (optional)

**Building**:

To build project under linux or windows simply run:
  $ *cmake CMakeLists.txt*
  $ *make* (linux only)

**Usage**:

To view help of logger simply type --hh or --help with executeable. After that help and usage examples shall be shown in console. During first run please generate your own 'config.txt' file, to see how simply type '../logger --hh config'

Generating configuration file is mandatory!
