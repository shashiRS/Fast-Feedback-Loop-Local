"""
    Copyright 2023 Continental Corporation

    This file contains the Next API class to read in the provided config file.

    :file: next_config_handler.py

    :author:
        - Jannik Fritz (uidr9189) <jannik.fritz@continental-corporation.com>
"""

import logging
import logging.handlers
import os
import re

from next_py_api.api_utils.config_classes import path_parsing
from next_py_api.api_utils.config_classes.simulation_export import SimulationExport
from next_py_api.api_utils.config_classes.simulation_flow import SimulationFlow
from next_py_api.api_utils.config_classes.simulation_input import SimulationInput
from next_py_api.api_utils.config_classes.simulation_nodes import SimulationNodes
from next_py_api.api_utils.config_classes.simulation_run import SimulationRun
from next_py_api.api_utils.helper_classes import helper_functions
from next_py_api.api_variables import global_vars

LOG = logging.getLogger("__main__")


class ConfigHandler:
    """
    Class to store all configuration settings for the API run
    """

    def __init__(self):
        # Dictionaries mapping the Config Category Name to the Class Object
        self.sim_flows = {}
        self.sim_runs = {}
        self.sim_input = {}
        self.sim_export = {}
        self.sim_nodes = {}

    def load_configuration(self, cfg=None):
        """
        Function used to load a config into the API.
        Can be either a file, a string or a dict.
        :param cfg: Config that shall be used by the api
        :type cfg: str or dict
        :return: True or False if config was loaded
        :rtype: bool
        """
        if isinstance(cfg, str):
            if os.path.isfile(cfg):
                return self.__load_configuration_from_file(cfg)
            return self.__load_configuration_from_string(cfg)
        if isinstance(cfg, dict):
            return self.__fill_config(cfg)
        return False

    def __load_configuration_from_string(self, cfg_str):
        # unwrap all env variables
        cfg = self.unwrap_env_vars(cfg_str)

        # Transform file to json
        cfg = helper_functions.return_json_content(cfg)
        if not cfg:
            return False

        # verify config with schema
        if not self.verify_config(input_json=cfg):
            return False

        return self.__fill_config(cfg)

    def __load_configuration_from_file(self, cfg_file):
        with open(cfg_file, encoding="utf-8") as cfg_in:
            cfg = cfg_in.read()
        return self.__load_configuration_from_string(cfg)

    def __fill_config(self, cfg):
        try:
            self.__fill_global_settings(cfg)
            self.__fill_sim_flows(cfg)
            self.__fill_sim_export(cfg)
            self.__fill_sim_input(cfg)
            self.__fill_sim_runs(cfg)
            self.__fill_sim_nodes(cfg)
        except Exception as e:  # noqa: E722 pylint: disable=W0718
            LOG.error("Exception in fill config: %s", e)
            return False
        return True

    def overwrite_configuration(self, args=None):
        """
        Overwrites the existing configuration based on arguments provided to the API.

        :param args: Command line arguments for the API
        :type args: argparser
        :return: None
        :rtype: None
        """
        if args:
            if args.i:
                # reset sim_input
                self.sim_input = {}
                # load json config file
                LOG.info("Start to overwrite configuration for Simulation Input using %s file", args.i)
                input_simulation_content = helper_functions.return_json_content(args.i)
                self.__fill_sim_input(input_simulation_content)
                LOG.info("Finish to overwrite configuration for Simulation Input")

    def __fill_global_settings(self, cfg):
        file_log_level = None
        console_log_level = None
        try:
            global_vars.root_dir = path_parsing.get_path(cfg["GlobalSettings"]["RootDir"])
        except KeyError:
            LOG.warning("No RootDir set in config file GlobalSettings.")
            LOG.warning("Going to take environment variable NEXT_API_ROOT or cwd for relative paths.")
        try:
            file_log_level = self.get_log_level(cfg["GlobalSettings"]["Logging"]["LogFileLevel"])
            console_log_level = self.get_log_level(cfg["GlobalSettings"]["Logging"]["LogConsoleLevel"])
        except KeyError:
            LOG.warning("No Logging set in config file GlobalSettings. Log Level is not set from config file")
        # set the log level
        for handler_item in LOG.handlers:
            if isinstance(handler_item, logging.StreamHandler) and console_log_level:
                LOG.info("Console log level is changed to %s", console_log_level)
                handler_item.setLevel(console_log_level)
            if isinstance(handler_item, logging.handlers.TimedRotatingFileHandler) and file_log_level:
                LOG.info("File log level is changed to %s", file_log_level)
                handler_item.setLevel(file_log_level)

    def __fill_sim_flows(self, cfg):
        for sim_flow in cfg["SimulationFlows"]:
            self.sim_flows[sim_flow] = SimulationFlow(cfg["SimulationFlows"][sim_flow])

    def __fill_sim_runs(self, cfg):
        for sim_run in cfg["SimulationRun"]:
            self.sim_runs[sim_run] = SimulationRun(cfg["SimulationRun"][sim_run])

    def __fill_sim_input(self, cfg):
        for sim_input in cfg["SimulationInput"]:
            self.sim_input[sim_input] = SimulationInput(cfg["SimulationInput"][sim_input])

    def __fill_sim_export(self, cfg):
        for sim_export in cfg["SimulationExport"]:
            self.sim_export[sim_export] = SimulationExport(cfg["SimulationExport"][sim_export])

    def __fill_sim_nodes(self, cfg):
        try:
            for sim_node in cfg["SimulationNodes"]:
                self.sim_nodes[sim_node] = SimulationNodes(cfg["SimulationNodes"][sim_node])
        except KeyError:
            LOG.warning("No simulation Node provided")

    @staticmethod
    def get_log_level(log_level_string: str):
        """
        Gets the log level defined in the provided string

        :param log_level_string: String containing the Log level
        :type log_level_string: str
        :return: Log level or None
        :rtype: logging.Loglevel
        """
        if log_level_string:
            if "error" in log_level_string.lower():
                return logging.ERROR
            if "info" in log_level_string.lower():
                return logging.INFO
            if "warn" in log_level_string.lower():
                return logging.WARNING
            if "debug" in log_level_string.lower():
                return logging.DEBUG

            LOG.warning(
                "LOG level set is not one of the level allowed: ERROR, INFO, WARN/WARNING, DEBUG."
                "Please select another level"
            )
        return None

    @staticmethod
    def verify_config(input_json_file: str = "", input_json=None):
        """
        Verifies the configuration that is handed in

        :param input_json_file: Path to json file that shall be used as config
        :type input_json_file: str
        :param input_json: Dictionary with value that shall be used as config
        :type input_json: dict
        :return: True or false if config is valid
        :rtype: bool
        """
        if input_json is None:
            input_json = {}
        try:
            if input_json_file:
                return helper_functions.validate_json_with_schema(input_json_file=input_json_file)
            if input_json:
                return helper_functions.validate_json_with_schema(input_dict=input_json)
            return False
        except Exception as e:  # pylint: disable=broad-exception-caught
            LOG.error("Error in Schema:\n%s", e)
            return False

    @staticmethod
    def unwrap_env_vars(cfg: str):
        """
        Takes a string and resolves all available environment variables that follow the syntax '$ENV(variable)$'
        If variable is not found, the str will not be altered

        :param cfg: String that will be checked for env variables
        :type cfg: str
        :return: String with resolved variables
        :rtype: str
        """
        last_found_pos = 0
        env_var_regex = r"\$ENV\((?P<name>.*)\)\$"
        full_cfg = cfg
        found_env = re.search(env_var_regex, full_cfg)
        if found_env:
            last_found_pos = found_env.span()[1]
        while found_env:
            try:
                env_var_value = helper_functions.unwrap_env_variable(found_env[0], found_env[0]).replace('"', "'")
            except AttributeError:
                return False
            full_cfg = full_cfg.replace(found_env[0], env_var_value)
            found_env = re.search(env_var_regex, full_cfg[last_found_pos:])
            if found_env:
                last_found_pos += found_env.span()[1]
        return full_cfg
