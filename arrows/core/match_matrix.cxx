// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Implementation of match matrix computation

#include "match_matrix.h"

#include <map>

namespace kwiver {

namespace arrows {

/// Compute the match matrix from a track set
Eigen::SparseMatrix< unsigned int >
match_matrix(
  vital::track_set_sptr tracks,
  std::vector< vital::frame_id_t >& frames )
{
  // if no frames ids specified then get all frame ids in the track set
  if( frames.empty() )
  {
    std::set< vital::frame_id_t > frame_ids = tracks->all_frame_ids();
    frames = std::vector< vital::frame_id_t >(
      frame_ids.begin(),
      frame_ids.end() );
  }

  const unsigned int num_frames = static_cast< unsigned int >( frames.size() );

  // build a frame map for reverse lookup of matrix indices
  std::map< vital::frame_id_t, unsigned int > frame_map;
  for( unsigned int i = 0; i < num_frames; ++i )
  {
    frame_map[ frames[ i ] ] = i;
  }

  // compute an upper bound on non-zero matrix entries to
  // pre-allocate the sparse matrix memory
  unsigned int max_size = 0;
  const std::vector< vital::track_sptr > trks = tracks->tracks();
  for( const vital::track_sptr& t : trks )
  {
    if( t->size() > max_size )
    {
      max_size = static_cast< unsigned int >( t->size() );
    }
  }

  Eigen::SparseMatrix< unsigned int > mm( num_frames, num_frames );
  mm.reserve( Eigen::VectorXi::Constant( num_frames, max_size ) );

  // fill in the matching matrix (lower triangular part only)
  for( const vital::track_sptr& t : trks )
  {
    // get all the frames covered by this track
    std::set< vital::frame_id_t > t_frames = t->all_frame_ids();
    // map the frames to a vector of all valid matrix indices
    std::set< unsigned int > t_ind;
    for( const vital::frame_id_t& fid : t_frames )
    {
      std::map< vital::frame_id_t,
        unsigned int >::const_iterator fmi = frame_map.find( fid );
      // only add to the vector if in the map
      if( fmi != frame_map.end() )
      {
        t_ind.insert( fmi->second );
      }
    }

    // fill in the matrix (lower triangular part)
    typedef std::set< unsigned int >::const_iterator sitr_t;
    for( sitr_t tfi1 = t_ind.begin(); tfi1 != t_ind.end(); ++tfi1 )
    {
      for( sitr_t tfi2 = tfi1; tfi2 != t_ind.end(); ++tfi2 )
      {
        ++mm.coeffRef( *tfi2, *tfi1 );
      }
    }
  }

  // compress storage by removing empty entries
  mm.makeCompressed();
  // return a symmetric view of the lower triangular matrix
  return mm.selfadjointView< Eigen::Lower >();
}

/// Compute a score for each track based on its importance to the match matrix.
std::map< vital::track_id_t, double >
match_matrix_track_importance(
  vital::track_set_sptr tracks,
  std::vector< vital::frame_id_t > const& frames,
  Eigen::SparseMatrix< unsigned int > const& mm )
{
  // build a frame map for reverse lookup of matrix indices
  std::map< vital::frame_id_t, unsigned int > frame_map;
  const unsigned int num_frames = static_cast< unsigned int >( frames.size() );
  for( unsigned int i = 0; i < num_frames; ++i )
  {
    frame_map[ frames[ i ] ] = i;
  }

  // score the importance of each track against the match matrix
  std::vector< vital::track_sptr > trks = tracks->tracks();
  std::map< vital::track_id_t, double > scores;
  for( const vital::track_sptr& t : trks )
  {
    // get all the frames covered by this track
    std::set< vital::frame_id_t > t_frames = t->all_frame_ids();
    // map the frames to a vector of all valid matrix indices
    std::set< unsigned int > t_ind;
    for( const vital::frame_id_t& fid : t_frames )
    {
      std::map< vital::frame_id_t,
        unsigned int >::const_iterator fmi = frame_map.find( fid );
      // only add to the vector if in the map
      if( fmi != frame_map.end() )
      {
        t_ind.insert( fmi->second );
      }
    }

    // get the scores from the match matrix
    double score = 0.0;
    typedef std::set< unsigned int >::const_iterator sitr_t;
    for( sitr_t tfi1 = t_ind.begin(); tfi1 != t_ind.end(); ++tfi1 )
    {
      for( sitr_t tfi2 = tfi1; tfi2 != t_ind.end(); ++tfi2 )
      {
        score += 1.0 / mm.coeff( *tfi2, *tfi1 );
      }
    }
    scores[ t->id() ] = score;
  }

  return scores;
}

} // end namespace arrows

} // end namespace kwiver
