// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <arrows/ocv/algo/extract_descriptors_BRIEF.h>
#include <arrows/ocv/algo/extract_descriptors_DAISY.h>
#include <arrows/ocv/algo/extract_descriptors_FREAK.h>
#include <arrows/ocv/algo/extract_descriptors_LATCH.h>
#include <arrows/ocv/algo/extract_descriptors_LUCID.h>

#include <vital/plugin_management/pluggable_macro_testing.h>
#include <vital/plugin_management/plugin_manager.h>

#include <vital/algo/algorithm.txx>

#include <gtest/gtest.h>

using namespace kwiver::vital;
using namespace kwiver::arrows::ocv;

// ----------------------------------------------------------------------------
int
main( int argc, char** argv )
{
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}

// ----------------------------------------------------------------------------
TEST ( extract_descriptors_BRIEF, create )
{
  plugin_manager::instance().load_all_plugins();
  EXPECT_NE(
    nullptr, create_algorithm< algo::extract_descriptors >( "ocv_BRIEF" ) );
}

// ----------------------------------------------------------------------------
TEST ( extract_descriptors_DAISY, create )
{
  plugin_manager::instance().load_all_plugins();
  EXPECT_NE(
    nullptr, create_algorithm< algo::extract_descriptors >( "ocv_DAISY" ) );
}

// ----------------------------------------------------------------------------
TEST ( extract_descriptors_FREAK, create )
{
  plugin_manager::instance().load_all_plugins();
  EXPECT_NE(
    nullptr, create_algorithm< algo::extract_descriptors >( "ocv_FREAK" ) );
}

// ----------------------------------------------------------------------------
TEST ( extract_descriptors_LATCH, create )
{
  plugin_manager::instance().load_all_plugins();
  EXPECT_NE(
    nullptr, create_algorithm< algo::extract_descriptors >( "ocv_LATCH" ) );
}

// ----------------------------------------------------------------------------
TEST ( extract_descriptors_LUCID, create )
{
  plugin_manager::instance().load_all_plugins();
  EXPECT_NE(
    nullptr, create_algorithm< algo::extract_descriptors >( "ocv_LUCID" ) );
}

// ----------------------------------------------------------------------------
TEST ( extract_descriptors_BRIEF, default_config )
{
  EXPECT_PLUGGABLE_IMPL(
    extract_descriptors_BRIEF,
    "OpenCV feature-point descriptor extraction via the BRIEF algorithm",
    PARAM_DEFAULT(
      bytes, int,
      "Length of descriptor in bytes. It can be equal 16, 32 "
      "or 64 bytes.",
      32 )
#if KWIVER_OPENCV_VERSION_MAJOR >= 3
    ,
    PARAM_DEFAULT(
      use_orientation, bool,
      "sample patterns using keypoints orientation, disabled "
      "by default.",
      false )
#endif
  );
}

// ----------------------------------------------------------------------------
TEST ( extract_descriptors_DAISY, default_config )
{
  EXPECT_PLUGGABLE_IMPL(
    extract_descriptors_DAISY,
    "OpenCV feature-point descriptor extraction via the DAISY algorithm",

    PARAM_DEFAULT(
      radius, float,
      "radius of the descriptor at the initial scale",
      15 ),

    PARAM_DEFAULT(
      q_radius, int,
      "amount of radial range division quantity",
      3 ),

    PARAM_DEFAULT(
      q_theta, int,
      "amount of angular range division quantity",
      3 ),

    PARAM_DEFAULT(
      q_hist, int,
      "amount of gradient orientations range division quantity",
      8 ),

//    PARAM_DEFAULT(
//      norm,
//      int,
//      "descriptor normalization type. valid choices:\n" +
//      list_norm_options,
//      cv::xfeatures2d::DAISY::NRM_NONE ),

    PARAM_DEFAULT(
      interpolation, bool,
      "", true ),

    PARAM_DEFAULT(
      use_orientation, bool,
      "", false ),
  );
}

// ----------------------------------------------------------------------------
TEST ( extract_descriptors_FREAK, default_config )
{
  EXPECT_PLUGGABLE_IMPL(
    extract_descriptors_FREAK,
    "OpenCV feature-point descriptor extraction via the FREAK algorithm",

    PARAM_DEFAULT(
      orientation_normalized, bool,
      "enable orientation normalization",
      true ),

    PARAM_DEFAULT(
      scale_normalized, bool,
      "enable scale normalization",
      true ),

    PARAM_DEFAULT(
      pattern_scale, float,
      "scaling of the description pattern",
      22 ),

    PARAM_DEFAULT(
      n_octaves, int,
      "number of octaves covered by the detected keypoints",
      4 )
  );
}

// ----------------------------------------------------------------------------
TEST ( extract_descriptors_LATCH, default_config )
{
  EXPECT_PLUGGABLE_IMPL(
    extract_descriptors_LATCH,
    "OpenCV feature-point descriptor extraction via the LATCH algorithm",

    PARAM_DEFAULT(
      bytes, int,
      "bytes",
      32 ),

    PARAM_DEFAULT(
      rotation_invariance, bool,
      "rotation_invariance",
      true ),

    PARAM_DEFAULT(
      half_ssd_size, int,
      "half_ssd_size",
      3 ),
  );
}

// ----------------------------------------------------------------------------
TEST ( extract_descriptors_LUCID, default_config )
{
  EXPECT_PLUGGABLE_IMPL(
    extract_descriptors_LUCID,
    "OpenCV feature-point descriptor extraction via the LUCID algorithm",

    PARAM_DEFAULT(
      lucid_kernel, int,
      "kernel for descriptor construction, where 1=3x3, "
      "2=5x5, 3=7x7 and so forth",
      1 ),

    PARAM_DEFAULT(
      blur_kernel, int,
      "kernel for blurring image prior to descriptor "
      "construction, where 1=3x3, 2=5x5, 3=7x7 and so forth",
      1 )
  );
}
