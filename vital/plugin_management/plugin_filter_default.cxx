// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include "plugin_factory.h"
#include "plugin_filter_default.h"
#include "plugin_loader.h"

#include <vital/exceptions/plugin.h>
#include <vital/util/demangle.h>

namespace kwiver {

namespace vital {

// ----------------------------------------------------------------------------
/// @brief Detault add_factory filter
///
/// This is the default implementation for the add_factory hook. This
/// checks to see if the plugin is already registered. If it is, then
/// an exception is thrown.
///
/// The signature of a plugin consists of interface-type,
/// concrete-type, and plugin-name.
///
/// Note that derived classes can override this hook to give different
/// behaviour.
///
/// @param fact Factory object handle
///
/// @return \b true if factory is to be added; \b false if factory
/// should not be added.
///
/// @throws plugin_already_exists if plugin is already registered
bool
plugin_filter_default
::add_factory( plugin_factory_handle_t fact ) const
{
  std::string file_name;
  fact->get_attribute( plugin_factory::PLUGIN_FILE_NAME, file_name );

  std::string interface_type;
  fact->get_attribute( plugin_factory::INTERFACE_TYPE, interface_type );

  std::string concrete_type;
  fact->get_attribute( plugin_factory::CONCRETE_TYPE, concrete_type );

  std::string new_name;
  fact->get_attribute( plugin_factory::PLUGIN_NAME, new_name );

  auto plugin_map = m_loader->get_plugin_map();
  auto fact_list = plugin_map[ interface_type ];

  // Make sure factory is not already in the list.
  // Check the two types and name as a signature.
  if( fact_list.size() > 0 )
  {
    for( auto const& afact : fact_list )
    {
      std::string interf;
      afact->get_attribute( plugin_factory::INTERFACE_TYPE, interf );

      std::string inst;
      afact->get_attribute( plugin_factory::CONCRETE_TYPE, inst );

      std::string name;
      afact->get_attribute( plugin_factory::PLUGIN_NAME, name );

      if( ( interface_type == interf ) &&
          ( concrete_type == inst ) &&
          ( new_name == name ) )
      {
        std::string old_file;
        afact->get_attribute( plugin_factory::PLUGIN_FILE_NAME, old_file );

        std::stringstream str;
        str << "Factory for \"" << demangle( interface_type ) << "\" : \""
            << demangle( concrete_type ) << "\" already has been registered by "
            << old_file << ".  This factory from "
            << file_name << " will not be registered.";

        VITAL_THROW( plugin_already_exists, str.str() );
      }
    } // end foreach
  }

  return true;
}

} // namespace vital

}   // end namespace
