project(arrows_test_dbow2)

set(CMAKE_FOLDER "Arrows/DBOW2/Tests")

include(kwiver-test-setup)

set(test_libraries
  kwiver_algo_dbow2
  vil_algo)

kwiver_discover_gtests( dbow2 match_descriptor_sets LIBRARIES ${test_libraries} )
