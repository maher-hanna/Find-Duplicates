cmake_minimum_required(VERSION 3.10)

project(FindDuplicates)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB SOURCES 
	"*.h"
	"*.cpp"
	)

add_executable(FindDuplicates ${SOURCES})