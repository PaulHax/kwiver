// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include "average_frames.h"

#include <arrows/vxl/image_container.h>

#include <vil/vil_convert.h>
#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
#include <vil/vil_pixel_format.h>

#include <deque>
#include <exception>
#include <map>
#include <memory>

namespace kwiver {

namespace arrows {

namespace vxl {

namespace {

// ----------------------------------------------------------------------------
class online_frame_averager_base
{
public:
  virtual ~online_frame_averager_base() = default;

  // Reset the internal average
  virtual void reset() = 0;

protected:
  // Should we spend a little bit of extra time rounding outputs?
  bool should_round_ = false;

  // The last average in double form
  vil_image_view< double > last_average_;
};

// ----------------------------------------------------------------------------
// Base class for all online frame averager instances
template < typename PixType >
class online_frame_averager : public online_frame_averager_base
{
public:
  // Process a new frame, returning the current frame average
  virtual void process_frame(
    vil_image_view< PixType > const& input,
    vil_image_view< PixType >& average ) = 0;

  // Process a new frame, and additionally compute a per-pixel instantaneous
  // variance estimation, which can be further averaged to estimate the
  // per-pixel variance over x frames
  void process_frame(
    vil_image_view< PixType > const& input, vil_image_view< PixType >& average,
    vil_image_view< double >& variance );

protected:
  // Is the resolution of the input image different from prior inputs?
  bool has_resolution_changed( vil_image_view< PixType > const& input );

private:
  // Temporary buffers used for variance calculations if they're enabled
  vil_image_view< double > dev1_tmp_space_;
  vil_image_view< double > dev2_tmp_space_;
};

// ----------------------------------------------------------------------------
// A cumulative frame averager
template < typename PixType >
class cumulative_frame_averager : public online_frame_averager< PixType >
{
public:
  cumulative_frame_averager( bool const should_round = false );

  // Process a new frame, returning the current frame average
  void process_frame(
    vil_image_view< PixType > const& input,
    vil_image_view< PixType >& average ) override;

  // Reset the internal average.
  void reset() override;

private:
  // The number of observed frames since the last reset
  size_t frame_count_;
};

// ----------------------------------------------------------------------------
// An exponential frame averager
template < typename PixType >
class exponential_frame_averager : public online_frame_averager< PixType >
{
public:
  exponential_frame_averager(
    bool const should_round = false,
    double const new_frame_weight = 0.5 );

  // Process a new frame, returning the current frame average
  void process_frame(
    vil_image_view< PixType > const& input,
    vil_image_view< PixType >& average ) override;

  // Reset the internal average.
  void reset() override;

private:
  // The exponential averaging coefficient
  double new_frame_weight_;

  // The number of observed frames since the last reset
  unsigned frame_count_;
};

// ----------------------------------------------------------------------------
// A windowed frame averager
template < typename PixType >
class windowed_frame_averager : public online_frame_averager< PixType >
{
public:
  using input_type = vil_image_view< PixType >;

  windowed_frame_averager(
    bool const should_round = false,
    unsigned const window_length = 20 );

  // Process a new frame, returning the current frame average
  void process_frame(
    vil_image_view< PixType > const& input,
    vil_image_view< PixType >& average ) override;

  // Reset the internal average
  void reset() override;

