// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// Test the FFmpeg image_io implementation

#include <test_tmpfn.h>

#include <arrows/ffmpeg/algo/ffmpeg_image_io.h>
#include <arrows/ffmpeg/tests/common.h>

#include <vital/plugin_management/plugin_manager.h>

#include <filesystem>
#include <limits>
#include <optional>

std::filesystem::path g_data_dir;

namespace {

// ----------------------------------------------------------------------------
template < class T >
kv::image
create_test_image(
  size_t width, size_t height, size_t depth, bool planar,
  size_t line_padding )
{
  constexpr double maximum = std::numeric_limits< T >::max();
  auto const pixel_traits = kv::image_pixel_traits_of< T >();
  std::optional< kv::image > image;

  auto const image_memory =
    std::make_shared< kv::image_memory >(
      ( width + line_padding ) * height * depth * pixel_traits.num_bytes + 64 );
  if( planar )
  {
    image.emplace(
      image_memory, image_memory->data(),
      width, height, depth,
      1, width + line_padding, height * ( width + line_padding ),
      pixel_traits );
  }
  else
  {
    image.emplace(
      image_memory, image_memory->data(),
      width, height, depth,
      depth, width * depth + line_padding, 1,
      pixel_traits );
  }

  for( size_t y = 0; y < height; ++y )
  {
    for( size_t x = 0; x < width; ++x )
    {
      for( size_t c = 0; c < depth; ++c )
      {
        if constexpr( std::is_same_v< T, bool > )
        {
          image->at< T >( x, y, c ) = static_cast< T >( ( y + x + c ) % 5 );
        }
        else
        {
          image->at< T >( x, y, c ) =
            static_cast< T >(
              std::max< double >(
                0, std::min< double >(
                  maximum,
                  maximum / ( height - 1 ) * y -
                  maximum / ( width - 1 ) * x +
                  maximum / ( depth - 1 ) * c ) ) );
        }
      }
    }
  }
  return *image;
}

// ----------------------------------------------------------------------------
template < class T >
void
assert_test_image(
  kv::image const& image, size_t width, size_t height, size_t depth,
  size_t epsilon = 0 )
{
  ASSERT_EQ( width, image.width() );
  ASSERT_EQ( height, image.height() );
  ASSERT_EQ( depth, image.depth() );
  ASSERT_EQ( kv::image_pixel_traits_of< T >(), image.pixel_traits() );

  constexpr double maximum = std::numeric_limits< T >::max();
  for( size_t y = 0; y < height; ++y )
  {
    for( size_t x = 0; x < width; ++x )
    {
      for( size_t c = 0; c < depth; ++c )
      {
        if constexpr( std::is_same_v< T, bool > )
        {
          ASSERT_EQ(
            static_cast< T >( ( y + x + c ) % 5 ),
            image.at< T >( x, y, c ) );
        }
        else
        {
          ASSERT_NEAR(
            static_cast< T >(
              std::max< double >(
                0, std::min< double >(
                  maximum,
                  maximum / ( height - 1 ) * y -
                  maximum / ( width - 1 ) * x +
                  maximum / ( depth - 1 ) * c ) ) ),
            image.at< T >( x, y, c ), epsilon );
        }
      }
    }
  }
}

} // namespace <anonymous>

// ----------------------------------------------------------------------------
int
main( int argc, char* argv[] )
{
  ::testing::InitGoogleTest( &argc, argv );
  TEST_LOAD_PLUGINS();

  GET_ARG( 1, g_data_dir );

  return RUN_ALL_TESTS();
}

// ----------------------------------------------------------------------------
TEST ( ffmpeg_image_io, create )
{
  EXPECT_NE(
    nullptr,
    kv::create_algorithm< kv::algo::image_io >( "ffmpeg" ) );
}

