// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <arrows/ocv/draw_detected_object_set.h>

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
TEST ( draw_detected_object_set, create )
{
  plugin_manager::instance().load_all_plugins();
  EXPECT_NE(
    nullptr, create_algorithm< algo::draw_detected_object_set >( "ocv" ) );
}

// ----------------------------------------------------------------------------
TEST ( draw_detected_object_set, default_config )
{
  EXPECT_PLUGGABLE_IMPL(
    draw_detected_object_set,
    "Draw bounding box around detected objects on supplied image.",

    PARAM_DEFAULT(
      threshold,
      float,
      "min threshold for output (float). "
      "Detections with confidence values below this value are not drawn.",
      -1 ),

    PARAM_DEFAULT(
      alpha_blend_prob,
      bool,
      "If true, those who are less likely will be more transparent.",
      true ),

    PARAM_DEFAULT(
      default_line_thickness,
      float,
      "The default line thickness, in pixels.",
      1 ),

    PARAM_DEFAULT(
      default_color,
      std::string,
      "The default color for a class (RGB).",
      "0 0 255" ),

    PARAM_DEFAULT(
      custom_class_color,
      std::string,
      "List of class/thickness/color seperated by semicolon. "
      "For example: person/3/255 0 0;car/2/0 255 0. "
      "Color is in RGB.",
      "" ),

    PARAM_DEFAULT(
      select_classes,
      std::string,
      "List of classes to display, separated by a semicolon. For example: person;car;clam",
      "*ALL*" ),

    PARAM_DEFAULT(
      text_scale,
      float,
      "Scaling for the text label. "
      "Font scale factor that is multiplied by the font-specific base size.",
      0.4 ),

    PARAM_DEFAULT(
      text_thickness,
      float,
      "Thickness of the lines used to draw a text.",
      1 ),

    PARAM_DEFAULT(
      clip_box_to_image,
      bool,
      "If this option is set to true, the bounding box is clipped to the image bounds.",
      false ),

    PARAM_DEFAULT(
      draw_text,
      bool,
      "If this option is set to true, the class name is drawn next to the detection.",
      true )
  );
}
