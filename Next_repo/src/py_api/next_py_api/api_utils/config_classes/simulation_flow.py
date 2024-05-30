"""
    Copyright 2022 Continental Corporation

    This file contains the config class to store Simulation Flow entries.

    :file: simulation_flow.py

    :author:
        - Jannik Fritz (uidr9189) <jannik.fritz@continental-corporation.com>
        - Alexandru Buraga <alexandru.2.buraga@continental-corporation.com>
"""

import logging
import os

from next_py_api.api_utils.config_classes import path_parsing
from next_py_api.api_utils.helper_classes import helper_functions
from next_py_api.api_variables import global_vars

CURRENT_FILE_PATH = os.path.dirname(os.path.realpath(__file__))
PYTHON_API_PATH = os.path.join(CURRENT_FILE_PATH, "..", "..", "..", "py_api")
LOG = logging.getLogger("__main__")


class SimulationFlow:  # pylint: disable=too-few-public-methods
    """
    Contains all available Config Settings
    """

    def __init__(self, flow_dict):
        self.sim_input = self.__parse_list_dict(flow_dict.get("SimulationInput"))
        self.sim_export = self.__parse_list_dict(flow_dict.get("SimulationExport"))
        self.sim_run = self.__parse_list_dict(flow_dict.get("SimulationRun"))
        self.sim_nodes = self.__parse_list_dict(flow_dict.get("SimulationNodes"))
        self.__fill_backend_settings(flow_dict.get("BackendSettings"))

    @staticmethod
    def __parse_list_dict(item):
        returned_list = []
        if isinstance(item, list):
            for i_item in item:
                returned_list.append(i_item)
        elif isinstance(item, str):
            returned_list.append(item)
        return returned_list

    def __fill_backend_settings(self, backend_settings_dict):
        self.backend_settings = global_vars.BackendConfig()
        if backend_settings_dict:
            if "Ports" in backend_settings_dict:
                self.backend_settings.ports = self.__fill_backend_ports(backend_settings_dict.get("Ports"))
            if "EcalNetwork" in backend_settings_dict:
                self.backend_settings.ecal_network = backend_settings_dict.get("EcalNetwork")
            if "BackendDirectory" in backend_settings_dict:
                self.backend_settings.backend_directory = path_parsing.get_path(
                    backend_settings_dict.get("BackendDirectory")
                )
            if "BackendConfig" in backend_settings_dict:
                self.backend_settings.backend_config_file = path_parsing.get_path(
                    backend_settings_dict.get("BackendConfig")
                )

    def __fill_backend_ports(self, ports_dict):
        self.backend_settings.ports = global_vars.Ports()
        if ports_dict:
            self.backend_settings.ports.controlbridge = (
                ports_dict.get("Controlbridge") if ports_dict.get("Controlbridge") else 8200
            )
            self.backend_settings.ports.databridge = (
                ports_dict.get("Databridge") if ports_dict.get("Databridge") else 8080
            )
            self.backend_settings.ports.databridge_broadcast = (
                ports_dict.get("DatabridgeBroadcast") if ports_dict.get("DatabridgeBroadcast") else 8082
            )
        self.backend_settings.ports.controlbridge = helper_functions.convert_to_int(
            self.backend_settings.ports.controlbridge
        )
        self.backend_settings.ports.databridge = helper_functions.convert_to_int(self.backend_settings.ports.databridge)
        self.backend_settings.ports.databridge_broadcast = helper_functions.convert_to_int(
            self.backend_settings.ports.databridge_broadcast
        )
        return self.backend_settings.ports
