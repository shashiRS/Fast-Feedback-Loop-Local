"""
    Copyright 2022 Continental Corporation

    This file contains classes providing executions commands for NEXT.

    :file: next_execution_classes.py

    :author:
        - Jannik Fritz (uidr9189) <jannik.fritz@continental-corporation.com>
"""

import logging
import os

# pylint: disable=import-error
from next_py_api.api_utils.helper_classes.helper_functions import convert_to_int
from next_py_api.api_variables import global_vars, next_execution_vars, next_status_code

# pylint: enable=import-error
LOG = logging.getLogger("__main__")


# pylint: disable=too-many-arguments
class ExecutionStep:
    """
    Parent class for all Execution Steps
    """

    def __init__(self, name=""):
        self.name = name
        self.next_execution = None
        self.process_handler = None

    def get_name(self):
        """
        Returns the name of the function to be executed
        :return: Name of class
        :rtype: str
        """
        return self.name

    def invoke(self, args, recording_batch, sim_nodes, export_settings, run_input_settings):
        """
        Function to be called. Will execute the actual code or only the dryrun if selected
        :param args: Arguments coming from the config
        :type args: any
        :param recording_batch: All Recordings available in this config
        :type recording_batch: [list of] str
        :param sim_nodes: List of Simulation Nodes that are selected in the flow
        :type sim_nodes: SimulationNodes
        :param export_settings: List of Exporter settings
        :type export_settings: SimulationExport
        :param run_input_settings: List of Recordings that shall be loaded
        :type run_input_settings: SimulationInputs
        """
        if global_vars.dryrun:
            self.invoke_dryrun(args, recording_batch, sim_nodes, export_settings, run_input_settings)
        else:
            self.invoke_step(args, recording_batch, sim_nodes, export_settings, run_input_settings)

    # pylint: disable-next=unused-argument
    def invoke_dryrun(self, args, recording_batch, sim_nodes, export_settings, run_input_settings):
        """
        Default function if dryrun is selected, will print the name of the execution

        :param args: Arguments coming from the config
        :type args: any
        :param recording_batch: All Recordings available in this config
        :type recording_batch: [list of] str
        :param sim_nodes: List of Simulation Nodes that are selected in the flow
        :type sim_nodes: SimulationNodes
        :param export_settings: List of Exporter settings
        :type export_settings: SimulationExport
        :param run_input_settings: List of Recordings that shall be loaded
        :type run_input_settings: SimulationInputs
        """
        LOG.info("Execution Step: %s", self.get_name())

    # pylint: disable-next=unused-argument
    def invoke_step(self, args, recording_batch, sim_nodes, export_settings, run_input_settings):
        """
        Default function for each execution step. Default will just pass.

        :param args: Arguments coming from the config
        :type args: any
        :param recording_batch: All Recordings available in this config
        :type recording_batch: [list of] str
        :param sim_nodes: List of Simulation Nodes that are selected in the flow
        :type sim_nodes: SimulationNodes
        :param export_settings: List of Exporter settings
        :type export_settings: SimulationExport
        :param run_input_settings: List of Recordings that shall be loaded
        :type run_input_settings: SimulationInputs
        """


class PlayRecording(ExecutionStep):
    """
    Wrapper class for the play_recording execution step
    """

    def __init__(self):
        ExecutionStep.__init__(self, "play_recording")

    def invoke_step(self, args, recording_batch, sim_nodes, export_settings, run_input_settings):
        if run_input_settings[0].time_intervals:
            time_intervals = run_input_settings[0].time_intervals
            count_number_of_iterations = 0
            for it_time_interval in time_intervals:
                if count_number_of_iterations > 0:
                    # TO DO : check if new export file should take? parameter of wait_until_EOF?
                    # self.next_execution.wait_until_EOF(300)
                    # LOG.info("Start executing the iteration number {count_number_of_iterations} of time interval")
                    # LOG.info("Start executing jump_to_beginning command")
                    # self.next_execution.jump_to_beginning()
                    break
                if isinstance(it_time_interval.start_time, int):
                    LOG.info("Start executing jump_to_timestamp with parameter: %s", it_time_interval.start_time)
                    self.next_execution.jump_to_timestamp(it_time_interval.start_time)
                    self.next_execution.wait_playback_paused_event_and_update_current_timestamp()
                if isinstance(it_time_interval.end_time, int):
                    LOG.info("Start executing play_until with parameter: %s", it_time_interval.end_time)
                    self.next_execution.play_until(it_time_interval.end_time)
                else:
                    self.next_execution.play_recording()
                count_number_of_iterations += 1
        else:
            self.next_execution.play_recording()


