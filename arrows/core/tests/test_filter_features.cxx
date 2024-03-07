// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <arrows/core/filter_features_magnitude.h>
#include <arrows/core/filter_features_nonmax.h>
#include <arrows/core/filter_features_scale.h>

#include <test_features.h>

#include <gtest/gtest.h>

#include <algorithm>

using namespace kwiver::vital;
using namespace kwiver::arrows::core;

// ----------------------------------------------------------------------------
// Establish constants and values for randomly generated track set

// Generate a collection of features with known paramaters
auto feature1 = std::make_shared< kwiver::vital::feature_d >(
  Eigen::Vector2d( 12.0, 9.0 ), 15.0, 27.0, 80.0 );

// Generate the feature set with default parameters
auto input_feature_set = kwiver::testing::generate_feature_set();

//
// ----------------------------------------------------------------------------
int
main( int argc, char** argv )
{
  // Generate a feature set with the provided parameters
  auto feature_set = kwiver::testing::generate_feature_set();

  // Access the first feature from the feature set (assuming it's not empty)
  auto features = feature_set->features();
  if( !features.empty() )
  {
    auto first_feature = features[ 0 ];

    // Access the magnitude attribute of the first feature
    double magnitude = first_feature->magnitude();

    // Print the magnitude attribute
    std::cout << "Magnitude of the first feature: " << magnitude << std::endl;
  }
  else
  {
    std::cerr << "Feature set is empty!" << std::endl;
  }

  // See if I can create and access a single feature - OK
  std::cout << "Feature 1's magnitude: " << feature1->magnitude() << '\n';

  std::vector< unsigned int > indices;

  // Generate instance of the filter_features_scale algorithm
  algo::filter_features_sptr scale_algo = create_algorithm< algo::filter_features >( "scale" );

  // Get the configuration of the filter_tracks algorithm
  config_block_sptr config = scale_algo->get_configuration();

  // Get the value of min_features from config block
  double min_features = config->get_value< double >( "min_features" );

  std::cout << "min features, should be 100, it is: " << min_features << '\n';

  // Run the filter
  auto filtered_set = scale_algo.filter( input_feature_set, indices );

  // Output the number of features before and after filtering
//    std::cout << "Number of features before filtering: " <<
// input_feature_set->size() << std::endl;
//    std::cout << "Number of features after filtering: " <<
// filtered_feature_set->size() << std::endl;

  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();

  return 0;
}

// ----------------------------------------------------------------------------
TEST ( filter_features_scale, create )
{
  using namespace kwiver::vital;

  plugin_manager::instance().load_all_plugins();

  EXPECT_NE( nullptr, create_algorithm< algo::filter_features >( "scale" ) );
}

// ----------------------------------------------------------------------------
TEST ( filter_features_magnitude, create )
{
  using namespace kwiver::vital;

  plugin_manager::instance().load_all_plugins();

  EXPECT_NE(
    nullptr,
    create_algorithm< algo::filter_features >( "magnitude" ) );
}

// ----------------------------------------------------------------------------
TEST ( filter_features_nonmax, create )
{
  using namespace kwiver::vital;

  plugin_manager::instance().load_all_plugins();

  EXPECT_NE( nullptr, create_algorithm< algo::filter_features >( "nonmax" ) );
}
