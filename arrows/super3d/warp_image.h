// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Header file for warp_image, warps an image using a homography
#ifndef KWIVER_ARROWS_SUPER3D_WARP_IMAGE_H_
#define KWIVER_ARROWS_SUPER3D_WARP_IMAGE_H_

/// \file
#include <vgl/algo/vgl_h_matrix_2d.h>

template < class T > class vil_image_view;

namespace kwiver {

namespace arrows {

namespace super3d {

/// Parameters for warp_image().
///
struct warp_image_parameters
{
  /// Kind of interpolation, nearest neighbor, bilinear, or bicubic
  enum interp_type { NEAREST, LINEAR, CUBIC, };

/// Create a parameter set with default values.
  warp_image_parameters()
    : off_i_( 0 ),
      off_j_( 0 ),
      fill_unmapped_( true ),
      unmapped_value_( 0.0 ),
      interpolator_( LINEAR ),
      shallow_copy_okay_( false )
  {}

/// Set the offset of the destination image coordinates.
///
/// The destination image coordinates are offset by (off_i0,off_j0).
/// This means that if the destination image size is ni x nj, then the
/// coordinates of the pixels are assumed to be from (off_i0,off_j0)
/// to (off_i0+ni-1,off_j0+nj-1), instead of the default (0,0) to
/// (ni-1,nj-1).
  warp_image_parameters&
  set_offset( int off_i, int off_j )
  {
    off_i_ = off_i;
    off_j_ = off_j;
    return *this;
  }

/// Should unmapped pixels be set to black?
///
/// If this parameter is set (\c true), then pixels in the
/// destination image that do not map into the source image are set
/// to the unmapped value (typically 0).  If not set, then those
/// pixels are unchanged.
  warp_image_parameters&
  set_fill_unmapped( bool v )
  {
    fill_unmapped_ = v;
    return *this;
  }

  warp_image_parameters&
  set_unmapped_value( double v )
  {
    unmapped_value_ = v;
    return *this;
  }

/// Set the interpolator for the warping
///
/// false (default) uses bilinear interpolation
/// true uses bicubic interpolation
  warp_image_parameters&
  set_interpolator( interp_type v )
  {
    interpolator_ = v;
    return *this;
  }

/// Is a shallow copy acceptable?
///
/// If this parameter is true, the warp routines will set the
/// destination to a shallow copy of the source whenever possible.
/// For example, when the homography is the identity transformation.
  warp_image_parameters&
  set_shallow_copy_okay( bool v )
  {
    shallow_copy_okay_ = v;
    return *this;
  }

  int off_i_;
  int off_j_;
  bool fill_unmapped_;
  double unmapped_value_;
  interp_type interpolator_;
  bool shallow_copy_okay_;
};

/// Warp an image using a homography.
///
/// The destination image \a dest should already be allocated.
template < class T >
bool
warp_image(
  vil_image_view< T > const& src,
  vil_image_view< T >& dest,
  vgl_h_matrix_2d< double > const& dest_to_src_homography,
  vil_image_view< bool >* const unmapped_mask = NULL );

/// Warp an image using a homography.
///
/// See warp_image_parameters::set_offset() for the meaning of off_i and off_j.
template < class T >
bool
warp_image(
  vil_image_view< T > const& src,
  vil_image_view< T >& dest,
  vgl_h_matrix_2d< double > const& dest_to_src_homography,
  int off_i, int off_j,
  vil_image_view< bool >* const unmapped_mask = NULL );

/// Warp an image using a homography.
///
/// \sa warp_image_parameters
template < class T >
bool
warp_image(
  vil_image_view< T > const& src,
  vil_image_view< T >& dest,
  vgl_h_matrix_2d< double > const& dest_to_src_homography,
  warp_image_parameters const& param,
  vil_image_view< bool >* const unmapped_mask = NULL );

} // end namespace depth

} // end namespace arrows

} // end namespace kwiver

#endif
