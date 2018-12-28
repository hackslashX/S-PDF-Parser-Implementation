cmake_minimum_required(VERSION 3.7)
project(PDF_SemesterProject)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp spdf-initialize.cpp spdf-primary.cpp spdf-structure.h spdf-supplementary.cpp spdf-validation.cpp spdf-xrefFunctions.cpp)
add_executable(PDF_SemesterProject ${SOURCE_FILES})