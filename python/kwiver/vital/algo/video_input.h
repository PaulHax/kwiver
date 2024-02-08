// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

// Generated using: ./scripts/cpp_to_pybind11.py -i ./vital/algo/video_input.h
// -o video_input -I . ../build/ ../../fletch/build/install/include/eigen3
// ../../fletch/build/install/include -d kwiver::vital::algo::video_input -e
// <pybind11/stl.h> -v
#ifndef KWIVER_PYTHON_VITAL_ALGO_VIDEO_INPUT_H
#define KWIVER_PYTHON_VITAL_ALGO_VIDEO_INPUT_H

#include <pybind11/pybind11.h>

namespace kwiver::vital::python {

namespace py = pybind11;

void video_input( py::module& m );

} // namespace kwiver::vital::python

#endif
