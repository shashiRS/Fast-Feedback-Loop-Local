"""
    Copyright 2022 Continental Corporation

    :file: controlbridge_Variables.py
    :platform: Windows, Linux
    :synopsis:
        Script containing only the variables for the software-tests from the controlbridge

    :author:
        - Fabian Janker (uif41320) <fabian.janker@continental-corporation.com>
"""

# Standard library imports
import json
import os
import sys
from enum import Enum

# Local application imports
from next_py_api.api_variables import global_vars


# Messages Controlbridge
# ---------------------------------------------------------------------------------------------------
def get_load_recording_msg(path):
    """
    Get message to load a recording
    Args:
        path (str): The path to the recording

    Returns:
        return the message that will be sent to load recording
    """
    adopted_path = path
    if sys.platform == "win32":
        adopted_path = os.path.normpath(path).replace("\\", "\\\\\\\\")
    return (
        '{"channel": "player","event": "UserRequestedInputOpen","source": "NextGUI","payload": {"fileName": "'
        + adopted_path
        + '", "mode": "OLS"}}'
    )


def get_load_configuration_msg(path):
    """
    Get message to load a configuration
    Args:
        path (str): The path to the configuration

    Returns:
        return the message that will be sent to load configuration
    """
    adopted_path = path
    if sys.platform == "win32":
        adopted_path = os.path.normpath(path).replace("\\", "\\\\\\\\")
    return (
        '{"channel": "configuration",'
        '"event": "UserPressedLoadGlobalConfig","source": "NextGUI","payload": {"filePath": "' + adopted_path + '"}}'
    )


class TimestampType(Enum):
    """
    enum used to set the type of timestamp
    """

    ABSOLUTE = "Absolute"
    RELATIVE = "Relative"


def get_jump_to_timestamp_msg(
    timestamp,
    timestamp_type=TimestampType.ABSOLUTE.value,
    stream_frames_in_between="false",
):
    """
    Get message to jump to timestamp
    Args:
        timestamp (int): The value of the next timestamp
        timestamp_type (TimestampType): Type of timestamp (Absolute or Relative)
        stream_frames_in_between (str): command used for play_until (stream frames in between) timestamp

    Returns:
        return the message that will be sent jump to timestamp
    """
    message = {
        "channel": "player",
        "event": "UserRequestedJumpToTimestamp",
        "source": "NextGUI",
        "payload": {
            "requestedTimeStamp": timestamp,
            "type": timestamp_type,
            "streamFramesInBetween": stream_frames_in_between,
        },
    }
    return json.dumps(message, indent=4)


CONTROLBRIDGE_LOAD_RECORDING = get_load_recording_msg(global_vars.input_folder)

# Message to close a recording
CONTROLBRIDGE_CLOSE_RECORDING = (
    '{"channel": "player",' + '"event": "UserPressedInputClearButton",' + '"source": "NextGUI",' + '"payload": {}}'
)

# Message to start a recording
CONTROLBRIDGE_PLAY_RECORDING = (
    '{"channel": "player",'
    + '"event": "UserPressedPlayButton",'
    + '"source": "NextGUI",'
    + '"payload": {"forward": true}}'
)
# Message to pause a recording
CONTROLBRIDGE_PAUSE_RECORDING = (
    '{"channel": "player",' + '"event": "UserPressedPauseButton",' + '"source": "NextGUI",' + '"payload": {}}'
)
# Message to jump to beginning
CONTROLBRIDGE_JUMP_TO_BEGINNING = (
    '{"channel": "player",' + '"event": "UserPressedJumpToBeginButton",' + '"source": "NextGUI",' + '"payload": {}}'
)

# Message to check the websocket connection
CONTROLBRIDGE_CHECK_WEBSOCKET_CONNECTION = (
    '{"channel": "player",'
    + '"event": "UserPressedCheckWebsocketConnection",'
    + '"source": "NextGUI",'
    + '"payload": {}}'
)


# Message to make n Steps
def controlbridge_step_n(steps):
    """
    Message to make n Steps
    Args:
        steps (int): number of steps

    Returns:
        return the message that will be sent to make n Steps
    """
    return (
        '{"channel": "player",'
        '"event": "UserPressedStepButton",'
        '"source": "NextGUI",'
        '"payload": {"requestedSteps": ' + steps + "}}"
    )


# Channel Controlbridge
# ---------------------------------------------------------------------------------------------------
# Channel: Player
RECEIVE_CHANNEL_PLAYER = "player"
# Channel: Logger
RECEIVE_CHANNEL_LOGGER = "logger"

