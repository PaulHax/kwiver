// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// Implementation of the KLV 0104 parser.

#include <arrows/klv/klv_0104.h>

#include <arrows/klv/klv_string.h>

#include <vital/exceptions.h>

#include <cctype>
#include <ctime>

namespace kv = kwiver::vital;

namespace kwiver {

namespace arrows {

namespace klv {

// ----------------------------------------------------------------------------
std::ostream&
operator<<( std::ostream& os, klv_0104_tag tag )
{
  return os << klv_0104_traits_lookup().by_tag( tag ).name();
}

// ----------------------------------------------------------------------------
klv_0104_universal_set_format
::klv_0104_universal_set_format()
  : klv_universal_set_format{ klv_0104_traits_lookup() }
{}

// ----------------------------------------------------------------------------
std::string
klv_0104_universal_set_format
::description_() const
{
  return "EG0104 Predator UAV US";
}

// ----------------------------------------------------------------------------
klv_uds_key
klv_0104_key()
{
  /// From Section 4.1 of https://gwg.nga.mil/misb/docs/eg/EG0104.5.pdf
  return { 0x060E2B3402010101, 0x0E01010201010000 };
}

// ----------------------------------------------------------------------------
klv_tag_traits_lookup const&
klv_0104_traits_lookup()
{
  // Constants here are taken from Section 8 of
  // https://gwg.nga.mil/misb/docs/standards/ST0601.12.pdf
  // Descriptions are edited for clarity, brevity, consistency, etc.
  // Note that the standard document is 0601, not 0104. This is because 0601
  // eclipsed 0104, which is its deprecated predecessor. Counterintuitively,
  // the most up-to-date information on 0104's fields are found in this version
  // of 0601, the last one to include backwards-compatible information.
  static klv_tag_traits_lookup const lookup = {
    { {},
      ENUM_AND_NAME( KLV_0104_UNKNOWN ),
      std::make_shared< klv_blob_format >(),
      "Unknown",
      "Unknown tag.",
      0 },
    { // "US Key" field
      { 0x060E2B3401010101, 0x0702010101050000 },
      // KWIVER enum
      ENUM_AND_NAME( KLV_0104_USER_DEFINED_TIMESTAMP ),
      // "Type" field: uint64
      std::make_shared< klv_uint_format >( 8 ),
      // "US Name" field
      "User Defined Timestamp",
      // "Notes" field
      "Coordinating Universal Time (UTC) represented in the number of "
      "microseconds elapsed since midnight, January 1, 1970. Derived from the "
      "POSIX IEEE 1003.1 standard.",
      // Mandatory
      1 },
    { { 0x060E2B3401010101, 0x0105050000000000 },
      ENUM_AND_NAME( KLV_0104_EPISODE_NUMBER ),
      std::make_shared< klv_ascii_format >(),
      "Episode Number",
      "Number to distinguish different missions started on a given day.",
      { 0, 1 } },
    { { 0x060E2B3401010107, 0x0701100106000000 },
      ENUM_AND_NAME( KLV_0104_PLATFORM_HEADING_ANGLE ),
      std::make_shared< klv_float_format >( 4 ),
      "Platform Heading Angle",
      "Angle between longitudinal axis and true north measured in the "
      "horizontal plane. Angles increase in a clockwise direction when "
      "looking from above the platform. Measured in degrees.",
      { 0, 1 } },
    { { 0x060E2B3401010107, 0x0701100105000000 },
      ENUM_AND_NAME( KLV_0104_PLATFORM_PITCH_ANGLE ),
      std::make_shared< klv_float_format >( 4 ),
      "Platform Pitch Angle",
      "Angle between longitudinal axis and horizonal plane. Positive angles "
      "above horizonal plane. Measured in degrees.",
      { 0, 1 } },
    { { 0x060E2B3401010107, 0x0701100104000000 },
      ENUM_AND_NAME( KLV_0104_PLATFORM_ROLL_ANGLE ),
      std::make_shared< klv_float_format >( 4 ),
      "Platform Roll Angle",
      "Angle between transverse axis and transverse-longitudinal plane. "
      "Measured in degrees.",
      { 0, 1 } },
    { { 0x060E2B3401010101, 0x0101200100000000 },
      ENUM_AND_NAME( KLV_0104_DEVICE_DESIGNATION ),
      std::make_shared< klv_ascii_format >(),
      "Device Designation",
      "Model name for the platform. Examples: 'Predator', 'Reaper'.",
      { 0, 1 } },
    { { 0x060E2B3401010101, 0x0420010201010000 },
      ENUM_AND_NAME( KLV_0104_IMAGE_SOURCE_DEVICE ),
      std::make_shared< klv_ascii_format >(),
      "Image Source Device",
      "Name of the currently active sensor. Examples: 'EO Nose', "
      "'IR Mitsubishi PtSi Model 500'.",
      { 0, 1 } },
    { { 0x060E2B3401010101, 0x0701010100000000 },
      ENUM_AND_NAME( KLV_0104_IMAGE_COORDINATE_SYSTEM ),
      std::make_shared< klv_ascii_format >(),
      "Image Coordinate System",
      "Name of the image coordinate system used.",
      { 0, 1 } },
    { { 0x060E2B3401010103, 0x0701020102040200 },
      ENUM_AND_NAME( KLV_0104_DEVICE_LATITUDE ),
      std::make_shared< klv_float_format >( 8 ),
      "Device Latitude",
      "Latitude of the currently active sensor, based on WGS84 ellipsoid.",
      { 0, 1 } },
    { { 0x060E2B3401010103, 0x0701020102060200 },
      ENUM_AND_NAME( KLV_0104_DEVICE_LONGITUDE ),
      std::make_shared< klv_float_format >( 8 ),
      "Device Longitude",
      "Longitude of the currently active sensor, based on WGS84 ellipsoid.",
      { 0, 1 } },
    { { 0x060E2B3401010101, 0x0701020102020000 },
      ENUM_AND_NAME( KLV_0104_DEVICE_ALTITUDE ),
      std::make_shared< klv_float_format >(),
      "Device Altitude",
      "Altitude of the currently active sensor, relative to Mean Sea Level. "
      "Measured in meters.",
      { 0, 1 } },
    { { 0x060E2B3401010102, 0x0420020101080000 },
      ENUM_AND_NAME( KLV_0104_HORIZONTAL_FOV ),
      std::make_shared< klv_float_format >( 4 ),
      "Horizontal Field of View",
      "Horizonal field of view of the currently active sensor. "
      "Measured in degrees.",
      { 0, 1 } },
    { { 0x060E2B3401010107, 0x04200201010A0100 },
      ENUM_AND_NAME( KLV_0104_VERTICAL_FOV ),
      std::make_shared< klv_float_format >( 4 ),
      "Vertical Field of View",
      "Vertical field of view of the currently active sensor. "
      "Measured in degrees.",
      { 0, 1 } },
    { { 0x060E2B3401010101, 0x0701080101000000 },
      ENUM_AND_NAME( KLV_0104_SLANT_RANGE ),
      std::make_shared< klv_float_format >(),
      "Slant Range",
      "Distance between currently active sensor and the image center. "
      "Measured in meters.",
      { 0, 1 } },
    { { 0x060E2B3401010101, 0x0701090201000000 },
      ENUM_AND_NAME( KLV_0104_TARGET_WIDTH ),
      std::make_shared< klv_float_format >(),
      "Target Width",
      "Target width within sensor field of view. Measured in meters.",
      { 0, 1 } },
    { { 0x060E2B3401010101, 0x0701020103020000 },
      ENUM_AND_NAME( KLV_0104_FRAME_CENTER_LATITUDE ),
      std::make_shared< klv_float_format >( 8 ),
      "Frame Center Latitude",
      "Latitude of image center, based on WGS84 ellipsoid.",
      { 0, 1 } },
    { { 0x060E2B3401010101, 0x0701020103040000 },
      ENUM_AND_NAME( KLV_0104_FRAME_CENTER_LONGITUDE ),
      std::make_shared< klv_float_format >( 8 ),
      "Frame Center Longitude",
      "Latitude of image center, based on WGS84 ellipsoid.",
      { 0, 1 } },
    { { 0x060E2B340101010A, 0x0701020103160000 },
      ENUM_AND_NAME( KLV_0104_FRAME_CENTER_ELEVATION ),
      std::make_shared< klv_float_format >(),
      "Frame Center Elevation",
      "Elevation of image center, relative to Mean Sea Level.",
      { 0, 1 } },
    { { 0x060E2B3401010103, 0x0701020103070100 },
      ENUM_AND_NAME( KLV_0104_CORNER_LATITUDE_POINT_1 ),
      std::make_shared< klv_float_format >( 8 ),
      "Corner Latitude Point 1",
      "Latitude for the upper left corner of the image.",
      { 0, 1 } },
    { { 0x060E2B3401010103, 0x0701020103080100 },
      ENUM_AND_NAME( KLV_0104_CORNER_LATITUDE_POINT_2 ),
      std::make_shared< klv_float_format >( 8 ),
      "Corner Latitude Point 2",
      "Latitude for the upper right corner of the image.",
      { 0, 1 } },
    { { 0x060E2B3401010103, 0x0701020103090100 },
      ENUM_AND_NAME( KLV_0104_CORNER_LATITUDE_POINT_3 ),
      std::make_shared< klv_float_format >( 8 ),
      "Corner Latitude Point 3",
      "Latitude for the lower right corner of the image.",
      { 0, 1 } },
    { { 0x060E2B3401010103, 0x07010201030A0100 },
      ENUM_AND_NAME( KLV_0104_CORNER_LATITUDE_POINT_4 ),
      std::make_shared< klv_float_format >( 8 ),
      "Corner Latitude Point 4",
      "Latitude for the lower left corner of the image.",
      { 0, 1 } },
    { { 0x060E2B3401010103, 0x07010201030B0100 },
      ENUM_AND_NAME( KLV_0104_CORNER_LONGITUDE_POINT_1 ),
      std::make_shared< klv_float_format >( 8 ),
      "Corner Longitude Point 1",
      "Longitude for the upper left corner of the image.",
      { 0, 1 } },
    { { 0x060E2B3401010103, 0x07010201030C0100 },
      ENUM_AND_NAME( KLV_0104_CORNER_LONGITUDE_POINT_2 ),
      std::make_shared< klv_float_format >( 8 ),
      "Corner Longitude Point 2",
      "Longitude for the upper right corner of the image.",
      { 0, 1 } },
    { { 0x060E2B3401010103, 0x07010201030D0100 },
      ENUM_AND_NAME( KLV_0104_CORNER_LONGITUDE_POINT_3 ),
      std::make_shared< klv_float_format >( 8 ),
      "Corner Longitude Point 3",
      "Longitude for the lower right corner of the image.",
      { 0, 1 } },
    { { 0x060E2B3401010103, 0x07010201030E0100 },
      ENUM_AND_NAME( KLV_0104_CORNER_LONGITUDE_POINT_4 ),
      std::make_shared< klv_float_format >( 8 ),
      "Corner Longitude Point 4",
      "Longitude for the lower left corner of the image.",
      { 0, 1 } },
    { { 0x060E2B3402030101, 0x0E01030302000000 },
      ENUM_AND_NAME( KLV_0104_SECURITY_LOCAL_SET ),
      std::make_shared< klv_blob_format >(),
      "Security Local Set",
      "MISB ST 0102 local set for security metadata.",
      { 0, 1 } },
    { { 0x060E2B3401010101, 0x0702010201010000 },
      ENUM_AND_NAME( KLV_0104_START_DATETIME ),
      std::make_shared< klv_ascii_format >(),
      "Start Datetime",
      "Start time of Motion Imagery Collection. "
      "Format: YYYYMMDDDThhmmss. UTC.",
      { 0, 1 } },
    { { 0x060E2B3401010101, 0x0702010207010000 },
      ENUM_AND_NAME( KLV_0104_EVENT_START_DATETIME ),
      std::make_shared< klv_ascii_format >(),
      "Event Start Datetime",
      "Start time of scene, project, event, mission, editing event, license, "
      "publication, etc. Format: YYYYMMDDDThhmmss. UTC.",
      { 0, 1 } },
    { { 0x060E2B3401010101, 0x0701100101000000 },
      ENUM_AND_NAME( KLV_0104_SENSOR_ROLL_ANGLE ),
      std::make_shared< klv_float_format >( 4 ),
      "Sensor Roll Angle",
      "Angle between sensor pointing direction and transverse-longitudinal "
      "plane. Measured in degrees.",
      { 0, 1 } },
    { { 0x060E2B3401010101, 0x0701100102000000 },
      ENUM_AND_NAME( KLV_0104_ANGLE_TO_NORTH ),
      std::make_shared< klv_float_format >( 4 ),
      "Angle to North",
      "Angle between sensor pointing direction and true north measured in the "
      "horizontal plane. Angles increase in a clockwise direction when "
      "looking from above the platform. Measured in degrees.",
      { 0, 1 } },
    { { 0x060E2B3401010101, 0x0701100103000000 },
      ENUM_AND_NAME( KLV_0104_OBLIQUITY_ANGLE ),
      std::make_shared< klv_float_format >( 4 ),
      "Obliquity Angle",
      "Inverse of sensor elevation angle. Measured in degrees. Examples: "
      "0 degrees is backward, 180 degrees is forward, 270 degrees is down.",
      { 0, 1 } } };

  return lookup;
}

// ----------------------------------------------------------------------------
uint64_t
klv_0104_datetime_to_unix_timestamp( std::string const& value )
{
  try
  {
    // Check datetime length - should be in YYYYMMDD[T]hhmmss format
    // All this would be much cleaner if we could use regex, but we still have
    // to support GCC 4.8.5, which doesn't have C++11 STL regex.
    constexpr size_t length = 15;
    constexpr size_t tless_length = 14;
    if( value.size() != length && value.size() != tless_length )
    {
      VITAL_THROW( kv::metadata_exception, "invalid length" );
    }

    auto const tless = value.size() == tless_length;

    // Check datetime format
    for( size_t i = 0; i < value.size(); ++i )
    {
      auto const c = value[ i ];
      if( ( !tless && i == 8 && c != 'T' ) ||
          ( ( tless || i != 8 ) && !std::isdigit( c ) ) )
      {
        VITAL_THROW( kv::metadata_exception, "invalid format" );
      }
    }

    // Parse datetime fields
    std::tm datetime = {};
    datetime.tm_year = std::stoi( value.substr( 0, 4 ) ) - 1900;
    datetime.tm_mon = std::stoi( value.substr( 4, 2 ) ) - 1;
    datetime.tm_mday = std::stoi( value.substr( 6, 2 ) );
    datetime.tm_hour = std::stoi( value.substr( 9 - tless, 2 ) );
    datetime.tm_min = std::stoi( value.substr( 11 - tless, 2 ) );
    datetime.tm_sec = std::stoi( value.substr( 13 - tless, 2 ) );
    datetime.tm_wday = -1;
    datetime.tm_yday = -1;
    datetime.tm_isdst = -1;

    auto const record = datetime;

    // Call system function to convert UTC -> Unix
    std::time_t const timestamp =
#ifdef _WIN32
      _mkgmtime64( &datetime );
#else
      timegm( &datetime );
#endif

    // Struct will be corrected if given date is invalid (e.g. hour = 24 ->
    // hour = 0 and day += 1). Catch when this happens and throw, since our
    // input is expected to be valid
    if( datetime.tm_year != record.tm_year ||
        datetime.tm_mon  != record.tm_mon ||
        datetime.tm_mday != record.tm_mday ||
        datetime.tm_hour != record.tm_hour ||
        datetime.tm_min  != record.tm_min ||
        datetime.tm_sec  != record.tm_sec )
    {
      VITAL_THROW( kv::metadata_exception, "invalid datetime" );
    }

    // (time_t)(-1) is returned if date is out of representable range.
    // If time_t is signed, negative values are possible (before epoch), but
    // not representable by our uint64_t return type.
    // When C++17 becomes available, use `if constexpr` here to check for signed
    // time_t
    if( timestamp == static_cast< std::time_t >( -1 ) || timestamp < 0 )
    {
      VITAL_THROW( kv::metadata_exception, "out of range" );
    }

    // Convert to microseconds
    return timestamp * 1000000;
  }
  catch( std::exception const& e )
  {
    VITAL_THROW(
      kv::metadata_exception,
      std::string() + "invalid 0104 timestamp: " + e.what() );
  }
}

} // namespace klv

} // namespace arrows

} // namespace kwiver
