################################################################################
#                                                                              #
#  Copyright (c) 2005-2011, Michele Bosi, Thiago Bastos                        #
#  All rights reserved.                                                        #
#                                                                              #
#  This file is part of Visualization Library                                  #
#  http://www.visualizationlibrary.org                                         #
#                                                                              #
#  Released under the OSI approved Simplified BSD License                      #
#  http://www.opensource.org/licenses/bsd-license.php                          #
#                                                                              #
################################################################################

################################################################################
# VLLoadWriter Library
################################################################################

project(VLExtLoadWriter)

# Gather VLExtLoadWriter source files
file(GLOB VLEXTLOADWRITER_SRC "*.cpp")
file(GLOB VLEXTLOADWRITER_INC "*.hpp")

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


# Handle extras added by plugins
#VL_PROJECT_GET(VLExtLoadWriter _SOURCES _DEFINITIONS _INCLUDE_DIRS _EXTRA_LIBS_D _EXTRA_LIBS_R)
add_definitions(${_DEFINITIONS})
include_directories(${_INCLUDE_DIRS} ${assimp_INCLUDE_DIRS})

add_library(VLExtLoadWriter ${VLEXT_SHARED_OR_STATIC} ${VLEXTLOADWRITER_SRC} ${VLEXTLOADWRITER_INC})
#VL_DEFAULT_TARGET_PROPERTIES(VLParticle)

target_link_libraries(VLExtLoadWriter ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} ${VL_LIBRARIES} ${assimp_LIBRARIES})

foreach(libName ${_EXTRA_LIBS_D})
	target_link_libraries(VLExtLoadWriter debug ${libName})
endforeach()

foreach(libName ${_EXTRA_LIBS_R})
	target_link_libraries(VLExtLoadWriter optimized ${libName})
endforeach()


################################################################################
# Install Rules
################################################################################

#VL_INSTALL_TARGET(VLEXTLOADWRITER)

# VLCore headers
install(FILES ${VLEXTLOADWRITER_INC} DESTINATION "${VLEXT_INCLUDE_INSTALL_DIR}/vlExtLoadWriter")