# Events Controlbridge
# ---------------------------------------------------------------------------------------------------
# Event: loaded a Recording
EXPECTED_EVENT_RECORDING_LOADED = "RecordingIsLoaded"
# Event: playing a Recording
EXPECTED_EVENT_PLAYBACK_PLAYING = "PlaybackIsPlaying"
# Event: pausing a Recording
EXPECTED_EVENT_PLAYBACK_PAUSED = "PlaybackIsPaused"
# Event: no recording is loaded
EXPECTED_EVENT_RECORDING_CLOSED = "SimulationStateIsIdle"
# Event: end of the file
EXPECTED_EVENT_END_OF_FILE = "PlaybackIsPaused"
# Event: jump to beginning
EXPECTED_EVENT_JUMP_TO_BEGINNING = "PlaybackIsPaused"
# Event: jump to timestamp
EXPECTED_EVENT_JUMP_TO_TIMESTAMP = "PlaybackJumped"
# Event update global config
EXPECTED_EVENT_LOAD_CONFIGURATION = "updateGlobalConfig"

# Events Databridge
# ---------------------------------------------------------------------------------------------------
# Event: updating the Signals
EXPECTED_EVENT_UPDATING_SIGNALS = "DataStreamFormatUpdate"
# Event: navigate through Signals
EXPECTED_EVENT_NAVIGATE_THROUGH_SIGNALS = "DataStreamFormatUpdate"

# Source Controlbridge
# ---------------------------------------------------------------------------------------------------
# Source: loaded a Recording
EXPECTED_SOURCE_RECORDING_LOADED = "NextBridge"

# Payload Controlbridge
# ---------------------------------------------------------------------------------------------------
# Payload: toleration of end of file (50 ms)
TOLERANCE_PAYLOAD_END_OF_FILE = 50000

# Payload Databridge
# ---------------------------------------------------------------------------------------------------
# Payload: sigHeader
# [[children, id, isDeletable, isEditable, is Expended, isLeaf, text], [...], ...]
EXPECTED_PAYLOAD_SIGHEADER = [
    [[], "ADC4xx.EML.EmlOutput.egoVehKinematic.sigHeader.eSigStatus", False, False, False, True, "eSigStatus"],
    [[], "ADC4xx.EML.EmlOutput.egoVehKinematic.sigHeader.uiCycleCounter", False, False, False, True, "uiCycleCounter"],
    [
        [],
        "ADC4xx.EML.EmlOutput.egoVehKinematic.sigHeader.uiMeasurementCounter",
        False,
        False,
        False,
        True,
        "uiMeasurementCounter",
    ],
    [[], "ADC4xx.EML.EmlOutput.egoVehKinematic.sigHeader.uiTimeStamp", False, False, False, True, "uiTimeStamp"],
]

# Signal list of the root directory
EXPECTED_SIGNAL_ROOT_DIRECTORY = [
    "ADC4xx",
    "ARS5xx",
    "AURIX",
    "DOC_CAM_Front",
    "DOC_CAM_Left",
    "DOC_CAM_Rear",
    "DOC_CAM_Right",
    "GPS_Mi",
    "Global_Vehicle_CAN1_HS",
    "Global_Vehicle_CAN2_HS",
    "Local_ARS_CANFD",
    "Local_SRR_FL_CANFD",
    "Local_SRR_FR_CANFD",
    "Local_SRR_RL_CANFD",
    "Local_SRR_RR_CANFD",
    "Local_Valeo_CANFD",
    "SRR5xx_Front_Left",
    "SRR5xx_Front_Right",
    "SRR5xx_Rear_Left",
    "SRR5xx_Rear_Right",
]

