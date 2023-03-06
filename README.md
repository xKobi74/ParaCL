# ParaCL

# How to build the project
Use commands from the project root
* mkdir build
* conan install . --output-folder=build --build=missing -s compiler=gcc
* cd build
* cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
* make
And use next line to run the project
* ./ParaCL
Or next to run tester
* ./tests/tester

UML.drawio can be edit in https://www.diagrameditor.com/