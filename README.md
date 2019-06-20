Logger:

Logger is set of two tools combined together:
  -parser
  -merger
Parser takes as an input log generated from server and parses it into more viewable version of JSON file. Merger on the other hand takes multiple JSON files and merges them into one JSON file. To view more about usage and examples please refer to the "Usage" section. Example logs that can be parsed can be found inside "examples" directory within the project.

Requirements:

- cmake (for cmake file included inside project directory)
- gcc (for compiling)

Building:

To build file under linux or windows simply run command:
  cmake CMakeLists.txt
  
Additionally on linux run command 'make'.

Usage:

To view help of logger simply type --hh or --help with executeable. After that help and usage examples shall be shown in console. During first run please generate your own 'config.txt' file, to see how simply type '../logger --hh config'


Upcoming features:
- Unit tests
- More generic version of parser to run in different circumstances.
