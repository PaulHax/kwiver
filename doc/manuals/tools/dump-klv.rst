.. _tools_dump_klv:

========
dump-klv
========

This program displays the KLV metadata packets that are embedded in a video
file. The default behavior is to print the metadata to standard output for all
frames of the video. This can be restricted to a subset of frames. The metadata
can be saved to a separate file in either a ``csv`` format to two different
variations of a ``json`` formatted file.

TODO: Add examples of output?

The default configuration of this tool depends on algorithm implementations in
the :ref:`arrows_ffmpeg` and :ref:`arrows_mvg` arrows which will only available
if the KWIVER_ENABLE_FFMPEG and KWIVER_ENABLE_MVG CMake flags are enabled. If
you are planning on using the tool to dump frame images, the default
configuration depends on the :ref:`arrows_opencv` so this feature will only be
available if the KWIVER_ENABLE_OPENCV CMake flags are enabled.

.. code-block:: bash

  kwiver dump-klv       [options]  video-file

**Required arguments:**


  ``video-file``  - name of video file.

**Options are:**

  ``-h, --help``
    Display applet usage.

  ``-c, --config filename``
    Provide configuration file.

  ``-c, --config arg``
    Configuration file for tool.

  ``-o, --output arg``
    Dump configuration to file and exit.

  ``-l, --log arg``
    Log metadata to a file. This requires the JSON serialization plugin.
    The file is structured as an array of frames where each frame contains an
    array of metadata packets associated with that frame. Each packet is an
    array of metadata fields. Alternatively, the configuration file,
    dump_klv.conf, can be updated to use CSV instead.

  ``--frame-range expr``
    Frame range to process, indexed from 1. May be a single number or two
    numbers separated by a hyphen, either of which may be omitted to process
    from the start or to the end of the video, respectively, e.g.
    '5', '10-100', or '64-'.

  ``-f, --frames extension``
    Dump frames into the given image format.

  ``--frames-dir path``
    Directory in which to dump frames. Defaults to current directory.

  ``-d, --detail``
    Display a detailed description of the metadata.

  ``-q, --quiet``
    Do not show metadata. Overrides -d/--detail.

  ``-e, --exporter format``
    Choose the format of the exported KLV data. Current options are:
    csv, json, klv-json.

  ``--multithread``
    Use multithreading to accelerate encoding of frame images. Number of worker
    threads is not configurable at this time.

  ``--compress``
    Compress output file. Only available for klv-json.

**Default configuration**

.. literalinclude:: ../../../config/applets/dump_klv.conf
