"""
    Copyright 2022 Continental Corporation

    :file: test_helper_functions.pytest

    :synopsis:
        Script containing unit tests for next helper functions

    :author:
        - Jannik Fritz (uidr9189) <jannik.fritz@continental-corporation.com>
        - Alexandru Buraga (uic928270) <alexandru.2.buraga@continental-corporation.com>
"""

import unittest

from tests.unit.py_api.test_helpers.path_import_extension import (  # pylint: disable=unused-import, import-error
    set_import_path,
)

set_import_path()
# pylint: disable-next=import-error, wrong-import-position
from src.py_api.next_py_api.api_utils.helper_classes import (  # noqa: E402
    helper_functions,
)


class TestHelperFunctions(unittest.TestCase):  # pylint: disable=missing-class-docstring
    def test_return_matched_patterns_no_inputs(self):
        """
        Test return_matched_patterns function from helper_classes
        TestCase: no inputs
        """
        # set input(s)
        pattern = ""
        config = []

        # set expected result(s)
        expected_output = []

        # Execute
        actual_output = helper_functions.return_matched_patterns(config=config, re_pattern=pattern)

        # Evaluate
        self.assertEqual(expected_output, actual_output)

    def test_return_matched_patterns_config_no_config(self):
        """
        Test return_matched_patterns function from helper_classes
        TestCase: no config
        """
        # set input(s)
        pattern = "KPI"
        config = []

        # set expected result(s)
        expected_output = []

        # Execute
        actual_output = helper_functions.return_matched_patterns(config=config, re_pattern=pattern)

        # Evaluate
        self.assertEqual(expected_output, actual_output)

    def test_return_matched_patterns_config_single_name(self):
        """
        Test return_matched_patterns function from helper_classes
        TestCase: pattern is a single name found in the configuration
        """
        # set input(s)
        pattern = "runKPI"
        config = ["runKPI", "exampleA", "runSet", "startKPI", "xxKPIxww"]

        # set expected result(s)
        expected_output = ["runKPI"]

        # Execute
        actual_output = helper_functions.return_matched_patterns(config=config, re_pattern=pattern)

        # Evaluate
        self.assertListEqual(expected_output, actual_output)

    def test_return_matched_patterns_config_single_name_not_in_config(self):
        """
        Test return_matched_patterns function from helper_classes
        TestCase: pattern is a single name NOT found in the configuration
        """
        # set input(s)
        pattern = "notInConfiguration"
        config = ["runKPI", "exampleA", "runSet", "startKPI", "xxKPIxww"]

        # set expected result(s)
        expected_output = []

        # Execute
        actual_output = helper_functions.return_matched_patterns(config=config, re_pattern=pattern)

        # Evaluate
        self.assertListEqual(expected_output, actual_output)

    def test_return_matched_patterns_config_multiple_name(self):
        """
        Test return_matched_patterns function from helper_classes
        TestCase: pattern is multiple name, not all are found in the configuration
                  tested the case-sensitive
        """
        # set input(s)
        pattern = "runKPI example sTaRtkPi noInConfig"
        config = ["runKPI", "exampleA", "runSet", "startKPI", "xxKPIxww"]

        # set expected result(s)
        expected_output = ["runKPI", "exampleA"]

        # Execute
        actual_output = helper_functions.return_matched_patterns(config=config, re_pattern=pattern)

        # Evaluate
        self.assertListEqual(expected_output, actual_output)

    def test_return_matched_patterns_config_single_regex(self):
        """
        Test return_matched_patterns function from helper_classes
        TestCase: pattern is a regex (case-sensitive) found in the configuration
                  return all keys that contains kpi (case-sensitive)
        """
        # set input(s)
        pattern = ".*KPI"
        config = ["runKPI", "exampleA", "runSet", "startKpI", "xxKPIxww"]

        # set expected result(s)
        expected_output = ["runKPI", "xxKPIxww"]

        # Execute
        actual_output = helper_functions.return_matched_patterns(config=config, re_pattern=pattern)

        # Evaluate
        self.assertListEqual(expected_output, actual_output)

    def test_return_matched_patterns_config_single_regex_not_in_config(self):
        """
        Test return_matched_patterns function from helper_classes
        TestCase: pattern is a regex(key start with "conf") (case-sensitive) NOT found in the configuration
        """
        # set input(s)
        pattern = "^(conf).+"
        config = ["runKPI", "exampleA", "runSet", "startKPI", "xxKPIxww"]

        # set expected result(s)
        expected_output = []

        # Execute
        actual_output = helper_functions.return_matched_patterns(config=config, re_pattern=pattern)

        # Evaluate
        self.assertListEqual(expected_output, actual_output)

    def test_return_matched_patterns_config_multiple_regexes(self):
        """
        Test return_matched_patterns function from helper_classes
        TestCase: pattern is multiple regexes, not all are found in the configuration
                  tested the case-sensitive
        """
        # set input(s)
        pattern = ".*KPI .+(LeA)$ [^a-zA-z]"
        config = ["runKPI", "exampleA", "runSet", "startKPI", "xxKpIxww"]

        # set expected result(s)
        expected_output = ["runKPI", "startKPI"]

        # Execute
        actual_output = helper_functions.return_matched_patterns(config=config, re_pattern=pattern)

        # Evaluate
        self.assertListEqual(expected_output, actual_output)

    def test_return_matched_patterns_config_multiple_name_and_regexes(self):
        """
        Test return_matched_patterns function from helper_classes
        TestCase: pattern is multiple regex name, not all are found in the configuration
                  tested the case-sensitive
        """
        # set input(s)
        pattern = ".*kPI .+(leA)$ runSet [^a-zA-Z]"
        config = ["runKPI", "exampleA", "runSet", "startKPI", "xxKPIxww"]

        # set expected result(s)
        expected_output = ["exampleA", "runSet"]

        # Execute
        actual_output = helper_functions.return_matched_patterns(config=config, re_pattern=pattern)

        # Evaluate
        self.assertListEqual(expected_output, actual_output)

    def test_return_unmatched_patterns_no_inputs(self):
        """
        Test return_unmatched_patterns function from helper_classes
        TestCase: no inputs
        """
        # set input(s)
        pattern = ""
        config = []

        # set expected result(s)
        expected_output = []

        # Execute
        actual_output = helper_functions.return_unmatched_patterns(config=config, re_pattern=pattern)

        # Evaluate
        self.assertEqual(expected_output, actual_output)

    def test_return_unmatched_patterns_no_match(self):
        """
        Test return_unmatched_patterns function from helper_classes
        TestCase: pattern doesn't match with a config element (CASE SENSITIVE)
                  return the entire config
        """
        # set input(s)
        pattern = "runkpi"
        config = ["runKPI", "exampleA", "startKPI", "runSeT", "KPITests"]

        # set expected result(s)
        expected_output = config

        # Execute
        actual_output = helper_functions.return_unmatched_patterns(config=config, re_pattern=pattern)

        # Evaluate
        self.assertEqual(expected_output, actual_output)

    def test_return_unmatched_patterns_no_pattern(self):
        """
        Test return_unmatched_patterns function from helper_classes
        TestCase: no patterns given, all config should be returned
        """
        # set input(s)
        pattern = ""
        config = ["runKPI", "exampleA", "startKPI", "runSeT", "KPITests"]

        # set expected result(s)
        expected_output = ["runKPI", "exampleA", "startKPI", "runSeT", "KPITests"]

        # Execute
        actual_output = helper_functions.return_unmatched_patterns(config=config, re_pattern=pattern)

        # Evaluate
        self.assertEqual(expected_output, actual_output)

    def test_return_unmatched_patterns_no_pattern_matched(self):
        """
        Test return_unmatched_patterns function from helper_classes
        TestCase: no patterns matched, all config should be returned
        """
        # set input(s)
        pattern = "noMatch"
        config = ["runKPI", "exampleA", "startKPI", "runSeT", "KPITests"]

        # set expected result(s)
        expected_output = ["runKPI", "exampleA", "startKPI", "runSeT", "KPITests"]

        # Execute
        actual_output = helper_functions.return_unmatched_patterns(config=config, re_pattern=pattern)

        # Evaluate
        self.assertEqual(expected_output, actual_output)

    def test_return_unmatched_patterns_single_name(self):
        """
        Test return_unmatched_patterns function from helper_classes
        TestCase: pattern is a single name found in the configuration
        """
        # set input(s)
        pattern = "runKPI"
        config = ["runKPI", "exampleA", "runSet", "startKPI", "xxKPIxww"]

        # set expected result(s)
        expected_output = ["exampleA", "runSet", "startKPI", "xxKPIxww"]

        # Execute
        actual_output = helper_functions.return_unmatched_patterns(config=config, re_pattern=pattern)

        # Evaluate
        self.assertEqual(expected_output, actual_output)

    def test_return_unmatched_patterns_single_regex(self):
        """
        Test return_unmatched_patterns function from helper_classes
        TestCase: pattern is a regex (case-sensitive) found in the configuration
                  return all keys that NOT contains kpi (case-sensitive)
        """
        # set input(s)
        pattern = ".*KPI"
        config = ["runKPI", "exampleA", "runSet", "startKpI", "xxKPIxww"]

        # set expected result(s)
        expected_output = ["exampleA", "runSet", "startKpI"]

        # Execute
        actual_output = helper_functions.return_unmatched_patterns(config=config, re_pattern=pattern)

        # Evaluate
        self.assertListEqual(expected_output, actual_output)

    def test_return_unmatched_patterns_config_single_regex_not_in_config(self):
        """
        Test return_unmatched_patterns function from helper_classes
        TestCase: pattern is a regex(key start with "conf") (case-sensitive) NOT found in the configuration
        """
        # set input(s)
        pattern = "^(conf).+"
        config = ["runKPI", "exampleA", "runSet", "startKPI", "xxKPIxww"]

        # set expected result(s)
        expected_output = ["runKPI", "exampleA", "runSet", "startKPI", "xxKPIxww"]

        # Execute
        actual_output = helper_functions.return_unmatched_patterns(config=config, re_pattern=pattern)

        # Evaluate
        self.assertListEqual(expected_output, actual_output)

    def test_return_unmatched_patterns_config_multiple_name_and_regexes(self):
        """
        Test return_unmatched_patterns function from helper_classes
        TestCase: pattern is multiple regex name, not all are found in the configuration
                  tested the case-sensitive
        """
        # set input(s)
        pattern = ".*KPI .+(LeA)$ runSet"
        config = ["runKPI", "exampleA", "runSet", "startKPI", "xxKPIxww"]

        # set expected result(s)
        expected_output = ["exampleA"]

        # Execute
        actual_output = helper_functions.return_unmatched_patterns(config=config, re_pattern=pattern)

        # Evaluate
        self.assertListEqual(expected_output, actual_output)


if __name__ == "__main__":
    unittest.main()
