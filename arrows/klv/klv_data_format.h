// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// Interface to the KLV data formats.

#ifndef KWIVER_ARROWS_KLV_KLV_DATA_FORMAT_H_
#define KWIVER_ARROWS_KLV_KLV_DATA_FORMAT_H_

#include "klv_blob.h"
#include "klv_key.h"
#include "klv_lengthy.h"
#include "klv_read_write.txx"
#include "klv_uuid.h"
#include "klv_value.h"

#include <arrows/klv/klv_length_constraints.h>
#include <arrows/klv/kwiver_algo_klv_export.h>
#include <vital/exceptions/metadata.h>
#include <vital/logger/logger.h>

#include <memory>
#include <ostream>
#include <sstream>
#include <typeinfo>
#include <vector>

namespace kwiver {

namespace arrows {

namespace klv {

// ----------------------------------------------------------------------------
class klv_checksum_packet_format;

// ----------------------------------------------------------------------------
/// Untyped base for all KLV data formats.
///
/// This class provides an interface for accessing reading, writing, and
/// printing capabilities for specific formats, as well as implementations of
/// basic methods common to all formats.
class KWIVER_ALGO_KLV_EXPORT klv_data_format
{
public:
  /// \param length_constraints
  ///   Constraints determining how many bytes the serialized KLV can be.
  explicit klv_data_format(
    klv_length_constraints const& length_constraints = {} );

  virtual ~klv_data_format() = default;

  /// Parse raw bytes into data type; return as \c klv_value.
  ///
  /// \param data
  ///   Iterator to input bytes. Will be set to the end of bytes read on
  ///   success, or back to its original value on failure.
  /// \param length The length of the value as parsed from the KLV length field.
  virtual klv_value read( klv_read_iter_t& data, size_t length ) const = 0;

  /// Write \c klv_value to raw bytes.
  ///
  /// \param value Value to write.
  /// \param data
  ///  Iterator to output bytes. Will be set to the end of bytes written.
  /// \param max_length
  ///  The maximum number of bytes to write. An exception will be thrown if
  ///  \p value cannot be written within that number of bytes.
  virtual void write(
    klv_value const& value, klv_write_iter_t& data,
    size_t max_length ) const = 0;

  /// Return the number of bytes required to write \p value.
  ///
  /// \param value Value to check the length of.
  ///
  /// \note The return value does not account for a checksum, if present.
  virtual size_t length_of( klv_value const& value ) const = 0;

  /// Return \c type_info for read / written type.
  virtual std::type_info const& type() const = 0;

  /// Return name of read / written type.
  std::string type_name() const;

  /// Print a string representation of \p value to \p os.
  virtual std::ostream& print(
    std::ostream& os, klv_value const& value ) const = 0;

  /// Return a string representation of \p value.
  std::string to_string( klv_value const& value ) const;

  /// Return a textual description of this data format.
  std::string description() const;

  /// Return the checksum format for the packet key and length only, or
  /// \c nullptr if this format does not have such a checksum.
  virtual klv_checksum_packet_format const* prefix_checksum_format() const;

  /// Return the checksum format for the packet payload only, or \c nullptr if
  /// this format does not have such a checksum.
  virtual klv_checksum_packet_format const* payload_checksum_format() const;

  /// Return the checksum format for the entire packet, or \c nullptr if this
  /// format does not have such a checksum.
  virtual klv_checksum_packet_format const* packet_checksum_format() const;

  /// Return the constraints on the length of this format.
  klv_length_constraints const& length_constraints() const;

  /// Set constraints on the length of this format.
  void set_length_constraints(
    klv_length_constraints const& length_constraints );

protected:
  /// Return a textual description of this data format, not mentioning length
  /// constraints.
  virtual std::string description_() const = 0;

  klv_length_constraints m_length_constraints;
};

using klv_data_format_sptr = std::shared_ptr< klv_data_format >;

// ----------------------------------------------------------------------------
/// Typed base for KLV data formats.
///
/// This class implements the functionality common to data formats of a
/// particular type. It takes care of checking for common edge cases like being
/// given empty data or invalid lengths, so specific derived data formats
/// don't need to duplicate that boilerplate in each class. Specific formats
/// only have to worry about overriding the \c *_typed(),
/// \c *_checksum_format(), and \c description_() functions .
template < class T >
class KWIVER_ALGO_KLV_EXPORT klv_data_format_ : public klv_data_format
{
public:
  using data_type = T;

