project(vlExt)
include (GenerateExportHeader)
cmake_minimum_required(VERSION 2.6)
set(VL_ROOT "D:/Program Files/Visualization_Library_SDK-2011.9" CACHE PATH "visualization library root" )
find_package(VL COMPONENTS VLCore VLGraphics VLGLUT REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT )
find_package(assimp)

set(GLFX_LIBRARIES "" CACHE FILEPATH "glfx libraries" )
set(GLFX_INCLUDE_DIRS "" CACHE PATH "glfx include dirs" )

# Dynamic vs Static Linking
option(VLEXT_DYNAMIC_LINKING "Set to ON to build VLEXT for dynamic linking, or OFF for static." ON)
if(VLEXT_DYNAMIC_LINKING)
	add_definitions(-DVLEXT_DYNAMIC_LINKING)
	set(VLEXT_SHARED_OR_STATIC "SHARED")
else()
	add_definitions(-DVLEXT_STATIC_LINKING)
	set(VLEXT_SHARED_OR_STATIC "STATIC")
endif()
include_directories(${VL_INCLUDE_DIRS} "src")
add_subdirectory(src)
# Debug postfix for all libraries.
set(CMAKE_DEBUG_POSTFIX "-d")

# High Warning Level
if(MSVC10)
	set(CMAKE_CXX_FLAGS "/W4 /EHsc /MP")
	add_definitions(-D_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES)
elseif(MSVC)
	set(CMAKE_CXX_FLAGS "/W4 /EHsc")
	add_definitions(-D_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES)
else()
	set(CMAKE_CXX_FLAGS "-W -Wall") # see also: -W -Wall -Wwrite-strings -Wcast-qual -Wconversion -Wshadow
endif()



if(WIN32)
	add_definitions(-DUNICODE)
	if(VLEXT_DYNAMIC_LINKING)
	  add_definitions(-DVLEXT_EXPORTS)
	endif()
endif()

if(MSVC)
	set(WINVER "0x0600" CACHE STRING "WINVER version (see MSDN documentation)")
	add_definitions(-DWINVER=${WINVER})
	add_definitions(-D_WIN32_WINNT=${WINVER})
endif()