class LoadRecording(ExecutionStep):
    """
    Wrapper class for the load_recording execution step
    """

    def __init__(self):
        ExecutionStep.__init__(self, "load_recording")

    def invoke_step(self, args, recording_batch, sim_nodes, export_settings, run_input_settings):
        if len(recording_batch) > 1:
            # Load multiple files in parallel
            pass
        elif len(recording_batch) == 1:
            if isinstance(args, int):
                self.next_execution.load_recording(recording_batch[0], args)
            else:
                self.next_execution.load_recording(recording_batch[0])

    def invoke_dryrun(self, args, recording_batch, sim_nodes, export_settings, run_input_settings):
        super().invoke_dryrun(args, recording_batch, sim_nodes, export_settings, run_input_settings)
        LOG.info("File: %s", recording_batch)


class LoadBackendConfig(ExecutionStep):
    """
    Wrapper class for the load_backend_config execution step
    """

    def __init__(self):
        ExecutionStep.__init__(self, "load_backend_config")

    def invoke_dryrun(self, args, recording_batch, sim_nodes, export_settings, run_input_settings):
        super().invoke_dryrun(args, recording_batch, sim_nodes, export_settings, run_input_settings)
        if isinstance(args, list):
            LOG.info("Backend Config Dir: %s, Timeout: %s", args[0], args[1])
        else:
            LOG.info("Backend Config Dir: %s", args[0])

    def invoke_step(self, args, recording_batch, sim_nodes, export_settings, run_input_settings):
        if isinstance(args, list):
            self.next_execution.load_backend_config(args[0], args[1])
        else:
            self.next_execution.load_backend_config(args)


class WaitUntilEOF(ExecutionStep):
    """
    Wrapper class for the wait_until_end_of_file execution step
    """

    def __init__(self):
        ExecutionStep.__init__(self, "wait_until_end_of_file")

    def invoke_step(self, args, recording_batch, sim_nodes, export_settings, run_input_settings):
        self.next_execution.wait_until_EOF(convert_to_int(args))


class PauseRecording(ExecutionStep):
    """
    Wrapper class for the pause execution step
    """

    def __init__(self):
        ExecutionStep.__init__(self, "pause")

    def invoke_step(self, args, recording_batch, sim_nodes, export_settings, run_input_settings):
        self.next_execution.pause_recording()


class JumpToBeginning(ExecutionStep):
    """
    Wrapper class for the jump_to_beginning execution step
    """

    def __init__(self):
        ExecutionStep.__init__(self, "jump_to_beginning")

    def invoke_step(self, args, recording_batch, sim_nodes, export_settings, run_input_settings):
        self.next_execution.jump_to_beginning()


class StartExporter(ExecutionStep):
    """
    Wrapper class for the start_export execution step
    """

    def __init__(self):
        ExecutionStep.__init__(self, "start_export")

    def invoke_step(self, args, recording_batch, sim_nodes, export_settings, run_input_settings):
        exporter_source_index = 0
        while exporter_source_index in next_execution_vars.EXPORTER_SOURCE_NAME_LIST:
            exporter_source_index += 1

        for setting in export_settings[0].export_signals:
            export_path = os.path.join(
                export_settings[0].output_config.output_path, export_settings[0].output_config.output_name
            )
            if setting.explicit_name:
                export_path += setting.explicit_name
            for filetype in setting.export_type:
                # only if is an output_name or an explicit name the filetype should be added
                if export_settings[0].output_config.output_name:
                    export_path += filetype
                exporter_source_name = "NextExporter-" + str(exporter_source_index)
                exporter_source_index += 1
                LOG.debug(
                    (
                        "Start exporter with the next parameters: \n source_name: %s\nexport_path: %s",
                        "\nfiletype: %s",
                        exporter_source_name,
                        export_path,
                        filetype,
                    )
                )
                is_success = self.next_execution.start_exporter(
                    signals=setting.signals,
                    trigger=setting.trigger,
                    extension=filetype,
                    filepath=export_path,
                    source_name=exporter_source_name,
                )
                if is_success:
                    next_execution_vars.EXPORTER_SOURCE_NAME_LIST.append((exporter_source_name, filetype))

    def invoke_dryrun(self, args, recording_batch, sim_nodes, export_settings, run_input_settings):
        super().invoke_dryrun(args, recording_batch, sim_nodes, export_settings, run_input_settings)
        for setting in export_settings[0].export_signals:
            export_path = os.path.join(
                export_settings[0].output_config.output_path, export_settings[0].output_config.output_name
            )
            if setting.explicit_name:
                export_path += setting.explicit_name
            for filetype in setting.export_type:
                LOG.info("Signals: %s", setting.signals)
                LOG.info("Trigger: %s", setting.trigger)
                LOG.info("Output Location: %s%s", export_path, filetype)


