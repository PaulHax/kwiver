// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// Interface to the basic KLV read/write functions.

// This file currently handles the serialization / deserialization of these
// basic formats of KLV data:
// - int : General signed or unsigned integer of any integral byte length up to
// 8. Important to get this one precisely right because it's the base of most
// KLV data. Written MSB first.
// - BER : Unsigned integer which encodes its own length. Up to 127 is
// identical to standard int, otherwise the first byte encodes the number of
// following bytes.
// - BER-OID : Unsigned integer which encodes its own length. First bit of each
// byte signals whether there is another following byte; lower seven bits
// concatenated together form the actual value.
// - flint : Predecessor to IMAP. Floating-point number between defined upper
// and lower limits, represented as a signed or unsigned integer, the full
// range of which is uniformly mapped between those limits. The signed version
// is always mapped to a range symmetrical around zero (e.g. -90 to 90). The
// lowest possible signed integer is used as an out-of-range / NaN indicator.
// Unsigned integers are used for non-symmetrical ranges (e.g. -30 to 100), and
// have no special values, so out-of-range values are clamped.
// - IMAP : Floating-point number between defined upper and lower limits,
// represented as an integer, the full range of which is uniformly mapped
// between those limits. Has special defined values for infinities, NaNs, etc.
// If zero is within the limits, the conversion maps one integral value to zero
// exactly. The number of bytes is variable and determines the precision of the
// mapping.
// - string : String separated into bytes regardless of encoding. Null
// characters are not terminators, but a single null byte signifies the empty
// string. This is to differentiate the empty string from the null / unknown
// value, which is indicated for all data types by a byte length of zero.
//
// The functions are templated to be able to handle any iterator type (pointer,
// std::vector, std::deque, etc), different native integer sizes (uint8_t ...
// uint64_t), and different KLV integer sizes (1 ... 8). This hopefully allows
// these functions to be reused in every KLV standard.

#ifndef KWIVER_ARROWS_KLV_KLV_READ_WRITE_H_
#define KWIVER_ARROWS_KLV_KLV_READ_WRITE_H_

#include <arrows/klv/klv_types.h>
#include <arrows/klv/kwiver_algo_klv_export.h>

#include <vital/util/interval.h>

#include <string>

#include <cstdlib>

