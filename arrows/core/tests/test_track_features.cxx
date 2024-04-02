// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <arrows/core/algo/track_features_augment_keyframes.h>
#include <arrows/core/algo/track_features_core.h>
#include <vital/plugin_management/pluggable_macro_testing.h>
#include <vital/plugin_management/plugin_manager.h>

#include <gtest/gtest.h>

#include <algorithm>

using namespace kwiver::vital;
using namespace kwiver::arrows::core;

// ----------------------------------------------------------------------------
int
main( int argc, char** argv )
{
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}

// ----------------------------------------------------------------------------
TEST ( track_features_core, create )
{
  plugin_manager::instance().load_all_plugins();

  EXPECT_NE( nullptr, create_algorithm< algo::track_features >( "core" ) );
}

// ----------------------------------------------------------------------------
TEST ( track_features_augment_keyframes, create )
{
  plugin_manager::instance().load_all_plugins();

  EXPECT_NE(
    nullptr, create_algorithm< algo::track_features >(
      "augment_keyframes" ) );
}

// ----------------------------------------------------------------------------
TEST ( track_features_core, default_config )
{
  EXPECT_PLUGGABLE_IMPL(
    track_features_core,
    "Track features from frame to frame"
    " using feature detection, matching, and loop closure.",
    PARAM_DEFAULT(
      features_dir, kwiver::vital::config_path_t,
      "Path to a directory in which to read or write the feature "
      "detection and description files.\n"
      "Using this directory requires a feature_io algorithm.",
      "" ),
    PARAM(
      feature_detector, vital::algo::detect_features_sptr,
      "feature_detector" ),
    PARAM(
      descriptor_extractor, vital::algo::extract_descriptors_sptr,
      "descriptor_extractor" ),
    PARAM(
      feature_io, vital::algo::feature_descriptor_io_sptr,
      "feature_io" ),
    PARAM(
      feature_matcher, vital::algo::match_features_sptr,
      "feature_matcher" ),
    PARAM(
      loop_closer, vital::algo::close_loops_sptr,
      "loop_closer" )
  );
}

// ----------------------------------------------------------------------------
TEST ( track_features_augment_keyframes, default_config )
{
  EXPECT_PLUGGABLE_IMPL(
    track_features_augment_keyframes,
    "If the current frame is a keyframe, detect and describe "
    "additional features and create new tracks on this frame.",
    PARAM(
      extractor, vital::algo::extract_descriptors_sptr,
      "Extractor" ),
    PARAM_DEFAULT(
      extractor_name, std::string,
      "Extractor name",
      "kf_only_descriptor_extractor" )
  );
}
