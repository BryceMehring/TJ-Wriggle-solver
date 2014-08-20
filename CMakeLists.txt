cmake_minimum_required(VERSION 2.8)
project(TJ-Wriggler CXX)

if(UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -pedantic")
	add_definitions(-DSOLVE_BFACTOR)
endif()

include_directories(${CMAKE_SOURCE_DIR}/source)

file(GLOB TJW_SOURCE
	${CMAKE_SOURCE_DIR}/source/*.cpp
	${CMAKE_SOURCE_DIR}/source/*.h)
	
if(UNIX)
	file(GLOB POLY_SOLVER
		${CMAKE_SOURCE_DIR}/source/PolySolver/*.cpp
		${CMAKE_SOURCE_DIR}/source/PolySolver/*.h)
		
    add_library(PolySolver STATIC ${POLY_SOLVER})
endif()

add_executable(tjw ${TJW_SOURCE})

if(UNIX)
	target_link_libraries(tjw PolySolver gsl gslcblas)
endif()


