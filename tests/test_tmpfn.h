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
#include <unistd.h>

#ifdef _WIN32
#define tempnam( d, p ) _tempnam( d, p )
#endif

namespace kwiver {

namespace testing {

// ----------------------------------------------------------------------------

/** @brief Generate a unique file name in the current working directory.
 *
 * @param prefix Prefix for generated file name.
 * @param suffix Suffix for generated file name, ignored.
 */
std::string
temp_file_name( char const* prefix, char const* suffix )
{
  // to use mkstemp, the template must have 6 X's at the end, so we ignore the
  // suffix
  std::string templateName = "./" + std::string( prefix ) + "XXXXXX";
  std::vector< char > tempFileName( templateName.begin(), templateName.end() );
  tempFileName.push_back( '\0' );

  int fd = mkstemp( tempFileName.data() );
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
