// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Implementation of create_detection_grid.h

#include "create_detection_grid.h"

#include <vital/types/bounding_box.h>
#include <vital/types/detected_object_set.h>

#include <vital/exceptions/algorithm.h>

namespace kwiver {

namespace arrows {

namespace core {

using namespace kwiver::vital;

/// Private implementation class
class create_detection_grid::priv
{
public:
  priv( create_detection_grid& parent )
    : parent( parent ),
      m_logger( vital::get_logger(
        "arrows.core.create_detection_grid" ) )
  {}

  create_detection_grid& parent;

  // Configuration values
  double c_width() { return parent.c_width; }
  double c_height() { return parent.c_height; }
  double c_x_step() { return parent.c_x_step; }
  double c_y_step() { return parent.c_y_step; }

  /// Logger handle
  vital::logger_handle_t m_logger;
};

// ------------------------------------------------
void
create_detection_grid
::initialize()
{
  KWIVER_INITIALIZE_UNIQUE_PTR( priv, d_ );
  attach_logger( "arrows.core.associate_detections_to_tracks_threshold" );
}

/// Destructor
create_detection_grid
::~create_detection_grid() noexcept
{}

bool
create_detection_grid
::check_configuration( vital::config_block_sptr config ) const
{
  if( config->get_value< double >( "detection_width" ) <= 0 || config->get_value< double >( "detection_width" ) <= 0 )
  {
    LOG_ERROR(
      d_->m_logger,
      "Detection width and height must be positive values" );
    return false;
  }
  if( config->get_value< double >( "x_step" ) <= 0 && config->get_value< double >( "y_step" ) <= 0 )
  {
    LOG_ERROR(d_->m_logger, "Detection steps must be positive values" );
    return false;
  }
  return true;
}

vital::detected_object_set_sptr
create_detection_grid
::detect( vital::image_container_sptr image_data ) const
{
  vital::detected_object_set_sptr grid( new vital::detected_object_set() );
  const size_t img_width = image_data->width();
  const size_t img_height = image_data->height();

  if( d_->c_width() > img_width || d_->c_height() > img_height )
  {
    VITAL_THROW(
      vital::algorithm_configuration_exception,
      this->interface_name(), this->plugin_name(),
      "Detection width and height must be no more than image width and height" );
  }

  // Get any non-overlapping grid spaces
  // Note that the last column and row are missing here
  for( int i = 0; i + d_->c_width() < img_width; i += d_->c_x_step() )
  {
    for( int j = 0; j + d_->c_height() < img_height; j += d_->c_y_step() )
    {
      vital::bounding_box< double > bbox( i, j, i + d_->c_width() - 1,
        j + d_->c_height() - 1 );
      vital::detected_object_sptr det_obj( new vital::detected_object( bbox ) );
      grid->add( det_obj );
    }
  }

  // Now get the bottom row
  for( int i = 0; i + d_->c_width() < img_width; i += d_->c_x_step() )
  {
    vital::bounding_box< double > bbox( i, img_height - d_->c_height(),
      i + d_->c_width() - 1, img_height - 1 );
    vital::detected_object_sptr det_obj( new vital::detected_object( bbox ) );
    grid->add( det_obj );
  }

  // Now get the bottom column
  for( int j = 0; j + d_->c_height() < img_height; j += d_->c_y_step() )
  {
    vital::bounding_box< double > bbox( img_width - d_->c_width(), j,
      img_width - 1,
      j + d_->c_height() - 1 );
    vital::detected_object_sptr det_obj( new vital::detected_object( bbox ) );
    grid->add( det_obj );
  }

  // Our last special case, the bottom right
  vital::bounding_box< double > bbox( img_width - d_->c_width(),
    img_height - d_->c_height(), img_width - 1, img_height - 1 );
  vital::detected_object_sptr det_obj( new vital::detected_object( bbox ) );
  grid->add( det_obj );

  return grid;
}

} // end namespace core

} // end namespace arrows

} // end namespace kwiver
