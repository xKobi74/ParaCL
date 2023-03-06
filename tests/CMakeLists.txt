cmake_minimum_required(VERSION 3.15.0)
project(tester)

find_package(GTest REQUIRED)
find_package (Threads REQUIRED)

add_executable(${PROJECT_NAME} tester.cpp)
set(CMAKE_CXX_STANDARD 11)
target_link_libraries(${PROJECT_NAME} PUBLIC
  paracl
  Threads::Threads
  gtest
  gtest_main
)