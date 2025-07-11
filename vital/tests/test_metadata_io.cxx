// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief core metadata_io tests

#include <tests/test_gtest.h>

#include <kwiversys/SystemTools.hxx>
#include <vital/exceptions.h>
#include <vital/io/metadata_io.h>
#include <vital/types/geo_point.h>
#include <vital/types/geodesy.h>

#include <iostream>
#include <sstream>

kwiver::vital::path_t g_data_dir;

static std::string sample_pos = "vital_data/sample_pos.pos";
static std::string sample_pos_no_name = "vital_data/sample_pos_no_name.pos";
static std::string invalid_pos = "vital_data/invalid_pos.pos";

// ----------------------------------------------------------------------------
int
main( int argc, char** argv )
{
  ::testing::InitGoogleTest( &argc, argv );

  GET_ARG( 1, g_data_dir );

  return RUN_ALL_TESTS();
}

// ----------------------------------------------------------------------------
class metadata_pos_io : public ::testing::Test
{
  TEST_ARG( data_dir );
};

// ----------------------------------------------------------------------------
TEST_F ( metadata_pos_io, pos_format_read )
{
  kwiver::vital::path_t test_read_file = data_dir + "/" + sample_pos;
  auto input_md = kwiver::vital::read_pos_file( test_read_file );
  print_metadata( std::cout, *input_md );

  test_read_file = data_dir + "/" + sample_pos_no_name;
  input_md = kwiver::vital::read_pos_file( test_read_file );
  print_metadata( std::cout, *input_md );
}

// ----------------------------------------------------------------------------
TEST_F ( metadata_pos_io, invalid_file_path )
{
  EXPECT_THROW(
    auto md = kwiver::vital::read_pos_file( data_dir + "/not_a_file.blob" ),
    kwiver::vital::file_not_found_exception )
    << "tried loading an invalid file path";
}

// ----------------------------------------------------------------------------
TEST_F ( metadata_pos_io, invalid_file_content )
{
  kwiver::vital::path_t invalid_content_file = data_dir + "/" + invalid_pos;
  EXPECT_THROW(
    auto md = kwiver::vital::read_pos_file( invalid_content_file ),
    kwiver::vital::invalid_data )
    << "tried loading a file with invalid data";
}

namespace {

// ----------------------------------------------------------------------------
void
compare_tag(
  kwiver::vital::metadata_item const& expected,
  kwiver::vital::metadata_sptr const& md )
{
  constexpr static double epsilon = 1e-8;

  SCOPED_TRACE( "At tag " + expected.name() );
  ASSERT_TRUE( md->has( expected.tag() ) );

  auto const& actual = md->find( expected.tag() );
  if( expected.type() == typeid( double ) )
  {
    EXPECT_NEAR( expected.as_double(), actual.as_double(), epsilon );
  }
  else if( expected.type() == typeid( uint64_t ) )
  {
    EXPECT_EQ( expected.as_uint64(), actual.as_uint64() );
  }
  else if( expected.type() == typeid( int ) )
  {
    EXPECT_EQ( expected.get< int >(), actual.get< int >() );
  }
  else if( expected.type() == typeid( std::string ) )
  {
    EXPECT_EQ( expected.as_string(), actual.as_string() );
  }
  else if( expected.type() == typeid( kwiver::vital::geo_point ) )
  {
    auto const v1 = expected.get< kwiver::vital::geo_point >();
    auto const v2 = actual.get< kwiver::vital::geo_point >();
    auto const& rv1 = v1.location( kwiver::vital::SRID::lat_lon_WGS84 );
    auto const& rv2 = v2.location( kwiver::vital::SRID::lat_lon_WGS84 );
    EXPECT_NEAR( rv1[ 1 ], rv2[ 1 ], epsilon ) << " (lat)";
    EXPECT_NEAR( rv1[ 0 ], rv2[ 0 ], epsilon ) << " (long)";
  }
  else
  {
    std::cout << "Unable to compare tag " << expected.name() << std::endl;
  }
}

} // namespace

// ----------------------------------------------------------------------------
TEST_F ( metadata_pos_io, output_format )
{
  kwiver::vital::path_t test_read_file = data_dir + "/" + sample_pos;
  auto input_md = kwiver::vital::read_pos_file( test_read_file );
  print_metadata( std::cout, *input_md );

  kwiver::vital::path_t temp_file = "temp.pos";
  kwiver::vital::write_pos_file( *input_md, temp_file );

  auto md = kwiver::vital::read_pos_file( temp_file );

  EXPECT_EQ( input_md->size(), md->size() )
    << "Metadata does not have same size after IO!";

  for( auto mdi : *input_md )
  {
    compare_tag( *mdi.second, md );
  }

  kwiversys::SystemTools::RemoveFile( temp_file );
}
