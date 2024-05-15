# This file is part of KWIVER, and is distributed under the
# OSI-approved BSD 3-Clause License. See top-level LICENSE file or
# https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

from kwiver.vital import plugin_management
from kwiver.vital.algo import (
    DetectedObjectSetInput,
    DetectedObjectSetOutput,
    DrawDetectedObjectSet,
    ImageIO,
    ImageObjectDetector,
)
from kwiver.vital.types import (
    types,
    BoundingBoxD,
    DetectedObject,
    DetectedObjectSet,
    DetectedObjectType,
)

import sys

import os

from pathlib import Path


def how_to_part_02_detections(data_dir, out_dir):
    # Initialize KWIVER and load up all plugins
    vpm = plugin_management.plugin_manager_instance()
    vpm.load_all_plugins()

    # Many vision algorithms are used to detect and identify items in an image.
    # Detectors are any class that implements the
    # kwiver.vital.algo.ImageObjectDetector interface
    # In this example we will explore the detection data types.

    # In the following section we will create dummy data in the data types in
    # lieu of running a detection algorithm

    # First, Load an image (see how_to_part_01_images)
    ocv_io = ImageIO.create_algorithm("ocv")
    ocv_img = ocv_io.load(data_dir + "/soda_circles.jpg")

    # Now let's run a detection algorithm that comes with kwiver
    detector = ImageObjectDetector.create_algorithm("hough_circle")
    hough_detections = detector.detect(ocv_img)

    # We can take this detection set and create a new image with the detections
    # overlaid on the image
    drawer = DrawDetectedObjectSet.create_algorithm("ocv")

    hough_img = drawer.draw(hough_detections, ocv_img)

    # Note: The C++ example (in examples/cpp/how_to_part_02_detection.cxx),
    # we display the images using OpenCV. We are currently lacking the function
    # that converts an vital image to an OpenCV one (vital_to_ocv).
    # Instead, we will save the results from the algorithms on disk.

    ocv_io.save(out_dir + "/soda_circles_hough_ocv.jpg", hough_img)
    assert os.path.exists(out_dir + "/soda_circles_hough_ocv.jpg")

    # Next, let's look at the detection data structures and we can make them

    # General detection data is defined by the detected_object class
    # Detectors will take in an image and return a detected_object_set_sptr
    # object
    # A detected_object_set_sptr is comprised of the following data:

    # A bounding box
    # bounding_box_d is a double based box where the top left and bottom right
    # corners are specificed as TODO pixel index?
    # The top left corner is the anchor. A bounding_box_i is interger based to
    # associate corners to pixels in the image
    bbox1 = BoundingBoxD(
        ocv_img.width() * 0.25,
        ocv_img.height() * 0.25,
        ocv_img.width() * 0.75,
        ocv_img.height() * 0.75,
    )
    # The confidence value is the confidence associated with the detection.
    # It should be a probability (0..1) that the detector is sure that it has
    # identified what it is supposed to find.
    confidence1 = 1.0

    # A Classification
    # The DetectedObjectType is created by a classifier which is sometimes part
    # of the detector.
    # It is a group of name / value pairs.The name being the name of the class.
    # The score is the probability that the object is that class.
    # It is optional and not required for a detected object although most
    # examples provide one just to be complete.

    type1 = DetectedObjectType()
    # This can have multiple entries / scores
    type1.set_score("car", 0.03)
    type1.set_score("fish", 0.52)
    type1.set_score("flag pole", 0.23)

    # Put it all together to make a detection
    detection1 = DetectedObject(bbox1, confidence1, type1)
    detection1.detector_name = "center"

    # Let's add a few more detections to our detection set and write it out in
    # various formats
    bbox2 = BoundingBoxD(
        ocv_img.width() * 0.05,
        ocv_img.height() * 0.05,
        ocv_img.width() * 0.55,
        ocv_img.height() * 0.55,
    )
    confidence2 = 0.50
    type2 = DetectedObjectType()
    type2.set_score("car", 0.04)
    type2.set_score("fish", 0.12)
    type2.set_score("flag pole", 0.67)

    detection2 = DetectedObject(bbox2, confidence2, type2)
    detection2.detector_name = "upper left"

    bbox3 = BoundingBoxD(
        ocv_img.width() * 0.45,
        ocv_img.height() * 0.45,
        ocv_img.width() * 0.95,
        ocv_img.height() * 0.95,
    )
    confidence3 = 0.75
    type3 = DetectedObjectType()
    type3.set_score("car", 0.22)
    type3.set_score("fish", 0.08)
    type2.set_score("flag pole", 0.07)

    detection3 = DetectedObject(bbox3, confidence3, type3)
    detection3.detector_name = "lower right"

    # Group multiple detections for an image in a set object
    detections = DetectedObjectSet()
    detections.add(detection1)
    detections.add(detection2)
    detections.add(detection3)

    img_detections = drawer.draw(detections, ocv_img)

    # Note: The C++ example (in examples/cpp/how_to_part_02_detection.cxx),
    # we display the images using OpenCV. We are currently lacking the function
    # that converts an vital image to an OpenCV one (vital_to_ocv).
    # Instead, we will save the results from the algorithms on disk.

    ocv_io.save(out_dir + "/soda_circles_detections_ocv.jpg", img_detections)
    assert os.path.exists(out_dir + "/soda_circles_detections_ocv.jpg")

    kpf_writer = DetectedObjectSetOutput.create_algorithm("kpf_output")
    kpf_reader = DetectedObjectSetInput.create_algorithm("kpf_input")
    if not kpf_writer:
        sys.stderr.write(
            "Make sure you have built the kpf arrow, which requires fletch to have yaml"
        )
    else:
        kpf_writer.open("detected_object_set.kpf")
        kpf_writer.write_set(detections, "")

        # Now let's read the kpf data back in
        kpf_reader.open("detected_object_set.kpf")
        kpf_detections, image_name = kpf_reader.read_set()

        for det in kpf_detections:
            bbox = BoundingBoxD(det.bounding_box())

            ss = (
                "detector_name "
                + det.detector_name()
                + "\n"
                + "bounding box :x1("
                + str(bbox.min_x())
                + ") y1("
                + str(bbox.min_y())
                + ") x2("
                + str(bbox.max_x())
                + ") y2("
                + str(bbox.max_y())
                + ") \n"
                + "confidence : "
                + str(det.confidence())
                + "\n"
                + "classifications : \n"
            )
            for t in det.type():
                ss = ss + "\t type : " + str(t.first) + " " + str(t.second)
            print(ss)


TEST_DATA_DIR = Path(__file__).parent


def test_how_to_part_01_images():
    path = str(TEST_DATA_DIR / ".." / "images")
    how_to_part_02_detections(path, "Testing/Temporary")
