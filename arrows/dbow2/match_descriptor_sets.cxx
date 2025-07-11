/*ckwg +29
 * Copyright 2017-2018 by Kitware, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *  * Neither name of Kitware, Inc. nor the names of any contributors may be
 * used
 *    to endorse or promote products derived from this software without specific
 *    prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * \file
 * \brief Implementation of bag of words matching
 */

#include "arrows/dbow2/match_descriptor_sets.h"
#include "DBoW2.h"
#include <opencv2/features2d.hpp>

#include <kwiversys/SystemTools.hxx>
#include <vital/algo/algorithm.txx>
#include <vital/algo/detect_features.h>
#include <vital/algo/extract_descriptors.h>
#include <vital/algo/image_io.h>
#include <vital/algo/match_features.h>
#include <vital/logger/logger.h>

using namespace kwiver::vital;

namespace kwiver {

namespace arrows {

namespace dbow2 {

class match_descriptor_sets::priv
{
public:
  priv( match_descriptor_sets& parent ) : parent( parent ) {}

  match_descriptor_sets& parent;

  void train_vocabulary(
    std::string training_image_list,
    std::string vocabulary_output_file );

  void train(
    std::vector< std::vector< cv::Mat > > const& features,
    std::string voc_file_path );

  void setup_voc();

  void load_vocabulary( std::string voc_file_path );

  void load_features(
    std::string training_image_list,
    std::vector< std::vector< cv::Mat > >& features );

  void descriptor_set_to_vec(
    const std::vector< descriptor_sptr >& desc_vec,
    std::vector< cv::Mat >& features,
    std::vector< size_t >& desc_indices ) const;

  cv::Mat descriptor_to_mat( descriptor_sptr ) const;

  std::vector< frame_id_t >
  query(
    const vital::descriptor_set_sptr desc,
    frame_id_t frame_number,
    bool append_to_index_on_query );

  void append_to_index(
    const vital::descriptor_set_sptr desc,
    vital::frame_id_t frame );

  kwiver::vital::logger_handle_t m_logger;

  // The vocabulary tree
  std::shared_ptr< OrbVocabulary > m_voc;

  // The inverted file database
  std::shared_ptr< OrbDatabase > m_db;

  /// The feature detector algorithm to use
  vital::algo::detect_features_sptr
  c_detector() const { return parent.c_detector; }

  /// The descriptor extractor algorithm to use
  vital::algo::extract_descriptors_sptr
  c_extractor() const { return parent.c_extractor; }

  // The image io to use
  vital::algo::image_io_sptr
  c_image_io() const { return parent.c_image_io; }

  // The path to the training image list
  const std::string&
  c_training_image_list_path() const
  {
    return parent.c_training_image_list_path;
  }

  // The path to the vocabulary
  const std::string&
  c_vocabulary_path() const { return parent.c_vocabulary_path; }

  std::map< DBoW2::EntryId, kwiver::vital::frame_id_t > m_entry_to_frame;

  int
  c_max_num_candidate_matches_from_vocabulary_tree() const
  {
    return parent.c_max_num_candidate_matches_from_vocabulary_tree;
  }

