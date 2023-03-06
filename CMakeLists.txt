cmake_minimum_required(VERSION 3.15.0) 
project(ParaCL)

add_subdirectory(modules/paracl)
add_subdirectory(tests)
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} paracl)