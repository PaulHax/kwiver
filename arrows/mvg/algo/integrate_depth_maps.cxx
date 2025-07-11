// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/**
 * \file
 * \brief Source file for integration of depth maps using a voxel grid
 */

#include <arrows/core/depth_utils.h>
#include <arrows/mvg/algo/integrate_depth_maps.h>

#include <vital/util/transform_image.h>

#include <sstream>

using namespace kwiver::vital;

namespace kwiver {

namespace arrows {

namespace mvg {

/// Private implementation class
class integrate_depth_maps::priv
{
public:
  // Constructor
  priv( integrate_depth_maps& parent )
    : parent( parent ),
      m_logger( vital::get_logger( "arrows.mvg.integrate_depth_maps" ) )
  {}

  integrate_depth_maps& parent;
  // Configuration values
  double
  c_ray_potential_rho() const { return parent.c_ray_potential_rho; }
  double
  c_ray_potential_thickness() const { return parent.c_ray_potential_thickness; }
  double
  c_ray_potential_eta() const { return parent.c_ray_potential_eta; }
  double
  c_ray_potential_epsilon() const { return parent.c_ray_potential_epsilon; }
  double
  c_ray_potential_delta() const { return parent.c_ray_potential_delta; }
  double
  c_voxel_spacing_factor() const { return parent.c_voxel_spacing_factor; }
  std::array< double, 3 >
  c_grid_spacing() const { return parent.c_grid_spacing; }

  // integrate a depth image into the integration volume
  void integrate_depth_map(
    image_of< double >& volume,
    camera_perspective const& camera,
    image_of< double > const& depth,
    image_of< double > const& weight,
    vector_3d const& origin,
    vector_3d const& spacing ) const;

  // integrate a depth image using a 3x4 camera matrix
  // this version is faster when there is no distrition
  void integrate_depth_map(
    image_of< double >& volume,
    matrix_3x4d const& camera,
    image_of< double > const& depth,
    image_of< double > const& weight,
    vector_3d const& origin,
    vector_3d const& spacing ) const;

  // compute the TSDF ray potential given an estimated depth and real depth
  double ray_potential( double est_depth, double real_depth ) const;

  // extract the depth at a point from the depth image and call ray_potiential
  double ray_potiential_at_point(
    vector_2d const& image_pt,
    double const& real_d,
    image_of< double > const& depth ) const;

  // weighted version of ray_potiential_at_point
  double ray_potiential_at_point(
    vector_2d const& image_pt,
    double const& real_d,
    image_of< double > const& depth,
    image_of< double > const& weight ) const;

  // Actual spacing is computed as
  //   voxel_scale_factor * pixel_to_world_scale * grid_spacing
  // relative spacings per dimension
  int grid_dims[ 3 ];

  double const_thickness;
  double const_delta;
  double const_slope;
  double const_freespace_val;
  double const_occluded_val;

