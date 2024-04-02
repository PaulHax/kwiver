// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <test_scene.h>

#include <arrows/mvg/algo/triangulate_landmarks.h>
#include <arrows/tests/test_triangulate_landmarks.h>

#include <vital/algo/algorithm.txx>
#include <vital/plugin_management/pluggable_macro_testing.h>
#include <vital/plugin_management/plugin_manager.h>

using namespace kwiver::vital;
using namespace kwiver::arrows::mvg;

// ----------------------------------------------------------------------------
int
main( int argc, char** argv )
{
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}

// ----------------------------------------------------------------------------
TEST ( triangulate_landmarks, create )
{
  using namespace kwiver::vital;

  plugin_manager::instance().load_all_plugins();

  EXPECT_NE(
    nullptr,
    kwiver::vital::create_algorithm< kwiver::vital::algo::triangulate_landmarks >
      ( "mvg" ) );
}

// ----------------------------------------------------------------------------
TEST ( triangulate_landmarks, default_config )
{
  EXPECT_PLUGGABLE_IMPL(
    triangulate_landmarks,
    "Triangulate landmarks from tracks and cameras"
    " using a simple least squares solver.",
    PARAM_DEFAULT(
      homogeneous, bool,
      "Use the homogeneous method for triangulating points. "
      "The homogeneous method can triangulate points at or near "
      "infinity and discard them.", false ),

    PARAM_DEFAULT(
      ransac, bool,
      "Use RANSAC in triangulating the points", true ),

    PARAM_DEFAULT(
      min_angle_deg, float,
      "minimum angle required to triangulate a point.", 1.0f ),

    PARAM_DEFAULT(
      inlier_threshold_pixels, float,
      "reprojection error threshold in pixels.", 2.0f ),

    PARAM_DEFAULT(
      frac_track_inliers_to_keep_triangulated_point, float,
      "fraction of measurements in track that must be inliers to "
      "keep the triangulated point", 0.5f ),

    PARAM_DEFAULT(
      max_ransac_samples, int,
      "maximum number of samples to take in RANSAC triangulation", 20 ),

    PARAM_DEFAULT(
      conf_thresh, double,
      "RANSAC sampling terminates when this confidences in the "
      "solution is reached.", 0.99 )
  )
}

// ----------------------------------------------------------------------------
// Input to triangulation is the ideal solution, make sure it doesn't diverge
TEST ( triangulate_landmarks, from_solution )
{
  kwiver::arrows::mvg::triangulate_landmarks tri_lm;
  kwiver::vital::config_block_sptr cfg = tri_lm.get_configuration();
  cfg->set_value( "homogeneous", "false" );
  cfg->set_value( "ransac", "false" );
  tri_lm.set_configuration( cfg );
  kwiver::testing::test_from_solution( tri_lm );
}

// ----------------------------------------------------------------------------
// Input to triangulation is the ideal solution, make sure it doesn't diverge
TEST ( triangulate_landmarks, from_solution_homog )
{
  kwiver::arrows::mvg::triangulate_landmarks tri_lm;
  kwiver::vital::config_block_sptr cfg = tri_lm.get_configuration();
  cfg->set_value( "homogeneous", "true" );
  cfg->set_value( "ransac", "false" );
  tri_lm.set_configuration( cfg );
  kwiver::testing::test_from_solution( tri_lm );
}

// ----------------------------------------------------------------------------
// Input to triangulation is the ideal solution, make sure it doesn't diverge
TEST ( triangulate_landmarks, from_solution_ransac )
{
  kwiver::arrows::mvg::triangulate_landmarks tri_lm;
  kwiver::vital::config_block_sptr cfg = tri_lm.get_configuration();
  cfg->set_value( "homogeneous", "false" );
  cfg->set_value( "ransac", "true" );
  tri_lm.set_configuration( cfg );
  kwiver::testing::test_from_solution( tri_lm );
}

// ----------------------------------------------------------------------------
// Input to triangulation is the ideal solution, make sure it doesn't diverge
TEST ( triangulate_landmarks, from_solution_homog_ransac )
{
  kwiver::arrows::mvg::triangulate_landmarks tri_lm;
  kwiver::vital::config_block_sptr cfg = tri_lm.get_configuration();
  cfg->set_value( "homogeneous", "true" );
  cfg->set_value( "ransac", "true" );
  tri_lm.set_configuration( cfg );
  kwiver::testing::test_from_solution( tri_lm );
}

