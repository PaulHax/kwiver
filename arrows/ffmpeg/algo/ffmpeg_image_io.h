// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// Declaration of the FFmpeg-based image_io algorithm.

#ifndef KWIVER_ARROWS_FFMPEG_FFMPEG_IMAGE_IO_H_
#define KWIVER_ARROWS_FFMPEG_FFMPEG_IMAGE_IO_H_

#include <arrows/ffmpeg/kwiver_algo_ffmpeg_export.h>
#include <vital/algo/algorithm.txx>
#include <vital/algo/image_io.h>

#include <memory>

namespace kwiver {

namespace arrows {

namespace ffmpeg {

// ----------------------------------------------------------------------------
/// Image reader / writer using FFmpeg (libav).
class KWIVER_ALGO_FFMPEG_EXPORT ffmpeg_image_io
  : public vital::algo::image_io
{
public:
  PLUGGABLE_IMPL(
    ffmpeg_image_io,
    "Use FFmpeg to read and write image files.",

    PARAM_DEFAULT(
      codec_name, std::string,
      "Name of FFmpeg codec to force usage of. Only effective when saving "
      "images.",
      ""
    ),

    PARAM_DEFAULT(
      quality, int,
      "Integer 2-31 controlling compression quality. Higher is lossier.",
      10
    ),

    PARAM_DEFAULT(
      approximate, bool,
      "When set to true, may use faster but inexact image processing.",
      false )
  )

  bool check_configuration( vital::config_block_sptr config ) const override;

  ~ffmpeg_image_io() override;

protected:
  void initialize() override;
  void set_configuration_internal( vital::config_block_sptr config ) override;

private:
  vital::image_container_sptr load_(
    std::string const& filename ) const override;

  void save_(
    std::string const& filename,
    kwiver::vital::image_container_sptr data ) const override;

  class impl;
  KWIVER_UNIQUE_PTR( impl, d );
};

} // namespace ffmpeg

} // namespace arrows

} // namespace kwiver

#endif
