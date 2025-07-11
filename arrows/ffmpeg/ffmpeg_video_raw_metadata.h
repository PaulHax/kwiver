// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// Declaration of FFmpeg video raw metadata.

#ifndef KWIVER_ARROWS_FFMPEG_FFMPEG_VIDEO_RAW_METADATA_H_
#define KWIVER_ARROWS_FFMPEG_FFMPEG_VIDEO_RAW_METADATA_H_

#include <arrows/ffmpeg/ffmpeg_util.h>
#include <arrows/ffmpeg/kwiver_algo_ffmpeg_export.h>

#include <arrows/klv/klv_stream_settings.h>

#include <vital/types/video_raw_metadata.h>

#include <list>

namespace kwiver {

namespace arrows {

namespace ffmpeg {

// ----------------------------------------------------------------------------
struct KWIVER_ALGO_FFMPEG_EXPORT ffmpeg_video_raw_metadata
  : public vital::video_raw_metadata
{
  ffmpeg_video_raw_metadata();

  ffmpeg_video_raw_metadata( ffmpeg_video_raw_metadata const& ) = delete;
  ffmpeg_video_raw_metadata&
  operator=( ffmpeg_video_raw_metadata const& ) = delete;

  struct packet_info
  {
    packet_info();

    packet_uptr packet;
    klv::klv_stream_settings stream_settings;
  };

  std::list< packet_info > packets;
};

using ffmpeg_video_raw_metadata_sptr =
  std::shared_ptr< ffmpeg_video_raw_metadata >;

} // namespace ffmpeg

} // namespace arrows

} // namespace kwiver

#endif
