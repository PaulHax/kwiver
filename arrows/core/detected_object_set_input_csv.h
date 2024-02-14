// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Interface for detected_object_set_input_csv

#ifndef KWIVER_ARROWS_CORE_DETECTED_OBJECT_SET_INPUT_CSV_H
#define KWIVER_ARROWS_CORE_DETECTED_OBJECT_SET_INPUT_CSV_H

#include <arrows/core/kwiver_algo_core_export.h>

#include <vital/algo/algorithm.h> // attempt to remove later
#include <vital/algo/algorithm.txx> // attempt to remove later
#include <vital/algo/detected_object_set_input.h>

namespace kwiver {

namespace arrows {

namespace core {

class KWIVER_ALGO_CORE_EXPORT detected_object_set_input_csv
  : public vital::algo::detected_object_set_input
{
public:
  PLUGGABLE_IMPL(
    detected_object_set_input_csv,
    "Detected object set reader using CSV format.\n\n"
    " - 1: frame number\n"
    " - 2: file name\n"
    " - 3: TL-x\n"
    " - 4: TL-y\n"
    " - 5: BR-x\n"
    " - 6: BR-y\n"
    " - 7: confidence\n"
    " - 8,9: class-name, score"
    " (this pair may be omitted or may repeat any number of times)",
    PARAM_DEFAULT( m_delim, std::string, "no description", "," )
  )

  virtual ~detected_object_set_input_csv();

  virtual bool read_set(
    kwiver::vital::detected_object_set_sptr& set,
    std::string& image_name );

private:
  virtual void new_stream();

  class priv;
  KWIVER_UNIQUE_PTR( priv, d );
};

} // namespace core

} // namespace arrows

}     // end namespace

#endif // KWIVER_ARROWS_CORE_DETECTED_OBJECT_SET_INPUT_CSV_H
