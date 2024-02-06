// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#ifndef ARROWS_CORE_VIDEO_INPUT_IMAGE_LIST_H
#define ARROWS_CORE_VIDEO_INPUT_IMAGE_LIST_H

#include <vital/algo/video_input.h>
#include <vital/algo/image_io.h>

#include <arrows/core/kwiver_algo_core_export.h>

#include <vital/algo/algorithm.txx>

namespace kwiver {

namespace arrows {

namespace core {

// ----------------------------------------------------------------------------
/// \brief Video input using list of images.
///
/// This class implements a video input algorithm using a list of images
/// to simulate a video. Only the images are returned.
/// This algorithm produces no metadata.
///
/// Example config:
///   # select reader type
///   image_reader:type = vxl
class KWIVER_ALGO_CORE_EXPORT video_input_image_list
  : public vital::algo::video_input
{
public:
  PLUGGABLE_IMPL(video_input_image_list,
               "Read a list of images from a list of file names"
               " and presents them in the same way as reading a video."
               " The actual algorithm to read an image is specified"
               " in the \"image_reader\" config block."
               " Read an image list as a video stream.",
               PARAM_DEFAULT(path, std::string,
                  "Path to search for image file. "
                  "If a file name is not absolute, this list of directories is scanned "
                  "to find the file. The current directory '.' is automatically appended "
                  "to the end of the path. "
                  "The format of this path is the same as the standard path specification, "
                  "a set of directories separated by a colon (':')",
                  ""),
               PARAM_DEFAULT(allowed_extensions, std::string,
                  "Semicolon-separated list of allowed file extensions. "
                  "Leave empty to allow all file extensions.",
                  ""),
               PARAM_DEFAULT(sort_by_time, bool,
                  "Instead of accepting the input list as-is, sort the input file list "
                  "based on the timestamp metadata provided for the file.",
                  false),
               PARAM(image_reader, kwiver::vital::algo::image_io_sptr,
                  "Algorithm to use for reading the images")
                  );


  video_input_image_list();
  virtual ~video_input_image_list();

  /// Check that the algorithm's currently configuration is valid.
  bool check_configuration( vital::config_block_sptr config ) const override;

  /// \brief Open a list of images.
  ///
  /// This method opens the file that contains the list of images. Each
  /// image verified to exist at this time.
  ///
  /// \param list_name Name of file that contains list of images.
  void open( std::string list_name ) override;
  void close() override;

  bool end_of_video() const override;
  bool good() const override;
  bool seekable() const override;
  size_t num_frames() const override;

  bool next_frame( kwiver::vital::timestamp& ts,
                   uint32_t timeout = 0 ) override;

  bool seek_frame( kwiver::vital::timestamp& ts,
                   kwiver::vital::timestamp::frame_t frame_number,
                   uint32_t timeout = 0 ) override;

  kwiver::vital::timestamp frame_timestamp() const override;
  kwiver::vital::image_container_sptr frame_image() override;
  kwiver::vital::metadata_vector frame_metadata() override;
  kwiver::vital::metadata_map_sptr metadata_map() override;

protected:
  void initialize() override;
  void set_configuration_internal(vital::config_block_sptr in_config) override;
private:
  /// \brief Private implementation class.
  class priv;

  KWIVER_UNIQUE_PTR(priv,d);
};

} // namespace core

} // namespace arrows

} // namespace kwiver

#endif
