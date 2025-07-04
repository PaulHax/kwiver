// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/**
 * \file
 *
 * \brief Google Test utilities for Eigen types
 *
 * This adds some utilities that improve working with Eigen types in Google
 * Test.
 */

#ifndef KWIVER_TEST_TEST_EIGEN_H_
#define KWIVER_TEST_TEST_EIGEN_H_

#include <Eigen/Core>

#include <gtest/gtest.h>

// ----------------------------------------------------------------------------
//
// Testing helper functions
//

namespace Eigen {

// ----------------------------------------------------------------------------
void
PrintTo( Vector2d const& v, ::std::ostream* os )
{
  // This function exists because a) it produces better formatting, and
  // b) Google Test needs an exact match or it will fall back to the generic
  // value printer...
  ( *os ) << v[ 0 ] << ", " << v[ 1 ];
}

// ----------------------------------------------------------------------------
void
PrintTo( Vector3d const& v, ::std::ostream* os )
{
  // This function exists because a) it produces better formatting, and
  // b) Google Test needs an exact match or it will fall back to the generic
  // value printer...
  ( *os ) << v[ 0 ] << ", " << v[ 1 ] << ", " << v[ 2 ];
}

// ----------------------------------------------------------------------------
template < typename T >
::std::ostream&
operator<<( ::std::ostream& os, Quaternion< T > const& v )
{
  os << v.x() << ", " << v.y() << ", " << v.z() << ", " << v.w();
  return os;
}

// ----------------------------------------------------------------------------
template < typename T >
bool
operator==( Quaternion< T > const& q1, Quaternion< T > const& q2 )
{
  return q1.coeffs() == q2.coeffs();
}

} // end namespace Eigen

namespace kwiver {

namespace testing {

// ----------------------------------------------------------------------------
struct matrix_comparator
{
  // --------------------------------------------------------------------------
  template < typename A, typename B >
  bool
  operator()( A const& m1, B const& m2 )
  {
    return m1.isApprox( m2 );
  }

  // --------------------------------------------------------------------------
  template < typename T, int M, int N >
  bool
  operator()(
    Eigen::Matrix< T, M, N > const& a,
    Eigen::Matrix< T, M, N > const& b,
    double epsilon )
  {
    for( unsigned i = 0; i < M; ++i )
    {
      for( unsigned j = 0; j < N; ++j )
      {
        if( std::abs( a( i, j ) - b( i, j ) ) > epsilon )
        {
          return false;
        }
      }
    }
    return true;
  }

  // --------------------------------------------------------------------------
  template < typename T >
  bool
  operator()(
    Eigen::Matrix< T, Eigen::Dynamic, 1 > const& a,
    Eigen::Matrix< T, Eigen::Dynamic, 1 > const& b,
    double epsilon )
  {
    if( a.size() != b.size() )
    {
      return false;
    }

    for( unsigned i = 0; i < a.size(); ++i )
    {
      if( std::abs( a[ i ] - b[ i ] ) > epsilon )
      {
        return false;
      }
    }
    return true;
  }

  // --------------------------------------------------------------------------
  template < typename T >
  bool
  operator()(
    Eigen::Quaternion< T > const& q1,
    Eigen::Quaternion< T > const& q2 )
  {
    return operator()( q1.coeffs(), q2.coeffs() );
  }
};

// ----------------------------------------------------------------------------
struct similar_matrix_comparator : matrix_comparator
{
  template < typename T, int M, int N >
  bool
  operator()(
    Eigen::Matrix< T, M, N > const& a,
    Eigen::Matrix< T, M, N > const& b,
    double epsilon )
  {
    if( a.cwiseProduct( b ).sum() < 0.0 )
    {
      return matrix_comparator::operator()( a, ( b * -1.0 ).eval(), epsilon );
    }
    return matrix_comparator::operator()( a, b, epsilon );
  }
};

// ----------------------------------------------------------------------------
static auto compare_matrices = matrix_comparator{};
static auto compare_similar_matrices = similar_matrix_comparator{};

#define EXPECT_MATRIX_EQ( a, b ) \
EXPECT_PRED2( ::kwiver::testing::compare_matrices, a, b )

#define EXPECT_MATRIX_NEAR( a, b, eps ) \
EXPECT_PRED3( ::kwiver::testing::compare_matrices, a, b, eps )

#define EXPECT_MATRIX_SIMILAR( a, b, eps ) \
EXPECT_PRED3( ::kwiver::testing::compare_similar_matrices, a, b, eps )

} // end namespace testing

} // end namespace kwiver

#endif
