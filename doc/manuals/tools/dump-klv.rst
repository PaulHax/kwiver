========
dump-klv
========

This program displays the KLV metadata packets that are embedded in a video file.

kwiver dump-klv       [options]  video-file
------------------------------------

  video-file  - name of video file.

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

  ``-f, --frames extension``
    Dump frames into the given image format.

  ``-f, --frames-dir path``
    Directory in which to dump frames. Defaults to current directory.

  ``-d, --detail``
    Display a detailed description of the metadata.

  ``-q, --quiet``
    Do not show metadata. Overrides -d/--detail.

  ``-e, --exporter format``
    Choose the format of the exported KLV data. Current options are:
    csv, json, klv-json.

  ``-e, --multithread``
    Use multithreading to accelerate encoding of frame images. Number of worker
    threads is not configurable at this time.

  ``-e, --compress``
    Compress output file. Only available for klv-json.
