// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include "timing_process_instrumentation.h"

#include <vital/util/enum_converter.h>
#include <vital/vital_config.h>

#include <fstream>
#include <iomanip>
#include <sprokit/pipeline/process.h>

namespace sprokit {

namespace {

enum timer_type
{
  timer_wall,
  timer_cpu,
};

ENUM_CONVERTER(
  timer_converter, timer_type,
  { "wall", timer_wall },
  { "cpu", timer_cpu } )

} // namespace

// ------------------------------------------------------------------
timing_process_instrumentation
::timing_process_instrumentation()
  : m_timer( std::make_shared< kwiver::vital::wall_timer >() ),
    m_output_file( 0 ),
    m_logger( kwiver::vital::get_logger(
      "sprokit.timing_process_instrumentation" ) )
{}

timing_process_instrumentation::
~timing_process_instrumentation()
{
  if( m_output_file )
  {
    // write out summary stats
    *m_output_file << m_step_stats;

    // Close file and clean up
    m_output_file->close();
    delete m_output_file;
    m_output_file = 0;
  }
}

// ------------------------------------------------------------------
void
timing_process_instrumentation
::start_init_processing( VITAL_UNUSED std::string const& data )
{
  m_timer->start();
}

// ------------------------------------------------------------------
void
timing_process_instrumentation
::stop_init_processing()
{
  // stop timer
  m_timer->stop();

  // write elapsed time
  write_interval( "init", m_timer->elapsed() );
}

// ------------------------------------------------------------------
void
timing_process_instrumentation
::start_finalize_processing( VITAL_UNUSED std::string const& data )
{
  m_timer->start();
}

// ------------------------------------------------------------------
void
timing_process_instrumentation
::stop_finalize_processing()
{
  // stop timer
  m_timer->stop();

  // write elapsed time
  write_interval( "finalize", m_timer->elapsed() );
}

// ------------------------------------------------------------------
void
timing_process_instrumentation
::start_reset_processing( VITAL_UNUSED std::string const& data )
{
  m_timer->start();
}

// ------------------------------------------------------------------
void
timing_process_instrumentation
::stop_reset_processing()
{
  m_timer->stop();
  write_interval( "reset", m_timer->elapsed() );
}

// ------------------------------------------------------------------
void
timing_process_instrumentation
::start_flush_processing( VITAL_UNUSED std::string const& data )
{
  m_timer->start();
}

// ------------------------------------------------------------------
void
timing_process_instrumentation
::stop_flush_processing()
{
  m_timer->stop();
  write_interval( "flush", m_timer->elapsed() );
}

// ------------------------------------------------------------------
void
timing_process_instrumentation
::start_step_processing( VITAL_UNUSED std::string const& data )
{
  m_timer->start();
}

// ------------------------------------------------------------------
void
timing_process_instrumentation
::stop_step_processing()
{
  m_timer->stop();
  write_interval( "step", m_timer->elapsed() );

  m_step_stats.add_datum( m_timer->elapsed() );
}

// ------------------------------------------------------------------
void
timing_process_instrumentation
::start_configure_processing( VITAL_UNUSED std::string const& data )
{
  m_timer->start();
}

// ------------------------------------------------------------------
void
timing_process_instrumentation
::stop_configure_processing()
{
  m_timer->stop();
  write_interval( "configure", m_timer->elapsed() );
}

// ------------------------------------------------------------------
void
timing_process_instrumentation
::start_reconfigure_processing( VITAL_UNUSED std::string const& data )
{
  m_timer->start();
}

// ------------------------------------------------------------------
void
timing_process_instrumentation
::stop_reconfigure_processing()
{
  m_timer->stop();
  write_interval( "reconfigure", m_timer->elapsed() );
}

// ----------------------------------------------------------------------------
void
timing_process_instrumentation
::configure( kwiver::vital::config_block_sptr const conf )
{
  // Starting with our generated vital::config_block to ensure that assumed
  // values are present
  // An alternative is to check for key presence before performing a get_value()
  // call.
  auto local_config = get_configuration();
  local_config->merge_config( conf );

  timer_type tt = local_config->get_enum_value< timer_converter >( "type" );
  std::string type_name;

  switch( tt )
  {
    case timer_type::timer_wall:
      m_timer = std::make_shared< kwiver::vital::wall_timer >();
      type_name = "wall_clock_duration";
      break;

    case timer_type::timer_cpu:
      m_timer = std::make_shared< kwiver::vital::cpu_timer >();
      type_name = "cpu_clock_duration";
      break;

    default:
      break;
  } // end switch

  // Get file name and create output file.
  std::string fname = local_config->get_value< std::string >( "output_file" );
  m_output_file = new std::ofstream( fname );
  if( !*m_output_file )
  {
    LOG_WARN(
      m_logger, "Unable to open output file \"" << fname
                                                << "\" for process " <<
        process()->name()
                                                << ". Disabling output." );
    delete m_output_file;
    m_output_file = 0; // disable output
    return;
  }

  *m_output_file << "#  method," << type_name << std::endl;
}

// ----------------------------------------------------------------------------
kwiver::vital::config_block_sptr
timing_process_instrumentation
::get_configuration() const
{
  auto conf = kwiver::vital::config_block::empty_config();

  conf->set_value(
    "type", "wall",
    "Type of timer to use. Allowable values are 'wall', and 'cpu'. "
    "Wall timer measures the elapsed time within the process method. "
    "Cpu timer measures the amount of cpu time used in that method. "
    "This may be different from the wall time if multiple cpu's are being used." );

  std::string process_name( "process" );
  if( process() )
  {
    process_name = process()->name();
  }

  conf->set_value(
    "output_file", process_name + "_timing.csv",
    "Name of the output file where the timing data is written." );

  return conf;
}

// ----------------------------------------------------------------------------
void
timing_process_instrumentation
::write_interval( const std::string& tag, double interval )
{
  if( m_output_file == 0 )
  {
    return;
  }

  *m_output_file << tag << "," << std::fixed
                 << interval << std::endl;
}

} // end namespace