namespace kwiver {

namespace arrows {

namespace klv {

// ----------------------------------------------------------------------------
/// Read an integer from a sequence of bytes (big-endian).
///
/// This function handles signed and unsigned integers as well as values of \p
/// length which are not powers of 2. It assumes there are \p length bytes
/// available in the source buffer pointed to by \p data.
///
/// \param[in,out] data Iterator to sequence of \c uint8_t. Set to end of read
/// bytes on success, left as is on error.
/// \param length Number of bytes to read.
///
/// \returns Integer read in from \p data.
///
/// \throws metadata_type_overflow When \p length is greater than the number of
///         bytes in the return type \c T.
template < class T >
T
klv_read_int( klv_read_iter_t& data, size_t length );

// ----------------------------------------------------------------------------
/// Write an integer to a sequence of bytes (big-endian).
///
/// This function handles signed and unsigned integers as well as values of \p
/// length which are not powers of 2. Values of \p length which are greater
/// than necessary to represent \p value will result in zero padding on the
/// left. Assumes there are \p length bytes available in the destination buffer
/// pointed to by \p data.
///
/// \param value Integer to write.
/// \param[in,out] data Writeable iterator to sequence of \c uint8_t. Set to
/// end of written bytes on success, left as is on error.
/// \param length Number of bytes to write.
///
/// \throws metadata_type_overflow When \p value is too large to fit in \p
/// length bytes.
template < class T >
void
klv_write_int( T value, klv_write_iter_t& data, size_t length );

// ----------------------------------------------------------------------------
/// Return the number of bytes required to store the given signed or unsigned
/// integer.
///
/// \param value Integer whose byte length is being queried.
///
/// \returns Bytes required to store \p value.
template < class T >
size_t
klv_int_length( T value );

// ----------------------------------------------------------------------------
/// Read an integer from a sequence of bytes, decoding it from BER format.
///
/// For an explanation of BER, see the MISB Motion Imagery Handbook, Section
/// 7.3.1.
/// \see
/// https://gwg.nga.mil/misb/docs/misp/MISP-2020.1_Motion_Imagery_Handbook.pdf
///
/// \param[in,out] data Iterator to sequence of \c uint8_t. Set to end of read
/// bytes on success, left as is on error.
/// \param max_length Maximum number of bytes to read.
///
/// \returns Integer decoded from \p data.
///
/// \throws metadata_buffer_overflow When decoding would require reading more
/// than \p max_length bytes.
/// \throws metadata_type_overflow When the decoded value is too large to fit
/// in the return type \c T.
template < class T >
T
klv_read_ber( klv_read_iter_t& data, size_t max_length );

// ----------------------------------------------------------------------------
/// Write an integer to a sequence of bytes, encoding it into BER format.
///
/// For an explanation of BER, see the MISB Motion Imagery Handbook, Section
/// 7.3.1.
/// \see
/// https://gwg.nga.mil/misb/docs/misp/MISP-2020.1_Motion_Imagery_Handbook.pdf
///
/// \param[in,out] data Writeable iterator to sequence of \c uint8_t. Set to
/// end of written bytes on success, left as is on error.
/// \param max_length Maximum number of bytes to write.
///
/// \throws metadata_buffer_overflow When encoding would require writing more
/// than \p max_length bytes.
template < class T >
void
klv_write_ber( T value, klv_write_iter_t& data, size_t max_length );

// ----------------------------------------------------------------------------
/// Return the number of bytes required to store the given integer in BER
/// format.
///
/// \param value Integer whose byte length is being queried.
///
/// \returns Bytes required to store \p value in BER format.
template < class T >
size_t
klv_ber_length( T value );

// ----------------------------------------------------------------------------
/// Read an integer from a sequence of bytes, decoding it from BER-OID format.
///
/// For an explanation of BER-OID, see the MISB Motion Imagery Handbook,
/// Section 7.3.2.
/// \see
/// https://gwg.nga.mil/misb/docs/misp/MISP-2020.1_Motion_Imagery_Handbook.pdf
///
/// \param[in,out] data Iterator to sequence of \c uint8_t. Set to end of read
/// bytes on success, left as is on error.
/// \param max_length Maximum number of bytes to read.
///
/// \returns Integer decoded from \p data.
///
/// \throws metadata_buffer_overflow When decoding would require reading more
/// than \p max_length bytes.
/// \throws metadata_type_overflow When the decoded value is too large to fit
/// in the return type \c T.
template < class T >
T
klv_read_ber_oid( klv_read_iter_t& data, size_t max_length );

// ----------------------------------------------------------------------------
/// Write an integer to a sequence of bytes, encoding it into BER-OID format.
///
/// For an explanation of BER-OID, see the MISB Motion Imagery Handbook,
/// Section 7.3.2.
/// \see
/// https://gwg.nga.mil/misb/docs/misp/MISP-2020.1_Motion_Imagery_Handbook.pdf
///
/// \param[in,out] data Writeable iterator to sequence of \c uint8_t. Set to
/// end of written bytes on success, left as is on error.
/// \param max_length Maximum number of bytes to write.
///
/// \throws metadata_buffer_overflow When encoding would require writing more
/// than \p max_length bytes.
template < class T >
void
klv_write_ber_oid( T value, klv_write_iter_t& data, size_t max_length );

// ----------------------------------------------------------------------------
/// Return the number of bytes required to store the given integer in BER-OID
/// format.
///
/// For an explanation of BER-OID, see the MISB Motion Imagery Handbook,
/// Section 7.3.2.
/// \see
/// https://gwg.nga.mil/misb/docs/misp/MISP-2020.1_Motion_Imagery_Handbook.pdf
///
/// \param value Integer whose byte length is being queried.
///
/// \returns Bytes required to store \p value in BER format.
template < class T >
size_t
klv_ber_oid_length( T value );

// ----------------------------------------------------------------------------
/// Read an integer from a sequence of bytes and map it to a defined
/// floating-point range.
///
/// This function allows signed or unsigned integer types. The mapping is
/// linear. The range is inclusive on both ends. The entire range of the
/// integer type is used, unless the integer is signed, in which case the
/// lowest representable value is mapped to quiet NaN.
///
/// \param interval Mapped range.
/// \param[in,out] data Iterator to sequence of \c uint8_t. Set to end of
/// read bytes on success, left as is on error.
/// \param length Number of bytes to read.
///
/// \returns Floating-point number mapped from read integer.
///
/// \throws metadata_type_overflow When the read value is too large for \c T.
template < class T >
double
klv_read_flint(
  vital::interval< double > const& interval,
  klv_read_iter_t& data, size_t length );

// ----------------------------------------------------------------------------
/// Map a floating-point number within a range to an integer and write it to a
/// sequence of bytes.
///
/// This function allows signed or unsigned integer types. The mapping is
/// linear. The range is inclusive on both ends. The entire range of the
/// integer type is used, unless the integer is signed, in which case invalid
/// or out-of-range \p value is mapped to the lowest representable integer. For
/// unsigned integers, out-of-range \p value is silently clamped to the range,
/// and NaN \p value is changed to \p minimum.
///
/// \param value Floating-point number to map to an integer.
/// \param interval Mapped range.
/// \param[in,out] data Writeable iterator to sequence of \c uint8_t. Set to
/// end of written bytes on success, left as is on error.
/// \param length Number of bytes to read.
///
/// \throws invalid_value When \p minimum is not less than \p maximum, or if
/// \c T is signed and the range is not symmetrical around zero.
template < class T >
void
klv_write_flint(
  double value, vital::interval< double > const& interval,
  klv_write_iter_t& data, size_t length );

// ----------------------------------------------------------------------------
/// Return the number of bytes required for the given flint specification.
///
/// Precision here is the distance between successive discrete mapped values.
///
/// \param interval Mapped range.
/// \param precision Desired precision of flint value.
///
/// \returns Byte length of a flint value meeting the provided parameters.
///
/// \throws metadata_type_overflow When the difference between \p minimum and
/// \p maximum is to large for a \c double to hold.
KWIVER_ALGO_KLV_EXPORT
size_t
klv_flint_length( vital::interval< double > const& interval, double precision );

// ----------------------------------------------------------------------------
/// Return the precision offered by the given flint specification.
///
/// Precision here is the distance between successive discrete mapped values.
///
/// \param interval Mapped range.
/// \param length Desired byte length of flint value.
///
/// \returns Precision of a flint value meeting the provided parameters.
KWIVER_ALGO_KLV_EXPORT
double
klv_flint_precision( vital::interval< double > const& interval, size_t length );

// ----------------------------------------------------------------------------
/// Read an IEEE-754 floating-point number from a sequence of bytes
/// (big-endian).
///
/// \param[in,out] data Iterator to sequence of \c uint8_t. Set to end of read
/// bytes on success, left as is on error.
/// \param length Number of bytes to read. Must be \c sizeof(float) or \c
/// sizeof(double).
///
/// \returns Floating-point value read from \p data.
///
/// \throws invalid_value When \p length is not \c sizeof(float) or \c
/// sizeof(double).
KWIVER_ALGO_KLV_EXPORT
double
klv_read_float( klv_read_iter_t& data, size_t length );

// ----------------------------------------------------------------------------
/// Write an IEEE-754 floating-point number to a sequence of bytes
/// (big-endian).
///
/// \param[in,out] data Writeable iterator to sequence of \c uint8_t. Set to
/// end of written bytes on success, left as is on error.
/// \param length Number of bytes to write. Must be \c sizeof(float) or \c
/// sizeof(double).
///
/// \throws invalid_value When \p length is not \c sizeof(float) or \c
/// sizeof(double).
KWIVER_ALGO_KLV_EXPORT
void
klv_write_float( double value, klv_write_iter_t& data, size_t length );

// ----------------------------------------------------------------------------
/// Read a string from a sequence of bytes.
///
/// This function performs a straightforward copy, except a single null
/// character is parsed as the empty string. Otherwise, null characters have no
/// special meaning.
///
/// \returns String read from \p data.
KWIVER_ALGO_KLV_EXPORT
std::string
klv_read_string( klv_read_iter_t& data, size_t length );

// ----------------------------------------------------------------------------
/// Write a string to a sequence of bytes.
///
/// This function performs a straightforward copy, except the empty string is
/// written as a single null character. Therefore, an input string consisting
/// of a single null character cannot be written. Otherwise, null characters
/// have no special meaning.
///
/// \param[in,out] data Writeable iterator to sequence of \c uint8_t. Set to
/// end of written bytes on success, left as is on error.
/// \param max_length Maximum number of bytes to write.
///
/// \throws metadata_buffer_overflow When required to write more than \p
/// max_length bytes.
/// \throws metadata_type_overflow When \p value is a single null character.
KWIVER_ALGO_KLV_EXPORT
void
klv_write_string(
  std::string const& value, klv_write_iter_t& data,
  size_t max_length );

// ----------------------------------------------------------------------------
/// Return the number of bytes required to store the given string.
///
/// \param value String whose byte length is being queried.
///
/// \returns Bytes required to store \p value.
///
/// \throws metadata_type_overflow When \p value is a single null character.
KWIVER_ALGO_KLV_EXPORT
size_t
klv_string_length( std::string const& value );

} // namespace klv

} // namespace arrows

} // namespace kwiver

#endif
