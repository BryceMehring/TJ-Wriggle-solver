cmake_minimum_required(VERSION 2.8)
project(TJ-Wriggler CXX)

if(UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -pedantic")
endif()

include_directories(${CMAKE_SOURCE_DIR}/Source)

file(GLOB_RECURSE TJW_SOURCE
	${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/*.h)

add_executable(tjw ${TJW_SOURCE})

SET(CPACK_PACKAGE_VERSION_MAJOR "3")
SET(CPACK_PACKAGE_VERSION_MINOR "0")
SET(CPACK_PACKAGE_VERSION_PATCH "0")

set(CPACK_GENERATOR "ZIP")
set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION "./")
include(InstallRequiredSystemLibraries)
include(CPack)

file(GLOB_RECURSE OUTPUT_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/*.txt.out)

install(FILES tjw.pro DESTINATION ./)
install(FILES README.md DESTINATION ./)
install(FILES ${OUTPUT_FILES} DESTINATION ./)
install(DIRECTORY source/ DESTINATION ./source)


