==============
track-features
==============

This program tracks feature point through a video or list of images and
produces a track file and optional homography sequence.


track-features       [options] video-file [track-file]
------------------------------------------------------

  video-file  - name of input video file.

  track-file  - name of output track file (default: results/tracks.txt)


**Options are:**

  ``-h, --help``
    Display applet usage information.

  ``-c, --config arg``
    Configuration file for tool.

  ``-o, --output-config arg``
    Output a configuration. This may be seeded with a configuration file from -c/--config.

  ``-g, --homography-file arg``
    An output homography file containing a sequence of homographies aligning one
    frame to another estimated from the tracks.

  ``-m, --mask-file arg``
    An input mask video or list of mask images to indicate which pixels to ignore.
