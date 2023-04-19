cmake_minimum_required(VERSION 3.15.0)

project(pcli)


find_package(FLEX REQUIRED)

if (FLEX_FOUND)
    ADD_CUSTOM_TARGET(
        flex_target
        COMMAND ${FLEX_EXECUTABLE} 
            --outfile=${CMAKE_CURRENT_SOURCE_DIR}/lex.yy.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/pcl.lex
        COMMENT "Generating pcl.tab.cpp"
    )
endif(FLEX_FOUND)


find_package(BISON REQUIRED)

if (BISON_FOUND)
    ADD_CUSTOM_TARGET(
        bison_target
        COMMAND ${BISON_EXECUTABLE} 
            --output=${CMAKE_CURRENT_SOURCE_DIR}/pcl.tab.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/pcl.y
        COMMENT "Generating pcl.tab.cpp"
    )
endif(BISON_FOUND)

add_executable(${PROJECT_NAME} pcl_bison.cpp lex.yy.cpp pcl.tab.cpp)
add_dependencies(${PROJECT_NAME} bison_target)
add_dependencies(${PROJECT_NAME} flex_target)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(${PROJECT_NAME} PUBLIC
  paracl
  boost_program_options
)