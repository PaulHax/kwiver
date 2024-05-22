# This file is part of KWIVER, and is distributed under the
# OSI-approved BSD 3-Clause License. See top-level LICENSE file or
# https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

import argparse
import sys
import os
from pathlib import Path

from kwiver.vital import plugin_management
from kwiver.vital.types import Timestamp, Metadata
from kwiver.vital.config import read_config_file
from kwiver.vital.algo import VideoInput
from kwiver.vital.types.metadata_traits import tag_traits_by_tag
import kwiver
from importlib.metadata import version

DEFAULT_CONFIG_PREFIX = (
    Path(kwiver.__file__).parents[0] / "share" / "kwiver" / version("kwiver") / "config"
)


def add_command_options():
    parser = argparse.ArgumentParser(
        prog="dump_klv",
        description="""[options]  video-file


     This program displays the KLV metadata packets that are embedded
     in a video file.""",
    )
    parser.add_argument(
        "-c",
        "--config",
        help="Configuration file for tool",
        type=argparse.FileType("r"),
    )
    parser.add_argument(
        "-d",
        "--detail",
        help="Display a detailed description of the metadata",
        action="store_true",
    )
    # positional parameters
    parser.add_argument("video_file", help="Video input file", metavar="video-file")

    return parser


def example_video_frames_metadata():

    # In this example, we will open a video and an optional configuration file.
    # By default, the configuration file dump_klv.conf installed with kwiver
    # will be used, but the user can add a configuration file with the option -c
    # that will be merged with dump_klv.conf.
    #
    # We will use klv to iterate over each frame, and print metadata according
    # to the configuration used.

    # Loading all the plugins
    vpm = plugin_management.plugin_manager_instance()
    vpm.load_all_plugins()

    # Parsing arguments
    parser = add_command_options()
    cmd_args = parser.parse_args()

    video_file: str = ""
    print(cmd_args)

    if cmd_args.video_file:
        video_file = cmd_args.video_file
    else:
        print("Missing video file name ", file=sys.stderr)
        parser.print_help()
        exit(1)

    # Intantiating the video reader
    video_reader = VideoInput()

    try:
        # the path exists when installed as a wheel
        config_path = DEFAULT_CONFIG_PREFIX / "applets" / "dump_klv.conf"
        config = read_config_file(str(config_path))
    except RuntimeError:
        # look into the parent directory for configuration files
        # this is the layout in the build/install tree
        prefix = os.path.dirname(sys.executable) + "/.."
        config = read_config_file("applets/dump_klv.conf", "", "", prefix)

    # If --config given, read in config file, merge in with default just generated
    if cmd_args.config:
        config.merge_config(read_config_file(cmd_args.config.name))

    # Printing the configuration.
    print(f"CONFIG #### {config}")

    video_reader = VideoInput.set_nested_algo_configuration("video_reader", config)

    video_reader.open(video_file)

    # We can now iterate over the frames and print metadata for each of them.

    # First, let's set up a few variables to keep track of time and on which frame we're at.
    count = 1
    ts = Timestamp()

    # Deciding how verbose we want to be
    detail = cmd_args.detail

    while video_reader.next_frame(ts):
        metadata = video_reader.frame_metadata()

        for meta in metadata:
            print(f"\n\n---------------- Metadata from: {meta.timestamp}")
            print(type(meta))
            if detail:
                for ix in meta:
                    name = ix[1].name
                    tag = ix[1].tag
                    descrip = tag_traits_by_tag(tag).description()
                    print(
                        f"""Metadata item: {name}
        {descrip}
        Data: < {ix[1].type} >: {Metadata.format_string(ix[1].as_string())}
        """
                    )
            else:
                Metadata.print_metadata(sys.stdout, meta)

        count += 1
    # end while over video


if __name__ == "__main__":
    example_video_frames_metadata()
