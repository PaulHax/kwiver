// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Frame to Frame Homography implementation

#include "homography_f2f.h"

#include <vital/exceptions/math.h>
#include <vital/vital_types.h>

namespace kwiver {

namespace vital {

/// Construct an identity homography for the given frame
f2f_homography
::f2f_homography( frame_id_t const frame_id )
  : h_( homography_sptr( new homography_< double >( ) ) ),
    from_id_( frame_id ),
    to_id_( frame_id )
{}

/// Construct a frame to frame homography given an existing transform
f2f_homography
::f2f_homography(
  homography_sptr const& h,
  frame_id_t const from_id,
  frame_id_t const to_id )
  : h_( std::static_pointer_cast< vital::homography >( h->clone() ) ),
    from_id_( from_id ),
    to_id_( to_id )
{}

/// Copy constructor
f2f_homography
::f2f_homography( f2f_homography const& h )
  : h_( std::static_pointer_cast< vital::homography >( h.h_->clone() ) ),
    from_id_( h.from_id_ ),
    to_id_( h.to_id_ )
{}

/// Get the homography transformation
homography_sptr
f2f_homography
::homography() const
{
  return this->h_;
}

/// Frame identifier that the homography maps from.
frame_id_t
f2f_homography
::from_id() const
{
  return this->from_id_;
}

/// Frame identifier that the homography maps to.
frame_id_t
f2f_homography
::to_id() const
{
  return this->to_id_;
}

/// Return a new inverse \p f2f_homography instance
f2f_homography
f2f_homography
::inverse() const
{
  return f2f_homography( this->h_->inverse(), this->to_id_, this->from_id_ );
}

/// Custom f2f_homography multiplication operator for \p f2f_homography
f2f_homography
f2f_homography
::operator*( f2f_homography const& rhs )
{
  if( this->from_id() != rhs.to_id() )
  {
    VITAL_THROW(
      invalid_matrix_operation,
      "Homography frame identifiers do not match up" );
  }

  Eigen::Matrix< double, 3, 3 > new_h = this->h_->matrix() * rhs.h_->matrix();
  return f2f_homography( new_h, rhs.from_id(), this->to_id() );
}

/// \p f2f_homography output stream operator
std::ostream&
operator<<( std::ostream& s, f2f_homography const& h )
{
  s << h.from_id() << " -> " << h.to_id() << "\n"
    << *h.homography();
  return s;
}

} // namespace vital

}   // end vital namespace
