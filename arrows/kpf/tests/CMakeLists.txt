project(arrows_test_kpf)

set(CMAKE_FOLDER "Arrows/KPF/Tests")

include(kwiver-test-setup)

set(test_libraries      vital vital_vpm kwiver_algo_kpf )

##############################
# Algorithms KPF tests
##############################
kwiver_discover_gtests(kpf kpf LIBRARIES ${test_libraries} ARGUMENTS "${kwiver_test_data_directory}")
