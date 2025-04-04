// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief core polygon class tests

#include <vital/types/polygon.h>

#include <test_gtest.h>

#include <iomanip>
#include <sstream>

using namespace kwiver::vital;

// ----------------------------------------------------------------------------
int
main( int argc, char** argv )
{
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}

namespace {

// ----------------------------------------------------------------------------
std::string
debug_print( polygon const& p )
{
  auto first = true;
  std::stringstream ss;
  ss << "{ ";
  for( auto const& point : p.get_vertices() )
  {
    first = first ? false : ( ss << ", ", false );
    ss << std::setprecision( 17 )
       << "( " << point[ 0 ] << ", " << point[ 1 ] << " )";
  }
  ss << " }";
  return ss.str();
}

// ----------------------------------------------------------------------------
std::string
debug_print( std::optional< polygon > const& p )
{
  return p ? debug_print( *p ) : std::string{ "(empty)" };
}

// ----------------------------------------------------------------------------
void
expect_equivalent(
  std::optional< polygon > const& a,
  std::optional< polygon > const& b )
{
  constexpr auto epsilon = 1.0e-15;
  ASSERT_EQ( a.has_value(), b.has_value() )
    << debug_print( a ) << '\n' << debug_print( b );
  if( a && b )
  {
    auto const a_v = a->get_vertices();
    auto const b_v = b->get_vertices();
    ASSERT_EQ( a_v.size(), b_v.size() )
      << debug_print( a ) << '\n' << debug_print( b );

    auto max_value = 0.0;
    for( auto const& v : { a_v, b_v } )
    {
      for( auto const& p : v )
      {
        max_value =
          std::max(
            max_value, std::max( std::abs( p[ 0 ] ), std::abs( p[ 1 ] ) ) );
      }
    }

    // Polygons are considered equivalent even if the order of the vertices is
    // shifted, i.e. (1 -> 2 -> 3) == (2 -> 3 -> 1) != (1 -> 3 -> 2)
    bool are_equal;
    for( size_t i = 0; i < a_v.size(); ++i )
    {
      are_equal = true;
      for( size_t j = 0; j < a_v.size(); ++j )
      {
        auto const k = ( i + j ) % a_v.size();
        if( std::abs( a_v[ j ][ 0 ] -  b_v[ k ][ 0 ] ) > max_value * epsilon ||
            std::abs( a_v[ j ][ 1 ] -  b_v[ k ][ 1 ] ) > max_value * epsilon )
        {
          are_equal = false;
          break;
        }
      }

      if( are_equal )
      {
        break;
      }
    }

    EXPECT_TRUE( are_equal )
      << debug_print( a ) << '\n' << debug_print( b );
  }
}

// ----------------------------------------------------------------------------
void
test_convex_combine(
  std::optional< polygon >( *fn )( polygon const&, polygon const& ),
  std::optional< polygon > const& expected,
  polygon const& a,
  polygon const& b )
{
  // Test both polygons with all possible vertex orders; with large and small
  // overall scale; positive and negative offsets from the origin; and
  // performing symmetric operations in both directions
  auto const a_points = a.get_vertices();
  auto const b_points = b.get_vertices();
  for( size_t i = 0; i < a_points.size(); ++i )
  {
    for( size_t j = 0; j < b_points.size(); ++j )
    {
      for( auto const scale : { 1.0, 1.0e-9, 1.0e9 } )
      {
        for( auto const offset : { 0.0, -1.0e9, 1.0e9 } )
        {
          std::stringstream ss;
          ss << "i=" << i << " j=" << j << " "
             << "scale=" << scale << " offset=" << offset;
          SCOPED_TRACE( ss.str() );

          std::vector< polygon::point_t > a_prime, b_prime;
          a_prime.reserve( a_points.size() );
          for( size_t k = 0; k < a_points.size(); ++k )
          {
            a_prime.emplace_back(
              ( a_points[ ( k + i ) % a_points.size() ].array() + offset ) *
              scale );
          }

          b_prime.reserve( b_points.size() );
          for( size_t k = 0; k < b_points.size(); ++k )
          {
            b_prime.emplace_back(
              ( b_points[ ( k + j ) % b_points.size() ].array() + offset ) *
              scale );
          }

          std::optional< polygon > expected_prime;
          if( expected )
          {
            auto expected_points = expected->get_vertices();
            for( auto& point : expected_points )
            {
              point = ( point.array() + offset ) * scale;
            }
            expected_prime.emplace( std::move( expected_points ) );
          }

          auto const result1 = fn( a_prime, b_prime );
          CALL_TEST( expect_equivalent, expected_prime, result1 );

          auto const result2 = fn( b_prime, a_prime );
          CALL_TEST( expect_equivalent, expected_prime, result2 );
        }
      }
    }
  }
}

// ----------------------------------------------------------------------------
void
test_convex_union(
  std::optional< polygon > const& expected,
  polygon const& a,
  polygon const& b )
{
  CALL_TEST( test_convex_combine, polygon::convex_union, expected, a, b );
}

// ----------------------------------------------------------------------------
void
test_convex_intersection(
  std::optional< polygon > const& expected,
  polygon const& a,
  polygon const& b )
{
  CALL_TEST(
    test_convex_combine, polygon::convex_intersection, expected, a, b );
}

static const polygon::point_t p1{ 10, 10 };
static const polygon::point_t p2{ 10, 50 };
static const polygon::point_t p3{ 50, 50 };
static const polygon::point_t p4{ 30, 30 };

} // namespace <anonymous>

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

