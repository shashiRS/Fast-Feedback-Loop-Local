"""
    Copyright 2022 Continental Corporation

    :file: test_next_config_handler.pytest

    :synopsis:
        Script containing unit tests for next config handler

    :author:
        - Jannik Fritz (uidr9189) <jannik.fritz@continental-corporation.com>
        - Alexandru Buraga (uic928270) <alexandru.2.buraga@continental-corporation.com>
"""

import argparse
import json
import os
import platform
import unittest

# pylint: disable=import-error, wrong-import-position, ungrouped-imports
from tests.unit.py_api.test_helpers.path_import_extension import (  # pylint: disable=unused-import, import-error
    set_import_path,
)

set_import_path()
from src.py_api.next_py_api.api_utils import next_config_handler  # noqa: E402
from src.py_api.next_py_api.api_utils.config_classes.simulation_input import (  # noqa: E402
    SimulationInput,
)
from src.py_api.next_py_api.api_utils.helper_classes import (  # noqa: E402
    helper_functions,
)

# pylint: disable=import-error, wrong-import-position, ungrouped-imports

CURRENT_FILE_PATH = os.path.dirname(os.path.realpath(__file__))
SIM_INPUT_JSON_FILE_PATH = os.path.join(CURRENT_FILE_PATH, "..", "..", "test_helpers", "test_configs", "sim_input.json")


