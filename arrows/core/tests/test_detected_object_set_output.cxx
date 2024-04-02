// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <arrows/core/algo/detected_object_set_output_csv.h>
#include <arrows/core/algo/detected_object_set_output_kw18.h>
#include <vital/plugin_management/pluggable_macro_testing.h>
#include <vital/plugin_management/plugin_manager.h>

#include <gtest/gtest.h>

#include <algorithm>

using namespace kwiver::vital;
using namespace kwiver::arrows::core;

// ----------------------------------------------------------------------------
int
main( int argc, char** argv )
{
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}

// ----------------------------------------------------------------------------
TEST ( detected_object_set_output_csv, create )
{
  plugin_manager::instance().load_all_plugins();

  EXPECT_NE(
    nullptr,
    create_algorithm< algo::detected_object_set_output >( "csv" ) );
}

// ----------------------------------------------------------------------------
TEST ( detected_object_set_output_kw18, create )
{
  plugin_manager::instance().load_all_plugins();

  EXPECT_NE(
    nullptr,
    create_algorithm< algo::detected_object_set_output >( "kw18" ) );
}

// ----------------------------------------------------------------------------
TEST ( detected_object_set_output_csv, default_config )
{
  EXPECT_PLUGGABLE_IMPL(
    detected_object_set_output_csv,
    "Detected object set writer using CSV format.\n\n"
    " - 1: frame number\n"
    " - 2: file name\n"
    " - 3: TL-x\n"
    " - 4: TL-y\n"
    " - 5: BR-x\n"
    " - 6: BR-y\n"
    " - 7: confidence\n"
    " - 8,9: class-name, score"
    " (this pair may be omitted or may repeat any number of times)",
    PARAM_DEFAULT( delim, std::string, "csv delimeter", "," )
  );
}

// ----------------------------------------------------------------------------
TEST ( detected_object_set_output_kw18, default_config )
{
  EXPECT_PLUGGABLE_IMPL(
    detected_object_set_output_kw18,
    "Detected object set writer using kw18 format.\n\n"
    "  - Column(s) 1: Track-id\n"
    "  - Column(s) 2: Track-length (number of detections)\n"
    "  - Column(s) 3: Frame-number (-1 if not available)\n"
    "  - Column(s) 4-5: Tracking-plane-loc(x,y) (could be same as World-loc)\n"
    "  - Column(s) 6-7: Velocity(x,y)\n"
    "  - Column(s) 8-9: Image-loc(x,y)\n"
    "  - Column(s) 10-13: Img-bbox(TL_x,TL_y,BR_x,BR_y)"
    " (location of top-left & bottom-right vertices)\n"
    "  - Column(s) 14: Area\n"
    "  - Column(s) 15-17: World-loc(x,y,z)"
    " (longitude, latitude, 0 - when available)\n"
    "  - Column(s) 18: Timesetamp (-1 if not available)\n"
    "  - Column(s) 19: Track-confidence (-1 if not available)",
    PARAM_DEFAULT( write_tot, bool, "write_tot", false ),
    PARAM_DEFAULT(
      tot_field1_ids, std::string,
      "Comma seperated list of ids used for TOT field 1.",
      "" ),
    PARAM_DEFAULT(
      tot_field2_ids, std::string,
      "Comma seperated list of ids used for TOT field 2.",
      "" )
  );
}
