cmake_minimum_required(VERSION 2.8)
project(TJ-Wriggle CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall")

include_directories(${CMAKE_SOURCE_DIR}/Source)

file(GLOB_RECURSE TJW_SOURCE
	${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/*.h)

add_executable(tjw ${TJW_SOURCE})

