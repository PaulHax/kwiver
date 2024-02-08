// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Definition for PDAL point cloud writer

#ifndef KWIVER_ARROWS_PDAL_POINTCLOUD_IO_H_
#define KWIVER_ARROWS_PDAL_POINTCLOUD_IO_H_

#include <arrows/pdal/kwiver_algo_pdal_export.h>

#include <vital/algo/algorithm.h>
#include <vital/algo/algorithm.txx>

#include <vital/algo/pointcloud_io.h>
#include <vital/types/landmark_map.h>
#include <vital/types/local_geo_cs.h>

namespace kwiver {

namespace arrows {

namespace pdal {

class KWIVER_ALGO_PDAL_EXPORT pointcloud_io
  : public vital::algo::pointcloud_io
{
public:
  PLUGGABLE_IMPL(
    pointcloud_io,
    "Use PDAL to write and read point clouds."
  )

  bool
  check_configuration( vital::config_block_sptr config )
  const override { return true; }
  /// \endcond

  // is this something that needs to get updated?
  void
  set_local_geo_cs( vital::local_geo_cs const& lgcs ) { m_lgcs = lgcs; }

private:
  vital::local_geo_cs m_lgcs;

  kwiver::vital::pointcloud_d load_( vital::path_t const& filename ) const;

  void save_(
    vital::path_t const& filename,
    std::vector< vital::vector_3d > const& points,
    std::vector< vital::rgb_color > const& colors ) const;
};

} // end namespace pdal

} // end namespace arrows

} // end namespace kwiver

#endif // KWIVER_ARROWS_PDAL_POINTCLOUD_IO_H_
