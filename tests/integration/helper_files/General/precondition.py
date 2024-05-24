"""
    Copyright 2022 Continental Corporation
    :file: precondition.py
    :platform: Windows, Linux
    :synopsis:
        Script containing precondition for the different software tests
    :author:
        - Fabian Janker (uif41320) <fabian.janker@continental-corporation.com>
"""

# standard Python import area
import time

# PTF or custom import area
# pylint: disable=import-error
import global_Variables
from helper_files.DataBridge import databridge_Variables
from next_api_path import path_import_extension
from ptf.ptf_utils.global_params import get_parameter
from ptf.ptf_utils.report import PRECONDITION, TESTSTEP
from ptf.verify_utils import ptf_asserts

# pylint: enable=import-error

# To fix flake8 check
if path_import_extension:  # pylint: disable=using-constant-test
    pass


def precondition_databridge_with_recording(rec_type="rrec"):
    """
    PRECONDITION:
        - Reset of the datastream
        - Reset of the dataclass
        - Loading a recording
    """

    # getting the next_api
    next_api = get_parameter("next_api")

    PRECONDITION("Load recording.")
    # sending the message to load a recording into the player
    if "zrec" == rec_type:
        recording_path = global_Variables.recording_dir(is_zrec=True)
    else:
        recording_path = global_Variables.recording_dir()
    recv_load_recording = next_api.execute.load_recording(recording_path)
    ptf_asserts.verify(
        bool(recv_load_recording),
        True,
        ("Timeout while loading the recording. Message received: %s", recv_load_recording),
    )
    PRECONDITION("Recording is loaded")

    TESTSTEP("Reset of the datastream")
    # if databridge_websocket returns False, a timeout is occurred
    ptf_asserts.verify(
        next_api.websocket.databridge.send(databridge_Variables.DATABRIDGE_RESET_DATASTREAM, False),
        True,
        "Timeout raised while reseting datastream",
    )
    PRECONDITION("Datastream is reseted")
    time.sleep(5)

    PRECONDITION("Reset of the dataclass")
    # if databridge_websocket returns False, a timeout is occurred
    ptf_asserts.verify(
        next_api.websocket.databridge.send(databridge_Variables.DATABRIDGE_RESET_DATACLASS, False),
        True,
        "Timeout raised while reseting dataclass",
    )
    TESTSTEP("Dataclass is reseted")
    time.sleep(5)


def precondition_databridge_without_recording():
    """
    PRECONDITION:
        - Reset of the datastream
        - Reset of the dataclass
    """
    # getting the next_api
    next_api = get_parameter("next_api")

    PRECONDITION("Reset of the datastream")
    # if databridge_websocket returns False, a timeout is occurred
    ptf_asserts.verify(
        next_api.websocket.databridge.send(databridge_Variables.DATABRIDGE_RESET_DATASTREAM, False),
        True,
        "Timeout raised while sending or receiving",
    )
    time.sleep(5)

    PRECONDITION("Reset of the dataclass")
    # if databridge_websocket returns False, a timeout is occurred
    ptf_asserts.verify(
        next_api.websocket.databridge.send(databridge_Variables.DATABRIDGE_RESET_DATACLASS, False),
        True,
        "Timeout raised while sending or receiving",
    )
    time.sleep(5)
