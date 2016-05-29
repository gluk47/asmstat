cmake_minimum_required(VERSION 2.6)
project(asmstat)

add_executable(asmstat main.cc DumpLine.cc Command.cc JumpInfo.cc)
add_definitions(-DDEBUG)

set_property(TARGET asmstat PROPERTY CXX_STANDARD 14)
install(TARGETS asmstat RUNTIME DESTINATION bin)
