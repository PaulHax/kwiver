// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include "timestamp.h"

#include <cstring>
#include <ctime>
#include <sstream>
#include <string>

namespace kwiver {

namespace vital {

timestamp
::timestamp()
  : m_valid_time( false ),
    m_valid_frame( false ),
    m_time( 0 ),
    m_frame( 0 ),
    m_time_domain_index( 0 )
{}

timestamp
::timestamp( time_usec_t t, frame_id_t f )
  : m_valid_time( true ),
    m_valid_frame( true ),
    m_time( t ),
    m_frame( f ),
    m_time_domain_index( 0 )
{}

// ----------------------------------------------------------------------------
timestamp&
timestamp
::set_time_usec( time_usec_t t )
{
  m_time = t;
  m_valid_time = true;

  return *this;
}

// ----------------------------------------------------------------------------
timestamp&
timestamp
::set_time_seconds( double t )
{
  m_time = static_cast< time_usec_t >( t * 1e6 );     // Convert to usec
  m_valid_time = true;

  return *this;
}

// ----------------------------------------------------------------------------
timestamp&
timestamp
::set_frame( frame_id_t f )
{
  m_frame = f;
  m_valid_frame = true;

  return *this;
}

// ----------------------------------------------------------------------------
timestamp&
timestamp
::set_invalid()
{
  m_valid_time = false;
  m_valid_frame = false;

  return *this;
}

// ----------------------------------------------------------------------------
timestamp&
timestamp
::set_time_domain_index( int dom )
{
  m_time_domain_index = dom;
  return *this;
}

// ----------------------------------------------------------------------------
double
timestamp
::get_time_seconds() const
{
  return static_cast< double >( m_time ) * 1e-6; // convert from usec to sec
}

// ----------------------------------------------------------------------------
/// Generic truth table for compares
///
/// There are some cases where the tow objects are incomparable. in these
/// cases the results are always false.
///
/// \code
///
/// times-valid | frames_valid | same_domain | time_condition | frame_condition
/// | result
/// ----------------------------------------------------------------------------
///   -      |       -      |     F       |       -        |       -         |
///   F (incomparable)
///   F      |       F      |     T       |       -        |       -         |
///   F (incomparable)
///          |              |             |                |                 |
///   T      |       T      |     T       |       T        |       T         |
///   T (meets condition)
///          |              |             |                |                 |
///   T      |       T      |     T       |       F        |       -         |
///   F
///   T      |       T      |     T       |       -        |       F         |
///   F
///          |              |             |                |                 |
///   T      |       F      |     T       |       F        |       -         |
///   F (time only compare)
///   T      |       F      |     T       |       T        |       -         |
///   T (time only compare)
///   F      |       T      |     T       |       -        |       F         |
///   F (frame only compare)
///   F      |       T      |     T       |       -        |       T         |
///   T (frame only compare)
///
/// \endcode
///
/// General implementation
///
/// ( ! same_domain ) -> F
/// ( ! time_valid ) & ( ! frame_valid ) -> F
/// ( time_valid & ( ! time_condition ) -> F
/// ( frame_valid & ( ! frame_condition ) -> F
///                           -> T
///

#define COMPARE( OP )                                                         \
bool                                                                          \
timestamp                                                                     \
::operator OP( timestamp const& rhs ) const                                   \
{                                                                             \
  if( this->m_time_domain_index != rhs.m_time_domain_index )                  \
  {                                                                           \
    return false;                                                             \
  }                                                                           \
                                                                              \
  const bool time_valid( this->has_valid_time() && rhs.has_valid_time() );    \
  const bool frame_valid( this->has_valid_frame() && rhs.has_valid_frame() ); \
                                                                              \
  if( ( !time_valid ) && ( !frame_valid ) )                                   \
  {                                                                           \
    return false;                                                             \
  }                                                                           \
                                                                              \
  if( time_valid  && !( this->get_time_usec() OP rhs.get_time_usec() ) )      \
  {                                                                           \
    return false;                                                             \
  }                                                                           \
                                                                              \
  if( frame_valid && !( this->get_frame() OP rhs.get_frame() ) )              \
  {                                                                           \
    return false;                                                             \
  }                                                                           \
                                                                              \
  return true;                                                                \
}

// ----------------------------------------------------------------------------
// Instantiate relational operators
//
COMPARE( == )
COMPARE( < )
COMPARE( > )
COMPARE( <= )
COMPARE( >= )

bool
timestamp
::operator!=( timestamp const& rhs ) const
{
  return !operator==( rhs );
}

// ----------------------------------------------------------------------------
std::string
timestamp
::pretty_print() const
{
  std::stringstream str;
  std::string c_tim( "" );
  std::time_t tt = static_cast< std::time_t >( this->get_time_seconds() );

  std::streamsize old_prec = str.precision();
  str.precision( 16 );

  str << "ts(f: ";

  if( this->has_valid_frame() )
  {
    str << this->get_frame();
  }
  else
  {
    str << "<inv>";
  }

  str << ", t: ";

  if( this->has_valid_time() )
  {
    char* p = ctime( &tt ); // this may return null if <tt> is out of range,
    if( p )
    {
      constexpr size_t BUFFER_SIZE = 128;
      char buffer[ BUFFER_SIZE + 1 ];
      c_tim = " (";
      buffer[ 0 ] = 0;
      buffer[ BUFFER_SIZE ] = 0;
      strncpy( buffer, p, BUFFER_SIZE );
      buffer[ std::strlen( buffer ) - 1 ] = 0; // remove NL

      c_tim = c_tim + buffer;
      c_tim = c_tim + ")";

      str << this->get_time_usec() << c_tim;
    }
    else
    {
      str << " (time " << tt << " out of bounds?)";
    }
  }
  else
  {
    str << "<inv>";
  }

  str << ", d: " << this->get_time_domain_index() <<  ")";

  str.precision( old_prec );
  return str.str();
}

} // namespace vital

}   // end namespace
