// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <arrows/core/algo/compute_association_matrix_from_features.h>
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
TEST ( compute_association_matrix_from_features, create )
{
  plugin_manager::instance().load_all_plugins();

  EXPECT_NE(
    nullptr,
    create_algorithm< algo::compute_association_matrix >( "from_features" ) );
}

// ----------------------------------------------------------------------------
TEST ( compute_association_matrix_from_features, default_config )
{
  EXPECT_PLUGGABLE_IMPL(
    compute_association_matrix_from_features,
    "Populate association matrix in tracking from detector features.",
    PARAM_DEFAULT(
      max_distance,
      double,
      "Maximum allowed pixel distance for matches. Is expressed "
      "in raw pixel distance.",
      -1.0 ),
    PARAM(
      filter,
      kwiver::vital::algo::detected_object_filter_sptr,
      "filter" )
  );
}