// ----------------------------------------------------------------------------
TEST ( polygon, convex_combine_self )
{
  polygon const a = { { 0.0, 0.0 }, { 1.0, 0.0 }, { 1.0, 1.0 }, { 0.0, 1.0 } };
  polygon const b = a;
  polygon const expected_intersection = a;
  polygon const expected_union = a;
  CALL_TEST( test_convex_intersection, expected_intersection, a, b );
  CALL_TEST( test_convex_union, expected_union, a, b );
}

// ----------------------------------------------------------------------------
TEST ( polygon, convex_combine_completely_internal )
{
  polygon const a = { { 0.0, 0.0 }, { 1.0, 0.0 }, { 1.0, 1.0 }, { 0.0, 1.0 } };
  polygon const b = {
    { 0.25, 0.25 }, { 0.75, 0.25 }, { 0.75, 0.75 }, { 0.25, 0.75 } };
  polygon const expected_intersection = b;
  polygon const expected_union = a;
  CALL_TEST( test_convex_intersection, expected_intersection, a, b );
  CALL_TEST( test_convex_union, expected_union, a, b );
}

// ----------------------------------------------------------------------------
TEST ( polygon, convex_combine_edge_crossing_edge )
{
  polygon const a = { { 0.0, 0.0 }, { 1.0, 0.0 }, { 1.0, 1.0 }, { 0.0, 1.0 } };
  polygon const b = { { 0.5, 0.5 }, { 1.5, 0.5 }, { 1.5, 1.5 }, { 0.5, 1.5 } };
  polygon const expected_intersection = {
    { 0.5, 0.5 }, { 1.0, 0.5 }, { 1.0, 1.0 }, { 0.5, 1.0 } };
  polygon const expected_union = {
    { 0.0, 0.0 }, { 1.0, 0.0 }, { 1.0, 0.5 }, { 1.5, 0.5 },
    { 1.5, 1.5 }, { 0.5, 1.5 }, { 0.5, 1.0 }, { 0.0, 1.0 } };
  CALL_TEST( test_convex_intersection, expected_intersection, a, b );
  CALL_TEST( test_convex_union, expected_union, a, b );
}

// ----------------------------------------------------------------------------
TEST ( polygon, convex_combine_sharing_corner )
{
  polygon const a = { { 0.0, 0.0 }, { 1.0, 0.0 }, { 1.0, 1.0 }, { 0.0, 1.0 } };
  polygon const b = {
    { 0.0, 0.0 }, { 0.75, 0.0 }, { 0.75, 0.75 }, { 0.0, 0.75 } };
  polygon const expected_intersection = b;
  polygon const expected_union = a;
  CALL_TEST( test_convex_intersection, expected_intersection, a, b );
  CALL_TEST( test_convex_union, expected_union, a, b );
}

