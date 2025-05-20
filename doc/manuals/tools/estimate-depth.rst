==============
estimate-depth
==============

The estimate-depth tool estimates a series of depth maps for video frames based
off of known or calculated cameras associated with those frames.

.. code-block:: bash

  kwiver estimate-depth       [options]

**Required arguments:**

  ``video-source`` - name of input video file.

  ``input-cameras-dir`` - name of the directory containing the krtd camera
  files(default: results/krtd)

  ``output-depths-dir`` - name of the directory to write depth maps
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