  explicit
  klv_data_format_( klv_length_constraints const& length_constraints = {} )
    : klv_data_format{ length_constraints }
  {}

  virtual
  ~klv_data_format_() {}

  klv_value
  read( klv_read_iter_t& data, size_t length ) const override final
  {
    auto const begin = data;
    if( !length )
    {
      // Zero length: null / unknown value
      return klv_value{};
    }

    try
    {
      // Call type-specific reading logic
      return read_( data, length );
    }
    catch( std::exception const& e )
    {
      // Return blob if parsing failed
      LOG_ERROR(
        kwiver::vital::get_logger( "klv" ),
        "error occurred during parsing: " << e.what() );
      return klv_read_blob( ( data = begin ), length );
    }
  }

  /// A version of \c read() which returns the specific relevant type instead of
  /// wrapping it in a \c klv_value.
  T
  read_( klv_read_iter_t& data, size_t length ) const
  {
    if( !length )
    {
      // Zero length makes no sense outside of a klv_value
      throw vital::metadata_exception{ "zero length given to read_()" };
    }

    if( !m_length_constraints.do_allow( length ) )
    {
      // Invalid length
      std::stringstream ss;
      ss << "format `" << description() << "` "
         << "received illegal number of bytes (" << length << ") "
         << "when reading";

      LOG_WARN( vital::get_logger( "klv" ), ss.str() );
    }

    // Call format-specific reading logic
    return read_typed( data, length );
  }

  void
  write(
    klv_value const& value, klv_write_iter_t& data,
    size_t max_length ) const override final
  {
    if( value.empty() )
    {
      // Null / unknown value: write nothing
      return;
    }
    else if( !value.valid() )
    {
      // Unparsed value: write raw bytes
      klv_write_blob( value.get< klv_blob >(), data, max_length );
    }
    else
    {
      // Call type-specific writing logic
      write_( value.get< T >(), data, max_length );
    }
  }

  /// A version of \c write() which accepts the specific relevant type instead
  /// of a \c klv_value.
  void
  write_( T const& value, klv_write_iter_t& data, size_t max_length ) const
  {
    // Ensure we have enough bytes
    auto const value_length = length_of_( value );
    if( value_length > max_length )
    {
      std::stringstream ss;
      ss << "format `" << description() << "` "
         << "has been asked to write value `" << to_string( value ) << "`, "
         << "which is too long (" << value_length << ") "
         << "for remaining buffer length (" << max_length << ")";
      throw vital::metadata_buffer_overflow{ ss.str() };
    }

    if( !m_length_constraints.do_allow( value_length ) )
    {
      // Invalid length
      std::stringstream ss;
      ss << "format `" << description() << "` "
         << "has been asked to write value `" << to_string( value ) << "`, "
         << "which serializes to an illegal number of bytes "
         << "(" << value_length << ")";

      LOG_WARN( vital::get_logger( "klv" ), ss.str() );
    }

    // Write the value with format-specific logic
    auto const begin = data;
    write_typed( value, data, value_length );

    // Ensure the number of bytes we wrote was how many planned to write
    auto const written_length =
      static_cast< size_t >( std::distance( begin, data ) );
    if( written_length != value_length )
    {
      std::stringstream ss;
      ss << "format `" << description() << "`: "
         << "written length (" << written_length << ") and "
         << "calculated length (" << value_length <<  ") not equal";
      throw std::logic_error( ss.str() );
    }
  }

  size_t
  length_of( klv_value const& value ) const override final
  {
    if( value.empty() )
    {
      // Empty value means zero bytes (ZLE)
      return 0;
    }
    else if( !value.valid() )
    {
      // Invalid value - just count the bytes
      return value.get< klv_blob >()->size();
    }
    else
    {
      // Call type-specific logic
      return length_of_( value.get< T >() );
    }
  }

  /// A version of \c length_of() which accepts the specific relevant type
  /// instead of a \c klv_value.
  size_t
  length_of_( T const& value ) const
  {
    return length_of_typed( value );
  }

  std::type_info const&
  type() const override final
  {
    return typeid( T );
  }

  std::ostream&
  print( std::ostream& os, klv_value const& value ) const override final
  {
    return !value.valid()
           ? ( os << value )
           : print_( os, value.get< T >() );
  }

