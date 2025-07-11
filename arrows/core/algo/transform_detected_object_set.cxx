// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include "transform_detected_object_set.h"

#include <Eigen/Core>
#include <iostream>
#include <vital/config/config_difference.h>
#include <vital/io/camera_io.h>
#include <vital/types/bounding_box.h>
#include <vital/util/string.h>

using namespace kwiver::vital;

namespace kwiver {

namespace arrows {

namespace core {

// ----------------------------------------------------------------------------
class transform_detected_object_set::priv
{
public:
  priv( transform_detected_object_set& parent )
    : parent( parent )
  {}

  transform_detected_object_set& parent;

  // Configuration values
  std::string c_src_camera_krtd_file_name()
  { return parent.c_src_camera_krtd_file_name; }
  std::string c_dest_camera_krtd_file_name()
  { return parent.c_dest_camera_krtd_file_name; }
};

// ----------------------------------------------------------------------------
transform_detected_object_set
::transform_detected_object_set(
  kwiver::vital::camera_perspective_sptr src_cam,
  kwiver::vital::camera_perspective_sptr dest_cam )
  : src_camera( src_cam ),
    dest_camera( dest_cam )
{}

// ----------------------------------------------------------------------------
void
transform_detected_object_set
::initialize()
{}

// ----------------------------------------------------------------------------
transform_detected_object_set
::~transform_detected_object_set()
{}

// ----------------------------------------------------------------------------
void
transform_detected_object_set
::set_configuration_internal( vital::config_block_sptr config_in )
{
  vital::config_block_sptr config = this->get_configuration();

  config->merge_config( config_in );

  this->src_camera =
    kwiver::vital::read_krtd_file( this->src_camera_krtd_file_name );
  this->dest_camera =
    kwiver::vital::read_krtd_file( this->dest_camera_krtd_file_name );
}

// ----------------------------------------------------------------------------
bool
transform_detected_object_set
::check_configuration( vital::config_block_sptr config ) const
{
  kwiver::vital::config_difference cd( this->get_configuration(), config );
  const auto key_list = cd.extra_keys();

  if( !key_list.empty() )
  {
    LOG_WARN(
      logger(), "Additional parameters found in config block that are "
                "not required or desired: "
        << kwiver::vital::join( key_list, ", " ) );
  }

  return true;
}

// ----------------------------------------------------------------------------
vector_3d
transform_detected_object_set
::backproject_to_ground(
  kwiver::vital::camera_perspective_sptr const camera,
  vector_2d const& img_pt ) const
{
  auto ground_plane = vector_4d( 0, 0, 1, 0 );
  return this->backproject_to_plane( camera, img_pt, ground_plane );
}

// ----------------------------------------------------------------------------
vector_3d
transform_detected_object_set
::backproject_to_plane(
  kwiver::vital::camera_perspective_sptr const camera,
  vector_2d const& img_pt,
  vector_4d const& plane ) const
{
  // Back an image point to a specified world plane
  vector_2d npt_ = camera->intrinsics()->unmap( img_pt );
  auto npt = vector_3d( npt_( 0 ), npt_( 1 ), 1.0 );

  matrix_3x3d M = camera->rotation().matrix().transpose();

  auto n = vector_3d( plane( 0 ), plane( 1 ), plane( 2 ) );
  double d = plane( 3 );

  vector_3d Mt = M * camera->translation();
  vector_3d Mp = M * npt;

  return Mp * ( n.dot( Mt ) - d ) / n.dot( Mp ) - Mt;
}

// ----------------------------------------------------------------------------
Eigen::Matrix< double, 8, 3 >
transform_detected_object_set
::backproject_bbox(
  kwiver::vital::camera_perspective_sptr const camera,
  vital::bounding_box< double > const& bbox ) const
{
  // project base of the box to the ground
  auto base_pt = vector_2d( ( bbox.min_x() + bbox.max_x() ) / 2, bbox.max_y() );
  vector_3d pc = this->backproject_to_ground( camera, base_pt );
  vector_3d ray = pc - camera->center();
  ray( 2 ) = 0.0;
  ray.normalize();

  vector_3d p1 = this->backproject_to_ground(
    camera, vector_2d( bbox.min_x(), bbox.max_y() ) );
  vector_3d p2 = this->backproject_to_ground(
    camera, vector_2d( bbox.max_x(), bbox.max_y() ) );

  vector_3d vh = p2 - p1;
  vector_3d vd = ray * vh.norm();
  vh = vector_3d( -vd( 1 ), vd( 0 ), 0 );
  p1 = pc - vh / 2;
  p2 = pc + vh / 2;

  vector_3d p3 = p2 + vd;
  vector_3d p4 = p1 + vd;

  // TODO: Check that norm of vd is not equal to 0.0?

  vector_3d n = vd.normalized();
  double d = -( n.transpose() * p3 )( 0 );

  vector_4d back_plane;
  back_plane << n, d;

  vector_3d p5 = this->backproject_to_plane(
    camera, bbox.upper_left(), back_plane );
  double height = p5( 2 );

  Eigen::Matrix< double, 8, 3 > box3d;
  box3d << p1.transpose(),
    p2.transpose(),
    p3.transpose(),
    p4.transpose(),
    p1( 0 ), p1( 1 ), height,
    p2( 0 ), p2( 1 ), height,
    p3( 0 ), p3( 1 ), height,
    p4( 0 ), p4( 1 ), height;

  return box3d;
}

// ----------------------------------------------------------------------------
vital::bounding_box< double >
transform_detected_object_set
::box_around_box3d(
  kwiver::vital::camera_perspective_sptr const camera,
  Eigen::Matrix< double, 8, 3 > const& box3d ) const
{
  Eigen::Matrix< double, 8, 2 > projected_points;

  // project the points
  for( int i = 0; i < 8; i++ )
  {
    projected_points.row( i ) = camera->project( box3d.row( i ) ).transpose();
  }

  return vital::bounding_box< double >(
    ( vector_2d ) projected_points.colwise().minCoeff(),
    ( vector_2d ) projected_points.colwise().maxCoeff() );
}

// ----------------------------------------------------------------------------
vital::bounding_box< double >
transform_detected_object_set
::view_to_view(
  kwiver::vital::camera_perspective_sptr const src_camera_,
  kwiver::vital::camera_perspective_sptr const dest_camera_,
  vital::bounding_box< double > const& bbox ) const
{
  Eigen::Matrix< double, 8, 3 > box3d =
    this->backproject_bbox( src_camera_, bbox );

  return this->box_around_box3d( dest_camera_, box3d );
}

// ----------------------------------------------------------------------------
vital::bounding_box< double >
transform_detected_object_set
::transform_bounding_box( vital::bounding_box< double > const& bbox ) const
{
  return this->view_to_view(
    this->src_camera,
    this->dest_camera,
    bbox );
}

// ----------------------------------------------------------------------------
vital::detected_object_set_sptr
transform_detected_object_set
::filter( vital::detected_object_set_sptr const input_set ) const
{
  auto ret_set = std::make_shared< vital::detected_object_set >();

  // loop over all detections
  for( auto det : *input_set )
  {
    auto out_det = det->clone();
    auto out_box = out_det->bounding_box();
    auto new_out_box = this->transform_bounding_box( out_box );
    out_det->set_bounding_box( new_out_box );
    ret_set->add( out_det );
  } // end foreach detection

  return ret_set;
} // transform_detected_object_set::filter

} // namespace core

} // namespace arrows

}         // end namespace
