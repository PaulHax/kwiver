// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// Interface to the KLV 0601 parser.

#ifndef KWIVER_ARROWS_KLV_KLV_0601_H_
#define KWIVER_ARROWS_KLV_KLV_0601_H_

#include <arrows/klv/kwiver_algo_klv_export.h>

#include "klv_0102.h"
#include "klv_checksum.h"
#include "klv_imap.h"
#include "klv_list.h"
#include "klv_packet.h"
#include "klv_series.h"
#include "klv_set.h"
#include "klv_util.h"

#include <optional>
#include <ostream>

namespace kwiver {

namespace arrows {

namespace klv {

// ----------------------------------------------------------------------------
/// Tag values for the ST0601 local set.
enum klv_0601_tag : klv_lds_key
{
  KLV_0601_UNKNOWN                             = 0,
  KLV_0601_CHECKSUM                            = 1,
  KLV_0601_PRECISION_TIMESTAMP                 = 2,
  KLV_0601_MISSION_ID                          = 3,
  KLV_0601_PLATFORM_TAIL_NUMBER                = 4,
  KLV_0601_PLATFORM_HEADING_ANGLE              = 5,
  KLV_0601_PLATFORM_PITCH_ANGLE                = 6,
  KLV_0601_PLATFORM_ROLL_ANGLE                 = 7,
  KLV_0601_PLATFORM_TRUE_AIRSPEED              = 8,
  KLV_0601_PLATFORM_INDICATED_AIRSPEED         = 9,
  KLV_0601_PLATFORM_DESIGNATION                = 10,
  KLV_0601_IMAGE_SOURCE_SENSOR                 = 11,
  KLV_0601_IMAGE_COORDINATE_SYSTEM             = 12,
  KLV_0601_SENSOR_LATITUDE                     = 13,
  KLV_0601_SENSOR_LONGITUDE                    = 14,
  KLV_0601_SENSOR_TRUE_ALTITUDE                = 15,
  KLV_0601_SENSOR_HORIZONTAL_FOV               = 16,
  KLV_0601_SENSOR_VERTICAL_FOV                 = 17,
  KLV_0601_SENSOR_RELATIVE_AZIMUTH_ANGLE       = 18,
  KLV_0601_SENSOR_RELATIVE_ELEVATION_ANGLE     = 19,
  KLV_0601_SENSOR_RELATIVE_ROLL_ANGLE          = 20,
  KLV_0601_SLANT_RANGE                         = 21,
  KLV_0601_TARGET_WIDTH                        = 22,
  KLV_0601_FRAME_CENTER_LATITUDE               = 23,
  KLV_0601_FRAME_CENTER_LONGITUDE              = 24,
  KLV_0601_FRAME_CENTER_ELEVATION              = 25,
  KLV_0601_OFFSET_CORNER_LATITUDE_POINT_1      = 26,
  KLV_0601_OFFSET_CORNER_LONGITUDE_POINT_1     = 27,
  KLV_0601_OFFSET_CORNER_LATITUDE_POINT_2      = 28,
  KLV_0601_OFFSET_CORNER_LONGITUDE_POINT_2     = 29,
  KLV_0601_OFFSET_CORNER_LATITUDE_POINT_3      = 30,
  KLV_0601_OFFSET_CORNER_LONGITUDE_POINT_3     = 31,
  KLV_0601_OFFSET_CORNER_LATITUDE_POINT_4      = 32,
  KLV_0601_OFFSET_CORNER_LONGITUDE_POINT_4     = 33,
  KLV_0601_ICING_DETECTED                      = 34,
  KLV_0601_WIND_DIRECTION                      = 35,
  KLV_0601_WIND_SPEED                          = 36,
  KLV_0601_STATIC_PRESSURE                     = 37,
  KLV_0601_DENSITY_ALTITUDE                    = 38,
  KLV_0601_OUTSIDE_AIR_TEMPERATURE             = 39,
  KLV_0601_TARGET_LOCATION_LATITUDE            = 40,
  KLV_0601_TARGET_LOCATION_LONGITUDE           = 41,
  KLV_0601_TARGET_LOCATION_ELEVATION           = 42,
  KLV_0601_TARGET_TRACK_GATE_WIDTH             = 43,
  KLV_0601_TARGET_TRACK_GATE_HEIGHT            = 44,
  KLV_0601_TARGET_ERROR_ESTIMATE_CE90          = 45,
  KLV_0601_TARGET_ERROR_ESTIMATE_LE90          = 46,
  KLV_0601_GENERIC_FLAG_DATA                   = 47,
  KLV_0601_SECURITY_LOCAL_SET                  = 48,
  KLV_0601_DIFFERENTIAL_PRESSURE               = 49,
  KLV_0601_PLATFORM_ANGLE_OF_ATTACK            = 50,
  KLV_0601_PLATFORM_VERTICAL_SPEED             = 51,
  KLV_0601_PLATFORM_SIDESLIP_ANGLE             = 52,
  KLV_0601_AIRFIELD_BAROMETRIC_PRESSURE        = 53,
  KLV_0601_AIRFIELD_ELEVATION                  = 54,
  KLV_0601_RELATIVE_HUMIDITY                   = 55,
  KLV_0601_PLATFORM_GROUND_SPEED               = 56,
  KLV_0601_GROUND_RANGE                        = 57,
  KLV_0601_PLATFORM_FUEL_REMAINING             = 58,
  KLV_0601_PLATFORM_CALL_SIGN                  = 59,
  KLV_0601_WEAPON_LOAD                         = 60,
  KLV_0601_WEAPON_FIRED                        = 61,
  KLV_0601_LASER_PRF_CODE                      = 62,
  KLV_0601_SENSOR_FOV_NAME                     = 63,
  KLV_0601_PLATFORM_MAGNETIC_HEADING           = 64,
  KLV_0601_VERSION_NUMBER                      = 65,
  KLV_0601_DEPRECATED                          = 66,
  KLV_0601_ALTERNATE_PLATFORM_LATITUDE         = 67,
  KLV_0601_ALTERNATE_PLATFORM_LONGITUDE        = 68,
  KLV_0601_ALTERNATE_PLATFORM_ALTITUDE         = 69,
  KLV_0601_ALTERNATE_PLATFORM_NAME             = 70,
  KLV_0601_ALTERNATE_PLATFORM_HEADING          = 71,
  KLV_0601_EVENT_START_TIME                    = 72,
  KLV_0601_RVT_LOCAL_SET                       = 73,
  KLV_0601_VMTI_LOCAL_SET                      = 74,
  KLV_0601_SENSOR_ELLIPSOID_HEIGHT             = 75,
  KLV_0601_ALTERNATE_PLATFORM_ELLIPSOID_HEIGHT = 76,
  KLV_0601_OPERATIONAL_MODE                    = 77,
  KLV_0601_FRAME_CENTER_ELLIPSOID_HEIGHT       = 78,
  KLV_0601_SENSOR_NORTH_VELOCITY               = 79,
  KLV_0601_SENSOR_EAST_VELOCITY                = 80,
  KLV_0601_IMAGE_HORIZON_PIXEL_PACK            = 81,
  KLV_0601_FULL_CORNER_LATITUDE_POINT_1        = 82,
  KLV_0601_FULL_CORNER_LONGITUDE_POINT_1       = 83,
  KLV_0601_FULL_CORNER_LATITUDE_POINT_2        = 84,
  KLV_0601_FULL_CORNER_LONGITUDE_POINT_2       = 85,
  KLV_0601_FULL_CORNER_LATITUDE_POINT_3        = 86,
  KLV_0601_FULL_CORNER_LONGITUDE_POINT_3       = 87,
  KLV_0601_FULL_CORNER_LATITUDE_POINT_4        = 88,
  KLV_0601_FULL_CORNER_LONGITUDE_POINT_4       = 89,
  KLV_0601_FULL_PLATFORM_PITCH_ANGLE           = 90,
  KLV_0601_FULL_PLATFORM_ROLL_ANGLE            = 91,
  KLV_0601_FULL_PLATFORM_ANGLE_OF_ATTACK       = 92,
  KLV_0601_FULL_PLATFORM_SIDESLIP_ANGLE        = 93,
  KLV_0601_MIIS_CORE_IDENTIFIER                = 94,
  KLV_0601_SAR_MOTION_IMAGERY_LOCAL_SET        = 95,
  KLV_0601_TARGET_WIDTH_EXTENDED               = 96,
  KLV_0601_RANGE_IMAGE_LOCAL_SET               = 97,
  KLV_0601_GEOREGISTRATION_LOCAL_SET           = 98,
  KLV_0601_COMPOSITE_IMAGING_LOCAL_SET         = 99,
  KLV_0601_SEGMENT_LOCAL_SET                   = 100,
  KLV_0601_AMEND_LOCAL_SET                     = 101,
  KLV_0601_SDCC_FLP                            = 102,
  KLV_0601_DENSITY_ALTITUDE_EXTENDED           = 103,
  KLV_0601_SENSOR_ELLIPSOID_HEIGHT_EXTENDED    = 104,
  KLV_0601_ALTERNATE_PLATFORM_ELLIPSOID_HEIGHT_EXTENDED = 105,
  KLV_0601_STREAM_DESIGNATOR                   = 106,
  KLV_0601_OPERATIONAL_BASE                    = 107,
  KLV_0601_BROADCAST_SOURCE                    = 108,
  KLV_0601_RANGE_TO_RECOVERY_LOCATION          = 109,
  KLV_0601_TIME_AIRBORNE                       = 110,
  KLV_0601_PROPULSION_UNIT_SPEED               = 111,
  KLV_0601_PLATFORM_COURSE_ANGLE               = 112,
  KLV_0601_ALTITUDE_ABOVE_GROUND_LEVEL         = 113,
  KLV_0601_RADAR_ALTIMETER                     = 114,
  KLV_0601_CONTROL_COMMAND                     = 115,
  KLV_0601_CONTROL_COMMAND_VERIFICATION_LIST   = 116,
  KLV_0601_SENSOR_AZIMUTH_RATE                 = 117,
  KLV_0601_SENSOR_ELEVATION_RATE               = 118,
  KLV_0601_SENSOR_ROLL_RATE                    = 119,
  KLV_0601_ONBOARD_MI_STORAGE_PERCENT_FULL     = 120,
  KLV_0601_ACTIVE_WAVELENGTH_LIST              = 121,
  KLV_0601_COUNTRY_CODES                       = 122,
  KLV_0601_NUMBER_OF_NAVSATS_IN_VIEW           = 123,
  KLV_0601_POSITIONING_METHOD_SOURCE           = 124,
  KLV_0601_PLATFORM_STATUS                     = 125,
  KLV_0601_SENSOR_CONTROL_MODE                 = 126,
  KLV_0601_SENSOR_FRAME_RATE_PACK              = 127,
  KLV_0601_WAVELENGTHS_LIST                    = 128,
  KLV_0601_TARGET_ID                           = 129,
  KLV_0601_AIRBASE_LOCATIONS                   = 130,
  KLV_0601_TAKEOFF_TIME                        = 131,
  KLV_0601_TRANSMISSION_FREQUENCY              = 132,
  KLV_0601_ONBOARD_MI_STORAGE_CAPACITY         = 133,
  KLV_0601_ZOOM_PERCENTAGE                     = 134,
  KLV_0601_COMMUNICATIONS_METHOD               = 135,
  KLV_0601_LEAP_SECONDS                        = 136,
  KLV_0601_CORRECTION_OFFSET                   = 137,
  KLV_0601_PAYLOAD_LIST                        = 138,
  KLV_0601_ACTIVE_PAYLOADS                     = 139,
  KLV_0601_WEAPONS_STORES                      = 140,
  KLV_0601_WAYPOINT_LIST                       = 141,
  KLV_0601_VIEW_DOMAIN                         = 142,
  KLV_0601_METADATA_SUBSTREAM_ID               = 143,
  KLV_0601_ENUM_END,
};

// ----------------------------------------------------------------------------
KWIVER_ALGO_KLV_EXPORT
std::ostream&
operator<<( std::ostream& os, klv_0601_tag tag );

// ----------------------------------------------------------------------------
/// Indicates whether ice forming on the aircraft has been detected.
enum KWIVER_ALGO_KLV_EXPORT klv_0601_icing_detected
{
  KLV_0601_ICING_DETECTED_DETECTOR_OFF,
  KLV_0601_ICING_DETECTED_FALSE,
  KLV_0601_ICING_DETECTED_TRUE,
  KLV_0601_ICING_DETECTED_ENUM_END,
};

// ----------------------------------------------------------------------------
/// Interprets data as a ST0601 icing detection status.
using klv_0601_icing_detected_format =
  klv_enum_format< klv_0601_icing_detected >;

// ----------------------------------------------------------------------------
KWIVER_ALGO_KLV_EXPORT
std::ostream&
operator<<( std::ostream& os, klv_0601_icing_detected value );

// ----------------------------------------------------------------------------
/// Indicates one of several discrete zoom levels.
enum KWIVER_ALGO_KLV_EXPORT klv_0601_sensor_fov_name
{
  KLV_0601_SENSOR_FOV_NAME_ULTRANARROW,
  KLV_0601_SENSOR_FOV_NAME_NARROW,
  KLV_0601_SENSOR_FOV_NAME_MEDIUM,
  KLV_0601_SENSOR_FOV_NAME_WIDE,
  KLV_0601_SENSOR_FOV_NAME_ULTRAWIDE,
  KLV_0601_SENSOR_FOV_NAME_NARROW_MEDIUM,
  KLV_0601_SENSOR_FOV_NAME_2X_ULTRANARROW,
  KLV_0601_SENSOR_FOV_NAME_4X_ULTRANARROW,
  KLV_0601_SENSOR_FOV_NAME_CONTINUOUS_ZOOM,
  KLV_0601_SENSOR_FOV_NAME_ENUM_END,
};

// ----------------------------------------------------------------------------
/// Interprets data as a ST0601 discrete sensor field of view.
using klv_0601_sensor_fov_name_format =
  klv_enum_format< klv_0601_sensor_fov_name >;

// ----------------------------------------------------------------------------
KWIVER_ALGO_KLV_EXPORT
std::ostream&
operator<<( std::ostream& os, klv_0601_sensor_fov_name value );

// ----------------------------------------------------------------------------
/// Source of the navigation positioning information.
enum KWIVER_ALGO_KLV_EXPORT klv_0601_positioning_method_source_bit
{
  KLV_0601_POSITIONING_METHOD_SOURCE_BIT_ON_BOARD_INS,
  KLV_0601_POSITIONING_METHOD_SOURCE_BIT_GPS,
  KLV_0601_POSITIONING_METHOD_SOURCE_BIT_GALILEO,
  KLV_0601_POSITIONING_METHOD_SOURCE_BIT_QZSS,
  KLV_0601_POSITIONING_METHOD_SOURCE_BIT_NAVIC,
  KLV_0601_POSITIONING_METHOD_SOURCE_BIT_GLONASS,
  KLV_0601_POSITIONING_METHOD_SOURCE_BIT_BEIDOU_1,
  KLV_0601_POSITIONING_METHOD_SOURCE_BIT_BEIDOU_2,
  KLV_0601_POSITIONING_METHOD_SOURCE_BIT_ENUM_END,
};

// ----------------------------------------------------------------------------
KWIVER_ALGO_KLV_EXPORT
std::ostream&
operator<<( std::ostream& os, klv_0601_positioning_method_source_bit value );

// ----------------------------------------------------------------------------
using klv_0601_positioning_method_source_format =
  klv_enum_bitfield_format< klv_0601_positioning_method_source_bit >;

// ----------------------------------------------------------------------------
/// Miscellaneous boolean values.
enum KWIVER_ALGO_KLV_EXPORT klv_0601_generic_flag_data_bit
{
  // 0 = laser off, 1 = laser on
  KLV_0601_GENERIC_FLAG_DATA_BIT_LASER_RANGE,
  // 0 = auto-track off, 1 = auto-track on
  KLV_0601_GENERIC_FLAG_DATA_BIT_AUTO_TRACK,
  // 0 = black hot, 1 = white hot
  KLV_0601_GENERIC_FLAG_DATA_BIT_IR_POLARITY,
  // 0 = no icing detected, 1 = icing detected
  KLV_0601_GENERIC_FLAG_DATA_BIT_ICING_STATUS,
  // 0 = slant range calculated, 1 = slant range measured
  KLV_0601_GENERIC_FLAG_DATA_BIT_SLANT_RANGE,
  // 0 = image valid, 1 = image invalid
  KLV_0601_GENERIC_FLAG_DATA_BIT_IMAGE_INVALID,
  KLV_0601_GENERIC_FLAG_DATA_BIT_ENUM_END,
};

// ----------------------------------------------------------------------------
KWIVER_ALGO_KLV_EXPORT
std::ostream&
operator<<( std::ostream& os, klv_0601_generic_flag_data_bit value );

// ----------------------------------------------------------------------------
using klv_0601_generic_flag_data_format =
  klv_enum_bitfield_format< klv_0601_generic_flag_data_bit >;

// ----------------------------------------------------------------------------
/// Indicates the operational mode of the FMV-producing platform.
enum KWIVER_ALGO_KLV_EXPORT klv_0601_operational_mode
{
  KLV_0601_OPERATIONAL_MODE_OTHER,
  KLV_0601_OPERATIONAL_MODE_OPERATIONAL,
  KLV_0601_OPERATIONAL_MODE_TRAINING,
  KLV_0601_OPERATIONAL_MODE_EXERCISE,
  KLV_0601_OPERATIONAL_MODE_MAINTENANCE,
  KLV_0601_OPERATIONAL_MODE_TEST,
  KLV_0601_OPERATIONAL_MODE_ENUM_END,
};

// ----------------------------------------------------------------------------
/// Interprets data as a ST0601 UAV operational mode.
using klv_0601_operational_mode_format =
  klv_enum_format< klv_0601_operational_mode >;

// ----------------------------------------------------------------------------
KWIVER_ALGO_KLV_EXPORT
std::ostream&
operator<<( std::ostream& os, klv_0601_operational_mode value );

// ----------------------------------------------------------------------------
/// Geodetic locations of the two points where the horizon meets image frame.
///
/// Used in tag 81 (Image Horizon Pixel Pack) of ST0601.
struct KWIVER_ALGO_KLV_EXPORT klv_0601_image_horizon_locations
{
  double latitude0;
  double longitude0;
  double latitude1;
  double longitude1;
};

// ----------------------------------------------------------------------------
KWIVER_ALGO_KLV_EXPORT
std::ostream&
operator<<( std::ostream& os, klv_0601_image_horizon_locations const& value );

// ----------------------------------------------------------------------------
DECLARE_CMP( klv_0601_image_horizon_locations )

// ----------------------------------------------------------------------------
/// Interprets data as the geodetic locations for an Image Horizon Pixel Pack.
class KWIVER_ALGO_KLV_EXPORT klv_0601_image_horizon_locations_format
  : public klv_data_format_< klv_0601_image_horizon_locations >
{
public:
  klv_0601_image_horizon_locations_format();

  std::string
  description_() const override;

private:
  klv_0601_image_horizon_locations
  read_typed( klv_read_iter_t& data, size_t length ) const override;

  void
  write_typed(
    klv_0601_image_horizon_locations const& value,
    klv_write_iter_t& data, size_t length ) const override;

  size_t
  length_of_typed(
    klv_0601_image_horizon_locations const& value ) const override;
};

// ----------------------------------------------------------------------------
/// Image-space and geodetic locations of the two points where the horizon
/// intersects the image frame.
struct KWIVER_ALGO_KLV_EXPORT klv_0601_image_horizon_pixel_pack
{
  uint8_t x0;
  uint8_t y0;
  uint8_t x1;
  uint8_t y1;
  std::optional< klv_0601_image_horizon_locations > locations;
};

// ----------------------------------------------------------------------------
KWIVER_ALGO_KLV_EXPORT
std::ostream&
operator<<( std::ostream& os, klv_0601_image_horizon_pixel_pack const& value );

// ----------------------------------------------------------------------------
DECLARE_CMP( klv_0601_image_horizon_pixel_pack )

// ----------------------------------------------------------------------------
/// Interprets data as an image horizon pixel pack.
class KWIVER_ALGO_KLV_EXPORT klv_0601_image_horizon_pixel_pack_format
  : public klv_data_format_< klv_0601_image_horizon_pixel_pack >
{
public:
  klv_0601_image_horizon_pixel_pack_format();

  std::string
  description_() const override;

private:
  klv_0601_image_horizon_pixel_pack
  read_typed( klv_read_iter_t& data, size_t length ) const override;

  void
  write_typed(
    klv_0601_image_horizon_pixel_pack const& value,
    klv_write_iter_t& data, size_t length ) const override;

  size_t
  length_of_typed(
    klv_0601_image_horizon_pixel_pack const& value ) const override;
};

// ----------------------------------------------------------------------------
/// Indicates the general status of the aircraft.
enum KWIVER_ALGO_KLV_EXPORT klv_0601_platform_status
{
  KLV_0601_PLATFORM_STATUS_ACTIVE,
  KLV_0601_PLATFORM_STATUS_PREFLIGHT,
  KLV_0601_PLATFORM_STATUS_PREFLIGHT_TAXIING,
  KLV_0601_PLATFORM_STATUS_RUNUP,
  KLV_0601_PLATFORM_STATUS_TAKEOFF,
  KLV_0601_PLATFORM_STATUS_INGRESS,
  KLV_0601_PLATFORM_STATUS_MANUAL_OPERATION,
  KLV_0601_PLATFORM_STATUS_AUTOMATED_ORBIT,
  KLV_0601_PLATFORM_STATUS_TRANSITIONING,
  KLV_0601_PLATFORM_STATUS_EGRESS,
  KLV_0601_PLATFORM_STATUS_LANDING,
  KLV_0601_PLATFORM_STATUS_LANDING_TAXIING,
  KLV_0601_PLATFORM_STATUS_LANDED_PARKED,
  KLV_0601_PLATFORM_STATUS_ENUM_END,
};

// ----------------------------------------------------------------------------
/// Interprets data as a ST0601 UAV platform status.
using klv_0601_platform_status_format =
  klv_enum_format< klv_0601_platform_status >;

// ----------------------------------------------------------------------------
KWIVER_ALGO_KLV_EXPORT
std::ostream&
operator<<( std::ostream& os, klv_0601_platform_status value );

// ----------------------------------------------------------------------------
/// Indicates how the sensor is being operated.
enum KWIVER_ALGO_KLV_EXPORT klv_0601_sensor_control_mode
{
  KLV_0601_SENSOR_CONTROL_MODE_OFF,
  KLV_0601_SENSOR_CONTROL_MODE_HOME_POSITION,
  KLV_0601_SENSOR_CONTROL_MODE_UNCONTROLLED,
  KLV_0601_SENSOR_CONTROL_MODE_MANUAL_CONTROL,
  KLV_0601_SENSOR_CONTROL_MODE_CALIBRATING,
  KLV_0601_SENSOR_CONTROL_MODE_AUTO_HOLDING_POSITION,
  KLV_0601_SENSOR_CONTROL_MODE_AUTO_TRACKING,
  KLV_0601_SENSOR_CONTROL_MODE_ENUM_END,
};

// ----------------------------------------------------------------------------
/// Interprets data as a ST0601 sensor control mode.
using klv_0601_sensor_control_mode_format =
  klv_enum_format< klv_0601_sensor_control_mode >;

// ----------------------------------------------------------------------------
KWIVER_ALGO_KLV_EXPORT
std::ostream&
operator<<( std::ostream& os, klv_0601_sensor_control_mode value );

// ----------------------------------------------------------------------------
/// Record of command sent to UAV.
struct KWIVER_ALGO_KLV_EXPORT klv_0601_control_command
{
  uint16_t id;
  std::string string;
  std::optional< uint64_t > timestamp;
};

// ----------------------------------------------------------------------------
KWIVER_ALGO_KLV_EXPORT
std::ostream&
operator<<( std::ostream& os, klv_0601_control_command const& value );

// ----------------------------------------------------------------------------
DECLARE_CMP( klv_0601_control_command )

// ----------------------------------------------------------------------------
/// Interprets data as a ST0601 control command.
class KWIVER_ALGO_KLV_EXPORT klv_0601_control_command_format
  : public klv_data_format_< klv_0601_control_command >
{
public:
  klv_0601_control_command_format();

  std::string
  description_() const override;

private:
  klv_0601_control_command
  read_typed( klv_read_iter_t& data, size_t length ) const override;

  void
  write_typed(
    klv_0601_control_command const& value,
    klv_write_iter_t& data, size_t length ) const override;

  size_t
  length_of_typed( klv_0601_control_command const& value ) const override;
};

// ---------------------------------------------------------------------------
/// Interprets data as a ST0601 control command verification list.
using klv_0601_control_command_verify_list_format =
  klv_list_format< klv_ber_oid_format >;

// ----------------------------------------------------------------------------
/// Frame rate expressed as a ratio of integers.
struct KWIVER_ALGO_KLV_EXPORT klv_0601_frame_rate
{
  uint32_t numerator;
  uint32_t denominator;
};

// ----------------------------------------------------------------------------
KWIVER_ALGO_KLV_EXPORT
std::ostream&
operator<<( std::ostream& os, klv_0601_frame_rate const& value );

// ----------------------------------------------------------------------------
DECLARE_CMP( klv_0601_frame_rate )

// ----------------------------------------------------------------------------
/// Interprets data as a frame rate.
class KWIVER_ALGO_KLV_EXPORT klv_0601_frame_rate_format
  : public klv_data_format_< klv_0601_frame_rate >
{
public:
  klv_0601_frame_rate_format();

  std::string
  description_() const override;

private:
  klv_0601_frame_rate
  read_typed( klv_read_iter_t& data, size_t length ) const override;

  void
  write_typed(
    klv_0601_frame_rate const& value,
    klv_write_iter_t& data, size_t length ) const override;

  size_t
  length_of_typed( klv_0601_frame_rate const& value ) const override;
};

// ----------------------------------------------------------------------------
/// Record of involvement of various countries in production of the FMV.
struct klv_0601_country_codes
{
  klv_0102_country_coding_method coding_method;
  std::optional< std::string > overflight_country;
  std::optional< std::string > operator_country;
  std::optional< std::string > country_of_manufacture;
};

// ----------------------------------------------------------------------------
KWIVER_ALGO_KLV_EXPORT
std::ostream&
operator<<( std::ostream& os, klv_0601_country_codes const& value );

// ----------------------------------------------------------------------------
DECLARE_CMP( klv_0601_country_codes )

// ----------------------------------------------------------------------------
/// Interprets data as country codes.
class KWIVER_ALGO_KLV_EXPORT klv_0601_country_codes_format
  : public klv_data_format_< klv_0601_country_codes >
{
public:
  klv_0601_country_codes_format();

  std::string
  description_() const override;

private:
  klv_0601_country_codes
  read_typed( klv_read_iter_t& data, size_t length ) const override;

  void
  write_typed(
    klv_0601_country_codes const& value,
    klv_write_iter_t& data, size_t length ) const override;

  size_t
  length_of_typed( klv_0601_country_codes const& value ) const override;
};

// ----------------------------------------------------------------------------
/// Geographic location.
struct klv_0601_location
{
  klv_imap latitude;
  klv_imap longitude;
  std::optional< klv_imap > altitude;
};

// ----------------------------------------------------------------------------
KWIVER_ALGO_KLV_EXPORT
std::ostream&
operator<<( std::ostream& os, klv_0601_location const& value );

// ----------------------------------------------------------------------------
DECLARE_CMP( klv_0601_location )

// ----------------------------------------------------------------------------
class KWIVER_ALGO_KLV_EXPORT klv_0601_location_format
  : public klv_data_format_< klv_0601_location >
{
public:
  klv_0601_location_format();

  std::string
  description_() const override;

private:
  klv_0601_location
  read_typed( klv_read_iter_t& data, size_t length ) const override;

  void
  write_typed(
    klv_0601_location const& value,
    klv_write_iter_t& data, size_t length ) const override;

  size_t
  length_of_typed( klv_0601_location const& value ) const override;
};

// ----------------------------------------------------------------------------
/// Geographic location of the take-off site and recovery site.
struct klv_0601_airbase_locations
{
  std::optional< klv_0601_location > take_off_location;
  std::optional< klv_0601_location > recovery_location;
};

// ----------------------------------------------------------------------------
KWIVER_ALGO_KLV_EXPORT
std::ostream&
operator<<( std::ostream& os, klv_0601_airbase_locations const& value );

// ----------------------------------------------------------------------------
DECLARE_CMP( klv_0601_airbase_locations )

// ----------------------------------------------------------------------------
/// Interprets data as airbase locations.
class KWIVER_ALGO_KLV_EXPORT klv_0601_airbase_locations_format
  : public klv_data_format_< klv_0601_airbase_locations >
{
public:
  klv_0601_airbase_locations_format();

  std::string
  description_() const override;

private:
  klv_0601_airbase_locations
  read_typed( klv_read_iter_t& data, size_t length ) const override;

  void
  write_typed(
    klv_0601_airbase_locations const& value,
    klv_write_iter_t& data, size_t length ) const override;

  size_t
  length_of_typed( klv_0601_airbase_locations const& value ) const override;
};

// ----------------------------------------------------------------------------
/// Angular interval used in tag 142 (View Domain) of ST0601.
struct klv_0601_view_domain_interval
{
  klv_imap start;
  klv_imap range;

