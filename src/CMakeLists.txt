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

# Add all your sources here and specify dependencies, e.g., link libraries
set( MODULE_SOURCES
     mymodule.cpp
     pif_psc_alpha.h pif_psc_alpha.cpp
     drop_odd_spike_connection.h drop_odd_spike_connection.cpp
     step_pattern_builder.h step_pattern_builder.cpp
     recording_backend_socket.h recording_backend_socket.cpp
   )

check_have_sfml_audio()
if ( HAVE_SFML_AUDIO )
  list( APPEND MODULE_SOURCES   
        recording_backend_soundclick.h recording_backend_soundclick.cpp
      )
  set( USER_LINK_LIBRARIES sfml-audio )
  add_definitions( -DHAVE_SFML_AUDIO )
else ()
  message( "-- Skip build of recording backend >SoundClick<. sfml-audio is not installed." )
endif ()

add_library( ${MODULE_NAME}_module MODULE ${MODULE_SOURCES} )
target_link_libraries(${MODULE_NAME}_module ${USER_LINK_LIBRARIES})
set_target_properties( ${MODULE_NAME}_module
   PROPERTIES
   COMPILE_FLAGS "${NEST_CXXFLAGS} -DLTX_MODULE"
   LINK_FLAGS "${NEST_LIBS}"
   PREFIX ""
   OUTPUT_NAME ${MODULE_NAME} )
install( TARGETS ${MODULE_NAME}_module
   DESTINATION ${CMAKE_INSTALL_LIBDIR}/nest
   )

