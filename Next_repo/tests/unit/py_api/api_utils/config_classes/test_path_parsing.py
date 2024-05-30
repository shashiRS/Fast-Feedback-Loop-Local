"""
    Copyright 2022 Continental Corporation

    :file: test_path_parsing.pytest

    :synopsis:
        Script containing unit tests for path parsing functions

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
from src.py_api.next_py_api.api_utils.config_classes import path_parsing  # noqa: E402

# pylint: enable=import-error, wrong-import-position, ungrouped-imports


class TestPathParsing(unittest.TestCase):
    """Test Class for testing path creation"""

    def test_get_path_return_normpath_on_valid_path_with_double_backslashes(self):
        """
        Tests return value of get_path if a valid input path is provided.
        get_path should return the input path as a normpath.:
        """
        # set inputs
        valid_input = os.path.dirname(os.path.realpath(__file__)).replace("/", "\\")
        # set expected results
        expected_output = os.path.normpath(valid_input)
        # Execute
        actual_output = path_parsing.get_path(valid_input)
        # Evaluate
        self.assertEqual(expected_output, actual_output)

    def test_get_path_return_normpath_on_valid_path_with_forward_slashes(self):
        """
        Tests return value of get_path if a valid input path is provided.
        get_path should return the input path as a normpath.:
        """
        # set inputs
        valid_input = os.path.dirname(os.path.realpath(__file__)).replace("\\", "/")
        # set expected results
        expected_output = os.path.normpath(valid_input)
        # Execute
        actual_output = path_parsing.get_path(valid_input)
        # Evaluate
        self.assertEqual(expected_output, actual_output)

    def test_get_path_return_normpath_input_on_invalid_path(self):
        """
        Tests return value of get_path if an invalid input path is provided.
        get_path should return the input path unchanged.
        """
        # set inputs
        invalid_input = "this/folder/is/not/there"
        # set expected results
        expected_output = os.path.normpath(invalid_input)
        # Execute
        actual_output = path_parsing.get_path(invalid_input)
        # Evaluate
        self.assertEqual(expected_output, actual_output)

    def test_get_input_on_non_path(self):
        """
        Tests return value of get_path if no input is provided.
        get_path should return the input unchanged.
        """
        # set inputs
        no_input = ["", None]
        for i in no_input:
            with self.subTest():
                # set expected results
                expected_output = i
                # Execute
                actual_output = path_parsing.get_path(i)
                # Evaluate
                self.assertEqual(expected_output, actual_output)

    def test_replace_wildcard_in_path(self):
        """
        Tests if replacing wildcards in str is working
        """
        file_path = os.path.dirname(os.path.realpath(__file__))
        wildcard_path = os.path.join(file_path, "*")
        print(file_path)
        print(os.listdir(file_path))
        print(wildcard_path)
        all_files = [os.path.join(file_path, x) for x in os.listdir(file_path) if not x.startswith(".")]
        actual_output = path_parsing.replace_wildcard_in_path(wildcard_path)
        self.assertEqual(all_files, actual_output)

    def test_get_path_replace_wildcard_in_path(self):
        """
        Tests if replacing wildcards in get_path is working
        """
        file_path = os.path.dirname(os.path.realpath(__file__))
        wildcard_path = os.path.join(file_path, "*")
        all_files = [os.path.join(file_path, x) for x in os.listdir(file_path) if not x.startswith(".")]
        actual_output = path_parsing.get_path(wildcard_path)
        self.assertEqual(all_files, actual_output)
