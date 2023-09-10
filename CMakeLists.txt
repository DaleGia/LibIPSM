# Minimum required version of CMake
cmake_minimum_required(VERSION 3.0)

# Project name and programming language
project(InterProcessSharedMemoryProject CXX)

# Add your source files to the project
add_executable(intForkTest test/intForkTest.cpp)
add_executable(structThreadTest test/structThreadTest.cpp)