"""
    Copyright 2022 Continental Corporation

    This file contains a class providing executions commands for NEXT.
    It provides methods to:
        Load a recording
        Start the player
        Pause the player
        Jump to beginning of recording
        Start the exporter
        Stop the exporter
        Jump to timestamp

    :file: next_execution.py

    :author:
        - Jannik Fritz (uidr9189) <jannik.fritz@continental-corporation.com>
"""

# Standard library imports
import logging

from next_py_api.api_utils import time_measurement

# Local application imports
from next_py_api.api_variables import global_vars
from next_py_api.api_variables import next_execution_vars as exec_var
from next_py_api.api_variables.next_status_code import (
    ResultStatusCode,
    StatusOrigin,
    get_status_code,
)

LOG = logging.getLogger("__main__")


class NextExecution:
    """Class representing the Next executions"""

    def __init__(self, next_websocket, status_handler):
        self.websocket = next_websocket
        self.time_meas = time_measurement.TimeMeasurement()
        self.available_commands = [
            "load",
            "play",
            "pause",
            "jump_to_beginning",
            "jump_to_timestamp",
            "play_until",
            "start_export",
            "stop_export",
            "set_synchronised_mode",
        ]
        self.status_handler = status_handler

    def load_recording(self, file: str, timeout=60, get_response=True, number_of_start_tries=3):
        """
        Load the recording
        Args:
            file (str): The path to the recording
            timeout (int): Time in seconds to wait for a response
            get_response (bool): send the websocket response
        Returns:
            return the message of load recording response from player
        """
        self.measure_time("load_recording")
        check_websocket_connection = self.check_websocket_connection(numbers_of_tries=2)
        if check_websocket_connection:
            LOG.info("Websocket connection is working")
        msg = exec_var.get_load_recording_msg(file)
        LOG.debug("Recording Name: %s", file)
        for it in range(number_of_start_tries):
            self.websocket.controlbridge.send(msg=msg, wait_response=False)
            status_code_start_load_recording = get_status_code(
                StatusOrigin.NEXT_PLAYER, ResultStatusCode.INFO_START_LOAD_RECORDING
            )
            ret_start_load_recording, message_start_load_recording = self.status_handler.wait_for_code(
                status_code=status_code_start_load_recording, timeout=timeout, consider_history=True
            )
            if ret_start_load_recording:
                LOG.info("Start load the recording status code received")
                LOG.info(message_start_load_recording.status_message)
                break
            if number_of_start_tries - 1 == it:
                LOG.warning(
                    "Failed to start to load the recording. Maximum number of tries %s exceeds", number_of_start_tries
                )
                return False
        returned_msg = self.websocket.controlbridge.wait_until_recv(
            receive_channel=exec_var.RECEIVE_CHANNEL_PLAYER,
            expected_event=exec_var.EXPECTED_EVENT_RECORDING_LOADED,
            get_response=get_response,
            time_out=timeout,
        )
        if returned_msg:
            if "payload" in returned_msg:
                if "currentTimestamp" in returned_msg["payload"]:
                    global_vars.start_timestamp_us = returned_msg["payload"]["currentTimestamp"]
                    global_vars.current_timestamp_us = returned_msg["payload"]["currentTimestamp"]
                if "endTimeStamp" in returned_msg["payload"]:
                    global_vars.end_timestamp_us = returned_msg["payload"]["endTimeStamp"]

        status_code_load_recording = get_status_code(StatusOrigin.NEXT_PLAYER, ResultStatusCode.SUCC_RECORDING_LOADED)
        ret_load_recording, message_load_recording = self.status_handler.wait_for_code(
            status_code=status_code_load_recording, timeout=timeout, consider_history=True
        )

        if ret_load_recording:
            LOG.info("Recording is loaded")
            LOG.info(message_load_recording.status_message)
        else:
            LOG.error("Failed to load the recording")
            return False
        self.measure_time("load_recording")
        return returned_msg

    # Load configuration after the controlbridge is started
    def load_backend_config(self, file: str, timeout=60):
        """
        Load backend config
        Args:
            file (str): The path to the backend configuration
            timeout (int): Time in seconds to wait for a response
        Returns:
            return the message of load configuration response
        """
        msg = exec_var.get_load_configuration_msg(file)
        LOG.debug("Backend configuration Path: %s", file)
        LOG.debug("Load Backend configuration command: %s", msg)
        return self.websocket.controlbridge.send(
            msg, expected_event=exec_var.EXPECTED_EVENT_LOAD_CONFIGURATION, timeout=timeout
        )

    def play_recording(self, blocking=False, timeout=60, get_response=True):
        """
        Play the recording
        Args:
            blocking (bool): if it's true play until the end of file
            timeout (int): Time in seconds to wait for a response
            get_response (bool): send the websocket response
        Returns:
            return the message of play recording response if run successfully
            return False if timeout or errors occurs
        """
        if blocking:
            if self.websocket.controlbridge.send(
                msg=exec_var.CONTROLBRIDGE_PLAY_RECORDING,
                expected_event=exec_var.EXPECTED_EVENT_PLAYBACK_PLAYING,
                receive_channel=exec_var.RECEIVE_CHANNEL_PLAYER,
                get_response=False,
            ):
                LOG.info("Replay started.")
                return self.wait_until_EOF(get_response=get_response, timeout=timeout)
            return False
        return self.websocket.controlbridge.send(
            msg=exec_var.CONTROLBRIDGE_PLAY_RECORDING,
            expected_event=exec_var.EXPECTED_EVENT_PLAYBACK_PLAYING,
            receive_channel=exec_var.RECEIVE_CHANNEL_PLAYER,
            get_response=get_response,
            timeout=timeout,
        )

    def wait_until_EOF(self, timeout=60, get_response=True):  # pylint: disable=C0103
        """
        Wait until end of file or timeout raised
        Args:
            timeout (int): Time in seconds to wait for a response
            get_response (bool): send the websocket response
        Returns:
            return the message of paused/finished to play until the end of recording response if run successfully
            return False if timeout or errors occurs
        """
        LOG.info("Wait until EOF or timeout after %s seconds.", timeout)
        returned_msg = self.websocket.controlbridge.wait_until_recv(
            receive_channel=exec_var.RECEIVE_CHANNEL_PLAYER,
            expected_event=exec_var.EXPECTED_EVENT_PLAYBACK_PAUSED,
            get_response=get_response,
            time_out=timeout,
        )
        if returned_msg:
            if "payload" in returned_msg:
                if "currentTimestamp" in returned_msg["payload"]:
                    global_vars.current_timestamp_us = returned_msg["payload"]["currentTimestamp"]
        return returned_msg

    def pause_recording(self, timeout=60, get_response=True):
        """
        Play the recording
        Args:
            timeout (int): Time in seconds to wait for a response
            get_response (bool): send the websocket response
        Returns:
            return the message of pause the recording response if run successfully
            return False if timeout or errors occurs
        """
        return self.websocket.controlbridge.send(
            msg=exec_var.CONTROLBRIDGE_PAUSE_RECORDING,
            expected_event=exec_var.EXPECTED_EVENT_PLAYBACK_PAUSED,
            receive_channel=exec_var.RECEIVE_CHANNEL_PLAYER,
            get_response=get_response,
            timeout=timeout,
        )

    def jump_to_beginning(self, timeout=60, get_response=True):
        """
        Jump to a beginning of the recording
        Args:
            get_response (bool): send the websocket response
            timeout (int): Time in seconds to wait for a response

        Returns:
            return the message of jump to beginning of the recording response if run successfully
            return False if timeout or errors occurs
        """
        msg_received = self.websocket.controlbridge.send(
            exec_var.CONTROLBRIDGE_JUMP_TO_BEGINNING,
            expected_event=exec_var.EXPECTED_EVENT_JUMP_TO_BEGINNING,
            receive_channel=exec_var.RECEIVE_CHANNEL_PLAYER,
            get_response=get_response,
            timeout=timeout,
        )
        if msg_received:
            global_vars.current_timestamp_us = global_vars.start_timestamp_us
        return msg_received

    def start_exporter(
        self, signals: list, trigger: str, extension: str, filepath="", source_name="NextExporter-0", timeout=60
    ):  # pylint: disable=too-many-arguments
        """
        Start the exporter
        Args:
            signals (list): list of the signals
            trigger (str): trigger signal used
            extension (str): extension of the output file
            filepath (str): output location
            source_name (str): source name of the exporter.
            timeout (int): Time in seconds to wait for a response

        Returns:
            return True if the exporter started
            return False if something going wrong
        """
        # Disable PackageDrop, Enable DataSynchronization
        if not self.set_synchronised_mode("false", "true"):
            return False

        # send message
        msg = exec_var.get_start_export_msg(
            signal_list=signals,
            trigger_url=trigger,
            file_format=extension,
            output_folder=filepath,
            source_name=source_name,
        )
        LOG.debug("Start export message: %s", msg)
        self.websocket.databridge.send(msg, wait_response=False)

        status_code_full_sub_start = get_status_code(
            StatusOrigin.NEXT_DATABRIDGE, ResultStatusCode.SUCC_EXPORTER_STARTED
        )
        status_code_partial_sub_start = get_status_code(
            StatusOrigin.NEXT_DATABRIDGE, ResultStatusCode.WARN_EXPORTER_PARTIAL_SUBSCRIPTION
        )
        if ".rec" in extension:
            comp_name = f"system_RecExporter::{source_name}"
        else:
            comp_name = f"system_SignalExporter::{source_name}"
        ret_full_sub_start, msg_full_sub = self.status_handler.wait_for_code(
            status_code=status_code_full_sub_start,
            comp_name=comp_name,
            timeout=timeout,
            consider_history=True,
        )

        if ret_full_sub_start:
            LOG.info("[NextExecutions]::start_exporter: Status code SUCC_EXPORTER_STARTED received.")
            LOG.info(msg_full_sub.status_message)
            return True
        ret_partial_sub_start, msg_partial_sub = self.status_handler.wait_for_code(
            status_code=status_code_partial_sub_start,
            comp_name=comp_name,
            timeout=timeout,
            consider_history=True,
        )
        if ret_partial_sub_start:
            LOG.info("[NextExecutions]::start_exporter: Status code WARN_EXPORTER_PARTIAL_SUBSCRIPTION received.")
            LOG.info(msg_partial_sub.status_message)
            return True
        LOG.error("[NextExecutions] Failed to start exporting.")
        return False

    def stop_exporter(self, source_name="NextExporter-0", timeout=60):
        """
        Stop the exporter
        Args:
            source_name (str): source name of the exporter.
            timeout (int): Time in seconds to wait for a response

        Returns:
            return True if the exporter started
            return False if something going wrong
        """
        # send message
        msg = exec_var.get_stop_export_msg(source_name=source_name)
        self.websocket.databridge.send(msg, wait_response=False)

        status_code_export_stopped = get_status_code(
            StatusOrigin.NEXT_DATABRIDGE, ResultStatusCode.SUCC_EXPORTER_STOPPED
        )

        ret_export_stopped, message_exporter = self.status_handler.wait_for_code(
            status_code=status_code_export_stopped, timeout=timeout, consider_history=True
        )

        if ret_export_stopped:
            LOG.info("[NextExecutions]::start_exporter: Status code SUCC_EXPORTER_STOPPED received.")
            LOG.info(message_exporter.status_message)
            return True
        LOG.error("[NextExecutions] Failed to stop exporting.")
        return False

    def set_synchronised_mode(self, allow_package_drop: str, enable_data_synchronization: str, timeout=60):
        """
        Set the synchronised mode
        Args:
            allow_package_drop (str): flag to enable/disable package drop true/false
            enable_data_synchronization (str): flag to enable/disable data synchronization
            timeout (int): Time in seconds to wait for a response

        Returns:
            return True if succeed
            return False if not succeed
        """
        # send message
        msg = exec_var.get_synchronised_mode_msg(allow_package_drop, enable_data_synchronization)
        self.websocket.databridge.send(msg, wait_response=False)

        status_code_backend_config_finished = get_status_code(
            StatusOrigin.NEXT_DATABRIDGE, ResultStatusCode.SUCC_SETUP_BACKEND_CONFIG_FINISHED
        )
        # wait till set sync mode is set in the databridge
        ret_backend_config_finished, message_backend_config = self.status_handler.wait_for_code(
            status_code=status_code_backend_config_finished, timeout=timeout, consider_history=True
        )
        if ret_backend_config_finished:
            LOG.info("[NextExecutions]::start_exporter: Status code SUCC_SETUP_BACKEND_CONFIG_FINISHED received.")
            LOG.info(message_backend_config.status_message)
        else:
            LOG.error("[NextExecutions] Failed to setup backend config.")
            return False

        return True

    def jump_to_timestamp(self, timestamp: int, timeout=60, get_response=True):
        """
        Jump to a specific timestamp.
        Args:
            timestamp (int): The timestamp to jump to.
            get_response (bool): send the websocket response
            timeout (int): Time in seconds to wait for a response

        Returns:
            None
        """
        if timestamp < global_vars.start_timestamp_us:
            LOG.warning(
                "([jump_to_timestamp] Given timestamp (%s) is smaller than start timestamp "
                "(%s). This might cause unwanted behavior in the simulation.",
                timestamp,
                timestamp,
            )
        msg = exec_var.get_jump_to_timestamp_msg(timestamp)

        returned_msg = self.websocket.controlbridge.send(
            msg=msg,
            expected_event=exec_var.EXPECTED_EVENT_JUMP_TO_TIMESTAMP,
            receive_channel=exec_var.RECEIVE_CHANNEL_PLAYER,
            get_response=get_response,
            timeout=timeout,
        )
        return returned_msg

    def play_until(self, timestamp: int):
        """
        Play until a specific timestamp is passed.
        Args:
            timestamp (int): The timestamp played.

        Returns:
            None

        Note:
        """
        if global_vars.end_timestamp_us > 0:
            if timestamp > global_vars.end_timestamp_us:
                LOG.warning(
                    "[play_until] Given timestamp (%s) is bigger than end timestamp "
                    "(%s). This might cause unwanted behavior in the simulation.",
                    timestamp,
                    global_vars.end_timestamp_us,
                )

        msg = exec_var.get_jump_to_timestamp_msg(timestamp, stream_frames_in_between="true")
        returned_msg = self.websocket.controlbridge.send(
            msg,
            expected_event=exec_var.EXPECTED_EVENT_PLAYBACK_PLAYING,
            receive_channel=exec_var.RECEIVE_CHANNEL_PLAYER,
            timeout=60,
        )
        return returned_msg

    def close_recording(self, timeout=60, get_response=True):
        """
        Closes the recording.

        Returns:
            None

        """
        return_msg = self.websocket.controlbridge.send(
            exec_var.CONTROLBRIDGE_CLOSE_RECORDING,
            expected_event=exec_var.EXPECTED_EVENT_RECORDING_CLOSED,
            expected_payload=None,
            receive_channel=exec_var.RECEIVE_CHANNEL_PLAYER,
            get_response=get_response,
            timeout=timeout,
        )
        return return_msg

    def check_signals(self, signals: list):
        """
        Check if the specified signals are part of the recording.

        Args:
            signals (list): A list of signals to check.

        Returns:
            bool: True if all signals are part of the recording, False otherwise.
        """
        # once status code events are added check for that
        self.websocket.databridge_broadcast.dump_cache()
        self.websocket.databridge.send(
            exec_var.DATABRIDGE_UPDATE_SIGNALS,
            wait_response=False,
        )

        response = self.websocket.databridge_broadcast.wait_until_recv(
            expected_event=exec_var.EXPECTED_EVENT_UPDATING_SIGNALS, get_response=True
        )
        payload = response["payload"]
        if not payload:
            LOG.error("[check_signals]: No payload received")
            return False

        for signal in signals:
            # TODO: remove after fix for search issue #pylint: disable=fixme
            formatted_signal = exec_var.remove_device_and_view(signal)
            message = exec_var.get_search_signal_msg(formatted_signal)
            self.websocket.databridge.send(message, wait_response=False)

            response = self.websocket.databridge_broadcast.wait_until_recv(
                expected_event=exec_var.EXPECTED_EVENT_UPDATING_SIGNALS, get_response=True
            )
            try:
                signal_id = response["payload"]["dataStreamSchema"][0]["id"]
                if signal_id != signal:
                    LOG.error("[check_signals]: %s is not equal with %s", signal_id, signal)
                    LOG.error("[check_signals]: IndexError exception. Response payload: %s", response["payload"])
                    return False
            except IndexError:
                return False
        return True

    def wait_playback_paused_event_and_update_current_timestamp(self):
        """
        Wait until the playback_paused event is received and update the current timestamp variable

        Args:
            self
        Returns:
            None

        """

        msg_recv = self.websocket.controlbridge.wait_until_recv(
            expected_event=exec_var.EXPECTED_EVENT_PLAYBACK_PAUSED,
            receive_channel=exec_var.RECEIVE_CHANNEL_PLAYER,
            get_response=True,
            time_out=60,
        )
        if msg_recv:
            if "payload" in msg_recv:
                if "currentTimestamp" in msg_recv["payload"]:
                    global_vars.current_timestamp_us = msg_recv["payload"]["currentTimestamp"]

    def check_websocket_connection(self, timeout=30, numbers_of_tries=1):
        """
        Check if the websocket connection is working
        Args:
            timeout (int): Time in seconds to wait for a response
            numbers_of_tries (int): Number of tries to check if websocket connection is working
        Returns:
            None

        """
        msg = exec_var.CONTROLBRIDGE_CHECK_WEBSOCKET_CONNECTION
        for _ in range(numbers_of_tries):
            self.websocket.controlbridge.send(msg=msg, get_response=False)
            status_code_get_websocket_connection = get_status_code(
                StatusOrigin.NEXT_CONTROLBRIDGE, ResultStatusCode.SUCC_RECEIVED_CHECK_WEBSOCKET_CONNECTION
            )
            ret_full_check_websocket, msg_check_websocket = self.status_handler.wait_for_code(
                status_code=status_code_get_websocket_connection, timeout=timeout, consider_history=True
            )
            if ret_full_check_websocket:
                LOG.info("Check Websocket connection message received")
                LOG.info(msg_check_websocket.status_message)
                return True
        LOG.error("Failed to check the websocket connection")
        return False

    def measure_time(self, name):
        """
        Measure time execution

        Args:
            name: name of the execution step
        Returns:
            return the measured time
        """
        return self.time_meas.measure_time(name)
