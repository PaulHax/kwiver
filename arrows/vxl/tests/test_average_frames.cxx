// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <test_gtest.h>

#include <arrows/vxl/average_frames.h>
#include <arrows/vxl/image_container.h>
#include <arrows/vxl/image_io.h>

#include <vital/algo/algorithm.txx>
#include <vital/plugin_management/pluggable_macro_testing.h>
#include <vital/plugin_management/plugin_manager.h>

#include <vil/vil_plane.h>

#include <gtest/gtest.h>

#include <vector>

namespace kv = kwiver::vital;
namespace ka = kwiver::arrows;

using namespace kwiver::vital;
using namespace kwiver::arrows;

kv::path_t g_data_dir;
static std::string test_red_image_name =
  "images/kitware_logos/small_red_logo.png";
static std::string test_green_image_name =
  "images/kitware_logos/small_green_logo.png";
static std::string test_blue_image_name =
  "images/kitware_logos/small_blue_logo.png";

static std::string window_first_expected_name =
  "images/kitware_logos/window_expected_first_average.png";
static std::string window_second_expected_name =
  "images/kitware_logos/window_expected_second_average.png";
static std::string window_third_expected_name =
  "images/kitware_logos/window_expected_third_average.png";

static std::string cumulative_first_expected_name =
  "images/kitware_logos/cumulative_expected_first_average.png";
static std::string cumulative_second_expected_name =
  "images/kitware_logos/cumulative_expected_second_average.png";
static std::string cumulative_third_expected_name =
  "images/kitware_logos/cumulative_expected_third_average.png";

static std::string exponential_first_expected_name =
  "images/kitware_logos/exponential_expected_first_average.png";
static std::string exponential_second_expected_name =
  "images/kitware_logos/exponential_expected_second_average.png";
static std::string exponential_third_expected_name =
  "images/kitware_logos/exponential_expected_third_average.png";

// ----------------------------------------------------------------------------
int
main( int argc, char** argv )
{
  ::testing::InitGoogleTest( &argc, argv );
  TEST_LOAD_PLUGINS();

  GET_ARG( 1, g_data_dir );

  return RUN_ALL_TESTS();
}

// ----------------------------------------------------------------------------
class average_frames : public ::testing::Test
{
  TEST_ARG( data_dir );
};

// ----------------------------------------------------------------------------
void
test_averaging_type(
  kv::path_t data_dir, std::string type,
  std::vector< std::string > expected_filenames )
{
  std::string red_filename = data_dir + "/" + test_red_image_name;
  std::string green_filename = data_dir + "/" + test_green_image_name;
  std::string blue_filename = data_dir + "/" + test_blue_image_name;

  std::string first_expected_filename =
    data_dir + "/" + expected_filenames.at( 0 );
  std::string second_expected_filename =
    data_dir + "/" + expected_filenames.at( 1 );
  std::string third_expected_filename =
    data_dir + "/" + expected_filenames.at( 2 );

  ka::vxl::average_frames filter;

  ka::vxl::image_io io;

  auto const first_channel = io.load( red_filename );
  auto const second_channel = io.load( green_filename );
  auto const third_channel = io.load( blue_filename );

  auto config = kv::config_block::empty_config();
  config->set_value( "type", type );
  filter.set_configuration( config );

  auto const first_filtered = filter.filter( first_channel );
  auto const second_filtered = filter.filter( second_channel );
  auto const third_filtered = filter.filter( third_channel );

  auto const first_expected = io.load( first_expected_filename );
  auto const second_expected = io.load( second_expected_filename );
  auto const third_expected = io.load( third_expected_filename );

  EXPECT_TRUE(
    equal_content(
      first_filtered->get_image(),
      first_expected->get_image() ) );
  EXPECT_TRUE(
    equal_content(
      second_filtered->get_image(),
      second_expected->get_image() ) );
  EXPECT_TRUE(
    equal_content(
      third_filtered->get_image(),
      third_expected->get_image() ) );
}

// ----------------------------------------------------------------------------
TEST_F ( average_frames, default_config )
{
  EXPECT_PLUGGABLE_IMPL(
    vxl::average_frames,
    "Use VXL to average frames together.",
    PARAM_DEFAULT(
      type, std::string,
      "Operating mode of this filter, possible values: " +
      vxl::average_frames::averager_converter().element_name_string(),
      vxl::average_frames::averager_converter().to_string(
        vxl::AVERAGER_window ) ),
    PARAM_DEFAULT(
      window_size, unsigned,
      "The window size if computing a windowed moving average.",
      10 ),
    PARAM_DEFAULT(
      exp_weight, double,
      "Exponential averaging coefficient if computing an exp average.",
      0.3 ),
    PARAM_DEFAULT(
      round, bool,
      "Should we spend a little extra time rounding when possible?",
      false ),
    PARAM_DEFAULT(
      output_variance, bool,
      "If set, will compute an estimated variance for each pixel which "
      "will be outputted as either a double-precision or byte image.",
      false )
  )
}

// ----------------------------------------------------------------------------
TEST_F ( average_frames, create )
{
  plugin_manager::instance().load_all_plugins();

  EXPECT_NE(
    nullptr,
    create_algorithm< algo::image_filter >( "vxl_average" ) );
}

// ----------------------------------------------------------------------------
TEST_F ( average_frames, window )
{
  test_averaging_type(
    data_dir, "window", { window_first_expected_name,
                          window_second_expected_name,
                          window_third_expected_name } );
}

// ----------------------------------------------------------------------------
TEST_F ( average_frames, cumulative )
{
  test_averaging_type(
    data_dir, "cumulative",
    { cumulative_first_expected_name,
      cumulative_second_expected_name,
      cumulative_third_expected_name } );
}

// ----------------------------------------------------------------------------
TEST_F ( average_frames, exponential )
{
  test_averaging_type(
    data_dir, "exponential",
    { exponential_first_expected_name,
      exponential_second_expected_name,
      exponential_third_expected_name } );
}
