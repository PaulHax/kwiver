// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#ifndef KWIVER_ARROWS_VTK_APPLETS_ESTIMATE_DEPTH_H
#define KWIVER_ARROWS_VTK_APPLETS_ESTIMATE_DEPTH_H

#include <vital/types/camera_map.h>
#include <vital/types/landmark_map.h>

#include <arrows/vtk/applets/kwiver_algo_vtk_applets_export.h>
#include <vital/applets/kwiver_applet.h>

namespace kwiver {

namespace arrows {

namespace vtk {

class KWIVER_ALGO_VTK_APPLETS_EXPORT estimate_depth
  : public kwiver::tools::kwiver_applet
{
public:
  virtual ~estimate_depth() = default;

  PLUGGABLE_IMPL(
    estimate_depth,
    "Depth estimation utility" );

  int run() override;
  void add_command_options() override;

protected:
  void initialize() override;

private:
  class priv;
  KWIVER_UNIQUE_PTR( priv, d );
}; // end of class

} // namespace vtk

} // namespace arrows

}     // end namespace

#endif
