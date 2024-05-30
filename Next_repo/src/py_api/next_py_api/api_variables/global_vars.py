"""
    Global Variables file for Next API.
    Copyright 2023 Continental Corporation

    :file: global_vars.py
    :synopsis:
        This file contains all the variables which are used globally within the API.

    :author:
        - Jannik Fritz <jannik.fritz@continental-corporation.com>
"""

import os

TOOL_NAME = "Next API"
# Address of the ControlBridge
CONTROLBRIDGE_IP_ADDRESS = "127.0.0.1"
DATABRIDGE_IP_ADDRESS = "127.0.0.1"


class Ports:  # pylint: disable=too-few-public-methods
    """
    Default port settings for the backend
    """

    def __init__(self, databridge=8082, databridge_broadcast=8080, controlbridge=8200):
        self.databridge = databridge
        self.databridge_broadcast = databridge_broadcast
        self.controlbridge = controlbridge


class BackendConfig:  # pylint: disable=too-few-public-methods, too-many-arguments
    """
    Default Backend configurations
    """

    def __init__(
        self,
        backend_directory=os.path.dirname(os.path.realpath(os.path.join(__file__, ".."))),
        backend_conf_file="",
        ecal_network="ecal_monitoring",
        databridge_port=8082,
        databridge_broadcast_port=8080,
        controlbridge_port=8200,
    ):
        self.ports = Ports(databridge_port, databridge_broadcast_port, controlbridge_port)
        self.ecal_network = ecal_network
        self.backend_directory = backend_directory
        self.backend_config_file = backend_conf_file

    def __eq__(self, other):
        if not other:
            return False

        return (
            self.ports.controlbridge == other.ports.controlbridge
            and self.ports.databridge == self.ports.databridge
            and self.ports.databridge_broadcast == other.ports.databridge_broadcast
            and self.backend_directory == other.backend_directory
            and self.backend_config_file == other.backend_config_file
            and self.ecal_network == other.ecal_network
        )


# exit codes
SUCCESS = 0
# if any general exception/error occurred due to config/setup
SETUP_ERR = 1
# if any issue occurred during execution
RUNTIME_ERR = 2
# Other/Undefinable
UNDEFINED_ERR = 3

# TODO will be replaced with cfg handler classes  # pylint: disable=fixme
# pylint: disable=invalid-name
backend_dir = ""
input_folder = ""
output_folder = ""

dryrun = False
root_dir = None

start_timestamp_us = 0.0
current_timestamp_us = 0.0
end_timestamp_us = -1.0

# pylint: enable=invalid-name

API_CONF_FOLDER = os.path.join(os.path.dirname(os.path.realpath(__file__)), "..", "conf")
DEFAULT_API_CONFIG = os.path.join(API_CONF_FOLDER, "next_api_full_example_config.json")
API_CONFIG_SCHEMA = os.path.join(API_CONF_FOLDER, "python_api_schema.json")
