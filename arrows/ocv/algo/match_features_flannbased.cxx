// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief OCV flann-based feature matcher wrapper implementation

#include "match_features_flannbased.h"

namespace kwiver {

namespace arrows {

namespace ocv {

class match_features_flannbased::priv
{
public:
  /// Constructor
  priv( match_features_flannbased& parent )
    : parent( parent )
  {
    this->create();
  }

  // Can't currently update parameters on BF implementation, so no update
  // function. Will need to create a new instance on each parameter update.

  /// Create a new flann-based matcher instance and set our matcher param to it
  void
  create()
  {
    // cross version compatible
    if( binary_descriptors() )
    {
      matcher = cv::Ptr< cv::FlannBasedMatcher >(
        new cv::FlannBasedMatcher(
          cv::makePtr< cv::flann::LshIndexParams >( 12, 20, 2 ) ) );
    }
    else
    {
      matcher = cv::Ptr< cv::FlannBasedMatcher >( new cv::FlannBasedMatcher );
    }
  }

  /// Compute descriptor matching from 1 to 2 and from 2 to 1.
  ///
  /// Only return descriptor matches if the one of the top N
  /// matches from 1 to 2 is also a top N match from 2 to 1.
  /// Here N is defined by parameter cross_check_knn
  void
  cross_check_match(
    const cv::Mat& descriptors1,
    const cv::Mat& descriptors2,
    std::vector< cv::DMatch >& filtered_matches12 ) const
  {
    filtered_matches12.clear();

    std::vector< std::vector< cv::DMatch > > matches12, matches21;
    matcher->knnMatch(
      descriptors1, descriptors2,
      matches12, cross_check_k() );
    matcher->knnMatch(
      descriptors2, descriptors1,
      matches21, cross_check_k() );
    for( size_t m = 0; m < matches12.size(); m++ )
    {
      bool find_cross_check = false;
      for( size_t fk = 0; fk < matches12[ m ].size(); fk++ )
      {
        cv::DMatch forward = matches12[ m ][ fk ];

        for( size_t bk = 0; bk < matches21[ forward.trainIdx ].size(); bk++ )
        {
          cv::DMatch backward = matches21[ forward.trainIdx ][ bk ];
          if( backward.trainIdx == forward.queryIdx )
          {
            filtered_matches12.push_back( forward );
            find_cross_check = true;
            break;
          }
        }
        if( find_cross_check ) { break; }
      }
    }
  }

  /// Parameters
  bool
  cross_check() const { return parent.get_cross_check(); }
  unsigned
  cross_check_k() const { return parent.get_cross_check_k(); }
  bool
  binary_descriptors() const { return parent.get_binary_descriptors(); }

  cv::Ptr< cv::FlannBasedMatcher > matcher;

  match_features_flannbased& parent;
}; // end match_features_flannbased::priv

void
match_features_flannbased
::initialize()
{
  KWIVER_INITIALIZE_UNIQUE_PTR( priv, p_ );
  attach_logger( "arrows.ocv.match_features_flannbased" );
}

match_features_flannbased
::~match_features_flannbased()
{}

void
match_features_flannbased
::set_configuration_internal( VITAL_UNUSED vital::config_block_sptr in_config )
{
  p_->create();
}

bool
match_features_flannbased
::check_configuration( vital::config_block_sptr in_config ) const
{
  vital::config_block_sptr config = get_configuration();
  config->merge_config( in_config );

  bool valid = true;

  unsigned k = config->get_value< unsigned >( "cross_check_k" );
  if( k == 0 )
  {
    logger()->log_error( "Cross-check K value must be greater than 0." );
    valid = false;
  }

  return valid;
}

void
match_features_flannbased
::ocv_match(
  const cv::Mat& descriptors1, const cv::Mat& descriptors2,
  std::vector< cv::DMatch >& matches ) const
{
  if( p_->cross_check() )
  {
    p_->cross_check_match( descriptors1, descriptors2, matches );
  }
  else
  {
    p_->matcher->match( descriptors1, descriptors2, matches );
  }
}

} // end namespace ocv

} // end namespace arrows

} // end namespace kwiver
