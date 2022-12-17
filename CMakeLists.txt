cmake_minimum_required(VERSION 3.23)
project(Parsers)

set(CMAKE_CXX_STANDARD 20)

include_directories(${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/utility)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(source
        ${CMAKE_SOURCE_DIR}/src/GrammarBase.h
        ${CMAKE_SOURCE_DIR}/src/BasicEarleyParser.h
        ${CMAKE_SOURCE_DIR}/src/utility/KMP.h
        ${CMAKE_SOURCE_DIR}/src/BasicLR1Parser.h)

add_executable(Parsers main.cpp ${source})

add_subdirectory(GoogleTests)
