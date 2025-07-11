// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <sprokit/processes/core/kwiver_processes_export.h>

#include <sprokit/pipeline/process_factory.h>
#include <vital/plugin_management/plugin_loader.h>

// -- list processes to register --
#include "associate_detections_to_tracks_process.h"
#include "compute_association_matrix_process.h"
#include "compute_homography_process.h"
#include "compute_stereo_depth_map_process.h"
#include "compute_track_descriptors_process.h"
#include "detect_features_if_keyframe_process.h"
#include "detect_features_process.h"
#include "close_loops_process.h"
#include "detected_object_filter_process.h"
#include "detected_object_input_process.h"
#include "detected_object_output_process.h"
#include "downsample_process.h"
#include "draw_detected_object_set_process.h"
#include "draw_tracks_process.h"
#include "extract_descriptors_process.h"
#include "frame_list_process.h"
#include "handle_descriptor_request_process.h"
#include "image_file_reader_process.h"
#include "image_filter_process.h"
#include "image_object_detector_process.h"
#include "image_writer_process.h"
#include "initialize_object_tracks_process.h"
#include "keyframe_selection_process.h"
#include "matcher_process.h"
#include "merge_detection_sets_process.h"
#include "perform_query_process.h"
#include "merge_images_process.h"
#include "print_config_process.h"
#include "detect_motion_process.h"
#include "read_descriptor_process.h"
#include "read_object_track_process.h"
#include "read_track_descriptor_process.h"
#include "refine_detections_process.h"
#include "serializer_process.h"
#include "deserializer_process.h"
#include "shift_detected_object_set_frames_process.h"
#include "split_image_process.h"
#include "stabilize_image_process.h"
#include "track_features_process.h"
#include "video_input_process.h"
#include "write_object_track_process.h"
#include "write_track_descriptor_process.h"

// ---------------------------------------------------------------------------------------
/*! \brief Regsiter processes
 *
 *
 */
extern "C"
KWIVER_PROCESSES_EXPORT
void
register_factories( kwiver::vital::plugin_loader& vpm )
{
  using namespace sprokit;
  using namespace kwiver;

  process_registrar reg( vpm, "kwiver_processes_core" );

  if ( reg.is_module_loaded() )
  {
    return;
  }

  reg.register_process< frame_list_process >();
  reg.register_process< stabilize_image_process >();
  reg.register_process< detect_features_process >();
  reg.register_process< extract_descriptors_process >();
  reg.register_process< matcher_process >();
  reg.register_process< compute_homography_process >();
  reg.register_process< compute_stereo_depth_map_process >();
  reg.register_process< draw_tracks_process >();
  reg.register_process< read_descriptor_process >();
  reg.register_process< refine_detections_process >();
  reg.register_process< image_object_detector_process >();
  reg.register_process< image_filter_process >();
  reg.register_process< image_writer_process >();
  reg.register_process< image_file_reader_process >();
  reg.register_process< detected_object_input_process >();
  reg.register_process< detected_object_output_process >();
  reg.register_process< detected_object_filter_process >();
  reg.register_process< downsample_process >();
  reg.register_process< video_input_process >();
  reg.register_process< draw_detected_object_set_process >();
  reg.register_process< split_image_process >();
  reg.register_process< merge_images_process >( process_registrar::no_test );
  reg.register_process< read_track_descriptor_process >();
  reg.register_process< write_track_descriptor_process >();
  reg.register_process< track_features_process >();
  reg.register_process< keyframe_selection_process >();
  reg.register_process< detect_features_if_keyframe_process >();
  reg.register_process< close_loops_process >();
  reg.register_process< read_object_track_process >();
  reg.register_process< print_config_process >( process_registrar::no_test );
  reg.register_process< write_object_track_process >();
  reg.register_process< associate_detections_to_tracks_process >();
  reg.register_process< compute_association_matrix_process >();
  reg.register_process< initialize_object_tracks_process >();
  reg.register_process< serializer_process >( process_registrar::no_test );
  reg.register_process< deserializer_process >( process_registrar::no_test );
  reg.register_process< merge_detection_sets_process >( process_registrar::no_test );
  reg.register_process< handle_descriptor_request_process >();
  reg.register_process< compute_track_descriptors_process >();
  reg.register_process< perform_query_process >();
  reg.register_process< detect_motion_process >();
  reg.register_process< shift_detected_object_set_frames_process >();

  reg.mark_module_as_loaded();
} // register_process