  /// A version of \c print() which accepts the specific relevant type instead
  /// of a \c klv_value.
  std::ostream&
  print_( std::ostream& os, T const& value ) const
  {
    return print_typed( os, value );
  }

protected:
  // These functions are overridden by the specific data format classes.
  // print_typed() has default behavior provided to make overriding optional.

  virtual T
  read_typed( klv_read_iter_t& data, size_t length ) const = 0;

  virtual void
  write_typed(
    T const& value, klv_write_iter_t& data, size_t length ) const = 0;

  virtual size_t
  length_of_typed( T const& value ) const = 0;

  virtual std::ostream&
  print_typed( std::ostream& os, T const& value ) const
  {
    if( std::is_same< T, std::string >::value )
    {
      return os << '"' << value << '"';
    }
    else
    {
      return os << value;
    }
  }
};

// ----------------------------------------------------------------------------
/// Treats data as a binary blob, or uninterpreted sequence of bytes.
class KWIVER_ALGO_KLV_EXPORT klv_blob_format
  : public klv_data_format_< klv_blob >
{
public:
  klv_blob_format( klv_length_constraints const& length_constraints = {} );

  virtual
  ~klv_blob_format() = default;

  std::string
  description_() const override;

protected:
  klv_blob
  read_typed( klv_read_iter_t& data, size_t length ) const override;

  void
  write_typed(
    klv_blob const& value,
    klv_write_iter_t& data, size_t length ) const override;

  size_t
  length_of_typed( klv_blob const& value ) const override;
};

// ----------------------------------------------------------------------------
/// Treats data as a 16-byte UUID
class KWIVER_ALGO_KLV_EXPORT klv_uuid_format
  : public klv_data_format_< klv_uuid >
{
public:
  klv_uuid_format();

  std::string
  description_() const override;

protected:
  klv_uuid
  read_typed( klv_read_iter_t& data, size_t length ) const override;

  void
  write_typed(
    klv_uuid const& value,
    klv_write_iter_t& data, size_t length ) const override;

  size_t
  length_of_typed( klv_uuid const& value ) const override;
};

// ----------------------------------------------------------------------------
/// Treats data as a single boolean value.
class KWIVER_ALGO_KLV_EXPORT klv_bool_format
  : public klv_data_format_< bool >
{
public:
  klv_bool_format();

  std::string
  description_() const override;

protected:
  bool
  read_typed( klv_read_iter_t& data, size_t length ) const override;

  void
  write_typed(
    bool const& value,
    klv_write_iter_t& data, size_t length ) const override;

  size_t
  length_of_typed( bool const& value ) const override;
};

// ----------------------------------------------------------------------------
/// Interprets data as an unsigned integer.
class KWIVER_ALGO_KLV_EXPORT klv_uint_format
  : public klv_data_format_< uint64_t >
{
public:
  klv_uint_format( klv_length_constraints const& length_constraints = {} );

  virtual
  ~klv_uint_format() = default;

  std::string
  description_() const override;

protected:
  uint64_t
  read_typed( klv_read_iter_t& data, size_t length ) const override;

  void
  write_typed(
    uint64_t const& value,
    klv_write_iter_t& data, size_t length ) const override;

  size_t
  length_of_typed( uint64_t const& value ) const override;
};

// ----------------------------------------------------------------------------
/// Interprets data as a signed integer.
class KWIVER_ALGO_KLV_EXPORT klv_sint_format
  : public klv_data_format_< int64_t >
{
public:
  klv_sint_format( klv_length_constraints const& length_constraints = {} );

  virtual
  ~klv_sint_format() = default;

  std::string
  description_() const override;

protected:
  int64_t
  read_typed( klv_read_iter_t& data, size_t length ) const override;

  void
  write_typed(
    int64_t const& value,
    klv_write_iter_t& data, size_t length ) const override;

  size_t
  length_of_typed( int64_t const& value ) const override;
};

// ----------------------------------------------------------------------------
/// Interprets data as an enum type.
template < class T >
class KWIVER_ALGO_KLV_EXPORT klv_enum_format
  : public klv_data_format_< std::decay_t< T > >
{
public:
  using data_type = std::decay_t< T >;

  klv_enum_format( klv_length_constraints const& length_constraints = { 1 } )
    : klv_data_format_< data_type >{ length_constraints }
  {}

  virtual
  ~klv_enum_format()
  {}

  std::string
  description_() const override
  {
    return "Enumeration '" + this->type_name() + "'";
  }

protected:
  data_type
  read_typed( klv_read_iter_t& data, size_t length ) const override
  {
    return static_cast< data_type >(
      klv_read_int< uint64_t >( data, length ) );
  }

  void
  write_typed(
    data_type const& value,
    klv_write_iter_t& data, size_t length ) const override
  {
    klv_write_int( static_cast< uint64_t >( value ), data, length );
  }

  size_t
  length_of_typed( data_type const& value ) const override
  {
    auto const int_length = klv_int_length( static_cast< uint64_t >( value ) );
    return std::max( this->m_length_constraints.fixed_or( 1 ), int_length );
  }

  size_t m_length;
};

// ----------------------------------------------------------------------------
/// Interprets data as an unsigned integer encoded in BER format.
class KWIVER_ALGO_KLV_EXPORT klv_ber_format
  : public klv_data_format_< uint64_t >
{
public:
  klv_ber_format();

  virtual
  ~klv_ber_format() = default;

  std::string
  description_() const override;

protected:
  uint64_t
  read_typed( klv_read_iter_t& data, size_t length ) const override;

  void
  write_typed(
    uint64_t const& value,
    klv_write_iter_t& data, size_t length ) const override;

  size_t
  length_of_typed( uint64_t const& value ) const override;
};

// ----------------------------------------------------------------------------
/// Interprets data as an unsigned integer encoded in BER-OID format.
class KWIVER_ALGO_KLV_EXPORT klv_ber_oid_format
  : public klv_data_format_< uint64_t >
{
public:
  klv_ber_oid_format();

  virtual
  ~klv_ber_oid_format() = default;

  std::string
  description_() const override;

protected:
  uint64_t
  read_typed( klv_read_iter_t& data, size_t length ) const override;

  void
  write_typed(
    uint64_t const& value,
    klv_write_iter_t& data, size_t length ) const override;

  size_t
  length_of_typed( uint64_t const& value ) const override;
};

// ----------------------------------------------------------------------------
/// Interprets data as IEEE-754 floating point value.
class KWIVER_ALGO_KLV_EXPORT klv_float_format
  : public klv_data_format_< klv_lengthy< double > >
{
public:
  klv_float_format( klv_length_constraints const& length_constraints = {} );

  virtual
  ~klv_float_format() = default;

  std::string
  description_() const override;

protected:
  klv_lengthy< double >
  read_typed( klv_read_iter_t& data, size_t length ) const override;

  void
  write_typed(
    klv_lengthy< double > const& value,
    klv_write_iter_t& data, size_t length ) const override;

  size_t
  length_of_typed( klv_lengthy< double > const& value ) const override;

  std::ostream&
  print_typed(
    std::ostream& os,
    klv_lengthy< double > const& value ) const override;
};

// ----------------------------------------------------------------------------
/// Interprets data as an unsigned integer mapped to a known floating-point
/// range.
class KWIVER_ALGO_KLV_EXPORT klv_sflint_format
  : public klv_data_format_< klv_lengthy< double > >
{
public:
  klv_sflint_format(
    vital::interval< double > const& interval,
    klv_length_constraints const& length_constraints = {} );

  virtual
  ~klv_sflint_format() = default;

  std::string
  description_() const override;

  vital::interval< double >
  interval() const;

protected:
  klv_lengthy< double >
  read_typed( klv_read_iter_t& data, size_t length ) const override;

  void
  write_typed(
    klv_lengthy< double > const& value,
    klv_write_iter_t& data, size_t length ) const override;

  size_t
  length_of_typed( klv_lengthy< double > const& value ) const override;

  std::ostream&
  print_typed(
    std::ostream& os,
    klv_lengthy< double > const& value ) const override;

  vital::interval< double > m_interval;
};

// ----------------------------------------------------------------------------
/// Interprets data as an signed integer mapped to a known floating-point
/// range.
class KWIVER_ALGO_KLV_EXPORT klv_uflint_format
  : public klv_data_format_< klv_lengthy< double > >
{
public:
  klv_uflint_format(
    vital::interval< double > const& interval,
    klv_length_constraints const& length_constraints = {} );

  virtual
  ~klv_uflint_format() = default;

  std::string
  description_() const override;

  vital::interval< double >
  interval() const;

protected:
  klv_lengthy< double >
  read_typed( klv_read_iter_t& data, size_t length ) const override;

  void
  write_typed(
    klv_lengthy< double > const& value,
    klv_write_iter_t& data, size_t length ) const override;

  size_t
  length_of_typed( klv_lengthy< double > const& value ) const override;

  std::ostream&
  print_typed(
    std::ostream& os,
    klv_lengthy< double > const& value ) const override;

  vital::interval< double > m_interval;
};

// ----------------------------------------------------------------------------
/// Wraps another format with a \c klv_lengthy<T> data type, stripping out the
/// length information and exposing just the underlying \c T datatype.
template < class Format >
class KWIVER_ALGO_KLV_EXPORT klv_lengthless_format
  : public klv_data_format_< typename Format::data_type::value_type >
{
public:
  using data_type = typename Format::data_type::value_type;

  template < class... Args >
  klv_lengthless_format( Args&&... args )
    : m_format{ args... }
  {
    if( !m_format.length_constraints().fixed() )
    {
      throw std::logic_error( "klv_lengthless_format requires fixed length" );
    }
    this->m_length_constraints = m_format.length_constraints();
    m_format.set_length_constraints( {} );
  }

  std::string
  description_() const
  {
    return m_format.description();
  }

protected:
  data_type
  read_typed( klv_read_iter_t& data, size_t length ) const
  {
    return m_format.read_( data, length ).value;
  }

  void
  write_typed(
    data_type const& value,
    klv_write_iter_t& data, size_t length ) const
  {
    m_format.write_(
      { value, *this->m_length_constraints.fixed() }, data, length );
  }

  size_t
  length_of_typed( [[maybe_unused]] data_type const& value ) const
  {
    return *this->m_length_constraints.fixed();
  }

  std::ostream&
  print_typed( std::ostream& os, data_type const& value ) const
  {
    return m_format.print_(
      os, { value, *this->m_length_constraints.fixed() } );
  }

  Format m_format;
};
using klv_lengthless_float_format = klv_lengthless_format< klv_float_format >;

// ----------------------------------------------------------------------------
template < class Enum, class Int >
std::set< Enum >
bitfield_to_enums( Int bitfield )
{
  static_assert( std::is_unsigned< Int >::value, "bitfield must be unsigned" );

  std::set< Enum > result;
  for( size_t i = 0; bitfield; ++i, bitfield >>= 1 )
  {
    if( bitfield & 1 )
    {
      result.emplace( static_cast< Enum >( i ) );
    }
  }
  return result;
}

// ----------------------------------------------------------------------------
template < class Enum, class Int = uint64_t >
Int
enums_to_bitfield( std::set< Enum > const& enums )
{
  static_assert( std::is_unsigned< Int >::value, "bitfield must be unsigned" );

  Int result = 0;
  for( auto const& element : enums )
  {
    result |= static_cast< Int >( 1 ) << static_cast< Int >( element );
  }
  return result;
}

// ----------------------------------------------------------------------------
/// Interprets data as an enumerated bitfield, where a number of boolean values
/// are encodeded as bits of an integer.
///
/// The data type here is a \c std::set of all values that are set to \c 1.
template < class Enum, class Format = klv_uint_format >
class KWIVER_ALGO_KLV_EXPORT klv_enum_bitfield_format
  : public klv_data_format_< std::set< Enum > >
{
public:
  template < class... Args >
  klv_enum_bitfield_format( Args&&... args )
    : m_format{ args... }
  {
    this->m_length_constraints = m_format.length_constraints();
    m_format.set_length_constraints( {} );
  }

  std::string
  description_() const
  {
    return "Bitfield";
  }

protected:
  std::set< Enum >
  read_typed( klv_read_iter_t& data, size_t length ) const
  {
    return bitfield_to_enums< Enum >( m_format.read_( data, length ) );
  }

  void
  write_typed(
    std::set< Enum > const& value,
    klv_write_iter_t& data, size_t length ) const
  {
    m_format.write_( enums_to_bitfield( value ), data, length );
  }

  size_t
  length_of_typed( std::set< Enum > const& value ) const
  {
    auto const int_length = m_format.length_of_( enums_to_bitfield( value ) );
    return std::max( this->m_length_constraints.fixed_or( 1 ), int_length );
  }

  Format m_format;
};

} // namespace klv

} // namespace arrows

} // namespace kwiver

#endif
