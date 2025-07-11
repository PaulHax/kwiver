// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <test_gtest.h>

#include <arrows/vxl/aligned_edge_detection.h>

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

kv::path_t g_data_dir;
static std::string test_image = "images/kitware_logos/small_grey_logo.png";

static std::string expected_seperate_edges =
  "images/kitware_logos/expected_edge_seperate.png";
static std::string expected_combined_edges =
  "images/kitware_logos/expected_edge_combined.png";

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
class aligned_edge_detection : public ::testing::Test
{
public:
  void SetUp();

  TEST_ARG( data_dir );

  ka::vxl::image_io io;
  ka::vxl::aligned_edge_detection filter;
  kv::image_container_sptr input_image;
};

// ----------------------------------------------------------------------------
void
aligned_edge_detection
::SetUp()
{
  std::string test_file = data_dir + "/" + test_image;
  ka::vxl::image_io io;
  input_image = io.load( test_file );
}

// ----------------------------------------------------------------------------
TEST_F ( aligned_edge_detection, default_config )
{
  EXPECT_PLUGGABLE_IMPL(
    ka::vxl::aligned_edge_detection,
    "Compute axis-aligned edges in an image.",
    PARAM_DEFAULT(
      threshold, float,
      "Minimum edge magnitude required to report as an edge "
      "in any output image.",
      10.0f ),
    PARAM_DEFAULT(
      produce_joint_output, bool,
      "Set to false if we do not want to spend time computing "
      "joint edge images comprised of both horizontal and "
      "vertical information.",
      true ),
    PARAM_DEFAULT(
      smoothing_sigma, double,
      "Smoothing sigma for the output NMS edge density map.",
      1.3 ),
    PARAM_DEFAULT(
      smoothing_half_step, unsigned,
      "Smoothing half step for the output NMS edge density map.",
      2 )
  );
}

// ----------------------------------------------------------------------------
TEST_F ( aligned_edge_detection, create )
{
  kwiver::vital::plugin_manager::instance().load_all_plugins();

  EXPECT_NE(
    nullptr,
    kwiver::vital::create_algorithm< kwiver::vital::algo::image_filter >(
      "vxl_aligned_edge_detection" )
  );
}

// ----------------------------------------------------------------------------
TEST_F ( aligned_edge_detection, seperate )
{
  auto const expected_filename = data_dir + "/" + expected_seperate_edges;

  auto config = kv::config_block::empty_config();
  config->set_value( "produce_joint_output", false );
  filter.set_configuration( config );

  auto const filtered_image_ptr = filter.filter( input_image );
  auto const expected_image_ptr = io.load( expected_filename );

  EXPECT_TRUE(
    equal_content(
      filtered_image_ptr->get_image(),
      expected_image_ptr->get_image() ) );
}

// ----------------------------------------------------------------------------
TEST_F ( aligned_edge_detection, combined )
{
  auto const expected_filename = data_dir + "/" + expected_combined_edges;

  auto const filtered_image_ptr = filter.filter( input_image );
  auto const expected_image_ptr = io.load( expected_filename );

  EXPECT_TRUE(
    equal_content(
      filtered_image_ptr->get_image(),
      expected_image_ptr->get_image() ) );
}
