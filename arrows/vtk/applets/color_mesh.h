// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#ifndef KWIVER_ARROWS_VTK_APPLETS_COLOR_MESH_H
#define KWIVER_ARROWS_VTK_APPLETS_COLOR_MESH_H

#include <arrows/vtk/applets/kwiver_algo_vtk_applets_export.h>
#include <vital/applets/kwiver_applet.h>

namespace kwiver {

namespace arrows {

namespace vtk {

class KWIVER_ALGO_VTK_APPLETS_EXPORT color_mesh
  : public kwiver::tools::kwiver_applet
{
public:
  virtual ~color_mesh() = default;

  PLUGGABLE_IMPL(
    color_mesh,
    "Color a mesh from a video and cameras" )
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
