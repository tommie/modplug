cmake_minimum_required(VERSION 2.8.2)

project(modplug-spotify C)
include_directories(${CMAKE_CURRENT_LIST_DIR}/src)
add_definitions(-O3 -Wall)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
include(LibFindMacros)

libfind_package(ModPlug ModPlug REQUIRED)

add_library(modplug MODULE src/common.c src/modplug-spotify.c src/parser.c src/playback.c)
set_target_properties(modplug PROPERTIES
	PREFIX ""
	SUFFIX ".splugin"
	LINK_FLAGS -Wl,--no-undefined)
target_link_libraries(modplug ${modplug_LIBRARIES})