  // Get number of frames used in the current window
  size_t frame_count() const;

private:
  // Buffer containing pointers to last window_length frames
  std::deque< vil_image_view< PixType > > window_buffer_;
  size_t window_buffer_capacity_;
};

// ----------------------------------------------------------------------------
// Shared functionality - process a frame while computing variance
template < typename PixType >
void
online_frame_averager< PixType >
::process_frame(
  vil_image_view< PixType > const& input,
  vil_image_view< PixType >& average,
  vil_image_view< double >& variance )
{
  // Check if this is the first time we have processed a frame of this size
  if( dev1_tmp_space_.ni() != input.ni() ||
      dev1_tmp_space_.nj() != input.nj() ||
      dev1_tmp_space_.nplanes() != input.nplanes() )
  {
    dev1_tmp_space_.set_size( input.ni(), input.nj(), input.nplanes() );
    dev2_tmp_space_.set_size( input.ni(), input.nj(), input.nplanes() );
    variance.set_size( input.ni(), input.nj(), input.nplanes() );
    variance.fill( 0.0 );
    this->process_frame( input, average );
    return;
  }

  // Calculate difference from last average
  vil_math_image_abs_difference( input, this->last_average_, dev1_tmp_space_ );

  // Update internal average
  this->process_frame( input, average );

  // Update the variance
  vil_math_image_abs_difference( input, average, dev2_tmp_space_ );
  vil_math_image_product( dev1_tmp_space_, dev2_tmp_space_, dev1_tmp_space_ );
  variance.deep_copy( dev1_tmp_space_ );
}

// ----------------------------------------------------------------------------
template < typename PixType >
bool
online_frame_averager< PixType >
::has_resolution_changed( vil_image_view< PixType > const& input )
{
  return ( input.ni() != this->last_average_.ni() ||
           input.nj() != this->last_average_.nj() ||
           input.nplanes() != this->last_average_.nplanes() );
}

// ----------------------------------------------------------------------------
// Helper function to allocate a completely new image, and cast the input image
// to whatever specified type the output image is, scaling by some factor if
// set and rounding if enabled, in one pass.
template < typename inT, typename outT >
void
copy_cast(
  vil_image_view< inT > const& input, vil_image_view< outT >& output,
  bool round )
{
  // Just deep copy if the pixel formats are equivalent
  if( std::is_same< inT, outT >::value )
  {
    output.deep_copy( input );
    return;
  }

  // Determine if any rounding would even be beneficial based on source types
  if( round )
  {
    round = !std::numeric_limits< inT >::is_integer &&
            std::numeric_limits< outT >::is_integer;
  }

  if( round )
  {
    vil_convert_round( input, output );
  }
  else
  {
    vil_convert_cast( input, output );
  }
}

// ----------------------------------------------------------------------------
// Cumulative averager
template < typename PixType >
cumulative_frame_averager< PixType >

::cumulative_frame_averager( bool const should_round )
{
  this->should_round_ = should_round;
  this->reset();
}

// ----------------------------------------------------------------------------
template < typename PixType >
void
cumulative_frame_averager< PixType >
::process_frame(
  vil_image_view< PixType > const& input,
  vil_image_view< PixType >& average )
{
  if( this->has_resolution_changed( input ) )
  {
    this->reset();
  }

  // If this is the first frame observed or there was an indicated reset
  if( this->frame_count_ == 0 )
  {
    vil_convert_cast( input, this->last_average_ );
  }
  // Standard update case
  else
  {
    // Calculate new average - TODO: Non-exponential cumulative average can be
    // modified to be more efficient and prevent precision losses by not using
    // math_add_fraction function. Can also be optimized in the byte case to
    // use integer instead of double operations, but it's good enough for now.
    auto const scale_factor =
      1.0 / static_cast< double >( this->frame_count_ + 1 );

    vil_math_add_image_fraction(
      this->last_average_, 1.0 - scale_factor,
      input, scale_factor );
  }

  // Copy a completely new image
  copy_cast( this->last_average_, average, this->should_round_ );

  // Increase observed frame count
  ++this->frame_count_;
}

// ----------------------------------------------------------------------------
template < typename PixType >
void
cumulative_frame_averager< PixType >
::reset()
{
  this->frame_count_ = 0;
}

// ----------------------------------------------------------------------------
// Exponential averager
template < typename PixType >
exponential_frame_averager< PixType >

::exponential_frame_averager(
  bool const should_round,
  double const new_frame_weight )
{
  this->should_round_ = should_round;
  this->new_frame_weight_ = new_frame_weight;
  this->reset();
}

// ----------------------------------------------------------------------------
template < typename PixType >
void
exponential_frame_averager< PixType >
::process_frame(
  vil_image_view< PixType > const& input,
  vil_image_view< PixType >& average )
{
  if( this->has_resolution_changed( input ) )
  {
    this->reset();
  }

  // If this is the first frame observed or there was an indicated reset
  if( this->frame_count_ == 0 )
  {
    vil_convert_cast( input, this->last_average_ );
  }
  // Standard update case
  else
  {
    vil_math_add_image_fraction(
      this->last_average_, 1.0 - new_frame_weight_,
      input, new_frame_weight_ );
  }

  // Copy a completely new image in case we are running in async mode
  copy_cast( this->last_average_, average, this->should_round_ );

  // Increase observed frame count
  ++this->frame_count_;
}

// ----------------------------------------------------------------------------
template < typename PixType >
void
exponential_frame_averager< PixType >
::reset()
{
  this->frame_count_ = 0;
}

// ----------------------------------------------------------------------------
// Windowed averager
template < typename PixType >
windowed_frame_averager< PixType >

::windowed_frame_averager(
  bool const should_round,
  unsigned const window_length )
{
  this->window_buffer_capacity_ = window_length;
  this->should_round_ = should_round;
  this->reset();
}

// ----------------------------------------------------------------------------
template < typename PixType >
void
windowed_frame_averager< PixType >
::process_frame(
  vil_image_view< PixType > const& input,
  vil_image_view< PixType >& average )
{
  if( this->has_resolution_changed( input ) )
  {
    this->reset();
  }

  // Early exit cases: the buffer is currently filling
  auto const window_buffer_size = window_buffer_.size();
  if( window_buffer_size == 0 )
  {
    vil_convert_cast( input, this->last_average_ );
  }
  else if( window_buffer_size < window_buffer_capacity_ )
  {
    double src_weight =
      1.0 / ( static_cast< double >( window_buffer_size ) + 1.0 );
    vil_math_add_image_fraction(
      this->last_average_, 1.0 - src_weight, input,
      src_weight );
  }
  // Standard case, buffer is full
  else
  {
    // Scan image subtracting the last frame, and adding the new one from
    // the previous average
    auto const ni = input.ni();
    auto const nj = input.nj();
    auto const np = input.nplanes();

    // Image A = Removed Entry, B = Added Entry, C = The Average Calculation
    auto const scale = 1.0 / static_cast< double >( window_buffer_size );

    input_type const& tmpA = window_buffer_[ window_buffer_size - 1 ];
    input_type const* imA = &tmpA;
    input_type const* imB = &input;
    vil_image_view< double >* imC = &( this->last_average_ );

    auto istepA = imA->istep();
    auto jstepA = imA->jstep();
    auto pstepA = imA->planestep();
    auto istepB = imB->istep();
    auto jstepB = imB->jstep();
    auto pstepB = imB->planestep();
    auto istepC = imC->istep();
    auto jstepC = imC->jstep();
    auto pstepC = imC->planestep();

    PixType const* planeA = imA->top_left_ptr();
    PixType const* planeB = imB->top_left_ptr();
    double*        planeC = imC->top_left_ptr();

    for( unsigned p = 0; p < np;

         ++p, planeA += pstepA, planeB += pstepB, planeC += pstepC )
    {
      PixType const* rowA = planeA;
      PixType const* rowB = planeB;
      double*        rowC = planeC;

      for( unsigned j = 0; j < nj;

           ++j, rowA += jstepA, rowB += jstepB, rowC += jstepC )
      {
        PixType const* pixelA = rowA;
        PixType const* pixelB = rowB;
        double*        pixelC = rowC;

        for( unsigned i = 0; i < ni;
             ++i, pixelA += istepA, pixelB += istepB, pixelC += istepC )
        {
          *pixelC += scale *
                     ( static_cast< double >( *pixelB ) -
                       static_cast< double >( *pixelA ) );
        }
      }
    }
  }

  // Add to buffer
  this->window_buffer_.push_back( input );
  if( this->window_buffer_.size() > this->window_buffer_capacity_ )
  {
    this->window_buffer_.pop_front();
  }

  // Copy into output
  copy_cast( this->last_average_, average, this->should_round_ );
}

// ----------------------------------------------------------------------------
template < typename PixType >
void
windowed_frame_averager< PixType >
::reset()
{
  this->window_buffer_.clear();
}

// ----------------------------------------------------------------------------
template < typename PixType >
size_t
windowed_frame_averager< PixType >
::frame_count() const
{
  return this->window_buffer_.size();
}

} // end anonoymous namespace

// ----------------------------------------------------------------------------
// Private implementation class
class average_frames::priv
{
public:
  using frame_averager_byte_sptr =
    std::unique_ptr< online_frame_averager< vxl_byte > >;
  using frame_averager_float_sptr =
    std::unique_ptr< online_frame_averager< double > >;

