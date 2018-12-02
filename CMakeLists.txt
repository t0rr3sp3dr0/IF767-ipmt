cmake_minimum_required(VERSION 3.5)
project(ipmt)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -O2)
set(EXECUTABLE_OUTPUT_PATH ./bin)

add_executable(ipmt ./src/main.cpp ./src/serializer.h ./src/serializer.cpp ./src/string_view.h ./src/string_view.cpp ./src/suffix_array.h ./src/suffix_array.cpp ./src/lz77.hpp ./src/watch.h)
