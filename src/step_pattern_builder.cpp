/*
 *  step_pattern_builder.cpp
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

#include <memory>

// Includes from libnestutil:
#include "logging.h"

// Includes from nestkernel:
#include "conn_parameter.h"
#include "exceptions.h"
#include "kernel_manager.h"
#include "nest_names.h"
#include "node.h"
#include "vp_manager_impl.h"

// Includes from sli:
#include "dict.h"
#include "fdstream.h"
#include "name.h"

#include "step_pattern_builder.h"

/** @BeginDocumentation
   Name: step_pattern - Rule connecting sources and targets with a step pattern

   Synopsis:
   source targets << /rule /step_pattern
                     /source_step M
                     /target_step N >> << syn spec >> Connect

   Parameters:
   source_step   - Make connection from every source_step'th neuron
   target_step   - Make connection to every target_step'th neuron

   Description:
   This connection rule subsamples the source and target arrays given with
   step sizes source_step and target_step, beginning with the first element
   in each array, and connects the selected nodes. If source_step and
   target_step both are equal 1, step_pattern is equivalent to all_to_all.

   Example:

   /n /iaf_psc_alpha 10 Create def
   n n << /rule /step_pattern /source_step 4 /target_step 3 >> Connect
   << >> GetConnections ==

     [<1,1,0,0,0> <1,4,0,0,1> <1,7,0,0,2> <1,10,0,0,3>
      <5,1,0,0,0> <5,4,0,0,1> <5,7,0,0,2> <5,10,0,0,3>
      <9,1,0,0,0> <9,4,0,0,1> <9,7,0,0,2> <9,10,0,0,3>]

   Remark:
   This rule is only provided as an example for how to write your own
   connection rule function.

   Author:
   Hans Ekkehard Plesser

   SeeAlso: Connect
*/

mynest::StepPatternBuilder::StepPatternBuilder( const nest::NodeCollectionPTR sources,
  const nest::NodeCollectionPTR targets,
  nest::ThirdOutBuilder* third_out,
  const DictionaryDatum& conn_spec,
  const std::vector< DictionaryDatum >& syn_spec )
  : nest::BipartiteConnBuilder( sources, targets, third_out, conn_spec, syn_spec )
  , source_step_( ( *conn_spec )[ Name( "source_step" ) ] )
  , target_step_( ( *conn_spec )[ Name( "target_step" ) ] )
{
  if ( source_step_ < 1 )
  {
    throw nest::BadParameter( "source_step >= 1 required." );
  }
  if ( target_step_ < 1 )
  {
    throw nest::BadParameter( "target_step >= 1 required." );
  }
}

void
mynest::StepPatternBuilder::connect_()
{
// This code is based on AllToAllBuilder, except that we step
// by source_step_ and target_step_ except for stepping by 1.

#pragma omp parallel
  {
    // get thread id
    const int tid = nest::kernel().vp_manager.get_thread_id();
    try

    {
      // allocate pointer to thread-specific random generator
      nest::RngPtr rng = nest::get_vp_specific_rng( tid );

      for ( auto target_it = targets_->begin(); target_it < targets_->end();
            advance_( target_it, targets_->end(), target_step_ ) )
      {
        for ( auto source_it = sources_->begin(); source_it < sources_->end();
              advance_( source_it, sources_->end(), source_step_ ) )
        {
          if ( not allow_autapses_ and ( *source_it ).node_id == ( *target_it ).node_id )
          {
            skip_conn_parameter_( tid );
            continue;
          }
          if ( not change_connected_synaptic_elements( ( *source_it ).node_id, ( *source_it ).node_id, tid, 1 ) )
          {
            for ( size_t i = 0; i < sources_->size(); ++i )
            {
              skip_conn_parameter_( tid );
            }
            continue;
          }
          nest::Node* const target = nest::kernel().node_manager.get_node_or_proxy( ( *target_it ).node_id, tid );
          const size_t target_thread = target->get_thread();
          single_connect_( ( *source_it ).node_id, *target, target_thread, rng );
        }
      }
    }
    catch ( std::exception& err )
    {
      // We must create a new exception here, err's lifetime ends at
      // the end of the catch block.
      exceptions_raised_.at( tid ) = std::make_shared< WrappedThreadException >( err );
    }
  }
}

void
mynest::StepPatternBuilder::advance_( nest::NodeCollection::const_iterator& it,
  const nest::NodeCollection::const_iterator& end,
  size_t step )
{
  while ( step > 0 and it < end )
  {
    --step;
    ++it;
  }
}
