// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Interface for video_input

#ifndef VITAL_ALGO_VIDEO_INPUT_H_
#define VITAL_ALGO_VIDEO_INPUT_H_

#include <vital/algo/vital_algo_export.h>
#include <vital/vital_config.h>

#include <vital/algorithm_capabilities.h>

#include <vital/algo/algorithm.h>
#include <vital/types/image_container.h>
#include <vital/types/metadata.h>
#include <vital/types/metadata_map.h>
#include <vital/types/timestamp.h>
#include <vital/types/video_raw_image.h>
#include <vital/types/video_raw_metadata.h>
#include <vital/types/video_settings.h>
#include <vital/types/video_uninterpreted_data.h>

#include <string>
#include <vector>

namespace kwiver {

namespace vital {

namespace algo {

// ----------------------------------------------------------------------------
/// An abstract base class for reading videos
///
/// This class represents an abstract interface for reading
/// videos. Once the video is opened, the frames are returned in order.
///
/// Use cases:
/// ----------------------------------------------------------------------------
///
/// 1) Reading video from a directory of images.
///
/// 2) Reading video frames from a list of file names.
///
/// 3) Reading video from mpeg/video file (one of many formats) (e.g. FMV)
///
/// 4) Reading video from mpeg/video file (one of many formats) with
///    cropping (e.g. WAMI). This includes Providing geostationary
///    images by cropping to a specific region from an image. This may
///    result in no data if the geo region and image do not intersect.
///
/// 5) Reading video from network stream. (RTSP) This may result in
///    unexpected end of video conditions and network related
///    disruptions (e.g. missing frames, connection terminating, ...)
///
/// A note about the basic capabilities:
///
/// HAS_EOV - This capability is set to true if the video source can
///     determine end of video. This is usually the case if the video
///     is being read from a file, but may not be known if the video is
///     coming from a streaming source.
///
/// HAS_FRAME_NUMBERS - This capability is set to true if the video source
///     supplies frame numbers. If the video source specifies a frame
///     number, then that number is used when forming a time stamp. If
///     the video does not supply a frame number, the time stamp will
///     not have a frame number.
///
/// HAS_FRAME_TIME - This capability is set to true if the video source
///     supplies a frame time. If a frame time is supplied, it is made
///     available in the time stamp for that frame. If the frame time
///     is not supplied, then the timestamp will not have the time set.
///
/// HAS_FRAME_DATA - This capability is set to true if the video source
///     supplies frame images. It may seem strange for a video input
///     algorithm to not supply image data, but happens with a reader
///     that only supplies the metadata.
///
/// HAS_ABSOLUTE_FRAME_TIME - This capability is set to true if the
///     video source supplies an absolute, rather than relative frame
///     time. This capability is not set if an absolute frame time can
///     not be found, or if the absolute frame time is configured as
///     "none".
///
/// HAS_METADATA - This capability is set if the video source supplies
///     some type of metadata. The metadata could be in 0601 or 0104 data
///     formats or a different source.
///
/// HAS_TIMEOUT - This capability is set if the implementation supports the
///     timeout parameter on the next_frame() method.
///
/// IS_SEEKABLE - This capability is set if the video source can seek to a
///      specific frame.
///
/// All implementations \b must support the basic traits, in that they
/// are registered with a \b true or \b false value. Additional
/// implementation specific (extended) traits may be added. The
/// application should first check to see if a extended trait is
/// registered by calling has_trait() since the actual implementation
/// is set by a configuration entry and not directly known by the
/// application.
///
/// Extended capabilities can be created to publish capabilities of
/// non-standard video sources. These capabilities should be namespaced using
/// the name (or abbreviation) of the concrete algorithm followed by
/// the abbreviation of the capability.
class VITAL_ALGO_EXPORT video_input
  : public kwiver::vital::algorithm
{
public:
  // Common capabilities
  // -- basic capabilities --
  static const algorithm_capabilities::capability_name_t HAS_EOV; // has end of
                                                                  // video
                                                                  // indication
  static const algorithm_capabilities::capability_name_t HAS_FRAME_NUMBERS;
  static const algorithm_capabilities::capability_name_t HAS_FRAME_TIME;
  static const algorithm_capabilities::capability_name_t HAS_FRAME_DATA;
  static const algorithm_capabilities::capability_name_t HAS_FRAME_RATE;
  static const algorithm_capabilities::capability_name_t HAS_ABSOLUTE_FRAME_TIME;
  static const algorithm_capabilities::capability_name_t HAS_METADATA;
  static const algorithm_capabilities::capability_name_t HAS_TIMEOUT;
  static const algorithm_capabilities::capability_name_t IS_SEEKABLE;
  static const algorithm_capabilities::capability_name_t HAS_RAW_IMAGE;
  static const algorithm_capabilities::capability_name_t HAS_RAW_METADATA;
  static const algorithm_capabilities::capability_name_t HAS_UNINTERPRETED_DATA;

  video_input();
  PLUGGABLE_INTERFACE( video_input );

  /// \brief Open a video stream.
  ///
  /// This method opens the specified video stream for reading. The
  /// format of the name depends on the concrete implementation. It
  /// could be a file name or it could be a URI.
  ///
  /// Capabilities are set in this call, so they are available after.
  ///
  /// \param video_name Identifier of the video stream.
  ///
  /// \note Once a video is opened, it starts in an invalid state
  /// (i.e. before the first frame of video). You must call \c next_frame()
  /// to step to the first frame of video before calling \c frame_image().
  ///
  /// \throws exception if open failed
  virtual void open( std::string video_name ) = 0;

  /// \brief Close video stream.
  ///
  /// Close the currently opened stream and release resources.  Closing
  /// a stream that is already closed does not cause a problem.
  virtual void close() = 0;

  /// \brief Return end of video status.
  ///
  /// This method returns the end-of-video status of the input
  /// video. \b true is returned if the last frame has been returned.
  ///
  /// This method will always return \b false for video streams that have
  /// no ability to detect end of video, such as network streams.
  ///
  /// \return \b true if at end of video, \b false otherwise.
  virtual bool end_of_video() const = 0;

  /// \brief Check whether state of video stream is good.
  ///
  /// This method checks the current state of the video stream to see
  /// if it is good. A stream is good if it refers to a valid frame
  /// such that calls to \c frame_image() and \c frame_metadata()
  /// are expected to return meaningful data.  After calling \c open()
  /// the initial video state is not good until the first call to
  /// \c next_frame().
  ///
  /// \return \b true if video stream is good, \b false if not good.
  virtual bool good() const = 0; // like io stream API

  /// \brief Return whether video stream is seekable.
  ///
  /// This method returns whether the video stream is seekable.
  ///
  /// \return \b true if video stream is seekable, \b false otherwise.
  virtual bool seekable() const = 0;

  /// \brief Get the number of frames in the video stream.
  ///
  /// Get the number of frames available in the video stream.
  ///
  /// \return the number of frames in the video stream, or 0 if the video stream
  /// is not seekable.
  ///
  /// \throws video_stream_exception when there is an error in the video stream.
  virtual size_t num_frames() const = 0;

  /// \brief Advance to next frame in video stream.
  ///
  /// This method advances the video stream to the next frame, making
  /// the image and metadata available. The returned timestamp is for
  /// new current frame.
  ///
  /// The timestamp returned may be missing either frame number or time
  /// or both, depending on the actual implementation.
  ///
  /// Calling this method will make a new image and metadata packets
  /// available. They can be retrieved by calling frame_image() and
  /// frame_metadata().
  ///
  /// Check the HAS_TIMEOUT capability from the concrete implementation to
  /// see if the timeout feature is supported.
  ///
  /// If the video input is already an end, then calling this method
  /// will return \b false.
  ///
  /// \param[out] ts Time stamp of new frame.
  /// \param[in] timeout Number of seconds to wait. 0 = no timeout.
  ///
  /// \return \b true if frame returned, \b false if end of video.
  ///
  /// \throws video_input_timeout_exception when the timeout expires.
  /// \throws video_stream_exception when there is an error in the video stream.
  virtual bool next_frame(
    kwiver::vital::timestamp& ts,
    uint32_t timeout = 0 ) = 0;

  /// \brief Seek to the given frame number in video stream.
  ///
  /// This method seeks the video stream to the requested frame, making
  /// the image and metadata available. The returned timestamp is for
  /// new current frame.
  ///
  /// The timestamp returned may be missing the time.
  ///
  /// Calling this method will make a new image and metadata packets
  /// available. They can be retrieved by calling frame_image() and
  /// frame_metadata().
  ///
  /// Check the HAS_TIMEOUT capability from the concrete implementation to
  /// see if the timeout feature is supported.
  ///
  /// If the frame requested does not exist, then calling this method
  /// will return \b false.
  ///
  /// If the video input is not seekable then calling this method will return
  /// \b false.
  ///
  /// \param[out] ts Time stamp of new frame.
  /// \param[in] frame_number The frame to seek to.
  /// \param[in] timeout Number of seconds to wait. 0 = no timeout.
  ///
  /// \return \b true if frame returned, \b false if end of video.
  ///
  /// \throws video_input_timeout_exception when the timeout expires.
  /// \throws video_stream_exception when there is an error in the video stream.
  virtual bool seek_frame(
    kwiver::vital::timestamp& ts,
    kwiver::vital::timestamp::frame_t frame_number,
    uint32_t timeout = 0 ) = 0;

  /// \brief Obtain the time stamp of the current frame.
  ///
  /// This method returns the time stamp of the current frame, if any, or an
  /// invalid time stamp. The returned time stamp shall have the same value
  /// as was set by the most recent call to \c next_frame().
  ///
  /// This method is idempotent. Calling it multiple times without
  /// calling next_frame() will return the same time stamp.
  ///
  /// \return The time stamp of the current frame.
  virtual kwiver::vital::timestamp frame_timestamp() const = 0;

  /// \brief Get current frame from video stream.
  ///
  /// This method returns the image from the current frame.  If the
  /// video input is already an end, then calling this method will
  /// return a null pointer.
  ///
  /// This method is idempotent. Calling it multiple times without
  /// calling next_frame() will return the same image.
  ///
  /// \return Pointer to image container.
  ///
  /// \throws video_stream_exception when there is an error in the video stream.
  virtual kwiver::vital::image_container_sptr frame_image() = 0;

  /// Return implementation-defined data for efficiently copying this frame's
  /// image.
  ///
  /// Using this method can help avoid the loss of efficiency and fidelity that
  /// comes with re-encoding an image, if no changes to the image are to be
  /// performed before writing it back out. May return \c nullptr, indicating
  /// the reader does not support this operation.
  ///
  /// \return Pointer to raw image data.
  virtual video_raw_image_sptr raw_frame_image();

  /// \brief Get metadata collection for current frame.
  ///
  /// This method returns the metadata collection for the current
  /// frame. It is best to call this after calling next_frame() to make
  /// sure the metadata and video are synchronized and that no metadata
  /// collections are lost.
  ///
  /// Metadata typically occurs less frequently than video frames, so
  /// if you call next_frame() and frame_metadata() together while
  /// processing a video, there may be times where no metadata is
  /// returned. In this case an empty metadata vector will be returned.
  ///
  /// Also note that the metadata collection contains a timestamp that
  /// can be used to determine where the metadata fits in the video
  /// stream.
  ///
  /// In video streams without metadata (as determined by the stream
  /// capability), this method may return and empty vector, indicating no
  /// new metadata has been found.
  ///
  /// Calling this method at end of video will return an empty metadata
  /// vector.
  ///
  /// Metadata is returned as a vector, instead of a single object, to
  /// handle cases where there is multiple metadata packets between
  /// frames. This can happen in video streams with a fast metadata
  /// rate and slow frame rate. Multiple metadata objects can be also
  /// returned from video streams that contain metadata in multiple
  /// standards, such as MISB-601 and MISB-104.
  ///
  /// In cases where there are multiple metadata packets between
  /// frames, it is inappropriate for the reader to try to select the
  /// best metadata packet. That is why they are all returned.
  ///
  /// This method is idempotent. Calling it multiple times without
  /// calling next_frame() will return the same metadata.
  ///
  /// @return Vector of metadata pointers.
  ///
  /// \throws video_stream_exception when there is an error in the video stream.
  virtual kwiver::vital::metadata_vector frame_metadata() = 0;

  /// Return implementation-defined data for efficiently copying this frame's
  /// metadata.
  ///
  /// Using this method can help avoid the loss of efficiency and fidelity that
  /// comes with re-encoding metadata, if no changes to the metadata are to be
  /// performed before writing it back out. May return \c nullptr, indicating
  /// the reader does not support this operation.
  ///
  /// \return Pointer to raw metadata.
  virtual video_raw_metadata_sptr raw_frame_metadata();

  /// Return an implementation-defined representation of uninterpreted data in
  /// this frame.
  ///
  /// This method enables passage of miscellaneous data - such as audio,
  /// unrecognized metadata, or secondary image streams - to a video output when
  /// transcoding.
  ///
  /// \return Pointer to uninterpreted data.
#ifndef KWIVER_PYBIND11_WRAPPING
  virtual video_uninterpreted_data_sptr uninterpreted_frame_data();
#endif

  /// \brief Get metadata map for video.
  ///
  /// This method returns a metadata map for the video assuming the video is
  /// seekable. If the video is not seekable it will return an empty map.
  /// Depending on the implementation if the metamap has not been previously
  /// requested then the video will have to loop over to create and store the
  /// metadata map.
  ///
  /// In video streams without metadata (as determined by the stream
  /// capability), this method will return an empty map, indicating no
  /// metadata has been found.
  ///
  /// @return Map of vectors of metadata pointers.
  ///
  /// \throws video_stream_exception when there is an error in the video stream.
  virtual kwiver::vital::metadata_map_sptr metadata_map() = 0;

  /// \brief Get frame rate from the video.
  ///
  /// If frame rate is not supported, return -1.
  ///
  /// \return Frame rate.
  virtual double frame_rate();

  /// Extract implementation-specific video encoding settings.
  ///
  /// The returned structure is intended to be passed to a video encoder of
  /// similar implementation so that the output video can be encoded using the
  /// settings of the input video.
  ///
  /// \return Implementation video settings, or \c nullptr if none are needed.
#ifndef KWIVER_PYBIND11_WRAPPING
  virtual video_settings_uptr implementation_settings() const;
#endif

  /// \brief Return capabilities of concrete implementation.
  ///
  /// This method returns the capabilities for the currently opened
  /// video.
  ///
  /// \return Reference to supported video capabilities.
  algorithm_capabilities const& get_implementation_capabilities() const;

protected:
  void set_capability(
    algorithm_capabilities::capability_name_t const& name,
    bool val );

private:
  algorithm_capabilities m_capabilities;
};

/// Shared pointer type for generic video_input definition type.
typedef std::shared_ptr< video_input > video_input_sptr;

} // namespace algo

} // namespace vital

} // namespace kwiver

#endif // VITAL_ALGO_VIDEO_INPUT_H_
