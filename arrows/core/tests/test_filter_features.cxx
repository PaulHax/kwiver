// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <arrows/core/filter_features_magnitude.h>
#include <arrows/core/filter_features_nonmax.h>
#include <arrows/core/filter_features_scale.h>

#include <test_features.h>
#include <test_features_A.h>

#include <gtest/gtest.h>

#include <algorithm>

using namespace kwiver::vital;
using namespace kwiver::arrows::core;

// ----------------------------------------------------------------------------
// Establish constants and values for randomly generated track set

// call the fn to generate features
void gen_features_fn();
void gen_feature_set_A();

// Fn to generate a single feature
void create_feature();

// Function to perform the filter action
void fnonmax_algo();

// Generate a single feature with known paramaters
auto feature1 = std::make_shared< kwiver::vital::feature_d >(
  Eigen::Vector2d( 12.0, 9.0 ), 15.0, 27.0, 80.0 );

// ---------------------------------------------
// function to call the feature generation

void
gen_features_fn()
{
  std::cout << "generate a set of default features " << "\n";

  // Generate a feature set with the default parameters
  feature_set_sptr feature_set =
    kwiver::testing::generate_feature_set();

  // Access the first feature and print magnitude
  std::vector< feature_sptr > features = feature_set->features();
  auto first_feature = features[ 0 ];
  double magnitude = first_feature->magnitude();
  std::cout << "1st feature magnitude: " << magnitude << std::endl;
  std::cout << "1st feature scale: " << first_feature->scale() << "\n";
}

// ------------------------------------------------------------------------
// function to call the feature generation ver A
void
gen_feature_set_A()
{
  std::cout << "generate feature set ver A " << "\n";

  // Generate a feature set with the default parameters
  kwiver::vital::feature_set_sptr feature_set =
    kwiver::testing::make_n_features< double >( 10 );

  // Access the first feature and print magnitude
  std::vector< feature_sptr > features = feature_set->features();
  auto first_feature = features[ 0 ];
  double magnitude = first_feature->magnitude();
  std::cout << "1st feature magnitude: " << magnitude << std::endl;
  std::cout << "1st feature scale: " << first_feature->scale() << "\n";
}

// -----------------------------------------------------------------
// function to call the feature generation
void
create_feature()
{
  // Define feature parameters
  Eigen::Vector2d loc( 100.0, 200.0 );  // Image coordinates
  double magnitude = 0.5;
  double scale = 1.2;
  double angle = 45.0;    // Degrees
  rgb_color color( 255, 0, 0 );  // Red color

  // Create a feature object
  feature_d my_feature( loc, magnitude, scale, angle, color );

  // Access feature attributes
  Eigen::Vector2d feature_loc = my_feature.loc();
  double feature_magnitude = my_feature.magnitude();
  double feature_scale = my_feature.scale();
  double feature_angle = my_feature.angle();
  rgb_color feature_color = my_feature.color();

  // Output feature attributes
  std::cout << "Feature Location: " << feature_loc.transpose() << std::endl;
  std::cout << "Feature Magnitude: " << feature_magnitude << std::endl;
  std::cout << "Feature Scale: " << feature_scale << std::endl;
  std::cout << "Feature Angle: " << feature_angle << " degrees" << std::endl;
  std::cout << "Feature Color: (" << feature_color.r << ", " <<
    feature_color.g << ", " << feature_color.b << ")" << std::endl;
}

// ---------------------------------------------------------------------------
// Function to perform the filter action
TEST ( filter_features_scale, nonmax_algo )
{
  // plugin_manager::instance().load_all_plugins();

//  create_feature();

//  gen_features_fn();
// gen_feature_set_A();

  // Generate feature set with specified parameters
  //   kwiver::vital::feature_set_sptr input_feature_set =
  //   kwiver::testing::generate_feature_set();

  plugin_manager::instance().load_all_plugins();

  // Generate instance of the filter_features_scale algorithm
  algo::filter_features_sptr filter_algo = create_algorithm<
    algo::filter_features >( "nonmax" );

  // Get the configuration of the filter_tracks algorithm
  config_block_sptr config = filter_algo->get_configuration();

  // Display the available parameters
  std::cout << "Available parameters:" << std::endl;

  auto keys = config->available_values();
  for( auto key : keys )
  {
    std::cout << key << std::endl;
  }

  // Get the value of num_features_target from config block
  int num_feats_tar = config->get_value< int >( "num_features_target" );

  // see if it worked - OK
  std::cout << "num_features_target, 500?: " << num_feats_tar << '\n';

  // run the filter
//  auto filtered_features = filter_algo->filter(input_feature_set);

//  std::cout << "number of filtered features: " <<
//   filtered_features->size() << '\n';
}

// ----------------------------------------------------------------------------
int
main( int argc, char** argv )
{
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

// Possibly useful code for later

/*
 *  // Iterate over the features in the set
 *  for (const auto& feature : feature_set->features())
 *   {
 *     // Access the magnitude value of the feature and print it
 *     double magnitude = feature->magnitude();
 *     std::cout << "Magnitude: " << magnitude << std::endl;
 *   }
 *
 *
 *  // See if I can create and access a single feature - OK
 *  std::cout << "Feature 1's magnitude: " << feature1->magnitude() << '\n';
 */