# List of Signals to export
DATABRIDGE_SIGNAL_EXPORT_LIST = [
    "ADC4xx.EML.EmlOutput.egoVehKinematic.sigHeader.uiTimeStamp",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.sigHeader.uiMeasurementCounter",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.sigHeader.uiCycleCounter",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.sigHeader.eSigStatus",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.state[0]",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.state[1]",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.state[2]",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.state[3]",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.state[4]",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.state[5]",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.state[6]",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.state[7]",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.state[8]",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.state[9]",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.state[10]",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.state[11]",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.state[12]",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.state[13]",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.state[14]",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.state[15]",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.state[16]",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.state[17]",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.state[18]",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.state[19]",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.state[20]",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.state[21]",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.state[22]",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.emlInputSource",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.egoMotion.contributingSensors",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.localization.contributingSensors",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.egoMotion.sideSlipAngle.mu",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.egoMotion.sideSlipAngle.sig",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.egoMotion.motionState.longMotStateOverall",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.egoMotion.motionState.fwdProbability",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.egoMotion.motionState.revProbability",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.egoMotion.motionState.ssProbability",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.egoMotion.motionState.confidence",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.localization.curvatureC0.mu",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.localization.curvatureC0.sig",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.localization.coveredDistance.mu",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.localization.coveredDistance.sig",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.localization.curvatureC1.mu",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.localization.curvatureC1.sig",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.egoMotion.velocityTrans.x.mu",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.egoMotion.velocityTrans.x.sig",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.egoMotion.velocityTrans.y.mu",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.egoMotion.velocityTrans.y.sig",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.egoMotion.velocityTrans.z.mu",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.egoMotion.velocityTrans.z.sig",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.egoMotion.velocityAng.y.mu",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.egoMotion.velocityAng.y.sig",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.egoMotion.velocityAng.x.mu",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.egoMotion.velocityAng.x.sig",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.egoMotion.velocityAng.z.mu",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.egoMotion.velocityAng.z.sig",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.egoMotion.accelerationAng.x.mu",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.egoMotion.accelerationAng.x.sig",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.egoMotion.accelerationAng.y.mu",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.egoMotion.accelerationAng.y.sig",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.egoMotion.accelerationAng.z.mu",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.egoMotion.accelerationAng.z.sig",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.egoMotion.accelerationTrans.x.mu",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.egoMotion.accelerationTrans.x.sig",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.egoMotion.accelerationTrans.y.mu",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.egoMotion.accelerationTrans.y.sig",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.egoMotion.accelerationTrans.z.mu",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.egoMotion.accelerationTrans.z.sig",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.localization.globalPose.latitude.mu",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.localization.globalPose.latitude.sig",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.localization.globalPose.heading.mu",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.localization.globalPose.heading.sig",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.localization.globalPose.longitude.mu",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.localization.globalPose.longitude.sig",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.localization.globalPose.elevation.mu",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.localization.globalPose.elevation.sig",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.localization.localPose.position.x.mu",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.localization.localPose.position.x.sig",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.localization.localPose.position.y.mu",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.localization.localPose.position.y.sig",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.localization.localPose.position.z.mu",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.localization.localPose.position.z.sig",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.localization.localPose.rotationParameters.yaw.mu",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.localization.localPose.rotationParameters.yaw.sig",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.localization.localPose.rotationParameters.roll.mu",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.localization.localPose.rotationParameters.roll.sig",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.localization.localPose.rotationParameters.pitch.mu",
    "ADC4xx.EML.EmlOutput.egoVehKinematic.localization.localPose.rotationParameters.pitch.sig",
]

DATABRIDGE_SIGNAL_EXPORTER_TRIGGER_URL = "ADC4xx.EML.EmlOutput.egoVehKinematic.sigHeader.uiTimeStamp"
DATABRIDGE_SIGNAL_EXPORT_FOLDER_PATH = os.path.join(
    os.path.dirname(os.path.realpath(__file__)), "..", "..", "..", "Export"
)

# ---------------------------------------------------------------------------------------------------
# Message to reset the DataStream
DATABRIDGE_RESET_DATASTREAM = (
    '{"channel":"dataStream",'
    + '"event":"UsedDataStreamUrisChanged",'
    + '"source":"NextGUI",'
    + '"payload":{"usedUris":[]}}'
)

# Message to reset the DataClass
DATABRIDGE_RESET_DATACLASS = (
    '{"channel":"dataStreamConfiguration",'
    '"event":"UsedDataClassesChanged",'
    '"source":"NextGUI",'
    '"payload":{"configuredDataStreams":[]}}'
)

# Message to update the signals
DATABRIDGE_UPDATE_SIGNALS = (
    '{"channel":"SignalExplorer","event":"UserPressedSignalUpdateButton","source":"NextGUI","payload":{}}'
)

# Message to update the config of the 3D-Viewer
DATABRIDGE_UPDATE_CONIFG = (
    '{"channel":"dataStreamConfiguration",'
    '"event":"UserRequestedAvailableDataClasses",'
    '"source":"NextGUI",'
    '"payload":{'
    '"schemaId":"3DView",'
    '"schemaVersion":"0.0.1",'
    '"availableInterfaces":[{'
    '"schemaId":"trafficParticipants",'
    '"schemaVersion":"0.0.1"},{'
    '"schemaId":"groundLines",'
    '"schemaVersion":"0.0.1"},{'
    '"schemaId":"pointClouds",'
    '"schemaVersion":"0.0.1"},{'
    '"schemaId":"primitives",'
    '"schemaVersion":"0.0.1"},{'
    '"schemaId":"egoVehicle",'
    '"schemaVersion":"0.0.1"}]}}'
)

# Message to navigate to the SigHeader of the egoVehKinematic of the ADC4xx signal
DATABRIDGE_NAVIGATE_TO_SIGNAL_SIGHEADER = (
    '{"channel": "dataStream",'
    '"event": "UserRequestedDataStreamFormat",'
    '"source": "NextGUI",'
    '"payload": {'
    '"selectedNodeId": "ADC4xx.EML.EmlOutput.egoVehKinematic.sigHeader"}}'
)

