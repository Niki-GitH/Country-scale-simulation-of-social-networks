cmake_minimum_required(VERSION 3.9)
project(COUNTRY_SCALE_SIM)

set(CMAKE_CXX_STANDARD 17)

find_package(Threads REQUIRED)


###  Output directory  ###
set(CMAKE_OUTPUT_DIR ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)


include_directories(${CMAKE_SOURCE_DIR}/include)

set (CMAKE_CXX_FLAGS "-g3 -W -Wall -Wextra -Wno-unused -fPIC")

set(SRC_PATHS
	${CMAKE_SOURCE_DIR}/src/HMM_Career.cpp
	${CMAKE_SOURCE_DIR}/src/HMMEvents.cpp
	${CMAKE_SOURCE_DIR}/src/INIConfigParser.cpp
	${CMAKE_SOURCE_DIR}/src/PersonInfo.cpp
	${CMAKE_SOURCE_DIR}/src/PersonInfoGen.cpp
	${CMAKE_SOURCE_DIR}/src/PopulationDB.cpp
	${CMAKE_SOURCE_DIR}/src/TextFileLogger.cpp
	${CMAKE_SOURCE_DIR}/src/Util.cpp
	)

add_executable(COUNTRY_SCALE_SIM ${SRC_PATHS} main.cpp) 

target_link_libraries(COUNTRY_SCALE_SIM LINK_PUBLIC ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT})
