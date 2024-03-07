// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Implementation of close_loops_keyframe

#include "close_loops_keyframe.h"
#include "match_tracks.h"

#include <set>
#include <string>
#include <vector>

#include <vital/algo/match_features.h>
#include <vital/exceptions/algorithm.h>
#include <vital/util/thread_pool.h>

namespace kwiver {

namespace arrows {

namespace core {

using namespace kwiver::vital;

/// Private implementation class
class close_loops_keyframe::priv
{
public:
  priv( close_loops_keyframe& parent )
    : parent( parent )
  {}

  close_loops_keyframe& parent;

  /// number of feature matches required for acceptance
  int c_match_req() { return parent.c_match_req; }

  /// number of adjacent frames to match
  int c_search_bandwidth() { return parent.c_search_bandwidth; }

  /// minimum number of keyframe misses before creating a new keyframe
  unsigned int c_min_keyframe_misses() { return parent.c_min_keyframe_misses; }

  /// stop matching against additional keyframes if at least one succeeds
  bool c_stop_after_match() { return parent.c_stop_after_match; }

  /// The feature matching algorithm to use
  vital::algo::match_features_sptr c_matcher()
  { return parent.c_matcher; }

  /// histogram of matches associated with each frame
  std::map< frame_id_t, unsigned int > frame_matches;

  /// a collection of recent frame that didn't match any keyframe
  std::vector< frame_id_t > keyframe_misses;
};

// ----------------------------------------------------------------------------
void
close_loops_keyframe
::initialize()
{
  KWIVER_INITIALIZE_UNIQUE_PTR( priv, d_ );
  attach_logger( "arrows.core.close_loops_keyframe" );
}

/// Destructor
close_loops_keyframe
::~close_loops_keyframe() noexcept
{}

// ----------------------------------------------------------------------------
bool
close_loops_keyframe
::check_configuration( vital::config_block_sptr config ) const
{
  return (
    check_nested_algo_configuration< algo::match_features >(
      "feature_matcher",
      config ) &&
    config->get_value< int >( "search_bandwidth" ) >= 1 &&
    config->get_value< int >( "min_keyframe_misses" ) >= 1
  );
}

// ----------------------------------------------------------------------------
/// Frame stitching using keyframe-base matching
vital::feature_track_set_sptr
close_loops_keyframe
::stitch(
  vital::frame_id_t frame_number,
  vital::feature_track_set_sptr input,
  vital::image_container_sptr,
  vital::image_container_sptr ) const
{
  // initialize frame matches for this frame
  d_->frame_matches[ frame_number ] = 0;

  // get a vector of all frame numbers contained in the tracks
  auto frame_set = input->all_frame_ids();
  // Indices of the the selected keyframes
  auto keyframe_set = input->keyframes();
  std::vector< frame_id_t > frames( frame_set.begin(), frame_set.end() );
  std::vector< frame_id_t > keyframes( keyframe_set.begin(),
    keyframe_set.end() );

  // do nothing for the first two frames, there is nothing to match
  if( frames.size() <= 2 )
  {
    return input;
  }

  // compute the last frame we need to match to within the search bandwidth
  // the conditional accounts for the boundary case at startup
  auto last_frame_itr = frames.rend();
  if( frames.size() > static_cast< size_t >( d_->c_search_bandwidth() ) )
  {
    last_frame_itr = frames.rbegin() + d_->c_search_bandwidth();
  }

  // the first frame is always a key frame (for now)
  // This could proably be improved
  if( keyframes.empty() )
  {
    keyframes.push_back( input->first_frame() );

    auto fd = input->frame_data( input->first_frame() );
    auto ffd =
      std::dynamic_pointer_cast< vital::feature_track_set_frame_data >( fd );
    if( !ffd )
    {
      ffd = std::make_shared< vital::feature_track_set_frame_data >();
      input->set_frame_data( ffd, input->first_frame() );
    }
    ffd->is_keyframe = true;
  }

  // extract the subset of tracks on the current frame and their
  // features and descriptors
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

  // Initialize frame_matches to the number of tracks already matched
  // between the current and previous frames.  This matching was done outside
  // of loop closure as part of the standard frame-to-frame tracking
  d_->frame_matches[ frame_number ] =
    static_cast< unsigned int >( current_set->active_tracks(
      frames[ frames.size() - 2 ] ).size() );

  // used to compute the maximum number of matches between the current frame
  // and any of the key frames
  int max_keyframe_matched = 0;

  // use this iterator to step backward through the keyframes
  // as we step backward through the neighborhood to identify
  // which neighborhood frames are also keyframes
  auto kitr = keyframes.rbegin();
  // since loop closure starts at frame n-2, if the latest
  // keyframe happens to be n-1 we need to skip that one
  if( *kitr == frames[ frames.size() - 2 ] )
  {
    ++kitr;
  }

  // access the thread pool
  vital::thread_pool& pool = vital::thread_pool::instance();

  std::map< vital::frame_id_t, std::future< track_pairs_t > > all_matches;
  // stitch with all frames within a neighborhood of the current frame
  for( auto f = frames.rbegin() + 2; f != last_frame_itr; ++f )
  {
    all_matches[ *f ] = pool.enqueue( match_func, *f );
  }
  // stitch with all previous keyframes
  for( auto k_itr = keyframes.rbegin(); k_itr != keyframes.rend(); ++k_itr )
  {
    // if this frame was already matched above then skip it
    if( last_frame_itr == frames.rend() || *k_itr >= *last_frame_itr )
    {
      continue;
    }
    all_matches[ *k_itr ] = pool.enqueue( match_func, *k_itr );
  }

  // stitch with all frames within a neighborhood of the current frame
  for( auto f = frames.rbegin() + 2; f != last_frame_itr; ++f )
  {
    if( !all_matches[ *f ].valid() )
    {
      LOG_WARN(
        logger(), "match from " << frame_number << " to "
                                << *f << " not available" );
      continue;
    }

    auto const& matches = all_matches[ *f ].get();
    int num_matched = static_cast< int >( matches.size() );
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
    // accumulate matches to help assign keyframes later
    d_->frame_matches[ frame_number ] += num_matched;

    // keyframes can occur within the current search neighborhood
    // if this frame is a keyframe then account for it in the
    // computation of the maximum number of matches to all keyframes
    std::string frame_name = "";
    if( kitr != keyframes.rend() && *f == *kitr )
    {
      if( num_matched > max_keyframe_matched )
      {
        max_keyframe_matched = num_matched;
      }
      ++kitr;
      frame_name = "keyframe ";
    }
    LOG_INFO(
      logger(), "Matching frame " << frame_number << " to "
                                  << frame_name << *f
                                  << " has " << num_matched << " matches and "
                                  << num_linked << " joined tracks" );
  }
  // divide by number of matched frames to get the average
  d_->frame_matches[ frame_number ] /=
    static_cast< unsigned int >( last_frame_itr - frames.rbegin() - 2 );

  // stitch with all previous keyframes
  for( auto k_itr = keyframes.rbegin(); k_itr != keyframes.rend(); ++k_itr )
  {
    // if this frame was already matched above then skip it
    if( last_frame_itr == frames.rend() || *k_itr >= *last_frame_itr )
    {
      continue;
    }
    if( !all_matches[ *k_itr ].valid() )
    {
      LOG_WARN(
        logger(), "keyframe match from " << frame_number << " to "
                                         << *k_itr << " not available" );
      continue;
    }

    auto const& matches = all_matches[ *k_itr ].get();
    int num_matched = static_cast< int >( matches.size() );
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
      logger(), "Matching frame " << frame_number << " to keyframe " << *k_itr
                                  << " has " << num_matched << " matches and "
                                  << num_linked << " joined tracks" );
    if( num_matched > max_keyframe_matched )
    {
      max_keyframe_matched = num_matched;
    }
    // if the stop-after-match option is set and we've already matched a
    // keyframe
    // but this key frame did not match, then exit the loop early and don't
    // match any more key frames.
    if( d_->c_stop_after_match() &&
        max_keyframe_matched >= d_->c_match_req() &&
        num_matched < d_->c_match_req() )
    {
      break;
    }
  } // end for

