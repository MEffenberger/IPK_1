# This file handles building each task as sub-project.
#
# Each task becomes an independent project in which you can experiment.
#
# Tasks are added as separate sub-projects so as compilation errors in
# one task don't affect another task.

cmake_minimum_required(VERSION 3.16)
project(VUT-IPK)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -Wextra")


add_executable(test main.cpp Config.cpp Config.h ClientOutput.cpp ClientOutput.h connectToServer.cpp connectToServer.h FSMValidate.cpp FSMValidate.h Parser.cpp Parser.h ProtocolHandler.cpp ProtocolHandler.h StreamHandler.cpp StreamHandler.h TCPMessageValidator.cpp TCPMessageValidator.h TCPProtocolHandler.cpp TCPProtocolHandler.h UDPProtocolHandler.cpp UDPProtocolHandler.h)
