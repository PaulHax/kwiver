// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/**
 * \file
 *
 * \brief Various functions for creating a set of features with attributes
 * for testing the filter_features implementations
 *
 * These functions are shared by various tests
 */

#ifndef KWIVER_TEST_TEST_FEATURES_H_
#define KWIVER_TEST_TEST_FEATURES_H_

#include <random>

#include <vital/types/feature.h>
#include <vital/types/feature_set.h>

namespace kwiver {

namespace testing {

// Generate a set of generic features
//
// parameters are:
// ..
// ..
// ..

// Generate a feature set with specified parameters
kwiver::vital::feature_set_sptr
generate_feature_set(
  size_t num_features = 500,
  double loc_min_x = 0.0,
  double loc_max_x = 100.0,
  double loc_min_y = 0.0,
  double loc_max_y = 100.0,
  double mag_min = 0.0,
  double mag_max = 1.0,
  double scale_min = 1.0,
  double scale_max = 1.1,
  double angle_min = 0.0,
  double angle_max = 180.0 )
{
  // Create an empty vector of features
  std::vector< kwiver::vital::feature_sptr > features;

  // Create random number generator
  std::default_random_engine rng( std::random_device{}() );
  std::uniform_real_distribution< double > loc_dist_x( loc_min_x, loc_max_x );
  std::uniform_real_distribution< double > loc_dist_y( loc_min_y, loc_max_y );
  std::uniform_real_distribution< double > mag_dist( mag_min, mag_max );
  std::uniform_real_distribution< double > scale_dist( scale_min, scale_max );
  std::uniform_real_distribution< double > angle_dist( angle_min, angle_max );

  // Generate random features and add them to the vector
  for( size_t i = 0; i < num_features; ++i )
  {
    // Generate random feature parameters
    double loc_x = loc_dist_x( rng );
    double loc_y = loc_dist_y( rng );
    double mag = mag_dist( rng );
    double scale = scale_dist( rng );
    double angle = angle_dist( rng );

    // Define feature parameters
    Eigen::Vector2d loc( loc_x, loc_y );  // Image coordinates
    double magnitude = mag;
    kwiver::vital::rgb_color color( 255, 0, 0 );  // Red color

    // Create a feature object
    auto feature = std::make_shared< kwiver::vital::feature_d >(
      loc, magnitude, scale, angle, color );

    // Add the feature to the vector
    features.push_back( feature );
  }

  // Create a simple_feature_set from the vector of features
  auto feature_set = std::make_shared< kwiver::vital::simple_feature_set >(
    features );

  return feature_set;
}

} // end namespace testing

} // end namespace kwiver

#endif
