// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Qt image container interface

#ifndef KWIVER_ARROWS_QT_IMAGE_CONTAINER_H_
#define KWIVER_ARROWS_QT_IMAGE_CONTAINER_H_

#include <arrows/qt/kwiver_algo_qt_export.h>

#include <vital/types/image_container.h>

#include <QImage>

namespace kwiver {

namespace arrows {

namespace qt {

/// This image container wraps a QImage
///
/// This class represents an image using QImage format to store the image data
/// by extending the basic image_container. Due to limitations of QImage (which
/// is a GUI-oriented type and not a general purpose container), some formats
/// are not supported or may result in loss of color information upon
/// conversion.
class KWIVER_ALGO_QT_EXPORT image_container
  : public vital::image_container
{
public:
  explicit image_container( QImage const& d ) : data_{ d } {}
  explicit image_container( vital::image const& vital_image )
    : data_{ vital_to_qt( vital_image ) }
  {}

  explicit image_container( vital::image_container const& container );

  image_container( image_container const& other ) = default;
  image_container( image_container&& other ) = default;

  image_container& operator=( image_container const& other ) = default;
  image_container& operator=( image_container&& other ) = default;

  operator bool() const { return !data_.isNull(); }
  bool
  operator!() const { return data_.isNull(); }

  /// \copydoc vital::image_container::size
  size_t
  size() const override
  { return static_cast< size_t >( data_.sizeInBytes() ); }

  /// \copydoc vital::image_container::width
  size_t
  width() const override
  { return static_cast< size_t >( data_.width() ); }

  /// \copydoc vital::image_container::height
  size_t
  height() const override
  { return static_cast< size_t >( data_.height() ); }

  /// \copydoc vital::image_container::depth
  size_t depth() const override;

  /// \copydoc vital::image_container::get_image
  vital::image
  get_image() const override
  { return qt_to_vital( data_ ); }
  using vital::image_container::get_image;

  /// Get QImage in this container.
  operator QImage const&() const { return data_; }

  /// Convert a QImage to a VITAL image.
  ///
  /// This converts a QImage to a VITAL image. Currently, the resulting VITAL
  /// image will always have a pixel type of uint8_t, regardless of the format
  /// of the input image. This may cause loss of precision or an increase in
  /// storage space requirements for some formats.
  static vital::image qt_to_vital( QImage const& img );

  /// Convert a VITAL image to a QImage.
  ///
  /// This converts a VITAL image to a QImage. Currently, only images of pixel
  /// type bool (with depth 1) or uint8_t (with depth 1, 3 or 4) are supported.
  static QImage vital_to_qt( vital::image const& img );

protected:
  /// Image data.
  QImage data_;
};

} // end namespace qt

} // end namespace arrows

} // end namespace kwiver

#endif