// ----------------------------------------------------------------------------
// Add noise to landmarks before input to triangulation
TEST ( triangulate_landmarks, noisy_landmarks )
{
  kwiver::arrows::mvg::triangulate_landmarks tri_lm;
  kwiver::vital::config_block_sptr cfg = tri_lm.get_configuration();
  cfg->set_value( "homogeneous", "false" );
  cfg->set_value( "ransac", "false" );
  tri_lm.set_configuration( cfg );
  kwiver::testing::test_noisy_landmarks( tri_lm );
}

// ----------------------------------------------------------------------------
// Add noise to landmarks before input to triangulation
TEST ( triangulate_landmarks, noisy_landmarks_ransac )
{
  kwiver::arrows::mvg::triangulate_landmarks tri_lm;
  kwiver::vital::config_block_sptr cfg = tri_lm.get_configuration();
  cfg->set_value( "homogeneous", "false" );
  cfg->set_value( "ransac", "true" );
  tri_lm.set_configuration( cfg );
  kwiver::testing::test_noisy_landmarks( tri_lm );
}

// ----------------------------------------------------------------------------
// Add noise to landmarks before input to triangulation
TEST ( triangulate_landmarks, noisy_landmarks_homog )
{
  kwiver::arrows::mvg::triangulate_landmarks tri_lm;
  kwiver::vital::config_block_sptr cfg = tri_lm.get_configuration();
  cfg->set_value( "homogeneous", "true" );
  cfg->set_value( "ransac", "false" );
  tri_lm.set_configuration( cfg );
  kwiver::testing::test_noisy_landmarks( tri_lm );
}

// ----------------------------------------------------------------------------
// Add noise to landmarks before input to triangulation
TEST ( triangulate_landmarks, noisy_landmarks_homog_ransac )
{
  kwiver::arrows::mvg::triangulate_landmarks tri_lm;
  kwiver::vital::config_block_sptr cfg = tri_lm.get_configuration();
  cfg->set_value( "homogeneous", "true" );
  cfg->set_value( "ransac", "true" );
  tri_lm.set_configuration( cfg );
  kwiver::testing::test_noisy_landmarks( tri_lm );
}

// ----------------------------------------------------------------------------
// Initialize all landmarks to the origin as input to triangulation
TEST ( triangulate_landmarks, zero_landmarks )
{
  kwiver::arrows::mvg::triangulate_landmarks tri_lm;
  kwiver::vital::config_block_sptr cfg = tri_lm.get_configuration();
  cfg->set_value( "homogeneous", "false" );
  cfg->set_value( "ransac", "false" );
  tri_lm.set_configuration( cfg );
  kwiver::testing::test_zero_landmarks( tri_lm );
}

// ----------------------------------------------------------------------------
// Initialize all landmarks to the origin as input to triangulation
TEST ( triangulate_landmarks, zero_landmarks_homog )
{
  kwiver::arrows::mvg::triangulate_landmarks tri_lm;
  kwiver::vital::config_block_sptr cfg = tri_lm.get_configuration();
  cfg->set_value( "homogeneous", "true" );
  cfg->set_value( "ransac", "false" );
  tri_lm.set_configuration( cfg );
  kwiver::testing::test_zero_landmarks( tri_lm );
}

// ----------------------------------------------------------------------------
// Initialize all landmarks to the origin as input to triangulation
TEST ( triangulate_landmarks, zero_landmarks_ransac )
{
  kwiver::arrows::mvg::triangulate_landmarks tri_lm;
  kwiver::vital::config_block_sptr cfg = tri_lm.get_configuration();
  cfg->set_value( "homogeneous", "false" );
  cfg->set_value( "ransac", "true" );
  tri_lm.set_configuration( cfg );
  kwiver::testing::test_zero_landmarks( tri_lm );
}

// ----------------------------------------------------------------------------
// Initialize all landmarks to the origin as input to triangulation
TEST ( triangulate_landmarks, zero_landmarks_homog_ransac )
{
  kwiver::arrows::mvg::triangulate_landmarks tri_lm;
  kwiver::vital::config_block_sptr cfg = tri_lm.get_configuration();
  cfg->set_value( "homogeneous", "true" );
  cfg->set_value( "ransac", "true" );
  tri_lm.set_configuration( cfg );
  kwiver::testing::test_zero_landmarks( tri_lm );
}

