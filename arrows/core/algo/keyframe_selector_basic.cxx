// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Implementation of keyframe_selector_basic

#include "keyframe_selector_basic.h"
#include <vital/types/feature_track_set.h>

using namespace kwiver::vital;

namespace kwiver {

namespace arrows {

namespace core {

class keyframe_selector_basic::priv
{
public:
  priv( keyframe_selector_basic& parent )
    : parent( parent )
  {}

  virtual ~priv() {}

  keyframe_selector_basic& parent;

  // Configuration values
  float
  c_fraction_tracks_lost_to_necessitate_new_keyframe() const
  {
    return parent.c_fraction_tracks_lost_to_necessitate_new_keyframe;
  }

  size_t
  c_keyframe_min_feature_count() const
  {
    return parent.c_keyframe_min_feature_count;
  }

  bool
  check_configuration( vital::config_block_sptr config ) const
  {
    bool success( true );

    float test_fraction_tracks_lost_to_necessitate_new_keyframe =
      config->get_value< float >(
        "fraction_tracks_lost_to_necessitate_new_keyframe",
        c_fraction_tracks_lost_to_necessitate_new_keyframe() );

    if( !( 0 < test_fraction_tracks_lost_to_necessitate_new_keyframe &&
           test_fraction_tracks_lost_to_necessitate_new_keyframe <= 1.0 ) )
    {
      LOG_ERROR(
        parent.logger(), "fraction_tracks_lost_to_necessitate_new_keyframe ("
          << test_fraction_tracks_lost_to_necessitate_new_keyframe
          << ") should be greater than zero and <= 1.0" );
      success = false;
    }

    int test_keyframe_min_feature_count = config->get_value< int >(
      "keyframe_min_feature_count", c_keyframe_min_feature_count() );

    if( test_keyframe_min_feature_count < 0 )
    {
      LOG_ERROR(
        parent.logger(), "keyframe_min_feature_count ("
          << test_keyframe_min_feature_count
          << ") should be greater than zero" );
      success = false;
    }

    return success;
  }

  void initial_keyframe_selection(
    kwiver::vital::track_set_sptr tracks );

  void continuing_keyframe_selection(
    kwiver::vital::track_set_sptr tracks );

