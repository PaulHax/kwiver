// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#ifndef KWIVER_ARROWS_MVG_APPLETS_TRACK_FEATURES_H
#define KWIVER_ARROWS_MVG_APPLETS_TRACK_FEATURES_H

#include <arrows/mvg/applets/kwiver_algo_mvg_applets_export.h>
#include <vital/applets/kwiver_applet.h>

namespace kwiver {

namespace arrows {

namespace mvg {

class KWIVER_ALGO_MVG_APPLETS_EXPORT track_features
  : public kwiver::tools::kwiver_applet
{
public:
  virtual ~track_features() = default;

  PLUGGABLE_IMPL(
    track_features,
    "Feature tracking utility" );

  int run() override;
  void add_command_options() override;

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
