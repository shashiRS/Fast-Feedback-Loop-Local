"""
    Copyright 2022 Continental Corporation

    This file contains the config class to store Simulation Nodes entries.

    :file: simulation_nodes.py

    :author:
        - Jannik Fritz (uidr9189) <jannik.fritz@continental-corporation.com>
        - Alexandru Buraga <alexandru.2.buraga@continental-corporation.com>
"""

import logging
import os

from next_py_api.api_utils.config_classes import path_parsing
from next_py_api.api_utils.helper_classes import helper_functions

LOG = logging.getLogger("__main__")


class Node:  # pylint: disable=too-few-public-methods
    """
    Contains all available Config Settings
    """

    def __init__(self, node_dict):
        self.executable = node_dict.get("Executable")
        self.executable_path = path_parsing.get_path(node_dict.get("ExecutablePath"))
        self.arguments = node_dict.get("Arguments")
        self.set_environment_variables(node_dict.get("EnvVariables"))

    def set_environment_variables(self, dict_value):
        """
        Sets environment variables that are defined in the config file

        :param dict_value: Config entry with variables to set
        :type dict_value: (list of) str
        :return: True of False if variables could be set
        :rtype: bool
        """
        if isinstance(dict_value, list):
            for i_list in dict_value:
                self.set_environment_variables(i_list)
        elif isinstance(dict_value, str):
            dict_value_parsing = dict_value.split("=")
            if len(dict_value_parsing) == 2:
                env_var_name = dict_value_parsing[0]
                env_var_value = dict_value_parsing[1]
                if env_var_name and env_var_value:
                    env_var_value = path_parsing.get_path(env_var_value)
                    if "$ENV" in env_var_name:
                        env_var_name = helper_functions.return_env_variable_name(env_var_name)
                    if env_var_name in os.environ:
                        if env_var_value in os.environ.get(env_var_name):
                            LOG.info("Environment variable %s with value: %s exist", env_var_name, env_var_value)
                        else:
                            os.environ[env_var_name] += os.pathsep + env_var_value
                    else:
                        os.environ[env_var_name] = env_var_value
                    return True
            LOG.warning("Failed to parse environment variable: %s", dict_value)
        return False


class SimulationNodes:  # pylint: disable=too-few-public-methods
    """
    Class to store Node information
    """

    def __init__(self, sim_node_dict):
        self.nodes = []
        node_obj = sim_node_dict.get("Nodes")
        if node_obj:
            if isinstance(node_obj, list):
                for node in node_obj:
                    self.nodes.append(Node(node))
            elif isinstance(node_obj, dict):
                self.nodes.append(Node(node_obj))
        else:
            LOG.warning("Not found the 'Nodes' key for sim_nodes element dict")
        self.node_config = path_parsing.get_path(sim_node_dict.get("NodeConfig"))