// ----------------------------------------------------------------------------
TEST ( polygon, convex_combine_sharing_partial_opposite_edges )
{
  polygon const a = { { 0.0, 0.0 }, { 1.0, 0.0 }, { 1.0, 1.0 }, { 0.0, 1.0 } };
  polygon const b = {
    { 0.0, 0.25 }, { 1.0, 0.25 }, { 1.0, 0.75 }, { 0.0, 0.75 } };
  polygon const expected_intersection = b;
  polygon const expected_union = a;
  CALL_TEST( test_convex_intersection, expected_intersection, a, b );
  CALL_TEST( test_convex_union, expected_union, a, b );
}

// ----------------------------------------------------------------------------
TEST ( polygon, convex_combine_sharing_full_edge )
{
  polygon const a = { { 0.0, 0.0 }, { 1.0, 0.0 }, { 1.0, 1.0 }, { 0.0, 1.0 } };
  polygon const b = { { 1.0, 0.0 }, { 2.0, 0.0 }, { 2.0, 1.0 }, { 1.0, 1.0 } };
  polygon const expected_intersection = { { 1.0, 0.0 }, { 1.0, 1.0 } };
  polygon const expected_union = {
    { 0.0, 0.0 }, { 2.0, 0.0 }, { 2.0, 1.0 }, { 0.0, 1.0 } };
  CALL_TEST( test_convex_intersection, expected_intersection, a, b );
  CALL_TEST( test_convex_union, expected_union, a, b );
}

// ----------------------------------------------------------------------------
TEST ( polygon, convex_combine_sharing_partial_edge )
{
  polygon const a = { { 0.0, 0.0 }, { 1.0, 0.0 }, { 1.0, 1.0 }, { 0.0, 1.0 } };
  polygon const b = { { 1.0, 0.5 }, { 2.0, 0.5 }, { 2.0, 1.5 }, { 1.0, 1.5 } };
  polygon const expected_intersection = { { 1.0, 0.5 }, { 1.0, 1.0 } };
  polygon const expected_union = {
    { 0.0, 0.0 }, { 1.0, 0.0 }, { 1.0, 0.5 }, { 2.0, 0.5 }, { 2.0, 1.5 },
    { 1.0, 1.5 }, { 1.0, 1.0 }, { 0.0, 1.0 } };
  CALL_TEST( test_convex_intersection, expected_intersection, a, b );
  CALL_TEST( test_convex_union, expected_union, a, b );
}

// ----------------------------------------------------------------------------
TEST ( polygon, convex_combine_sharing_vertex_external_right )
{
  polygon const a = { { 0.0, 0.0 }, { 1.0, 0.0 }, { 1.0, 1.0 }, { 0.0, 1.0 } };
  polygon const b = { { 1.0, 1.0 }, { 2.0, 1.0 }, { 2.0, 2.0 }, { 1.0, 2.0 } };
  polygon const expected_intersection = { { 1.0, 1.0 } };
  polygon const expected_union = {
    { 0.0, 0.0 }, { 1.0, 0.0 }, { 1.0, 1.0 }, { 2.0, 1.0 }, { 2.0, 2.0 },
    { 1.0, 2.0 }, { 1.0, 1.0 }, { 0.0, 1.0 } };
  CALL_TEST( test_convex_intersection, expected_intersection, a, b );
  CALL_TEST( test_convex_union, expected_union, a, b );
}

// ----------------------------------------------------------------------------
TEST ( polygon, convex_combine_sharing_vertex_external_acute )
{
  polygon const a = { { 0.0, 0.0 }, { 1.0, 0.0 }, { 2.0, 5.0 } };
  polygon const b = { { 3.0, 0.0 }, { 4.0, 0.0 }, { 2.0, 5.0 } };
  polygon const expected_intersection = { { 2.0, 5.0 } };
  polygon const expected_union = {
    { 0.0, 0.0 }, { 1.0, 0.0 }, { 2.0, 5.0 }, { 3.0, 0.0 }, { 4.0, 0.0 },
    { 2.0, 5.0 } };
  CALL_TEST( test_convex_intersection, expected_intersection, a, b );
  CALL_TEST( test_convex_union, expected_union, a, b );
}

