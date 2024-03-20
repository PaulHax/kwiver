project(arrows_test_cuda)

set(CMAKE_FOLDER "Arrows/CUDA/Tests")

include(kwiver-test-setup)

set( test_libraries    kwiver_algo_cuda vital vital_vpm )

##############################
# Algorithms CUDA plugin tests
##############################
kwiver_discover_gtests(cuda integrate_depth_maps      LIBRARIES ${test_libraries} kwiver_algo_core)
