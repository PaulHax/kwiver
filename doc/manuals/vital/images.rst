Image Data Types and Related Algorithms
=======================================

.. _vital_image:

Image
----------

..  doxygenclass:: kwiver::vital::image
    :project: kwiver
    :members:

.. _vital_timestamp:

Time Stamp
----------

..  doxygenclass:: kwiver::vital::timestamp
    :project: kwiver
    :members:


.. _vital_image_container:

Image Container
--------------------

..  doxygenclass:: kwiver::vital::image_container
    :project: kwiver
    :members:


.. _vital_image_io:

Image I/O Algorithm
-------------------

Instantiate with: ::

   kwiver::vital::algo::image_io_sptr img_io = kwiver::vital::algo::image_io::create("<impl_name>");

============================== ====================== ========================
 Arrow & Configuration           <impl_name> options    CMake Flag to Enable
============================== ====================== ========================
:ref:`GDAL<gdal_image_io>`              gdal            KWIVER_ENABLE_GDAL
:ref:`FFmpeg<ffmpeg_image_io>`          ffmpeg          KWIVER_ENABLE_FFMPEG
:ref:`OpenCV<ocv_image_io>`             ocv             KWIVER_ENABLE_OPENCV
:ref:`VXL<vxl_image_io>`                vxl             KWIVER_ENABLE_VXL
============================== ====================== ========================

..  doxygenclass:: kwiver::vital::algo::image_io
    :project: kwiver
    :members:

.. _vital_convert_image:

Convert Image Algorithm
-----------------------

Instantiate with: ::

   kwiver::vital::algo::convert_image_sptr img_bypas = kwiver::vital::algo::convert_image::create("<impl_name>");

====================================== ====================== ========================
 Arrow & Configuration                  <impl_name> options    CMake Flag to Enable
====================================== ====================== ========================
:ref:`Core<core_convert_image_bypass>`  bypass                 KWIVER_ENABLE_ARROWS
====================================== ====================== ========================

..  doxygenclass:: kwiver::vital::algo::convert_image
    :project: kwiver
    :members:


.. _vital_image_filter:

Image Filter Algorithm
----------------------

Instantiate with: ::

   kwiver::vital::algo::image_filter_sptr img_filter = kwiver::vital::algo::image_filter::create("<impl_name>");

============================================== ==================================== ======================
 Arrow & Configuration                           <impl_name> options                 CMake Flag to Enable
============================================== ==================================== ======================
:ref:`VXL<vxl_aligned_edge_detection>`          vxl_aligned_edge_detection           KWIVER_ENABLE_VXL
:ref:`VXL<vxl_average_frames>`                  vxl_average                          KWIVER_ENABLE_VXL
:ref:`VXL<vxl_color_commonality_filter>`        vxl_color_commonality                KWIVER_ENABLE_VXL
:ref:`VXL<vxl_convert_image>`                   vxl_convert_image                    KWIVER_ENABLE_VXL
:ref:`VXL<vxl_hashed_image_classifier_filter>`  vxl_hashed_image_classifier_filter   KWIVER_ENABLE_VXL
:ref:`VXL<vxl_high_pass_filter>`                vxl_high_pass_filter                 KWIVER_ENABLE_VXL
:ref:`VXL<vxl_morphology>`                      vxl_morphology                       KWIVER_ENABLE_VXL
:ref:`VXL<vxl_pixel_feature_extractor>`         vxl_pixel_feature_extractor          KWIVER_ENABLE_VXL
:ref:`VXL<vxl_threshold>`                       vxl_threshold                        KWIVER_ENABLE_VXL
============================================== ==================================== ======================

..  doxygenclass:: kwiver::vital::algo::image_filter
    :project: kwiver
    :members:


.. _vital_split_image:

Split Image Algorithm
---------------------

Instantiate with: ::

   kwiver::vital::algo::split_image_sptr img_split = kwiver::vital::algo::split_image::create("<impl_name>");

============================== ===================== ========================
 Arrow & Configuration          <impl_name> options    CMake Flag to Enable
============================== ===================== ========================
:ref:`OpenCV<ocv_split_image>`          ocv            KWIVER_ENABLE_OPENCV
:ref:`VXL<vxl_split_image>`             vxl            KWIVER_ENABLE_VXL
============================== ===================== ========================

..  doxygenclass:: kwiver::vital::algo::split_image
    :project: kwiver
    :members:


.. _vital_video_input:

Video Input Algorithm
-----------------------

Instantiate with: ::

   kwiver::vital::algo::video_input_sptr img_bypas = kwiver::vital::algo::video_input::create("<impl_name>");

===================================================== ========================= ========================
 Arrow & Configuration                                 <impl_name> options       CMake Flag to Enable
===================================================== ========================= ========================
:ref:`FFMPEG<ffmpeg_video_input>`                      ffmpeg                    KWIVER_ENABLE_FFMPEG
:ref:`FFMPEG<ffmpeg_video_input_clip>`                 ffmpeg_clip               KWIVER_ENABLE_FFMPEG
:ref:`Core<core_video_input_buffered_metadata_filter>` buffered_metadata_filter  KWIVER_ENABLE_ARROWS
:ref:`Core<core_video_input_image_list>`               image_list                KWIVER_ENABLE_ARROWS
:ref:`Core<core_video_input_filter>`                   filter                    KWIVER_ENABLE_ARROWS
:ref:`Core<core_video_input_metadata_filter>`          metadata_filter           KWIVER_ENABLE_ARROWS
:ref:`Core<core_video_input_pos>`                      pos                       KWIVER_ENABLE_ARROWS
:ref:`Core<core_video_input_splice>`                   splice                    KWIVER_ENABLE_ARROWS
:ref:`Core<core_video_input_split>`                    split                     KWIVER_ENABLE_ARROWS
===================================================== ========================= ========================

..  doxygenclass:: kwiver::vital::algo::video_input
    :project: kwiver
    :members:

.. _vital_video_output:

Video Output Algorithm
-----------------------

Instantiate with: ::

   kwiver::vital::algo::video_output_sptr img_bypas = kwiver::vital::algo::video_output::create("<impl_name>");

===================================================== ========================= ========================
 Arrow & Configuration                                 <impl_name> options       CMake Flag to Enable
===================================================== ========================= ========================
:ref:`FFMPEG<ffmpeg_video_output>`                      ffmpeg                    KWIVER_ENABLE_FFMPEG
===================================================== ========================= ========================

..  doxygenclass:: kwiver::vital::algo::video_output
    :project: kwiver
    :members:

Code Example
------------

.. literalinclude:: ../../../examples/cpp/how_to_part_01_images.cxx
   :language: cpp
   :lines: 30-
