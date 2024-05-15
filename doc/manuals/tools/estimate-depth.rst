==============
estimate-depth
==============

The color-mesh tool colors an input-mesh from a video (or list of images) and a
list of camera files stored in a directory. A mesh colored with the
average color or with a color for a particular camera is produced.

kwiver estimate-depth       [options]
------------------------------

  video-source - name of input video file.

  input-cameras-dir - name of the directory containing the krtd camera
  files(default: results/krtd)

  output-depths-dir - name of the directory to write depth maps
  to (default: results/depth)

**Options are:**

  ``-h, --help``
    Display applet usage information.

  ``-c, --config arg``
    Configuration file for tool

  ``-o, --output-config arg``
    Output a configuration. This may be seeded with a configuration file from -c/--config.

  ``-f, --frame arg``
    The frame number to compute depth for.

  ``-l, --input-landmarks-file arg``
    3D sparse features

  ``-m, --mask-source arg``
    Masks of unusable regions