// ----------------------------------------------------------------------------
TEST ( ffmpeg_image_io, load_png )
{
  auto const path = g_data_dir / "images/test.png";

  ffmpeg::ffmpeg_image_io io;
  auto const loaded_image = io.load( path.string() );

  ASSERT_NE( nullptr, loaded_image );

  ASSERT_EQ( 40, loaded_image->height() );
  ASSERT_EQ( 60, loaded_image->width() );
  ASSERT_EQ( 3,  loaded_image->depth() );

  EXPECT_EQ( 0, loaded_image->get_image().at< uint8_t >( 0, 0, 0 ) );
  EXPECT_EQ( 0, loaded_image->get_image().at< uint8_t >( 0, 0, 1 ) );
  EXPECT_EQ( 0, loaded_image->get_image().at< uint8_t >( 0, 0, 2 ) );

  EXPECT_EQ( 0,   loaded_image->get_image().at< uint8_t >( 59, 0, 0 ) );
  EXPECT_EQ( 245, loaded_image->get_image().at< uint8_t >( 59, 0, 1 ) );
  EXPECT_EQ( 0,   loaded_image->get_image().at< uint8_t >( 59, 0, 2 ) );

  EXPECT_EQ( 245, loaded_image->get_image().at< uint8_t >( 59, 39, 0 ) );
  EXPECT_EQ( 0,   loaded_image->get_image().at< uint8_t >( 59, 39, 1 ) );
  EXPECT_EQ( 0,   loaded_image->get_image().at< uint8_t >( 59, 39, 2 ) );

  EXPECT_EQ( 0,   loaded_image->get_image().at< uint8_t >( 0, 39, 0 ) );
  EXPECT_EQ( 0,   loaded_image->get_image().at< uint8_t >( 0, 39, 1 ) );
  EXPECT_EQ( 245, loaded_image->get_image().at< uint8_t >( 0, 39, 2 ) );
}

// ----------------------------------------------------------------------------
TEST ( ffmpeg_image_io, load_jpeg )
{
  auto const path = g_data_dir / "images/test.jpg";

  ffmpeg::ffmpeg_image_io io;
  auto const loaded_image = io.load( path.string() );

  ASSERT_NE( nullptr, loaded_image );

  ASSERT_EQ( 32, loaded_image->height() );
  ASSERT_EQ( 32, loaded_image->width() );
  ASSERT_EQ( 3,  loaded_image->depth() );

  EXPECT_EQ( 0, loaded_image->get_image().at< uint8_t >( 0, 0, 0 ) );
  EXPECT_EQ( 0, loaded_image->get_image().at< uint8_t >( 0, 0, 1 ) );
  EXPECT_EQ( 0, loaded_image->get_image().at< uint8_t >( 0, 0, 2 ) );

  EXPECT_EQ( 1,   loaded_image->get_image().at< uint8_t >( 31, 0, 0 ) );
  EXPECT_EQ( 240, loaded_image->get_image().at< uint8_t >( 31, 0, 1 ) );
  EXPECT_EQ( 0,   loaded_image->get_image().at< uint8_t >( 31, 0, 2 ) );

  EXPECT_EQ( 240, loaded_image->get_image().at< uint8_t >( 31, 31, 0 ) );
  EXPECT_EQ( 0,   loaded_image->get_image().at< uint8_t >( 31, 31, 1 ) );
  EXPECT_EQ( 0,   loaded_image->get_image().at< uint8_t >( 31, 31, 2 ) );

  EXPECT_EQ( 0,   loaded_image->get_image().at< uint8_t >( 0, 31, 0 ) );
  EXPECT_EQ( 0,   loaded_image->get_image().at< uint8_t >( 0, 31, 1 ) );
  EXPECT_EQ( 240, loaded_image->get_image().at< uint8_t >( 0, 31, 2 ) );
}

// ----------------------------------------------------------------------------
TEST ( ffmpeg_image_io, load_tiff )
{
  auto const path = g_data_dir / "images/test.tif";

  ffmpeg::ffmpeg_image_io io;
  auto const loaded_image = io.load( path.string() );

  ASSERT_NE( nullptr, loaded_image );

  loaded_image->get_image().at< uint16_t >( 31, 31, 0 );
  ASSERT_EQ( 32, loaded_image->height() );
  ASSERT_EQ( 32, loaded_image->width() );
  ASSERT_EQ( 1,  loaded_image->depth() );

  EXPECT_EQ( 0,     loaded_image->get_image().at< uint16_t >( 0, 0, 0 ) );
  EXPECT_EQ( 0,     loaded_image->get_image().at< uint16_t >( 31, 0, 0 ) );
  EXPECT_EQ( 61504, loaded_image->get_image().at< uint16_t >( 31, 31, 0 ) );
  EXPECT_EQ( 0,     loaded_image->get_image().at< uint16_t >( 0, 31, 0 ) );
}

// ----------------------------------------------------------------------------
TEST ( ffmpeg_image_io, save_png_bool )
{
  auto const path = kwiver::testing::temp_file_name( "test-", ".png" );

  ffmpeg::ffmpeg_image_io io;
  auto const image = create_test_image< bool >( 32, 64, 1, false, 8 );
  io.save( path, std::make_shared< kv::simple_image_container >( image ) );

  _tmp_file_deleter tmp_file_deleter{ path };

  auto const loaded_image = io.load( path );
  CALL_TEST(
    assert_test_image< bool >, loaded_image->get_image(), 32, 64, 1, 0 );
}

