// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

// Generated using: ./scripts/cpp_to_pybind11.py -i ./vital/algo/image_io.h -o
// image_io -I . ../build/ ../../fletch/build/install/include/eigen3
// ../../fletch/build/install/include -d kwiver::vital::algo::image_io -n
// ImageIO -v -t
#ifndef KWIVER_PYTHON_VITAL_ALGO_IMAGE_IO_H
#define KWIVER_PYTHON_VITAL_ALGO_IMAGE_IO_H

#include <pybind11/pybind11.h>

namespace kwiver::vital::python {

namespace py = pybind11;

void image_io( py::module& m );

} // namespace kwiver::vital::python

#endif
