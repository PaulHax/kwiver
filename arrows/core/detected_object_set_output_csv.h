// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Interface for detected_object_set_output_csv

#ifndef KWIVER_ARROWS_DETECTED_OBJECT_SET_OUTPUT_CSV_H
#define KWIVER_ARROWS_DETECTED_OBJECT_SET_OUTPUT_CSV_H

#include <arrows/core/kwiver_algo_core_export.h>

#include <vital/algo/algorithm.txx>
#include <vital/algo/detected_object_set_output.h>

namespace kwiver {

namespace arrows {

namespace core {

class KWIVER_ALGO_CORE_EXPORT detected_object_set_output_csv
  : public vital::algo::detected_object_set_output
{
public:
  // NOTE: Keep description in sync with detected_object_set_input_csv

  PLUGGABLE_IMPL(
    detected_object_set_output_csv,
    "Detected object set writer using CSV format.\n\n"
    " - 1: frame number\n"
    " - 2: file name\n"
    " - 3: TL-x\n"
    " - 4: TL-y\n"
    " - 5: BR-x\n"
    " - 6: BR-y\n"
    " - 7: confidence\n"
    " - 8,9: class-name, score"
    " (this pair may be omitted or may repeat any number of times)",
    PARAM_DEFAULT( delim, std::string, "csv delimeter", "," )
  )

  virtual ~detected_object_set_output_csv();

  virtual bool check_configuration( vital::config_block_sptr config ) const;

  virtual void write_set(
    const kwiver::vital::detected_object_set_sptr set,
    std::string const& image_name );

private:
  void initialize() override;

  class priv;
  KWIVER_UNIQUE_PTR( priv, d );
};

} // namespace core

} // namespace arrows

}     // end namespace

#endif // KWIVER_ARROWS_DETECTED_OBJECT_SET_OUTPUT_CSV_H
