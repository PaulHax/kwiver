// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#ifndef KWIVER_ARROWS_GEOCALC_CONSTANTS_H_
#define KWIVER_ARROWS_GEOCALC_CONSTANTS_H_

namespace kwiver {

namespace arrows {

namespace geocalc {

// ----------------------------------------------------------------------------
struct ellipsoid_wgs84
{
  constexpr static double a = 6'378'137.000;
  constexpr static double b = 6'356'752.314'245;
  constexpr static double a2 = a * a;
  constexpr static double b2 = b * b;
  constexpr static double a2_minus_b2 = a2 - b2;
  constexpr static double e2 = a2_minus_b2 / a2;
  constexpr static double eprime2 = a2_minus_b2 / b2;
};

} // namespace geocalc

} // namespace arrows

} // namespace kwiver

#endif
