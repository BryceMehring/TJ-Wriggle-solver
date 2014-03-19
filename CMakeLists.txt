cmake_minimum_required(VERSION 2.8)
project(TJ-Wriggler CXX)

if(UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -pedantic")
	add_definitions(-DSOLVE_BFACTOR)
endif()

include_directories(${CMAKE_SOURCE_DIR}/source)

file(GLOB TJW_SOURCE
	${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/*.h)
	
if(UNIX)
	file(GLOB POLY_SOLVER
		${CMAKE_CURRENT_SOURCE_DIR}/source/PolySolver/*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/source/PolySolver/*.h)
		
	list(APPEND ${TJW_SOURCE} ${POLY_SOLVER})
endif()

add_executable(tjw ${TJW_SOURCE})

if(UNIX)
	target_link_libraries(tjw gsl gslcblas)
endif()


