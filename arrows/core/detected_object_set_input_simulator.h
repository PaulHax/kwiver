// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Interface for detected_object_set_input_simulator

#ifndef KWIVER_ARROWS_CORE_DETECTED_OBJECT_SET_INPUT_SIMULATOR_H
#define KWIVER_ARROWS_CORE_DETECTED_OBJECT_SET_INPUT_SIMULATOR_H

#include <arrows/core/kwiver_algo_core_export.h>

#include <vital/algo/algorithm.txx>
#include <vital/algo/detected_object_set_input.h>

namespace kwiver {

namespace arrows {

namespace core {

class KWIVER_ALGO_CORE_EXPORT detected_object_set_input_simulator
  : public vital::algo::detected_object_set_input
{
public:
  // NOTE: Keep description in sync with detected_object_set_output_simulator
  PLUGGABLE_IMPL(
    detected_object_set_input_simulator,
    "Detected object set reader using SIMULATOR format.\n\n"
    "Detection are generated algorithmicly.",
    PARAM_DEFAULT(
      center_x, double,
      "Bounding box center x coordinate.",
      100.0 ),
    PARAM_DEFAULT(
      center_y, double,
      "Bounding box center y coordinate.",
      100.0 ),
    PARAM_DEFAULT(
      height, double,
      "Bounding box height.",
      200.0 ),
    PARAM_DEFAULT(
      width, double,
      "Bounding box width.",
      200.0 ),
    PARAM_DEFAULT(
      dx, double,
      "Bounding box x translation per frame.",
      0.0 ),
    PARAM_DEFAULT(
      dy, double,
      "Bounding box y translation per frame.",
      0.0 ),
    PARAM_DEFAULT(
      max_sets, int,
      "Number of detection sets to generate.",
      10 ),
    PARAM_DEFAULT(
      set_size, int,
      "Number of detection in a set.",
      4 ),
    PARAM_DEFAULT(
      detection_class, std::string,
      "Label for detection detected object type",
      "detection" ),
    PARAM(
      image_name, std::string,
      "Image name to return with each detection set" )
  )

  virtual ~detected_object_set_input_simulator();

  void initialize() override;
  virtual bool check_configuration( vital::config_block_sptr config ) const;

  virtual void open( std::string const& filename );
  virtual bool read_set(
    kwiver::vital::detected_object_set_sptr& set,
    std::string& image_name );

  class priv;
  KWIVER_UNIQUE_PTR( priv, d );
};

} // namespace core

} // namespace arrows

}     // end namespace

#endif // KWIVER_ARROWS_CORE_DETECTED_OBJECT_SET_INPUT_SIMULATOR_H