  // keep track of frames that matched no keyframes
  if( max_keyframe_matched < d_->c_match_req() )
  {
    d_->keyframe_misses.push_back( frame_number );
    LOG_DEBUG(
      logger(), "Frame " << frame_number << " added to keyframe misses. "
                         << "Count: " << d_->keyframe_misses.size() );
  }

  // If we've seen enough keyframe misses and the first miss has passed outside
  // of the search bandwidth, then add a new key frame by selecting the frame
  // since the first miss that has been most successful at matching.
  if( d_->keyframe_misses.size() > d_->c_min_keyframe_misses() &&
      d_->keyframe_misses.front() < *last_frame_itr )
  {
    auto hitr = d_->frame_matches.find( d_->keyframe_misses.front() );
    unsigned int max_matches = 0;
    frame_id_t max_id = d_->keyframe_misses.front();
    // find the frame with the most accumulated matches
    for( ++hitr; hitr != d_->frame_matches.end(); ++hitr )
    {
      if( hitr->second > max_matches )
      {
        max_matches = hitr->second;
        max_id = hitr->first;
      }
    }
    // the new key frame must have the required number of matches on average
    if( max_matches > static_cast< unsigned int >( d_->c_match_req() ) )
    {
      // create the new keyframe and clear the list of misses
      LOG_INFO(logger(), "creating new keyframe on frame " << max_id);
      keyframes.push_back( max_id );
      d_->keyframe_misses.clear();

      auto fd = input->frame_data( max_id );
      auto ffd = std::dynamic_pointer_cast< vital::feature_track_set_frame_data >( fd );
      if( !ffd )
      {
        ffd = std::make_shared< vital::feature_track_set_frame_data >();
        input->set_frame_data( ffd, max_id );
      }
      ffd->is_keyframe = true;
    }
  }

  return input;
}

} // end namespace core

} // end namespace arrows

} // end namespace kwiver
