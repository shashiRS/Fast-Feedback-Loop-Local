"""
    Copyright 2022 Continental Corporation

    :file: test_simulation_flow.pytest

    :synopsis:
        Script containing unit tests for simulation flow script

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
from src.py_api.next_py_api.api_utils.config_classes import (  # noqa: E402
    simulation_flow,
)
from tests.unit.py_api.test_helpers import format_output  # noqa: E402

# pylint: enable=import-error, wrong-import-position, ungrouped-imports
current_file = os.path.realpath(__file__)


class TestSimulationFlow(unittest.TestCase):
    """Test Class for checking SimulationFlow Class"""

    def test_simulation_flow_class_no_input(self):
        """
        Test init function for SimulationFlow class
        TestCase: no inputs
        """
        # set input(s)
        input_dict = {}

        # set expected result(s)
        expected_dict = {
            "sim_input": [],
            "sim_export": [],
            "sim_run": [],
            "sim_nodes": [],
            "backend_settings": {
                "backend_directory": os.path.realpath(
                    os.path.join(__file__, "..", "..", "..", "..", "..", "..", "src", "py_api", "next_py_api")
                ),
                "backend_config_file": "",
                "ecal_network": "ecal_monitoring",
                "ports": {"databridge": 8082, "databridge_broadcast": 8080, "controlbridge": 8200},
            },
        }

        # Execute
        actual_output = simulation_flow.SimulationFlow(input_dict)
        # Convert the output to dict
        actual_dict = format_output.to_dict(actual_output)

        # Evaluate
        self.assertDictEqual(expected_dict, actual_dict)

    def test_required_categories_input_output_list_with_one_element(self):
        """
        Test init function for SimulationFlow class
        TestCase: input output list with one element
        """
        # set input(s)
        input_dict = {
            "SimulationInput": "single_file",
            "SimulationExport": "single_export_signal",
            "SimulationRun": "single_execution_step",
        }

        # set expected result(s)
        expected_dict = {
            "sim_input": ["single_file"],
            "sim_export": ["single_export_signal"],
            "sim_run": ["single_execution_step"],
            "sim_nodes": [],
            "backend_settings": {
                "backend_directory": os.path.realpath(
                    os.path.join(__file__, "..", "..", "..", "..", "..", "..", "src", "py_api", "next_py_api")
                ),
                "backend_config_file": "",
                "ecal_network": "ecal_monitoring",
                "ports": {"databridge": 8082, "databridge_broadcast": 8080, "controlbridge": 8200},
            },
        }

        # Execute
        actual_output = simulation_flow.SimulationFlow(input_dict)
        # Convert the output to dict
        actual_dict = format_output.to_dict(actual_output)

        # Evaluate
        self.assertDictEqual(expected_dict, actual_dict)

    def test_all_categories_valid_input_output_list(self):
        """
        Test init function for SimulationFlow class
        TestCase: input output list
        """
        # set input(s)
        input_dict = {
            "SimulationInput": "list_of_files_read_parallel",
            "SimulationExport": "list_of_export_signals",
            "SimulationRun": "multiple_execution_steps",
            "SimulationNodes": "single_executable",
        }

        # set expected result(s)
        expected_dict = {
            "sim_input": ["list_of_files_read_parallel"],
            "sim_export": ["list_of_export_signals"],
            "sim_run": ["multiple_execution_steps"],
            "sim_nodes": ["single_executable"],
            "backend_settings": {
                "backend_directory": os.path.realpath(
                    os.path.join(__file__, "..", "..", "..", "..", "..", "..", "src", "py_api", "next_py_api")
                ),
                "backend_config_file": "",
                "ecal_network": "ecal_monitoring",
                "ports": {"databridge": 8082, "databridge_broadcast": 8080, "controlbridge": 8200},
            },
        }

        # Execute
        actual_output = simulation_flow.SimulationFlow(input_dict)
        # Convert the output to dict
        actual_dict = format_output.to_dict(actual_output)

        # Evaluate
        self.assertDictEqual(expected_dict, actual_dict)

    def test_all_categories_multiple_valid_input_outputs_list_with_all_entries(self):
        """
        Test init function for SimulationFlow class
        TestCase: list with all entries
        """
        # set input(s)
        input_dict = {
            "SimulationInput": ["list_of_files_read_parallel", "all_rec_from_folder_with_recording_type_parallel"],
            "SimulationExport": ["list_of_export_signals", "single_export_signal"],
            "SimulationRun": [
                "multiple_execution_steps",
                "execution_steps_with_teardown",
                "execution_steps_with_setup",
            ],
            "SimulationNodes": ["single_executable", "load_executables_from_env"],
        }

        # set expected result(s)
        expected_dict = {
            "sim_input": ["list_of_files_read_parallel", "all_rec_from_folder_with_recording_type_parallel"],
            "sim_export": ["list_of_export_signals", "single_export_signal"],
            "sim_run": ["multiple_execution_steps", "execution_steps_with_teardown", "execution_steps_with_setup"],
            "sim_nodes": ["single_executable", "load_executables_from_env"],
            "backend_settings": {
                "backend_directory": os.path.realpath(
                    os.path.join(__file__, "..", "..", "..", "..", "..", "..", "src", "py_api", "next_py_api")
                ),
                "backend_config_file": "",
                "ecal_network": "ecal_monitoring",
                "ports": {"databridge": 8082, "databridge_broadcast": 8080, "controlbridge": 8200},
            },
        }

        # Execute
        actual_output = simulation_flow.SimulationFlow(input_dict)
        # Convert the output to dict
        actual_dict = format_output.to_dict(actual_output)

        # Evaluate
        self.assertDictEqual(expected_dict, actual_dict)

    def test_single_and_multiple_selections_all_categories_mixed(self):
        """
        Test init function for SimulationFlow class
        TestCase: input output list with mixed selections
        """
        # set input(s)
        input_dict = {
            "SimulationInput": ["list_of_files_read_parallel", "all_rec_from_folder_with_recording_type_parallel"],
            "SimulationExport": "list_of_export_signals",
            "SimulationRun": ["multiple_execution_steps", "execution_steps_with_teardown"],
            "SimulationNodes": "load_executables_from_env",
        }

        # set expected result(s)
        expected_dict = {
            "sim_input": ["list_of_files_read_parallel", "all_rec_from_folder_with_recording_type_parallel"],
            "sim_export": ["list_of_export_signals"],
            "sim_run": ["multiple_execution_steps", "execution_steps_with_teardown"],
            "sim_nodes": ["load_executables_from_env"],
            "backend_settings": {
                "backend_directory": os.path.realpath(
                    os.path.join(__file__, "..", "..", "..", "..", "..", "..", "src", "py_api", "next_py_api")
                ),
                "backend_config_file": "",
                "ecal_network": "ecal_monitoring",
                "ports": {"databridge": 8082, "databridge_broadcast": 8080, "controlbridge": 8200},
            },
        }

        # Execute
        actual_output = simulation_flow.SimulationFlow(input_dict)
        # Convert the output to dict
        actual_dict = format_output.to_dict(actual_output)

        # Evaluate
        self.assertDictEqual(expected_dict, actual_dict)

    def test_flow_with_backend_ports_settings(self):
        """
        Test init function for SimulationFlow class
        TestCase: input list with bad values for ports
        """
        # set input(s)
        input_dict = {
            "SimulationInput": "single_file",
            "SimulationExport": "single_export_signal",
            "SimulationRun": "single_execution_step",
            "BackendSettings": {"Ports": {"Controlbridge": 1000, "Databridge": 2000, "DatabridgeBroadcast": 3000}},
        }

        # set expected result(s)
        expected_dict = {
            "sim_input": ["single_file"],
            "sim_export": ["single_export_signal"],
            "sim_run": ["single_execution_step"],
            "sim_nodes": [],
            "backend_settings": {
                "backend_directory": os.path.realpath(
                    os.path.join(__file__, "..", "..", "..", "..", "..", "..", "src", "py_api", "next_py_api")
                ),
                "backend_config_file": "",
                "ecal_network": "ecal_monitoring",
                "ports": {"databridge": 2000, "databridge_broadcast": 3000, "controlbridge": 1000},
            },
        }

        # Execute
        actual_output = simulation_flow.SimulationFlow(input_dict)
        # Convert the output to dict
        actual_dict = format_output.to_dict(actual_output)

        # Evaluate
        self.assertDictEqual(expected_dict, actual_dict)

    def test_flow_with_backend_settings(self):
        """
        Test init function for SimulationFlow class
        TestCase: input output list with backend settings
        """
        # set input(s)
        input_dict = {
            "SimulationInput": "single_file",
            "SimulationExport": "single_export_signal",
            "SimulationRun": "single_execution_step",
            "BackendSettings": {
                "Ports": {"Controlbridge": 8200, "Databridge": 8080, "DatabridgeBroadcast": 8082},
                "EcalNetwork": "ecal_monitoring_example",
                "BackendDirectory": "D:\\path\\to\\backend\\executables",
                "BackendConfig": "path/to/backend/config/file.json",
            },
        }

        # set expected result(s)
        expected_dict = {
            "sim_input": ["single_file"],
            "sim_export": ["single_export_signal"],
            "sim_run": ["single_execution_step"],
            "sim_nodes": [],
            "backend_settings": {
                "backend_directory": os.path.normpath("D:\\path\\to\\backend\\executables"),
                "backend_config_file": os.path.normpath("path/to/backend/config/file.json"),
                "ecal_network": "ecal_monitoring_example",
                "ports": {"databridge": 8080, "databridge_broadcast": 8082, "controlbridge": 8200},
            },
        }

        # Execute
        actual_output = simulation_flow.SimulationFlow(input_dict)
        # Convert the output to dict
        actual_dict = format_output.to_dict(actual_output)

        # Evaluate
        self.assertDictEqual(expected_dict, actual_dict)

    def test_flow_with_backend_settings_from_env(self):
        """
        Test init function for SimulationFlow class
        TestCase: input output list with BackendDirectory and BackendConfig as envVariable and bad ports values
        """

        # set input(s)
        orig_backend_dir_location_value = ""
        orig_config_location_value = ""

        if os.getenv("backend_dir_location"):
            orig_backend_dir_location_value = os.environ["backend_dir_location"]
            del os.environ["backend_dir_location"]
        if os.getenv("config_location"):
            orig_config_location_value = os.environ["config_location"]
            del os.environ["config_location"]

        input_dict = {
            "SimulationInput": "single_file",
            "SimulationExport": "single_export_signal",
            "SimulationRun": "single_execution_step",
            "BackendSettings": {
                "Ports": {"Controlbridge": 1000, "Databridge": 2000, "DatabridgeBroadcast": 3000},
                "EcalNetwork": "ecal_monitoring_example",
                "BackendDirectory": "$ENV(backend_dir_location)$/config_classes",
                "BackendConfig": "path/with/$ENV(config_location)$/backend/to/config/file.json",
            },
        }

        # set executable_location env var
        backend_dir_location_value = os.path.normpath("./../api_utils")
        os.environ["backend_dir_location"] = backend_dir_location_value

        # set expected result(s)
        expected_dict = {
            "sim_input": ["single_file"],
            "sim_export": ["single_export_signal"],
            "sim_run": ["single_execution_step"],
            "sim_nodes": [],
            "backend_settings": {
                "backend_directory": os.path.normpath("$ENV(backend_dir_location)$/config_classes"),
                "backend_config_file": os.path.normpath("path/with/$ENV(config_location)$/backend/to/config/file.json"),
                "ecal_network": "ecal_monitoring_example",
                "ports": {"databridge": 2000, "databridge_broadcast": 3000, "controlbridge": 1000},
            },
        }

        try:
            # Execute
            actual_output = simulation_flow.SimulationFlow(input_dict)
            # Convert the output to dict
            actual_dict = format_output.to_dict(actual_output)

            # Evaluate
            self.assertDictEqual(expected_dict, actual_dict)
        finally:
            if os.getenv("backend_dir_location"):
                del os.environ["backend_dir_location"]
                if orig_backend_dir_location_value:
                    os.environ["backend_dir_location"] = orig_backend_dir_location_value
            if os.getenv("config_location"):
                del os.environ["config_location"]
                if orig_config_location_value:
                    os.environ["config_location"] = orig_config_location_value

    def test_flow_with_both_env_var_from_backend_settings(self):
        """
        Test init function for SimulationFlow class
        TestCase: input output list with BackendDirectory and BackendConfig as envVariable
        """
        # set input(s)
        orig_backend_dir_location_value = ""
        orig_config_location_value = ""
        config_location_suffix = os.path.normpath("./../../")

        if os.getenv("backend_dir_location"):
            orig_backend_dir_location_value = os.environ["backend_dir_location"]
            del os.environ["backend_dir_location"]
        if os.getenv("config_location"):
            orig_config_location_value = os.environ["config_location"]
            del os.environ["config_location"]

        input_dict = {
            "SimulationInput": "single_file",
            "SimulationExport": "single_export_signal",
            "SimulationRun": "single_execution_step",
            "BackendSettings": {
                "Ports": {"Controlbridge": 8200, "Databridge": 8082, "DatabridgeBroadcast": 8080},
                "EcalNetwork": "ecal_monitoring_example",
                "BackendDirectory": "$ENV(backend_dir_location)$/config_classes",
                "BackendConfig": f"{config_location_suffix}/$ENV(config_location)$/test_configs/node_config.json",
            },
        }

        # set executable_location env var
        backend_dir_location_value = os.path.normpath("./../api_utils")
        os.environ["backend_dir_location"] = backend_dir_location_value
        config_location_value = os.path.normpath("test_helpers")
        os.environ["config_location"] = config_location_value

        # set expected result(s)
        expected_dict = {
            "sim_input": ["single_file"],
            "sim_export": ["single_export_signal"],
            "sim_run": ["single_execution_step"],
            "sim_nodes": [],
            "backend_settings": {
                "backend_directory": os.path.normpath("$ENV(backend_dir_location)$/config_classes"),
                "backend_config_file": os.path.normpath(
                    config_location_suffix + "/$ENV(config_location)$" + "/test_configs/node_config.json"
                ),
                "ecal_network": "ecal_monitoring_example",
                "ports": {"databridge": 8082, "databridge_broadcast": 8080, "controlbridge": 8200},
            },
        }
        try:
            # Execute
            actual_output = simulation_flow.SimulationFlow(input_dict)
            # Convert the output to dict
            actual_dict = format_output.to_dict(actual_output)

            # Evaluate
            self.assertDictEqual(expected_dict, actual_dict)
        finally:
            if os.getenv("backend_dir_location"):
                del os.environ["backend_dir_location"]
                if orig_backend_dir_location_value:
                    os.environ["backend_dir_location"] = orig_backend_dir_location_value
            if os.getenv("config_location"):
                del os.environ["config_location"]
                if orig_config_location_value:
                    os.environ["config_location"] = orig_config_location_value


if __name__ == "__main__":
    unittest.main()
