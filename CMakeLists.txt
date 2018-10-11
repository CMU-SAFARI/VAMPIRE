cmake_minimum_required(VERSION 3.10)
project(vampire)

set(CMAKE_CXX_STANDARD 11)

include_directories(src)

add_executable(vampire
        src/address.cpp
        src/address.h
        src/config.cpp
        src/config.h
        src/consts.h
        src/dramSpec.cpp
        src/dramSpec.h
        src/dramStruct.cpp
        src/dramStruct.h
        src/equations.cpp
        src/equations.h
        src/helper.cpp
        src/helper.h
        src/main.cpp
        src/main.h
        src/mapping.cpp
        src/parser.cpp
        src/parser.h
        src/statistics.cpp
        src/statistics.h
        src/vampire.cpp
        src/vampire.h)
