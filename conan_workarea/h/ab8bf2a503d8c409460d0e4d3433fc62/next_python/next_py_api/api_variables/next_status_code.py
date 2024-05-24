"""
    Copyright 2024 Continental Corporation

    This file contains classes and function to get the status code value.
    Classes:
        StatusOrigin
        NodeNames
        ResultStatusCode
    It provides methods to:
        get_status_code

    :file: next_execution.py

    :author:
        - Dirk, Koltermann (uif75429) <dirk.koltermann@continental-corporation.com>
"""

from enum import Enum, IntEnum


class StatusOrigin(IntEnum):
    """
    origin of the status code
    """

    DEFAULT = 1
    NEXT_DATABRIDGE = 2
    NEXT_CONTROLBRIDGE = 3
    NEXT_PLAYER = 4
    SIM_NODE = 5


class NodeNames(str, Enum):
    """
    names of the node
    """

    DEFAULT = "default"
    NEXT_DATABRIDGE = "next_databridge"
    NEXT_CONTROLBRIDGE = "next_controlbridge"
    NEXT_PLAYER = "next_player"
    SIM_NODE = "sim_node"


node_name_to_status_origin = {
    NodeNames.DEFAULT: StatusOrigin.DEFAULT,
    NodeNames.NEXT_DATABRIDGE: StatusOrigin.NEXT_DATABRIDGE,
    NodeNames.NEXT_CONTROLBRIDGE: StatusOrigin.NEXT_CONTROLBRIDGE,
    NodeNames.NEXT_PLAYER: StatusOrigin.NEXT_PLAYER,
    NodeNames.SIM_NODE: StatusOrigin.SIM_NODE,
}


class ResultStatusCode(IntEnum):
    """
    enum for status codes inside next framework
    Hint: file needs to be in sync with status codes from framework
    *** STATUS CODE NUMBER***
    the status code consists of 3 digits:  <status_severity><status_source><status_description>
    1. digit "status_severity": 1 = INFO, 2 = SUCCESSFUL, 3 = WARNING, 4 = ERROR
    2. digit "status source": grouping of status codes belonging to each other,
                              e.g. Lifecycle + Components in the range 1- 2 at the 2. digit
    3. digit "status description": unique number to describe status code of this source

    Example: status code 241 means: <succesful><exporter><started>

    *** STATUS CODE NAME ***
    status code naming convention: <status_severity>_<status_source>_<status_description>
    status severities are the following strings: 1 = INFO, 2 = SUCC, 3 = WARN, 4 = ERR
    e.g. SUCC_EXPORTER_STARTED

    Hints:
    1. The status code publisher event is intended to send the ResultStatusCode and will add the component type in
       front of the status code value, e.g. 1241 for exporter started (1 == databridge)
    2. One status code should only report one specific status and shouldn't be reused at other sources inside one
       executable/component (e.g. databridge). That means e.g. status codes of the Lifecycle + Components must not
       used inside the plugins of databridge.
    """

    # ** ** ** Generic 0-9 ** ** ** /

    # ** ** ** * Lifecycle + Components Range 10-29 ** ** * /
    SUCC_COMP_INIT_FINISHED = 211
    SUCC_COMP_CONFIG_FINISHED = 212
    SUCC_COMP_SHUTDOWN_FINISHED = 213

    ERR_COMP_INIT_FAILED = 411
    # ERR_COMP_CONFIG_FAILED = 412,
    # ERR_COMP_SHUTDOWN_FAILED = 413

    # ** ** ** ** Plugins Range 30-49 ** ** ** ** /
    # ERR_PLUGIN_FAILED = 431

    # ** ** * Special Plugin e.g EXPORTER 40 ** ** * /
    SUCC_EXPORTER_STARTED = 241
    SUCC_EXPORTER_STOPPED = 242
    SUCC_EXPORTER_SUBSCRIBING_FINISHED = 243

    WARN_EXPORTER_PARTIAL_SUBSCRIPTION = 341
    WARN_EXPORTER_NO_SUBSCRIPTION = 342

    ERR_EXPORTER_TRIGGER_NOT_FOUND = 441
    ERR_EXPORTER_FILE_FORMAT_NOT_SUPPORTED = 442
    ERR_EXPORTER_CONFIGURATION_NOT_SUPPORTED = 443

    # ** ** ** Player 50-69 ** ** ** ** /
    SUCC_RECORDING_LOADED = 251
    ERR_RECORDING_LOADED = 451

    SUCC_PLAYER_TRIGGER_FINISHED = 252
    ERR_PLAYER_TRIGGER_FINISHED = 452
    WARN_PLAYER_TRIGGER_NO_RECEIVER_FOUND = 352

    INFO_START_LOAD_RECORDING = 153

    # ** ** * Bridges 70-89 ** ** ** * /
    SUCC_SETUP_BACKEND_CONFIG_FINISHED = 271
    SUCC_PROCESS_QUEUE_FINISHED = 272
    SUCC_RECEIVED_CHECK_WEBSOCKET_CONNECTION = 273

    WARN_PLUGIN_NOT_FOUND = 371

    # ** ** ** * Reserved 90-99 ** ** * /
    # ERR_MODULE_NOT_FOUND = 490
    # ERR_CRASH = 491


def get_status_code(origin, code):
    """
    Function to generate status code based on the origin of the node and the ResultStatusCode
    Args:
        origin (StatusOrigin): origin of the status code
        code (ResultStatusCode): code of the status code type
        return (int):  status code
    """
    status_code = None
    if code and origin:
        status_code = origin * 1000 + code

    return status_code
