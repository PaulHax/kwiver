# ckwg +29
# Copyright 2020 by Kitware, Inc.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#  * Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#
#  * Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
#  * Neither name of Kitware, Inc. nor the names of any contributors may be used
#    to endorse or promote products derived from this software without specific
#    prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF

from unittest import TestCase
from kwiver.vital.algo import ImageIO
import tempfile
import os
from kwiver.vital.types import ImageContainer
import numpy as np
from kwiver.vital import plugin_management


class TestVitalImageIO(TestCase):
    def setUp(self):
        vpm = plugin_management.plugin_manager_instance()
        vpm.load_all_plugins()
        self.instance = ImageIO.create("SimpleImageIO")

    def test_load_nonexistant(self):
        with self.assertRaises(RuntimeError):
            self.instance.load("nonexistant_filename.txt")

    def test_load_directory(self):
        with self.assertRaises(RuntimeError):
            with tempfile.TemporaryDirectory() as directory_name:
                self.instance.load(directory_name)

    def test_load_metadata_nonexistant(self):
        with self.assertRaises(RuntimeError):
            self.instance.load_metadata("nonexistant_filename.txt")

    def test_load_metadata_directory(self):
        with self.assertRaises(RuntimeError):
            with tempfile.TemporaryDirectory() as directory_name:
                self.instance.load_metadata(directory_name)

    def test_save_nonexistant(self):
        with self.assertRaises(RuntimeError):
            dummy_image = np.zeros([100, 100])
            image_container = ImageContainer.fromarray(dummy_image)
            self.instance.save("nonexistant_filename.txt", image_container)

    def test_save_directory(self):
        with self.assertRaises(RuntimeError):
            dummy_image = np.zeros([100, 100])
            image_container = ImageContainer.fromarray(dummy_image)
            with tempfile.TemporaryDirectory() as directory_name:
                self.instance.save(directory_name, image_container)
