// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Header defining abstract image object detector

#ifndef VITAL_ALGO_IMAGE_OBJECT_DETECTOR_H_
#define VITAL_ALGO_IMAGE_OBJECT_DETECTOR_H_

#include <vital/algo/algorithm.h>
#include <vital/types/detected_object_set.h>
#include <vital/types/image_container.h>

#include <vector>

namespace kwiver {

namespace vital {

namespace algo {

// ----------------------------------------------------------------------------

/// @brief Image object detector base class/
///
class VITAL_ALGO_EXPORT image_object_detector
  : public kwiver::vital::algorithm
{
public:
  image_object_detector();
  PLUGGABLE_INTERFACE( image_object_detector );
  /// Find all objects on the provided image
  ///
  /// This method analyzes the supplied image and along with any saved
  /// context, returns a vector of detected image objects.
  ///
  /// \param image_data the image pixels
  /// \returns vector of image objects found
  virtual detected_object_set_sptr
  detect( image_container_sptr image_data ) const = 0;
};

/// Shared pointer for generic image_object_detector definition type.
typedef std::shared_ptr< image_object_detector > image_object_detector_sptr;

} // namespace algo

} // namespace vital

} // namespace kwiver

#endif // VITAL_ALGO_IMAGE_OBJECT_DETECTOR_H_
