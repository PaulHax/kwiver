// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief OCV FAST feature detector wrapper implementation

#include "detect_features_FAST.h"

using namespace kwiver::vital;

namespace kwiver {

namespace arrows {

namespace ocv {

class detect_features_FAST::priv
{
public:
  /// Constructor
  priv( detect_features_FAST& parent )
    : parent( parent )
  {}

  /// Create a new FAST detector instance with the current parameter values
  cv::Ptr< cv::FastFeatureDetector >
  create() const
  {
#if KWIVER_OPENCV_VERSION_MAJOR < 3
    // 2.4.x version constructor
    return cv::Ptr< cv::FastFeatureDetector >(
      new cv::FastFeatureDetector( threshold(), nonmaxSuppression() )
    );
#else
    // 3.x version constructor
    return cv::FastFeatureDetector::create(
      threshold(), nonmaxSuppression(),
      cv::FastFeatureDetector::DetectorType( neighborhood_type() ) );
#endif
  }

  /// Update the parameters of the given detector with the currently set values
  void
  update( cv::Ptr< cv::FeatureDetector > detector ) const
  {
#if KWIVER_OPENCV_VERSION_MAJOR < 3
    detector->set( "threshold", threshold() );
    detector->set( "nonmaxSuppression", nonmaxSuppression() );
#else
    auto det = detector.dynamicCast< cv::FastFeatureDetector >();
    det->setThreshold( threshold() );
    det->setNonmaxSuppression( nonmaxSuppression() );
    det->setType(
      cv::FastFeatureDetector::DetectorType(
        neighborhood_type() ) );
#endif
  }

  /// Check config parameter values
  bool
  check_config(
    vital::config_block_sptr const& config,
    logger_handle_t const& logger ) const
  {
    bool valid = true;

#if KWIVER_OPENCV_VERSION_MAJOR >= 3
    // Check that the input integer is one of the valid enum values
    int nt = config->get_value< int >( "neighborhood_type" );
    if( !( nt == cv::FastFeatureDetector::TYPE_5_8 ||
           nt == cv::FastFeatureDetector::TYPE_7_12 ||
           nt == cv::FastFeatureDetector::TYPE_9_16 ) )
    {
      std::stringstream ss;
      ss << "FAST feature detector neighborhood type is not one of the valid "
            "values (see config comment). Given " << nt;
      LOG_ERROR( logger, ss.str() );
      valid = false;
    }
#endif

    return valid;
  }

  int&
  threshold() const { return parent.c_threshold; }
  bool
  nonmaxSuppression() const { return parent.get_nonmaxSuppression(); }

#if KWIVER_OPENCV_VERSION_MAJOR >= 4
  cv::FastFeatureDetector::DetectorType
  neighborhood_type() const
  {
    return cv::FastFeatureDetector::DetectorType(
      parent.get_neighborhood_type() );
  }
#else
  int
  neighborhood_type() const { return parent.get_neighborhood_type(); }
#endif

  int
  targetNumDetections() const
  {
    return parent.get_target_num_features_detected();
  }

