cmake_minimum_required(VERSION 3.15.0)

project(bison)


find_package(FLEX REQUIRED)

if (FLEX_FOUND)
    ADD_CUSTOM_TARGET(
        flex_target
        COMMAND ${FLEX_EXECUTABLE} 
            --outfile=${CMAKE_CURRENT_SOURCE_DIR}/lex.yy.c
            ${CMAKE_CURRENT_SOURCE_DIR}/pcl.lex
        COMMENT "Generating pcl.tab.c"
    )
endif(FLEX_FOUND)


find_package(BISON REQUIRED)

if (BISON_FOUND)
    ADD_CUSTOM_TARGET(
        bison_target
        COMMAND ${BISON_EXECUTABLE} 
            --output=${CMAKE_CURRENT_SOURCE_DIR}/pcl.tab.c
            ${CMAKE_CURRENT_SOURCE_DIR}/pcl.y
        COMMENT "Generating pcl.tab.c"
    )
endif(BISON_FOUND)

#add_flex_bison_dependency(scanner parser)

add_executable(${PROJECT_NAME} lex.yy.c pcl.tab.c pcl_bison.cpp)
set(CMAKE_CXX_STANDARD 17)
target_link_libraries(${PROJECT_NAME} PUBLIC
  paracl
  boost
)