// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#ifndef KWIVER_ARROWS_OCV_RESECTION_CAMERA_OPTIONS_H_
#define KWIVER_ARROWS_OCV_RESECTION_CAMERA_OPTIONS_H_

#include <arrows/ocv/kwiver_algo_ocv_export.h>

#include <arrows/mvg/camera_options.h>

#include <vital/vital_config.h>

namespace kwiver {

namespace arrows {

namespace ocv {

struct KWIVER_ALGO_OCV_EXPORT resection_camera_options : public mvg::
                                                         camera_options
{
  using vectorf = std::vector< float >;
  resection_camera_options() : m_logger{ vital::get_logger(
                                 "arrows.ocv.resection_camera" ) }
  {}

  vital::logger_handle_t m_logger;
  // leave enogh of margin for inliers
  double reproj_accuracy = 16.0;
  // maximum number of iterations for camera calibration
  int max_iterations = 32;
  // focal length scales to optimize f*scale over
  vectorf focal_scales{ 1 };

  void get_configuration( vital::config_block_sptr config ) const override;
  void set_configuration( vital::config_block_sptr config ) override;
};

using resection_camera_options_sptr = std::shared_ptr< resection_camera_options >;

KWIVER_ALGO_OCV_EXPORT
std::ostream&
operator<<( std::ostream& s, resection_camera_options::vectorf const& v );

KWIVER_ALGO_OCV_EXPORT
std::istream&
operator>>( std::istream& s, resection_camera_options::vectorf& v );

} // namespace ocv

} // namespace arrows

} // namespace kwiver

#endif