  detect_features_FAST& parent;
};

/// Constructor
void
detect_features_FAST
::initialize()
{
  KWIVER_INITIALIZE_UNIQUE_PTR( priv, p_ );
  attach_logger( "arrows.ocv.detect_features_FAST" );
  detector = p_->create();
}

/// Destructor
detect_features_FAST
::~detect_features_FAST()
{}

void
detect_features_FAST
::update_detector_parameters() const
{
  p_->update( detector );
}

void
detect_features_FAST
::set_configuration_internal( VITAL_UNUSED vital::config_block_sptr in_config )
{
  this->update_detector_parameters();
}

bool
detect_features_FAST
::check_configuration( vital::config_block_sptr in_config ) const
{
  vital::config_block_sptr config = get_configuration();
  config->merge_config( in_config );
  return p_->check_config( config, logger() );
}

/// Extract a set of image features from the provided image
vital::feature_set_sptr
detect_features_FAST
::detect(
  vital::image_container_sptr image_data,
  vital::image_container_sptr mask ) const
{
  float close_detect_thresh = 0.1; // be within 10% of target
  const int duplicate_feat_count_thresh = 4;
  auto last_det_feat_set = ocv::detect_features::detect( image_data, mask );

  if( p_->targetNumDetections() <= 0 )
  {
    return last_det_feat_set;
  }

  if( last_det_feat_set->size() >
      ( 1.0 + close_detect_thresh ) * p_->targetNumDetections() )
  {
    // we got too many features
    int same_num_feat_count = 0;
    while( true )
    {
      auto last_threshold = p_->threshold();
      // make the threshold higher so we detect fewer features
      p_->threshold() *= ( 1.0 + close_detect_thresh );
      if( p_->threshold() == last_threshold )
      {
        p_->threshold() += 1;
      }
      p_->update( detector );

      // Update the wrapped algo inst with new parameters

      auto higher_thresh_feat_set = ocv::detect_features::detect(
        image_data,
        mask );

      if( last_det_feat_set->size() == higher_thresh_feat_set->size() )
      {
        ++same_num_feat_count;
      }
      else
      {
        same_num_feat_count = 0;
      }

      if( higher_thresh_feat_set->size() <=
          static_cast< size_t >( p_->targetNumDetections() ) ||
          same_num_feat_count > duplicate_feat_count_thresh )
      {
        // ok, we've crossed from too many to too few features
        // or we aren't changing the number of detected features much
        int higher_diff = abs(
          p_->targetNumDetections() -
          int( higher_thresh_feat_set->size() ) );
        int last_diff = abs(
          int( last_det_feat_set->size() ) -
          p_->targetNumDetections() );

        if( higher_diff < last_diff )
        {
          last_det_feat_set = higher_thresh_feat_set;
          // keep existing threshold. it worked.
        }
        else
        {
          // set threshold back to one used in last detection
          p_->threshold() = last_threshold;
          p_->update( detector );
        }
        break;
      }
      last_det_feat_set = higher_thresh_feat_set;
      last_threshold = p_->threshold();
    }
  }
  else
  {
    if( last_det_feat_set->size() <
        ( 1.0 - close_detect_thresh ) * p_->targetNumDetections() )
    {
      // we got too few features
      // or we aren't changing the number of detected features much
      int same_num_feat_count = 0;
      while( true )
      {
        auto last_threshold = p_->threshold();
        p_->threshold() *= ( 1.0 - close_detect_thresh );  // make the threshold
                                                           // higer so we detect
                                                           // fewer features
        if( p_->threshold() == last_threshold )
        {
          p_->threshold() -= 1;
        }
        if( p_->threshold() <= 0 )
        {
          // can't have a non-positive detection threshold.
          break;
        }
        p_->update( detector );

        auto lower_thresh_feat_set = ocv::detect_features::detect(
          image_data,
          mask );

        if( last_det_feat_set->size() == lower_thresh_feat_set->size() )
        {
          ++same_num_feat_count;
        }
        else
        {
          same_num_feat_count = 0;
        }

        if( lower_thresh_feat_set->size() >=
            static_cast< size_t >( p_->targetNumDetections() ) ||
            same_num_feat_count > duplicate_feat_count_thresh )
        {
          int lower_diff = abs(
            int( lower_thresh_feat_set->size() ) -
            p_->targetNumDetections() );
          int last_diff = abs(
            int( last_det_feat_set->size() ) -
            p_->targetNumDetections() );

          // ok, we've crossed from too few to too many features
          if( lower_diff < last_diff )
          {
            last_det_feat_set = lower_thresh_feat_set;
            // keep existing threshold. it worked.
          }
          else
          {
            // set threshold back to one used in last detection
            p_->threshold() = last_threshold;
            p_->update( detector );
          }
          break;
        }
        last_det_feat_set = lower_thresh_feat_set;
        last_threshold = p_->threshold();
      }
    }
  }

  return last_det_feat_set;
}

} // end namespace ocv

} // end namespace arrows

} // end namespace kwiver
