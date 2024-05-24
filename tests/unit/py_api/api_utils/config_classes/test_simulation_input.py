"""
    Copyright 2022 Continental Corporation

    :file: test_simulation_input.pytest

    :synopsis:
        Script containing unit tests for simulation input script

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
    simulation_input,
)
from tests.unit.py_api.test_helpers import format_output  # noqa: E402

# pylint: enable=import-error, wrong-import-position, ungrouped-imports

# generate dynamic path from the current file and get back 3 paths
config_path = os.path.normpath(
    os.path.join(
        os.path.dirname(os.path.realpath(__file__)),
        "..",
        "..",
        "test_helpers",
        "test_configs",
        "test_config_sim_input.json",
    )
).replace("\\", "\\\\")

recording_path = os.path.normpath(
    os.path.join(
        os.path.dirname(os.path.realpath(__file__)),
        "..",
        "..",
        "test_helpers",
        "test_configs",
        "test_recording_valid.rec",
    )
).replace("\\", "\\\\")


class TestSimulationInput(unittest.TestCase):
    """Test Class for checking Simulation Input"""

    def __init__(self, *args, **kwargs):
        super(TestSimulationInput, self).__init__(*args, **kwargs)  # pylint: disable=super-with-arguments
        # set expected result(s)
        self.recording_batches = None
        self.time_intervals = None
        self._recording_type = None
        self._split = None
        self.start_time = None
        self.end_time = None
        self.relative = None

    def test_simulation_input_class_with_list(self):
        """
        Tests the initialization of the class with a list
        """
        # set input(s)
        input_dict = []

        # set expected result(s)
        self.recording_batches = []
        self.time_intervals = []
        self._recording_type = []
        self._split = None

        # execute
        simulation_output = simulation_input.SimulationInput(input_dict)

        # evaluate
        self.assertEqual(simulation_output.recording_batches, self.recording_batches)
        self.assertEqual(simulation_output.time_intervals, self.time_intervals)
        self.assertEqual(simulation_output._recording_type, self._recording_type)  # pylint: disable=protected-access
        self.assertEqual(simulation_output._split, self._split)  # pylint: disable=protected-access

    def test_simulation_input_class_with_no_dict(self):
        """
        Tests the initialization of the class with empty dictionary
        """
        # set input(s)
        input_dict = {}

        # set expected result(s)
        self.recording_batches = []
        self.time_intervals = []
        self._recording_type = []
        self._split = None

        # execute
        simulation_output = simulation_input.SimulationInput(input_dict)

        # evaluate
        self.assertEqual(simulation_output.recording_batches, self.recording_batches)
        self.assertEqual(simulation_output.time_intervals, self.time_intervals)
        self.assertEqual(simulation_output._recording_type, self._recording_type)  # pylint: disable=protected-access
        self.assertEqual(simulation_output._split, self._split)  # pylint: disable=protected-access

    def test_simulation_input_class_with_dict_wrong_keys(self):
        """
        Tests the initialization of the class with a valid dictionary but with wrong keys
        No data given
        """
        # set input(s)
        input_dict = {
            "Spl": None,
            "Rec": "",
            "Reco": "",
            "Sim": "",
            "Tim": "",
        }

        # set expected result(s)
        self.recording_batches = []
        self.time_intervals = []
        self._recording_type = []
        self._split = None

        # execute
        simulation_output = simulation_input.SimulationInput(input_dict)

        # evaluate
        self.assertEqual(simulation_output.recording_batches, self.recording_batches)
        self.assertEqual(simulation_output.time_intervals, self.time_intervals)
        self.assertEqual(simulation_output._recording_type, self._recording_type)  # pylint: disable=protected-access
        self.assertEqual(simulation_output._split, self._split)  # pylint: disable=protected-access

    def test_simulation_input_class_with_valid_dict(self):
        """
        Tests the initialization of the class with a valid dictionary but with wrong keys
        Data:
            Split, RecordingType{string), TimeInterval(dict) given
            RecordingLocation, SimulationInputFile not given
        """
        # set input(s)
        input_dict = {
            "Split": "File",
            "RecordingType": ".rec",
            "TimeInterval": {"StartTimeUs": 1234, "EndTimeUs": 3456, "Relative": False},
        }

        # set expected result(s)
        self.recording_batches = []
        self.time_intervals = [{"start_time": 1234, "end_time": 3456, "relative": False}]
        self._recording_type = [".rec"]
        self._split = "File"

        # execute
        simulation_output = simulation_input.SimulationInput(input_dict)

        # create and append time TimeInterval obj into list
        time_output = [format_output.to_dict(simulation_output.time_intervals)]

        # evaluate
        self.assertEqual(simulation_output.recording_batches, self.recording_batches)
        self.assertEqual(time_output, self.time_intervals)
        self.assertEqual(simulation_output._recording_type, self._recording_type)  # pylint: disable=protected-access
        self.assertEqual(simulation_output._split, self._split)  # pylint: disable=protected-access

    @unittest.skip("global_vars.")
    def test_simulation_input_class_with_dict_correct_keys_rec_loc_and_sim_input(self):
        """
        Tests the initialization of the class with a valid dictionary with correct keys
        Data:
            Split, RecordingType{list), TimeInterval(list), RecordingLocation(list), SimulationInputFile(list) given
        """
        # TODO: Fix test, need to rework the get_recordings_from_file() OR  # pylint: disable=fixme
        # TODO: Refactor path_parsing.get_path() to always return paths as list  # pylint: disable=fixme
        # set input(s)
        input_dict = {
            "Split": None,
            "RecordingType": [".rec"],
            "RecordingLocation": [recording_path],
            "SimulationInputFile": [config_path, config_path],
            "TimeInterval": [
                {"StartTimeUs": 1234, "EndTimeUs": 3456, "Relative": True},
                {"StartTimeUs": 567, "EndTimeUs": 897, "Relative": True},
            ],
        }

        # set expected result(s)
        self.recording_batches = []
        self.time_intervals = []
        self._recording_type = []
        self._split = None

        # execute
        simulation_output = simulation_input.SimulationInput(input_dict)

        # evaluate
        self.assertEqual(simulation_output.recording_batches, self.recording_batches)
        self.assertEqual(simulation_output.time_intervals, self.time_intervals)
        self.assertEqual(simulation_output._recording_type, self._recording_type)  # pylint: disable=protected-access
        self.assertEqual(simulation_output._split, self._split)  # pylint: disable=protected-access

    @unittest.skip("global_vars.")
    def test_simulation_input_class_with_dict_correct_keys_sim_input(self):
        """
        Tests the initialization of the class with a valid dictionary with correct keys
        Data:
            Split, RecordingType{list), TimeInterval(list), SimulationInputFile(list) given
            RecordingLocation() not given
        """
        # TODO: Fix test, need to rework the get_recordings_from_file() OR  # pylint: disable=fixme
        # TODO: Refactor path_parsing.get_path() to always return paths as list  # pylint: disable=fixme

        # set input(s)
        input_dict = {
            "Split": None,
            "RecordingType": [".rec"],
            "SimulationInputFile": [config_path, config_path],
            "TimeInterval": {"StartTimeUs": 1234, "EndTimeUs": 3456, "Relative": False},
        }

        # set expected result(s)
        self.recording_batches = []
        self.time_intervals = []
        self._recording_type = []
        self._split = None

        # execute
        simulation_output = simulation_input.SimulationInput(input_dict)

        # evaluate
        self.assertEqual(simulation_output.recording_batches, self.recording_batches)
        self.assertEqual(simulation_output.time_intervals, self.time_intervals)
        self.assertEqual(simulation_output._recording_type, self._recording_type)  # pylint: disable=protected-access
        self.assertEqual(simulation_output._split, self._split)  # pylint: disable=protected-access

    @unittest.skip("global_vars.")
    def test_simulation_input_class_with_dict_correct_keys_rec_loc(self):
        """
        Tests the initialization of the class with a valid dictionary with correct keys
        Data:
            Split, RecordingType{list), TimeInterval(list), RecordingLocation(list) given
            SimulationInputFile() not given
        """
        # TODO: Refactor path_parsing.get_path() to always return paths as list  # pylint: disable=fixme
        # set input(s)
        input_dict = {
            "Split": None,
            "RecordingType": [".rec"],
            "RecordingLocation": [recording_path, recording_path],
            "TimeInterval": {"StartTimeUs": 1234, "EndTimeUs": 3456, "Relative": False},
        }

        # set expected result(s)
        self.recording_batches = [recording_path, recording_path]
        self.time_intervals = []
        self._recording_type = [".rec"]
        self._split = None

        # execute
        simulation_output = simulation_input.SimulationInput(input_dict)

        # evaluate
        self.assertEqual(simulation_output.recording_batches, self.recording_batches)
        self.assertEqual(simulation_output.time_intervals, self.time_intervals)
        self.assertEqual(simulation_output._recording_type, self._recording_type)  # pylint: disable=protected-access
        self.assertEqual(simulation_output._split, self._split)  # pylint: disable=protected-access

    def test_simulation_input_time_interval(self):
        """
        Tests the initialization of TimeInterval class with input
        """
        self.start_time = 1234
        self.end_time = 3456
        self.relative = True

        # execute
        time_output = simulation_input.TimeInterval(self.start_time, self.end_time, self.relative)

        # evaluate
        self.assertEqual(time_output.start_time, self.start_time)
        self.assertEqual(time_output.end_time, self.end_time)
        self.assertEqual(time_output.relative, self.relative)

    def test_simulation_input_time_interval_typecast(self):
        """
        Tests the initialization of TimeInterval class with input
        """
        self.start_time = 1234
        self.end_time = 3456
        self.relative = True
        time_interval = {"StartTimeUs": "1234", "EndTimeUs": '"3456"', "Relative": True}

        # execute
        time_output = simulation_input.add_time_interval(time_interval)

        # evaluate
        self.assertEqual(time_output.start_time, self.start_time)
        self.assertEqual(time_output.end_time, self.end_time)
        self.assertEqual(time_output.relative, self.relative)


if __name__ == "__main__":
    unittest.main()
