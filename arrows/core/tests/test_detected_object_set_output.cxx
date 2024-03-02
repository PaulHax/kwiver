// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <arrows/core/detected_object_set_output_csv.h>
#include <arrows/core/detected_object_set_output_kw18.h>
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
  using namespace kwiver::vital;

  plugin_manager::instance().load_all_plugins();

  EXPECT_NE(
    nullptr,
    create_algorithm< algo::detected_object_set_output >( "csv" ) );
}

// ----------------------------------------------------------------------------
TEST ( detected_object_set_output_kw18, create )
{
  using namespace kwiver::vital;

  plugin_manager::instance().load_all_plugins();

  EXPECT_NE(
    nullptr,
    create_algorithm< algo::detected_object_set_output >( "kw18" ) );
}