// ----------------------------------------------------------------------------
// Select a subset of cameras to triangulation from
TEST ( triangulate_landmarks, subset_cameras )
{
  kwiver::arrows::mvg::triangulate_landmarks tri_lm;
  kwiver::vital::config_block_sptr cfg = tri_lm.get_configuration();
  cfg->set_value( "homogeneous", "false" );
  cfg->set_value( "ransac", "false" );
  tri_lm.set_configuration( cfg );
  kwiver::testing::test_subset_cameras( tri_lm );
}

// ----------------------------------------------------------------------------
// Select a subset of cameras to triangulation from
TEST ( triangulate_landmarks, subset_cameras_homog )
{
  kwiver::arrows::mvg::triangulate_landmarks tri_lm;
  kwiver::vital::config_block_sptr cfg = tri_lm.get_configuration();
  cfg->set_value( "homogeneous", "true" );
  cfg->set_value( "ransac", "false" );
  tri_lm.set_configuration( cfg );
  kwiver::testing::test_subset_cameras( tri_lm );
}

// ----------------------------------------------------------------------------
// Select a subset of cameras to triangulation from
TEST ( triangulate_landmarks, subset_cameras_ransac )
{
  kwiver::arrows::mvg::triangulate_landmarks tri_lm;
  kwiver::vital::config_block_sptr cfg = tri_lm.get_configuration();
  cfg->set_value( "homogeneous", "false" );
  cfg->set_value( "ransac", "true" );
  tri_lm.set_configuration( cfg );
  kwiver::testing::test_subset_cameras( tri_lm );
}

// ----------------------------------------------------------------------------
// Select a subset of cameras to triangulation from
TEST ( triangulate_landmarks, subset_cameras_homog_ransac )
{
  kwiver::arrows::mvg::triangulate_landmarks tri_lm;
  kwiver::vital::config_block_sptr cfg = tri_lm.get_configuration();
  cfg->set_value( "homogeneous", "true" );
  cfg->set_value( "ransac", "true" );
  tri_lm.set_configuration( cfg );
  kwiver::testing::test_subset_cameras( tri_lm );
}

// ----------------------------------------------------------------------------
// Select a subset of landmarks to triangulate
TEST ( triangulate_landmarks, subset_landmarks )
{
  kwiver::arrows::mvg::triangulate_landmarks tri_lm;
  kwiver::vital::config_block_sptr cfg = tri_lm.get_configuration();
  cfg->set_value( "homogeneous", "false" );
  cfg->set_value( "ransac", "false" );
  tri_lm.set_configuration( cfg );
  kwiver::testing::test_subset_landmarks( tri_lm );
}

// ----------------------------------------------------------------------------
// Select a subset of landmarks to triangulate
TEST ( triangulate_landmarks, subset_landmarks_homog )
{
  kwiver::arrows::mvg::triangulate_landmarks tri_lm;
  kwiver::vital::config_block_sptr cfg = tri_lm.get_configuration();
  cfg->set_value( "homogeneous", "true" );
  cfg->set_value( "ransac", "false" );
  tri_lm.set_configuration( cfg );
  kwiver::testing::test_subset_landmarks( tri_lm );
}

// ----------------------------------------------------------------------------
// Select a subset of landmarks to triangulate
TEST ( triangulate_landmarks, subset_landmarks_ransac )
{
  kwiver::arrows::mvg::triangulate_landmarks tri_lm;
  kwiver::vital::config_block_sptr cfg = tri_lm.get_configuration();
  cfg->set_value( "homogeneous", "false" );
  cfg->set_value( "ransac", "true" );
  tri_lm.set_configuration( cfg );
  kwiver::testing::test_subset_landmarks( tri_lm );
}

// ----------------------------------------------------------------------------
// Select a subset of landmarks to triangulate
TEST ( triangulate_landmarks, subset_landmarks_homog_ransac )
{
  kwiver::arrows::mvg::triangulate_landmarks tri_lm;
  kwiver::vital::config_block_sptr cfg = tri_lm.get_configuration();
  cfg->set_value( "homogeneous", "true" );
  cfg->set_value( "ransac", "true" );
  tri_lm.set_configuration( cfg );
  kwiver::testing::test_subset_landmarks( tri_lm );
}

