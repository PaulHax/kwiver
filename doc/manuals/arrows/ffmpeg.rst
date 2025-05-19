.. _arrows_ffmpeg:

FFMPEG
======

The FFMPEG arrow provides importing and exporting video data in KWIVER using
the `FFmpeg <https://ffmpeg.org/>`_ library. This includes extracting individual
frames as images and extracting or writing metadata to  KLV (Key-Length-Value)
data streams of the video. The encoding and decoding of this data is handled by
the :ref:`arrows_klv` arrow. This arrow can be built by enabling the
KWIVER_ENABLE_FFMPEG CMake flag.

How to Use
----------

The primary way ot use the FFMPEG arrow is via the :ref:`ffmpeg_video_input` and
:ref:`ffmpeg_video_input_clip` implementations of the
:ref:`vital_video_input <vital_video_input>` interface. Import methods for this
interface include the ``open`` method used to open a vide for reading. The
```next_frame``` and ``seek_frame``` allow navigation of the video and the
``frame_image`` and ``frame_metadata`` can extract data from the current frame.

The arrow also includes an implementation of the
:ref:`vital_video_output <vital_video_output>` interface which can write out
videos and the :ref:`vital_image_io <vital_image_io>` interface which allows
reading and writing frame images without relying on an implementation from
another arrow.

Algorithm Implementations
-------------------------

.. _ffmpeg_video_input:
ffmpeg_video_input
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
..  doxygenclass:: kwiver::arrows::ffmpeg::ffmpeg_video_input
    :project: kwiver
    :members:

.. _ffmpeg_video_input_clip:
ffmpeg_video_input_clip
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
..  doxygenclass:: kwiver::arrows::ffmpeg::ffmpeg_video_input_clip
    :project: kwiver
    :members:

.. _ffmpeg_video_input_rewire:
ffmpeg_video_input_rewire
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
..  doxygenclass:: kwiver::arrows::ffmpeg::ffmpeg_video_input_rewire
    :project: kwiver
    :members:

.. _ffmpeg_image_io:
ffmpeg_image_io
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
..  doxygenclass:: kwiver::arrows::ffmpeg::ffmpeg_image_io
    :project: kwiver
    :members:

.. _ffmpeg_video_output:
ffmpeg_video_output
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
..  doxygenclass:: kwiver::arrows::ffmpeg::ffmpeg_video_output
    :project: kwiver
    :members:
