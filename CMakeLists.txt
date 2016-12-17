# CMake entry point
cmake_minimum_required (VERSION 2.6)
project (Whac_A_Mole)

find_package(OpenGL REQUIRED)

# Compile external dependencies 
add_subdirectory (external)

# On Visual 2005 and above, this module can set the debug working directory
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/external/rpavlik-cmake-modules-1c73e35")
include(CreateLaunchers)
include(MSVCMultipleProcessCompile) # /MP

# - try to find the OpenAL ALUT library
#
# Users may optionally supply:
#  ALUT_ROOT_DIR - a prefix to start searching.
#
# Cache Variables: (probably not for direct use in your scripts)
#  ALUT_INCLUDE_DIR
#  ALUT_LIBRARY
#
# Non-cache variables you might use in your CMakeLists.txt:
#  ALUT_FOUND
#  ALUT_INCLUDE_DIRS
#  ALUT_LIBRARIES
#  ALUT_WORKAROUND_INCLUDE_DIRS - add these to your include path with
#   include_directories(${ALUT_WORKAROUND_INCLUDE_DIRS} ${ALUT_INCLUDE_DIRS})
#   so you can always #include <AL/al.h> and #include <AL/alut.h> even on
#   Mac where the paths might differ.
#
# Requires these CMake modules:
#  FindPackageHandleStandardArgs (known included with CMake >=2.6.2)
#
# Original Author:
# 2009-2010 Ryan Pavlik <rpavlik@iastate.edu> <abiryan@ryand.net>
# http://academic.cleardefinition.com
# Iowa State University HCI Graduate Program/VRAC
#
# Copyright Iowa State University 2009-2010.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

set(ALUT_ROOT_DIR
        "${ALUT_ROOT_DIR}"
        CACHE
        PATH
        "Path to search for ALUT library")

# Share search paths with OpenAL
if(NOT "$ENV{OPENALDIR}" STREQUAL "")
        if(NOT ALUT_ROOT_DIR)
                set(ALUT_ROOT_DIR "$ENV{OPENALDIR}")
        endif()
else()
        if(ALUT_ROOT_DIR)
                set(ENV{OPENALDIR} "${ALUT_ROOT_DIR}")
        endif()
endif()



###
# Configure ALUT
###
find_path(ALUT_INCLUDE_DIR
        NAMES
        alut.h
        HINTS
        "${ALUT_ROOT_DIR}"
        PATH_SUFFIXES
        AL
        alut
        OpenAL
        include
        include/alut
        include/freealut
        include/AL
        include/OpenAL
        PATHS
        /usr/local
        /opt/local
        /sw)
mark_as_advanced(ALUT_INCLUDE_DIR)

find_library(ALUT_LIBRARY
        NAMES
        alut
        HINTS
        "${ALUT_ROOT_DIR}"
        PATH_SUFFIXES
        lib
        lib64
        PATHS
        /usr/local
        /opt/local
        /sw)
mark_as_advanced(ALUT_LIBRARY)

###
# Prereq: OpenAL
###

# On Mac OS X, the ALUT headers were in the OpenAL framework until 10.4.7
# If we found ALUT headers elsewhere, it's probably freealut which may
# define the same symbols as the library in the framework (?)
# so we might want to save/restore the CMake variable that controls searching
# in frameworks
find_package(OpenAL QUIET)

# handle the QUIETLY and REQUIRED arguments and set xxx_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ALUT
        DEFAULT_MSG
        ALUT_LIBRARY
        ALUT_INCLUDE_DIR
        OPENAL_FOUND)

if(ALUT_FOUND)
        set(ALUT_INCLUDE_DIRS "${OPENAL_INCLUDE_DIR}" "${ALUT_INCLUDE_DIR}")
        set(ALUT_LIBRARIES "${OPENAL_LIBRARY}" ${ALUT_LIBRARY})
        if(APPLE)
                get_filename_component(_moddir ${CMAKE_CURRENT_LIST_FILE} PATH)
                if("${OPENAL_INCLUDE_DIR}" MATCHES "\\.framework$")
                        # OpenAL is in a framework - need a workaround
                        set(OPENAL_WORKAROUND_INCLUDE_DIR
                                "${_moddir}/workarounds/mac-openal")
                        list(APPEND
                                ALUT_WORKAROUND_INCLUDE_DIRS
                                "${OPENAL_WORKAROUND_INCLUDE_DIR}")
                endif()
                if("${ALUT_INCLUDE_DIR}" MATCHES "\\.framework$")
                        # ALUT is in the OpenAL framework - need a workaround
                        set(ALUT_WORKAROUND_INCLUDE_DIR
                                "${_moddir}/workarounds/mac-alut-framework")
                        list(APPEND
                                ALUT_WORKAROUND_INCLUDE_DIRS
                                "${ALUT_WORKAROUND_INCLUDE_DIR}")
                endif()
        endif()

        if("${ALUT_INCLUDE_DIR}" MATCHES "AL$")
                get_filename_component(_parent "${ALUT_INCLUDE_DIR}/.." ABSOLUTE)
                list(APPEND ALUT_INCLUDE_DIRS "${_parent}")
        endif()
        mark_as_advanced(ALUT_ROOT_DIR)
endif()


# - Find OpenAL
# Find the OpenAL includes and library
#
#  OPENAL_INCLUDE_DIR - Where to find OpenAL includes
#  OPENAL_LIBRARIES   - List of libraries when using OpenAL
#  OPENAL_FOUND       - True if OpenAL was found

