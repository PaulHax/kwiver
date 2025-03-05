// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief core polygon class tests

#include <vital/types/polygon.h>

#include <gtest/gtest.h>

using namespace kwiver::vital;

// ----------------------------------------------------------------------------
int
main( int argc, char** argv )
{
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}

namespace {

static const polygon::point_t p1{ 10, 10 };
static const polygon::point_t p2{ 10, 50 };
static const polygon::point_t p3{ 50, 50 };
static const polygon::point_t p4{ 30, 30 };

} // namespace

// ----------------------------------------------------------------------------
TEST ( polygon, default_constructor )
{
  polygon p;
  EXPECT_EQ( 0, p.num_vertices() );
}

// ----------------------------------------------------------------------------
TEST ( polygon, construct_from_vector )
{
  std::vector< polygon::point_t > vec;

  vec.push_back( p1 );
  vec.push_back( p2 );
  vec.push_back( p3 );
  vec.push_back( p4 );

  polygon p( vec );
  EXPECT_EQ( 4, p.num_vertices() );
}

// ----------------------------------------------------------------------------
TEST ( polygon, add_points )
{
  polygon p;

  p.push_back( p1 );
  ASSERT_EQ( 1, p.num_vertices() );

  p.push_back( p2 );
  ASSERT_EQ( 2, p.num_vertices() );

  p.push_back( p3 );
  ASSERT_EQ( 3, p.num_vertices() );

  p.push_back( p4 );
  ASSERT_EQ( 4, p.num_vertices() );

  EXPECT_EQ( p1, p.at( 0 ) );
  EXPECT_EQ( p2, p.at( 1 ) );
  EXPECT_EQ( p3, p.at( 2 ) );
  EXPECT_EQ( p4, p.at( 3 ) );
}

// ----------------------------------------------------------------------------
TEST ( polygon, contains )
{
  polygon p;

  p.push_back( p1 );
  p.push_back( p2 );
  p.push_back( p3 );
  p.push_back( p4 );

  EXPECT_TRUE( p.contains( 30, 30 ) );
  EXPECT_FALSE( p.contains( 70, 70 ) );
}

// ----------------------------------------------------------------------------
TEST ( polygon, get_vertices )
{
  polygon p;

  p.push_back( p1 );
  p.push_back( p2 );
  p.push_back( p3 );
  p.push_back( p4 );

  auto vec = p.get_vertices();

  ASSERT_EQ( 4, vec.size() );
  EXPECT_EQ( p1, vec.at( 0 ) );
  EXPECT_EQ( p2, vec.at( 1 ) );
  EXPECT_EQ( p3, vec.at( 2 ) );
  EXPECT_EQ( p4, vec.at( 3 ) );
}

// ----------------------------------------------------------------------------
TEST ( polygon, area_degenerate )
{
  polygon p;

  // Empty
  ASSERT_DOUBLE_EQ( 0.0, p.area() );

  // One point
  p = { { 1.0, 1.0 } };
  ASSERT_DOUBLE_EQ( 0.0, p.area() );

  // Two points
  p = { { 1.0, 1.0 }, { 2.0, 2.0 } };
  ASSERT_DOUBLE_EQ( 0.0, p.area() );

  // Colinear points
  p = { { 1.0, 1.0 }, { 2.0, 2.0 }, { 3.0, 3.0 } };
  ASSERT_DOUBLE_EQ( 0.0, p.area() );

  // Traces back on itself
  p = {
    { -1.0, -1.0 },
    { -0.5, -1.0 },
    { -0.5, +3.0 },
    { -0.5, -1.0 } };
  ASSERT_DOUBLE_EQ( 0.0, p.area() );
}

// ----------------------------------------------------------------------------
TEST ( polygon, area )
{
  polygon p;

  // Simple triangle
  p = { { 1.0, 1.0 }, { 2.0, 2.0 }, { 1.0, 3.0 } };
  ASSERT_DOUBLE_EQ( 1.0, p.area() );

  // Redundant points
  p = {
    { 1.0, 1.0 },
    { 1.5, 1.5 },
    { 2.0, 2.0 },
    { 2.0, 2.0 },
    { 1.0, 3.0 },
    { 1.0, 1.0 } };
  ASSERT_DOUBLE_EQ( 1.0, p.area() );

  // Large numbers
  p = {
    { 100000000000001.0, 100000000000001.0 },
    { 100000000000002.0, 100000000000002.0 },
    { 100000000000001.0, 100000000000003.0 } };
  ASSERT_DOUBLE_EQ( 1.0, p.area() );

  // Concave in the y direction
  p = {
    { +1.0, +1.0 },
    { +2.0, +2.0 },
    { +1.5, +2.5 },
    { +1.0, +2.0 },
    { -1.0, +3.0 },
    { -3.0, +2.0 } };
  ASSERT_DOUBLE_EQ( 4.75, p.area() );

  // Concave in the x direction
  p = {
    { +2.0, -3.0 },
    { +3.0, -1.0 },
    { +2.0, +1.0 },
    { +2.5, +1.5 },
    { +2.0, +2.0 },
    { +1.0, +1.0 } };
  ASSERT_DOUBLE_EQ( 4.75, p.area() );

  // Plus sign
  p = {
    { -1.0, +0.5 },
    { -1.0, -0.5 },
    { -0.5, -0.5 },
    { -0.5, -1.0 },
    { +0.5, -1.0 },
    { +0.5, -0.5 },
    { +1.0, -0.5 },
    { +1.0, +0.5 },
    { +0.5, +0.5 },
    { +0.5, +1.0 },
    { -0.5, +1.0 },
    { -0.5, +0.5 } };
  ASSERT_DOUBLE_EQ( 3.0, p.area() );
}

// ----------------------------------------------------------------------------
TEST ( polygon, negative_area )
{
  // Clockwise plus sign
  polygon p = {
    { -0.5, +0.5 },
    { -0.5, +1.0 },
    { +0.5, +1.0 },
    { +0.5, +0.5 },
    { +1.0, +0.5 },
    { +1.0, -0.5 },
    { +0.5, -0.5 },
    { +0.5, -1.0 },
    { -0.5, -1.0 },
    { -0.5, -0.5 },
    { -1.0, -0.5 },
    { -1.0, +0.5 } };
  ASSERT_DOUBLE_EQ( -3.0, p.area() );
}
