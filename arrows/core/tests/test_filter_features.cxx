// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <arrows/core/filter_features_magnitude.h>
#include <arrows/core/filter_features_nonmax.h>
#include <arrows/core/filter_features_scale.h>

#include <test_features.h>
#include <test_features_temp.h>

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

// ----------------------------------------------------------------------------
int
main( int argc, char** argv )
{
  plugin_manager::instance().load_all_plugins();

  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}

// ---------------------------------------------------------------------------
// Function to inspect the feature set
TEST ( filter_features_scale, feature_set )
{
  // Generate feature set with specified parameters
  kwiver::vital::feature_set_sptr feature_set =
    kwiver::testing::make_n_features< double >( 5 );

  std::cout << "Feature set values (angle is fraction of pi)" << "\n";

  // Iterate over the features in the set and print out the values
  for( const auto& f : feature_set->features() )
  {
    Eigen::Matrix< double, 2, 1 > loc = f->loc();
    double x = loc[ 0 ];
    double y = loc[ 1 ];

    std::cout <<
      "Location: " << "(" << x << ", " << y << ")" << ", " <<
      "Magnitude: " << f->magnitude() << ", " <<
      "Scale: " << f->scale() << ", " <<
      "Angle: " << f->angle() << "\n";
  }
}

// ----------------------------------------------------------------------------
TEST ( filter_features_scale, create )
{
  using namespace kwiver::vital;

  EXPECT_NE( nullptr, create_algorithm< algo::filter_features >( "scale" ) );
}

// ----------------------------------------------------------------------------
TEST ( filter_features_magnitude, create )
{
  using namespace kwiver::vital;

  EXPECT_NE(
    nullptr,
    create_algorithm< algo::filter_features >( "magnitude" ) );
}

// ----------------------------------------------------------------------------
TEST ( filter_features_nonmax, create )
{
  using namespace kwiver::vital;

  EXPECT_NE( nullptr, create_algorithm< algo::filter_features >( "nonmax" ) );
}

// ---------------------------------------------------------------------------
// Function to perform the filter action
TEST ( filter_features_scale, scale_algo )
{
  using namespace kwiver::vital;

  unsigned int num_features = 2000;

  // Generate feature set with specified parameters
  kwiver::vital::feature_set_sptr feature_set =
    kwiver::testing::make_n_features< double >( num_features );

  // de reference smart pointer
  std::vector< feature_sptr > features = feature_set->features();
  std::cout << "Filter set with: " << features.size() <<
    " features" << "\n";

  // Generate instance of the filter_features_scale algorithm
  algo::filter_features_sptr filter_algo = create_algorithm<
    algo::filter_features >( "scale" );

  // Get the configuration of the filter_tracks algorithm
  config_block_sptr config = filter_algo->get_configuration();

  // Display the available parameters
  std::cout << "Available parameters:" << std::endl;

  auto keys = config->available_values();
  for( auto key : keys )
  {
    std::cout << key << std::endl;
  }

  // Get default parameter values from config block
  int max_features = config->get_value< int >( "max_features" );
  int min_features = config->get_value< int >( "min_features" );
  double top_fraction = config->get_value< double >( "top_fraction" );

  std::cout << "num max features: " << max_features << '\n';
  std::cout << "top_fraction: " << top_fraction << "\n";

  // run the filter
  kwiver::vital::feature_set_sptr filtered_set =
    filter_algo->filter( feature_set );

  // de reference smart pointer
  std::vector< feature_sptr > filtered_features = filtered_set->features();
  std::cout << "Filtered set has: " << filtered_features.size() <<
    " features" << "\n";

  std::cout << "top_fraction * num_features " <<
    top_fraction * num_features << "\n";

  // check that number of featurs matches top_fraction value
  EXPECT_EQ( top_fraction * num_features, filtered_features.size() );

  // check that number of features is limited by min_features parameter
  kwiver::vital::feature_set_sptr min_set =
    kwiver::testing::make_n_features< double >( 200 );

  auto filtered_min_set = filter_algo->filter( min_set );

  std::cout << "filtered min set number: " <<
    filtered_min_set->features().size() << "\n";

  EXPECT_EQ( min_features, filtered_min_set->features().size() );

  // check that number of features is limited by max_features parameter
  feature_set_sptr max_set =
    kwiver::testing::make_n_features< double >( 5100 );

  auto filtered_max_set = filter_algo->filter( max_set );

  std::cout << "filtered max set number: " <<
    filtered_max_set->features().size() << "\n";

  EXPECT_EQ( max_features, filtered_max_set->features().size() );

  config->set_value( "min_features", 150 );
  config->set_value( "max_features", 900 );
  config->set_value( "top_fraction", 0.3 );

  double top_fraction_A = config->get_value< double >( "top_fraction" );
  std::cout << "top_fraction A: " << top_fraction_A << "\n";

  feature_set_sptr set_of_10 =
    kwiver::testing::make_10_features< double >();
  auto filter_10_to_3 = filter_algo->filter( max_set );

  std::cout << "filtered set of 10 to 3 number: " <<
    filter_10_to_3->features().size() << "\n";

//    EXPECT_EQ(4.5, filtered_max_set->features().size());
}

