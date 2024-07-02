// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// Declaration of FFmpeg image conversion utilities.

#ifndef KWIVER_ARROWS_FFMPEG_FFMPEG_CONVERT_IMAGE_H_
#define KWIVER_ARROWS_FFMPEG_FFMPEG_CONVERT_IMAGE_H_

#include <arrows/ffmpeg/kwiver_algo_ffmpeg_export.h>

#include <arrows/ffmpeg/ffmpeg_util.h>

#include <vital/types/image_container.h>

namespace kwiver {

namespace arrows {

namespace ffmpeg {

// ----------------------------------------------------------------------------
// Create a vital image from a libav frame.
vital::image_container_sptr frame_to_vital_image(
  AVFrame* frame, sws_context_uptr* cached_sws = nullptr );

// ----------------------------------------------------------------------------
// Copy vital image data to a libav frame, attempting compatibility with
// codec_context if supplied.
frame_uptr vital_image_to_frame(
  vital::image_container_scptr const& image,
  AVCodecContext const* codec_context = nullptr,
  sws_context_uptr* cached_sws = nullptr );

// ----------------------------------------------------------------------------
// Determine which libav pixel format is closest to the vital image's layout.
AVPixelFormat vital_to_frame_pix_fmt(
  size_t depth,
  vital::image_pixel_traits const& traits,
  bool prefer_planar = false );

// ----------------------------------------------------------------------------
// Determine which vital-friendly libav pixel format a frame should be
// converted to.
//
// Any format returned here is guaranteed to return true for exactly one of
// `is_pix_fmt_planar()`, `is_pix_fmt_packed()`.
AVPixelFormat frame_to_vital_pix_fmt( AVPixelFormat src_fmt );

// ----------------------------------------------------------------------------
// Return true if the pixels are in separate channel planes.
// (RRR... GGG... BBB...)
bool is_image_planar( vital::image const& image );

// ----------------------------------------------------------------------------
// Return true if the pixel channels are densely interleaved.
// (RGBRGBRGB...)
bool is_image_packed( vital::image const& image );

// ----------------------------------------------------------------------------
// Return true if the format organizes pixels in separate channel planes.
bool is_pix_fmt_planar( AVPixelFormat pix_fmt );

// ----------------------------------------------------------------------------
// Return true if the format densely interleaves pixels.
bool is_pix_fmt_packed( AVPixelFormat pix_fmt );

} // namespace ffmpeg

} // namespace arrows

} // namespace kwiver

#endif
