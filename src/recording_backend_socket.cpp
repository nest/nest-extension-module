/*
 *  recording_backend_socket.cpp
 *
 *  This file is part of NEST.
 *
 *  Copyright (C) 2004 The NEST Initiative
 *
 *  NEST is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  NEST is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with NEST.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

// Includes from libnestutil:
#include "compose.hpp"

// Includes from nestkernel:
#include "recording_device.h"
#include "vp_manager_impl.h"

// Includes from sli:
#include "dictutils.h"

#include "recording_backend_socket.h"

mynest::RecordingBackendSocket::RecordingBackendSocket()
{
}

mynest::RecordingBackendSocket::~RecordingBackendSocket() throw()
{
}

void
mynest::RecordingBackendSocket::initialize()
{
  // nothing to do
}

void
mynest::RecordingBackendSocket::finalize()
{
  // nothing to do
}

void
mynest::RecordingBackendSocket::enroll( const nest::RecordingDevice& device, const DictionaryDatum& params )
{
  if ( device.get_type() != nest::RecordingDevice::SPIKE_RECORDER )
  {
    throw nest::BadProperty( "Only spike recorders can record to recording backend 'socket'" );
  }
}

void
mynest::RecordingBackendSocket::disenroll( const nest::RecordingDevice& device )
{
  // nothing to do
}

void
mynest::RecordingBackendSocket::set_value_names( const nest::RecordingDevice&,
  const std::vector< Name >&,
  const std::vector< Name >& )
{
  // nothing to do
}

void
mynest::RecordingBackendSocket::pre_run_hook()
{
  // nothing to do
}

void
mynest::RecordingBackendSocket::post_run_hook()
{
  // nothing to do
}

void
mynest::RecordingBackendSocket::post_step_hook()
{
  // nothing to do
}

void
mynest::RecordingBackendSocket::prepare()
{
  B_.addr_.sin_family = AF_INET;
  inet_aton( P_.ip_.c_str(), &B_.addr_.sin_addr );
  B_.addr_.sin_port = htons( P_.port_ );

  B_.socket_ = socket( PF_INET, SOCK_DGRAM, 0 );
}

void
mynest::RecordingBackendSocket::cleanup()
{
  close( B_.socket_ );
}

void
mynest::RecordingBackendSocket::write( const nest::RecordingDevice& device,
				       const nest::Event& event,
  const std::vector< double >& double_values,
  const std::vector< long >& long_values )
{
  assert( device.get_type() == nest::RecordingDevice::SPIKE_RECORDER );

#pragma omp critical
  {
    size_t sd_node_id = device.get_node_id();
    size_t node_node_id = event.get_sender_node_id();
    std::string msg = String::compose( "spike_recorder %1 got a spike by node %2", sd_node_id, node_node_id );

    // We explicitly ignore errors here by not evaluating the return
    // code of the sendto() function.
    sendto( B_.socket_, msg.c_str(), msg.size(), 0, ( struct sockaddr* ) &B_.addr_, sizeof( B_.addr_ ) );
  }
}

mynest::RecordingBackendSocket::Parameters_::Parameters_()
  : ip_( "127.0.0.1" )
  , port_( 50000 )
{
}

void
mynest::RecordingBackendSocket::Parameters_::get( DictionaryDatum& d ) const
{
  ( *d )[ "ip" ] = ip_;
  ( *d )[ "port" ] = port_;
}

void
mynest::RecordingBackendSocket::Parameters_::set( const DictionaryDatum& d )
{
  updateValue< std::string >( d, "ip", ip_ );
  updateValue< long >( d, "port", port_ );
}

// Set the status of the recording backend
void
mynest::RecordingBackendSocket::set_status( const DictionaryDatum& d )
{
  Parameters_ ptmp = P_; // temporary copy in case of errors
  ptmp.set( d );         // throws if BadProperty

  // if we get here, temporaries contain consistent set of properties
  P_ = ptmp;
}

// Return the status of the recording backend
void
mynest::RecordingBackendSocket::get_status( DictionaryDatum& d ) const
{
  P_.get( d );
}

void
mynest::RecordingBackendSocket::check_device_status( const DictionaryDatum& ) const
{
  // nothing to do
}

void
mynest::RecordingBackendSocket::get_device_defaults( DictionaryDatum& ) const
{
  // nothing to do
}

void
mynest::RecordingBackendSocket::get_device_status( const nest::RecordingDevice&, DictionaryDatum& ) const
{
  // nothing to do
}
