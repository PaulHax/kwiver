// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include "class_probability_filter.h"

#include <vital/config/config_difference.h>
#include <vital/util/string.h>

/// \todo The interactions between the list of classes and keep_all_classes
/// is confusing and awkward.  Maybe keep_all_classes should be
/// disabled if there are any items in the list of classes.  Possibly
/// add '-all-' as a meta-class name that specifies all classes and
/// remove the keep_all_classes option.

namespace kwiver {

namespace arrows {

namespace core {

using namespace kwiver::vital;

// ----------------------------------------------------------------------------
class class_probability_filter::priv
{
public:
  priv( class_probability_filter& parent )
    : parent( parent )
  {}

  class_probability_filter& parent;

  /// Configuration values
  bool c_keep_all_classes() { return parent.c_keep_all_classes; }
  double c_threshold() { return parent.c_threshold; }
  std::string c_list_of_classes() { return parent.c_list_of_classes; }
};

/*
 *  //
 * ----------------------------------------------------------------------------
 *  vital::config_block_sptr
 *  class_probability_filter
 *  ::get_configuration() const
 *  {
 *  // Get base config from base class
 *  vital::config_block_sptr config = vital::algorithm::get_configuration();
 *
 *  std::string list_of_classes;
 *  for( std::set< std::string >::const_iterator i = m_keep_classes.begin();
 *      i != m_keep_classes.end(); ++i )
 *  {
 *   list_of_classes += ( list_of_classes.empty() ) ? "" : ";" + *i;
 *  }
 *
 *  // Note that specifying a list of classes to keep and a keep-all can be
 *  // ambiguous.
 *  // What to do if keep_classes is specified in addition to keep_all_classes?
 *
 *  return config;
 *  }
 *
 *  //
 * ----------------------------------------------------------------------------
 *  void
 *  class_probability_filter
 *  ::set_configuration( vital::config_block_sptr config_in )
 *  {
 *  vital::config_block_sptr config = this->get_configuration();
 *
 *  config->merge_config( config_in );
 *  this->m_threshold = config->get_value< double >( "threshold" );
 *  m_keep_all_classes = config->get_value< bool >( "keep_all_classes" );
 *
 *  std::string list = config->get_value< std::string >( "keep_classes" );
 *  std::string parsed;
 *  std::stringstream ss( list );
 *
 *  while( std::getline( ss, parsed, ';' ) )
 *  {
 *   if( !parsed.empty() )
 *   {
 *     m_keep_classes.insert( parsed );
 *   }
 *  }
 *  }
 */

// ----------------------------------------------------------------------------
bool
class_probability_filter
::check_configuration( vital::config_block_sptr config ) const
{
  kwiver::vital::config_difference cd( this->get_configuration(), config );
  const auto key_list = cd.extra_keys();

  if( !key_list.empty() )
  {
    LOG_WARN(
      logger(),
      "Additional parameters found in config block that are not required or desired: "
        << kwiver::vital::join( key_list, ", " ) );
  }

  return true;
}

// ----------------------------------------------------------------------------
vital::detected_object_set_sptr
class_probability_filter
::filter( const vital::detected_object_set_sptr input_set ) const
{
  auto ret_set = std::make_shared< vital::detected_object_set >();

  // loop over all detections
  auto ie = input_set->cend();
  for( auto det = input_set->cbegin(); det != ie; ++det )
  {
    bool det_selected( false );
    auto out_dot = std::make_shared< vital::detected_object_type >();

    // Make sure that there is an associated DOT
    auto input_dot = ( *det )->type();
    if( !input_dot )
    {
      // This is unexpected
      LOG_WARN(
        logger(),
        "No detected_object_type associated with a detected object" );
      continue;
    }

    // Get list of class names that are above threshold
    auto selected_names = input_dot->class_names( parent.c_threshold );

    // Loop over all selected class names
    for( const std::string& a_name : selected_names )
    {
      if( parent.c_keep_all_classes ||
          parent.c_keep_all_classes.count( a_name ) )
      {
        // insert class-name/score into DOT
        out_dot->set_score( a_name, input_dot->score( a_name ) );
        LOG_TRACE(
          logger(),
          "Selecting class: " << a_name << "  score: " <<
            input_dot->score( a_name ) );
        det_selected = true;
      }
    } // end foreach class-name

    // It this detection has been selected, add it to output list
    // Clone input detection and replace DOT.
    // Add to returned set
    if( det_selected )
    {
      auto out_det = ( *det )->clone();
      out_det->set_type( out_dot );
      ret_set->add( out_det );
    }
  } // end foreach detection

  return ret_set;
} // class_probability_filter::filter

} // namespace core

} // namespace arrows

}         // end namespace