  bool a_keyframe_was_selected(
    kwiver::vital::track_set_sptr tracks );
};

void
keyframe_selector_basic::priv
::initial_keyframe_selection(
  kwiver::vital::track_set_sptr tracks )
{
  auto ftracks = std::static_pointer_cast< feature_track_set >( tracks );
  auto keyframes = ftracks->keyframes();

  // start with first frame, can it be a keyframe?  If so add it, if not keep
  // going until we find a first suitable keyframe.
  auto frame_ids = tracks->all_frame_ids();
  for( auto frame : frame_ids )
  {
    if( keyframes.find( frame ) != keyframes.end() )
    {
      // If we are running this function, there haven't been any keyframes yet.
      // So, any frame in the metadata now will be a non-keyframe.
      continue;
    }

    bool is_keyframe = false;
    if( tracks->num_active_tracks( frame ) >= c_keyframe_min_feature_count() )
    {
      is_keyframe = true;
    }

    // this is the first frame that can be a keyframe
    auto ftsfd = std::make_shared< feature_track_set_frame_data >();
    ftsfd->is_keyframe = is_keyframe;

    tracks->set_frame_data( ftsfd, frame );
    if( is_keyframe )
    {
      break;
    }
  }
}

void
keyframe_selector_basic::priv
::continuing_keyframe_selection(
  kwiver::vital::track_set_sptr tracks )
{
  auto ftracks = std::static_pointer_cast< feature_track_set >( tracks );
  auto keyframes = ftracks->keyframes();
  // go to the last key-frame, then consider each frame newer than that one in
  // order and decide if it should be a keyframe
  if( keyframes.empty() )
  {
    return;
  }

  // find the last keyframe
  frame_id_t last_keyframe_id = *keyframes.rbegin();

  // calculate the id of the next frame that doesn't have keyframe metadata
  frame_id_t next_candidate_keyframe_id =
    ftracks->all_feature_frame_data().rbegin()->first + 1;

  frame_id_t last_frame_id = tracks->last_frame();

  for(; next_candidate_keyframe_id <= last_frame_id;
      ++next_candidate_keyframe_id )
  {
    auto active_tracks = tracks->active_tracks( next_candidate_keyframe_id );
    if( active_tracks.empty() )
    {
      // absolutely no tracks for this frame so it was skipped when reading.
      continue;
    }

    bool is_keyframe = true;
    double percentage_tracked =
      tracks->percentage_tracked(
        last_keyframe_id,
        next_candidate_keyframe_id );
    if( percentage_tracked >
        ( 1.0 - c_fraction_tracks_lost_to_necessitate_new_keyframe() ) )
    {
      is_keyframe = false;
    }

    // ok we could make this a keyframe.  Does it have enough features?
    if( active_tracks.size() < c_keyframe_min_feature_count() )
    {
      is_keyframe = false;
    }

    // add it's metadata to tracks
    auto ftsfd = std::make_shared< feature_track_set_frame_data >();
    ftsfd->is_keyframe = is_keyframe;
    tracks->set_frame_data( ftsfd, next_candidate_keyframe_id );
    if( is_keyframe )
    {
      last_keyframe_id = next_candidate_keyframe_id;
    }
  }
}

bool
keyframe_selector_basic::priv
::a_keyframe_was_selected(
  kwiver::vital::track_set_sptr tracks )
{
  auto ftracks = std::static_pointer_cast< feature_track_set >( tracks );
  auto keyframes = ftracks->keyframes();
  return !keyframes.empty();
}

// -----------------------------------------------------------------------------
void
keyframe_selector_basic
::initialize()
{
  KWIVER_INITIALIZE_UNIQUE_PTR( priv, d_ );
  attach_logger( "arrows.core.keyframe_selector_basic" );
}

// ----------------------------------------------------------------------------
void
keyframe_selector_basic
::set_configuration_internal( vital::config_block_sptr config )
{
  if( config->has_value( "fraction_tracks_lost_to_necessitate_new_keyframe" ) )
  {
    c_fraction_tracks_lost_to_necessitate_new_keyframe = config->get_value< float >(
      "fraction_tracks_lost_to_necessitate_new_keyframe" );
  }
  if( config->has_value( "keyframe_min_feature_count" ) )
  {
    c_keyframe_min_feature_count = config->get_value< size_t >(
      "keyframe_min_feature_count" );
  }
}

// ----------------------------------------------------------------------------
bool
keyframe_selector_basic
::check_configuration( vital::config_block_sptr config ) const
{
  return d_->check_configuration( config );
}

kwiver::vital::track_set_sptr
keyframe_selector_basic
::select( kwiver::vital::track_set_sptr tracks ) const
{
  // General idea here:
  // Add a key frame if
  // 1) Number of continuous feature tracks to a frame drops below 90%
  //    of features existing in any neighboring key-frame
  // 2) number of features in frame is greater than some minimum.  This prevents
  //    keyframes from being added in areas with little texture (few features).

  track_set_sptr cur_tracks = tracks;

  if( !d_->a_keyframe_was_selected( cur_tracks ) )
  {
    // we don't have any keyframe data yet for this set of tracks.
    d_->initial_keyframe_selection( cur_tracks );
  }

  if( d_->a_keyframe_was_selected( cur_tracks ) )
  {
    // check again because initial keyframe selection could have added a
    // keyframe
    d_->continuing_keyframe_selection( cur_tracks );
  }

  // return the copy of tracks
  return cur_tracks;
}

} // end namespace core

} // end namespace arrows

} // end namespace kwiver
