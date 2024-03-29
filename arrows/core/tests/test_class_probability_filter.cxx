// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <arrows/core/algo/class_probability_filter.h>
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
TEST ( class_probability_filter, create )
{
  using namespace kwiver::vital;

  plugin_manager::instance().load_all_plugins();

  EXPECT_NE(
    nullptr, create_algorithm< algo::detected_object_filter >(
      "class_probability_filter" ) );
}

// ----------------------------------------------------------------------------
TEST ( class_probability_filter, default_config )
{
  using namespace kwiver::vital;

  EXPECT_PLUGGABLE_IMPL(
    class_probability_filter,
    "Filters detections based on class probability.\n\n"
    "This algorithm filters out items that are less than the threshold."
    " The following steps are applied to each input detected object set.\n\n"
    "1) Select all class names with scores greater than threshold.\n\n"
    "2) Create a new detected_object_type object with all selected class"
    " names from step 1. The class name can be selected individually"
    " or with the keep_all_classes option.\n\n"
    "3) The input detection_set is cloned and the detected_object_type"
    " from step 2 is attached.",
    PARAM_DEFAULT(
      threshold, double,
      "Detections are passed through this filter if they have a selected classification that is "
      "above this threshold.",
      0.0 ),
    PARAM_DEFAULT(
      keep_all_classes, bool,
      "If this options is set to true, all classes are passed through this filter "
      "if they are above the selected threshold.",
      true ),
    PARAM_DEFAULT(
      list_of_classes,
      std::string,
      "A list of class names to pass through this filter. "
      "Multiple names are separated by a ';' character. "
      "The keep_all_classes parameter overrides this list of classes. "
      "So be sure to set that to false if you only want the listed classes.",
      "" )
  );
}
