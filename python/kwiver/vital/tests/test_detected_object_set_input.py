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

Tests for the DetectedObjectSetInput wrapping class
"""

import unittest

from kwiver.vital.algo import DetectedObjectSetInput
from kwiver.vital.config import empty_config
from kwiver.vital import plugin_management
from kwiver.vital.types import DetectedObjectSet

SIMULATOR_CONFIG = dict(
    center_x=3,
    center_y=5,
    dx=0.3,
    dy=0.2,
    height=10,
    width=15,
    max_sets=10,
    set_size=4,
    image_name="foo.png",
)


def _create_simulator_config():
    cfg = empty_config()
    for k, v in SIMULATOR_CONFIG.items():
        cfg[k] = v
    return cfg


class TestVitalDetectedObjectSetInput(unittest.TestCase):
    def setUp(self):
        vpm = plugin_management.plugin_manager_instance()
        vpm.load_all_plugins()

    def test_registered_names(self):
        """Print all the registered detected object set input arrows"""
        print("All registered detected object set input arrows:")
        for arrow in DetectedObjectSetInput.registered_names():
            print("  " + arrow)

    def test_open(self):
        """Create a DetectedObjectSetInput and call .open on it"""
        dosi = DetectedObjectSetInput.create("simulator")
        # The simulator doesn't actually try to open its file
        dosi.open("/example/file/path")

    def test_read_set(self):
        """Create and configure a DetectedObjectSetInput and call .read_set"""
        dosi = DetectedObjectSetInput.create("simulator")
        dosi.set_configuration(_create_simulator_config())
        dos, image_name = dosi.read_set()
        counter = 0
        while dos:
            counter += 1
            self.assertIsInstance(dos, DetectedObjectSet)
            self.assertEqual(len(dos), SIMULATOR_CONFIG["set_size"])
            self.assertEqual(image_name, SIMULATOR_CONFIG["image_name"])
            dos, image_name = dosi.read_set()
        self.assertEqual(counter, SIMULATOR_CONFIG["max_sets"])
