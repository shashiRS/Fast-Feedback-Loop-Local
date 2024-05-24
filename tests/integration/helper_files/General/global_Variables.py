# pylint: disable=invalid-name
"""
    Copyright 2022 Continental Corporation

    :file: global_Variables.py
    :platform: Windows, Linux
    :synopsis:
        Script containing only global variables shared for the softwaretests

    :author:
        - Fabian Janker (uif41320) <fabian.janker@continental-corporation.com>
"""

# standard Python import area
import os
import platform
import re

# PTF or custom import area
# pylint: disable=import-error
from ptf.verify_utils import ptf_asserts

# pylint: enable=import-error

# path to the conan_workarea where the packages are stored
PACKAGE_PATH = os.path.join(os.path.dirname(os.path.realpath(__file__)), "..", "..", "..", "..", "conan_workarea")

# regex pattern for the right package detection
REGEX_PACKAGE_WS = r"^package\.next\.generic\.(\d+\.)?(\d+\.)?(\*|\d+)(-\S+)?\.vs2017$"
REGEX_PACKAGE_BTEST_WS = r"^package\.next\.generic\.btest\.GENERIC\.(\d+\.)?(\d+\.)?(\*|\d+)(-\S+)?\.vs2017$"
REGEX_PACKAGE_LX = r"^package\.next\.generic\.(\d+\.)?(\d+\.)?(\*|\d+)(-\S+)?\.linux64_x86_gcc_7-v2(-release)?$"
REGEX_PACKAGE_BTEST_LX = (
    r"^package\.next\.generic\.btest\.GENERIC\.(\d+\.)?(\d+\.)?(\*|\d+)(-\S+)?\.linux64_x86_gcc_7-v2(-release)?$"
)

# define on which operating system ConTest is operating
operating_system = platform.system()

# searching for the right package name for the executables
if operating_system == "Windows":
    # searching for Windows the package folder for controlbridge, databridge, Next-Player
    for list_element in os.listdir(PACKAGE_PATH):
        matches = re.fullmatch(REGEX_PACKAGE_WS, list_element, re.MULTILINE)
        if matches is not None:
            EXE_DIR = os.path.join(
                os.path.dirname(os.path.realpath(__file__)),
                "..",
                "..",
                "..",
                "..",
                "conan_workarea",
                list_element,
                "bin",
            )

    # searching for Windows the package folder for mock_component
    for list_element in os.listdir(PACKAGE_PATH):
        matches = re.fullmatch(REGEX_PACKAGE_BTEST_WS, list_element, re.MULTILINE)
        if matches is not None:
            DUMMY_COMPONENT_DIR = os.path.join(
                os.path.dirname(os.path.realpath(__file__)),
                "..",
                "..",
                "..",
                "..",
                "conan_workarea",
                list_element,
                "tests",
                "cases",
                "dummy_component",
            )
elif operating_system == "Linux":
    # searching for Linux the package folder for controlbridge, databridge, Next-Player
    for list_element in os.listdir(PACKAGE_PATH):
        matches = re.fullmatch(REGEX_PACKAGE_LX, list_element, re.MULTILINE)
        if matches is not None:
            EXE_DIR = os.path.join(
                os.path.dirname(os.path.realpath(__file__)),
                "..",
                "..",
                "..",
                "..",
                "conan_workarea",
                list_element,
                "bin",
            )

    # searching for Linux the package folder for mock_component
    for list_element in os.listdir(PACKAGE_PATH):
        matches = re.fullmatch(REGEX_PACKAGE_BTEST_LX, list_element, re.MULTILINE)
        if matches is not None:
            DUMMY_COMPONENT_DIR = os.path.join(
                os.path.dirname(os.path.realpath(__file__)),
                "..",
                "..",
                "..",
                "..",
                "conan_workarea",
                list_element,
                "tests",
                "cases",
                "dummy_component",
            )
else:
    ptf_asserts.fail("Operating system could not be defined!")


def recording_dir(is_zrec=False):
    """
    Returns path to recording for tests

    :param is_zrec: Set true if zrec should be used, otherwise rrec is used
    :type is_zrec: bool
    :return: path to recording
    :rtype: str
    """
    if is_zrec is False:
        # file name of the recording in the Recordings folders
        # pylint: disable-next=invalid-name
        RECORDING = "4_parallel_park_in_succ_extract_ext_00.00.20.204.418_00.00.25.115.625.rrec"
    else:
        # file name of the zrec recording file in the Recording Folder
        # pylint: disable-next=invalid-name
        RECORDING = "4_parallel_park_in_succ_extract_ext_00.00.20.204.418_00.00.25.115.625.zrec"

    # Directory path saved for the recording and replacing the escape character
    # pylint: disable-next=invalid-name
    RECORDING_DIR_LOCAL = os.path.join(
        os.path.dirname(os.path.realpath(__file__)), "..", "..", "..", "Recordings", RECORDING
    ).replace("\\", "\\\\")

    recording_path = os.getenv("NEXT_CONTEST_RECORDING_PATH")

    if recording_path is None:
        return RECORDING_DIR_LOCAL
    return os.path.join(recording_path, RECORDING).replace("\\", "\\\\")