// ---------------------------------------------------------------------------
// Function to perform the filter action
TEST ( filter_features_scale, set_of_10 )
{
  using namespace kwiver::vital;

  // Generate instance of the filter_features_scale algorithm
  algo::filter_features_sptr filter_algo = create_algorithm<
    algo::filter_features >( "scale" );

  // Get the configuration of the filter_tracks algorithm
  config_block_sptr config = filter_algo->get_configuration();

  // Adjust filter to capture 3 features with known top scale values
  config->set_value( "min_features", 3 );
  config->set_value( "top_fraction", 0.3 );

  // Set the updated configuration to the filter algorithm
  filter_algo->set_configuration( config );

  feature_set_sptr set_of_10 =
    kwiver::testing::make_10_features< double >();

  auto filtered_3 = filter_algo->filter( set_of_10 );

  std::cout << "filtered set of 10 to 3 number: " <<
    filtered_3->features().size() << "\n";

  // Extract scale values from the filtered set
  std::vector< double > scale_values;
  for( const auto& f : filtered_3->features() )
  {
    scale_values.push_back( f->scale() );
  }

  // Calculate the sum of scale values
  double sum_of_scales = std::accumulate(
    scale_values.begin(),
    scale_values.end(), 0.0 );

  std::cout << "sum of scales is: " << sum_of_scales << "\n";

  // Perform the EXPECT_EQ test
  EXPECT_EQ( sum_of_scales, 5.5 );
}

/*
 *  //
 * ---------------------------------------------------------------------------
 *  // Function to perform the filter action
 *  TEST(filter_features_nonmax, nonmax_algo)
 *  {
 *  // Generate feature set with specified parameters
 *   kwiver::vital::feature_set_sptr input_feature_set =
 *     kwiver::testing::make_n_features<double>(500);
 *
 *
 *  // Generate instance of the filter_features_scale algorithm
 *  algo::filter_features_sptr filter_algo = create_algorithm<
 *  algo::filter_features >( "nonmax" );
 *
 *  // Get the configuration of the filter_tracks algorithm
 *  config_block_sptr config = filter_algo->get_configuration();
 *
 *  // Display the available parameters
 *  std::cout << "Available parameters:" << std::endl;
 *  auto keys = config->available_values();
 *   for (auto key : keys) {
 *       std::cout << key << std::endl;
 *   }
 *
 *  // Get the value of num_features_target from config block
 *  int num_feats_tar = config->get_value< int >( "num_features_target" );
 *
 *  // see if it worked - OK
 *  std::cout << "num_features_target, 500?: " << num_feats_tar << '\n';
 *
 *  // run the filter
 *  auto filtered_features = filter_algo->filter(input_feature_set);
 *
 *  }
 */