  // Logger handle
  vital::logger_handle_t m_logger;
};

// ----------------------------------------------------------------------------

// integrate a depth image into the integration volume
template < typename OP >
void
accumulate_projections(
  image_of< double >& volume,
  vector_3d const& origin,
  vector_3d const& spacing,
  matrix_3x4d const& camera,
  OP&& accum_func )
{
  auto const ni = volume.width();
  auto const nj = volume.height();
  auto const nk = static_cast< int64_t >( volume.depth() );

  // origin offset by half a step to center voxels
  vector_3d offset = origin + 0.5 * spacing;
  vector_3d homog_pt_base = camera.leftCols< 3 >() * offset + camera.col( 3 );

  vector_3d const x_step = spacing[ 0 ] * camera.col( 0 );
  vector_3d const y_step = spacing[ 1 ] * camera.col( 1 );
  vector_3d const z_step = spacing[ 2 ] * camera.col( 2 );

#pragma omp parallel for
  for( int64_t k = 0; k < nk; ++k )
  {
    vector_3d homog_pt_y = k * z_step + homog_pt_base;
    for( size_t j = 0; j < nj; ++j, homog_pt_y += y_step )
    {
      vector_3d homog_pt = homog_pt_y;
      for( size_t i = 0; i < ni; ++i, homog_pt += x_step )
      {
        volume( i, j, k ) += accum_func( homog_pt );
      }
    }
  }
}

// ----------------------------------------------------------------------------

double
integrate_depth_maps::priv
::ray_potiential_at_point(
  vector_2d const& image_pt,
  double const& real_d,
  image_of< double > const& depth ) const
{
  int const u = static_cast< int >( image_pt.x() + 0.5 );
  int const v = static_cast< int >( image_pt.y() + 0.5 );
  if( u < 0 || u >= static_cast< int >( depth.width() ) ||
      v < 0 || v >= static_cast< int >( depth.height() ) )
  {
    return 0.0;
  }

  double const& d = depth( u, v );

  if( d <= 0.0 )
  {
    return 0.0;
  }
  return ray_potential( d, real_d );
}

// ----------------------------------------------------------------------------

double
integrate_depth_maps::priv
::ray_potiential_at_point(
  vector_2d const& image_pt,
  double const& real_d,
  image_of< double > const& depth,
  image_of< double > const& weight ) const
{
  int const u = static_cast< int >( image_pt.x() + 0.5 );
  int const v = static_cast< int >( image_pt.y() + 0.5 );
  if( u < 0 || u >= static_cast< int >( depth.width() ) ||
      v < 0 || v >= static_cast< int >( depth.height() ) )
  {
    return 0.0;
  }

  double const& d = depth( u, v );
  double const& a = weight( u, v );

  if( d <= 0.0 || a <= 0.0 )
  {
    return 0.0;
  }
  return a * ray_potential( d, real_d );
}

// ----------------------------------------------------------------------------

// integrate a depth image into the integration volume
void
integrate_depth_maps::priv
::integrate_depth_map(
  image_of< double >& volume,
  camera_perspective const& camera,
  image_of< double > const& depth,
  image_of< double > const& weight,
  vector_3d const& origin,
  vector_3d const& spacing ) const
{
  if( camera.intrinsics()->dist_coeffs().empty() )
  {
    // For imagery without distortion we can combine the intrinsic and
    // extrinsic paramters into a single 3x4 projection for faster iteration
    if( weight.size() == 0 )
    {
      auto func = [ this, &depth ](vector_3d const& hpt){
                    vector_2d image_pt{ hpt[ 0 ] / hpt[ 2 ],
                                        hpt[ 1 ] / hpt[ 2 ] };
                    return ray_potiential_at_point( image_pt, hpt[ 2 ], depth );
                  };
      accumulate_projections(
        volume, origin, spacing,
        camera.as_matrix(), func );
    }
    else
    {
      auto func = [ this, &depth, &weight ](vector_3d const& hpt){
                    vector_2d image_pt{ hpt[ 0 ] / hpt[ 2 ],
                                        hpt[ 1 ] / hpt[ 2 ] };
                    return ray_potiential_at_point(
                      image_pt, hpt[ 2 ], depth,
                      weight );
                  };
      accumulate_projections(
        volume, origin, spacing,
        camera.as_matrix(), func );
    }
  }
  else
  {
    auto const K = camera.intrinsics();
    if( weight.size() == 0 )
    {
      auto func = [ this, K, &depth ](vector_3d const& hpt){
                    vector_2d image_pt = K->map( hpt );
                    return ray_potiential_at_point( image_pt, hpt[ 2 ], depth );
                  };
      accumulate_projections(
        volume, origin, spacing,
        camera.pose_matrix(), func );
    }
    else
    {
      auto func = [ this, K, &depth, &weight ](vector_3d const& hpt){
                    vector_2d image_pt = K->map( hpt );
                    return ray_potiential_at_point(
                      image_pt, hpt[ 2 ], depth,
                      weight );
                  };
      accumulate_projections(
        volume, origin, spacing,
        camera.pose_matrix(), func );
    }
  }
}

// ----------------------------------------------------------------------------

// compute the TSDF ray potential given an estimated depth and real depth
double
integrate_depth_maps::priv
::ray_potential( double est_depth, double real_depth ) const
{
  double diff = real_depth - est_depth;

  double abs_diff = std::abs( diff );

  if( abs_diff > const_delta )
  {
    return diff > 0.0 ? const_occluded_val
                      : const_freespace_val;
  }
  else if( abs_diff > const_thickness )
  {
    return std::copysign( c_ray_potential_rho(), diff );
  }

  return const_slope * diff;
}

// ----------------------------------------------------------------------------

/// Constructor
void
integrate_depth_maps
::initialize()
{
  KWIVER_INITIALIZE_UNIQUE_PTR( priv, d_ );
}

// ----------------------------------------------------------------------------

/// Destructor
integrate_depth_maps::~integrate_depth_maps()
{}

// ----------------------------------------------------------------------------

/// Check that the algorithm's currently configuration is valid
bool
integrate_depth_maps
::check_configuration( vital::config_block_sptr ) const
{
  return true;
}

// ----------------------------------------------------------------------------

void
integrate_depth_maps
::integrate(
  vector_3d const& minpt_bound,
  vector_3d const& maxpt_bound,
  std::vector< image_container_sptr > const& depth_maps,
  std::vector< image_container_sptr > const& weight_maps,
  std::vector< camera_perspective_sptr > const& cameras,
  image_container_sptr& volume,
  vector_3d& spacing ) const
{
  double pixel_to_world_scale;
  pixel_to_world_scale =
    kwiver::arrows::core::
    compute_pixel_to_world_scale( minpt_bound, maxpt_bound, cameras );

  vector_3d diff = maxpt_bound - minpt_bound;
  vector_3d orig = minpt_bound;

  spacing = vector_3d(
    d_->c_grid_spacing()[ 0 ], d_->c_grid_spacing()[ 1 ],
    d_->c_grid_spacing()[ 2 ] );
  spacing *= pixel_to_world_scale * d_->c_voxel_spacing_factor();

  double max_spacing = spacing.maxCoeff();

  // precompute constants to make ray potential computation more efficient
  d_->const_delta = d_->c_ray_potential_delta() * max_spacing;
  d_->const_thickness = d_->c_ray_potential_thickness() * max_spacing;
  d_->const_slope = d_->c_ray_potential_rho() / d_->const_thickness;
  d_->const_freespace_val = -d_->c_ray_potential_eta() *
                            d_->c_ray_potential_rho();
  d_->const_occluded_val = d_->c_ray_potential_epsilon() *
                           d_->c_ray_potential_rho();

  for( int i = 0; i < 3; i++ )
  {
    d_->grid_dims[ i ] = static_cast< int >( ( diff[ i ] / spacing[ i ] ) );
  }

  LOG_DEBUG(
    logger(), "voxel size: " << spacing[ 0 ]
                             << " "         << spacing[ 1 ]
                             << " "         << spacing[ 2 ] );
  LOG_DEBUG(
    logger(), "grid: " << d_->grid_dims[ 0 ]
                       << " "   << d_->grid_dims[ 1 ]
                       << " "   << d_->grid_dims[ 2 ] );

  LOG_INFO( logger(), "initialize volume" );

  image_of< double > voxel_grid;
  if( volume )
  {
    voxel_grid = volume->get_image();
  }
  voxel_grid.set_size(
    d_->grid_dims[ 0 ],
    d_->grid_dims[ 1 ],
    d_->grid_dims[ 2 ] );

  // fill volume with zeros
  transform_image( voxel_grid, [](double){ return 0.0; } );

  for( size_t i = 0; i < depth_maps.size(); ++i )
  {
    image_of< double > depth{ depth_maps[ i ]->get_image() };
    image_of< double > weight;
    if( i < weight_maps.size() )
    {
      auto const& w = weight_maps[ i ];
      if( w->width() == depth.width() &&
          w->height() == depth.height() )
      {
        weight = weight_maps[ i ]->get_image();
      }
    }
    if( i >= cameras.size() || !cameras[ i ] )
    {
      continue;
    }

    // integrate depthms
    LOG_INFO( logger(), "depth map " << i );
    d_->integrate_depth_map(
      voxel_grid, *cameras[ i ], depth, weight,
      orig, spacing );
  }

  volume = std::make_shared< simple_image_container >( voxel_grid );
}

} // end namespace mvg

} // end namespace arrows

} // end namespace kwiver
