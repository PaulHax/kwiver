==========
fuse-depth
==========

kwiver fuse-depth       [options]
--------------------------

  input-cameras-dir - name of the directory containing the krtd camera
  files(default: results/krtd)

  input-depths-dir - name of the directory to read depth maps from
  (default: results/depths)


**Options are:**

  ``-h, --help``
    Display applet usage information.

  ``-c, --config arg``
    Configuration file for tool.

  ``-o, --output-config arg``
    Output a configuration. This may be seeded with a configuration file from -c/--config.

  ``-l, --input-landmarks-file arg``
    3D sparse features (default: results/landmarks.ply)

  ``-g, --input-geo-origin-file arg``
    Input geographic origin file (default: results/geo_origin.txt)

  ``-m, --output-mesh-file arg``
    Write out isocontour mesh to file (default: results/mesh.vtp)

  ``-v, --output-volume-file arg``
    Write out integrated integrated depth data to file (default: results/volume.vti)

  ``-t, --isosurface-threshold arg``
    isosurface extraction threshold (default: 0.000000).