# pylint: disable-next=missing-class-docstring
class TestNextConfigHandler(unittest.TestCase):
    def test_simulation_update_sim_input_content_argument_present(self):
        """
        Test init function for ConfigHandler class
        TestCase: simulation input (-i) is set
        """
        # set input(s)
        cfg_content = {
            "SimulationInput": {"original_input": {"RecordingLocation": os.path.normpath(CURRENT_FILE_PATH)}},
            "SimulationExport": {},
            "simpleKPI_fromSecondSIL": {},
            "SimulationRun": {},
            "SimulationFlows": {},
            "GlobalSettings": {},
        }
        sim_input_content = helper_functions.return_json_content(SIM_INPUT_JSON_FILE_PATH)

        # Execute
        args = argparse
        args.i = SIM_INPUT_JSON_FILE_PATH
        config_handler = next_config_handler.ConfigHandler()
        config_handler.load_configuration(cfg_content)
        config_handler.overwrite_configuration(args)
        actual_results = config_handler.sim_input

        # set expected result(s)
        expected_result = SimulationInput(sim_input_content["SimulationInput"]["updated_input"])

        # Evaluate
        self.assertEqual(actual_results["updated_input"], expected_result)

    def test_no_simulation_input_as_argument(self):
        """
        Test init function for ConfigHandler class
        TestCase: simulation input (-i) is NOT set
        """
        # set input(s)
        cfg_content = {
            "SimulationInput": {"original_input": {"RecordingLocation": os.path.normpath(CURRENT_FILE_PATH)}},
            "SimulationExport": {},
            "simpleKPI_fromSecondSIL": {},
            "SimulationRun": {},
            "SimulationFlows": {},
            "GlobalSettings": {},
        }

        # Execute
        config_handler = next_config_handler.ConfigHandler()
        config_handler.load_configuration(cfg_content)
        config_handler.overwrite_configuration()
        actual_results = config_handler.sim_input

        # set expected result(s)
        expected_result = SimulationInput(cfg_content["SimulationInput"]["original_input"])

        # Evaluate
        self.assertEqual(actual_results["original_input"], expected_result)

    def test_load_cfg_single_valid_env(self):
        """
        Test unwrap of env variables in config manager
        TestCase: input output list with BackendDirectory and BackendConfig as envVariable
        """
        orig_recording_location = ""
        config_file_path = os.path.realpath(
            os.path.join(os.path.dirname(__file__), "..", "..", "test_helpers", "test_configs")
        )
        config_file = "test_config_sim_input_single_env.json"
        expected_file = "test_recording_valid.rec"
        config_path = os.path.join(config_file_path, config_file)
        expected_file_path = config_file_path + "\\" + expected_file

        if os.getenv("recording_location"):
            orig_recording_location = os.environ["recording_location"]
            del os.environ["recording_location"]
        os.environ["recording_location"] = config_file_path

        with open(config_path, encoding="utf-8") as cfg:
            full_cfg = cfg.read()

        try:
            config_handler = next_config_handler.ConfigHandler()
            actual_output = config_handler.unwrap_env_vars(full_cfg)
            actual_json = json.loads(actual_output)
            replaced_env = actual_json["SimulationInput"]["radar_mi_663"]["RecordingLocation"][0]
            self.assertEqual(replaced_env, expected_file_path)
        finally:
            if orig_recording_location:
                os.environ["recording_location"] = orig_recording_location

    def test_load_cfg_multiple_valid_env(self):  # pylint: disable=too-many-locals
        """
        Test unwrap of env variables in config manager
        TestCase: Multiple valid entries for the environment variables
        """
        config_file_path = os.path.realpath(
            os.path.join(os.path.dirname(__file__), "..", "..", "test_helpers", "test_configs")
        )
        config_file = "test_config_sim_input_multiple_env.json"
        config_path = os.path.join(config_file_path, config_file)

        with open(config_path, encoding="utf-8") as cfg:
            full_cfg = cfg.read()

        # Save original environment variables
        orig_envs = os.environ

        os.environ["recording_folder"] = "config_file_path"
        os.environ["OUTPUT_DATA_PATH"] = "D:\\\\test\\\\path"
        os.environ["OUTPUT_FILE_NAME"] = "output_file_name"
        os.environ["TIMEOUT"] = "120"

        expected_simulation_input = f"D:\\workspace\\{os.environ['recording_folder']}\\test_configs\\test_recording.rec"
        expected_out_path = os.path.normpath(os.environ["OUTPUT_DATA_PATH"])
        expected_out_name = os.environ["OUTPUT_FILE_NAME"]
        expected_timeout = os.environ["TIMEOUT"]

        try:
            config_handler = next_config_handler.ConfigHandler()
            actual_output = config_handler.unwrap_env_vars(full_cfg)
            actual_json = json.loads(actual_output)
            replaced_env_rec = actual_json["SimulationInput"]["radar_mi_663"]["RecordingLocation"][0]
            replaced_env_out_path = actual_json["SimulationExport"]["simpleKPI_fromFirstSIL"]["Output"]["OutputPath"]
            replaced_env_out_name = actual_json["SimulationExport"]["simpleKPI_fromFirstSIL"]["Output"]["OutputName"]
            replaced_env_timeout = actual_json["SimulationRun"]["exportReplayNoSimulation"]["Run"][
                "wait_until_end_of_file"
            ]
            if platform.system() != "Windows":
                replaced_env_out_path = replaced_env_out_path.replace("\\", "\\\\")

            self.assertEqual(replaced_env_rec, expected_simulation_input)
            self.assertEqual(replaced_env_out_path, expected_out_path)
            self.assertEqual(replaced_env_out_name, expected_out_name)
            self.assertEqual(replaced_env_timeout, expected_timeout)
        finally:
            # reset environment variables to original
            os.environ = orig_envs

    def test_load_cfg_invalid_env(self):
        """
        Test unwrap of env variables in config manager
        TestCase: Non existing environment variables
        """
        config_file_path = os.path.realpath(
            os.path.join(os.path.dirname(__file__), "..", "..", "test_helpers", "test_configs")
        )
        config_file = "test_config_sim_input_multiple_env.json"
        config_path = os.path.join(config_file_path, config_file)

        with open(config_path, encoding="utf-8") as cfg:
            full_cfg = cfg.read()

        # Save original environment variables
        orig_envs = os.environ

        # Remove used env variables if available
        try:
            del os.environ["recording_folder"]
        except KeyError:
            # Env variable does not exist from the start
            pass
        try:
            del os.environ["OUTPUT_DATA_PATH"]
        except KeyError:
            # Env variable does not exist from the start
            pass
        try:
            del os.environ["OUTPUT_FILE_NAME"]
        except KeyError:
            # Env variable does not exist from the start
            pass
        try:
            del os.environ["TIMEOUT"]
        except KeyError:
            # Env variable does not exist from the start
            pass

        try:
            config_handler = next_config_handler.ConfigHandler()
            print("Following Error Logs are expected; from test_load_cfg_invalid_env!")
            actual_output = config_handler.unwrap_env_vars(full_cfg)
            print("Above Error Logs are expected; from test_load_cfg_invalid_env!!")
            # No env variable found, config stays unchanged
            self.assertEqual(actual_output, full_cfg)
        finally:
            # reset environment variables to original
            os.environ = orig_envs


if __name__ == "__main__":
    unittest.main()
