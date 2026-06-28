# CMakeLists.txt
#
# This file is part of the NEST example module.
#
# Copyright (C) 2004 The NEST Initiative
#
# NEST is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# NEST is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with NEST.  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required( VERSION 3.19 )

list( APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake )
include( ProcessDependencies )

# Build the complete module name:
set( MODULE_NAME mymodule )

# Specify your module version
set( MODULE_VERSION_MAJOR 0 )
set( MODULE_VERSION_MINOR 1 )
set( MODULE_VERSION "${MODULE_VERSION_MAJOR}.${MODULE_VERSION_MINOR}" )

if ( with-nest )
   # NESTSimulator_ROOT is the variable CMake's find_package() itself looks
   # for (CMP0074) to locate NESTSimulatorConfig.cmake -- no need to fold it
   # into CMAKE_PREFIX_PATH by hand.
   set( NESTSimulator_ROOT "${with-nest}" )
   set( NEST_CONFIG "${NESTSimulator_ROOT}/bin/nest-config" )
else ()
   message( FATAL_ERROR "-Dwith-nest=<path_to_nest_installation> is required" )
endif ()

# Use `nest-config` to get the compiler that was used for NEST. Needed before
# project() can run, so this -- and the basic check that `nest-config` is
# actually reachable under the given root -- cannot go through find_package().
execute_process(
    COMMAND ${NEST_CONFIG} --compiler
    RESULT_VARIABLE RES_VAR
    OUTPUT_VARIABLE NEST_COMPILER
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if ( NOT RES_VAR EQUAL 0 )
  message( FATAL_ERROR "Cannot run `${NEST_CONFIG}`. Please specify a valid NEST install prefix via -Dwith-nest=... " )
endif ()

set( CMAKE_CXX_COMPILER "${NEST_COMPILER}" )
project( ${MODULE_NAME} CXX )

# Find NEST's CMake CONFIG-mode package (NESTSimulatorConfig.cmake), hinted
# by NESTSimulator_ROOT above. Provides the NEST::nest target (compile
# features, include dirs, link libraries -- the Darwin "dynamic_lookup" link
# option is already part of the target, so no separate APPLE-specific
# handling is needed here as it used to be with `nest-config --libs`).
find_package( NESTSimulator REQUIRED )

# Same install prefix find_package() just used -- no nest-config call needed.
set( NEST_INSTALL_PREFIX "${NESTSimulator_ROOT}" )

# Everything below is information `nest-config` provides that the CONFIG
# package above does not (yet): the Python interpreter NEST was built
# against, and its install layout for docs/data/libs.
execute_process(
    COMMAND ${NEST_CONFIG} --python-executable
    RESULT_VARIABLE RES_VAR
    OUTPUT_VARIABLE PYTHON_EXECUTABLE
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND ${NEST_CONFIG} --python-version
    RESULT_VARIABLE RES_VAR
    OUTPUT_VARIABLE PYTHON_VERSION_STRING
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND ${NEST_CONFIG} --datadir
    RESULT_VARIABLE RES_VAR
    OUTPUT_VARIABLE NEST_INSTALL_DATADIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND ${NEST_CONFIG} --docdir
    RESULT_VARIABLE RES_VAR
    OUTPUT_VARIABLE NEST_INSTALL_DOCDIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND ${NEST_CONFIG} --libdir
    RESULT_VARIABLE RES_VAR
    OUTPUT_VARIABLE NEST_LIBDIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# on OS X
set( CMAKE_MACOSX_RPATH ON )

# Use the NEST_INSTALL_* variables as CMAKE_INSTALL_*
set( CMAKE_INSTALL_PREFIX "${NEST_INSTALL_PREFIX}" CACHE STRING "Install path prefix, prepended onto install directories." FORCE )
set( CMAKE_INSTALL_LIBDIR "${NEST_LIBDIR}" CACHE STRING "object code libraries (lib/nest or lib64/nest or lib/<multiarch-tuple>/nest on Debian)" FORCE )
set( CMAKE_INSTALL_DOCDIR "${NEST_INSTALL_DOCDIR}" CACHE STRING "documentation root (DATAROOTDIR/doc/nest)" FORCE )
set( CMAKE_INSTALL_DATADIR "${NEST_INSTALL_DATADIR}" CACHE STRING "read-only architecture-independent data (DATAROOTDIR/nest)" FORCE )

include( GNUInstallDirs )

# CPack stuff. Required for target `dist`.
set( CPACK_GENERATOR TGZ )
set( CPACK_SOURCE_GENERATOR TGZ )

set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "NEST Module ${MODULE_NAME}" )
set( CPACK_PACKAGE_VENDOR "NEST Initiative (https://www.nest-initiative.org/)" )

set( CPACK_PACKAGE_VERSION_MAJOR ${MODULE_VERSION_MAJOR} )
set( CPACK_PACKAGE_VERSION_MINOR ${MODULE_VERSION_MINOR} )
set( CPACK_PACKAGE_VERSION ${MODULE_VERSION} )

set( CPACK_SOURCE_IGNORE_FILES
    "\\\\.gitignore"
    "\\\\.git/"
    "\\\\.travis\\\\.yml"

    # if we have in source builds
    "/build/"
    "/_CPack_Packages/"
    "CMakeFiles/"
    "cmake_install\\\\.cmake"
    "Makefile.*"
    "CMakeCache\\\\.txt"
    "CPackConfig\\\\.cmake"
    "CPackSourceConfig\\\\.cmake"
    )
set( CPACK_SOURCE_PACKAGE_FILE_NAME ${MODULE_NAME} )

set( CPACK_PACKAGE_INSTALL_DIRECTORY "${MODULE_NAME} ${MODULE_VERSION}" )
include( CPack )

# add make dist target
add_custom_target( dist
    COMMAND ${CMAKE_MAKE_PROGRAM} package_source
    # not sure about this... seems, that it will be removed before dist...
    # DEPENDS doc
    COMMENT "Creating a source distribution from ${MODULE_NAME}..."
    )

# check the user link libraries whether installed or not
foreach( LINKLIB ${USER_LINK_LIBRARIES} )
  find_library( LIB_${LINKLIB} ${LINKLIB} )
  if( NOT LIB_${LINKLIB} )
    message(FATAL_ERROR "ERROR: " ${LINKLIB} " library not found")
  else()  
    message( "-- " ${LINKLIB} " library found")
  endif()
endforeach()

add_subdirectory( src )

# Install help --- based on doc/CMakeLists.txt
# Install only if Py >= 2.7.8 and NEST was installed with help
if ( ( NOT CMAKE_CROSSCOMPILING )
     AND PYTHON_EXECUTABLE
     AND ( IS_DIRECTORY ${NEST_INSTALL_PREFIX}/${NEST_INSTALL_DOCDIR}/help )
   )

    # Extract help from all source files in the source code, put
    # them in doc/help and generate a local help index in the
    # build directory containing links to the help files.
    install( CODE
      "execute_process(
         COMMAND ${PYTHON_EXECUTABLE} -B generate_help.py \"${PROJECT_SOURCE_DIR}\" \"${PROJECT_BINARY_DIR}\"
         WORKING_DIRECTORY \"${NEST_INSTALL_PREFIX}/${NEST_INSTALL_DATADIR}/help_generator\"
         )"
      )

    # Copy the local doc/help directory to the global installation
    # directory for documentation.
    install( DIRECTORY "${PROJECT_BINARY_DIR}/doc/help"
      DESTINATION "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DOCDIR}"
      OPTIONAL
      )

    # Update the global help index to include all help files in
    # the global installation directory for documentation.
    install( CODE
      "execute_process(
         COMMAND ${PYTHON_EXECUTABLE} -B generate_helpindex.py \"${NEST_INSTALL_PREFIX}/${NEST_INSTALL_DOCDIR}\"
         WORKING_DIRECTORY \"${NEST_INSTALL_PREFIX}/${NEST_INSTALL_DATADIR}/help_generator\"
         )"
      )

  endif ()

