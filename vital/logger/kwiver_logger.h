// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#ifndef KWIVER_KWIVER_LOGGER_H_
#define KWIVER_KWIVER_LOGGER_H_

#include "location_info.h"
#include <vital/logger/vital_logger_export.h>

#include <cstdlib>
#include <functional>
#include <memory>
#include <sstream>

#include <vital/noncopyable.h>

namespace kwiver {

namespace vital {

namespace logger_ns {

class kwiver_logger_factory;

} // namespace logger_ns

// ----------------------------------------------------------------------------
/// @brief kwiver logger interface definition
///
/// This class is the abstract base class for all loggers. It provides
/// the interface to the application so it can generate log messages.
///
/// A new logger is created for each named logger category. The
/// concrete implementation determines how the category name is used.
class VITAL_LOGGER_EXPORT kwiver_logger
  : public std::enable_shared_from_this< kwiver_logger >,
    private kwiver::vital::noncopyable
{
public:
  enum log_level_t
  {
    LEVEL_NONE = 1,
    LEVEL_TRACE,
    LEVEL_DEBUG,
    LEVEL_INFO,
    LEVEL_WARN,
    LEVEL_ERROR,
    LEVEL_FATAL,
  };

  virtual ~kwiver_logger();

// Check to see if level is enabled
  virtual bool is_fatal_enabled() const = 0;
  virtual bool is_error_enabled() const = 0;
  virtual bool is_warn_enabled()  const = 0;
  virtual bool is_info_enabled()  const = 0;
  virtual bool is_debug_enabled() const = 0;
  virtual bool is_trace_enabled() const = 0;

  virtual void set_level( log_level_t lev ) = 0;
  virtual log_level_t get_level() const = 0;

/// Type alias for the callback function signature
  using callback_t =
    std::function< void ( log_level_t, std::string const& name,
                          std::string const& msg,
                          logger_ns::location_info const& loc ) >;

/// Set a callback to be called on logging events for this logger instance
  void set_local_callback( callback_t cb );

/// Set a callback to be called on logging events for all logger instances
  static void set_global_callback( callback_t cb );

/// @brief Get logger name.
  std::string get_name() const;

/// @brief Log a message string with the FATAL level.
///
/// This method first checks if this logger has <code>FATAL</code>
/// enabled by comparing the level of this logger with the FATAL
/// level. If this logger has <code>FATAL</code> enabled, it proceeds
/// to format and create a log message using the specified message.
///
/// @param msg the message string to log.
  virtual void log_fatal( std::string const& msg ) = 0;

/// @brief Log a message string with the FATAL level.
///
/// This method first checks if this logger has <code>FATAL</code>
/// enabled by comparing the level of this logger with the FATAL
/// level. If this logger has <code>FATAL</code> enabled, it proceeds
/// to format and create a log message using the specified message
/// and logging location.
///
/// @param msg the message string to log.
/// @param location location of source of logging request.
  virtual void log_fatal(
    std::string const& msg,
    logger_ns::location_info const& location ) = 0;

/// @brief Log a message string with the ERROR level.
///
/// This method first checks if this logger has <code>ERROR</code>
/// enabled by comparing the level of this logger with the ERROR
/// level. If this logger has <code>ERROR</code> enabled, it proceeds
/// to format and create a log message using the specified message.
///
/// @param msg the message string to log.
  virtual void log_error( std::string const& msg ) = 0;

/// @brief Log a message string with the ERROR level.
///
/// This method first checks if this logger has <code>ERROR</code>
/// enabled by comparing the level of this logger with the ERROR
/// level. If this logger has <code>ERROR</code> enabled, it proceeds
/// to format and create a log message using the specified message
/// and logging location.
///
/// @param msg the message string to log.
/// @param location location of source of logging request.
  virtual void log_error(
    std::string const& msg,
    logger_ns::location_info const& location ) = 0;

/// @brief Log a message string with the WARN level.
///
/// This method first checks if this logger has <code>WARN</code>
/// enabled by comparing the level of this logger with the WARN
/// level. If this logger has <code>WARN</code> enabled, it proceeds
/// to format and create a log message using the specified message.
///
/// @param msg the message string to log.
  virtual void log_warn( std::string const& msg ) = 0;

/// @brief Log a message string with the WARN level.
///
/// This method first checks if this logger has <code>WARN</code>
/// enabled by comparing the level of this logger with the WARN
/// level. If this logger has <code>WARN</code> enabled, it proceeds
/// to format and create a log message using the specified message
/// and logging location.
///
/// @param msg the message string to log.
/// @param location location of source of logging request.
  virtual void log_warn(
    std::string const& msg,
    logger_ns::location_info const& location ) = 0;

/// @brief Log a message string with the INFO level.
///
/// This method first checks if this logger has <code>INFO</code>
/// enabled by comparing the level of this logger with the INFO
/// level. If this logger has <code>INFO</code> enabled, it proceeds
/// to format and create a log message using the specified message.
///
/// @param msg the message string to log.
  virtual void log_info( std::string const& msg ) = 0;

/// @brief Log a message string with the INFO level.
///
/// This method first checks if this logger has <code>INFO</code>
/// enabled by comparing the level of this logger with the INFO
/// level. If this logger has <code>INFO</code> enabled, it proceeds
/// to format and create a log message using the specified message
/// and logging location.
///
/// @param msg the message string to log.
/// @param location location of source of logging request.
  virtual void log_info(
    std::string const& msg,
    logger_ns::location_info const& location ) = 0;

/// @brief Log a message string with the DEBUG level.
///
/// This method first checks if this logger has <code>DEBUG</code>
/// enabled by comparing the level of this logger with the DEBUG
/// level. If this logger has <code>DEBUG</code> enabled, it proceeds
/// to format and create a log message using the specified message.
///
/// @param msg the message string to log.
  virtual void log_debug( std::string const& msg ) = 0;

/// @brief Log a message string with the DEBUG level.
///
/// This method first checks if this logger has <code>DEBUG</code>
/// enabled by comparing the level of this logger with the DEBUG
/// level. If this logger has <code>DEBUG</code> enabled, it proceeds
/// to format and create a log message using the specified message
/// and logging location.
///
/// @param msg the message string to log.
/// @param location location of source of logging request.
  virtual void log_debug(
    std::string const& msg,
    logger_ns::location_info const& location ) = 0;

/// @brief Log a message string with the TRACE level.
///
/// This method first checks if this logger has <code>TRACE</code>
/// enabled by comparing the level of this logger with the TRACE
/// level. If this logger has <code>TRACE</code> enabled, it proceeds
/// to format and create a log message using the specified message.
///
/// @param msg the message string to log.
  virtual void log_trace( std::string const& msg ) = 0;

/// @brief Log a message string with the TRACE level.
///
/// This method first checks if this logger has <code>TRACE</code>
/// enabled by comparing the level of this logger with the TRACE
/// level. If this logger has <code>TRACE</code> enabled, it proceeds
/// to format and create a log message using the specified message
/// and logging location.
///
/// @param msg the message string to log.
/// @param location location of source of logging request.
  virtual void log_trace(
    std::string const& msg,
    logger_ns::location_info const& location ) = 0;

/// @brief Log a message string with specified level.
///
/// This method first checks if this logger has the specified enabled
/// by comparing the level of this logger with the churrent logger
/// level. If this logger has this level enabled, it proceeds to
/// format and create a log message using the specified message.
///
/// @param msg the message string to log.
  virtual void log_message( log_level_t level, std::string const& msg ) = 0;

/// @brief Log a message string with specified level.
///
/// This method first checks if this logger has the specified enabled
/// by comparing the level of this logger with the churrent logger
/// level. If this logger has this level enabled, it proceeds to
/// format and create a log message using the specified message and
/// location.
///
/// @param msg the message string to log.
/// @param location location of source of logging request.
  virtual void log_message(
    log_level_t level, std::string const& msg,
    logger_ns::location_info const& location ) = 0;

/// @brief Convert level code to string.
///
/// @param lev level value to convert
  static char const* get_level_string( kwiver_logger::log_level_t lev );

/// @brief Get name of logger factory / back-end provider
///
/// This method returns the name of the logger factory that created
/// this logger.
///
/// @return Name of logger factory.
  std::string const& get_factory_name() const;

protected:
/// @brief Constructor for logger object
///
/// A new logger object is constructed for the specified category.
///
/// @param fact Pointer to logger factory
/// @param name Name of logger to create
  kwiver_logger(
    logger_ns::kwiver_logger_factory* fact,
    std::string const& name );

/// @brief Call the registered callback functions, if any
  void do_callback(
    log_level_t level, std::string const& msg,
    logger_ns::location_info const& location ) const;

private:
  class impl;

  const std::unique_ptr< impl > m_impl;
}; // end class logger

/// @brief Handle for kwiver logger objects.
typedef std::shared_ptr< kwiver_logger > logger_handle_t;

} // namespace vital

}   // end namespace

#endif
