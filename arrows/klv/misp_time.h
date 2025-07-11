// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// Declare MISP timestamp utility functions.

// Code here based on the following standards:
// https://gwg.nga.mil/misb/docs/standards/ST0603.5.pdf
// https://gwg.nga.mil/misb/docs/standards/ST0604.3.pdf

#ifndef KWIVER_ARROWS_KLV_MISP_TIME_H
#define KWIVER_ARROWS_KLV_MISP_TIME_H

#include <arrows/klv/klv_types.h>
#include <arrows/klv/kwiver_algo_klv_export.h>

#include <vital/range/iota.h>

#include <algorithm>
#include <chrono>
#include <string>
#include <vector>

#include <cstddef>
#include <cstdint>

namespace kwiver {

namespace arrows {

namespace klv {

namespace misp_detail {

constexpr std::ptrdiff_t tag_length = 16;
constexpr std::ptrdiff_t status_length = 1;
constexpr std::ptrdiff_t timestamp_length = 8 + 3;
constexpr std::ptrdiff_t packet_length =
  tag_length + status_length + timestamp_length;

// Used for MPEG-2 and H.264
uint8_t const tag_string[ tag_length ] = {
  'M', 'I', 'S', 'P', 'm', 'i', 'c', 'r',
  'o', 's', 'e', 'c', 't', 'i', 'm', 'e' };

// Used for H.265
uint8_t const tag_uuid[ tag_length ] = {
  0xA8, 0x68, 0x7D, 0xD4, 0xD7, 0x59, 0x37, 0x58,
  0xA5, 0xCE, 0xF0, 0x33, 0x8B, 0x65, 0x45, 0xF1 };

// Used for H.265
uint8_t const tag_uuid_nano[ tag_length ] = {
  0xCF, 0x84, 0x82, 0x78, 0xEE, 0x23, 0x30, 0x6C,
  0x92, 0x65, 0xE8, 0xFE, 0xF2, 0x2F, 0xB8, 0xB8 };

} // namespace misp_detail

// ----------------------------------------------------------------------------
enum misp_timestamp_tag_type
{
  MISP_TIMESTAMP_TAG_STRING,
  MISP_TIMESTAMP_TAG_UUID,
};

// ----------------------------------------------------------------------------
/// Bit indices for the MISP timestamp status.
enum misp_timestamp_status_bit
{
  // Bits 0-4 reserved for future use; should be set to 1 for now

  // 0 = jump forward in time, 1 = jump backward in time
  MISP_TIMESTAMP_STATUS_BIT_DISCONTINUITY_REVERSE = 5,

  // 0 = normal, 1 = time discontinuity (jump forward or backward)
  MISP_TIMESTAMP_STATUS_BIT_DISCONTINUITY = 6,

  // 0 = time is locked to absolute reference, 1 = time may not be locked
  MISP_TIMESTAMP_STATUS_BIT_NOT_LOCKED = 7,

  MISP_TIMESTAMP_STATUS_BIT_ENUM_END,
};

// ----------------------------------------------------------------------------
/// Frame timestamp information embedded in the video stream.
class KWIVER_ALGO_KLV_EXPORT misp_timestamp
{
public:
  static constexpr uint8_t default_status = 0x9F;

  misp_timestamp();

  misp_timestamp(
    std::chrono::microseconds timestamp, uint8_t status = default_status );

  misp_timestamp(
    std::chrono::nanoseconds timestamp, uint8_t status = default_status );

  std::chrono::microseconds microseconds() const;
  void set_microseconds( std::chrono::microseconds microseconds );

  std::chrono::nanoseconds nanoseconds() const;
  void set_nanoseconds( std::chrono::nanoseconds nanoseconds );

  uint8_t status() const;
  void set_status( uint8_t status );

  bool has_nanoseconds() const;

private:
  std::chrono::nanoseconds m_timestamp;
  uint8_t m_status;
  bool m_has_nanoseconds;
};

// ----------------------------------------------------------------------------
/// Locate a MISP microsecond timestamp packet in a sequence of bytes.
///
/// \param begin Iterator to beginning of byte sequence.
/// \param end Iterator to end of byte sequence.
///
/// \return Iterator to beginning of MISP packet, or \p end on failure.
KWIVER_ALGO_KLV_EXPORT
klv_read_iter_t
find_misp_timestamp(
  klv_read_iter_t begin, klv_read_iter_t end,
  misp_timestamp_tag_type tag_type );

// ----------------------------------------------------------------------------
/// Determine whether the MISP timestamp at \p data is in nanoseconds.
///
/// \param data Iterator to beginning of MISP packet.
///
/// \return
///   \c true if the timestamp uses nanoseconds, \c false if it uses
///   microseconds.
KWIVER_ALGO_KLV_EXPORT
bool
is_misp_timestamp_nano( klv_read_iter_t data );

// ----------------------------------------------------------------------------
/// Read a MISP timestamp from a sequence of bytes.
///
/// \param data
///   Iterator to beginning of MISP packet. Set to end of read bytes on success.
///
/// \return MISP timestamp.
KWIVER_ALGO_KLV_EXPORT
misp_timestamp
read_misp_timestamp( klv_read_iter_t& data );

// ----------------------------------------------------------------------------
/// Write a MISP timestamp to a sequence of bytes.
///
/// \param value Timestamp value to write.
/// \param data
///   Iterator to sequence of \c uint8_t. Set to end of written bytes on
///   success.
KWIVER_ALGO_KLV_EXPORT
void
write_misp_timestamp(
  misp_timestamp value, klv_write_iter_t& data,
  misp_timestamp_tag_type tag_type, bool is_nano );

// ----------------------------------------------------------------------------
/// Return the length of a MISP timestamp packet in bytes.
KWIVER_ALGO_KLV_EXPORT
size_t
misp_timestamp_length();

// ----------------------------------------------------------------------------
/// Returns the current time in microseconds according to the MISP system.
///
/// \warning Until C++20, we cannot guarantee the accuracy of the result.
KWIVER_ALGO_KLV_EXPORT
std::chrono::microseconds
misp_microseconds_now();

// ----------------------------------------------------------------------------
/// Returns the current time in nanoseconds according to the MISP system.
///
/// \warning Until C++20, we cannot guarantee the accuracy of the result.
KWIVER_ALGO_KLV_EXPORT
std::chrono::nanoseconds
misp_nanoseconds_now();

} // namespace klv

} // namespace arrows

} // namespace kwiver

#endif