message( "" )
message( "-------------------------------------------------------" )
message( "${MODULE_NAME} Configuration Summary" )
message( "-------------------------------------------------------" )
message( "" )
get_target_property( NEST_INCLUDES NEST::nest INTERFACE_INCLUDE_DIRECTORIES )
get_target_property( NEST_LIBS NEST::nest INTERFACE_LINK_LIBRARIES )

message( "C++ compiler         : ${CMAKE_CXX_COMPILER}" )
message( "Build static libs    : ${NEST_STATIC_LIB}" )
message( "C++ compiler flags   : ${CMAKE_CXX_FLAGS}" )
message( "NEST include dirs    : ${NEST_INCLUDES}" )
message( "NEST libraries       : ${NEST_LIBS}" )
message( "User link libraries  : ${USER_LINK_LIBRARIES}" )
message( "" )
message( "-------------------------------------------------------" )
message( "" )
message( "You can now build and install '${MODULE_NAME}' using" )
message( "  make" )
message( "  make install" )
message( "" )
if ( "${CMAKE_HOST_SYSTEM_NAME}" MATCHES "^Darwin.*" )
  message( "The library file lib${MODULE_NAME}.dylib will be installed to" )
else ()
  message( "The library file lib${MODULE_NAME}.so will be installed to" )
endif()
message( "  ${CMAKE_INSTALL_FULL_LIBDIR}/nest/" )
message( "Help files will be installed to" )
message( "  ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DOCDIR}" )
message( "" )
message( "The module can be loaded into NEST using" )
message( "  nest.Install('${MODULE_NAME}')  (in PyNEST)" )
message( "  (${MODULE_NAME}) Install        (in SLI)" )
message( "" )

if( NOT "${CMAKE_INSTALL_PREFIX}" STREQUAL "${NEST_INSTALL_PREFIX}" )
  message( "The module will be installed into a non-default location!" )
  message( "Make sure to set the environment variables:" )
  if ( "${CMAKE_HOST_SYSTEM_NAME}" MATCHES "^Darwin.*" )
    message( "  export DYLD_LIBRARY_PATH=${CMAKE_INSTALL_FULL_LIBDIR}/nest:$DYLD_LIBRARY_PATH" )
  else ()
    message( "  export LD_LIBRARY_PATH=${CMAKE_INSTALL_FULL_LIBDIR}/nest:$LD_LIBRARY_PATH" )
  endif()
  message( "" )
endif ()