// ----------------------------------------------------------------------------
TEST ( polygon, convex_combine_sharing_vertex_external_obtuse )
{
  polygon const a = { { 0.0, 0.0 }, { -1.0, 5.0 }, { -1.0, -5.0 } };
  polygon const b = { { 0.0, 0.0 }, { 1.0, -5.0 }, { 1.0, 5.0 } };
  polygon const expected_intersection = { { 0.0, 0.0 } };
  polygon const expected_union = {
    { 0.0, 0.0 }, { 1.0, -5.0 }, { 1.0, 5.0 }, { 0.0, 0.0 }, { -1.0, 5.0 },
    { -1.0, -5.0 } };
  CALL_TEST( test_convex_intersection, expected_intersection, a, b );
  CALL_TEST( test_convex_union, expected_union, a, b );
}

// ----------------------------------------------------------------------------
TEST ( polygon, convex_combine_sharing_vertex_external_obtuse_acute )
{
  polygon const a = { { 0.0, 0.0 }, { -1.0, 5.0 }, { -1.0, -5.0 } };
  polygon const b = { { 0.0, 0.0 }, { 1.0, -0.1 }, { 1.0, 0.1 } };
  polygon const expected_intersection = { { 0.0, 0.0 } };
  polygon const expected_union = {
    { 0.0, 0.0 }, { 1.0, -0.1 }, { 1.0, 0.1 }, { 0.0, 0.0 }, { -1.0, 5.0 },
    { -1.0, -5.0 } };
  CALL_TEST( test_convex_intersection, expected_intersection, a, b );
  CALL_TEST( test_convex_union, expected_union, a, b );
}

// ----------------------------------------------------------------------------
TEST ( polygon, convex_combine_sharing_vertex_internal_1 )
{
  polygon const a = { { 0.0, 0.0 }, { 1.0, 0.0 }, { 1.0, 1.0 }, { 0.0, 1.0 } };
  polygon const b = {
    { 0.0, 0.0 }, { 0.75, 0.25 }, { 0.75, 0.75 }, { 0.25, 0.75 } };
  polygon const expected_intersection = b;
  polygon const expected_union = a;
  CALL_TEST( test_convex_intersection, expected_intersection, a, b );
  CALL_TEST( test_convex_union, expected_union, a, b );
}

// ----------------------------------------------------------------------------
TEST ( polygon, convex_combine_sharing_vertex_internal_2 )
{
  polygon const a = { { 0.0, 0.0 }, { 1.0, 0.0 }, { 1.0, 1.0 }, { 0.0, 1.0 } };
  polygon const b = {
    { 0.0, 0.0 }, { 1.25, 0.25 }, { 1.5, 1.5 }, { 0.25, 1.25 } };
  polygon const expected_intersection = {
    { 0.0, 0.0 }, { 1.0, 0.2 }, { 1.0, 1.0 }, { 0.2, 1.0 } };
  polygon const expected_union = {
    { 0.0, 0.0 }, { 1.0, 0.0 }, { 1.0, 0.2 }, { 1.25, 0.25 }, { 1.5, 1.5 },
    { 0.25, 1.25 }, { 0.2, 1.0 }, { 0.0, 1.0 } };
  CALL_TEST( test_convex_intersection, expected_intersection, a, b );
  CALL_TEST( test_convex_union, expected_union, a, b );
}

// ----------------------------------------------------------------------------
TEST ( polygon, convex_combine_corner_touching_edge_external )
{
  polygon const a = { { 0.0, 0.0 }, { 1.0, 0.0 }, { 1.0, 1.0 } };
  polygon const b = { { 0.0, 1.0 }, { 0.5, 0.5 }, { 0.5, 2.0 } };
  polygon const expected_intersection = { { 0.5, 0.5 } };
  polygon const expected_union = {
    { 0.0, 0.0 }, { 1.0, 0.0 }, { 1.0, 1.0 }, { 0.5, 0.5 }, { 0.5, 2.0 },
    { 0.0, 1.0 }, { 0.5, 0.5 } };
  CALL_TEST( test_convex_intersection, expected_intersection, a, b );
  CALL_TEST( test_convex_union, expected_union, a, b );
}