  priv( average_frames& parent ) : parent{ parent } {}

  average_frames& parent;

  const std::string&
  c_type() const { return parent.c_type; }
  unsigned
  c_window_size() const { return parent.c_window_size; }
  double
  c_exp_weight() const { return parent.c_exp_weight; }
  bool
  c_round() const { return parent.c_round; }
  bool
  c_output_variance() const { return parent.c_output_variance; }

  // The actual frame averagers
  using averager_ptr = std::unique_ptr< online_frame_averager_base >;

  std::map< vil_pixel_format, averager_ptr > frame_averager;

  // --------------------------------------------------------------------------
  // Load model, special optimizations are in place for the byte case
  template < typename PixType >
  online_frame_averager< PixType >*
  load_model()
  {
    auto& averager = frame_averager[ vil_pixel_format_of( PixType{} ) ];
    if( !averager )
    {
      switch( averager_converter().from_string( c_type() ) )
      {
        case AVERAGER_window:
        {
          averager.reset( new windowed_frame_averager< PixType >{} );
          break;
        }
        case AVERAGER_cumulative:
        {
          averager.reset(
            new cumulative_frame_averager< PixType >{ c_round() } );
          break;
        }
        case AVERAGER_exponential:
        {
          if( c_exp_weight() <= 0 || c_exp_weight() >= 1 )
          {
            throw std::runtime_error{
                    "Invalid exponential averaging coefficient!" };
          }

          averager.reset(
            new exponential_frame_averager< PixType >{ c_round(),
                                                       c_exp_weight() } );
          break;
        }
      }
    }

    return static_cast< online_frame_averager< PixType >* >( averager.get() );
  }

