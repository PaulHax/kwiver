// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief test range validity-filter

#include <vital/range/valid.h>

#include <gtest/gtest.h>

#include <memory>
#include <vector>

using namespace kwiver::vital;

// ----------------------------------------------------------------------------
int
main( int argc, char** argv )
{
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}

// ----------------------------------------------------------------------------
TEST ( range_valid, empty )
{
  auto test_values = std::vector< int >{};

  auto counter = int{ 0 };
  for( auto x : test_values | range::valid )
  {
    static_cast< void >( x );
    ++counter;
  }

  EXPECT_EQ( 0, counter );
}

// ----------------------------------------------------------------------------
TEST ( range_valid, none )
{
  auto test_values = std::vector< bool >{ false, false };

  auto counter = int{ 0 };
  for( auto x : test_values | range::valid )
  {
    static_cast< void >( x );
    ++counter;
  }

  EXPECT_EQ( 0, counter );
}

// ----------------------------------------------------------------------------
TEST ( range_valid, basic )
{
  auto test_values = std::vector< std::shared_ptr< int > >{
    std::shared_ptr< int >{ nullptr },
    std::shared_ptr< int >{ new int{ 1 } },
    std::shared_ptr< int >{ nullptr },
    std::shared_ptr< int >{ new int{ 2 } },
    std::shared_ptr< int >{ new int{ 3 } },
    std::shared_ptr< int >{ nullptr },
    std::shared_ptr< int >{ nullptr },
    std::shared_ptr< int >{ new int{ 4 } },
    std::shared_ptr< int >{ new int{ 5 } },
    std::shared_ptr< int >{ nullptr } };

  auto accumulator = int{ 0 };
  for( auto p : test_values | range::valid )
  {
    accumulator += *p;
  }

  EXPECT_EQ( 15, accumulator );
}

// ----------------------------------------------------------------------------
TEST ( range_valid, mutating )
{
  auto test_values = std::vector< int >{ 1, 2, 3, 4, 5 };

  for( auto& x : test_values | range::valid )
  {
    if( x == 3 )
    {
      x = 42;
    }
  }

  EXPECT_EQ( 42, test_values[ 2 ] );
}