// ----------------------------------------------------------------------------
TEST ( polygon, convex_combine_corner_touching_edge_internal_1 )
{
  polygon const a = { { 0.0, 0.0 }, { 1.0, 0.0 }, { 1.0, 1.0 } };
  polygon const b = { { 0.75, 0.5 }, { 0.5, 0.5 }, { 0.5, 0.25 } };
  polygon const expected_intersection = b;
  polygon const expected_union = a;
  CALL_TEST( test_convex_intersection, expected_intersection, a, b );
  CALL_TEST( test_convex_union, expected_union, a, b );
}

// ----------------------------------------------------------------------------
TEST ( polygon, convex_combine_corner_touching_edge_internal_2 )
{
  polygon const a = { { 0.0, 0.0 }, { 1.0, 0.0 }, { 1.0, 1.0 } };
  polygon const b = { { 2.0, 0.5 }, { 0.5, 0.5 }, { 2.0, 0.0 } };
  polygon const expected_intersection = {
    { 1.0, 0.5 }, { 0.5, 0.5 }, { 1.0, 1.0 / 3.0 } };
  polygon const expected_union = {
    { 0.0, 0.0 }, { 1.0, 0.0 }, { 1.0, 1.0 / 3.0 }, { 2.0, 0.0 }, { 2.0, 0.5 },
    { 1.0, 0.5 }, { 1.0, 1.0 } };
  CALL_TEST( test_convex_intersection, expected_intersection, a, b );
  CALL_TEST( test_convex_union, expected_union, a, b );
}

// ----------------------------------------------------------------------------
TEST ( polygon, convex_combine_corners_through_corners )
{
  polygon const a = { { 0.0, 0.0 }, { 1.0, 0.0 }, { 1.0, 1.0 } };
  polygon const b = {
    { 0.0, 1.0 }, { 0.0, 0.0 }, { 0.5, 0.2 }, { 0.7, 0.5 }, { 1.0, 1.0 } };
  polygon const expected_intersection = {
    { 0.0, 0.0 }, { 0.5, 0.2 }, { 0.7, 0.5 }, { 1.0, 1.0 } };
  polygon const expected_union = {
    { 0.0, 1.0 }, { 0.0, 0.0 }, { 1.0, 0.0 }, { 1.0, 1.0 } };
  CALL_TEST( test_convex_intersection, expected_intersection, a, b );
  CALL_TEST( test_convex_union, expected_union, a, b );
}

// ----------------------------------------------------------------------------
TEST ( polygon, convex_combine_edges_through_corners )
{
  polygon const a = { { 0.0, 0.0 }, { 1.0, 0.0 }, { 1.0, 1.0 } };
  polygon const b = {
    { -1.0, 2.0 }, { -0.75, -0.25 }, { 0.75, 0.25 }, { 1.25, 1.75 } };
  polygon const expected_intersection = {
    { 0.0, 0.0 }, { 0.75, 0.25 }, { 1.0, 1.0 } };
  polygon const expected_union = {
    { -1.0, 2.0 }, { -0.75, -0.25 }, { 0.0, 0.0 }, { 1.0, 0.0 }, { 1.0, 1.0 },
    { 1.25, 1.75 } };
  CALL_TEST( test_convex_intersection, expected_intersection, a, b );
  CALL_TEST( test_convex_union, expected_union, a, b );
}

