// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// Test simple_tiled_image_container.

#include <vital/types/tiled_image_container_simple.h>

#include <tests/test_gtest.h>

using namespace kwiver::vital;

// ----------------------------------------------------------------------------
int
main( int argc, char** argv )
{
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}

// ----------------------------------------------------------------------------
TEST ( tiled_image_container_simple, create )
{
  auto container =
    simple_tiled_image_container(
      64, 32, 5, 7, 3, image_pixel_traits_of< float >() );
  EXPECT_EQ( 64 * 5, container.width() );
  EXPECT_EQ( 64, container.tile_width() );
  EXPECT_EQ( 5, container.tile_grid_width() );
  EXPECT_EQ( 32 * 7, container.height() );
  EXPECT_EQ( 32, container.tile_height() );
  EXPECT_EQ( 7, container.tile_grid_height() );
  EXPECT_EQ( 3, container.depth() );
  EXPECT_EQ( 5 * 7, container.tile_grid_size() );
  EXPECT_EQ( 0, container.size() );
  EXPECT_EQ( 0, container.tile_count() );
  EXPECT_EQ(
    image_pixel_traits_of< float >(),
    container.get_image().pixel_traits() );
  EXPECT_EQ( nullptr, container.get_image().first_pixel() );
  EXPECT_EQ( nullptr, container.get_tile( 0, 0 ) );
}

// ----------------------------------------------------------------------------
TEST ( tiled_image_container_simple, set_metadata )
{
  auto container = simple_tiled_image_container( 1, 1, 1, 1, 1 );
  auto md = std::make_shared< metadata >();
  container.set_metadata( md );
  EXPECT_EQ( md, container.get_metadata() );
}

// ----------------------------------------------------------------------------
TEST ( tiled_image_container_simple, set_tile )
{
  auto container = simple_tiled_image_container( 64, 32, 5, 7, 3 );
  auto image1 = image( 64, 32, 3 );
  auto tile1 = std::make_shared< simple_image_container >( image1 );
  auto image2 = image( 64, 32, 3, true );
  auto tile2 = std::make_shared< simple_image_container >( image2 );
  container.set_tile( 1, 2, tile1 );
  container.set_tile( 4, 6, tile2 );

  EXPECT_EQ( nullptr, container.get_tile( 0, 1 ) );
  EXPECT_EQ( tile1, container.get_tile( 1, 2 ) );
  EXPECT_EQ( tile2, container.get_tile( 4, 6 ) );
  EXPECT_EQ( tile1->size() + tile2->size(), container.size() );
  EXPECT_EQ( 2, container.tile_count() );

  size_t x, y;
  ASSERT_TRUE( container.next_tile( x, y, true ) );
  EXPECT_EQ( x, 1 );
  EXPECT_EQ( y, 2 );
  ASSERT_TRUE( container.next_tile( x, y ) );
  EXPECT_EQ( x, 4 );
  EXPECT_EQ( y, 6 );
  ASSERT_FALSE( container.next_tile( x, y ) );
}
