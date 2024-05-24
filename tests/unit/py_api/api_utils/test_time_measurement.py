"""
    Copyright 2022 Continental Corporation

    :file: test_time_measurement.pytest

    :synopsis:
        Script containing unit tests for time measurements executions

    :author:
        - Jannik Fritz (uidr9189) <jannik.fritz@continental-corporation.com>
        - Alexandru Buraga (uic928270) <alexandru.2.buraga@continental-corporation.com>
"""

import time
import unittest

from tests.unit.py_api.test_helpers.path_import_extension import (  # pylint: disable=import-error
    set_import_path,
)

set_import_path()
# pylint: disable-next=import-error, wrong-import-position
from src.py_api.next_py_api.api_utils import time_measurement  # noqa: E402


class TestTimeMeasurement(unittest.TestCase):
    """Test Class for checking TimeMeasurement Class"""

    def test_time_measurement_class_default_initialization(self):
        """
        Verifies that init with no inputs will create a default entry for the class
        """
        time_meas = time_measurement.TimeMeasurement()

        # Evaluate
        self.assertEqual(time_meas.times["default"].elapsed_time, 0)

    def test_time_measurement_class_name_initialization(self):
        """
        Verifies that the init with a name creates a default entry and an entry for the given name
        """
        time_meas = time_measurement.TimeMeasurement("test")

        # Evaluate
        self.assertEqual(time_meas.times["test"].elapsed_time, 0)
        self.assertEqual(time_meas.times["default"].elapsed_time, 0)

    def test_time_measurement_one_name_after_init(self):
        """
        Verifies the correct difference between init and first function call.
        """
        # set input(s)
        time_sleep = 0.1
        time_measurement_obj = time_measurement.TimeMeasurement("test1")

        # set expected result(s)
        expected_value = time_sleep

        # Execute
        time.sleep(time_sleep)
        time_measured_after = time_measurement_obj.measure_time("test1")

        # Evaluate
        self.assertAlmostEqual(time_measured_after, expected_value, delta=time_sleep)

    def test_time_measurement_one_name_one_iteration(self):
        """
        Verifies the correct difference when calling the measure_time twice with same name
        """
        # set input(s)
        time_sleep = 0.1
        time_measurement_obj = time_measurement.TimeMeasurement()

        # set expected result(s)
        expected_value = time_sleep

        # Execute
        time_measurement_obj.measure_time("test1")
        time.sleep(time_sleep)
        time_measured_after = time_measurement_obj.measure_time("test1")

        # Evaluate
        self.assertAlmostEqual(time_measured_after, expected_value, delta=time_sleep)

    def test_time_measurement_get_default_after_multiple_name_calls(self):
        """
        Verifies the correct difference when calling the measure_time multiple times with same name
        """
        # set input(s)
        time_sleep = 0.1
        time_measurement_obj = time_measurement.TimeMeasurement("test1")

        # Execute
        time_measurement_obj.measure_time("test1")
        time.sleep(time_sleep)
        time_measurement_obj.measure_time("test1")
        time.sleep(time_sleep)
        time_measurement_obj.measure_time("test1")
        time.sleep(time_sleep)
        time_measured_after = time_measurement_obj.measure_time()

        # Evaluate
        self.assertAlmostEqual(time_measured_after, time_sleep * 3, delta=time_sleep)

    def test_time_measurement_get_name_after_multiple_default_calls(self):
        """
        Verifies the correct difference when calling the measure_time twice with same name,
         after calling multiple other calls in between
        """
        # set input(s)
        time_sleep = 0.1
        time_measurement_obj = time_measurement.TimeMeasurement("test1")

        # Execute
        time.sleep(time_sleep)
        time_measurement_obj.measure_time()
        time.sleep(time_sleep)
        time_measurement_obj.measure_time()
        time_measured_after = time_measurement_obj.measure_time("test1")

        # Evaluate
        self.assertAlmostEqual(time_measured_after, time_sleep * 2, delta=time_sleep)

    def test_time_measurement_call_multiple_times(self):
        """
        Verifies the correct difference when calling the measure_time multiple times
         with and without a name,
        """
        time_sleep = 0.1
        time_meas = time_measurement.TimeMeasurement("test")
        time_meas.measure_time("test")

        for _ in range(3):
            time.sleep(time_sleep)
            time_measured = time_meas.measure_time("test")
            self.assertAlmostEqual(time_measured, time_sleep, delta=time_sleep)

        time_meas.measure_time()

        for _ in range(3):
            time.sleep(time_sleep)
            time_measured = time_meas.measure_time()
            self.assertAlmostEqual(time_measured, time_sleep, delta=time_sleep)

    def test_time_measurement_two_inputs_one_iteration(self):
        """
        Verifies correct time difference for multiple entries
        """
        # set input(s)
        time_sleep = 0.1
        time_measurement_obj = time_measurement.TimeMeasurement()

        # set expected result(s)
        expected_value = time_sleep

        # Execute
        time_measurement_obj.measure_time("test1")
        time_measurement_obj.measure_time("test2")
        time.sleep(time_sleep)
        time_measured_after = time_measurement_obj.measure_time("test1")
        time_measured_after2 = time_measurement_obj.measure_time("test2")

        # Evaluate
        self.assertAlmostEqual(time_measured_after, expected_value, delta=time_sleep)
        self.assertAlmostEqual(time_measured_after2, expected_value, delta=time_sleep)

    def test_time_measurement_two_inputs_multiple_iteration(self):
        """
        Verifies correct time difference for multiple entries being called multiple times
        """
        # set input(s)
        time_sleep = 0.1
        time_measurement_obj = time_measurement.TimeMeasurement()

        # set expected result(s)
        expected_value = time_sleep

        # Execute
        time_measurement_obj.measure_time("test1")
        time_measurement_obj.measure_time("test2")

        for _ in range(3):
            time.sleep(time_sleep)
            time_measured_after = time_measurement_obj.measure_time("test1")
            time_measured_after2 = time_measurement_obj.measure_time("test2")
            self.assertAlmostEqual(time_measured_after, expected_value, delta=time_sleep)
            self.assertAlmostEqual(time_measured_after2, expected_value, delta=time_sleep)

    def test_time_measurement_list_inputs_one_iteration(self):
        """
        Verifies correct time difference for list entries being called as list
        """
        # set input(s)
        time_sleep = 0.1
        time_measurement_obj = time_measurement.TimeMeasurement()

        # set expected result(s)
        expected_value = time_sleep

        # Execute
        time_measurement_obj.measure_time(["start_full_test", "start_test_step"])
        time.sleep(time_sleep)
        time_measured_after = time_measurement_obj.measure_time(["start_full_test", "start_test_step"])

        # Evaluate
        self.assertAlmostEqual(time_measured_after["start_full_test"], expected_value, delta=time_sleep)
        self.assertAlmostEqual(time_measured_after["start_test_step"], expected_value, delta=time_sleep)

    def test_time_measurement_list_inputs_separate_measurements(self):
        """
        Verifies correct time difference for list entries being called as individual calls
        """
        # set input(s)
        time_sleep = 0.1
        time_measurement_obj = time_measurement.TimeMeasurement()

        # Execute
        time_measurement_obj.measure_time(["start_full_test", "start_test_step"])
        time.sleep(time_sleep)
        time_measured_after = time_measurement_obj.measure_time("start_full_test")

        # Evaluate
        self.assertAlmostEqual(time_measured_after, time_sleep, delta=time_sleep)

        time.sleep(time_sleep)
        time_measured_after = time_measurement_obj.measure_time("start_test_step")
        self.assertAlmostEqual(time_measured_after, time_sleep * 2, delta=time_sleep)

    def test_time_measurement_separate_inputs_list_measurement(self):
        """
        Verifies correct time difference for individual calls  being called as list
        """
        # set input(s)
        time_sleep = 0.1
        time_measurement_obj = time_measurement.TimeMeasurement()

        # Execute
        time_measurement_obj.measure_time("start_full_test")
        time.sleep(time_sleep)
        time_measurement_obj.measure_time("start_test_step")
        time.sleep(time_sleep)

        # Evaluate
        elapsed_time = time_measurement_obj.measure_time(["start_full_test", "start_test_step"])
        self.assertAlmostEqual(elapsed_time["start_test_step"], time_sleep, delta=time_sleep)
        self.assertAlmostEqual(elapsed_time["start_full_test"], time_sleep * 2, delta=time_sleep)


if __name__ == "__main__":
    unittest.main()
