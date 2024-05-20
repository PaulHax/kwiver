# This file is part of KWIVER, and is distributed under the
# OSI-approved BSD 3-Clause License. See top-level LICENSE file or
# https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

from kwiver.vital import plugin_management
from kwiver.vital.algo import ImageIO, SplitImage
from kwiver.vital.types import types

from matplotlib import pyplot

from pathlib import Path


def how_to_part_01_images(image_file, sleep_time=2):

    # All algorithms are implemented/encapsulated in an arrow, and operate on
    # vital classes
    # There are various algorithms (arrows) that kwiver provides that you can use
    # to analyze imagry
    # In this example, while we will look at a few algorithms, this example
    # highlights the vital data types used by algorithms
    # These vital data types can then be used as inputs or outputs for
    # algorithms.
    # The vital data types are a sort of common 'glue' between dispart algorithms
    # allowing them to work together.

    # Image I/O algorithms are derived from the kwiver.vital.algo.ImageIO algorithm
    # interface

    # A key feature of the KWIVER architecture is the ability to dynamically load
    # available algorithms at runtime.
    # This ability allow you to write your application with a set of basic data
    # types and algorithm interfaces and
    # then dynamically replace or reconfigure algorithms at run time without
    # needing to recompile
    # New algorithms can be dropped on disk at and KWIVER can run them
    # The first thing to do is to tell kwiver to load up all it's plugins (which
    # includes all the algorithms)

    vpm = plugin_management.plugin_manager_instance()
    vpm.load_all_plugins()

    def display(image, title):
        """
        Displays a kwiver image with matplotlib.
        """
        fig, ax = pyplot.subplots()

        # Set the window title
        fig.canvas.manager.set_window_title(title)

        # Display the image
        # Convert image to a numpy array with asarray() API
        ax.imshow(image.asarray())
        ax.axis("off")
        pyplot.show(block=False)

        # Wait for sleep_time seconds
        pyplot.pause(sleep_time)

        pyplot.close(fig)

    ##################
    ## Image I/O ##
    ##################

    # The main ImageIo libraries used in KWIVER are the OpenCV and VXL libraries
    ocv_io = ImageIO.create_algorithm("ocv")
    vxl_io = ImageIO.create_algorithm("vxl")

    # The image_io interface is simple, and has a load and save method
    # These methods will operate on the vital object ImageContainer
    # The ImageContainer is intended to be a wrapper for image to facilitate
    # conversion between
    # various representations. It provides limited access to the underlying
    # data and is not intended for direct use in image processing algorithms.
    ocv_img = ocv_io.load(image_file)
    vxl_img = vxl_io.load(image_file)

    # Let's display the loaded images
    display(ocv_img, "Image loaded by OpenCV")
    display(vxl_img, "Image loaded by VXL")

    ##################
    ## Image Filter ##
    ##################

    # Currently, there is no arrow implementing image filtering
    # _filter = ImageFilter.create_algorithm( "<impl_name>" )

    #################
    ## Split Image ##
    #################

    # These algorithms split an image in half (left and right)
    ocv_split = SplitImage.create_algorithm("ocv")
    vxl_split = SplitImage.create_algorithm("vxl")

    ocv_imgs = ocv_split.split(ocv_img)
    vxl_imgs = ocv_split.split(vxl_img)

    # We can display the produced images
    for img in ocv_imgs:
        display(img, "OpenCV Split Image")
    for img in vxl_imgs:
        display(img, "VXL Split Image")


TEST_DATA_DIR = Path(__file__).parent  # / "../images/"


def test_how_to_part_01_images():
    image = str(TEST_DATA_DIR / ".." / "images" / "cat.jpg")
    how_to_part_01_images(image, 0.1)
