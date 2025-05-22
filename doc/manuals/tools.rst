Tools
=====

KWIVER provides command applet tools using the command line to leverage
the use of KWIVER and its capabilities without requiring any code to be written.
The available applets can be viewed with the terminal command ``kwiver help``.
Further help and commands to configure and run the kwiver applets can be
displayed with::
  # via a bash shell
  $ kwiver <applet> -h

Even more customization such as switching out algorithm implementations used in
the applet can be achieved by passing a config file to the applet. The base
configuration can produced from the applet with::
  # via a bash shell
  $ kwiver <applet> -o <config filename> [required args]

.. toctree::
   :maxdepth: 1

   :hidden:

  tools/bundle-adjust-tool
  tools/color-mesh
  tools/compare-klv
  tools/dump-klv
  tools/estimate-depth
  tools/explore-config
  tools/fuse-depth
  tools/init-cameras-landmarks
  tools/track-features

..
  The following tools require sprokit to be ported to v2
  tools/pipe-config.rst
  tools/pipe-to-dot.rst
  tools/pipeline_runner.rst
  tools/plugin_explorer.rst (these plugin methods are not valid)
