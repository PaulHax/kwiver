.. _arrows_klv:

KLV
===

The KLV (Key-Length-Value) arrow provides decoding and encoding capabilities for
many common `MISB standards <https://nsgreg.nga.mil/misb.jsp>`_. This arrow does
not handle the muxing or demuxing of KLV streams in specific media formats; that
is handled in the :ref:`arrows_ffmpeg` arrow. This arrow can be built by
enabling the KWIVER_ENABLE_KLV CMake flag.

The guiding principles for this implementation are as follows:

* **Compliance.** The KLV arrow attempts to comply with the MISB standards.

* **Completeness.** The KLV arrow attempts to handle all tags, corner cases,
  and obscure features in any standard it implements.

* **Robustness.** The KLV arrow should not crash when erroneous data is
  encountered, but instead attempt to parse as much as it can. Any sections of
  unparseable KLV data are still retained in the form of raw bytes.

* **Editability.** Incoming KLV data is decoded into an intermediate software
  representation composed of standard C++ types and custom classes, allowing it
  to be manipulated independently of the encode / decode process.

* **Losslessness.** Any valid data decoded by the KLV arrow can be re-encoded in an
  equivalent form, with "equivalent" indicating no useful information is lost,
  not byte-for-byte equality.

* **Efficiency.** Encoded KLV data is quite small compared to a typical accompanying
  video stream. The KLV arrow attempts to keep the compute and memory cost of
  processing KLV to negligable levels.

* **Recency.** The KLV arrow keeps up to date with the latest versions of the MISB
  standards and does not attempt to offer perfect backwards-compatibility or
  compliance with outdated versions. However, support for some deprecated
  features may be kept around if enough data that uses those features exists in
  the wild, or if doing so is trivial.

While KWIVER may log various warnings and errors, this implementation does not
attempt to recognize all nonconformance with MISB standards that may be present
in input data, nor does it always prevent the encoding of nonconformant data.
This latter policy allows the system to process slightly nonconformant data as
may be present in the wild, but **it places the responsibility on the user** to
ensure that the data and encoding settings they pass in are standard-conformant.

How to Use
----------

The easiest way to leverage the capabilities of the KLV arrow is through the
``dump-klv`` command-line applet. The default printed output and ``csv``
exporter give an interpreted summary of commonly useful metadata fields
(latitude/longitude, etc.) extracted mostly from ST0601, while the ``klv-json``
exporter will give a complete tag-by-tag report across all standards.

To manipulate KLV programmatically, parsed :ref:`klv_packet`\ s may be obtained
from an :ref:`ffmpeg_video_input` through the ``frame_metadata()`` method. Each of
the returned ``vital::metadata`` objects which successfully ``dynamic_cast``\ s
to :ref:`klv_metadata` contains one frame of :ref:`klv_packet`\ s from one KLV stream
in the source media.

Starting with a :ref:`klv_packet` at the root, parsed KLV data is stored as a hierachy.
Along this hierachy, some data (e.g. :ref:`klv_packet.key`) is stored directly, while
some (e.g. :ref:`klv_packet.value`) is stored in a :ref:`klv_value` container. A
:ref:`klv_value` can be thought of as being in three basic states:
  1. **Empty.**
    There is no data for this field. In a local set, this indicates a zero-length
    element (ZLE), relevant for Report-on-Change behavior. In other contexts, it
    may simply indicate an omitted optional field.
  2. **Invalid.**
    There is data in this field, but we cannot parse it. Either it is irrecoverably
    nonconformant, or we have not yet implemented the relevant standard. In either
    case, the byte sequence is stored in a :ref:`klv_blob` object within
    the :ref:`klv_value`. That byte sequence can then be written back out
    verbatim to prevent data loss.
  3. **Valid.**
    The :ref:`klv_value` contains data of the appropriate type. For example, if the
    ``key`` of the :ref:`klv_packet` is the ST0601 universal key, a valid ``value``
    would contain an object of type :ref:`klv_local_set`.

.. warning::
    When processing parsed KLV, developers should make sure to handle cases where
    a :ref:`klv_value` is not valid.

The transformations between encoded KLV and KWIVER's in-memory data structures
are organized via :ref:`klv_data_format` classes, such as :ref:`klv_string_format` and
:ref:`klv_0601_local_set_format`. Each format implements reading and writing a
particular type of data, and may invoke other formats internally. In addition to
``read()`` and ``write()`` methods, each format has a ``length_of()`` method,
which allows the writer to preallocate the exact number of bytes needed to hold
the encoded KLV data.

However, in most cases users will be reading from or writing to a video file, in
which case the ``video_input`` or ``video_output`` will handle the KLV packet decoding
and encoding internally. To read or write packets independently of a video stream,
see the :ref:`klv_read_packet` and :ref:`klv_write_packet` functions. These will
automatically select the correct format to handle the data given to them.

Algorithm Implementations
-------------------------

.. _apply_child_klv:
apply_child_klv
^^^^^^^^^^^^^^^
..  doxygenclass:: kwiver::arrows::klv::apply_child_klv
    :project: kwiver
    :members:

.. _update_klv:
update_klv
^^^^^^^^^^
..  doxygenclass:: kwiver::arrows::klv::update_klv
    :project: kwiver
    :members:

Other Classes
-------------

.. _klv_0601_local_set_format:
klv_0601_local_set_format
^^^^^^^^^^^^^^^^^^^^^^^^^
..  doxygenclass:: kwiver::arrows::klv::klv_0601_local_set_format
    :project: kwiver
    :members:

.. _klv_blob:
klv_blob
^^^^^^^^
..  doxygenclass:: kwiver::arrows::klv::klv_blob
    :project: kwiver
    :members:

.. _klv_data_format:
klv_data_format
^^^^^^^^^^^^^^^
..  doxygenclass:: kwiver::arrows::klv::klv_data_format
    :project: kwiver
    :members:

.. _klv_metadata:
klv_metadata
^^^^^^^^^^^^
..  doxygenclass:: kwiver::arrows::klv::klv_metadata
    :project: kwiver
    :members:

.. _klv_packet:
klv_packet
^^^^^^^^^^
..  doxygenstruct:: kwiver::arrows::klv::klv_packet
    :project: kwiver
    :members:

.. _klv_string_format:
klv_string_format
^^^^^^^^^^^^^^^^^
..  doxygenclass:: kwiver::arrows::klv::klv_string_format
    :project: kwiver
    :members:

.. _klv_value:
klv_value
^^^^^^^^^
..  doxygenclass:: kwiver::arrows::klv::klv_value
    :project: kwiver
    :members:

Utility Functions
-----------------

.. _klv_read_packet:
klv_read_packet
^^^^^^^^^^^^^^^
..  doxygenfunction:: kwiver::arrows::klv::klv_read_packet
    :project: kwiver

.. _klv_write_packet:
klv_write_packet
^^^^^^^^^^^^^^^^
..  doxygenfunction:: kwiver::arrows::klv::klv_write_packet
    :project: kwiver