# Message to close the Signal in the TableView
DATABRIDGE_CLOSE_SIGNAL = (
    '{"channel":"dataStream",'
    '"event":"UsedDataStreamUrisChanged",'
    '"source":"NextGUI",'
    '"payload":{'
    '"usedUris":[]}}'
)

EXPORTER_SOURCE_NAME_LIST: list = []


def __signal_export_list_to_string(signal_list: list):
    """
    internal export converter for signal list from type list to type string

    :param signal_list: List of Signals to export
    :type signal_list:  list
    :return: converted export list
    :rtype: string
    """
    return '\\"' + '\\",\\"'.join(str(e) for e in signal_list) + '\\"'


def __signal_exporter_output_folder_converter(output_folder: str):
    """
    internal export converter for output folder

    :param output_folder: output folder with path
    :type output_folder:  string
    :return: converted output folder path list
    :rtype: string
    """
    if output_folder == "":
        return DATABRIDGE_SIGNAL_EXPORT_FOLDER_PATH.replace("\\", "\\\\\\\\")
    return output_folder.replace("/", "\\").replace("\\", "\\\\\\\\")


# Message to start the Signal Exporter
def get_start_export_msg(
    signal_list: list, trigger_url: str, file_format: str, output_folder: str = "", source_name: str = "NextExporter-0"
):
    """
    getter to construct the message for the signal exporter to run

    :param signal_list: List of Signals to export
    :type signal_list:  list
    :param trigger_url: the trigger url for the exporter
    :type trigger_url: string
    :param file_format: export file type (.csv or .bsig)
    :type file_format: string
    :param output_folder: location and name of the output folder
    :type output_folder: string
    :param source_name: name of source, useful for multiple export
    :type source_name: string
    :return: message to start the exporter with the given configs
    :rtype: string
    """
    return (
        '{"channel" : "DataStream",'
        '"event":"WidgetRequestsDataStreamUpdateEvent",'
        '"source":"' + source_name + '",'
        '"payload":{'
        '"dataClass":{'
        '"name":"NextExporter",'
        '"version":"0.0.1"},'
        '"arguments":"{'
        '\\"event\\":\\"UserPressedStartSignalExportingButton\\",'
        '\\"listofURLs\\":[' + __signal_export_list_to_string(signal_list) + "],"
        '\\"triggerURL\\":\\"' + trigger_url + '\\",'
        '\\"fileFormat\\":\\"' + file_format + '\\",'
        '\\"outputFolder\\":\\"' + __signal_exporter_output_folder_converter(output_folder) + '\\"}"}}'
    )


# Message to stop the Signal Exporter
def get_stop_export_msg(source_name="NextExporter-0"):
    """
    getter to construct the message for the signal exporter to stop

    :return: message to stop the exporter
    :rtype: string
    """
    return (
        '{"channel":"DataStream",'
        '"event":"WidgetRequestsDataStreamUpdateEvent",'
        '"source":"' + source_name + '",'
        '"payload":{"dataClass":{"name":"NextExporter","version":"0.0.1"},'
        '"arguments":"{\\"event\\":\\"UserPressedStopSignalExportingButton\\",}"}}'
    )


# Message to set synchronised mode
def get_synchronised_mode_msg(allow_package_drop: str, enable_data_synchronization: str):
    """
    getter to construct the message for setting the synchronization mode
    :param allow_package_drop: flag to enable/disable package drop true/false
    :type allow_package_drop: string
    :param enable_data_synchronization: : flag to enable/disable data synchronization
    :type enable_data_synchronization: string
    :return: message set the synchronization mode
    :rtype: string
    """
    return (
        '{"channel":"userInteraction",'
        '"event":"UserUpdatedBackendConfiguration",'
        '"source":"NextGUI",'
        '"payload":{"allowPackageDrop":' + allow_package_drop + ',"enableDataSynchronization":'
        "" + enable_data_synchronization + "}}"
    )


def get_search_signal_msg(signal):
    """
    Get message to search signal
    Args:
        signal (str): signal name

    Returns:
        return the message that will be sent to search the signal
    """
    message = {
        "channel": "SignalExplorer",
        "event": "UserPressedSignalSearchButton",
        "source": "NextGUI",
        "payload": {"searchString": signal},
    }
    return json.dumps(message, indent=4)


def remove_device_and_view(url: str):
    """
    Remove the device and view
    """
    parts = url.split(".")
    if len(parts) >= 3:
        result = ".".join(parts[2:])
        return result
    return url
