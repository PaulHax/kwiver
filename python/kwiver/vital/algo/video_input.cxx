// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

// Generated using: ./scripts/cpp_to_pybind11.py -i ./vital/algo/video_input.h
// -o video_input -I . ../build/ ../../fletch/build/install/include/eigen3
// ../../fletch/build/install/include -d kwiver::vital::algo::video_input -e
// <pybind11/stl.h> -v
#ifndef KWIVER_PYTHON_VITAL_ALGO_VIDEO_INPUT_H
#define KWIVER_PYTHON_VITAL_ALGO_VIDEO_INPUT_H

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <python/kwiver/vital/algo/algorithm.txx>
#include <python/kwiver/vital/algo/trampoline/video_input_trampoline.txx>
#include <vital/algo/video_input.h>

namespace kwiver::vital::python {

namespace py = pybind11;

void
video_input( py::module& m )
{
  py::class_< kwiver::vital::algo::video_input,
    std::shared_ptr< kwiver::vital::algo::video_input >,
    kwiver::vital::algorithm,
    video_input_trampoline<> > instance( m,  "VideoInput" );

  instance
    .def( py::init<>() )
    .def_static(
      "interface_name",
      &kwiver::vital::algo::video_input::interface_name )
    .def(
      "open", &kwiver::vital::algo::video_input::open,
      R"( \brief Open a video stream.

 This method opens the specified video stream for reading. The
 format of the name depends on the concrete implementation. It
 could be a file name or it could be a URI.

 Capabilities are set in this call, so they are available after.

 \param video_name Identifier of the video stream.

 \note Once a video is opened, it starts in an invalid state
 (i.e. before the first frame of video). You must call \c next_frame()
 to step to the first frame of video before calling \c frame_image().

 \throws exception if open failed)",
      py::arg( "video_name" ) )
    .def(
      "close", &kwiver::vital::algo::video_input::close,
      R"( \brief Close video stream.

 Close the currently opened stream and release resources.  Closing
 a stream that is already closed does not cause a problem.)" )
    .def(
      "end_of_video", &kwiver::vital::algo::video_input::end_of_video,
      R"( \brief Return end of video status.

 This method returns the end-of-video status of the input
 video. \b true is returned if the last frame has been returned.

 This method will always return \b false for video streams that have
 no ability to detect end of video, such as network streams.

 \return \b true if at end of video, \b false otherwise.)" )
    .def(
      "good", &kwiver::vital::algo::video_input::good,
      R"( \brief Check whether state of video stream is good.

 This method checks the current state of the video stream to see
 if it is good. A stream is good if it refers to a valid frame
 such that calls to \c frame_image() and \c frame_metadata()
 are expected to return meaningful data.  After calling \c open()
 the initial video state is not good until the first call to
 \c next_frame().

 \return \b true if video stream is good, \b false if not good.)" )
    .def(
      "seekable", &kwiver::vital::algo::video_input::seekable,
      R"( \brief Return whether video stream is seekable.

 This method returns whether the video stream is seekable.

 \return \b true if video stream is seekable, \b false otherwise.)" )
    .def(
      "num_frames", &kwiver::vital::algo::video_input::num_frames,
      R"( \brief Get the number of frames in the video stream.

 Get the number of frames available in the video stream.

 \return the number of frames in the video stream, or 0 if the video
 stream
 is not seekable.

 \throws video_stream_exception when there is an error in the video
 stream.)" )
    .def(
      "next_frame", &kwiver::vital::algo::video_input::next_frame,
      R"( \brief Advance to next frame in video stream.

 This method advances the video stream to the next frame, making
 the image and metadata available. The returned timestamp is for
 new current frame.

 The timestamp returned may be missing either frame number or time
 or both, depending on the actual implementation.

 Calling this method will make a new image and metadata packets
 available. They can be retrieved by calling frame_image() and
 frame_metadata().

 Check the HAS_TIMEOUT capability from the concrete implementation to
 see if the timeout feature is supported.

 If the video input is already an end, then calling this method
 will return \b false.

 \param[out] ts Time stamp of new frame.
 \param[in] timeout Number of seconds to wait. 0 = no timeout.

 \return \b true if frame returned, \b false if end of video.

 \throws video_input_timeout_exception when the timeout expires.
 \throws video_stream_exception when there is an error in the video
 stream.)",
      py::arg( "ts" ), py::arg( "timeout" ) = 0 )
    .def(
      "seek_frame", &kwiver::vital::algo::video_input::seek_frame,
      R"( \brief Seek to the given frame number in video stream.

 This method seeks the video stream to the requested frame, making
 the image and metadata available. The returned timestamp is for
 new current frame.

 The timestamp returned may be missing the time.

 Calling this method will make a new image and metadata packets
 available. They can be retrieved by calling frame_image() and
 frame_metadata().

 Check the HAS_TIMEOUT capability from the concrete implementation to
 see if the timeout feature is supported.

 If the frame requested does not exist, then calling this method
 will return \b false.

 If the video input is not seekable then calling this method will return
 \b false.

 \param[out] ts Time stamp of new frame.
 \param[in] frame_number The frame to seek to.
 \param[in] timeout Number of seconds to wait. 0 = no timeout.

 \return \b true if frame returned, \b false if end of video.

 \throws video_input_timeout_exception when the timeout expires.
 \throws video_stream_exception when there is an error in the video
 stream.)",
      py::arg( "ts" ), py::arg( "frame_number" ), py::arg( "timeout" ) = 0 )
    .def(
      "frame_timestamp", &kwiver::vital::algo::video_input::frame_timestamp,
      R"( \brief Obtain the time stamp of the current frame.

 This method returns the time stamp of the current frame, if any, or an
 invalid time stamp. The returned time stamp shall have the same value
 as was set by the most recent call to \c next_frame().

 This method is idempotent. Calling it multiple times without
 calling next_frame() will return the same time stamp.

 \return The time stamp of the current frame.)" )
    .def(
      "frame_image", &kwiver::vital::algo::video_input::frame_image,
      R"( \brief Get current frame from video stream.

 This method returns the image from the current frame.  If the
 video input is already an end, then calling this method will
 return a null pointer.

 This method is idempotent. Calling it multiple times without
 calling next_frame() will return the same image.

 \return Pointer to image container.

 \throws video_stream_exception when there is an error in the video
 stream.)" )
    .def(
      "raw_frame_image", &kwiver::vital::algo::video_input::raw_frame_image,
      R"( Return implementation-defined data for efficiently copying this frame's
 image.

 Using this method can help avoid the loss of efficiency and fidelity that
 comes with re-encoding an image, if no changes to the image are to be
 performed before writing it back out. May return \c nullptr, indicating
 the reader does not support this operation.

 \return Pointer to raw image data.)" )
    .def(
      "frame_metadata", &kwiver::vital::algo::video_input::frame_metadata,
      R"( \brief Get metadata collection for current frame.

 This method returns the metadata collection for the current
 frame. It is best to call this after calling next_frame() to make
 sure the metadata and video are synchronized and that no metadata
 collections are lost.

 Metadata typically occurs less frequently than video frames, so
 if you call next_frame() and frame_metadata() together while
 processing a video, there may be times where no metadata is
 returned. In this case an empty metadata vector will be returned.

 Also note that the metadata collection contains a timestamp that
 can be used to determine where the metadata fits in the video
 stream.

 In video streams without metadata (as determined by the stream
 capability), this method may return and empty vector, indicating no
 new metadata has been found.

 Calling this method at end of video will return an empty metadata
 vector.

 Metadata is returned as a vector, instead of a single object, to
 handle cases where there is multiple metadata packets between
 frames. This can happen in video streams with a fast metadata
 rate and slow frame rate. Multiple metadata objects can be also
 returned from video streams that contain metadata in multiple
 standards, such as MISB-601 and MISB-104.

 In cases where there are multiple metadata packets between
 frames, it is inappropriate for the reader to try to select the
 best metadata packet. That is why they are all returned.

 This method is idempotent. Calling it multiple times without
 calling next_frame() will return the same metadata.

 @return Vector of metadata pointers.

 \throws video_stream_exception when there is an error in the video
 stream.)" )
    .def(
      "raw_frame_metadata",
      &kwiver::vital::algo::video_input::raw_frame_metadata,
      R"( Return implementation-defined data for efficiently copying this frame's
 metadata.

 Using this method can help avoid the loss of efficiency and fidelity that
 comes with re-encoding metadata, if no changes to the metadata are to be
 performed before writing it back out. May return \c nullptr, indicating
 the reader does not support this operation.

 \return Pointer to raw metadata.)" )
    .def(
      "metadata_map", &kwiver::vital::algo::video_input::metadata_map,
      R"( \brief Get metadata map for video.

 This method returns a metadata map for the video assuming the video is
 seekable. If the video is not seekable it will return an empty map.
 Depending on the implementation if the metamap has not been previously
 requested then the video will have to loop over to create and store the
 metadata map.

 In video streams without metadata (as determined by the stream
 capability), this method will return an empty map, indicating no
 metadata has been found.

 @return Map of vectors of metadata pointers.

 \throws video_stream_exception when there is an error in the video
 stream.)" )
    .def(
      "frame_rate", &kwiver::vital::algo::video_input::frame_rate,
      R"( \brief Get frame rate from the video.

 If frame rate is not supported, return -1.

 \return Frame rate.)" )
    .def(
      "get_implementation_capabilities",
      &kwiver::vital::algo::video_input::get_implementation_capabilities,
      R"( \brief Return capabilities of concrete implementation.

 This method returns the capabilities for the currently opened
 video.

 \return Reference to supported video capabilities.)" )
    .def_readonly_static(
      "HAS_EOV",
      &kwiver::vital::algo::video_input::HAS_EOV )
    .def_readonly_static(
      "HAS_FRAME_NUMBERS",
      &kwiver::vital::algo::video_input::HAS_FRAME_NUMBERS )
    .def_readonly_static(
      "HAS_FRAME_TIME",
      &kwiver::vital::algo::video_input::HAS_FRAME_TIME )
    .def_readonly_static(
      "HAS_FRAME_DATA",
      &kwiver::vital::algo::video_input::HAS_FRAME_DATA )
    .def_readonly_static(
      "HAS_FRAME_RATE",
      &kwiver::vital::algo::video_input::HAS_FRAME_RATE )
    .def_readonly_static(
      "HAS_ABSOLUTE_FRAME_TIME",
      &kwiver::vital::algo::video_input::HAS_ABSOLUTE_FRAME_TIME )
    .def_readonly_static(
      "HAS_METADATA",
      &kwiver::vital::algo::video_input::HAS_METADATA )
    .def_readonly_static(
      "HAS_TIMEOUT",
      &kwiver::vital::algo::video_input::HAS_TIMEOUT )
    .def_readonly_static(
      "IS_SEEKABLE",
      &kwiver::vital::algo::video_input::IS_SEEKABLE )
    .def_readonly_static(
      "HAS_RAW_IMAGE",
      &kwiver::vital::algo::video_input::HAS_RAW_IMAGE )
    .def_readonly_static(
      "HAS_RAW_METADATA",
      &kwiver::vital::algo::video_input::HAS_RAW_METADATA )
    .def_readonly_static(
      "HAS_UNINTERPRETED_DATA",
      &kwiver::vital::algo::video_input::HAS_UNINTERPRETED_DATA )
  ;
  register_algorithm< kwiver::vital::algo::video_input >( instance );
}

} // namespace kwiver::vital::python

#endif
