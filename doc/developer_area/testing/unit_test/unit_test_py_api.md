Unit Test Py API {#unit_test_py_api}
=====

[TOC]

---

## Overview

The py API unit test cases are for testing the next python API module using pytest python module

---

## Prerequisites

Make sure you have installed all the <B>prerequisites</B>:
 
 - python  
 - python library from tests\unit\py_api\requirements.txt
```cmd
python -m pip install -r tests\unit\py_api\requirements.txt
```

---

## How to run it

Run pytest command:
1. Run all unit test
```cmd
python -m pytest tests\unit\py_api
```
2. Run a specific test
```cmd
python -m pytest tests\unit\py_api\{test_case}
```

---

## Example to create a unit test for next py API

Steps:
-# Create a test file with naming convention test_{your_file}.py
-# Import unittest from the standard library and others python libraries needed
-# Import the module(s)/package(s) that want to test it
-# Import others module(s)/package(s) needed for testing (helpers function)
-# Define a new test case class, which inherits from unittest.TestCase:
   * define a test method
       * set the inputs
       * set the expected results
       * assign the result of function/method tested to a variable
       * if needed, convert the results to be able to compare to expected results
       * compare the expected result to actual results
   * defines other test method
-# Defines a command-line entry point, which runs the unittest test-runner .main()
     
### Code Example

```{.python .numberLines}
import os
import unittest

from src.py_api.api_utils.config_classes import simulation_nodes
from tests.unit.py_api.test_helpers import format_output


class TestSimulationNodes(unittest.TestCase):
    """Test Class for checking SimulationNodes Class"""

    def test_simulationNode_class_no_input(self):
        """
        Test init function for SimulationNode class
        TestCase: no inputs
        """
        # set input(s)
        input_dict = {}

        # set expected result(s)
        expected_dict = {"nodes": [], "nodeConfig": None}

        # Execute
        actual_output = simulation_nodes.SimulationNodes(input_dict)
        # Convert the output to dict
        actual_dict = format_output.to_dict(actual_output)

        # Evaluate
        self.assertDictEqual(expected_dict, actual_dict)

    def test_simulationNode_class_single_executable(self):...
    def test_simulationNode_class_single_executable_with_arguments(self):...
    def test_simulationNode_class_single_executable_with_config(self):...
    def test_simulationNode_class_multiple_executables(self):...
    def test_simulationNode_class_multiple_executables_with_config(self):...
    def test_simulationNode_class_load_executables_from_env(self):...


if __name__ == "__main__":
    unittest.main()

```
