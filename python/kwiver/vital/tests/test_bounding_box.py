"""
ckwg +31
Copyright 2020 by Kitware, Inc.
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

==============================================================================

Tests for BoundingBox python class, templates implemented as BoundingBoxD/F/I depending on type.

"""

import unittest
import numpy as np
import pytest

from kwiver.vital.types import (
    BoundingBoxD as bbD,
    BoundingBoxF as bbF,
    BoundingBoxI as bbI,
)


class TestBoundingBox(unittest.TestCase):
    @classmethod
    def setUp(self):
        self.ul_i = np.array([0, 0])
        self.ul_d = np.array([0.0, 0.0])
        self.lr_i = np.array([180, 240])
        self.lr_d = np.array([180.0, 240.0])
        self.width_i = 180
        self.height_i = 240
        self.width_d = 180.0
        self.height_d = 240.0
        self.min_x_i = 0
        self.min_y_i = 0
        self.min_x_d = 0.0
        self.min_y_d = 0.0

    def runner(self, bbox, type_):
        m = bbox(
            type_(self.min_x_d),
            type_(self.min_y_d),
            type_(self.width_d),
            type_(self.height_d),
        )
        self.check_points(m)
        self.check_dimensions(m)
        self.check_has_points(m)
        self.check_py_overloads(m)

    def check_points(self, bbox):
        assert bbox.min_x() == pytest.approx(self.ul_d[0])
        assert bbox.min_y() == pytest.approx(self.ul_d[1])
        assert bbox.max_x() == pytest.approx(self.lr_d[0])
        assert bbox.max_y() == pytest.approx(self.lr_d[1])
        assert bbox.upper_left() == pytest.approx(self.ul_d)
        assert bbox.lower_right() == pytest.approx(self.lr_d)

    def check_dimensions(self, bbox):
        assert bbox.center() == pytest.approx([90.0, 120.0])
        assert bbox.width() == pytest.approx(self.width_d)
        assert bbox.height() == pytest.approx(self.height_d)
        assert bbox.area() == pytest.approx(self.width_d * self.height_d)

    def check_has_points(self, bbox):
        assert bbox.is_valid()
        assert bbox.contains(np.array([25, 25]))
        assert not bbox.contains(np.array([-1, 0]))

    def check_is_not_valid(self, bbox):
        assert not bbox.is_valid()

    def check_py_overloads(self, bbox):
        try:
            assert bbox.__nice__() == "0, 0, 180, 240"
            assert bbox.__repr__()[13:29] == "(0, 0, 180, 240)"
            assert str(bbox)[13:29] == "(0, 0, 180, 240)"
        except:
            assert bbox.__nice__() == "0.0, 0.0, 180.0, 240.0"
            assert bbox.__repr__()[13:37] == "(0.0, 0.0, 180.0, 240.0)"
            assert str(bbox)[13:37] == "(0.0, 0.0, 180.0, 240.0)"

    def test_constructorI(self):
        bbI()
        self.check_is_not_valid(bbI())
        bbI(self.ul_i, self.lr_i)
        bbI(self.ul_i, self.width_i, self.height_i)
        bbI(self.min_x_i, self.min_y_i, self.width_i, self.height_i)

    def test_constructorF(self):
        bbF()
        self.check_is_not_valid(bbF())
        bbF(self.ul_d, self.lr_d)
        bbF(self.ul_d, self.width_d, self.height_d)
        bbF(self.min_x_d, self.min_y_d, self.width_d, self.height_d)

    def test_constructorD(self):
        bbD()
        self.check_is_not_valid(bbD())
        bbD(self.ul_d, self.lr_d)
        bbD(self.ul_d, self.width_d, self.height_d)
        bbD(self.min_x_d, self.min_y_d, self.width_d, self.height_d)

    def test_bounding_boxes(self):
        self.runner(bbI, int)
        self.runner(bbF, float)
        self.runner(bbD, float)
