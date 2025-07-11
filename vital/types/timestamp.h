// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#ifndef _VITAL_TIMESTAMP_H_
#define _VITAL_TIMESTAMP_H_

#include <istream>
#include <ostream>
#include <stdint.h>

#include <vital/types/vital_types_export.h>
#include <vital/vital_types.h>

namespace kwiver {

namespace vital {

// ----------------------------------------------------------------------------
/// \brief Frame time.
///
/// This class represents a timestamp for a single video frame.  The
/// time is stored in micro-seconds and frame numbers start at one.
///
/// A timestamp has the notion of valid time and valid frame. This is
/// useful when dealing with interpolated timestamps. In this case, a
/// timestamp may have a time, but no frame.
///
/// When comparing timestamps, they must be from the same domain. If
/// not, then they are not comparable and \b all relative operators
/// return false.
///
/// If both timestamps have a time, then they are ordered by that
/// value.  If both do not have time but both have frame numbers, they
/// are ordered by frame number. If the timestamps do not have some way
/// of being compared, all relational operators return false.
class VITAL_TYPES_EXPORT timestamp
{
public:
  typedef kwiver::vital::frame_id_t frame_t;
  typedef kwiver::vital::time_usec_t time_t;

  /// \brief Default constructor.
  ///
  /// Created an invalid timestamp.
  timestamp();

  /// \brief Constructor
  ///
  /// Creates a valid timestamp with specified time and frame number.
  ///
  /// @param t Time for timestamp in micro-seconds
  /// @param f Frame number for timestamp
  explicit timestamp( time_usec_t t, frame_id_t f );

  /// \brief Is timestamp valid.
  ///
  /// Both the time and frame must be set for a timestamp to be totally
  /// valid.
  ///
  /// @return \b true if both time and frame are valid
  bool
  is_valid() const { return m_valid_time && m_valid_frame; }

  /// \brief Timestamp has valid time.
  ///
  /// Indicates that the time has been set for this timestamp.
  ///
  /// @return \b true if time has been set
  bool
  has_valid_time() const { return m_valid_time; }

  /// \brief Timestamp has valid frame number.
  ///
  /// Indicates that the frame number has been set for this timestamp.
  ///
  /// @return \b true if frame number has been set
  bool
  has_valid_frame() const { return m_valid_frame; }

  /// \brief Get time from timestamp.
  ///
  /// The time portion of the timestamp is returned in
  /// micro-seconds. The value will be undetermined if the timestamp
  /// does not have a valid time.
  ///
  /// \sa has_valid_time()
  ///
  /// @return Frame time in micro-seconds
  time_usec_t
  get_time_usec() const { return m_time; }

  /// \brief Get time in seconds.
  ///
  /// The time portion of the timestamp is returned in seconds and fractions.
  ///
  /// \return time in seconds.
  double get_time_seconds() const;

  /// \brief Get frame number from timestamp.
  ///
  /// The frame number value from the timestamp is returned. The first
  /// frame in a sequence is usually one. The frame number will be
  /// undetermined if the timestamp does not have a valid frame number
  /// set.
  /// \sa has_valid_frame()
  ///
  /// @return Frame number.
  frame_id_t
  get_frame() const { return m_frame; }

  /// \brief Set time portion of timestamp.
  ///
  /// @param t Time for frame.
  timestamp& set_time_usec( time_usec_t t );

  /// \brief Set time portion of timestamp.
  ///
  /// @param t Time for frame in seconds.
  timestamp& set_time_seconds( double t );

  /// \brief Set frame portion of timestamp.
  ///
  /// @param f Frame number
  timestamp& set_frame( frame_id_t f );

  /// \brief Set timestamp totally invalid.
  ///
  /// Both the frame and time are set to invalid
  timestamp& set_invalid();

  /// @brief Set time domain index for this timestamp
  ///
  /// @param dom Time domain index
  ///
  /// @return Reference to this object.
  timestamp& set_time_domain_index( int dom );

  int
  get_time_domain_index() const { return m_time_domain_index; }

  /// \brief Format object in a readable manner.
  ///
  /// This method formats a time stamp in a readable and recognizable
  /// manner suitable form debugging and logging.
  ///
  /// @return formatted timestamp
  std::string pretty_print() const;

  bool operator==( timestamp const& rhs ) const;
  bool operator!=( timestamp const& rhs ) const;
  bool operator<( timestamp const& rhs ) const;
  bool operator>( timestamp const& rhs ) const;
  bool operator<=( timestamp const& rhs ) const;
  bool operator>=( timestamp const& rhs ) const;

private:
  bool m_valid_time;            ///< indicates valid time
  bool m_valid_frame;           ///< indicates valid frame number

  time_usec_t m_time;             ///< frame time in micro-seconds
  frame_id_t m_frame;          ///< frame number

  // index used to determine the time domain for this timestamp.
  int m_time_domain_index;
}; // end class timestamp

inline std::ostream&
operator<<( std::ostream& str, timestamp const& obj )
{ str << obj.pretty_print().c_str(); return str; }

} // namespace vital

}   // end namespace

#endif // _VITAL_TIMESTAMP_H_
