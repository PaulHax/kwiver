// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

// Generated using: ./scripts/cpp_to_pybind11.py -i
// ./vital/algo/metadata_map_io.h -o metadata_map_io -I . ../build/
// ../../fletch/build/install/include/eigen3 ../../fletch/build/install/include
// -d kwiver::vital::algo::metadata_map_io -n MetadataMapIO -v -t
#ifndef METADATA_MAP_IO_TRAMPOLINE_TXX
#define METADATA_MAP_IO_TRAMPOLINE_TXX

#include <pybind11/pybind11.h>
#include <python/kwiver/vital/algo/trampoline/algorithm_trampoline.txx>
#include <vital/algo/metadata_map_io.h>

namespace kwiver::vital::python {

template < class metadata_map_io_base = kwiver::vital::algo::metadata_map_io >
class metadata_map_io_trampoline
  : public algorithm_trampoline< metadata_map_io_base >
{
public:
  using algorithm_trampoline< metadata_map_io_base >::algorithm_trampoline;

  kwiver::vital::metadata_map_sptr
  load_( ::std::istream& fin, ::std::string const& filename ) const override
  {
    PYBIND11_OVERLOAD_PURE(
      kwiver::vital::metadata_map_sptr,
      kwiver::vital::algo::metadata_map_io,
      load_,
      fin, filename
    );
  }

  void
  save_(
    ::std::ostream& fout, ::kwiver::vital::metadata_map_sptr data,
    ::std::string const& filename ) const override
  {
    PYBIND11_OVERLOAD_PURE(
      void,
      kwiver::vital::algo::metadata_map_io,
      save_,
      fout, data, filename
    );
  }

  std::ios_base::openmode
  load_open_mode( ::std::string const& filename ) const override
  {
    PYBIND11_OVERLOAD_PURE(
      std::ios_base::openmode,
      kwiver::vital::algo::metadata_map_io,
      load_open_mode,
      filename
    );
  }

  std::ios_base::openmode
  save_open_mode( ::std::string const& filename ) const override
  {
    PYBIND11_OVERLOAD_PURE(
      std::ios_base::openmode,
      kwiver::vital::algo::metadata_map_io,
      save_open_mode,
      filename
    );
  }

  void
  set_configuration( ::kwiver::vital::config_block_sptr config ) override
  {
    PYBIND11_OVERLOAD(
      void,
      kwiver::vital::algo::metadata_map_io,
      set_configuration,
      config
    );
  }

  bool
  check_configuration(
    ::kwiver::vital::config_block_sptr config ) const override
  {
    PYBIND11_OVERLOAD(
      bool,
      kwiver::vital::algo::metadata_map_io,
      check_configuration,
      config
    );
  }
}; // class

} // namespace

#endif
