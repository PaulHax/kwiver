// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#ifndef KWIVER_PYTHON_ARROW_SERIALIZE_JSON_SERIALIZE_TRACK_STATE_H_
#define KWIVER_PYTHON_ARROW_SERIALIZE_JSON_SERIALIZE_TRACK_STATE_H_

#include <pybind11/pybind11.h>

namespace py = pybind11;

namespace kwiver {

namespace arrows {

namespace python {

void serialize_track_state( py::module& m );

} // namespace python

} // namespace arrows

} // namespace kwiver

#endif
