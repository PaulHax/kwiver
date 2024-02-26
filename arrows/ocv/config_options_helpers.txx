// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#ifndef CONFIG_OPTIONS_HELPERS_TXX
#define CONFIG_OPTIONS_HELPERS_TXX

#include <memory>
#include <type_traits>
#include <vital/config/config_block.h>

#include <arrows/ocv/resection_camera_options.h>

namespace kwiver::vital {

// ----------------------------------------------------------------------------
// specializations of set/get_config_helper for resection_camera_options
// for base implementations see config_helpers.txx

/// A helper for populating \p key in \p config based on the configuration of
/// the solver_options given in \p value.
template < typename ValueType,
  typename std::enable_if_t< detail::is_shared_ptr< ValueType >::value,
    bool > = true,
  typename std::enable_if_t< std::is_base_of_v< kwiver::arrows::ocv::resection_camera_options, typename ValueType::element_type >, bool > = true >
void
set_config_helper(
  vital::config_block_sptr config, const std::string& key,
  const ValueType& value )
{
  if( value )
  {
    value->get_configuration( config );
  }
}

/// A helper for retrieving a value from a config block. This specialization is
/// for keys that correspond to nested resection_camera_options.
template < typename ValueType,
  typename std::enable_if_t< detail::is_shared_ptr< ValueType >::value,
    bool > = true,
  typename std::enable_if_t< std::is_base_of_v< kwiver::arrows::ocv::resection_camera_options, typename ValueType::element_type >, bool > = true >
ValueType
get_config_helper(
  vital::config_block_sptr config,
  config_block_key_t const& key )
{
  ValueType camera_options = std::make_shared< kwiver::arrows::ocv::resection_camera_options >();
  camera_options->set_configuration( config );
  return camera_options;
}

} // namespace kwiver::arrows::ocv

#endif
