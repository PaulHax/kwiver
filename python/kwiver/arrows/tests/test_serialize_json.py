"""
ckwg +29
Copyright 2019-2020 by Kitware, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

 * Neither name of Kitware, Inc. nor the names of any contributors may be used
   to endorse or promote products derived from this software without specific
   prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Tests for json serialization of vital types
"""

from __future__ import print_function, absolute_import

import nose.tools

from kwiver.arrows.serialize.json import serialize_activity_type
from kwiver.arrows.serialize.json import deserialize_activity_type
from kwiver.arrows.serialize.json import serialize_bounding_box
from kwiver.arrows.serialize.json import deserialize_bounding_box
from kwiver.arrows.serialize.json import serialize_detected_object
from kwiver.arrows.serialize.json import deserialize_detected_object
from kwiver.arrows.serialize.json import serialize_detected_object_set
from kwiver.arrows.serialize.json import deserialize_detected_object_set
from kwiver.arrows.serialize.json import serialize_detected_object_type
from kwiver.arrows.serialize.json import deserialize_detected_object_type
from kwiver.arrows.serialize.json import serialize_image
from kwiver.arrows.serialize.json import deserialize_image
from kwiver.arrows.serialize.json import serialize_timestamp
from kwiver.arrows.serialize.json import deserialize_timestamp
from kwiver.arrows.serialize.json import serialize_track_state
from kwiver.arrows.serialize.json import deserialize_track_state
from kwiver.arrows.serialize.json import serialize_track
from kwiver.arrows.serialize.json import deserialize_track
from kwiver.arrows.serialize.json import serialize_track_set
from kwiver.arrows.serialize.json import deserialize_track_set
from kwiver.arrows.serialize.json import serialize_object_track_state
from kwiver.arrows.serialize.json import deserialize_object_track_state
from kwiver.arrows.serialize.json import serialize_object_track_set
from kwiver.arrows.serialize.json import deserialize_object_track_set

from kwiver.arrows.tests.serialize_json_utils import create_activity_type
from kwiver.arrows.tests.serialize_json_utils import compare_activity_type
from kwiver.arrows.tests.serialize_json_utils import create_bounding_box
from kwiver.arrows.tests.serialize_json_utils import compare_bounding_box
from kwiver.arrows.tests.serialize_json_utils import create_detected_object
from kwiver.arrows.tests.serialize_json_utils import compare_detected_object
from kwiver.arrows.tests.serialize_json_utils import create_detected_object_set
from kwiver.arrows.tests.serialize_json_utils import compare_detected_object_set
from kwiver.arrows.tests.serialize_json_utils import create_detected_object_type
from kwiver.arrows.tests.serialize_json_utils import compare_detected_object_type
from kwiver.arrows.tests.serialize_json_utils import create_image
from kwiver.arrows.tests.serialize_json_utils import compare_image
from kwiver.arrows.tests.serialize_json_utils import create_timestamp
from kwiver.arrows.tests.serialize_json_utils import compare_timestamp
from kwiver.arrows.tests.serialize_json_utils import create_track_state
from kwiver.arrows.tests.serialize_json_utils import compare_track_state
from kwiver.arrows.tests.serialize_json_utils import create_track
from kwiver.arrows.tests.serialize_json_utils import compare_track
from kwiver.arrows.tests.serialize_json_utils import create_track_set
from kwiver.arrows.tests.serialize_json_utils import compare_track_set
from kwiver.arrows.tests.serialize_json_utils import create_object_track_state
from kwiver.arrows.tests.serialize_json_utils import compare_object_track_state
from kwiver.arrows.tests.serialize_json_utils import create_object_track_set
from kwiver.arrows.tests.serialize_json_utils import compare_object_track_set

import unittest


class TestJsonSerialize(unittest.TestCase):
    def test_serialize_activity_type(self):
        at = create_activity_type()
        serialized_at = serialize_activity_type(at)
        deserialized_at = deserialize_activity_type(serialized_at)
        self.assertTrue(compare_activity_type(at, deserialized_at))

    def test_serialize_bounding_box(self):
        bbox = create_bounding_box()
        serialized_bbox = serialize_bounding_box(bbox)
        deserialized_bbox = deserialize_bounding_box(serialized_bbox)
        self.assertTrue(compare_bounding_box(bbox, deserialized_bbox))

    def test_serialize_detected_object(self):
        do = create_detected_object()
        serialized_do = serialize_detected_object(do)
        deserialized_do = deserialize_detected_object(serialized_do)
        self.assertTrue(compare_detected_object(do, deserialized_do))

    def test_serialize_detected_object_set(self):
        dos = create_detected_object_set()
        serialized_dos = serialize_detected_object_set(dos)
        deserialized_dos = deserialize_detected_object_set(serialized_dos)
        self.assertTrue(compare_detected_object_set(dos, deserialized_dos))

    def test_serialize_detected_object_type(self):
        dot = create_detected_object_type()
        serialized_dot = serialize_detected_object_type(dot)
        deserialized_dot = deserialize_detected_object_type(serialized_dot)
        self.assertTrue(compare_detected_object_type(dot, deserialized_dot))

    def test_serialize_image(self):
        img = create_image()
        serialized_img = serialize_image(img)
        deserialized_img = deserialize_image(serialized_img)
        self.assertTrue(compare_image(img, deserialized_img))

    def test_serialize_timestamp(self):
        ts = create_timestamp()
        serialized_ts = serialize_timestamp(ts)
        deserialized_ts = deserialize_timestamp(serialized_ts)
        self.assertTrue(compare_timestamp(ts, deserialized_ts))

    def test_serialize_track_state(self):
        ts = create_track_state()
        serialized_ts = serialize_track_state(ts)
        deserialized_ts = deserialize_track_state(serialized_ts)
        self.assertTrue(compare_track_state(ts, deserialized_ts))

    def test_serialize_track(self):
        ts = create_track()
        serialized_ts = serialize_track(ts)
        deserialized_ts = deserialize_track(serialized_ts)
        self.assertTrue(compare_track(ts, deserialized_ts))

    def test_serialize_track_set(self):
        ts = create_track_set()
        serialized_ts = serialize_track_set(ts)
        deserialized_ts = deserialize_track_set(serialized_ts)
        self.assertTrue(compare_track_set(ts, deserialized_ts))

    def test_serialize_object_track_set(self):
        ts = create_object_track_state()
        serialized_ts = serialize_object_track_state(ts)
        deserialized_ts = deserialize_object_track_state(serialized_ts)
        self.assertTrue(compare_track_state(ts, deserialized_ts))