  // --------------------------------------------------------------------------
  // Compute the updated average with the current frame
  // return the average or the variance
  template < typename PixType >
  kwiver::vital::image_container_sptr
  process_frame( vil_image_view< PixType > input )
  {
    auto* const averager = load_model< PixType >();

    if( !c_output_variance() )
    {
      vil_image_view< PixType > output;
      averager->process_frame( input, output );
      return std::make_shared< vxl::image_container >( output );
    }
    else
    {
      vil_image_view< PixType > tmp;
      vil_image_view< double > output;
      averager->process_frame( input, tmp, output );
      return std::make_shared< vxl::image_container >( output );
    }
  }
};

// ----------------------------------------------------------------------------
void
average_frames
::initialize()
{
  KWIVER_INITIALIZE_UNIQUE_PTR( priv, d );
  attach_logger( "arrows.vxl.average_frames" );
}

// ----------------------------------------------------------------------------
bool
average_frames
::check_configuration( vital::config_block_sptr config ) const
{
  auto const& c_type = config->get_enum_value< averager_converter >( "type" );
  if( !( c_type == AVERAGER_cumulative || c_type == AVERAGER_window ||
         c_type == AVERAGER_exponential ) )
  {
    return false;
  }
  else if( c_type == AVERAGER_exponential )
  {
    double exp_weight = config->get_value< double >( "exp_weight" );
    if( exp_weight <= 0 || exp_weight > 1 )
    {
      return false;
    }
  }
  return true;
}

// ----------------------------------------------------------------------------
kwiver::vital::image_container_sptr
average_frames
::filter( kwiver::vital::image_container_sptr image_data )
{
  // Perform Basic Validation
  if( !image_data )
  {
    LOG_ERROR( logger(), "Invalid input image." );
    return image_data;
  }

  // Get input image
  vil_image_view_base_sptr view =
    vxl::image_container::vital_to_vxl( image_data->get_image() );

  // Perform different actions based on input type
#define HANDLE_CASE( T )                                           \
  case T:                                                          \
    {                                                              \
      using pix_t = vil_pixel_format_type_of< T >::component_type; \
      vil_image_view< pix_t > input = view;                        \
      return d->process_frame( input );                            \
      break;                                                       \
    }

  switch( view->pixel_format() )
  {
  HANDLE_CASE( VIL_PIXEL_FORMAT_BOOL );
  HANDLE_CASE( VIL_PIXEL_FORMAT_BYTE );
  HANDLE_CASE( VIL_PIXEL_FORMAT_SBYTE );
  HANDLE_CASE( VIL_PIXEL_FORMAT_UINT_16 );
  HANDLE_CASE( VIL_PIXEL_FORMAT_INT_16 );
  HANDLE_CASE( VIL_PIXEL_FORMAT_UINT_32 );
  HANDLE_CASE( VIL_PIXEL_FORMAT_INT_32 );
  HANDLE_CASE( VIL_PIXEL_FORMAT_UINT_64 );
  HANDLE_CASE( VIL_PIXEL_FORMAT_INT_64 );
  HANDLE_CASE( VIL_PIXEL_FORMAT_FLOAT );
  HANDLE_CASE( VIL_PIXEL_FORMAT_DOUBLE );
#undef HANDLE_CASE

    default:
      // The image type was not one we handle
      LOG_ERROR(
        logger(), "Unsupported input format " << view->pixel_format()
                                              << " type received" );
      return nullptr;
  }

  // Code not reached, prevent warning
  return nullptr;
}

} // end namespace vxl

} // end namespace arrows

} // end namespace kwiver
