// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include "hashed_image_classifier.h"
#include "hashed_image_classifier_filter.h"

#include <arrows/vxl/image_container.h>

#include <vital/config/config_block_io.h>

#include <vital/vital_config.h>

#include <vil/vil_convert.h>
#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
#include <vil/vil_plane.h>

#include <limits>
#include <type_traits>

#include <cstdlib>

namespace kwiver {

namespace arrows {

namespace vxl {

// ----------------------------------------------------------------------------
/// Private implementation class
class hashed_image_classifier_filter::priv
{
public:
  priv( hashed_image_classifier_filter& parent ) : parent( parent )
  {}

  hashed_image_classifier_filter& parent;

  // Convert the type
  template < typename ipix_t > vil_image_view< ipix_t >
  convert( vil_image_view_base_sptr& view );

  // Scale and convert the image
  bool load_model();

  hashed_image_classifier< vxl_byte, double > hashed_classifier;
  bool model_loaded{ false };

  double
  c_offset() const { return parent.c_offset; }
  const std::string&
  c_model_file() const { return parent.c_model_file; }
};

// ----------------------------------------------------------------------------
bool
hashed_image_classifier_filter::priv
::load_model()
{
  if( !model_loaded )
  {
    auto const& model_paths = vital::find_config_file( c_model_file() );
    if( model_paths.empty() )
    {
      LOG_ERROR(
        parent.logger(),
        "Could not locate \"" << c_model_file() << "\" model" );
      return false;
    }

    if( !hashed_classifier.load_from_file( model_paths.front() ) )
    {
      LOG_ERROR(
        parent.logger(),
        "Could not load \"" << model_paths.front() << "\" model" );
      return false;
    }
    model_loaded = true;
  }
  return true;
}

// ----------------------------------------------------------------------------
void
hashed_image_classifier_filter
::initialize()
{
  KWIVER_INITIALIZE_UNIQUE_PTR( priv, d );
  attach_logger( "arrows.vxl.hashed_image_classifier_filter" );
}

// ----------------------------------------------------------------------------
bool
hashed_image_classifier_filter
::check_configuration( VITAL_UNUSED vital::config_block_sptr config ) const
{
  return true;
}

// ----------------------------------------------------------------------------
kwiver::vital::image_container_sptr
hashed_image_classifier_filter
::filter( kwiver::vital::image_container_sptr image_data )
{
  // Perform Basic Validation
  if( !image_data )
  {
    LOG_ERROR( logger(), "Image pointer was null" );
    return kwiver::vital::image_container_sptr();
  }

  // Get input image
  vil_image_view_base_sptr view =
    vxl::image_container::vital_to_vxl( image_data->get_image() );

  if( !view )
  {
    LOG_ERROR( logger(), "Data contained in the image container is null" );
    return nullptr;
  }

  if( view->pixel_format() != VIL_PIXEL_FORMAT_BYTE )
  {
    LOG_ERROR( logger(), "Only byte images can be proccessed" );
    return nullptr;
  }

  if( !d->load_model() )
  {
    return nullptr;
  }

  vil_image_view< double > weight_image;

  d->hashed_classifier.classify_images( view, weight_image, d->c_offset() );

  return std::make_shared< vxl::image_container >( weight_image );
}

} // end namespace vxl

} // end namespace arrows

} // end namespace kwiver
