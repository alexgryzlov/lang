cmake_minimum_required(VERSION 3.15)
project(lang)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(Compiler)
add_subdirectory(ASM)
#add_executable(bin BinaryTranslator/OP.hpp BinaryTranslator/RealASMTranslator.cpp)