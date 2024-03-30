// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.
#ifndef PLUGGABLE_MACRO_TESTING_H

#include <vital/plugin_management/pluggable_macro_magic.h>

#include <cmath>
#include <limits>
#include <string>

namespace kwiver::vital::detail {

template < typename ValueType1, typename ValueType2 >
bool
is_equal( const ValueType1 a, const ValueType2 b )
{
// for floating point numbers we use the same criterion as numpy:
// https://numpy.org/doc/stable/reference/generated/numpy.isclose.html
//    absolute(a - b) <= (atol + rtol * absolute(b))

  if constexpr( std::is_same_v< ValueType1, float > )
  {
    const float atol = 1e-08;
    const float rtol = 1e-05;
    static_assert(
      std::is_same_v< ValueType2, float >,
      "value type of a float parameter should be float" );
    return std::fabs( a - b ) <= ( atol + rtol * std::fabs( b ) );
  }
  if constexpr( std::is_same_v< ValueType2, double > )
  {
    const double atol = 1e-08;
    const double rtol = 1e-05;
    static_assert(
      std::is_same_v< ValueType2, double >,
      "value type of a double parameter should be double" );
    return std::fabs( a - b ) <= ( atol + rtol * std::fabs( b ) );
  }
  if constexpr( std::is_same_v< ValueType1,
    std::string > && std::is_same_v< ValueType2, char > )
  {
    is_equal<>( a, std::string( b ) );
  }
  else
  {
    return a == b;
  }
}

} // namespace kwiver::vital::detail

#define EXPECT_PARAM_DESCRIPTION( tuple ) EXPECT_PARAM_DESCRIPTION_ tuple
#define EXPECT_PARAM_DESCRIPTION_( param, type, description, default )     \
IF_ELSE( HAS_ARGS( default ) )                                             \
(                                                                          \
  {                                                                        \
    const type& value = cfg->get_value< type >( #param );                  \
    bool success = kwiver::vital::detail::is_equal( value, default );      \
    if( !success )                                                         \
    {                                                                      \
      ADD_FAILURE() << "Param " << #param << " has wrong default value..." \
                                             "Expected " << default <<     \
        ", but got " << value << " instead.";                              \
    }                                                                      \
    if( cfg->get_description( #param ) != description )                    \
    {                                                                      \
      ADD_FAILURE() << "Wrong description for parameter " << #param;       \
    }                                                                      \
  },                                                                       \
)

// ----------------------------------------------------------------------------
// Test utility that allows to test a configuration by copy / pasting
// PLUGGABLE_IMPL from the header
#define EXPECT_PLUGGABLE_IMPL( class_name, description, ... )     \
do                                                                \
{                                                                 \
  class_name instance;                                            \
  if( instance.plugin_description() != description )              \
  {                                                               \
    ADD_FAILURE() << "Description for the class is not correct."; \
  }                                                               \
  auto cfg = instance.get_configuration();                        \
  IF( HAS_ARGS( __VA_ARGS__ ) )(                                  \
    MAP( EXPECT_PARAM_DESCRIPTION, EMPTY, __VA_ARGS__ )           \
  )                                                               \
} while( false );
#endif
