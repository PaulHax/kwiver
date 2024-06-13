// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// Supplemental macro definitions for test cases.

#ifndef KWIVER_TEST_TEST_TMPFN_H_
#define KWIVER_TEST_TEST_TMPFN_H_

#include <chrono>
#include <random>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include <cstdio>
#include <cstdlib>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#include <sys\types.h>
#else
#include <unistd.h>
#endif

namespace kwiver {

namespace testing {

namespace detail {

// ----------------------------------------------------------------------------
inline
int
mkstemps( std::string& templated_string, size_t suffix_len )
{
#ifdef _WIN32
  if( templated_string.size() < suffix_len + 6 )
  {
    return -1;
  }

  auto const x_end =
    templated_string.data() + templated_string.size() - suffix_len;
  auto const x_begin = x_end - 6;
  if( std::string( x_begin, x_end ) != "XXXXXX" )
  {
    return -1;
  }

  auto const thread_id =
    std::hash< std::thread::id >()( std::this_thread::get_id() );
  auto const time = static_cast< size_t >(
    std::chrono::high_resolution_clock::now().time_since_epoch().count() );
  auto rng = std::mt19937_64( thread_id ^ time );
  static char const chars[ 37 ] = "0123456789abcdefghijklmnopqrstuvwxyz";
  constexpr size_t char_count = 36;

  int fd = -1;
  while( fd < 0 )
  {
    for( auto ptr = x_begin; ptr != x_end; ++ptr )
    {
      *ptr = chars[ rng() % char_count ];
    }
    fd = _open(
      templated_string.c_str(), _O_CREAT | _O_EXCL, _S_IREAD | _S_IWRITE );
  }

  return fd;
#else
  return ::mkstemps( templated_string.data(), suffix_len );
#endif
}

} // namespace detail

// ----------------------------------------------------------------------------
/// Generate a unique file name in the current working directory.
///
/// \param prefix Prefix for generated file name.
/// \param suffix Suffix for generated file name.
inline
std::string
temp_file_name( std::string const& prefix, std::string const& suffix )
{
  // To use mkstemps, the template must have 6 X's to be filled in with
  // arbitrary characters
  std::string result = "./" + prefix + "XXXXXX" + suffix;
  int fd = detail::mkstemps( result, suffix.size() );
  if( fd < 0 )
  {
    throw std::runtime_error( "Failed to create temporary file" );
  }

  close( fd );

  return result;
}

} // namespace testing

} // namespace kwiver

#endif
