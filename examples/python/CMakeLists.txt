if (KWIVER_ENABLE_PYTHON_TESTS)
   include("${CMAKE_SOURCE_DIR}/CMake/kwiver-setup-tests-python.cmake")

  set( __testable_modnames
    how_to_part_01_images
    )

  if (KWIVER_ENABLE_KPF)
    list( APPEND __testable_modnames how_to_part_02_detections )
  endif()

  set( __all_modnames
    __init__
    ${__testable_modnames}
    )

  if (KWIVER_ENABLE_FFMPEG)
    list( APPEND __all_modnames example_video_frames_metadata )
  endif()

  set( _test_data
    "../images/cat.jpg"
    "../images/soda_circles.jpg"
    )

  foreach( modname IN LISTS __all_modnames )
    kwiver_add_python_module( ${CMAKE_CURRENT_SOURCE_DIR}/${modname}.py
      examples/tests
      ${modname}
      )
  endforeach()

  foreach( test IN LISTS __testable_modnames )
    kwiver_add_pytest( "${CMAKE_CURRENT_SOURCE_DIR}/${test}" python)
  endforeach()

  foreach( file_name IN LISTS _test_data )
    kwiver_python_add_test_data(${file_name} ${kwiver_test_working_path})
  endforeach()
endif()
