cmake_minimum_required(VERSION 2.8.2)

project(modplug-spotify C)
include_directories(${CMAKE_CURRENT_LIST_DIR}/src)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
include(LibFindMacros)

libfind_package(modplug ModPlug REQUIRED)
libfind_package(zip LibZip)

if(zip_FOUND)
	add_definitions(-DMPSP_HAVE_ZIP -DMPSP_HAVE_LIBZIP)
endif()

add_library(modplug MODULE src/common.c src/modplug-spotify.c src/parser.c src/playback.c)

set_target_properties(modplug PROPERTIES
	PREFIX ""
	SUFFIX ".splugin"
	COMPILE_FLAGS "-O3 -Wall"
	LINK_FLAGS "-Wl,--no-undefined")
target_link_libraries(modplug ${modplug_LIBRARIES} ${zip_LIBRARIES})
