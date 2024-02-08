set( THIS_MODULE vital/algo )

kwiver_add_python_module( ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py "${THIS_MODULE}" __init__ )

################################################################################
set(module_headers
  algorithm.h
  image_io.h
  metadata_map_io.h
  video_input.h

  trampoline/algorithm_trampoline.txx
  trampoline/image_io_trampoline.txx
  trampoline/metadata_map_io_trampoline.txx
  trampoline/video_input_trampoline.txx

)

set(module_sources
  algorithm_module.cxx

  algorithm.cxx
  image_io.cxx
  metadata_map_io.cxx
  video_input.cxx
)

# Python Module Library: _algos
kwiver_add_python_library(
  algos
  "${THIS_MODULE}"
  SOURCES ${module_headers}
          ${module_sources}
  PRIVATE pybind11::pybind11
          vital
          vital_config
          vital_algo
)
