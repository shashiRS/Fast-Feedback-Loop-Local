"""
    Copyright 2023 Continental Corporation

    This file contains the Next API class to read in the provided executions from the config file.

    :file: simulation_run.py

    :author:
        - Jannik Fritz (uidr9189) <jannik.fritz@continental-corporation.com>
"""

import inspect
import logging

from next_py_api.api_utils.execution_classes import next_execution_classes
from next_py_api.custom_executions import custom_executions

LOG = logging.getLogger("__main__")


class SimulationRun:  # pylint: disable=too-many-instance-attributes
    """
    Contains all available Config Settings
    """

    def __init__(self, execution_dict):
        self.setup = []
        self.setup_args = []
        self.run = []
        self.run_args = []
        self.teardown = []
        self.teardown_args = []

        self.execution = execution_dict

        self.available_executions = {}
        self.find_available_executions()

        if "Setup" in execution_dict:
            self.fill_setup(execution_dict["Setup"])
        if "Run" in execution_dict:
            self.fill_run(execution_dict["Run"])
        if "Teardown" in execution_dict:
            self.fill_teardown(execution_dict["Teardown"])

    def find_available_executions(self):
        """
        Checks available executions in next executions and custom executions
        Adds them to dict of available executions
        """
        # Go through predefined file
        for _, obj in inspect.getmembers(next_execution_classes):
            if inspect.isclass(obj):
                # create object for each class in file
                execution_step = obj()
                self.available_executions[execution_step.get_name()] = execution_step
        # Go through custom file
        for _, obj in inspect.getmembers(custom_executions):
            if inspect.isclass(obj):
                # create object for each class in file
                execution_step = obj()
                self.available_executions[execution_step.get_name()] = execution_step

    def fill_setup(self, setup_dict):
        """
        Checks the Config and adds all execution steps for the setup if available

        :param setup_dict: Execution steps from the config
        :type setup_dict: dict
        """
        for step in setup_dict:
            if step in self.available_executions:
                self.setup.append(tuple((self.available_executions[step], setup_dict[step])))
                self.setup_args.append(setup_dict[step])
            else:
                LOG.warning("Did not find setup step '%s' as an executable command.", step)
                LOG.warning(
                    "Please make sure to use an existing one,"
                    " or create a custom command in custom_executions/custom_executions.py"
                )

    def fill_run(self, run_dict):
        """
        Checks the Config and adds all execution steps for the setup if available

        :param run_dict: Execution steps from the config
        :type run_dict: dict
        """
        for step in run_dict:
            if step in self.available_executions:
                self.run.append(tuple((self.available_executions[step], run_dict[step])))
                self.run_args.append(run_dict[step])
            else:
                LOG.warning("Did not find run step '%s' as an executable command.", step)
                LOG.warning(
                    "Please make sure to use an existing one,"
                    " or create a custom command in custom_executions/custom_executions.py"
                )

    def fill_teardown(self, teardown_dict):
        """
        Checks the Config and adds all execution steps for the setup if available

        :param teardown_dict: Execution steps from the config
        :type teardown_dict: dict
        """
        for step in teardown_dict:
            if step in self.available_executions:
                self.teardown.append(tuple((self.available_executions[step], teardown_dict[step])))
                self.teardown_args.append(teardown_dict[step])
            else:
                LOG.warning("Did not find teardown step '%s' as an executable command.", step)
                LOG.warning(
                    "Please make sure to use an existing one,"
                    " or create a custom command in custom_executions/custom_executions.py"
                )
