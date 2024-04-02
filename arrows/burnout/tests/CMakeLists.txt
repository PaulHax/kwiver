project(arrows_test_vxl)

set(CMAKE_FOLDER "Arrows/Burnout/Tests")

include(kwiver-test-setup)

set(test_libraries
  kwiver_algo_burnout
  )

##############################
# Algorithms VXL tests
##############################
kwiver_discover_gtests(burnout test_burnout_image_enhancer         LIBRARIES ${test_libraries})
# kwiver_discover_gtests(vxl morphology                     LIBRARIES ${test_libraries}  ARGUMENTS "${kwiver_test_data_directory}")
