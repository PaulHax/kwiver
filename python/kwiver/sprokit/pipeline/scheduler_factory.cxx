// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/**
 * \file scheduler_factory.cxx
 *
 * \brief Python bindings for \link sprokit::scheduler_factory\endlink.
 */

#include <sprokit/pipeline/pipeline.h>
#include <sprokit/pipeline/scheduler.h>
#include <sprokit/pipeline/scheduler_factory.h>
#include <sprokit/pipeline/scheduler_registry_exception.h>

#include <python/kwiver/vital/util/python_exceptions.h>

#include <vital/plugin_management/plugin_manager.h>

#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>

using namespace pybind11;

namespace kwiver {

namespace sprokit {

namespace python {

static void register_scheduler(
  ::sprokit::scheduler::type_t const& type,
  ::sprokit::scheduler::description_t const& desc,
  object obj );
static bool is_scheduler_loaded( const std::string& name );
static void mark_scheduler_loaded( const std::string& name );
static std::string get_description( const std::string& name );
static std::vector< std::string > scheduler_names();
static std::string get_default_type();

// ============================================================================
typedef std::function< pybind11::object ( ::sprokit::pipeline_t const& pipe,
                                          kwiver::vital::config_block_sptr const& config ) > py_scheduler_factory_func_t;

class python_scheduler_factory
  : public ::sprokit::scheduler_factory
{
public:
  python_scheduler_factory(
    const std::string& type,
    const std::string& itype,
    py_scheduler_factory_func_t factory );

  virtual ~python_scheduler_factory() = default;

  virtual ::sprokit::scheduler_t create_object(
    ::sprokit::pipeline_t const& pipe,
    kwiver::vital::config_block_sptr const& config );

private:
  py_scheduler_factory_func_t m_factory;
};

// ------------------------------------------------------------------
python_scheduler_factory
::python_scheduler_factory(
  const std::string& type,
  const std::string& itype,
  py_scheduler_factory_func_t factory )
  : scheduler_factory( type, itype ),
    m_factory( factory )
{
  this->add_attribute( CONCRETE_TYPE, type )
    .add_attribute( PLUGIN_FACTORY_TYPE, typeid( *this ).name() )
    .add_attribute( PLUGIN_CATEGORY, "scheduler" );
}

// ----------------------------------------------------------------------------
::sprokit::scheduler_t
python_scheduler_factory
::create_object(
  ::sprokit::pipeline_t const& pipe,
  kwiver::vital::config_block_sptr const& config )
{
  pybind11::gil_scoped_acquire acquire;
  ( void ) acquire;

  // Call sprokit factory function.
  pybind11::object obj = m_factory( pipe, config );
  obj.inc_ref();
  ::sprokit::scheduler_t schd_ptr = obj.cast< ::sprokit::scheduler_t >();
  return schd_ptr;
}

} // namespace python

} // namespace sprokit

} // namespace kwiver

using namespace kwiver::sprokit::python;

// ==================================================================
PYBIND11_MODULE( scheduler_factory, m )
{
  bind_vector< std::vector< std::string > >( m, "string_vector" );

  // Define unbound functions.
  m.def(
    "add_scheduler", &register_scheduler,
    call_guard< pybind11::gil_scoped_release >(),
    arg( "type" ), arg( "description" ), arg( "ctor" ),
    "Registers a function which creates a scheduler of the given type.",
    return_value_policy::reference_internal );

  m.def(
    "create_scheduler", &sprokit::create_scheduler,
    call_guard< pybind11::gil_scoped_release >(),
    arg( "type" ), arg( "pipeline" ),
    arg( "config" ) = kwiver::vital::config_block::empty_config(),
    "Creates a new scheduler of the given type." );

  m.def(
    "is_scheduler_module_loaded", &is_scheduler_loaded,
    call_guard< pybind11::gil_scoped_release >(),
    ( arg( "module" ) ),
    "Returns True if the module has already been loaded, False otherwise." );

  m.def(
    "mark_scheduler_module_as_loaded", &mark_scheduler_loaded,
    call_guard< pybind11::gil_scoped_release >(),
    ( arg( "module" ) ),
    "Marks a module as loaded." );

  m.def(
    "types", &scheduler_names, call_guard< pybind11::gil_scoped_release >(),
    "A list of known scheduler types." );

  m.def(
    "description", &get_description,
    call_guard< pybind11::gil_scoped_release >(),
    ( arg( "type" ) ),
    "The description for the given scheduler type." );

  m.def(
    "default_type", &get_default_type,
    call_guard< pybind11::gil_scoped_release >(),
    "The default scheduler type." );

  m.attr( "Scheduler" ) =
    m.import( "kwiver.sprokit.pipeline.scheduler" ).attr( "PythonScheduler" );
}

