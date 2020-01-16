###############################################################################
#
# cmake settings
#
###############################################################################

cmake_minimum_required (VERSION 2.8.11)

# no source tree builds please.
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR
        "Running cmake in the source directory is not supported!"
        "\n Remove the CMakeCache.txt file and try again from another folder, e.g.:"
        "\n "
        "\n rm CMakeCache.txt"
        "\n mkdir build"
        "\n cd build"
        "\n cmake .."
    )
endif()

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_SOURCE_DIR}/cmake")

# Use solution folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake Targets")

###############################################################################
#
# project settings
#
###############################################################################

project( lualept CXX )

set( VERSION_MAJOR 0 )
set( VERSION_MINOR 5 )
set( VERSION_PATCH 0 )
set( VERSION_PLAIN ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH} )

option( USE_SDL2 "Use SDL2 to to view Pix" ON )

set( CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_SOURCE_DIR}/cmake" )
set( EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin" )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}" )

###############################################################################
#
# compiler and linker
#
###############################################################################

set( LIBRARY_TYPE SHARED )
if ( STATIC )
    set( LIBRARY_TYPE )
endif()

if (WIN32)
    if (MSVC)
        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W1")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
    endif()
endif()

###############################################################################
#
# configure
#
###############################################################################

set( AUTOCONFIG_SRC "${CMAKE_SOURCE_DIR}/config/cmake-config.h.in" )
set( AUTOCONFIG "${CMAKE_BINARY_DIR}/src/config.h" )

include( Configure )

configure_file( "${AUTOCONFIG_SRC}" "${AUTOCONFIG}" )

###############################################################################
#
# build
#
###############################################################################

add_definitions( -DHAVE_CONFIG_H )

include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/src" )
include_directories( "${CMAKE_CURRENT_BINARY_DIR}/src" )

add_subdirectory( src )
add_subdirectory( prog )

