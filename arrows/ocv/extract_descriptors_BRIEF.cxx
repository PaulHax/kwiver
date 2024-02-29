// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief OCV BRIEF descriptor extractor wrapper implementation

#include "extract_descriptors_BRIEF.h"

#if KWIVER_OPENCV_VERSION_MAJOR < 3 || defined( HAVE_OPENCV_XFEATURES2D )

#include <sstream>

#if KWIVER_OPENCV_VERSION_MAJOR < 3
typedef cv::BriefDescriptorExtractor cv_BRIEF_t;
#else
#include <opencv2/xfeatures2d.hpp>
typedef cv::xfeatures2d::BriefDescriptorExtractor cv_BRIEF_t;
#endif

using namespace kwiver::vital;

namespace kwiver {

namespace arrows {

namespace ocv {

namespace {

/// Create new algorithm instance using current parameter values
cv::Ptr< cv_BRIEF_t >
create( const extract_descriptors_BRIEF& parent )
{
#if KWIVER_OPENCV_VERSION_MAJOR < 3
  return cv::Ptr< cv_BRIEF_t >( new cv_BRIEF_t( parent.get_bytes() ) );
#else
  return cv_BRIEF_t::create(
    parent.get_bytes(),
    parent.get_use_orientation() );
#endif
}

} // namespace

void
extract_descriptors_BRIEF
::initialize()
{
  attach_logger( "arrows.ocv.BRIEF" );
  extractor = create( *this );
}

/// Destructor
extract_descriptors_BRIEF
::~extract_descriptors_BRIEF()
{}

bool
extract_descriptors_BRIEF
::check_configuration( vital::config_block_sptr in_config ) const
{
  vital::config_block_sptr config = get_configuration();
  config->merge_config( in_config );

  bool valid = true;

  // check that bytes param is one of the required 3 values
  int b = config->get_value< int >( "bytes" );
  if( !( b == 16 || b == 32 || b == 64 ) )
  {
    LOG_ERROR(
      logger(),
      "Bytes parameter must be either 16, 32 or 64. Given: " << b );
    valid = false;
  }

  return valid;
}

void
extract_descriptors_BRIEF
::update_extractor_parameters() const
{
#if KWIVER_OPENCV_VERSION_MAJOR < 3
  descriptor->set( "bytes", parent->get_bytes() );
#else
  this->extractor.constCast< cv::Feature2D >() = create( *this );
#endif
}

} // end namespace ocv

} // end namespace arrows

} // end namespace kwiver

#endif // has OCV support
