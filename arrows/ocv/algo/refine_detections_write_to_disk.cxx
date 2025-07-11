// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Implementation of OCV refine detections draw debugging algorithm

#include "refine_detections_write_to_disk.h"

#include <algorithm>
#include <deque>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <vital/exceptions/io.h>
#include <vital/util/string.h>
#include <vital/vital_config.h>

#include <kwiversys/SystemTools.hxx>

#include <arrows/ocv/image_container.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace kwiver::vital;

namespace kwiver {

namespace arrows {

namespace ocv {

using ST = kwiversys::SystemTools;

// Destructor
refine_detections_write_to_disk
::~refine_detections_write_to_disk()
{}

// ----------------------------------------------------------------------------
// Check that the algorithm's currently configuration is valid
bool
refine_detections_write_to_disk
::check_configuration( VITAL_UNUSED vital::config_block_sptr config ) const
{
  return true;
}

// ----------------------------------------------------------------------------
// Output images with tracked features drawn on them
vital::detected_object_set_sptr
refine_detections_write_to_disk
::refine(
  vital::image_container_sptr image_data,
  vital::detected_object_set_sptr detections ) const
{
  cv::Mat img = ocv::image_container::vital_to_ocv(
    image_data->get_image(),
    ocv::image_container::BGR_COLOR );

  // Get input filename if it's in the vital_metadata
  std::string filename;
  auto md = image_data->get_metadata();
  if( md )
  {
    if( auto& mdi = md->find( VITAL_META_IMAGE_URI ) )
    {
      // Get the full path, and then extract just the filename proper
      filename = ST::GetFilenameName( mdi.as_string() );
    }
  }

  for( auto det : *detections )
  {
    vital::bounding_box_d bbox = det->bounding_box();

    cv::Size s = img.size();
    vital::bounding_box_d bounds( vital::bounding_box_d::vector_type( 0, 0 ),
      vital::bounding_box_d::vector_type( s.width, s.height ) );

    // Clip detection box to image bounds.
    bbox = intersection( bounds, bbox );

    // Generate output filename
    std::string ofn = kwiver::vital::string_format(
      this->get_pattern(),
      this->id++, filename.c_str(),
      bbox.upper_left()[ 0 ], bbox.upper_left()[ 1 ],
      bbox.width(), bbox.height() );
    if( ofn.empty() )
    {
      LOG_ERROR(
        logger(),
        "Could not format output file name: \"" << this->get_pattern() <<
          "\"" );
      return detections;
    }

    // Output image to file
    // Make CV rect for out bbox coordinates
    cv::Rect r( bbox.upper_left()[ 0 ], bbox.upper_left()[ 1 ],
      bbox.width(), bbox.height() );

    cv::Mat crop = img( r );
    cv::imwrite( ofn, crop );
  } // end for

  return detections;
}

} // end namespace ocv

} // end namespace arrows

} // end namespace kwiver