  // Byte length of one of (start, range), or half the length of the whole pack.
  size_t semi_length;
};

// ----------------------------------------------------------------------------
KWIVER_ALGO_KLV_EXPORT
std::ostream&
operator<<( std::ostream& os, klv_0601_view_domain_interval const& value );

// ----------------------------------------------------------------------------
DECLARE_CMP( klv_0601_view_domain_interval )

// ---------------------------------------------------------------------------
/// Interprets data as a view domain interval.
class KWIVER_ALGO_KLV_EXPORT klv_0601_view_domain_interval_format
  : public klv_data_format_< klv_0601_view_domain_interval >
{
public:
  klv_0601_view_domain_interval_format(
    vital::interval< double > const& start_interval );

  std::string
  description_() const override;

private:
  klv_0601_view_domain_interval
  read_typed( klv_read_iter_t& data, size_t length ) const override;

  void
  write_typed(
    klv_0601_view_domain_interval const& value,
    klv_write_iter_t& data, size_t length ) const override;

  size_t
  length_of_typed( klv_0601_view_domain_interval const& value ) const override;

  klv_imap_format const m_start_format;
  static klv_imap_format const range_format;
};

// ----------------------------------------------------------------------------
/// Specifies the domain of values for
/// Relative Sensor Azimuth, Elevation and Roll Angles.
struct klv_0601_view_domain
{
  std::optional< klv_0601_view_domain_interval > azimuth;
  std::optional< klv_0601_view_domain_interval > elevation;
  std::optional< klv_0601_view_domain_interval > roll;
};

// ----------------------------------------------------------------------------
KWIVER_ALGO_KLV_EXPORT
std::ostream&
operator<<( std::ostream& os, klv_0601_view_domain const& value );

// ----------------------------------------------------------------------------
DECLARE_CMP( klv_0601_view_domain )

// ---------------------------------------------------------------------------
/// Interprets data as view domain.
class KWIVER_ALGO_KLV_EXPORT klv_0601_view_domain_format
  : public klv_data_format_< klv_0601_view_domain >
{
public:
  klv_0601_view_domain_format();

  std::string
  description_() const override;

private:
  klv_0601_view_domain
  read_typed( klv_read_iter_t& data, size_t length ) const override;

  void
  write_typed(
    klv_0601_view_domain const& value,
    klv_write_iter_t& data, size_t length ) const override;

  size_t
  length_of_typed( klv_0601_view_domain const& value ) const override;

  static klv_0601_view_domain_interval_format const azimuth_format;
  static klv_0601_view_domain_interval_format const elevation_format;
  static klv_0601_view_domain_interval_format const roll_format;
};

// ----------------------------------------------------------------------------
/// A set of bit values containing varied information about a waypoint.
enum klv_0601_waypoint_info_bit : uint8_t
{
  // 0 = automated, 1 = manual
  KLV_0601_WAYPOINT_INFO_BIT_MODE,
  // 0 = pre-planned, 1 = ad-hoc
  KLV_0601_WAYPOINT_INFO_BIT_SOURCE,
  KLV_0601_WAYPOINT_INFO_BIT_ENUM_END,
};

// ----------------------------------------------------------------------------
KWIVER_ALGO_KLV_EXPORT
std::ostream&
operator<<( std::ostream& os, klv_0601_waypoint_info_bit value );

// ----------------------------------------------------------------------------
/// Interprets data as a waypoint information bitfield.
using klv_0601_waypoint_info_format =
  klv_enum_bitfield_format< klv_0601_waypoint_info_bit, klv_ber_oid_format >;

// ----------------------------------------------------------------------------
/// Aircraft destinations used to navigate the aircraft to certain locations.
struct klv_0601_waypoint_record
{
  uint16_t id;
  int16_t order;
  std::optional< std::set< klv_0601_waypoint_info_bit > > info;
  std::optional< klv_0601_location > location;
};

// ----------------------------------------------------------------------------
KWIVER_ALGO_KLV_EXPORT
std::ostream&
operator<<( std::ostream& os, klv_0601_waypoint_record const& value );

// ----------------------------------------------------------------------------
DECLARE_CMP( klv_0601_waypoint_record )

// ----------------------------------------------------------------------------
/// Interprets data as a waypoint record.
class KWIVER_ALGO_KLV_EXPORT klv_0601_waypoint_record_format
  : public klv_data_format_< klv_0601_waypoint_record >
{
public:
  klv_0601_waypoint_record_format();

  std::string
  description_() const override;

private:
  klv_0601_waypoint_record
  read_typed( klv_read_iter_t& data, size_t length ) const override;

  void
  write_typed(
    klv_0601_waypoint_record const& value,
    klv_write_iter_t& data, size_t length ) const override;

  size_t
  length_of_typed( klv_0601_waypoint_record const& value ) const override;
};

// ----------------------------------------------------------------------------
/// Interprets data as a list of waypoint records.
using klv_0601_waypoint_list_format =
  klv_series_format< klv_0601_waypoint_record_format >;

// ----------------------------------------------------------------------------
/// General status of weapons stores.
enum KWIVER_ALGO_KLV_EXPORT klv_0601_weapon_general_status
{
  KLV_0601_WEAPON_GENERAL_STATUS_OFF,
  KLV_0601_WEAPON_GENERAL_STATUS_INITIALIZATION,
  KLV_0601_WEAPON_GENERAL_STATUS_DEGRADED,
  KLV_0601_WEAPON_GENERAL_STATUS_ALL_UP_ROUND,
  KLV_0601_WEAPON_GENERAL_STATUS_LAUNCH,
  KLV_0601_WEAPON_GENERAL_STATUS_FREE_FLIGHT,
  KLV_0601_WEAPON_GENERAL_STATUS_ABORT,
  KLV_0601_WEAPON_GENERAL_STATUS_MISS_FIRE,
  KLV_0601_WEAPON_GENERAL_STATUS_HANG_FIRE,
  KLV_0601_WEAPON_GENERAL_STATUS_JETTISONED,
  KLV_0601_WEAPON_GENERAL_STATUS_STEPPED_OVER,
  KLV_0601_WEAPON_GENERAL_STATUS_NO_STATUS,
  KLV_0601_WEAPON_GENERAL_STATUS_ENUM_END,
};

// ----------------------------------------------------------------------------
/// Interprets data as a weapons stores general status.
using klv_0601_weapons_general_status_format =
  klv_enum_format< klv_0601_weapon_general_status >;

// ----------------------------------------------------------------------------
KWIVER_ALGO_KLV_EXPORT
std::ostream&
operator<<( std::ostream& os, klv_0601_weapon_general_status value );

// ----------------------------------------------------------------------------
/// A set of bit values to report the status of a weapon before it’s launched.
enum KWIVER_ALGO_KLV_EXPORT klv_0601_weapon_engagement_status_bit
{
  // 0 = fuse functions not set, 1 = fuse functions set
  KLV_0601_WEAPON_ENGAGEMENT_STATUS_BIT_FUSE_ENABLED,
  // 0 = laser functions not set, 1 = laser functions set
  KLV_0601_WEAPON_ENGAGEMENT_STATUS_BIT_LASER_ENABLED,
  // 0 = target functions not set, 1 = target functions set
  KLV_0601_WEAPON_ENGAGEMENT_STATUS_BIT_TARGET_ENABLED,
  // 0 = master arm not set, 1 = master arm set
  KLV_0601_WEAPON_ENGAGEMENT_STATUS_BIT_WEAPON_ARMED,
  KLV_0601_WEAPON_ENGAGEMENT_STATUS_BIT_ENUM_END,
};

// ----------------------------------------------------------------------------
KWIVER_ALGO_KLV_EXPORT
std::ostream&
operator<<( std::ostream& os, klv_0601_weapon_engagement_status_bit value );

// ----------------------------------------------------------------------------
/// List of weapon stores and status.
struct klv_0601_weapons_store
{
  uint16_t station_id;
  uint16_t hardpoint_id;
  uint16_t carriage_id;
  uint16_t store_id;
  klv_0601_weapon_general_status general_status;
  std::set< klv_0601_weapon_engagement_status_bit > engagement_status;
  std::string weapon_type;
};

// ----------------------------------------------------------------------------
KWIVER_ALGO_KLV_EXPORT
std::ostream&
operator<<( std::ostream& os, klv_0601_weapons_store const& value );

// ----------------------------------------------------------------------------
DECLARE_CMP( klv_0601_weapons_store )

// ---------------------------------------------------------------------------
/// Interprets data as a weapons store.
class KWIVER_ALGO_KLV_EXPORT klv_0601_weapons_store_format
  : public klv_data_format_< klv_0601_weapons_store >
{
public:
  klv_0601_weapons_store_format();

  std::string
  description_() const override;

private:
  klv_0601_weapons_store
  read_typed( klv_read_iter_t& data, size_t length ) const override;

  void
  write_typed(
    klv_0601_weapons_store const& value,
    klv_write_iter_t& data, size_t length ) const override;

  size_t
  length_of_typed( klv_0601_weapons_store const& vaue ) const override;
};

// ---------------------------------------------------------------------------
/// Interprets data as a list of weapons stores.
using klv_0601_weapons_store_list_format =
  klv_series_format< klv_0601_weapons_store_format >;

// ---------------------------------------------------------------------------
/// Types of optical and non-optical sensor payloads.
enum KWIVER_ALGO_KLV_EXPORT klv_0601_payload_type
{
  KLV_0601_PAYLOAD_TYPE_ELECTRO_OPTICAL,
  KLV_0601_PAYLOAD_TYPE_LIDAR,
  KLV_0601_PAYLOAD_TYPE_RADAR,
  KLV_0601_PAYLOAD_TYPE_SIGINT,
  KLV_0601_PAYLOAD_TYPE_ENUM_END,
};

// ---------------------------------------------------------------------------
/// Interprets data as a payload type.
using klv_0601_payload_type_format =
  klv_enum_format< klv_0601_payload_type >;

// ---------------------------------------------------------------------------
KWIVER_ALGO_KLV_EXPORT
std::ostream&
operator<<( std::ostream& os, klv_0601_payload_type const& value );

// ---------------------------------------------------------------------------
/// Type, name, and id of a payload.
struct klv_0601_payload_record
{
  uint16_t id;
  klv_0601_payload_type type;
  std::string name;
};

// ---------------------------------------------------------------------------
KWIVER_ALGO_KLV_EXPORT
std::ostream&
operator<<( std::ostream& os, klv_0601_payload_record const& value );

// ---------------------------------------------------------------------------
DECLARE_CMP( klv_0601_payload_record )

// ---------------------------------------------------------------------------
/// Interprets data as a payload record.
class KWIVER_ALGO_KLV_EXPORT klv_0601_payload_record_format
  : public klv_data_format_< klv_0601_payload_record >
{
public:
  klv_0601_payload_record_format();

  std::string
  description_() const override;

private:
  klv_0601_payload_record
  read_typed( klv_read_iter_t& data, size_t length ) const override;

  void
  write_typed(
    klv_0601_payload_record const& value,
    klv_write_iter_t& data, size_t length ) const override;

  size_t
  length_of_typed( klv_0601_payload_record const& value ) const;
};

// ---------------------------------------------------------------------------
/// A possibly-partial list of payloads.
///
/// This has a separate \c count member indicating how many payloads exist in
/// total. Not all payloads may be described in every instance of a payload
/// list, so \c count may be greater than \c payloads.size().
struct klv_0601_payload_list
{
  uint16_t count;
  std::vector< klv_0601_payload_record > payloads;
};

// ----------------------------------------------------------------------------
KWIVER_ALGO_KLV_EXPORT
std::ostream&
operator<<( std::ostream& os, klv_0601_payload_list const& value );

// ----------------------------------------------------------------------------
DECLARE_CMP( klv_0601_payload_list )

// ---------------------------------------------------------------------------
/// Interprets data as a list of payload records.
class KWIVER_ALGO_KLV_EXPORT klv_0601_payload_list_format
  : public klv_data_format_< klv_0601_payload_list >
{
public:
  klv_0601_payload_list_format();

  std::string
  description_() const override;

private:
  klv_0601_payload_list
  read_typed( klv_read_iter_t& data, size_t length ) const override;

  void
  write_typed(
    klv_0601_payload_list const& value,
    klv_write_iter_t& data, size_t length ) const override;

  size_t
  length_of_typed(
    klv_0601_payload_list const& value ) const override;
};

// ----------------------------------------------------------------------------
/// List of currently active payloads from the payload list (Item 138).
using klv_0601_active_payloads_format = klv_enum_bitfield_format< uint16_t >;

// ----------------------------------------------------------------------------
/// A sensor wavelength record.
struct klv_0601_wavelength_record
{
  uint16_t id;
  klv_imap min;
  klv_imap max;
  std::string name;
};

// ----------------------------------------------------------------------------
KWIVER_ALGO_KLV_EXPORT
std::ostream&
operator<<( std::ostream& os, klv_0601_wavelength_record const& value );

// ----------------------------------------------------------------------------
DECLARE_CMP( klv_0601_wavelength_record )

// ----------------------------------------------------------------------------
/// Interprets data as a wavelength.
class KWIVER_ALGO_KLV_EXPORT klv_0601_wavelength_record_format
  : public klv_data_format_< klv_0601_wavelength_record >
{
public:
  klv_0601_wavelength_record_format();

  std::string
  description_() const override;

private:
  klv_0601_wavelength_record
  read_typed( klv_read_iter_t& data, size_t length ) const override;

  void
  write_typed(
    klv_0601_wavelength_record const& value,
    klv_write_iter_t& data, size_t length ) const override;

  size_t
  length_of_typed( klv_0601_wavelength_record const& value ) const override;
};

// ----------------------------------------------------------------------------
using klv_0601_wavelengths_list_format =
  klv_series_format< klv_0601_wavelength_record_format >;

// ---------------------------------------------------------------------------
/// Interprets data as a list of active ST0601 wavelengths.
using klv_0601_active_wavelength_list_format =
  klv_list_format< klv_ber_oid_format >;

// ----------------------------------------------------------------------------
/// A metadata substream id.
struct klv_0601_msid
{
  uint32_t local_id;
  klv_uuid universal_id;
};

// ----------------------------------------------------------------------------
KWIVER_ALGO_KLV_EXPORT
std::ostream&
operator<<( std::ostream& os, klv_0601_msid const& value );

// ----------------------------------------------------------------------------
DECLARE_CMP( klv_0601_msid )

// ----------------------------------------------------------------------------
/// Interprets data as a metadata substream id.
class KWIVER_ALGO_KLV_EXPORT klv_0601_msid_format
  : public klv_data_format_< klv_0601_msid >
{
public:
  klv_0601_msid_format();

  std::string
  description_() const override;

private:
  klv_0601_msid
  read_typed( klv_read_iter_t& data, size_t length ) const override;

  void
  write_typed(
    klv_0601_msid const& value,
    klv_write_iter_t& data, size_t length ) const override;

  size_t
  length_of_typed( klv_0601_msid const& value ) const override;
};

// ----------------------------------------------------------------------------
/// Interprets data as a ST0601 local set.
class KWIVER_ALGO_KLV_EXPORT klv_0601_local_set_format
  : public klv_local_set_format
{
public:
  klv_0601_local_set_format();

  std::string
  description_() const override;

  klv_checksum_packet_format const*
  packet_checksum_format() const override;

private:
  klv_running_sum_16_packet_format m_checksum_format;
};

// ----------------------------------------------------------------------------
/// Returns the UDS key for a ST0601 local set.
KWIVER_ALGO_KLV_EXPORT
klv_uds_key
klv_0601_key();

// ----------------------------------------------------------------------------
/// Returns a lookup object for the traits of the ST0601 local set tags.
KWIVER_ALGO_KLV_EXPORT
klv_tag_traits_lookup const&
klv_0601_traits_lookup();

} // namespace klv

} // namespace arrows

} // namespace kwiver

#endif
