// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#ifndef KWIVER_TOOLS_TRANSCODE_H
#define KWIVER_TOOLS_TRANSCODE_H

#include <vital/applets/kwiver_applet.h>

namespace kwiver {

namespace arrows {

namespace core {

class transcode_applet : public tools::kwiver_applet
{
public:
  PLUGGABLE_IMPL(
    transcode_applet,
    "Transcode video.\n\n"
    "This program reads video from one format, "
    "then writes it to another format." );

  void add_command_options() override;

  int run() override;
};

} // namespace core

} // namespace arrows

} // namespace kwiver

#endif
