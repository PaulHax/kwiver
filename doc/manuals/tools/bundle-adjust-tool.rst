==================
bundle-adjust-tool
==================

The bundle-adjust-tool optimizes cameras and landmarks via a bundle adjustment
algorithm.

.. code-block:: bash

  kwiver bundle-adjust-tool    [options]

**Options are:**

  ``-h, --help``
    Display applet usage information.

  ``-c, --config arg``
    Configuration file for tool.

  ``-p, --GCP arg``
    Input 3D Ground Control Points (GCP) with corresponding 2D
    Camera Registration Points (CRP) as JSON file.

  ``-v, --video arg``
    Input video file or image.txt list.

  ``-t, --tracks arg``
    Input tracks.txt

  ``-i, --cam_in arg``
    Input camera models.txt list.

  ``-k, --cam_out arg``
    Output directory for camera models.

  ``-l, --landmarks arg``
    Output landmarks.ply file.

  ``-g, --geo-origin arg``
    Output geographic origin file.

  ``-o, --output-config arg``
    Output a configuration, which may be seeded with a configuration file
    from -c/--config.
