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

namespace {

// Number of features, used in multiple tests
int const num_features = 2000;

} // namespace

// ----------------------------------------------------------------------------
int
main( int argc, char** argv )
{
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}

// ----------------------------------------------------------------------------
TEST ( filter_features_scale, create )
{
  plugin_manager::instance().load_all_plugins();

  EXPECT_NE( nullptr, create_algorithm< algo::filter_features >( "scale" ) );
}

// ----------------------------------------------------------------------------
TEST ( filter_features_magnitude, create )
{
  plugin_manager::instance().load_all_plugins();

  EXPECT_NE(
    nullptr,
    create_algorithm< algo::filter_features >( "magnitude" ) );
}

// ----------------------------------------------------------------------------
TEST ( filter_features_nonmax, create )
{
  plugin_manager::instance().load_all_plugins();

  EXPECT_NE( nullptr, create_algorithm< algo::filter_features >( "nonmax" ) );
}

// ---------------------------------------------------------------------------
// Test checks configuration, and filter function with parameters;
// top_fraction, min_features and max_features
TEST ( filter_features_scale, filter )
{
  plugin_manager::instance().load_all_plugins();

  // Generate instance of the filter algorithm
  algo::filter_features_sptr filter_algo = create_algorithm<
    algo::filter_features >( "scale" );

  // Get the configuration of the algorithm
  config_block_sptr config = filter_algo->get_configuration();

  EXPECT_TRUE( filter_algo->check_configuration( config ) );

  // Get default parameter values from config block
  int max_features = config->get_value< int >( "max_features" );
  int min_features = config->get_value< int >( "min_features" );
  double top_fraction = config->get_value< double >( "top_fraction" );

  // Generate feature set with 2000 features
  feature_set_sptr feature_set =
    kwiver::testing::make_n_features< double >( num_features );

  // Run the filter function
  feature_set_sptr filtered_set = filter_algo->filter( feature_set );

  // Check that number of features matches top_fraction value
  // default is 0.2
  EXPECT_EQ( top_fraction * num_features, ( filtered_set->features() ).size() );

  // Check that number of features is limited by min_features parameter
  // default is 100
  feature_set_sptr min_set =
    kwiver::testing::make_n_features< double >( 200 );

  auto filtered_min_set = filter_algo->filter( min_set );

  EXPECT_EQ( min_features, ( filtered_min_set->features() ).size() );

  // check that number of features is limited by max_features parameter
  // default is 1000
  feature_set_sptr max_set =
    kwiver::testing::make_n_features< double >( 5200 );

  auto filtered_max_set = filter_algo->filter( max_set );

  EXPECT_EQ( max_features, ( filtered_max_set->features() ).size() );
}

// ---------------------------------------------------------------------------
// Test checks configuration, and filter function with parameters;
// top_fraction and min_features
TEST ( filter_features_magnitude, filter )
{
  plugin_manager::instance().load_all_plugins();

  // Generate instance of the filter algorithm
  algo::filter_features_sptr filter_algo = create_algorithm<
    algo::filter_features >( "magnitude" );

  // Get the configuration of the algorithm
  config_block_sptr config = filter_algo->get_configuration();

  // Check configuration
  EXPECT_TRUE( filter_algo->check_configuration( config ) );

  // Get default parameter values from config block
  int min_features = config->get_value< int >( "min_features" );
  double top_fraction = config->get_value< double >( "top_fraction" );

  feature_set_sptr feature_set =
    kwiver::testing::make_n_features< double >( num_features );

  // Run filter function
  feature_set_sptr filtered_set = filter_algo->filter( feature_set );

  // Check that number of features matches top_fraction value
  // default is 0.2
  EXPECT_EQ( top_fraction * num_features, ( filtered_set->features() ).size() );

  // Check that number of features is limited by min_features parameter
  // default is 100
  feature_set_sptr min_set =
    kwiver::testing::make_n_features< double >( 200 );

  auto filtered_min_set = filter_algo->filter( min_set );

  EXPECT_EQ( min_features, ( filtered_min_set->features() ).size() );
}

// ---------------------------------------------------------------------------
// Test configuration, and filter operation with parameters;
// num_features_target and num_features_range
TEST ( filter_features_nonmax, filter )
{
  plugin_manager::instance().load_all_plugins();

  // Generate instance of the filter algorithm
  algo::filter_features_sptr filter_algo = create_algorithm<
    algo::filter_features >( "nonmax" );

  // Get the configuration of the filter_tracks algorithm
  config_block_sptr config = filter_algo->get_configuration();

  // Check configuration
  EXPECT_TRUE( filter_algo->check_configuration( config ) );

  // Get default parameter values from config block
  int num_features_target = config->get_value< int >( "num_features_target" );
  int num_features_range = config->get_value< int >( "num_features_range" );

  // Generate feature set with 2000 features
  feature_set_sptr feature_set =
    kwiver::testing::make_n_features< double >( num_features );

  // run the filter
  feature_set_sptr filtered_set = filter_algo->filter( feature_set );

  // Get the size of the filtered set of features
  size_t filtered_size = ( filtered_set->features() ).size();

  // Check if the size is between 500 and 550 default values
  EXPECT_GE( filtered_size, num_features_target );
  EXPECT_LE( filtered_size, num_features_target + num_features_range );
}

// ---------------------------------------------------------------------------
// Tests configuration values can be modified and
// filters 3 out of 10 features with known scale values:
// {1.0, 2.0, 1.8, 1.2, 1.1, 1.3, 1.7, 1.2, 1.1, 1.1 };

TEST ( filter_features_scale, filter_10_features )
{
  plugin_manager::instance().load_all_plugins();

  // Generate instance of the filter_features_scale algorithm
  algo::filter_features_sptr filter_algo = create_algorithm<
    algo::filter_features >( "scale" );

  // Get the configuration of the filter_tracks algorithm
  config_block_sptr config = filter_algo->get_configuration();

  // Adjust config to filter 3 from 10 features
  config->set_value( "min_features", 3 );
  config->set_value( "top_fraction", 0.3 );

  // Set the updated configuration to the filter algorithm
  filter_algo->set_configuration( config );

  feature_set_sptr set_of_10 =
    kwiver::testing::make_10_features< double >();

  auto filtered_3 = filter_algo->filter( set_of_10 );

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

  // Sum of top three scale values should be 5.5
  EXPECT_EQ( sum_of_scales, 5.5 );
}

// ---------------------------------------------------------------------------
// Tests configuration values can be modified
// Filters 3 out of 10 features with known magnitude values:
//  { 0.7, 0.1, 0.1, 0.2, 0.3, 0.5, 0.8, 0.5, 0.9, 0.1 };

TEST ( filter_features_magnitude, filter_10_features )
{
  plugin_manager::instance().load_all_plugins();

  // Generate instance of the filter_features_scale algorithm
  algo::filter_features_sptr filter_algo = create_algorithm<
    algo::filter_features >( "magnitude" );

  // Get the configuration of the filter_tracks algorithm
  config_block_sptr config = filter_algo->get_configuration();

  // Adjust config to filter 3 from 10 features
  config->set_value( "min_features", 3 );
  config->set_value( "top_fraction", 0.3 );

  // Set the updated configuration to the filter algorithm
  filter_algo->set_configuration( config );

  feature_set_sptr set_of_10 =
    kwiver::testing::make_10_features< double >();

  auto filtered_3 = filter_algo->filter( set_of_10 );

  // Extract magnitude values from the filtered set
  std::vector< double > mag_values;
  for( const auto& f : filtered_3->features() )
  {
    mag_values.push_back( f->magnitude() );
  }

  // Calculate the sum of magnitude values
  double sum_of_magnitudes = std::accumulate(
    mag_values.begin(),
    mag_values.end(), 0.0 );

  // Sum of top three scale values should be 2.4
  EXPECT_EQ( sum_of_magnitudes, 2.4 );
}

// ---------------------------------------------------------------------------
// Test changing config and filtering with to different values
TEST ( filter_features_nonmax, change_config )
{
  plugin_manager::instance().load_all_plugins();

  // Generate instance of the filter algorithm
  algo::filter_features_sptr filter_algo = create_algorithm<
    algo::filter_features >( "nonmax" );

  // Get the configuration
  config_block_sptr config = filter_algo->get_configuration();

  // Adjust config to different values from default
  config->set_value< int >( "num_features_target", 200 );
  config->set_value< int >( "num_features_range", 20 );

  // Set the updated configuration to the filter algorithm
  filter_algo->set_configuration( config );

  // Generate feature set with 2000 features
  feature_set_sptr feature_set =
    kwiver::testing::make_n_features< double >( num_features );

  // Run the filter function
  feature_set_sptr filtered_set = filter_algo->filter( feature_set );

  // Get the size of the filtered set of features
  size_t filtered_size = ( filtered_set->features() ).size();

  // Check if the size is between 200 and 220 default values
  EXPECT_GE( filtered_size, 200 );
  EXPECT_LE( filtered_size, 220 );
}
