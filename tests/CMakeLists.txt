cmake_minimum_required(VERSION 3.15.0)
project(tester)

find_package(GTest REQUIRED)
if (NOT GTEST_FOUND)
    message(WARNING "Could not find GTest, install it manually")
endif()
find_package (Threads REQUIRED)
if (NOT THREADS_FOUND)
    message(FATAL_ERROR "Could not find Threads")
endif()

add_executable(${PROJECT_NAME} tester.cpp)
set(CMAKE_CXX_STANDARD 11)
target_link_libraries(${PROJECT_NAME} PUBLIC
  paracl
  Threads::Threads
  gtest
  gtest_main
)