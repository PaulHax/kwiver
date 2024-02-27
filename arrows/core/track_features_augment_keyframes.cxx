// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Implementation of core track_features_augment_keyframes

#include "track_features_augment_keyframes.h"

#include <vector>

#include <vital/algo/detect_features.h>
#include <vital/algo/extract_descriptors.h>
#include <vital/exceptions/image.h>
using namespace kwiver::vital;

namespace kwiver {

namespace arrows {

namespace core {

class track_features_augment_keyframes::priv
{
public:
  priv( track_features_augment_keyframes& parent )
    : parent( parent ),
      detector_name( "kf_only_feature_detector" )
  {}

  track_features_augment_keyframes& parent;

  /// Configuration Values
  vital::algo::extract_descriptors_sptr c_extractor()
  { return parent.c_extractor; }
  vital::algo::extract_descriptors_sptr c_extractor_name()
  { return parent.c_extractor_name; }

  // Local value
  const std::string detector_name;
};

/// Augment existing tracks with additional feature if a keyframe
vital::feature_track_set_sptr
track_features_augment_keyframes
::track(
  kwiver::vital::feature_track_set_sptr tracks,
  kwiver::vital::frame_id_t frame_number,
  kwiver::vital::image_container_sptr image_data,
  kwiver::vital::image_container_sptr mask ) const
{
  // FORCE DETECTION ON EVERY FRAME

  // auto fmap = tracks->all_feature_frame_data();
  // auto ftsfd = fmap.find(frame_number);
  // if (ftsfd == fmap.end() || !ftsfd->second || !ftsfd->second->is_keyframe)
  // {
  //  // this is not a keyframe, so return the orignial tracks
  //  // no changes made so no deep copy necessary
  //  return tracks;
  // }

  auto track_states = tracks->frame_states( frame_number );
  auto new_feat = tracks->frame_features( frame_number );

  // describe the features.  Note this will recalculate the feature angles.
  vital::descriptor_set_sptr new_desc =
    d_->c_extractor()->extract( image_data, new_feat, mask );

  std::vector< feature_sptr > vf = new_feat->features();
  std::vector< descriptor_sptr > df = new_desc->descriptors();
  for( size_t i = 0; i < vf.size(); ++i )
  {
    auto feat = vf[ i ];
    auto desc = df[ i ];

    // Go through existing features and find the one that equals feat.
    // The feature pointers may have changed in detect so we can't use them
    // directly with a map.
    for( auto ts : track_states )
    {
      auto fts = std::static_pointer_cast< feature_track_state >( ts );
      if( fts && fts->feature && fts->feature->equal_except_for_angle( *feat ) )
      {
        // feature must be set because extract will have calculated a new
        // feature angle
        fts->feature = feat;
        fts->descriptor = desc;
        break;
      }
    }
  }

  return tracks;
}

// ----------------------------------------------------------------------------
void
track_features_augment_keyframes
::initialize()
{
  KWIVER_INITIALIZE_UNIQUE_PTR( priv, d_ );
  attach_logger( "arrows.core.track_features_augment_keyframes" );
}

/// Destructor
track_features_augment_keyframes
::~track_features_augment_keyframes() noexcept
{}

bool
track_features_augment_keyframes
::check_configuration( vital::config_block_sptr config ) const
{
  bool config_valid = true;

  config_valid = check_nested_algo_configuration< algo::detect_features >(
    d_->detector_name, config ) && config_valid;

  config_valid = check_nested_algo_configuration< algo::extract_descriptors >(
    d_->c_extractor_name(), config ) && config_valid;

  return config_valid;
}

} // end namespace core

} // end namespace arrows

} // end namespace kwiver
