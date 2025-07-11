"""
This file is part of KWIVER, and is distributed under the
OSI-approved BSD 3-Clause License. See top-level LICENSE file or
https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

Tests for Python interface to vital::point

"""

from kwiver.vital.types import Covar2f, Covar3f, Covar4f
from kwiver.vital.types.point import *

import numpy as np
import unittest


class TestVitalPoint(unittest.TestCase):
    def test_default_constructors(self):
        Point2i()
        Point2d()
        Point2f()
        Point3d()
        Point3f()
        Point4d()
        Point4f()

    def test_type_name_def_ctor(self):
        pts = [
            Point2i(),
            Point2d(),
            Point2f(),
            Point3d(),
            Point3f(),
            Point4d(),
            Point4f(),
        ]
        expected_types = ["2i", "2d", "2f", "3d", "3f", "4d", "4f"]

        for pt, t in zip(pts, expected_types):
            self.assertEqual(pt.type_name, t)

    def test_type_name_nondef_ctor(self):
        loc2_int = np.random.randint(low=-10, high=10, size=2)
        loc2 = np.random.uniform(low=-10, high=10, size=2)
        loc3 = np.random.uniform(low=-10, high=10, size=3)
        loc4 = np.random.uniform(low=-10, high=10, size=4)
        cov2, cov3, cov4 = Covar2f(), Covar3f(), Covar4f()

        pts = [
            Point2i(loc2_int, cov2),
            Point2d(loc2, cov2),
            Point2f(loc2, cov2),
            Point3d(loc3, cov3),
            Point3f(loc3, cov3),
            Point4d(loc4, cov4),
            Point4f(loc4, cov4),
        ]
        expected_types = ["2i", "2d", "2f", "3d", "3f", "4d", "4f"]

        for pt, t in zip(pts, expected_types):
            self.assertEqual(pt.type_name, t)

    def test_non_default_constructors(self):
        vector_init = np.array([-5, 10])
        Point2i(vector_init)
        Point2i(vector_init, Covar2f())
        Point2i(vector_init, Covar2f(20))

        vector_init = np.random.uniform(low=-10, high=10, size=2)
        Point2d(vector_init)
        Point2f(vector_init)
        Point2d(vector_init, Covar2f())
        Point2f(vector_init, Covar2f())
        Point2d(vector_init, Covar2f(20))
        Point2f(vector_init, Covar2f(20))

        vector_init = np.random.uniform(low=-10, high=10, size=3)
        Point3d(vector_init)
        Point3f(vector_init)
        Point3d(vector_init, Covar3f())
        Point3f(vector_init, Covar3f())
        Point3d(vector_init, Covar3f(20))
        Point3f(vector_init, Covar3f(20))

        vector_init = np.random.uniform(low=-10, high=10, size=4)
        Point4d(vector_init)
        Point4f(vector_init)
        Point4d(vector_init, Covar4f())
        Point4f(vector_init, Covar4f())
        Point4d(vector_init, Covar4f(20))
        Point4f(vector_init, Covar4f(20))

    def test_initial_value(self):
        np.testing.assert_array_equal(Point2i().value, np.zeros(2))
        np.testing.assert_array_equal(Point2d().value, np.zeros(2))
        np.testing.assert_array_equal(Point2f().value, np.zeros(2))
        np.testing.assert_array_equal(Point3d().value, np.zeros(3))
        np.testing.assert_array_equal(Point3f().value, np.zeros(3))
        np.testing.assert_array_equal(Point4d().value, np.zeros(4))
        np.testing.assert_array_equal(Point4f().value, np.zeros(4))

        # Try setting some values
        loc2_int = np.random.randint(low=-10, high=10, size=2)
        loc2 = np.random.uniform(low=-10, high=10, size=2)
        loc3 = np.random.uniform(low=-10, high=10, size=3)
        loc4 = np.random.uniform(low=-10, high=10, size=4)
        cov2, cov3, cov4 = Covar2f(), Covar3f(), Covar4f()

        np.testing.assert_array_equal(Point2i(loc2_int, cov2).value, loc2_int)
        np.testing.assert_array_almost_equal(Point2d(loc2, cov2).value, loc2, 16)
        np.testing.assert_array_almost_equal(Point2f(loc2, cov2).value, loc2, 6)
        np.testing.assert_array_almost_equal(Point3d(loc3, cov3).value, loc3, 16)
        np.testing.assert_array_almost_equal(Point3f(loc3, cov3).value, loc3, 6)
        np.testing.assert_array_almost_equal(Point4d(loc4, cov4).value, loc4, 16)
        np.testing.assert_array_almost_equal(Point4f(loc4, cov4).value, loc4, 6)

    def test_initial_covariance(self):
        eye2, eye3, eye4 = np.eye(2), np.eye(3), np.eye(4)
        loc2, loc3, loc4 = np.ones(2), np.ones(3), np.ones(4)

        # Use constructor without arguments
        np.testing.assert_array_equal(Point2i().covariance.matrix(), eye2)
        np.testing.assert_array_equal(Point2d().covariance.matrix(), eye2)
        np.testing.assert_array_equal(Point2f().covariance.matrix(), eye2)
        np.testing.assert_array_equal(Point3d().covariance.matrix(), eye3)
        np.testing.assert_array_equal(Point3f().covariance.matrix(), eye3)
        np.testing.assert_array_equal(Point4d().covariance.matrix(), eye4)
        np.testing.assert_array_equal(Point4f().covariance.matrix(), eye4)

        # Use other constructor with location only
        np.testing.assert_array_equal(Point2i(loc2).covariance.matrix(), eye2)
        np.testing.assert_array_equal(Point2d(loc2).covariance.matrix(), eye2)
        np.testing.assert_array_equal(Point2f(loc2).covariance.matrix(), eye2)
        np.testing.assert_array_equal(Point3d(loc3).covariance.matrix(), eye3)
        np.testing.assert_array_equal(Point3f(loc3).covariance.matrix(), eye3)
        np.testing.assert_array_equal(Point4d(loc4).covariance.matrix(), eye4)
        np.testing.assert_array_equal(Point4f(loc4).covariance.matrix(), eye4)

        # Use other constructor with a location and covariance
        rand_coeff = np.random.uniform(-10, 10)
        cov2, cov3, cov4 = Covar2f(rand_coeff), Covar3f(rand_coeff), Covar4f(rand_coeff)
        cov2_mat, cov3_mat, cov4_mat = cov2.matrix(), cov3.matrix(), cov4.matrix()

        np.testing.assert_array_almost_equal(
            Point2i(loc2, cov2).covariance.matrix(), cov2_mat, 6
        )
        np.testing.assert_array_almost_equal(
            Point2d(loc2, cov2).covariance.matrix(), cov2_mat, 6
        )
        np.testing.assert_array_almost_equal(
            Point2f(loc2, cov2).covariance.matrix(), cov2_mat, 6
        )
        np.testing.assert_array_almost_equal(
            Point3d(loc3, cov3).covariance.matrix(), cov3_mat, 6
        )
        np.testing.assert_array_almost_equal(
            Point3f(loc3, cov3).covariance.matrix(), cov3_mat, 6
        )
        np.testing.assert_array_almost_equal(
            Point4d(loc4, cov4).covariance.matrix(), cov4_mat, 6
        )
        np.testing.assert_array_almost_equal(
            Point4f(loc4, cov4).covariance.matrix(), cov4_mat, 6
        )

    def test_set_covariance_wrong_dim(self):
        with self.assertRaises(TypeError):
            Point2i().covariance = Covar3f()

        with self.assertRaises(TypeError):
            Point3f().covariance = Covar4f()

        with self.assertRaises(TypeError):
            Point4f().covariance = Covar3f()

    def test_set_value_wrong_dim(self):
        with self.assertRaises(TypeError):
            Point2i().value = np.ones(3)

        with self.assertRaises(TypeError):
            Point3f().value = np.ones(4)

        with self.assertRaises(TypeError):
            Point4f().value = np.ones(3)

    def test_cast_int_value(self):
        p = Point2i()
        p.value = [10.5, -7.6]
        np.testing.assert_array_equal(p.value, np.array([10, -7]))

    def test_get_and_set_value(self):
        # Doubles
        p = Point4d()

        val = np.random.uniform(low=-10, high=10, size=4)
        p.value = val
        np.testing.assert_array_almost_equal(p.value, val, 16)

        p.value = np.array([0, 0, 0, 0])
        np.testing.assert_array_equal(p.value, [0, 0, 0, 0])

        # Floats
        p = Point4f()

        val = np.random.uniform(low=-10, high=10, size=4)
        p.value = val
        np.testing.assert_array_almost_equal(p.value, val, 6)

        p.value = np.array([0, 0, 0, 0])
        np.testing.assert_array_equal(p.value, [0, 0, 0, 0])

    def test_get_and_set_covariance(self):
        # Doubles
        p = Point4d()

        val = Covar4f(np.random.uniform(low=-10, high=10))
        p.covariance = val
        np.testing.assert_array_almost_equal(p.covariance.matrix(), val.matrix(), 6)

        val = Covar4f()
        p.covariance = val
        np.testing.assert_array_equal(p.covariance.matrix(), val.matrix())

        # Floats
        p = Point4f()

        val = Covar4f(np.random.uniform(low=-10, high=10))
        p.covariance = val
        np.testing.assert_array_almost_equal(p.covariance.matrix(), val.matrix(), 6)

        val = Covar4f()
        p.covariance = val
        np.testing.assert_array_equal(p.covariance.matrix(), val.matrix())
