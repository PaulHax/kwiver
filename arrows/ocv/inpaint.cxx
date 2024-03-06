// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include "inpaint.h"

#include <arrows/ocv/image_container.h>

#include <vital/vital_config.h>

#include <opencv2/core/core.hpp>
#include <opencv2/photo.hpp>

namespace kwiver {

namespace arrows {

namespace ocv {

// ----------------------------------------------------------------------------
void
inpaint
::initialize()
{
  attach_logger( "arrows.ocv.inpaint" );
}

// ----------------------------------------------------------------------------
inpaint::~inpaint()
{}

// ----------------------------------------------------------------------------
bool
inpaint
::check_configuration( vital::config_block_sptr config ) const
{
  auto const radius = config->get_value< float >( "radius" );
  if( radius <= 0 )
  {
    LOG_ERROR(
      logger(),
      "Radius should be positive but instead was " << radius );
    return false;
  }

  // try to convert string to known enum value
  try
  {
    const inpainting_method method =
      method_converter().from_string(
        config->get_value< std::string >(
          "method" ) );
    ( void ) ( method );
  }
  catch( const std::runtime_error& ex )
  {
    LOG_ERROR( logger(), ex.what() );
    return false;
  }

  return true;
}

// ----------------------------------------------------------------------------
kwiver::vital::image_container_sptr
inpaint
::merge(
  kwiver::vital::image_container_sptr image,
  kwiver::vital::image_container_sptr mask ) const
{
  auto const& cv_image = ocv::image_container::vital_to_ocv(
    image->get_image(), ocv::image_container::RGB_COLOR );
  auto const& cv_mask = ocv::image_container::vital_to_ocv(
    mask->get_image(), ocv::image_container::RGB_COLOR );
  auto cv_out = cv::Mat{};

  if( cv_image.size() != cv_mask.size() )
  {
    LOG_ERROR(
      logger(),
      "Image size " << cv_image.size() << " does not match mask size " <<
        cv_mask.size() );
    return image;
  }

  const inpainting_method method =
    method_converter().from_string( this->get_inpaint_method() );

  switch( method )
  {
    case METHOD_navier_stokes:
    {
      cv_out = cv::Mat::zeros( cv_image.size(), cv_image.type() );
      cv::inpaint(
        cv_image, cv_mask, cv_out, this->get_radius(),
        cv::INPAINT_NS );
      break;
    }
    case METHOD_mask:
    {
      cv::Mat zeros = cv::Mat::zeros( cv_image.size(), cv_image.type() );
      cv_out = cv_image.clone();
      zeros.copyTo( cv_out, cv_mask );
      break;
    }
    default:
    {
      LOG_ERROR( logger(), "Method not supported" );
    }
  }
  return std::make_shared< ocv::image_container >(
    cv_out, ocv::image_container::RGB_COLOR );
}

} // namespace ocv

} // namespace arrows

} // namespace kwiver
