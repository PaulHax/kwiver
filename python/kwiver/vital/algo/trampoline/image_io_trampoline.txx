// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

// Generated using: ./scripts/cpp_to_pybind11.py -i ./vital/algo/image_io.h -o
// image_io -I . ../build/ ../../fletch/build/install/include/eigen3
// ../../fletch/build/install/include -d kwiver::vital::algo::image_io -n
// ImageIO -v -t
#ifndef IMAGE_IO_TRAMPOLINE_TXX
#define IMAGE_IO_TRAMPOLINE_TXX

#include <pybind11/pybind11.h>
#include <python/kwiver/vital/algo/trampoline/algorithm_trampoline.txx>
#include <vital/algo/image_io.h>

namespace kwiver::vital::python {

template < class image_io_base = kwiver::vital::algo::image_io >
class image_io_trampoline
  : public algorithm_trampoline< image_io_base >
{
public:
  using algorithm_trampoline< image_io_base >::algorithm_trampoline;

  kwiver::vital::image_container_sptr
  load_( ::std::string const& filename ) const override
  {
    PYBIND11_OVERLOAD_PURE(
      kwiver::vital::image_container_sptr,
      kwiver::vital::algo::image_io,
      load_,
      filename
    );
  }

  void
  save_(
    ::std::string const& filename,
    ::kwiver::vital::image_container_sptr data ) const override
  {
    PYBIND11_OVERLOAD_PURE(
      void,
      kwiver::vital::algo::image_io,
      save_,
      filename, data
    );
  }

  kwiver::vital::metadata_sptr
  load_metadata_( ::std::string const& filename ) const override
  {
    PYBIND11_OVERLOAD_PURE(
      kwiver::vital::metadata_sptr,
      kwiver::vital::algo::image_io,
      load_metadata_,
      filename
    );
  }
}; // class

} // namespace

#endif
