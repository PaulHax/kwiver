// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

// Generated using: ./scripts/cpp_to_pybind11.py -i
// ./vital/algo/metadata_map_io.h -o metadata_map_io -I . ../build/
// ../../fletch/build/install/include/eigen3 ../../fletch/build/install/include
// -d kwiver::vital::algo::metadata_map_io -n MetadataMapIO -v -t
#ifndef KWIVER_PYTHON_VITAL_ALGO_METADATA_MAP_IO_H
#define KWIVER_PYTHON_VITAL_ALGO_METADATA_MAP_IO_H

#include <pybind11/pybind11.h>

#include <python/kwiver/vital/algo/algorithm.txx>
#include <python/kwiver/vital/algo/trampoline/metadata_map_io_trampoline.txx>
#include <vital/algo/metadata_map_io.h>

namespace kwiver::vital::python {

namespace py = pybind11;

void
metadata_map_io( py::module& m )
{
  py::class_< kwiver::vital::algo::metadata_map_io,
    std::shared_ptr< kwiver::vital::algo::metadata_map_io >,
    kwiver::vital::algorithm,
    metadata_map_io_trampoline<> > instance( m,  "MetadataMapIO" );

  instance
    .def( py::init<>() )
    .def_static(
      "interface_name",
      &kwiver::vital::algo::metadata_map_io::interface_name )
    .def(
      "load",
      ( kwiver::vital::metadata_map_sptr ( kwiver::vital::algo::metadata_map_io
                                           ::* )(
          ::std::string const& ) const ) &
      kwiver::vital::algo::metadata_map_io::load,
      R"( Load metadata from the file.

 \throws kwiver::vital::path_not_exists
   Thrown when the given path does not exist.
 \throws kwiver::vital::path_not_a_file
   Thrown when the given path does not point to a file (i.e. it points to
   a directory).

 \param filename The path to the file to load.
 \returns A metadata_map_sptr pointing to the data.)",
      py::arg( "filename" ) )
    .def(
      "load",
      ( kwiver::vital::metadata_map_sptr ( kwiver::vital::algo::metadata_map_io
                                           ::* )(
          ::std::istream&,
          ::std::string const& ) const ) &
      kwiver::vital::algo::metadata_map_io::load,
      R"( Load metadata from the file.

 \param fin Stream to read from.
 \param filename Filename that generated the stream. May be used for
                 logging, or in case of other operations for which the
                 filesystem path to the input is relevant.
 \returns A metadata_map_sptr pointing to the data.)",
      py::arg( "fin" ), py::arg( "filename" ) = "stream" )
    .def(
      "save",
      ( void ( kwiver::vital::algo::metadata_map_io::* )(
        ::std::string const&,
        ::kwiver::vital::metadata_map_sptr ) const ) &
      kwiver::vital::algo::metadata_map_io::save,
      R"( Save metadata to a file.

 \throws kwiver::vital::path_not_exists
   Thrown when the expected containing directory of the given path does
   not exist.
 \throws kwiver::vital::path_not_a_directory
   Thrown when the expected containing directory of the given path is not
   actually a directory.

 \param filename The path to the file to save.
 \param data Pointer to the metadata to write.)",
      py::arg( "filename" ), py::arg( "data" ) )
    .def(
      "save",
      ( void ( kwiver::vital::algo::metadata_map_io::* )(
        ::std::ostream&,
        ::kwiver::vital::metadata_map_sptr,
        ::std::string const& ) const ) &
      kwiver::vital::algo::metadata_map_io::save,
      R"( Save metadata to a stream.

 \param fout Stream to write to.
 \param data Pointer to the metadata to write.
 \param filename Filename that generated the stream. May be used for
                 logging, or in case of other operations for which the
                 filesystem path to the input is relevant.)",
      py::arg( "fout" ), py::arg( "data" ), py::arg( "filename" ) = "stream" )
    .def(
      "get_implementation_capabilities",
      &kwiver::vital::algo::metadata_map_io::get_implementation_capabilities,
      R"( Return capabilities of concrete implementation.

 This method returns the capabilities for the current metadata
 reader/writer.

 \return Reference to supported algorithm capabilities.)" )
    .def(
      "set_configuration",
      &kwiver::vital::algo::metadata_map_io::set_configuration,
      py::arg( "config" ) )
    .def(
      "check_configuration",
      &kwiver::vital::algo::metadata_map_io::check_configuration,
      py::arg( "config" ) )
  ;
  register_algorithm< kwiver::vital::algo::metadata_map_io >( instance );
}

} // namespace kwiver::vital::python

#endif
