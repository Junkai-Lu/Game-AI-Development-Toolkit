cmake_minimum_required(VERSION 3.2)
project(Paradoxes VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXE_LINKER_FLAGS "-pthread")

option(EXAMPLE "Output libParadoxes." OFF)
IF(${EXAMPLE} STREQUAL "ON")
	MESSAGE(STATUS "Output 'libParadoxes' and example games.")
	ADD_SUBDIRECTORY(./examples)
ENDIF()

ADD_SUBDIRECTORY(./lib/json11)
AUX_SOURCE_DIRECTORY(./src SRC_LIST)
ADD_LIBRARY(Paradoxes STATIC ${SRC_LIST})
TARGET_LINK_LIBRARIES(Paradoxes json11)
ADD_DEPENDENCIES(Paradoxes json11)
