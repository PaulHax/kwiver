// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#ifndef KWIVER_ARROWS_MVG_APPLETS_INIT_CAMERAS_LANDMARKS_H
#define KWIVER_ARROWS_MVG_APPLETS_INIT_CAMERAS_LANDMARKS_H

#include <arrows/mvg/applets/kwiver_algo_mvg_applets_export.h>
#include <vital/applets/kwiver_applet.h>

namespace kwiver {

namespace arrows {

namespace mvg {

class KWIVER_ALGO_MVG_APPLETS_EXPORT init_cameras_landmarks
  : public kwiver::tools::kwiver_applet
{
public:
  virtual ~init_cameras_landmarks() = default;

  PLUGGABLE_IMPL(
    init_cameras_landmarks,
    "Estimate cameras and landmarks from a set of feature tracks" );

  virtual int run() override;
  virtual void add_command_options() override;

protected:
  void initialize() override;

private:
  class priv;
  KWIVER_UNIQUE_PTR( priv, d_ );
}; // end of class

} // namespace mvg

} // namespace arrows

}     // end namespace

#endif