// ----------------------------------------------------------------------------
TEST ( polygon, convex_combine_all_vertices_outside )
{
  polygon const a = { { 0.0, 0.0 }, { 2.0, 0.0 }, { 1.0, 1.0 } };
  polygon const b = { { 0.0, 0.5 }, { 1.0, -1.0 }, { 2.0, 0.5 } };
  polygon const expected_intersection = {
    { 0.5, 0.5 }, { 0.2, 0.2 }, { 1.0 / 3.0, 0.0 }, { 5.0 / 3.0, 0.0 },
    { 1.8, 0.2 }, { 1.5, 0.5 } };
  polygon const expected_union = {
    { 0.5, 0.5 }, { 0.0, 0.5 }, { 0.2, 0.2 }, { 0.0, 0.0 }, { 1.0 / 3.0, 0.0 },
    { 1.0, -1.0 }, { 5.0 / 3.0, 0.0 }, { 2.0, 0.0 }, { 1.8, 0.2 }, { 2.0, 0.5 },
    { 1.5, 0.5 }, { 1.0, 1.0 } };
  CALL_TEST( test_convex_intersection, expected_intersection, a, b );
  CALL_TEST( test_convex_union, expected_union, a, b );
}

// ----------------------------------------------------------------------------
TEST ( polygon, convex_combine_acute_1 )
{
  polygon const a = { { 0.0, 0.0 }, { 1.0, 0.0 }, { 1.0, 1.0 }, { 0.0, 1.0 } };
  polygon const b = { { 1.0, 0.5 }, { -1.0, 1.1 }, { -1.0, 0.7 } };
  polygon const expected_intersection = {
    { 0.0, 0.8 }, { 0.0, 0.6 }, { 1.0, 0.5 } };
  polygon const expected_union = {
    { 0.0, 0.0 }, { 1.0, 0.0 }, { 1.0, 1.0 }, { 0.0, 1.0 }, { 0.0, 0.8 },
    { -1.0, 1.1 }, { -1.0, 0.7 }, { 0.0, 0.6 } };
  CALL_TEST( test_convex_intersection, expected_intersection, a, b );
  CALL_TEST( test_convex_union, expected_union, a, b );
}

// ----------------------------------------------------------------------------
TEST ( polygon, convex_combine_acute_2 )
{
  polygon const a = { { 0.0, 0.0 }, { 1.0, 0.0 }, { 1.0, 1.0 }, { 0.0, 1.0 } };
  polygon const b = { { 2.0, 0.5 }, { -1.0, 0.35 }, { -1.0, 0.05 } };
  polygon const expected_intersection = {
    { 0.0, 0.4 }, { 0.0, 0.2 }, { 1.0, 0.35 }, { 1.0, 0.45 } };
  polygon const expected_union = {
    { 0.0, 0.0 }, { 1.0, 0.0 }, { 1.0, 0.35 }, { 2.0, 0.5 }, { 1.0, 0.45 },
    { 1.0, 1.0 }, { 0.0, 1.0 }, { 0.0, 0.4 }, { -1.0, 0.35 }, { -1.0, 0.05 },
    { 0.0, 0.2 }, };
  CALL_TEST( test_convex_intersection, expected_intersection, a, b );
  CALL_TEST( test_convex_union, expected_union, a, b );
}

// ----------------------------------------------------------------------------
TEST ( polygon, convex_combine_separate_1 )
{
  polygon const a = { { 0.0, 0.0 }, { 1.0, 0.0 }, { 1.0, 1.0 }, { 0.0, 1.0 } };
  polygon const b = { { 1.5, 0.0 }, { 2.5, 0.0 }, { 2.5, 1.0 }, { 1.5, 1.0 } };
  auto const expected_intersection = std::nullopt;
  auto const expected_union = std::nullopt;
  CALL_TEST( test_convex_intersection, expected_intersection, a, b );
  CALL_TEST( test_convex_union, expected_union, a, b );
}

// ----------------------------------------------------------------------------
TEST ( polygon, convex_combine_separate_2 )
{
  polygon const a = { { 0.0, 0.0 }, { 1.0, 0.0 }, { 1.0, 1.0 }, { 0.0, 1.0 } };
  polygon const b = { { 1.1, 0.5 }, { 2.0, 0.0 }, { 2.0, 1.0 } };
  auto const expected_intersection = std::nullopt;
  auto const expected_union = std::nullopt;
  CALL_TEST( test_convex_intersection, expected_intersection, a, b );
  CALL_TEST( test_convex_union, expected_union, a, b );
}
