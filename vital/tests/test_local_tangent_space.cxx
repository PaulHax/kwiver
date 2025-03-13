// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <tests/test_gtest.h>

#include <arrows/geocalc/geo_conv.h>

#include <vital/plugin_management/plugin_manager.h>
#include <vital/types/geodesy.h>
#include <vital/types/local_tangent_space.h>

#include <Eigen/Geometry>

#include <cmath>

using namespace kwiver;
using namespace kwiver::vital;

namespace {

// ----------------------------------------------------------------------------
// Precision within a millimeter should be sufficient
constexpr double epsilon_meters = 1.0e-3;
constexpr double epsilon_degrees = 1.0e-8;

// ----------------------------------------------------------------------------
// Test that the coordinate system is right-handed
void
test_right_handed( local_tangent_space const& space )
{
  auto const point1 =
    space.to_global( { 0.0, 0.0, 0.0 } ).location( SRID::ECEF_WGS84 );
  auto const point2 =
    space.to_global( { 1.0, 0.0, 0.0 } ).location( SRID::ECEF_WGS84 );
  auto const point3 =
    space.to_global( { 0.0, 1.0, 0.0 } ).location( SRID::ECEF_WGS84 );
  auto const point4 =
    space.to_global( { 0.0, 0.0, 1.0 } ).location( SRID::ECEF_WGS84 );

  vector_3d const v1 = ( point2 - point1 ).cross( point3 - point1 );
  vector_3d const v2 = ( point4 - point1 );
  EXPECT_NEAR( v1[ 0 ], v2[ 0 ], epsilon_meters );
  EXPECT_NEAR( v1[ 1 ], v2[ 1 ], epsilon_meters );
  EXPECT_NEAR( v1[ 2 ], v2[ 2 ], epsilon_meters );
}

// ----------------------------------------------------------------------------
// Test that the coordinate system uses meters in a cartesian space
void
test_cartesian_meters( local_tangent_space const& space )
{
  auto const point1 =
    space.to_global( { 0.0, 0.0, 0.0 } ).location( SRID::ECEF_WGS84 );
  auto const point2 =
    space.to_global( { 3'000.0, 4'000.0, 5'000.0 } )
    .location( SRID::ECEF_WGS84 );

  EXPECT_NEAR(
    std::sqrt( 50'000'000.0 ), ( point2 - point1 ).norm(), epsilon_meters );
}

// ----------------------------------------------------------------------------
// Test that the X axis is east and Y axis is north
void
test_east_north( local_tangent_space const& space )
{
  auto const point1 =
    space.to_global( { 0.0, 0.0, 300.0 } ).location( SRID::lat_lon_WGS84 );
  auto const point2 =
    space.to_global( { -1'000.0, 0.0, 300.0 } ).location( SRID::lat_lon_WGS84 );
  auto const point3 =
    space.to_global( { 0.0, -2'000.0, 300.0 } ).location( SRID::lat_lon_WGS84 );

  // X axis - latitude and height should not change much
  EXPECT_NEAR( point1[ 1 ], point2[ 1 ], 1.0e-6 );
  EXPECT_NEAR( point1[ 2 ], point2[ 2 ], 0.5 );

  // Y axis - longitude should not change, height should not change much
  EXPECT_NEAR( point1[ 0 ], point3[ 0 ], epsilon_degrees );
  EXPECT_NEAR( point1[ 2 ], point3[ 2 ], 1.0 );
}

// ----------------------------------------------------------------------------
// Test that the Z axis is up
void
test_up( local_tangent_space const& space )
{
  auto const point1 =
    space.to_global( { 0.0, 0.0, 300.0 } ).location( SRID::lat_lon_WGS84 );
  auto const point2 =
    space.to_global( { 0.0, 0.0, 100'300.0 } ).location( SRID::lat_lon_WGS84 );

  // Z axis - latitude and longitude should not change
  EXPECT_NEAR( point1[ 0 ], point2[ 0 ], epsilon_degrees );
  EXPECT_NEAR( point1[ 1 ], point2[ 1 ], epsilon_degrees );
  EXPECT_NEAR( 100'000.0, point2[ 2 ] - point1[ 2 ], epsilon_meters );
}

// ----------------------------------------------------------------------------
// Test that global -> local -> global yields the same original points
void
test_global_round_trip( local_tangent_space const& space )
{
  std::vector< geo_point > const points = {
    { vector_3d{ 0.0, 0.0, 0.0 }, SRID::ECEF_WGS84 },
    { vector_3d{ 0.0, 90.0, 100.0 }, SRID::lat_lon_WGS84 },
    { vector_3d{ 42.0, 89.999'999, -10.0 }, SRID::lat_lon_WGS84 },
    { vector_3d{ -42.0, -89.999'999, 0.0 }, SRID::lat_lon_WGS84 },
    { vector_3d{ 0.0, 0.0, 0.0 }, SRID::lat_lon_WGS84 },
    { vector_3d{ 90.0, 0.0, 30.0 }, SRID::lat_lon_WGS84 },
    { vector_3d{ 180.0, 0.0, -1'000.0 }, SRID::lat_lon_WGS84 },
    { vector_3d{ -90.0, 0.0, 100'000.0 }, SRID::lat_lon_WGS84 }, };

  for( auto const& point : points )
  {
    auto const result =
      space.to_global( space.to_local( point ) ).location( SRID::ECEF_WGS84 );
    auto const original = point.location( SRID::ECEF_WGS84 );
    EXPECT_NEAR( original[ 0 ], result[ 0 ], epsilon_meters );
    EXPECT_NEAR( original[ 1 ], result[ 1 ], epsilon_meters );
    EXPECT_NEAR( original[ 2 ], result[ 2 ], epsilon_meters );
  }
}

// ----------------------------------------------------------------------------
// Test that local -> global -> local yields the same original points
void
test_local_round_trip( local_tangent_space const& space )
{
  std::vector< vector_3d > const points = {
    { 0.0, 0.0, 0.0 },
    { 1.0, 2.0, 3.0 },
    { -3.0, -2.0, -1.0 },
    { 1.0e-6, 2.0e-6, 3.0e-6 },
    { 3.0e6, 2.0e6, 1.0e6 }, };

  for( auto const& point : points )
  {
    auto const result = space.to_local( space.to_global( point ) );
    EXPECT_NEAR( point[ 0 ], result[ 0 ], epsilon_meters );
    EXPECT_NEAR( point[ 1 ], result[ 1 ], epsilon_meters );
    EXPECT_NEAR( point[ 2 ], result[ 2 ], epsilon_meters );
  }
}

} // namespace <anonymous>

// ----------------------------------------------------------------------------
int
main( int argc, char** argv )
{
  ::testing::InitGoogleTest( &argc, argv );

  arrows::geocalc::geo_conversion converter;
  set_geo_conv( &converter );

  return RUN_ALL_TESTS();
}

// ----------------------------------------------------------------------------
TEST ( local_tangent_space, origin_at_center_of_earth )
{
  local_tangent_space space{
    { vector_3d{ 0.0, 0.0, -6'378'137.0 }, SRID::lat_lon_WGS84 } };
  CALL_TEST( test_right_handed, space );
  CALL_TEST( test_cartesian_meters, space );
  CALL_TEST( test_global_round_trip, space );
  CALL_TEST( test_local_round_trip, space );
}

// ----------------------------------------------------------------------------
TEST ( local_tangent_space, origin_at_north_pole )
{
  local_tangent_space space{
    { vector_3d{ 0.0, 90.0, 0.0 }, SRID::lat_lon_WGS84 } };
  CALL_TEST( test_right_handed, space );
  CALL_TEST( test_cartesian_meters, space );
  CALL_TEST( test_global_round_trip, space );
  CALL_TEST( test_local_round_trip, space );
  CALL_TEST( test_up, space );
}

// ----------------------------------------------------------------------------
TEST ( local_tangent_space, origin_near_north_pole )
{
  local_tangent_space space{
    { vector_3d{ 90.0, 89.999'999'999, -500.0 }, SRID::lat_lon_WGS84 } };
  CALL_TEST( test_right_handed, space );
  CALL_TEST( test_cartesian_meters, space );
  CALL_TEST( test_global_round_trip, space );
  CALL_TEST( test_local_round_trip, space );
  CALL_TEST( test_up, space );
}

// ----------------------------------------------------------------------------
TEST ( local_tangent_space, origin_at_south_pole )
{
  local_tangent_space space{
    { vector_3d{ 180.0, -90.0, 0.0 }, SRID::lat_lon_WGS84 } };
  CALL_TEST( test_right_handed, space );
  CALL_TEST( test_cartesian_meters, space );
  CALL_TEST( test_global_round_trip, space );
  CALL_TEST( test_local_round_trip, space );
  CALL_TEST( test_up, space );
}

// ----------------------------------------------------------------------------
TEST ( local_tangent_space, origin_near_south_pole )
{
  local_tangent_space space{
    { vector_3d{ -90.0, -89.999'999'999, 500.0 }, SRID::lat_lon_WGS84 } };
  CALL_TEST( test_right_handed, space );
  CALL_TEST( test_cartesian_meters, space );
  CALL_TEST( test_global_round_trip, space );
  CALL_TEST( test_local_round_trip, space );
  CALL_TEST( test_up, space );
}

// ----------------------------------------------------------------------------
TEST ( local_tangent_space, origin_at_equator )
{
  local_tangent_space space{
    { vector_3d{ 179.0, 0.0, 0.0 }, SRID::lat_lon_WGS84 } };
  CALL_TEST( test_right_handed, space );
  CALL_TEST( test_cartesian_meters, space );
  CALL_TEST( test_global_round_trip, space );
  CALL_TEST( test_local_round_trip, space );
  CALL_TEST( test_east_north, space );
  CALL_TEST( test_up, space );
}

// ----------------------------------------------------------------------------
TEST ( local_tangent_space, origin_near_equator )
{
  local_tangent_space space{
    { vector_3d{ -179.0, 0.000'001, 10.0 }, SRID::lat_lon_WGS84 } };
  CALL_TEST( test_right_handed, space );
  CALL_TEST( test_cartesian_meters, space );
  CALL_TEST( test_global_round_trip, space );
  CALL_TEST( test_local_round_trip, space );
  CALL_TEST( test_east_north, space );
  CALL_TEST( test_up, space );
}

// ----------------------------------------------------------------------------
TEST ( local_tangent_space, arbitrary_origin_northern_hemisphere )
{
  local_tangent_space space{
    { vector_3d{ -73.7737921, 42.8644703, 50'000.0 }, SRID::lat_lon_WGS84 } };
  CALL_TEST( test_right_handed, space );
  CALL_TEST( test_cartesian_meters, space );
  CALL_TEST( test_global_round_trip, space );
  CALL_TEST( test_local_round_trip, space );
  CALL_TEST( test_east_north, space );
  CALL_TEST( test_up, space );
}

// ----------------------------------------------------------------------------
TEST ( local_tangent_space, arbitrary_origin_southern_hemisphere )
{
  local_tangent_space space{
    { vector_3d{ 73.7737921, -42.8644703, -500.0 }, SRID::lat_lon_WGS84 } };
  CALL_TEST( test_right_handed, space );
  CALL_TEST( test_cartesian_meters, space );
  CALL_TEST( test_global_round_trip, space );
  CALL_TEST( test_local_round_trip, space );
  CALL_TEST( test_east_north, space );
  CALL_TEST( test_up, space );
}
