set(test_exclusions)

if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "windows")
  list(APPEND test_exclusions
    # Some degenerate behavior in GoogleTest support.
    "^>$"

    # GoogleTest tests do not initialize Python properly. Exclude them pending
    # further investigation. See #7
    "^(ceres|core|ffmpeg|gdal|klv|mvg|ocv|pdal|qt|serialize-json|super3d|vital|vtk|vxl):"
    # This C++ test also doesn't initialize it properly.
    "^CXX::demo_python_impl_call$"

    # Plugins aren't found. Needs investigation.
    "^test-python-test_image_io$"
    )
endif ()

# format:
# Add one per line:
# "^name_of_test$"
list(APPEND test_exclusions
  )

string(REPLACE ";" "|" test_exclusions "${test_exclusions}")
if (test_exclusions)
  set(test_exclusions "(${test_exclusions})")
endif ()
