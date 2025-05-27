===========
compare-klv
===========

This program prints differences found between the KLV in two files. These files
can be either video files with a KLV metadata stream or could be KLV in a json
file like the ones produced by the :ref:`tools_dump_klv` tool.

The default configuration of this tool depends on algorithm implementations in
the :ref:`arrows_ffmpeg` and :ref:`arrows_klv` arrows which will only available
if the KWIVER_ENABLE_FFMPEG and KWIVER_ENABLE_KLV CMake flags are enabled.

.. code-block:: bash

  kwiver compare-klv [options] lhs-file rhs-file

**Required arguments:**

  ``lhs-file`` - Left-hand-side video or JSON file for comparison.

  ``rhs-file`` - Right-hand-side video or JSON file for comparison.


**Options are:**

  ``-h, --help``                 Display applet usage.

  ``-c, --config filename``      Provide configuration file.

**Default configuration**

.. literalinclude:: ../../../config/applets/compare_klv.conf