  // returns node ids this many levels up from the base of the voc tree
  int m_levels_up = 2;
};

// -----------------------------------------------------------------------------
void
match_descriptor_sets::priv
::setup_voc()
{
  if( !m_voc )
  {
    // first time we will make the voc.  Then just load it.
    try
    {
      load_vocabulary( c_vocabulary_path() );
    }
    catch( const path_not_a_file& e )
    {
      LOG_DEBUG( m_logger, e.what() );
      m_voc.reset();
    }
    catch( const path_not_exists& e )
    {
      LOG_DEBUG( m_logger, e.what() );
      m_voc.reset();
    }

    if( !m_voc )
    {
      train_vocabulary( c_training_image_list_path(), c_vocabulary_path() );
    }

    m_db = std::make_shared< OrbDatabase >( *m_voc, true, 3 );
  }
}

// -----------------------------------------------------------------------------
void
match_descriptor_sets::priv
::append_to_index(
  const vital::descriptor_set_sptr desc,
  vital::frame_id_t frame_number )
{
  setup_voc();

  if( desc->size() == 0 )
  {
    return;
  }

  std::vector< cv::Mat > desc_mats;
  std::vector< size_t > desc_mat_indices;
  auto desc_vec = desc->descriptors();
  descriptor_set_to_vec( desc_vec, desc_mats, desc_mat_indices );

  if( desc_mats.size() == 0 )
  {
    // only features without descriptors in this frame
    return;
  }

  // run them through the vocabulary to get the BOW vector
  DBoW2::BowVector bow_vec;
  DBoW2::FeatureVector feat_vec;
  m_voc->transform( desc_mats, bow_vec, feat_vec, m_levels_up );

  // store node ids in feature_track_states
  for( auto node_data : feat_vec )
  {
    auto node_id = node_data.first;
    for( auto f_idx : node_data.second )
    {
      desc_vec[ desc_mat_indices[ f_idx ] ]->set_node_id( node_id );
    }
  }

  const DBoW2::EntryId ent = m_db->add( bow_vec, feat_vec );
  std::pair< const DBoW2::EntryId, kwiver::vital::frame_id_t >
  new_ent( ent, frame_number );

  m_entry_to_frame.insert( new_ent );
}

// -----------------------------------------------------------------------------
std::vector< frame_id_t >
match_descriptor_sets::priv
::query(
  const vital::descriptor_set_sptr desc,
  frame_id_t frame_number,
  bool append_to_index_on_query )
{
  setup_voc();

  std::vector< frame_id_t > putative_matches;

  if( desc->size() == 0 )
  {
    return putative_matches;
  }

  std::vector< cv::Mat > desc_mats;
  std::vector< size_t > desc_mat_indices;
  auto desc_vec = desc->descriptors();
  descriptor_set_to_vec( desc_vec, desc_mats, desc_mat_indices );

  if( desc_mats.size() == 0 )
  {
    // only features without descriptors in this frame
    return putative_matches;
  }

  // run them through the vocabulary to get the BOW vector
  DBoW2::BowVector bow_vec;
  DBoW2::FeatureVector feat_vec;  // vector of [node id][descriptor index]
  m_voc->transform( desc_mats, bow_vec, feat_vec, m_levels_up );

  // store node ids in feature_track_states
  for( auto node_data : feat_vec )
  {
    auto node_id = node_data.first;
    for( auto f_idx : node_data.second )
    {
      desc_vec[ desc_mat_indices[ f_idx ] ]->set_node_id( node_id );
    }
  }

  int max_res = c_max_num_candidate_matches_from_vocabulary_tree();
  DBoW2::QueryResults ret;

  // add them to the database
  if( append_to_index_on_query )
  {
    const DBoW2::EntryId ent = m_db->add( bow_vec, feat_vec );
    std::pair< const DBoW2::EntryId, kwiver::vital::frame_id_t >
    new_ent( ent, frame_number );

    m_entry_to_frame.insert( new_ent );

    // querry the database for matches

    m_db->query( bow_vec, ret, max_res, ent );  // ent at the end prevents the
                                                // querry from returning the
                                                // current image.
  }
  else
  {
    m_db->query( bow_vec, ret, max_res );
  }

  putative_matches.reserve( ret.size() );

  for( auto r : ret )
  {
    auto put_match = m_entry_to_frame.find( r.Id );
    if( put_match == m_entry_to_frame.end() )
    {
      continue;
    }
    putative_matches.push_back( put_match->second );
  }

  return putative_matches;
}

// -----------------------------------------------------------------------------
void
match_descriptor_sets::priv
::train_vocabulary(
  std::string training_image_list,
  std::string vocabulary_output_file )
{
  std::vector< std::vector< cv::Mat > > features;
  load_features( training_image_list, features );

  train( features, vocabulary_output_file );
}

// -----------------------------------------------------------------------------
void
match_descriptor_sets::priv
::train(
  std::vector< std::vector< cv::Mat > > const& features,
  std::string voc_file_path )
{
  const int k = 10;  // branching factor

  size_t total_features = 0;
  for( auto& feat : features )
  {
    total_features += feat.size();
  }

  // number of levels
  int L = std::min(
    std::max(
      1,
      ( static_cast< int >( log( total_features ) / log( k ) ) ) - 1 ), 4 );

  const DBoW2::WeightingType weight = DBoW2::TF_IDF;

  const DBoW2::ScoringType score = DBoW2::L1_NORM;

  m_voc = std::make_shared< OrbVocabulary >( k, L, weight, score );
  m_voc->create( features );

  // save the vocabulary to disk
  LOG_INFO(m_logger, "Saving vocabulary ..." );
  m_voc->save( voc_file_path );
  LOG_INFO(m_logger, "Done saving vocabulary" );
}

// -----------------------------------------------------------------------------
void
match_descriptor_sets::priv
::load_vocabulary( std::string voc_file_path )
{
  if( !kwiversys::SystemTools::FileExists( voc_file_path ) )
  {
    throw path_not_exists( voc_file_path );
  }
  else if( kwiversys::SystemTools::FileIsDirectory( voc_file_path ) )
  {
    throw path_not_a_file( voc_file_path );
  }

  m_voc = std::make_shared< OrbVocabulary >( voc_file_path );
}

// -----------------------------------------------------------------------------
void
match_descriptor_sets::priv
::load_features(
  std::string training_image_list,
  std::vector< std::vector< cv::Mat > >& features )
{
  features.clear();
  features.reserve( 100 );

  cv::Ptr< cv::ORB > orb = cv::ORB::create();

  std::ifstream im_list;
  im_list.open( training_image_list );

  std::cout << "Extracting features..." << std::endl;

  std::string line;
  if( !im_list.is_open() )
  {
    LOG_ERROR(m_logger, "error while opening file " + training_image_list);
    VITAL_THROW(
      vital::invalid_file, training_image_list,
      "unable to open training image file" );
  }

  while( std::getline( im_list, line ) )
  {
    image_container_sptr im = c_image_io()->load( line );
    LOG_INFO(m_logger, "Extracting features for image " + line);

    feature_set_sptr im_features = c_detector()->detect( im );
    descriptor_set_sptr im_descriptors = c_extractor()->extract(
      im,
      im_features );

    std::vector< size_t > desc_mat_indices;
    features.push_back( std::vector< cv::Mat >() );
    descriptor_set_to_vec(
      im_descriptors->descriptors(), features.back(),
      desc_mat_indices );
  }

  if( im_list.bad() )
  {
    LOG_ERROR(m_logger, "error while reading " + training_image_list);
    VITAL_THROW(
      vital::invalid_file, training_image_list,
      "training image list bad" );
  }
}

// -----------------------------------------------------------------------------
void
match_descriptor_sets::priv
::descriptor_set_to_vec(
  const std::vector< descriptor_sptr >& desc,
  std::vector< cv::Mat >& features,
  std::vector< size_t >& desc_indices ) const
{
  desc_indices.resize( desc.size() );
  features.resize( desc.size() );

  unsigned int dn = 0;
  size_t desc_idx = 0;
  for( auto d : desc )
  {
    if( !d )
    {
      ++desc_idx;
      // skip null descriptors
      continue;
    }
    desc_indices[ dn ] = desc_idx++;
    features[ dn++ ] = descriptor_to_mat( d );
  }

  features.resize( dn );  // resize to only return features for non-null
                          // descriptors
  desc_indices.resize( dn );
}

// -----------------------------------------------------------------------------
cv::Mat
match_descriptor_sets::priv
::descriptor_to_mat( descriptor_sptr desc ) const
{
  const byte* db = desc->as_bytes();
  auto const num_bytes = desc->num_bytes();
  cv::Mat desc_mat = cv::Mat( 1, static_cast< int >( num_bytes ), CV_8UC1 );

  for( unsigned int bn = 0; bn < num_bytes; ++bn, ++db )
  {
    desc_mat.at< unsigned char >( 0, bn ) = *db;
  }
  return desc_mat;
}

// -----------------------------------------------------------------------------
void
match_descriptor_sets
::initialize()
{
  KWIVER_INITIALIZE_UNIQUE_PTR( priv, d );
  attach_logger( "arrows.dbow2.match_descriptor_sets" );
}

void
match_descriptor_sets
::append_to_index(
  const vital::descriptor_set_sptr desc,
  frame_id_t frame_number )
{
  d->append_to_index( desc, frame_number );
}

std::vector< frame_id_t >
match_descriptor_sets
::query( const vital::descriptor_set_sptr desc )
{
  return d->query( desc, -1, false );
}

std::vector< frame_id_t >
match_descriptor_sets
::query_and_append(
  const vital::descriptor_set_sptr desc,
  frame_id_t frame )
{
  return d->query( desc, frame, true );
}

// ------------------------------------------------------------------
bool
match_descriptor_sets
::check_configuration( vital::config_block_sptr config ) const
{
  bool config_valid = true;

  config_valid =
    check_nested_algo_configuration< algo::detect_features >(
      "feature_detector", config ) && config_valid;

  config_valid =
    check_nested_algo_configuration< algo::extract_descriptors >(
      "descriptor_extractor", config ) && config_valid;

  config_valid =
    check_nested_algo_configuration< algo::image_io >( "image_io", config ) &&
    config_valid;

  int max_cand_matches =
    config->get_value< int >(
      "max_num_candidate_matches_from_vocabulary_tree" );

  if( max_cand_matches <= 0 )
  {
    LOG_ERROR(
      d->m_logger,
      "max_num_candidate_matches_from_vocabulary_tree must be a positive "
      "(nonzero) integer" );
    config_valid = false;
  }

  auto voc_path = config->get_value< std::string >(
    "vocabulary_path",
    d->c_vocabulary_path() );
  auto train_path = config->get_value< std::string >(
    "training_image_list_path",
    d->c_training_image_list_path() );
  if( ( !kwiversys::SystemTools::FileExists( voc_path ) ||
        kwiversys::SystemTools::FileIsDirectory( voc_path ) ) &&
      ( !kwiversys::SystemTools::FileExists( train_path ) ||
        kwiversys::SystemTools::FileIsDirectory( train_path ) ) )
  {
    LOG_ERROR(
      d->m_logger,
      "Could not find a valid vocabulary file or training image list\n"
      "  voc file: " << voc_path << "\n"
                                    "  train list: " << train_path);
    config_valid = false;
  }

  return config_valid;
}

} // namespace dbow2

} // namespace arrows

}     // end namespace