class StopExporter(ExecutionStep):
    """
    Wrapper class for the stop_export execution step
    """

    def __init__(self):
        ExecutionStep.__init__(self, "stop_export")

    def invoke_step(self, args, recording_batch, sim_nodes, export_settings, run_input_settings):
        exporter_source_name_list_copy = next_execution_vars.EXPORTER_SOURCE_NAME_LIST.copy()
        for i_exporter_source_name in exporter_source_name_list_copy:
            if len(i_exporter_source_name) == 2:
                source_name = i_exporter_source_name[0]  # source_name
                extension_type = i_exporter_source_name[1]  # export_type
            else:
                LOG.warning(
                    "Failed to stop the exporter. Issue with the exporter_source_name_list_copy. "
                    "Length of the element of list: %s"
                    "Content of the element of list: %s",
                    len(i_exporter_source_name),
                    i_exporter_source_name,
                )
                return False
            is_success = self.next_execution.stop_exporter(source_name, extension_type)
            if is_success:
                next_execution_vars.EXPORTER_SOURCE_NAME_LIST.remove(i_exporter_source_name)
                return True
        if next_execution_vars.EXPORTER_SOURCE_NAME_LIST:
            LOG.warning(
                "Not all exporters are stopped. The next are still open: %s",
                next_execution_vars.EXPORTER_SOURCE_NAME_LIST,
            )
        return False


class JumpToTimestamp(ExecutionStep):
    """
    Wrapper class for the jump_to_timestamp execution step
    """

    def __init__(self):
        ExecutionStep.__init__(self, "jump_to_timestamp")

    def invoke_step(self, args, recording_batch, sim_nodes, export_settings, run_input_settings):
        self.next_execution.jump_to_timestamp(convert_to_int(args))
        self.next_execution.wait_playback_paused_event_and_update_current_timestamp()


class PlayUntil(ExecutionStep):
    """
    Wrapper class for the play_until execution step
    """

    def __init__(self):
        ExecutionStep.__init__(self, "play_until")

    def invoke_step(self, args, recording_batch, sim_nodes, export_settings, run_input_settings):
        self.next_execution.play_until(convert_to_int(args))


class CloseRecording(ExecutionStep):
    """
    Wrapper class for the close_recording execution step
    """

    def __init__(self):
        ExecutionStep.__init__(self, "close_recording")

    def invoke_step(self, args, recording_batch, sim_nodes, export_settings, run_input_settings):
        self.next_execution.close_recording()


class CheckSignals(ExecutionStep):
    """
    Wrapper class for the check_signals execution step
    """

    def __init__(self):
        ExecutionStep.__init__(self, "check_signals")

    def invoke_step(self, args, recording_batch, sim_nodes, export_settings, run_input_settings):
        self.next_execution.check_signals(args)


class StartNodes(ExecutionStep):
    """
    Wrapper class for the start_nodes execution step
    """

    def __init__(self):
        ExecutionStep.__init__(self, "start_nodes")

    def invoke_dryrun(self, args, recording_batch, sim_nodes, export_settings, run_input_settings):
        super().invoke_dryrun(args, recording_batch, sim_nodes, export_settings, run_input_settings)
        for node in sim_nodes[0].nodes:
            LOG.info("    Executable: %s", node.executable)
            LOG.info("    location: %s", node.executable_path)

    def invoke_step(self, args, recording_batch, sim_nodes, export_settings, run_input_settings):
        # shallow copy node names
        existing_sim_node_names = dict.fromkeys(self.process_handler.get_all_sim_nodes().keys(), [])
        for node in sim_nodes[0].nodes:
            comp_name, _ = os.path.splitext(node.executable)
            if comp_name.find("ecal_swc_", 0, 8):
                comp_name = comp_name[9:]
            # add node to process handler
            if comp_name not in existing_sim_node_names:
                self.process_handler.add_sim_node(
                    comp_name,
                    next_status_code.StatusOrigin.SIM_NODE,
                    node.executable_path,
                    node.executable,
                    proc_args=["-n"],
                    init_status_code=next_status_code.ResultStatusCode.SUCC_COMP_INIT_FINISHED,
                )
            else:
                existing_sim_node_names.pop(comp_name, None)

        for comp_name in existing_sim_node_names:
            self.process_handler.remove_sim_node(comp_name)

        # start nodes
        self.process_handler.start_all_sim_nodes(timeout=600)


class MeasureTime(ExecutionStep):
    """
    Wrapper class for the measure_time execution step
    """

    def __init__(self):
        ExecutionStep.__init__(self, "measure_time")

    def invoke_step(self, args, recording_batch, sim_nodes, export_settings, run_input_settings):
        self.next_execution.measure_time(args)


# pylint: disable=too-many-arguments
