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

#include <python/kwiver/vital/algo/algorithm.txx>
#include <python/kwiver/vital/algo/trampoline/image_io_trampoline.txx>
#include <vital/algo/image_io.h>

namespace kwiver::vital::python {

namespace py = pybind11;

void
image_io( py::module& m )
{
  py::class_< kwiver::vital::algo::image_io,
    std::shared_ptr< kwiver::vital::algo::image_io >,
    kwiver::vital::algorithm,
    image_io_trampoline<> > instance( m,  "ImageIO" );

  instance
    .def( py::init<>() )
    .def_static(
      "interface_name",
      &kwiver::vital::algo::image_io::interface_name )
    .def(
      "load", &kwiver::vital::algo::image_io::load,
      R"( Load image from the file

 \throws kwiver::vital::path_not_exists Thrown when the given path does
 not
 exist.

 \throws kwiver::vital::path_not_a_file Thrown when the given path does
    not point to a file (i.e. it points to a directory).

 \param filename the path to the file to load
 \returns an image container refering to the loaded image)",
      py::arg( "filename" ) )
    .def(
      "save", &kwiver::vital::algo::image_io::save,
      R"( Save image to a file

 Image file format is based on file extension.

 \throws kwiver::vital::path_not_exists Thrown when the expected
    containing directory of the given path does not exist.

 \throws kwiver::vital::path_not_a_directory Thrown when the expected
    containing directory of the given path is not actually a
    directory.

 \param filename the path to the file to save
 \param data the image container refering to the image to write)",
      py::arg( "filename" ), py::arg( "data" ) )
    .def(
      "load_metadata", &kwiver::vital::algo::image_io::load_metadata,
      R"( Get the image metadata

 \throws kwiver::vital::path_not_exists Thrown when the given path does
 not
 exist.

 \throws kwiver::vital::path_not_a_file Thrown when the given path does
    not point to a file (i.e. it points to a directory).

 \param filename the path to the file to read
 \returns pointer to the loaded metadata)",
      py::arg( "filename" ) )
    .def(
      "get_implementation_capabilities",
      &kwiver::vital::algo::image_io::get_implementation_capabilities,
      R"( \brief Return capabilities of concrete implementation.

 This method returns the capabilities for the current image reader/writer.

 \return Reference to supported image capabilities.)" )
    .def_readonly_static( "HAS_TIME", &kwiver::vital::algo::image_io::HAS_TIME )
  ;
  register_algorithm< kwiver::vital::algo::image_io >( instance );
}

} // namespace kwiver::vital::python

#endif
