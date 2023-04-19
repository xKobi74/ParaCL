# ParaCL

# How to build the project
Use commands from the project root
* mkdir build
* cd build
* cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
* make
And use next line to run the project
* .modules/bison/pcli
Or next to run tester
* ./tests/tester

# Project usuage
Now we recommend to use IDE for ParaCL interpreter, to open please read:
* ParaCL/ide/README.md
## Comand line usuage
To use in command line: `./pcli [file with code] [options]`  
To see the list of options: `./pcli --help`

UML.drawio can be edit in https://www.diagrameditor.com/
