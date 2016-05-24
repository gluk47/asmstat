cmake_minimum_required(VERSION 2.6)
project(asmstat)

add_executable(asmstat main.cpp)

install(TARGETS asmstat RUNTIME DESTINATION bin)
