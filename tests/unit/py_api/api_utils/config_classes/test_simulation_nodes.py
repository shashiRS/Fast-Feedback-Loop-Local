"""
    Copyright 2022 Continental Corporation

    :file: test_simulation_nodes.pytest

    :synopsis:
        Script containing unit tests for simulation nodes script

    :author:
        - Jannik Fritz (uidr9189) <jannik.fritz@continental-corporation.com>
        - Alexandru Buraga (uic928270) <alexandru.2.buraga@continental-corporation.com>
"""

import os
import unittest

# pylint: disable=import-error, wrong-import-position, ungrouped-imports
from tests.unit.py_api.test_helpers.path_import_extension import (  # pylint: disable=unused-import, import-error
    set_import_path,
)

set_import_path()
# pylint: disable-next=
from src.py_api.next_py_api.api_utils.config_classes import (  # noqa: E402
    path_parsing,
    simulation_nodes,
)
from tests.unit.py_api.test_helpers import format_output  # noqa: E402

# pylint: enable=import-error, wrong-import-position, ungrouped-imports


class TestSimulationNodes(unittest.TestCase):
    """Test Class for checking SimulationNodes Class"""

    def test_simulation_node_class_no_input(self):
        """
        Test init function for SimulationNode class
        TestCase: no inputs
        """
        # set input(s)
        input_dict = {}

        # set expected result(s)
        expected_dict = {"nodes": [], "node_config": None}

        # Execute
        actual_output = simulation_nodes.SimulationNodes(input_dict)
        # Convert the output to dict
        actual_dict = format_output.to_dict(actual_output)

        # Evaluate
        self.assertDictEqual(expected_dict, actual_dict)

    def test_simulation_node_class_single_executable(self):
        """
        Test init function for SimulationNode class
        TestCase: single executable and relative path to executable_path
        """
        # set input(s)
        executable_path = os.path.normpath("../api_utils")
        input_dict = {"Nodes": {"Executable": "sim_watcher", "ExecutablePath": executable_path}}

        # set expected result(s)
        expected_dict = {
            "nodes": [
                {
                    "executable": "sim_watcher",
                    "executable_path": executable_path,
                    "arguments": None,
                }
            ],
            "node_config": None,
        }

        # Execute
        actual_output = simulation_nodes.SimulationNodes(input_dict)
        # Convert the output to dict
        actual_dict = format_output.to_dict(actual_output)

        # Evaluate
        self.assertDictEqual(expected_dict, actual_dict)

    def test_simulation_node_class_single_executable_with_arguments(self):
        """
        Test init function for SimulationNode class
        TestCase: single executable with arguments and relative path to executable_path
        """
        # set input(s)
        executable_path = os.path.normpath("./../api_utils")
        input_dict = {
            "Nodes": {
                "Executable": "si_core.exe",
                "ExecutablePath": executable_path,
                "Arguments": "-n",
            }
        }

        # set expected result(s)
        expected_dict = {
            "nodes": [
                {
                    "executable": "si_core.exe",
                    "executable_path": executable_path,
                    "arguments": "-n",
                }
            ],
            "node_config": None,
        }

        # Execute
        actual_output = simulation_nodes.SimulationNodes(input_dict)
        # Convert the output to dict
        actual_dict = format_output.to_dict(actual_output)

        # Evaluate
        self.assertDictEqual(expected_dict, actual_dict)

    def test_simulation_node_class_single_executable_with_config(self):
        """
        Test init function for SimulationNode class
        TestCase: single executable without arguments and with config
        TestCase: executable_path is windows relative path
        TestCase: node_config_path is full path
        """
        # set input(s)
        executable_path = os.path.normpath("\\..\\api_utils")
        node_config_path = os.path.realpath("\\..\\..\\test_helpers\\test_configs\\node_config.json")
        input_dict = {
            "Nodes": {"Executable": "sim_watcher", "ExecutablePath": executable_path},
            "NodeConfig": node_config_path,
        }

        # set expected result(s)
        expected_dict = {
            "nodes": [
                {
                    "executable": "sim_watcher",
                    "executable_path": executable_path,
                    "arguments": None,
                }
            ],
            "node_config": node_config_path,
        }

        # Execute
        actual_output = simulation_nodes.SimulationNodes(input_dict)
        # Convert the output to dict
        actual_dict = format_output.to_dict(actual_output)

        # Evaluate
        self.assertDictEqual(expected_dict, actual_dict)

    def test_simulation_node_class_multiple_executables(self):
        """
        Test init function for SimulationNode class
        TestCase: multiple executables
        """
        # set input(s)
        executable_path_1 = os.path.normpath("\\..\\api_utils")
        executable_path_2 = os.path.normpath("\\..\\api_variables")
        input_dict = {
            "Nodes": [
                {"Executable": "si_core.exe", "ExecutablePath": executable_path_1, "Arguments": "-n"},
                {"Executable": "some_other_node", "ExecutablePath": executable_path_2},
            ]
        }

        # set expected result(s)
        expected_dict = {
            "nodes": [
                {
                    "executable": "si_core.exe",
                    "executable_path": executable_path_1,
                    "arguments": "-n",
                },
                {
                    "executable": "some_other_node",
                    "executable_path": executable_path_2,
                    "arguments": None,
                },
            ],
            "node_config": None,
        }

        # Execute
        actual_output = simulation_nodes.SimulationNodes(input_dict)
        # Convert the output to dict
        actual_dict = format_output.to_dict(actual_output)

        # Evaluate
        self.assertDictEqual(expected_dict, actual_dict)

    def test_simulation_node_class_multiple_executables_with_config(self):
        """
        Test init function for SimulationNode class
        TestCase: multiple_executables_with_config
        """
        # set input(s)
        executable_path_1 = os.path.normpath("\\..\\api_utils")
        executable_path_2 = os.path.normpath("\\..\\api_variables")
        node_config_path = os.path.realpath("\\..\\..\\test_helpers\\test_configs\\node_config.json")
        input_dict = {
            "Nodes": [
                {"Executable": "si_core.exe", "ExecutablePath": executable_path_1, "Arguments": "-n"},
                {"Executable": "some_other_node", "ExecutablePath": executable_path_2},
            ],
            "NodeConfig": node_config_path,
        }

        # set expected result(s)
        expected_dict = {
            "nodes": [
                {
                    "executable": "si_core.exe",
                    "executable_path": executable_path_1,
                    "arguments": "-n",
                },
                {
                    "executable": "some_other_node",
                    "executable_path": executable_path_2,
                    "arguments": None,
                },
            ],
            "node_config": node_config_path,
        }

        # Execute
        actual_output = simulation_nodes.SimulationNodes(input_dict)
        # Convert the output to dict
        actual_dict = format_output.to_dict(actual_output)

        # Evaluate
        self.assertDictEqual(expected_dict, actual_dict)

    def test_simulation_node_class_one_node_one_env_variable(self):
        """
        Test init function for SimulationNode class
        TestCase: one node with one envVariable as string
        """
        # set input(s)
        orig_env_var_value = ""
        env_var_name = "LD_LIBRARY_PATH"
        env_var_value = "./../api_variables"
        if os.getenv(env_var_name):
            orig_env_var_value = os.environ[env_var_name]
            del os.environ[env_var_name]
        input_dict = {
            "Nodes": {
                "Executable": "si_core.exe",
                "ExecutablePath": "D:\\full\\path\\to\\executable",
                "Arguments": "-n",
                "EnvVariables": f"{env_var_name}={env_var_value}",
            },
            "NodeConfig": "C:\\this\\is\\what\\they\\do.json",
        }

        # set expected result(s)
        expected_dict = {
            "nodes": [
                {
                    "executable": "si_core.exe",
                    "executable_path": os.path.normpath("D:\\full\\path\\to\\executable"),
                    "arguments": "-n",
                },
            ],
            "node_config": os.path.normpath("C:\\this\\is\\what\\they\\do.json"),
        }

        try:
            # Execute
            actual_output = simulation_nodes.SimulationNodes(input_dict)
            # Convert the output to dict
            actual_dict = format_output.to_dict(actual_output)

            # Evaluate
            env_var_value = path_parsing.get_path(env_var_value)
            self.assertEqual(os.getenv(f"{env_var_name}"), env_var_value)
            self.assertDictEqual(expected_dict, actual_dict)
        finally:
            if os.getenv(env_var_name):
                del os.environ[env_var_name]
                if orig_env_var_value:
                    os.environ[env_var_name] = orig_env_var_value

    def test_simulation_node_class_one_node_list_one_env_variable(self):
        """
        Test init function for SimulationNode class
        TestCase: one node with one envVariable as list of strings
        """
        # set input(s)
        orig_env_var_value = ""
        env_var_name = "LD_LIBRARY_PATH"
        env_var_value = "./../api_variables"
        if os.getenv(env_var_name):
            orig_env_var_value = os.environ[env_var_name]
            del os.environ[env_var_name]
        input_dict = {
            "Nodes": {
                "Executable": "si_core.exe",
                "ExecutablePath": "D:\\full\\path\\to\\executable",
                "Arguments": "-n",
                "EnvVariables": [f"{env_var_name}={env_var_value}"],
            },
            "NodeConfig": "C:\\this\\is\\what\\they\\do.json",
        }

        # set expected result(s)
        expected_dict = {
            "nodes": [
                {
                    "executable": "si_core.exe",
                    "executable_path": os.path.normpath("D:\\full\\path\\to\\executable"),
                    "arguments": "-n",
                },
            ],
            "node_config": os.path.normpath("C:\\this\\is\\what\\they\\do.json"),
        }
        try:
            # Execute
            actual_output = simulation_nodes.SimulationNodes(input_dict)
            # Convert the output to dict
            actual_dict = format_output.to_dict(actual_output)

            # Evaluate
            env_var_value = path_parsing.unwrap_relative_path(env_var_value)
            self.assertEqual(os.getenv(f"{env_var_name}"), os.path.realpath(env_var_value))
            self.assertDictEqual(expected_dict, actual_dict)
        finally:
            if os.getenv(env_var_name):
                del os.environ[env_var_name]
                if orig_env_var_value:
                    os.environ[env_var_name] = orig_env_var_value

    def test_simulation_node_class_one_node_list_more_env_variables(self):
        """
        Test init function for SimulationNode class
        TestCase: one node with more envVariables as list of strings
        """
        # set input(s)
        orig_env_var_value = ""
        env_var_name = "LD_LIBRARY_PATH"
        env_var_value_1 = "./../api_variables"
        env_var_value_2 = "./../api_utils"
        env_var_value_3 = "./../api_websocket"
        if os.getenv(env_var_name):
            orig_env_var_value = os.environ[env_var_name]
            del os.environ[env_var_name]
        input_dict = {
            "Nodes": {
                "Executable": "si_core.exe",
                "ExecutablePath": "D:\\full\\path\\to\\executable",
                "Arguments": "-n",
                "EnvVariables": [
                    f"{env_var_name}={env_var_value_1}",
                    f"{env_var_name}={env_var_value_2}",
                    f"{env_var_name}={env_var_value_3}",
                ],
            },
            "NodeConfig": "C:\\this\\is\\what\\they\\do.json",
        }

        # set expected result(s)
        expected_dict = {
            "nodes": [
                {
                    "executable": "si_core.exe",
                    "executable_path": os.path.normpath("D:\\full\\path\\to\\executable"),
                    "arguments": "-n",
                },
            ],
            "node_config": os.path.normpath("C:\\this\\is\\what\\they\\do.json"),
        }
        try:
            # Execute
            actual_output = simulation_nodes.SimulationNodes(input_dict)
            # Convert the output to dict
            actual_dict = format_output.to_dict(actual_output)

            # Evaluate
            env_var_value_1 = path_parsing.unwrap_relative_path(env_var_value_1)
            env_var_value_2 = path_parsing.unwrap_relative_path(env_var_value_2)
            env_var_value_3 = path_parsing.unwrap_relative_path(env_var_value_3)
            self.assertIn(os.path.realpath(env_var_value_1), os.getenv(env_var_name))
            self.assertIn(os.path.realpath(env_var_value_2), os.getenv(env_var_name))
            self.assertIn(os.path.realpath(env_var_value_3), os.getenv(env_var_name))
            self.assertDictEqual(expected_dict, actual_dict)
        finally:
            if os.getenv(env_var_name):
                del os.environ[env_var_name]
                if orig_env_var_value:
                    os.environ[env_var_name] = orig_env_var_value

    # pylint: disable-next=too-many-locals
    def test_simulation_node_class_more_nodes_list_more_env_variables_one_duplicate(self):
        """
        Test init function for SimulationNode class
        TestCase: more nodes with more envVariable as list of strings but one is duplicate
        """
        # set input(s)
        orig_env_var_value_1 = ""
        orig_env_var_value_2 = ""
        env_var_name_1 = "LD_LIBRARY_PATH_1"
        env_var_name_2 = "LD_LIBRARY_PATH_2"
        env_var_value_1 = "./../api_variables"
        env_var_value_2 = "./../api_utils"
        if os.getenv(env_var_name_1):
            orig_env_var_value_1 = os.environ[env_var_name_1]
            del os.environ[env_var_name_1]
        if os.getenv(env_var_name_2):
            orig_env_var_value_2 = os.environ[env_var_name_1]
            del os.environ[env_var_name_2]
        input_dict_1 = {
            "Nodes": {
                "Executable": "si_core.exe",
                "ExecutablePath": "D:\\full\\path\\to\\executable",
                "Arguments": "-n",
                "EnvVariables": [f"{env_var_name_1}={env_var_value_1}"],
            },
            "NodeConfig": "C:\\this\\is\\what\\they\\do.json",
        }
        input_dict_2 = {
            "Nodes": {
                "Executable": "si_core.exe",
                "ExecutablePath": "D:\\full\\path\\to\\executable",
                "Arguments": "-n",
                "EnvVariables": [f"{env_var_name_2}={env_var_value_2}"],
            },
            "NodeConfig": "C:\\this\\is\\what\\they\\do.json",
        }
        input_dict_3 = {
            "Nodes": {
                "Executable": "si_core.exe",
                "ExecutablePath": "D:\\full\\path\\to\\executable",
                "Arguments": "-n",
                "EnvVariables": [f"{env_var_name_1}={env_var_value_1}"],
            },
            "NodeConfig": "C:\\this\\is\\what\\they\\do.json",
        }

        # set expected result(s)
        expected_dict_template = {
            "nodes": [
                {
                    "executable": "si_core.exe",
                    "executable_path": os.path.normpath("D:\\full\\path\\to\\executable"),
                    "arguments": "-n",
                },
            ],
            "node_config": os.path.normpath("C:\\this\\is\\what\\they\\do.json"),
        }
        expected_list = [expected_dict_template, expected_dict_template, expected_dict_template]
        try:
            # Execute
            actual_output_1 = simulation_nodes.SimulationNodes(input_dict_1)
            actual_output_2 = simulation_nodes.SimulationNodes(input_dict_2)
            actual_output_3 = simulation_nodes.SimulationNodes(input_dict_3)
            # Convert the output to dict
            actual_list = [
                format_output.to_dict(actual_output_1),
                format_output.to_dict(actual_output_2),
                format_output.to_dict(actual_output_3),
            ]

            # Evaluate
            env_var_value_1 = path_parsing.unwrap_relative_path(env_var_value_1)
            env_var_value_2 = path_parsing.unwrap_relative_path(env_var_value_2)
            self.assertIn(os.path.realpath(env_var_value_1), os.getenv(env_var_name_1))
            self.assertIn(os.path.realpath(env_var_value_2), os.getenv(env_var_name_2))
            # check if env_var_value_1 is only once in env_var_name_1
            self.assertTrue(
                os.getenv(env_var_name_1).count(env_var_value_1) == 1,
                f"{env_var_value_1} count more than 1: {os.getenv(env_var_name_1).count(env_var_value_1)}",
            )
            self.assertListEqual(expected_list, actual_list)
        finally:
            if os.getenv(env_var_name_1):
                del os.environ[env_var_name_1]
                if orig_env_var_value_1:
                    os.environ[env_var_name_1] = orig_env_var_value_1
            if os.getenv(env_var_name_2):
                del os.environ[env_var_name_2]
                if orig_env_var_value_2:
                    os.environ[env_var_name_2] = orig_env_var_value_2

    def test_simulation_node_class_one_node_list_more_env_variables_one_not_ok(self):
        """
        Test init function for SimulationNode class
        TestCase: one node with more envVariables as list of strings with one not ok
        """
        # set input(s)
        orig_env_var_value = ""
        env_var_name = "LD_LIBRARY_PATH"
        env_var_value_1 = "./../api_variables"
        env_var_value_2 = "./../api_utils"
        env_var_value_3 = "./../api_websocket"
        if os.getenv(env_var_name):
            orig_env_var_value = os.environ[env_var_name]
            del os.environ[env_var_name]
        input_dict = {
            "Nodes": {
                "Executable": "si_core.exe",
                "ExecutablePath": "D:\\full\\path\\to\\executable",
                "Arguments": "-n",
                "EnvVariables": [
                    f"{env_var_name}={env_var_value_1}",
                    f"{env_var_name}{env_var_value_2}",
                    f"{env_var_name},{env_var_value_3}",
                ],
            },
            "NodeConfig": "C:\\this\\is\\what\\they\\do.json",
        }

        # set expected result(s)
        expected_dict = {
            "nodes": [
                {
                    "executable": "si_core.exe",
                    "executable_path": os.path.normpath("D:\\full\\path\\to\\executable"),
                    "arguments": "-n",
                },
            ],
            "node_config": os.path.normpath("C:\\this\\is\\what\\they\\do.json"),
        }
        try:
            # Execute
            actual_output = simulation_nodes.SimulationNodes(input_dict)
            # Convert the output to dict
            actual_dict = format_output.to_dict(actual_output)

            # Evaluate
            env_var_value_1 = path_parsing.unwrap_relative_path(env_var_value_1)
            env_var_value_2 = path_parsing.unwrap_relative_path(env_var_value_2)
            env_var_value_3 = path_parsing.unwrap_relative_path(env_var_value_3)
            self.assertIn(os.path.realpath(env_var_value_1), os.getenv(env_var_name))
            self.assertIsNotNone(os.path.realpath(env_var_value_2), os.getenv(env_var_name))
            self.assertIsNotNone(os.path.realpath(env_var_value_3), os.getenv(env_var_name))
            self.assertDictEqual(expected_dict, actual_dict)
        finally:
            if os.getenv(env_var_name):
                del os.environ[env_var_name]
                if orig_env_var_value:
                    os.environ[env_var_name] = orig_env_var_value

    # pylint: disable=too-many-statements, too-many-locals
    def test_simulation_node_class_more_nodes_list_more_env_variables(self):
        """
        Test init function for SimulationNode class
        TestCase: more nodes with more envVariable as list of strings
        """
        # set input(s)
        orig_env_var_value_1 = ""
        orig_env_var_value_2 = ""
        orig_env_var_value_3 = ""
        env_var_name_1 = "LD_LIBRARY_PATH_1"
        env_var_name_2 = "LD_LIBRARY_PATH_2"
        env_var_name_3 = "LD_LIBRARY_PATH_3"
        env_var_value_1 = "./api_variables"
        env_var_value_2 = "./api_utils"
        env_var_value_3 = "./api_websocket"
        if os.getenv(env_var_name_1):
            orig_env_var_value_1 = os.environ[env_var_name_1]
            del os.environ[env_var_name_1]
        if os.getenv(env_var_name_2):
            orig_env_var_value_2 = os.environ[env_var_name_1]
            del os.environ[env_var_name_2]
        if os.getenv(env_var_name_3):
            orig_env_var_value_3 = os.environ[env_var_name_1]
            del os.environ[env_var_name_3]
        input_dict_1 = {
            "Nodes": {
                "Executable": "si_core.exe",
                "ExecutablePath": "D:\\full\\path\\to\\executable",
                "Arguments": "-n",
                "EnvVariables": [f"{env_var_name_1}={env_var_value_1}"],
            },
            "NodeConfig": "C:\\this\\is\\what\\they\\do.json",
        }
        input_dict_2 = {
            "Nodes": {
                "Executable": "si_core.exe",
                "ExecutablePath": "D:\\full\\path\\to\\executable",
                "Arguments": "-n",
                "EnvVariables": [f"{env_var_name_2}={env_var_value_2}"],
            },
            "NodeConfig": "C:\\this\\is\\what\\they\\do.json",
        }
        input_dict_3 = {
            "Nodes": {
                "Executable": "si_core.exe",
                "ExecutablePath": "D:\\full\\path\\to\\executable",
                "Arguments": "-n",
                "EnvVariables": [f"{env_var_name_3}={env_var_value_3}"],
            },
            "NodeConfig": "C:\\this\\is\\what\\they\\do.json",
        }

        # set expected result(s)
        expected_dict_template = {
            "nodes": [
                {
                    "executable": "si_core.exe",
                    "executable_path": os.path.normpath("D:\\full\\path\\to\\executable"),
                    "arguments": "-n",
                },
            ],
            "node_config": os.path.normpath("C:\\this\\is\\what\\they\\do.json"),
        }
        expected_list = [expected_dict_template, expected_dict_template, expected_dict_template]
        try:
            # Execute
            env_var_value_1 = path_parsing.unwrap_relative_path(env_var_value_1)
            env_var_value_2 = path_parsing.unwrap_relative_path(env_var_value_2)
            env_var_value_3 = path_parsing.unwrap_relative_path(env_var_value_3)
            actual_output_1 = simulation_nodes.SimulationNodes(input_dict_1)
            actual_output_2 = simulation_nodes.SimulationNodes(input_dict_2)
            actual_output_3 = simulation_nodes.SimulationNodes(input_dict_3)
            # Convert the output to dict
            actual_list = [
                format_output.to_dict(actual_output_1),
                format_output.to_dict(actual_output_2),
                format_output.to_dict(actual_output_3),
            ]

            # Evaluate
            env_var_value_1 = path_parsing.unwrap_relative_path(env_var_value_1)
            env_var_value_2 = path_parsing.unwrap_relative_path(env_var_value_2)
            env_var_value_3 = path_parsing.unwrap_relative_path(env_var_value_3)
            self.assertTrue(
                env_var_value_1 in os.getenv(env_var_name_1),
                f"{env_var_value_1} not in {env_var_name_1}. Value of env var: {os.getenv(env_var_name_1)}",
            )
            self.assertTrue(
                env_var_value_2 in os.getenv(env_var_name_2),
                f"{env_var_value_2} not in {env_var_name_2}. Value of env var: {os.getenv(env_var_name_1)}",
            )
            self.assertTrue(
                env_var_value_3 in os.getenv(env_var_name_3),
                f"{env_var_value_3} not in {env_var_name_3}. Value of env var: {os.getenv(env_var_name_1)}",
            )
            self.assertListEqual(expected_list, actual_list)
        finally:
            if os.getenv(env_var_name_1):
                del os.environ[env_var_name_1]
                if orig_env_var_value_1:
                    os.environ[env_var_name_1] = orig_env_var_value_1
            if os.getenv(env_var_name_2):
                del os.environ[env_var_name_2]
                if orig_env_var_value_2:
                    os.environ[env_var_name_2] = orig_env_var_value_2
            if os.getenv(env_var_name_3):
                del os.environ[env_var_name_3]
                if orig_env_var_value_3:
                    os.environ[env_var_name_3] = orig_env_var_value_3


if __name__ == "__main__":
    unittest.main()
