// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#ifndef KWIVER_ARROWS_TRANSFER_BBOX_WITH_DEPTH_MAP_H_
#define KWIVER_ARROWS_TRANSFER_BBOX_WITH_DEPTH_MAP_H_

#include <arrows/core/kwiver_algo_core_export.h>

#include <vital/algo/detected_object_filter.h>
#include <vital/algo/image_io.h>
#include <vital/io/camera_io.h>

#include <vital/algo/algorithm.h>
#include <vital/algo/algorithm.txx>

using namespace kwiver::vital;

namespace kwiver {

namespace arrows {

namespace core {

/// Backproject image point to a depth map
KWIVER_ALGO_CORE_EXPORT
vector_3d
backproject_to_depth_map(
  kwiver::vital::camera_perspective_sptr const camera,
  kwiver::vital::image_container_sptr const depth_map,
  vector_2d const& img_pt );

/// Backproject an image point (top) assumed to be directly above another
KWIVER_ALGO_CORE_EXPORT
std::tuple< vector_3d, vector_3d >
backproject_wrt_height(
  kwiver::vital::camera_perspective_sptr const camera,
  kwiver::vital::image_container_sptr const depth_map,
  vector_2d const& img_pt_bottom,
  vector_2d const& img_pt_top );

/// Transfer a bounding box wrt two cameras and a depth map
KWIVER_ALGO_CORE_EXPORT
vital::bounding_box< double >
transfer_bbox_with_depth_map_stationary_camera(
  kwiver::vital::camera_perspective_sptr const src_camera,
  kwiver::vital::camera_perspective_sptr const dest_camera,
  kwiver::vital::image_container_sptr const depth_map,
  vital::bounding_box< double > const bbox );

/// Transforms detections based on source and destination cameras.
class KWIVER_ALGO_CORE_EXPORT transfer_bbox_with_depth_map
  : public vital::algo::detected_object_filter
{
public:
  PLUGGABLE_IMPL(
    transfer_bbox_with_depth_map,
    "Transforms detected object set bounding boxes based on source "
    "and destination cameras with respect the source cameras depth "
    "map.\n\n",
    PARAM_DEFAULT(
      src_camera_krtd_file_name, std::string,
      "Source camera KRTD file name path",
      "" ),
    PARAM_DEFAULT(
      dest_camera_krtd_file_name, std::string,
      "Destination camera KRTD file name path",
      "" ),
    PARAM_DEFAULT(
      src_camera_depth_map_file_name, std::string,
      "Source camera depth map file name path",
      "" ),
    PARAM(
      image_reader, std::shared_ptr< vital::algo::image_io >,
      "image_reader" )
  )

  /// Destructor
  virtual ~transfer_bbox_with_depth_map() noexcept;

  /// Constructor taking source and destination cameras directly
  transfer_bbox_with_depth_map(
    kwiver::vital::camera_perspective_sptr src_cam,
    kwiver::vital::camera_perspective_sptr dest_cam,
    kwiver::vital::image_container_sptr src_cam_depth_map );

  /// Check that the algorithm's currently configuration is valid
  virtual bool check_configuration( vital::config_block_sptr config ) const;

  /// Apply the transformation
  virtual vital::detected_object_set_sptr
  filter( vital::detected_object_set_sptr const input_set ) const;

protected:
  void initialize() override;
  void set_configuration_internal( vital::config_block_sptr config ) override;

private:
  class priv;

  kwiver::vital::camera_perspective_sptr src_camera;
  kwiver::vital::camera_perspective_sptr dest_camera;
  kwiver::vital::image_container_sptr depth_map;
};

} // namespace core

} // namespace arrows

}   // End namespace

#endif // KWIVER_ARROWS_TRANSFER_BBOX_WITH_DEPTH_MAP_H_
