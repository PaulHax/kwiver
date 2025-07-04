// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#ifndef KWIVER_ARROWS_MVG_APPLETS_BUNDLE_ADJUST_TOOL_H
#define KWIVER_ARROWS_MVG_APPLETS_BUNDLE_ADJUST_TOOL_H

#include <arrows/mvg/applets/kwiver_algo_mvg_applets_export.h>
#include <vital/applets/kwiver_applet.h>

namespace kwiver {

namespace arrows {

namespace mvg {

class KWIVER_ALGO_MVG_APPLETS_EXPORT bundle_adjust_tool
  : public kwiver::tools::kwiver_applet
{
public:
  virtual ~bundle_adjust_tool() = default;

  PLUGGABLE_IMPL(
    bundle_adjust_tool,
    "Optimize cameras and landmarks via a bundle adjustment algorithm."
  );

  virtual int run() override;
  virtual void add_command_options() override;

protected:
  void initialize() override;

private:
  struct priv;
  KWIVER_UNIQUE_PTR( priv, d_ );
}; // bundle_adjust_tool

} // mvg

} // arrows

} // kwiver

#endif
