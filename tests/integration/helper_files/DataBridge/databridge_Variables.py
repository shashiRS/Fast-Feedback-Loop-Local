"""
    Copyright 2022 Continental Corporation

    :file: databridge_Variables.py
    :platform: Windows, Linux
    :synopsis:
        Script containing only the variables for the softwaretests from the databridge

    :author:
        - Fabian Janker (uif41320) <fabian.janker@continental-corporation.com>
"""
import json

# standard Python import area
import os

DATABRIDGE_IP_ADDRESS = "127.0.0.1"
# Port of the databridge to send commands
DATABRIDGE_PORT = "8082"
# Broadcast port from the databridge
DATABRIDGE_BROADCAST_PORT = "8080"
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
    '{"channel":"dataStream",' '"event":"UsedDataStreamUrisChanged",' '"source":"NextGUI",' '"payload":{"usedUris":[]}}'
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
    '{"channel":"SignalExplorer",' '"event":"UserPressedSignalUpdateButton",' '"source":"NextGUI",' '"payload":{}}'
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


def databridge_test_signal_Format(node_id, view=0, group=0, count_subgroup=0):
    """
    This message function generates an update signal call

    :param node_id: This is the name of the node (sdl file)
    :type node_id: string
    :param view: This is the folder under the node, defaults to 1
    :type view: int, optional
    :param group: This is the folder under the view, defaults to 1
    :type group: int, optional
    :param count_subgroup: This is the folderunder folder structure, defaults to 1
    :type count_subgroup: int, optional
    :return: Message of the update signal format
    :rtype: string
    """
    msg = (
        '{"channel":"dataStream",'
        '"event":"UserRequestedDataStreamFormat",'
        '"source":"NextGUI",'
        '"payload":{"selectedNodeId":"'
    )
    msg = msg + node_id
    if view > 0:
        msg = msg + ".View" + str(view)
    if group > 0:
        msg = msg + ".Group" + str(group)
    if count_subgroup > 0:
        for i in range(1, (count_subgroup + 1)):
            msg = msg + ".SubGroup" + str(i)
    msg = msg + '"}}'
    return msg


def databridge_confirm_signal(node_id, view=0, group=0, count_subgroup=0):
    """
    This message function generate a confirm Signal call

    :param node_id: This is the name of the node (sdl file)
    :type node_id: string
    :param view: This is the folder under the node, defaults to 1
    :type view: int, optional
    :param group: This is the folder under the view, defaults to 1
    :type group: int, optional
    :param count_subgroup: This is the folder under folder structure, defaults to 1
    :type count_subgroup: int, optional
    :return: Message of the confirmation signal format
    :rtype: string
    """
    msg = (
        '{"channel":"dataStream",'
        '"event":"UsedDataStreamUrisChanged",'
        '"source":"NextGUI",'
        '"payload":{"usedUris":"'
    )
    msg = msg + node_id
    if view > 0:
        msg = msg + ".View" + str(view)
    if group > 0:
        msg = msg + ".Group" + str(group)
    if count_subgroup > 0:
        for i in range(1, (count_subgroup + 1)):
            msg = msg + ".SubGroup" + str(i)
    msg = msg + '"}}'
    return msg


def exe_dummy_component(node_id):
    """
    generating the cmd command to start the dummy_component.exe

    :param node_id: name of the node from the signals
    :type node_id: string
    :return: cmd command
    :rtype: string
    """
    return (
        "dummy_component.exe "
        + node_id
        + " "
        + os.path.join(os.path.dirname(os.path.realpath(__file__)), node_id)
        + ".sdl"
    )


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
    else:
        return output_folder.replace("/", "\\").replace("\\", "\\\\\\\\")


# Message to start the Signal Exporter
def get_start_export_msg(signal_list: list, trigger_url: str, file_format: str, output_folder: str = ""):
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
    :return: message to start the exporter with the given configs
    :rtype: string
    """
    return (
        '{"channel" : "DataStream",'
        '"event":"WidgetRequestsDataStreamUpdateEvent",'
        '"payload":{'
        '"arguments":"{'
        '\\"event\\":\\"UserPressedStartSignalExportingButton\\",'
        '\\"listofURLs\\":[' + __signal_export_list_to_string(signal_list) + "],"
        '\\"triggerURL\\":\\"' + trigger_url + '\\",'
        '\\"fileFormat\\":\\".' + file_format + '\\",'
        '\\"outputFolder\\":\\"' + __signal_exporter_output_folder_converter(output_folder) + '\\"}",'
        '"dataClass":{'
        '"name":"NextExporter",'
        '"version":"0.0.1"}},'
        '"source":"NextGUI"}'
    )


# Message to stop the Signal Exporter
def get_stop_export_msg():
    """
    getter to construct the message for the signal exporter to stop

    :return: message to stop the exporter
    :rtype: string
    """
    return (
        '{"channel":"DataStream",'
        '"event":"WidgetRequestsDataStreamUpdateEvent",'
        '"source":"NextGUI",'
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
    message = {
        "channel": "SignalExplorer",
        "event": "UserPressedSignalSearchButton",
        "source": "NextGUI",
        "payload": {"searchString": signal},
    }
    return json.dumps(message, indent=4)


def remove_device_and_view(url: str):
    parts = url.split(".")
    if len(parts) >= 3:
        result = ".".join(parts[2:])
        return result
    else:
        return url


def ecu_sil_compl_dir():
    EXU_SIL_COMPL_DIR_LOCAL = os.path.join(
        os.path.dirname(os.path.realpath(__file__)), "..", "..", "..", "..", "..", "ECU_SIL_compl_3_0"
    )

    ecu_sil_compl_path = os.getenv("ECU_SIL_COMPL_PATH")

    if ecu_sil_compl_path is None:
        return EXU_SIL_COMPL_DIR_LOCAL
    else:
        return ecu_sil_compl_path


# Events Databridge
# ---------------------------------------------------------------------------------------------------
# Event: updating the Signals
EXPECTED_EVENT_UPDATING_SIGNALS = "DataStreamFormatUpdate"
# Event: navigate through Signals
EXPECTED_EVENT_NAVIGATE_THROUGH_SIGNALS = "DataStreamFormatUpdate"

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

# Name of the csv file which contains the expected values of the recording
CSV_NAME_REC = "2022.02.14_at_11.41.29_radar-mi_663_ext_00.05.01.454.005_00.08.56.193.112.csv"

# Path of the csv file from the expected values of the test_signals
DEFAULT_EXPECTED_VALUES_REC_LOC = os.path.join(os.path.dirname(os.path.realpath(__file__)), CSV_NAME_REC)

# Name of the csv file which contains the expected values of the test_signals
CSV_NAME_WITHOUT_REC = "test_signals.csv"

# Path of the csv file from the expected values of the test_signals
DEFAULT_EXPECTED_VALUES_WITHOUT_REC_PATH = os.path.join(
    os.path.dirname(os.path.realpath(__file__)), CSV_NAME_WITHOUT_REC
)
