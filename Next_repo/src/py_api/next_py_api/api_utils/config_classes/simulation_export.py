"""
    Copyright 2022 Continental Corporation

    This file contains the config class to store Simulation Export entries.

    :file: simulation_export.py

    :author:
        - Jannik Fritz (uidr9189) <jannik.fritz@continental-corporation.com>
        - Alexandru Buraga <alexandru.2.buraga@continental-corporation.com>
"""

import json
import logging
import os

from next_py_api.api_utils.config_classes import path_parsing

LOG = logging.getLogger("__main__")


# pylint: disable=too-few-public-methods
class ExportSignal:
    """
    Contains all available Config Settings
    """

    def __init__(self, signal_data):
        # Handle the case where signal_data is not a dictionary
        self.run_export_file = []
        self.signals = []
        self.trigger = ""
        self.export_type = []
        self.explicit_name = ""

        if isinstance(signal_data, dict):
            if isinstance(signal_data.get("SimulationExportFile"), list):
                for i_run_export_file in signal_data.get("SimulationExportFile"):
                    self.run_export_file.append(path_parsing.get_path(i_run_export_file))
            else:
                self.run_export_file = path_parsing.get_path(signal_data.get("SimulationExportFile", []))
            self.signals = signal_data.get("Signals", [])

            self.trigger = signal_data.get("Trigger", "")
            if isinstance(signal_data.get("ExportType"), list):
                for i_export_type in signal_data.get("ExportType"):
                    self.export_type.append(i_export_type)
            else:
                self.export_type.append(signal_data.get("ExportType"))
            self.explicit_name = signal_data.get("ExplicitName", "")

        # Check if the run_export_file option exists
        if self.run_export_file:
            # If it exists, read signals and trigger from the specified file
            self._read_signals_from_file()

    def _read_signals_from_file(self):
        # Check if run_export_file is a list
        if not isinstance(self.run_export_file, list):
            if not os.path.isfile(self.run_export_file):
                return False
            with open(self.run_export_file, "r", encoding="utf-8") as file:
                data = json.load(file)
                self.signals += data.get("Signals", [])
                self.trigger = data.get("Trigger", "")
                return True
        else:
            for file_path in self.run_export_file:
                # Check if the file path is valid
                if not os.path.isfile(file_path):
                    return False
                # Read signals and trigger from the specified file
                with open(file_path, "r", encoding="utf-8") as file:
                    data = json.load(file)
                    self.signals += data.get("Signals", [])
                    self.trigger = data.get("Trigger", "")
                    return True


class OutputConfig:
    """
    Class to store Options for output path
    """

    def __init__(self, output_data):
        self.output_path = path_parsing.get_path(output_data.get("OutputPath", ""), create=True)
        self.output_name = output_data.get("OutputName", "")
        self.overwrite = output_data.get("Overwrite", False)


class SimulationExport:
    """
    Class to store information for the next exporter
    """

    def __init__(self, export_dict):
        self.export_signals = []
        self.output_config = None
        self._parse_configs(export_dict)

    def _parse_configs(self, export_dict):
        for key, config in export_dict.items():
            if key == "ExportSignals":
                print(key, config)
                print("-------------------------------------")
                if isinstance(config, list):
                    for i_export_signal in config:
                        self.export_signals.append(ExportSignal(i_export_signal))
                else:
                    self.export_signals.append(ExportSignal(config))
            elif key == "Output":
                self.output_config = OutputConfig(config)
                print(self.output_config)
                print("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@")


# pylint: enable=too-few-public-methods
