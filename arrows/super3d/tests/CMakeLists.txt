project(arrows_test_super3d)

set(CMAKE_FOLDER "Arrows/super3d/Tests")

include(kwiver-test-setup)

set(test_libraries      kwiver_algo_vxl )

##############################
# Algorithms super3d tests
##############################
kwiver_discover_gtests(super3d compute_depth  LIBRARIES ${test_libraries} )