namespace kwiver {

namespace sprokit {

namespace python {

class python_scheduler_wrapper
{
public:
  python_scheduler_wrapper( object obj );
  ~python_scheduler_wrapper();

  object operator()(
    ::sprokit::pipeline_t const& pipeline,
    kwiver::vital::config_block_sptr const& config );

private:
  object const m_obj;
};

void
register_scheduler(
  ::sprokit::scheduler::type_t const& type,
  ::sprokit::scheduler::description_t const& desc,
  object obj )
{
  python_scheduler_wrapper const wrap( obj );

  kwiver::vital::plugin_manager& vpm =
    kwiver::vital::plugin_manager::instance();
  auto fact = vpm.add_factory(
    new python_scheduler_factory(
      type,
      typeid( ::sprokit::scheduler ).name(),
      wrap ) );

  fact->add_attribute( kwiver::vital::plugin_factory::PLUGIN_NAME, type )
    .add_attribute(
    kwiver::vital::plugin_factory::PLUGIN_MODULE_NAME,
    "python-runtime" )
    .add_attribute( kwiver::vital::plugin_factory::PLUGIN_DESCRIPTION, desc );
}

// ------------------------------------------------------------------
bool
is_scheduler_loaded( const std::string& name )
{
  kwiver::vital::plugin_manager& vpm =
    kwiver::vital::plugin_manager::instance();
  return vpm.is_module_loaded( name );
}

// ------------------------------------------------------------------
void
mark_scheduler_loaded( const std::string& name )
{
  kwiver::vital::plugin_manager& vpm =
    kwiver::vital::plugin_manager::instance();
  vpm.mark_module_as_loaded( name );
}

// ------------------------------------------------------------------
std::string
get_description( const std::string& type )
{
  kwiver::vital::plugin_factory_handle_t a_fact;

  try
  {
    typedef kwiver::vital::implementation_factory_by_name< ::sprokit::scheduler > proc_factory;

    proc_factory ifact;

    VITAL_PYTHON_TRANSLATE_EXCEPTION(
      a_fact = ifact.find_factory( type );
    )
  }
  catch( const std::exception& e )
  {
    typedef kwiver::vital::implementation_factory_by_name< object > proc_factory;

    proc_factory ifact;

    VITAL_PYTHON_TRANSLATE_EXCEPTION(
      a_fact = ifact.find_factory( type );
    )
  }

  std::string buf = "-- Not Set --";
  a_fact->get_attribute(
    kwiver::vital::plugin_factory::PLUGIN_DESCRIPTION,
    buf );

  return buf;
}

// ------------------------------------------------------------------
std::vector< std::string >
scheduler_names()
{
  std::vector< std::string > name_list;

  kwiver::vital::plugin_manager& vpm =
    kwiver::vital::plugin_manager::instance();
  auto fact_list = vpm.get_factories< ::sprokit::scheduler >();
  for( auto fact : fact_list )
  {
    std::string buf;
    if( fact->get_attribute( kwiver::vital::plugin_factory::PLUGIN_NAME, buf ) )
    {
      name_list.push_back( buf );
    }
  } // end foreach

  return name_list;
}

// ------------------------------------------------------------------
std::string
get_default_type()
{
  return ::sprokit::scheduler_factory::default_type;
}

// ------------------------------------------------------------------
python_scheduler_wrapper
::python_scheduler_wrapper( object obj )
  : m_obj( obj )
{}

python_scheduler_wrapper
::~python_scheduler_wrapper()
{}

// ------------------------------------------------------------------
object
python_scheduler_wrapper
::operator()(
  ::sprokit::pipeline_t const& pipeline,
  kwiver::vital::config_block_sptr const& config )
{
  pybind11::gil_scoped_acquire acquire;
  ( void ) acquire;
  return m_obj( pipeline, config );
}

} // namespace python

} // namespace sprokit

} // namespace kwiver
