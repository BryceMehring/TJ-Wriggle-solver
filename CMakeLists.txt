cmake_minimum_required(VERSION 2.8)
project(TJ-Wriggler CXX)

if(UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -pedantic")

	find_library(LIB_GSL gsl gslcblas)
	if(LIB_GSL)
		option(ENABLE_BRANCHING_FACTOR_CALCULATION "Enable branching factor calculation(requires gsl)" true)
		if(ENABLE_BRANCHING_FACTOR_CALCULATION)
			add_definitions(-DSOLVE_BFACTOR)
		endif()
		message("libgsl is found. Branching factor calculation can be enabled.")
	else()
		message("libgsl is not found. Branching factor calculation is disabled.")
	endif()
endif()

include_directories(${CMAKE_SOURCE_DIR}/source)

file(GLOB TJW_SOURCE
	${CMAKE_SOURCE_DIR}/source/*.cpp
	${CMAKE_SOURCE_DIR}/source/*.h)
	
if(ENABLE_BRANCHING_FACTOR_CALCULATION AND LIB_GSL)
	file(GLOB POLY_SOLVER
		${CMAKE_SOURCE_DIR}/source/PolySolver/*.cpp
		${CMAKE_SOURCE_DIR}/source/PolySolver/*.h)
		
	add_library(PolySolver STATIC ${POLY_SOLVER})
	target_link_libraries(PolySolver gsl gslcblas)
endif()

add_executable(tjw ${TJW_SOURCE})

if(ENABLE_BRANCHING_FACTOR_CALCULATION AND LIB_GSL)
	target_link_libraries(tjw PolySolver)
endif()
