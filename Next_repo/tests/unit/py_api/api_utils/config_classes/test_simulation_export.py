"""
    Copyright 2022 Continental Corporation

    :file: test_simulation_export.pytest

    :synopsis:
        Script containing unit tests for simulation export script

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
    simulation_export,
)
from tests.unit.py_api.test_helpers import format_output  # noqa: E402

# pylint: enable=import-error, wrong-import-position, ungrouped-imports


class TestExportSignal(unittest.TestCase):  # pylint: disable=too-many-instance-attributes
    """Test Class for checking ExportSignal class"""

    def __init__(self, *args, **kwargs):
        super(TestExportSignal, self).__init__(*args, **kwargs)  # pylint: disable=super-with-arguments
        # set expected result(s)
        self.run_export_file = None
        self.signals = None
        self.trigger = None
        self.export_type = None
        self.output_name = None
        self.overwrite = None
        self.export_signals = None
        self.output_config = None
        self.explicit_name = None
        self.output_path = None

    def test_export_signal_init_class_with_empty_dict_exptype_is_list(self):
        """
        Tests the initialization of the class with a valid dictionary with empty keys
        Export multiple types
        """
        # set input(s)
        signal_data = {"SimulationExportFile": "", "Signals": [], "Trigger": "", "ExportType": [], "ExplicitName": ""}

        # set expected result(s)
        self.run_export_file = ""
        self.signals = []
        self.trigger = ""
        self.export_type = []
        self.explicit_name = ""

        # execute
        export_signal = simulation_export.ExportSignal(signal_data)

        # evaluate
        self.assertEqual(export_signal.run_export_file, self.run_export_file)
        self.assertEqual(export_signal.signals, self.signals)
        self.assertEqual(export_signal.trigger, self.trigger)
        self.assertEqual(export_signal.export_type, self.export_type)
        self.assertEqual(export_signal.explicit_name, self.explicit_name)

    def test_export_signal_init_class_with_empty_dict_exptype_not_list(self):
        """
        Tests the initialization of the class with a valid dictionary with empty keys
        Export one type
        """
        # set input(s)
        signal_data = {
            "SimulationExportFile": "",
            "Signals": [],
            "Trigger": "",
            "ExportType": ".csv",
            "ExplicitName": "",
        }

        # set expected result(s)
        self.run_export_file = ""
        self.signals = []
        self.trigger = ""
        self.export_type = [".csv"]
        self.explicit_name = ""

        # execute
        export_signal = simulation_export.ExportSignal(signal_data)

        # evaluate
        self.assertEqual(export_signal.run_export_file, self.run_export_file)
        self.assertEqual(export_signal.signals, self.signals)
        self.assertEqual(export_signal.trigger, self.trigger)
        self.assertEqual(export_signal.export_type, self.export_type)
        self.assertEqual(export_signal.explicit_name, self.explicit_name)

    def test_export_signal_init_class_with_not_dict(self):
        """
        Tests the initialization of the class with not a dictionary
        """
        # set input(s)
        signal_data = []

        # set expected result(s)
        # pylint: disable=duplicate-code
        self.run_export_file = []
        self.signals = []
        self.trigger = ""
        self.export_type = []
        self.explicit_name = ""
        # pylint: enable=duplicate-code

        # execute
        export_signal = simulation_export.ExportSignal(signal_data)

        # evaluate
        self.assertTrue(type(export_signal), list)
        self.assertEqual(export_signal.run_export_file, self.run_export_file)
        self.assertEqual(export_signal.signals, self.signals)
        self.assertEqual(export_signal.trigger, self.trigger)
        self.assertEqual(export_signal.export_type, self.export_type)
        self.assertEqual(export_signal.explicit_name, self.explicit_name)

    def test_export_signal_init_class_export_valid_dict_exptype_not_list_r_one_file(self):
        """
        Tests the initialization of the class with a valid dictionary
        Export one type
        Export file exists
        If it exists, read signals and trigger from the specified file
        """
        # generate dynamic path from the current file and get back 3 paths
        path_one = os.path.normpath(
            os.path.join(
                os.path.dirname(os.path.realpath(__file__)),
                "..",
                "..",
                "test_helpers",
                "test_configs",
                "test_signals_sim_export_one.json",
            )
        )

        # set input(s)
        signal_data = {
            "SimulationExportFile": path_one,
            "Signals": [],
            "Trigger": "",
            "ExportType": ".csv",
            "ExplicitName": "",
        }

        # set expected result(s)
        self.run_export_file = path_one
        self.signals = [
            "Signal.DummyFile.one.first",
            "Signal.DummyFile.one.second",
            "Signal.DummyFile.one.third",
            "Signal.DummyFile.one.fourth",
        ]
        self.trigger = "Trigger.DummyFile.one"
        self.export_type = [".csv"]
        self.explicit_name = ""

        # execute
        export_signal = simulation_export.ExportSignal(signal_data)

        # evaluate
        self.assertEqual(export_signal.run_export_file, self.run_export_file)
        self.assertEqual(export_signal.signals, self.signals)
        self.assertEqual(export_signal.trigger, self.trigger)
        self.assertEqual(export_signal.export_type, self.export_type)
        self.assertEqual(export_signal.explicit_name, self.explicit_name)

    def test_export_signal_init_class_export_valid_dict_exptype_not_list_r_list_of_files(self):
        """
        Tests the initialization of the class with a valid dictionary
        Export one type
        Export file exists
        Give multiple files
        If it exists, read signals and trigger from the specified file
        """
        # generate dynamic path from the current file and get back 3 paths
        path_one = os.path.normpath(
            os.path.join(
                os.path.dirname(os.path.realpath(__file__)),
                "..",
                "..",
                "test_helpers",
                "test_configs",
                "test_signals_sim_export_one.json",
            )
        )

        path_two = os.path.normpath(
            os.path.join(
                os.path.dirname(os.path.realpath(__file__)),
                "..",
                "..",
                "test_helpers",
                "test_configs",
                "test_signals_sim_export_two.json",
            )
        )

        # set input(s)
        signal_data = {
            "SimulationExportFile": [path_one, path_two],
            "Signals": [],
            "Trigger": "",
            "ExportType": ".csv",
            "ExplicitName": "",
        }

        # set expected result(s)
        self.run_export_file = [path_one, path_two]
        self.signals = [
            "Signal.DummyFile.one.first",
            "Signal.DummyFile.one.second",
            "Signal.DummyFile.one.third",
            "Signal.DummyFile.one.fourth",
        ]
        self.trigger = "Trigger.DummyFile.one"
        self.export_type = [".csv"]
        self.explicit_name = ""

        # execute
        export_signal = simulation_export.ExportSignal(signal_data)

        # evaluate
        self.assertEqual(export_signal.run_export_file, self.run_export_file)
        self.assertEqual(export_signal.signals, self.signals)
        self.assertEqual(export_signal.trigger, self.trigger)
        self.assertEqual(export_signal.export_type, self.export_type)
        self.assertEqual(export_signal.explicit_name, self.explicit_name)

    def test_export_signal_init_class_input_dict_empty_keys(self):
        """
        Test the initialization of the class OutputConfig with empty input
        """
        # set input(s)
        output_data = {"OutputPath": "", "OutputName": "", "Overwrite": True}

        # set expected result(s)
        self.output_path = ""
        self.output_name = ""
        self.overwrite = True
        # execute
        output = simulation_export.OutputConfig(output_data)

        # evaluate
        self.assertEqual(output.output_path, self.output_path)
        self.assertEqual(output.output_name, self.output_name)
        self.assertEqual(output.overwrite, self.overwrite)

    def test_export_signal_init_class_input_dict(self):
        """
        Test the initialization of the class OutputConfig with input
        """
        # set input(s)
        output_data = {
            "OutputPath": "D:\\Workspace_MTS_NEXT\\NEXT_API_OUT",
            "OutputName": "next_exporter_sil_sil_test",
            "Overwrite": False,
        }

        # set expected result(s)
        self.output_path = "D:\\Workspace_MTS_NEXT\\NEXT_API_OUT"
        self.output_name = "next_exporter_sil_sil_test"
        self.overwrite = False
        # execute
        output = simulation_export.OutputConfig(output_data)

        # evaluate
        self.assertEqual(output.output_path, self.output_path)
        self.assertEqual(output.output_name, self.output_name)
        self.assertEqual(output.overwrite, self.overwrite)

    def test_export_signal_init_class_input_empty_dict_parse_conf(self):
        """
        Test the initialization of the class SimulationExport with no dictionary
        Parse config with empty dictionary
        """
        # set input(s)
        export_dict = {}

        # set expected result(s)
        self.export_signals = []
        self.output_config = None

        # execute
        output = simulation_export.SimulationExport(export_dict)

        # evaluate
        self.assertEqual(output.export_signals, self.export_signals)
        self.assertEqual(output.output_config, self.output_config)

    def test_export_signal_init_class_input_dict_parse_conf_call_exportsignal_with_list(self):
        """
        Test the initialization of the class SimulationExport with dictionary
        Parse config with correct keys dictionary
        Iterate over with key into dict keys and config into values
        Call ExportSignal with a list
        In ExportSignal it will handle the case where signal_data is not a dictionary
        In OutputConfig it will add in data members what is send as input
        """
        # set input(s)
        export_dict = {
            "ExportSignals": ["ADC4xx.EML.EmlOutput.egoVehKinematic.sigHeader.uiTimeStamp"],
            "Output": {
                "OutputPath": "D:\\Workspace_MTS_NEXT\\NEXT_API_OUT",
                "OutputName": "next_exporter_sil_sil_test",
                "Overwrite": True,
            },
        }

        # set expected result(s)
        self.export_signals = {
            "run_export_file": [],
            "signals": [],
            "trigger": "",
            "export_type": [],
            "explicit_name": "",
        }

        self.output_config = {
            "output_path": "D:\\Workspace_MTS_NEXT\\NEXT_API_OUT",
            "output_name": "next_exporter_sil_sil_test",
            "overwrite": True,
        }

        # execute
        output = simulation_export.SimulationExport(export_dict)
        exp_output = format_output.to_dict(output.export_signals)
        conf_output = format_output.to_dict(output.output_config)

        # evaluate
        self.assertEqual(exp_output, self.export_signals)
        self.assertEqual(conf_output, self.output_config)

    def test_export_signal_init_class_input_dict_no_correct_keys(self):
        """
        Test the initialization of the class SimulationExport with dictionary
        Parse config with not the correct dictionary keys
        """
        # set input(s)
        export_dict = {
            "NOT_ExportSignals": ["ADC4xx.EML.EmlOutput.egoVehKinematic.sigHeader.uiTimeStamp"],
            "NOT_Output": {
                "OutputPath": "D:\\Workspace_MTS_NEXT\\NEXT_API_OUT",
                "OutputName": "next_exporter_sil_sil_test",
                "Overwrite": True,
            },
        }

        # set expected result(s)
        self.export_signals = []
        self.output_config = None

        # execute
        output = simulation_export.SimulationExport(export_dict)

        # evaluate
        self.assertEqual(output.export_signals, self.export_signals)
        self.assertEqual(output.output_config, self.output_config)


if __name__ == "__main__":
    unittest.main()
