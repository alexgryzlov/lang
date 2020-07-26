cmake_minimum_required(VERSION 3.15)
project(lang)

set(CMAKE_CXX_STANDARD 20)

add_executable(lang BinaryTranslator/OP.hpp main.cpp)
add_executable(bin BinaryTranslator/OP.hpp BinaryTranslator/RealASMTranslator.cpp)