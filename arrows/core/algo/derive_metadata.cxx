// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Compute derived metadata fields.

#include <arrows/core/algo/derive_metadata.h>

#include <vital/types/geo_point.h>
#include <vital/types/metadata.h>
#include <vital/types/metadata_tags.h>
#include <vital/types/metadata_traits.h>
#include <vital/types/rotation.h>

#include <algorithm>

#include <vital/math_constants.h>

#include <memory>

#include <cmath>

namespace kv = kwiver::vital;

namespace kwiver {

namespace arrows {

namespace core {

namespace {

///////////////////////////////////////////////////////////////////////////////

//BEGIN helpers

// ----------------------------------------------------------------------------
kwiver::vital::rotation_d
get_platform_rotation( kwiver::vital::metadata_sptr const& metadata )
{
  kv::metadata_item const& yaw_item =
    metadata->find( kv::VITAL_META_PLATFORM_HEADING_ANGLE );
  kv::metadata_item const& pitch_item =
    metadata->find( kv::VITAL_META_PLATFORM_PITCH_ANGLE );
  kv::metadata_item const& roll_item =
    metadata->find( kv::VITAL_META_PLATFORM_ROLL_ANGLE );

  if( !yaw_item || !pitch_item || !roll_item ||
      !std::isfinite( yaw_item.as_double() ) ||
      !std::isfinite( pitch_item.as_double() ) ||
      !std::isfinite( roll_item.as_double() ) )
  {
    VITAL_THROW(
      kv::invalid_value,
      "metadata does not contain platform orientation" );
  }

  auto const yaw = yaw_item.as_double() * kv::deg_to_rad;
  auto const pitch = pitch_item.as_double() * kv::deg_to_rad;
  auto const roll = roll_item.as_double() * kv::deg_to_rad;

  return { yaw, pitch, roll };
}

// ----------------------------------------------------------------------------
kwiver::vital::rotation_d
get_sensor_rotation( kwiver::vital::metadata_sptr const& metadata )
{
  // All relative to platform
  kv::metadata_item const& yaw_item =
    metadata->find( kv::VITAL_META_SENSOR_REL_AZ_ANGLE );
  kv::metadata_item const& pitch_item =
    metadata->find( kv::VITAL_META_SENSOR_REL_EL_ANGLE );
  kv::metadata_item const& roll_item =
    metadata->find( kv::VITAL_META_SENSOR_REL_ROLL_ANGLE );

  if( !yaw_item || !pitch_item || !roll_item ||
      !std::isfinite( yaw_item.as_double() ) ||
      !std::isfinite( pitch_item.as_double() ) ||
      !std::isfinite( roll_item.as_double() ) )
  {
    VITAL_THROW(
      kv::invalid_value,
      "metadata does not contain sensor orientation" );
  }

  auto const yaw = yaw_item.as_double() * kv::deg_to_rad;
  auto const pitch = pitch_item.as_double() * kv::deg_to_rad;
  auto const roll = roll_item.as_double() * kv::deg_to_rad;

  return { yaw, pitch, roll };
}

// ----------------------------------------------------------------------------
kwiver::vital::rotation_d
get_total_rotation( kwiver::vital::metadata_sptr const& metadata )
{
  // Absolute (not relative to platform)
  kv::rotation_d const platform_rotation = get_platform_rotation( metadata );
  kv::rotation_d const sensor_rotation = get_sensor_rotation( metadata );
  return platform_rotation * sensor_rotation;
}

// ----------------------------------------------------------------------------
// Returns in radians
double
get_sensor_horizontal_fov( kwiver::vital::metadata_sptr const& metadata )
{
  kv::metadata_item const& item =
    metadata->find( kv::VITAL_META_SENSOR_HORIZONTAL_FOV );

  if( !item || !std::isfinite( item.as_double() ) )
  {
    VITAL_THROW(
      kv::invalid_value,
      "metadata does not contain horizontal sensor fov" );
  }

  return item.as_double() * kv::deg_to_rad;
}

// ----------------------------------------------------------------------------
// Returns in radians
double
get_sensor_vertical_fov( kwiver::vital::metadata_sptr const& metadata )
{
  kv::metadata_item const& item =
    metadata->find( kv::VITAL_META_SENSOR_VERTICAL_FOV );

  if( !item || !std::isfinite( item.as_double() ) )
  {
    VITAL_THROW(
      kv::invalid_value,
      "metadata does not contain vertical sensor fov" );
  }

  return item.as_double() * kv::deg_to_rad;
}

// ----------------------------------------------------------------------------
double
get_slant_range( kwiver::vital::metadata_sptr const& metadata )
{
  kv::metadata_item const& item =
    metadata->find( kv::VITAL_META_SLANT_RANGE );

  if( !item || !std::isfinite( item.as_double() ) )
  {
    VITAL_THROW(
      kv::invalid_value,
      "metadata does not contain slant range" );
  }

  return item.as_double();
}

// ----------------------------------------------------------------------------
kv::geo_point
get_sensor_location( kwiver::vital::metadata_sptr const& metadata )
{
  kv::metadata_item const& item =
    metadata->find( kv::VITAL_META_SENSOR_LOCATION );

  if( item )
  {
    auto const typed_item = item.get< kv::geo_point >();
    if( std::isfinite( typed_item.location()[ 0 ] ) &&
        std::isfinite( typed_item.location()[ 1 ] ) &&
        std::isfinite( typed_item.location()[ 2 ] ) )
    {
      return typed_item;
    }
  }

  VITAL_THROW(
    kv::invalid_value, "metadata does not contain sensor location" );
}

// ----------------------------------------------------------------------------
kv::geo_point
get_frame_center( kwiver::vital::metadata_sptr const& metadata )
{
  kv::metadata_item const& item =
    metadata->find( kv::VITAL_META_FRAME_CENTER );

  if( item )
  {
    auto const typed_item = item.get< kv::geo_point >();
    if( std::isfinite( typed_item.location()[ 0 ] ) &&
        std::isfinite( typed_item.location()[ 1 ] ) &&
        std::isfinite( typed_item.location()[ 2 ] ) )
    {
      return typed_item;
    }
  }

  VITAL_THROW(
    kv::invalid_value, "metadata does not contain frame center" );
}

// ----------------------------------------------------------------------------
double
get_target_width( kwiver::vital::metadata_sptr const& metadata )
{
  kv::metadata_item const& item =
    metadata->find( kv::VITAL_META_TARGET_WIDTH );

  if( !item || !std::isfinite( item.as_double() ) )
  {
    VITAL_THROW(
      kv::invalid_value,
      "metadata does not contain target width" );
  }

  return item.as_double();
}

// ----------------------------------------------------------------------------
double
compute_slant_range( kwiver::vital::metadata_sptr const& metadata )
{
  double slant_range = 0.0;
  try
  {
    // Attempt to acquire slant range directly
    slant_range = get_slant_range( metadata );
  }
  catch( kv::invalid_value const& e )
  {
    // Slant range must be calculated from other values
    kv::rotation_d const total_rotation = get_total_rotation( metadata );
    double yaw, pitch, roll;
    total_rotation.get_yaw_pitch_roll( yaw, pitch, roll );
    if( pitch >= 0 )
    {
      VITAL_THROW( kv::invalid_value, "pitch must be negative" );
    }

    // Determine the altitude of the sensor above the frame center
    kv::geo_point const sensor_location = get_sensor_location( metadata );
    double const sensor_altitude = sensor_location.location()[ 2 ];
    kv::geo_point const frame_center = get_frame_center( metadata );
    double const frame_center_altitude = frame_center.location()[ 2 ];
    double const altitude_difference = sensor_altitude - frame_center_altitude;

    slant_range = altitude_difference / std::sin( -pitch );
  }

  return slant_range;
}

// ----------------------------------------------------------------------------
double
compute_horizontal_gsd(
  double slant_range, double sensor_horizontal_fov,
  double frame_width )
{
  return 2.0 * slant_range *
         std::tan( sensor_horizontal_fov / frame_width / 2.0 );
}

// ----------------------------------------------------------------------------
double
compute_vertical_gsd(
  double slant_range, double sensor_vertical_fov,
  double pitch, double frame_height )
{
  if( pitch >= 0 )
  {
    VITAL_THROW( kv::invalid_value, "pitch must be negative" );
  }
  return 2.0 * slant_range *
         std::tan( sensor_vertical_fov / frame_height / 2.0 ) /
         std::sin( -pitch );
}

// ----------------------------------------------------------------------------
double
compute_gsd(
  kwiver::vital::metadata_sptr const& metadata,
  size_t frame_width, size_t frame_height )
{
  if( frame_width < 1 || frame_height < 1 )
  {
    VITAL_THROW( kv::invalid_value, "frame dimensions must both be positive" );
  }

  try
  {
    // Intermediate values
    kv::rotation_d const total_rotation = get_total_rotation( metadata );
    double yaw, pitch, roll;
    total_rotation.get_yaw_pitch_roll( yaw, pitch, roll );

    double const slant_range = get_slant_range( metadata );
    double const sensor_horizontal_fov =
      get_sensor_horizontal_fov( metadata );
    double const sensor_vertical_fov = get_sensor_vertical_fov( metadata );

    // Approximate dimensions of image on ground plane
    double const gsd_horizontal =
      compute_horizontal_gsd( slant_range, sensor_horizontal_fov, frame_width );

    double const gsd_vertical =
      compute_vertical_gsd(
        slant_range, sensor_vertical_fov, pitch,
        frame_height );

    // GSD is the geometric mean of each dimensions's GSD
    // All values in meters per pixel
    return std::sqrt( gsd_horizontal * gsd_vertical );
  }
  catch( kv::invalid_value const& e )
  {
    // Move onto the next case
  }

  // Horizontal axis only
  try
  {
    auto const sensor_horizontal_fov = get_sensor_horizontal_fov( metadata );
    // Note that the reference implementation doesn't use computed slant range
    // for this method
    auto const slant_range = get_slant_range( metadata );

    return compute_horizontal_gsd(
      slant_range, sensor_horizontal_fov,
      frame_width );
  }
  catch( kv::invalid_value const& e )
  {
    // Move on to the next case
  }

  // Target width horizontal axis only
  try
  {
    auto const target_width = get_target_width( metadata );

    return target_width / frame_width;
  }
  catch( kv::invalid_value const& e )
  {
    // Move on to the next case
  }

  VITAL_THROW( kv::invalid_value, "insufficient metadata to calculate GSD" );
}

// ----------------------------------------------------------------------------
// Compute Video NIIRS image quality measure
// Estimation based on General Image Quality Equation v5 (GIQE5)
// See https://gwg.nga.mil/ntb/baseline/docs/GIQE-5_for_Public_Release.pdf
double
compute_vniirs( double gsd, double rer, double snr )
{
  // Taken from Table 2
  constexpr double a0 = 9.57, a1 = -3.32, a2 = 3.32,
    a3 = -1.9, a4 = -2.0,  a5 = -1.8;

  constexpr double meters_to_inches = 1.0 / 0.0254;
  gsd *= meters_to_inches;

  double const log10_gsd = std::log10( gsd );

  // double const log10_rer = std::log10( rer );
  // auto vniirs = a0 +
  //               a1 * log10_gsd +
  //               a2 * ( 1.0 - std::exp( a3 / snr ) ) * log10_rer +
  //               a4 * std::pow( log10_rer, 4 ) +
  //               a5 / snr;
  // Above is full equation; below is partial equation since we don't actually
  // know RER or SNR
  ( void ) rer;
  ( void ) snr;
  ( void ) a2;
  ( void ) a3;
  ( void ) a4;
  ( void ) a5;

  auto vniirs = a0 + a1 * log10_gsd;

  // 2.0 is defined as the lower bound for VNIIRs
  vniirs = std::max( vniirs, 2.0 );
  return vniirs;
}

// ----------------------------------------------------------------------------
double
compute_rer( VITAL_UNUSED kwiver::vital::image_container_scptr const& image )
{
  // TODO: Implement
  return 0.3; // Dummy value within reasonable range
}

// ----------------------------------------------------------------------------
double
compute_snr( VITAL_UNUSED kwiver::vital::image_container_scptr const& image )
{
  // TODO: Implement
  return 15.0; // Dummy value within reasonable range
}

// ----------------------------------------------------------------------------
std::string
compute_wavelength( std::string const& image_source )
{
  auto const contains_any_of =
    [ &image_source ]( std::vector< std::string > const& strings ){
      auto const contains_string = [ & ]( std::string const& s ){
                                     return image_source.find( s ) !=
                                            image_source.npos;
                                   };
      return std::any_of( strings.begin(), strings.end(), contains_string );
    };

  if( contains_any_of( { "VIS", "EO", "TV" } ) )
  {
    return "VIS";
  }
  if( contains_any_of( { "NIR", "NWIR", "SIR", "SWIR" } ) )
  {
    return "NIR";
  }
  if( contains_any_of( { "MIR", "MWIR" } ) )
  {
    return "MIR";
  }
  if( contains_any_of( { "LIR", "LWIR" } ) )
  {
    return "LIR";
  }
  if( contains_any_of( { "FIR", "FWIR" } ) )
  {
    return "FIR";
  }
  if( contains_any_of( { "IR" } ) )
  {
    return "IR";
  }
  return "";
}

} // namespace <anonymous>

//END helpers

///////////////////////////////////////////////////////////////////////////////

//BEGIN algorithm interface
// ----------------------------------------------------------------------------
void
derive_metadata
::initialize()
{
  this->set_capability( CAN_USE_FRAME_IMAGE, false );
}

// ----------------------------------------------------------------------------
bool
derive_metadata
::check_configuration( vital::config_block_sptr ) const
{
  // No configuration, so always return true
  return true;
}

// ----------------------------------------------------------------------------
kwiver::vital::metadata_vector
derive_metadata
::filter(
  kwiver::vital::metadata_vector const& input_metadata,
  kwiver::vital::image_container_scptr const& input_image )
{
  kv::metadata_vector updated_values;

  for( auto const& metadata : input_metadata )
  {
    // Deep copy metadata
    auto updated_metadata = kv::metadata_sptr( metadata->clone() );

    try
    {
      // Compute wavelength
      auto const& image_source =
        metadata->find( kv::VITAL_META_IMAGE_SOURCE_SENSOR );
      if( image_source && !metadata->has( kv::VITAL_META_WAVELENGTH ) )
      {
        auto const wavelength = compute_wavelength( image_source.as_string() );
        if( !wavelength.empty() )
        {
          updated_metadata->add< kv::VITAL_META_WAVELENGTH >( wavelength );
        }
      }

      // Compute slant range. Must be inserted before GSD calculation
      auto const slant_range = compute_slant_range( updated_metadata );
      updated_metadata->add< kv::VITAL_META_SLANT_RANGE >( slant_range );

      if( input_image )
      {
        auto const frame_width = input_image->width();
        auto const frame_height = input_image->height();

        // Compute GSD
        auto const gsd =
          compute_gsd( updated_metadata, frame_width, frame_height );
        if( !std::isfinite( gsd ) || gsd <= 0 )
        {
          VITAL_THROW( kv::invalid_value, "invalid GSD result" );
        }
        updated_metadata->add< kv::VITAL_META_AVERAGE_GSD >( gsd );

        // Compute VNIIRS
        auto const rer = compute_rer( input_image );
        auto const snr = compute_snr( input_image );
        auto const vniirs = compute_vniirs( gsd, rer, snr );
        updated_metadata->add< kv::VITAL_META_VNIIRS >( vniirs );
      }
    }
    catch( kv::invalid_value const& e )
    {
      // Fail silently
    }

    updated_values.push_back( updated_metadata );
  }

  return updated_values;
}

//END algorithm interface

} // namespace core

} // namespace arrows

} // namespace kwiver
