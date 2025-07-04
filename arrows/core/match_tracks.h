// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Header defining functions to match tracks
///
/// This file contains various convenience functions that apply an algorithm
/// designed to match features to a set of feature tracks which contain
/// features.  In short, these functions extract features from the tracks,
/// match them with the provided algorithm, and then map those feature
/// matches back to the corresponding track matches.

#ifndef KWIVER_ARROWS_CORE_MATCH_TRACKS_H_
#define KWIVER_ARROWS_CORE_MATCH_TRACKS_H_

#include <map>
#include <vector>

#include <vital/algo/match_features.h>
#include <vital/types/feature_track_set.h>

namespace kwiver {

namespace arrows {

namespace core {

/// Typedef for a vector of pairs of tracks
typedef std::vector< std::pair< vital::track_sptr,
  vital::track_sptr > > track_pairs_t;
/// Typedef for a map from one track to another
typedef std::map< vital::track_sptr, vital::track_sptr > track_map_t;

/// Compute matching feature track pairs between two frames
///
/// This function extracts all the feature tracks found on \p current_frame and
/// \p target_frame.  It then extracts the corresponding features and
/// descriptors
/// and uses the provided matcher algorithm to identify matching tracks
///
///  \param matcher The matcher algorithm to use in feature/descriptor matching
///  \param all_tracks The set of all feature tracks on which to detect matches
///  \param current_frame The index of the source frame to match
///  \param target_frame The index of the destination frame to match
///  \return A vector of matching track pairs of the form (current, target)
track_pairs_t match_tracks(
  vital::algo::match_features_sptr matcher,
  vital::feature_track_set_sptr all_tracks,
  vital::frame_id_t current_frame,
  vital::frame_id_t target_frame );

/// Compute matching feature track pairs between two frames
///
/// This function extracts all the feature tracks found on \p target_frame.
/// It then extracts the corresponding features and descriptors
/// and uses the provided matcher algorithm to identify matching tracks between
/// the set of provided current tracks, features, and descriptors.  This version
/// of the function exists so that the current tracks, features, and
/// descriptors do not need to be extracted each time if matching against
/// multiple target frames.
///
///  \param matcher The matcher algorithm to use in feature/descriptor matching
///  \param all_tracks The set of all feature tracks on which to detect matches
///  \param current_tracks A subset of \p all_tracks intersecting the source
/// frame
///  \param current_features The features corresponding to \p current_tracks on
/// the source frame
///  \param current_descriptors The descriptors corresponding to \p
/// current_tracks on the source frame
///  \param target_frame The index of the destination frame to match
///  \return A vector of matching track pairs of the form (current, target)
track_pairs_t match_tracks(
  vital::algo::match_features_sptr matcher,
  vital::feature_track_set_sptr all_tracks,
  vital::feature_track_set_sptr current_tracks,
  vital::feature_set_sptr current_features,
  vital::descriptor_set_sptr current_descriptors,
  vital::frame_id_t target_frame );

/// Compute matching feature track pairs between two frames
///
/// This function uses the provide matcher algorithm to identify matching tracks
/// between the sets of provided tracks, features, and descriptors.
/// It is assumed that the current and target track sets contain only tracks
/// with states covering the current and target frames respectively.
/// Furthermore, the provided features and descriptors are extracted from the
/// corresponding tracks on those frames.  This version of the function exists
/// so that the current tracks, features, and descriptors do not need to be
/// extracted each time if matching multiple frame combinations.
///
///  \param matcher The matcher algorithm to use in feature/descriptor matching
///  \param current_tracks A set of feature tracks intersecting the source frame
///  \param current_features The features corresponding to \p current_tracks on
/// the source frame
///  \param current_descriptors The descriptors corresponding to \p
/// current_tracks on the source frame
///  \param target_tracks A set of feature tracks intersecting the target frame
///  \param target_features The features corresponding to \p target_tracks on
/// the target frame
///  \param target_descriptors The descriptors corresponding to \p target_tracks
/// on the target frame
///  \return A vector of matching track pairs of the form (current, target)
track_pairs_t match_tracks(
  vital::algo::match_features_sptr matcher,
  vital::feature_track_set_sptr current_tracks,
  vital::feature_set_sptr current_features,
  vital::descriptor_set_sptr current_descriptors,
  vital::feature_track_set_sptr target_tracks,
  vital::feature_set_sptr target_features,
  vital::descriptor_set_sptr target_descriptors );

} // end namespace core

} // end namespace arrows

} // end namespace kwiver

#endif
