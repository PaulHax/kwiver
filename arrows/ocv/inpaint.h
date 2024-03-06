// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#ifndef KWIVER_ARROWS_OCV_INPAINT_
#define KWIVER_ARROWS_OCV_INPAINT_

#include <arrows/ocv/kwiver_algo_ocv_export.h>

#include <vital/algo/merge_images.h>
#include <vital/util/enum_converter.h>

namespace kwiver {

namespace arrows {

namespace ocv {

/// OCV image inpainting process.
///
/// Replace pixels in the image specified by non-zero elements in the mask with
/// inpainted values estimated from surrounding pixels.
class KWIVER_ALGO_OCV_EXPORT inpaint
  : public vital::algo::merge_images
{
public:
  enum inpainting_method
  {
    METHOD_mask,
    METHOD_navier_stokes,
  };

  PLUGGABLE_IMPL(
    inpaint,
    "Inpaint pixels specified by non-zero mask values.",

    PARAM_DEFAULT(
      inpaint_method,
      std::string,
      "Inpainting method, possible values: "  +
      method_converter().element_name_string(),
      method_converter().to_string( METHOD_navier_stokes ) ),

    PARAM_DEFAULT(
      radius,
      float,
      "Radius parameter for the inpainting method",
      3.0 )

  );

  virtual ~inpaint();

  /// Check that the algorithm's currently configuration is valid
  bool check_configuration( vital::config_block_sptr config ) const override;

  /// Inpaint image based on locations specied in mask
  kwiver::vital::image_container_sptr merge(
    kwiver::vital::image_container_sptr image,
    kwiver::vital::image_container_sptr mask ) const override;

  ENUM_CONVERTER(
    method_converter, inpainting_method, { "mask", METHOD_mask },
    { "navier_stokes", METHOD_navier_stokes } )

private:
  void initialize() override;
};

} // namespace ocv

} // namespace arrows

} // namespace kwiver

#endif
