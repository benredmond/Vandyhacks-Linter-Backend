set(CMAKE_CXX_STANDARD 14)
cmake_minimum_required(VERSION 3.9)
project(vandyhacks)

set(SOURCE_FILES main.cpp)
add_executable(Main ${SOURCE_FILES})

set(SOURCE_FILES Scripting.cpp)
add_executable(Scripting ${SOURCE_FILES})

set(SOURCE_FILES CompileCheck.cpp)
add_executable(CompileCheck ${SOURCE_FILES})
