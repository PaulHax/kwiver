// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/**
 * \file
 *
 * \brief Supplemental macro definitions for test cases
 */

#ifndef KWIVER_TEST_TEST_TMPFN_H_
#define KWIVER_TEST_TEST_TMPFN_H_

#include <stdexcept>
#include <string>
#include <vector>

#include <cstdio>
#include <cstdlib>

#ifdef _WIN32
#include <errno.h>
#include <fcntl.h>
#include <io.h>
#include <sys\types.h>
#else
#include <unistd.h>
#endif

namespace kwiver {

namespace testing {

namespace detail {

inline
int
mkstemp( char* templated_string )
{
#ifdef _WIN32
  char* result = _mktemp( templated_string );
  if( result == NULL )
  {
    if( errno == EINVAL )
    {
      fprintf( stderr, "Bad parameter for _mktemp" );
    }
    else if( errno == EEXIST )
    {
      fprintf( stderr, "Out of unique filenames" );
    }
    return -1;
  }

  int fd = _open( result, _O_CREAT );
  return fd;
#else
  return ::mkstemp( templated_string );
#endif
}

} // namespace detail

// ----------------------------------------------------------------------------

/** @brief Generate a unique file name in the current working directory.
 *
 * @param prefix Prefix for generated file name.
 * @param suffix Suffix for generated file name.
 */
inline
std::string
temp_file_name( char const* prefix, char const* suffix )
{
  // to use mkstemp, the template must have 6 X's at the end, so we ignore the
  // suffix
  std::string templateName = "./" + std::string( prefix ) + "XXXXXX";
  std::vector< char > tempFileName( templateName.begin(), templateName.end() );
  tempFileName.push_back( '\0' );

  int fd = detail::mkstemp( tempFileName.data() );
  if( fd == -1 )
  {
    throw std::runtime_error( "Failed to create temporary file" );
  }

  close( fd );

  // add the suffix back, because some files are typed based on suffix.
  std::string result( tempFileName.data() );

  return result + suffix;
}

} // end namespace testing

} // end namespace kwiver

#endif