// ----------------------------------------------------------------------------
TEST ( ffmpeg_image_io, save_png_gray )
{
  auto const path = kwiver::testing::temp_file_name( "test-", ".png" );

  ffmpeg::ffmpeg_image_io io;
  auto const image = create_test_image< uint8_t >( 32, 64, 1, false, 0 );
  io.save( path, std::make_shared< kv::simple_image_container >( image ) );

  _tmp_file_deleter tmp_file_deleter{ path };

  auto const loaded_image = io.load( path );
  CALL_TEST(
    assert_test_image< uint8_t >, loaded_image->get_image(), 32, 64, 1, 0 );
}

// ----------------------------------------------------------------------------
TEST ( ffmpeg_image_io, save_png_gray_alpha )
{
  auto const path = kwiver::testing::temp_file_name( "test-", ".png" );

  ffmpeg::ffmpeg_image_io io;
  auto const image = create_test_image< uint8_t >( 32, 64, 2, true, 0 );
  io.save( path, std::make_shared< kv::simple_image_container >( image ) );

  _tmp_file_deleter tmp_file_deleter{ path };

  auto const loaded_image = io.load( path );
  CALL_TEST(
    assert_test_image< uint8_t >, loaded_image->get_image(), 32, 64, 2, 0 );
}

// ----------------------------------------------------------------------------
TEST ( ffmpeg_image_io, save_png_rgb )
{
  auto const path = kwiver::testing::temp_file_name( "test-", ".png" );

  ffmpeg::ffmpeg_image_io io;
  auto const image = create_test_image< uint8_t >( 32, 64, 3, true, 0 );
  io.save( path, std::make_shared< kv::simple_image_container >( image ) );

  _tmp_file_deleter tmp_file_deleter{ path };

  auto const loaded_image = io.load( path );
  CALL_TEST(
    assert_test_image< uint8_t >, loaded_image->get_image(), 32, 64, 3, 0 );
}

// ----------------------------------------------------------------------------
TEST ( ffmpeg_image_io, save_png_rgba )
{
  auto const path = kwiver::testing::temp_file_name( "test-", ".png" );

  ffmpeg::ffmpeg_image_io io;
  auto const image = create_test_image< uint16_t >( 32, 64, 4, false, 1 );
  io.save( path, std::make_shared< kv::simple_image_container >( image ) );

  _tmp_file_deleter tmp_file_deleter{ path };

  auto const loaded_image = io.load( path );
  CALL_TEST(
    assert_test_image< uint16_t >, loaded_image->get_image(), 32, 64, 4, 0 );
}

// ----------------------------------------------------------------------------
TEST ( ffmpeg_image_io, save_jpeg )
{
  auto const path = kwiver::testing::temp_file_name( "test-", ".jpg" );

  ffmpeg::ffmpeg_image_io io;

  // Set JPEG to highest quality
  auto config = io.get_configuration();
  config->set_value( "quality", 1 );
  io.set_configuration( config );

  auto const image = create_test_image< uint8_t >( 64, 32, 3, true, 2 );
  io.save( path, std::make_shared< kv::simple_image_container >( image ) );

  _tmp_file_deleter tmp_file_deleter{ path };

  auto const loaded_image = io.load( path );
  CALL_TEST(
    assert_test_image< uint8_t >, loaded_image->get_image(), 64, 32, 3, 10 );
}

// ----------------------------------------------------------------------------
TEST ( ffmpeg_image_io, save_tiff )
{
  auto const path = kwiver::testing::temp_file_name( "test-", ".tif" );

  ffmpeg::ffmpeg_image_io io;
  auto const image = create_test_image< uint16_t >( 32, 64, 3, true, 0 );
  io.save( path, std::make_shared< kv::simple_image_container >( image ) );

  _tmp_file_deleter tmp_file_deleter{ path };

  auto const loaded_image = io.load( path );
  CALL_TEST(
    assert_test_image< uint16_t >, loaded_image->get_image(), 32, 64, 3, 0 );
}

// ----------------------------------------------------------------------------
TEST ( ffmpeg_image_io, save_tiff_gray )
{
  auto const path = kwiver::testing::temp_file_name( "test-", ".tif" );

  ffmpeg::ffmpeg_image_io io;
  auto const image = create_test_image< uint16_t >( 32, 64, 1, false, 31 );
  io.save( path, std::make_shared< kv::simple_image_container >( image ) );

  _tmp_file_deleter tmp_file_deleter{ path };

  auto const loaded_image = io.load( path );
  CALL_TEST(
    assert_test_image< uint16_t >, loaded_image->get_image(), 32, 64, 1, 0 );
}