// ----------------------------------------------------------------------------
// Select a subset of tracks/track_states to constrain the problem
TEST ( triangulate_landmarks, subset_tracks )
{
  kwiver::arrows::mvg::triangulate_landmarks tri_lm;
  kwiver::vital::config_block_sptr cfg = tri_lm.get_configuration();
  cfg->set_value( "homogeneous", "false" );
  cfg->set_value( "ransac", "false" );
  tri_lm.set_configuration( cfg );
  kwiver::testing::test_subset_tracks( tri_lm );
}

// ----------------------------------------------------------------------------
// Select a subset of tracks/track_states to constrain the problem
TEST ( triangulate_landmarks, subset_tracks_homog )
{
  kwiver::arrows::mvg::triangulate_landmarks tri_lm;
  kwiver::vital::config_block_sptr cfg = tri_lm.get_configuration();
  cfg->set_value( "homogeneous", "true" );
  cfg->set_value( "ransac", "false" );
  tri_lm.set_configuration( cfg );
  kwiver::testing::test_subset_tracks( tri_lm );
}

// ----------------------------------------------------------------------------
// Select a subset of tracks/track_states to constrain the problem
TEST ( triangulate_landmarks, subset_tracks_ransac )
{
  kwiver::arrows::mvg::triangulate_landmarks tri_lm;
  kwiver::vital::config_block_sptr cfg = tri_lm.get_configuration();
  cfg->set_value( "homogeneous", "false" );
  cfg->set_value( "ransac", "true" );
  tri_lm.set_configuration( cfg );
  kwiver::testing::test_subset_tracks( tri_lm );
}

// ----------------------------------------------------------------------------
// Select a subset of tracks/track_states to constrain the problem
TEST ( triangulate_landmarks, subset_tracks_homog_ransac )
{
  kwiver::arrows::mvg::triangulate_landmarks tri_lm;
  kwiver::vital::config_block_sptr cfg = tri_lm.get_configuration();
  cfg->set_value( "homogeneous", "true" );
  cfg->set_value( "ransac", "true" );
  tri_lm.set_configuration( cfg );
  kwiver::testing::test_subset_tracks( tri_lm );
}

// ----------------------------------------------------------------------------
// Select a subset of tracks/track_states and add noise
TEST ( triangulate_landmarks, noisy_tracks )
{
  kwiver::arrows::mvg::triangulate_landmarks tri_lm;
  kwiver::vital::config_block_sptr cfg = tri_lm.get_configuration();
  cfg->set_value( "homogeneous", "false" );
  cfg->set_value( "ransac", "false" );
  tri_lm.set_configuration( cfg );
  kwiver::testing::test_noisy_tracks( tri_lm );
}

// ----------------------------------------------------------------------------
// Select a subset of tracks/track_states and add noise
TEST ( triangulate_landmarks, noisy_tracks_homog )
{
  kwiver::arrows::mvg::triangulate_landmarks tri_lm;
  kwiver::vital::config_block_sptr cfg = tri_lm.get_configuration();
  cfg->set_value( "homogeneous", "true" );
  cfg->set_value( "ransac", "false" );
  tri_lm.set_configuration( cfg );
  kwiver::testing::test_noisy_tracks( tri_lm );
}

// ----------------------------------------------------------------------------
// Select a subset of tracks/track_states and add noise
TEST ( triangulate_landmarks, noisy_tracks_ransac )
{
  kwiver::arrows::mvg::triangulate_landmarks tri_lm;
  kwiver::vital::config_block_sptr cfg = tri_lm.get_configuration();
  cfg->set_value( "homogeneous", "false" );
  cfg->set_value( "ransac", "true" );
  tri_lm.set_configuration( cfg );
  kwiver::testing::test_noisy_tracks( tri_lm );
}

// ----------------------------------------------------------------------------
// Select a subset of tracks/track_states and add noise
TEST ( triangulate_landmarks, noisy_tracks_homog_ransac )
{
  kwiver::arrows::mvg::triangulate_landmarks tri_lm;
  kwiver::vital::config_block_sptr cfg = tri_lm.get_configuration();
  cfg->set_value( "homogeneous", "true" );
  cfg->set_value( "ransac", "true" );
  tri_lm.set_configuration( cfg );
  kwiver::testing::test_noisy_tracks( tri_lm );
}
