// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Implementation of close_loops_exhaustive

#include "close_loops_exhaustive.h"
#include "match_tracks.h"

#include <algorithm>
#include <set>
#include <vector>

#include <vital/algo/match_features.h>
#include <vital/exceptions/algorithm.h>
#include <vital/util/thread_pool.h>

namespace kwiver {

namespace arrows {

namespace core {

using namespace kwiver::vital;

/// Private implementation class
class close_loops_exhaustive::priv
{
public:
  priv( close_loops_exhaustive& parent )
    : parent( parent )
  {}

  close_loops_exhaustive& parent;

  /// number of feature matches required for acceptance
  size_t c_match_req() { return parent.c_match_req; }

  /// Max frames to close loops back to (-1 to beginning of sequence)
  int c_num_look_back() { return parent.c_num_look_back; }

  /// The feature matching algorithm to use
  vital::algo::match_features_sptr c_matcher()
  { return parent.c_matcher; }
};

// ----------------------------------------------------------------------------
void
close_loops_exhaustive
::initialize()
{
  KWIVER_INITIALIZE_UNIQUE_PTR( priv, d_ );
  attach_logger( "arrows.core.close_loops_exhaustive" );
}

/// Destructor
close_loops_exhaustive
::~close_loops_exhaustive() noexcept
{}

// ----------------------------------------------------------------------------
bool
close_loops_exhaustive
::check_configuration( vital::config_block_sptr config ) const
{
  return (
    check_nested_algo_configuration< algo::match_features >(
      "feature_matcher",
      config )
  );
}

/// Exaustive loop closure
vital::feature_track_set_sptr
close_loops_exhaustive
::stitch(
  vital::frame_id_t frame_number,
  vital::feature_track_set_sptr input,
  vital::image_container_sptr,
  vital::image_container_sptr ) const
{
  frame_id_t last_frame = 0;
  if( d_->c_num_look_back() >= 0 )
  {
    const int fnum = static_cast< int >( frame_number );
    last_frame = std::max< int >( fnum - d_->c_num_look_back(), 0 );
  }

  std::vector< vital::track_sptr > all_tracks = input->tracks();
  auto current_set = std::make_shared< vital::feature_track_set >(
    input->active_tracks( frame_number ) );

  std::vector< vital::track_sptr > current_tracks = current_set->tracks();
  vital::descriptor_set_sptr current_descriptors =
    current_set->frame_descriptors( frame_number );
  vital::feature_set_sptr current_features =
    current_set->frame_features( frame_number );

  // lambda function to encapsulate the parameters to be shared across all
  // threads
  auto match_func = [=](frame_id_t f){
                      return match_tracks(
                        d_->c_matcher(), input, current_set,
                        current_features, current_descriptors, f );
                    };

  // access the thread pool
  vital::thread_pool& pool = vital::thread_pool::instance();

  std::map< vital::frame_id_t, std::future< track_pairs_t > > all_matches;
  // enqueue a task to run matching for each frame within a neighborhood
  for( vital::frame_id_t f = frame_number - 2; f >= last_frame; f-- )
  {
    all_matches[ f ] = pool.enqueue( match_func, f );
  }

  std::map< vital::frame_id_t, track_pairs_t > all_matches_done;
  // ensure all parallel jobs are finished before modifying input
  for( auto& pair : all_matches )
  {
    all_matches_done[ pair.first ] = std::move( pair.second.get() );
  }

  // retrieve match results and stitch frames together
  for( vital::frame_id_t f = frame_number - 2; f >= last_frame; f-- )
  {
    auto const& matches = all_matches_done[ f ];
    size_t num_matched = matches.size();
    int num_linked = 0;
    if( num_matched >= d_->c_match_req() )
    {
      for( auto const& m : matches )
      {
        if( input->merge_tracks( m.first, m.second ) )
        {
          ++num_linked;
        }
      }
    }

    LOG_INFO(
      logger(), "Matching frame " << frame_number << " to " << f
                                  << " has " << num_matched << " matches and "
                                  << num_linked << " joined tracks" );
  }

  return input;
}

} // end namespace core

} // end namespace arrows

} // end namespace kwiver