IF(OPENAL_INCLUDE_DIR)
  SET(OPENAL_FIND_QUIETLY TRUE)
ENDIF(OPENAL_INCLUDE_DIR)

FIND_PATH(OPENAL_INCLUDE_DIR al.h
  PATHS
  $ENV{OPENAL_HOME}/include
  $ENV{EXTERNLIBS}/OpenAL/include
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw # Fink
  /opt/local # DarwinPorts
  /opt/csw # Blastwave
  /opt
  [HKEY_LOCAL_MACHINE\\SOFTWARE\\Creative\ Labs\\OpenAL\ 1.1\ Software\ Development\ Kit\\1.00.0000;InstallDir]
  PATH_SUFFIXES include/AL include/OpenAL include
  DOC "OpenAL - Headers"
)

SET(OPENAL_NAMES OpenAL al openal OpenAL32)
SET(OPENAL_DBG_NAMES OpenALd ald openald OpenAL32d)

FIND_LIBRARY(OPENAL_LIBRARY NAMES ${OPENAL_NAMES}
  PATHS
  $ENV{OPENAL_HOME}/lib
  $ENV{EXTERNLIBS}/OpenAL/lib
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw
  /opt/local
  /opt/csw
  /opt
  [HKEY_LOCAL_MACHINE\\SOFTWARE\\Creative\ Labs\\OpenAL\ 1.1\ Software\ Development\ Kit\\1.00.0000;InstallDir]
  PATH_SUFFIXES lib64 lib libs64 libs libs/Win32 libs/Win64
  DOC "OpenAL - Library"
)

INCLUDE(FindPackageHandleStandardArgs)

IF(MSVC)
  # VisualStudio needs a debug version
  FIND_LIBRARY(OPENAL_LIBRARY_DEBUG NAMES ${OPENAL_DBG_NAMES}
    PATHS
    $ENV{OPENAL_HOME}/lib
    $ENV{EXTERNLIBS}/OpenAL/lib
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local
    /usr
    /sw
    /opt/local
    /opt/csw
    /opt
    [HKEY_LOCAL_MACHINE\\SOFTWARE\\Creative\ Labs\\OpenAL\ 1.1\ Software\ Development\ Kit\\1.00.0000;InstallDir]
    PATH_SUFFIXES lib64 lib libs64 libs libs/Win32 libs/Win64
    DOC "OpenAL - Library (Debug)"
  )

  IF(OPENAL_LIBRARY_DEBUG AND OPENAL_LIBRARY)
    SET(OPENAL_LIBRARIES optimized ${OPENAL_LIBRARY} debug ${OPENAL_LIBRARY_DEBUG})
  ENDIF(OPENAL_LIBRARY_DEBUG AND OPENAL_LIBRARY)

  FIND_PACKAGE_HANDLE_STANDARD_ARGS(OPENAL DEFAULT_MSG OPENAL_LIBRARY OPENAL_LIBRARY_DEBUG OPENAL_INCLUDE_DIR)

  MARK_AS_ADVANCED(OPENAL_LIBRARY OPENAL_LIBRARY_DEBUG OPENAL_INCLUDE_DIR)

ELSE(MSVC)
  # rest of the world
  SET(OPENAL_LIBRARIES ${OPENAL_LIBRARY})

  FIND_PACKAGE_HANDLE_STANDARD_ARGS(OPENAL DEFAULT_MSG OPENAL_LIBRARY OPENAL_INCLUDE_DIR)

  MARK_AS_ADVANCED(OPENAL_LIBRARY OPENAL_INCLUDE_DIR)

ENDIF(MSVC)

IF(OPENAL_FOUND)
  SET(OPENAL_INCLUDE_DIRS ${OPENAL_INCLUDE_DIR})
ENDIF(OPENAL_FOUND)


if(INCLUDE_DISTRIB)
	add_subdirectory(distrib)
endif(INCLUDE_DISTRIB)



include_directories(
        external/AntTweakBar-1.16/include/
	external/glfw-2.7.6/include/
	external/glm-0.9.4.0/
	external/glew-1.9.0/include/
	external/assimp-3.0.1270/include/
	external/bullet-2.81-rev2613/src/
	.
)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	GLFW_276
        GLEW_190
        ${OPENAL_LIBRARIES}
        ${ALUT_LIBRARIES}
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

# Project
add_executable(Whac_A_Mole
	Whac_A_Mole/whac.cpp
	common/shader.cpp
	common/shader.hpp
	common/controls.cpp
	common/controls.hpp
	common/texture.cpp
	common/texture.hpp
	common/objloader.cpp
	common/objloader.hpp
	common/text2D.cpp
	common/text2D.hpp
)
target_link_libraries(Whac_A_Mole
	${ALL_LIBS}
)
# Xcode and Visual working directories
set_target_properties(Whac_A_Mole PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Whac_A_Mole/")
create_target_launcher(Whac_A_Mole WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Whac_A_Mole/")


SOURCE_GROUP(common REGULAR_EXPRESSION ".*/common/.*" )


if (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )

add_custom_command(
   TARGET Whac_A_Mole POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/Whac_A_Mole${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/Whac_A_Mole/"
)


elseif (${CMAKE_GENERATOR} MATCHES "Xcode" )

endif (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )
