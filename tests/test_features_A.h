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

#ifndef KWIVER_TEST_TEST_FEATURES_A_H_
#define KWIVER_TEST_TEST_FEATURES_A_H_

#include <random>

#include <vital/types/feature.h>
#include <vital/types/feature_set.h>

using namespace kwiver::vital;

namespace kwiver {

namespace testing {

// Generate a set of generic features
//
// parameters are:
// ..
// ..
// ..

template < typename T >
feature_set_sptr
make_n_features( size_t num_feat )
{
  std::vector< feature_sptr > feat;
  for( unsigned i = 0; i < num_feat; ++i )
  {
    T v = static_cast< T >( i ) / num_feat;
    auto f = std::make_shared< feature_< T > >();
    T x = v * 10, y = v * 15 + 5;
    f->set_loc( Eigen::Matrix< T, 2, 1 >( x, y ) );
    f->set_scale( 1.0 + v );
    f->set_magnitude( 1 - v );
    f->set_angle( v * 3.14159f );
    f->set_color(
      rgb_color(
        static_cast< uint8_t >( i ),
        static_cast< uint8_t >( i + 5 ),
        static_cast< uint8_t >( i + 10 ) ) );
    f->set_covar( covariance_< 2, T >( v ) );
    feat.push_back( f );
  }

  return std::make_shared< simple_feature_set >( feat );
}

} // end namespace testing

} // end namespace kwiver

#endif
