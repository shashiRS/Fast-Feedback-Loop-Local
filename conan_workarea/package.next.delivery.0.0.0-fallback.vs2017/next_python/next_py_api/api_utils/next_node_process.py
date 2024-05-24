"""
    Copyright 2022 Continental Corporation

    This file contains the classes to control processes with the next API

    :file: next_node_process.py

    :author:
        - Dirk Koltermann (uif75429) <dirk.koltermann@continental-corporation.com>
"""

# Standard library imports
import logging
import os
import platform
import signal
import subprocess
import sys
import threading
import time
from enum import Enum
from threading import Thread

from next_py_api.api_utils.status_code_handler import StatusCodeMessage
from next_py_api.api_variables.next_status_code import (
    ResultStatusCode,
    StatusOrigin,
    get_status_code,
)

LOG = logging.getLogger("__main__")


class CloseProcessState(Enum):  # pylint: disable=missing-class-docstring
    SUCCESS = 0
    CLOSE_PROCESS_SHUTDOWN_FAILED = 1


class StartProcessState(Enum):  # pylint: disable=missing-class-docstring
    SUCCESS = 0
    ERROR_START_PROCESS_PATH_TO_PROCESS_DOES_NOT_EXIST = 1
    ERROR_START_PROCESS_TIMEOUT = 2
    ERROR_START_PROCESS_ERROR_DURING_START_PROCESS = 3
    ERROR_START_PROCESS_PROCESS_IS_ALREADY_RUNNING = 4
    ERROR_START_PROCESS_PROCESS_IS_NOT_STARTED = 5


class NodeState(Enum):  # pylint: disable=missing-class-docstring
    NOT_RUNNING = 0
    RUNNING = 1


class NodeProcessProp:  # pylint: disable=too-few-public-methods
    """
    Class to store all needed properties for a Node Process
    """

    def __init__(  # pylint: disable=too-many-arguments, too-few-public-methods
        self,
        comp_name,
        comp_type=StatusOrigin.SIM_NODE,
        exec_dir="",
        proc_name="",
        proc_args=None,
        init_status_code=ResultStatusCode.SUCC_COMP_INIT_FINISHED,
    ):
        """
        Args:
            comp_name (str): internal component/instance name of this executable
            comp_type (enum StatusOrigin): enum specifying the type of the component
            exec_dir (str): directory of the node executable to start
            proc_name (str): name of the node executable to start
            proc_args (list): list of command line arguments for starting the process
            init_status_code (enum from ResultStatusCode): status code value which indicates that the initialization of
                                                            the process is finished
        """
        if proc_args is None:
            proc_args = []
        self.comp_name = comp_name
        self.comp_type = comp_type
        self.exec_dir = os.path.realpath(exec_dir)
        self.proc_name = proc_name
        self.proc_args = proc_args
        self.init_status_code = init_status_code


class NodeProcess:  # pylint: disable=too-many-instance-attributes
    """
    Class to control Node Processes
    """

    def __init__(  # pylint: disable=too-many-arguments
        self, status_handler, comp_name, comp_type, exec_dir, proc_name, proc_args=None, init_status_code=None
    ):
        """
        Args:
            status_handler (class status_code_handler): handler to get status codes
            comp_name (str): internal component/instance name of this executable
            comp_type (enum StatusOrigin): enum specifying the type of the component
            exec_dir (str): directory of the node executable to start
            proc_name (str): name of the node executable to start
            proc_args (list): list of command line arguments for starting the process
            init_status_code (enum from ResultStatusCode): status code value which indicates that the initialization of
                                                            the process is finished
        """
        if proc_args is None:
            proc_args = []
        self.comp_name = comp_name
        self.comp_type = comp_type
        self.exec_dir = os.path.realpath(exec_dir)
        self.proc_name = proc_name
        self.proc_args = proc_args

        self.proc_instance = None  # Subprocess

        self.status_handler = status_handler
        self.init_status_code = init_status_code
        self.id_init_status_code_callback = None
        self.condition_wait_for_process_started = threading.Condition()
        self.process_starting = False

        self.thread_check_processed_finished = None

        self.shutdown_iterations = 3
        self.iterations_check_node_started = 3

    @classmethod
    def init_by_node_process_prop(cls, status_handler, node_process_prop):
        """
        Initialize NodeProcess by NodeProcessProp
        Args:
            cls: Will be filled in automatically.
            status_handler (class status_code_handler): handler to get status codes
            node_process_prop (obj NodeProcessProp): contains node properties
        """
        return cls(
            status_handler,
            node_process_prop.comp_name,
            node_process_prop.comp_type,
            node_process_prop.exec_dir,
            node_process_prop.proc_name,
            node_process_prop.proc_args,
            node_process_prop.init_status_code,
        )

    def add_listener(self, callback, code=None):
        """
        add function callback which gets called in case of specified status_code
        Args:
            callback (func): function hook which gets called in case status_code is received
            code (enum ResultStatusCode) (optional): status code which will trigger calling of the callback
                                                    If empty, callback will be called for all status codes of
                                                    this component.
            return (int):  Internally created ID for this callback which can be used for removal.
        """
        status_code = get_status_code(self.comp_type, code)
        return self.status_handler.add_listener(callback, self.comp_name, self.comp_type, status_code)

    def remove_listener(self, code):
        """
        remove function callback(s) which get called in case of specified status_code. If several callbacks
        are registered for this status code, all will be removed.
        Args:
            code (enum ResultStatusCode): status code which will trigger calling of the callback(s)
        """
        status_code = get_status_code(self.comp_type, code)
        self.status_handler.remove_listener(StatusCodeMessage(self.comp_name, self.comp_type, status_code))

    def remove_listener_by_id(self, callback_id):
        """
        remove function callback with the specified ID which gets called in case of a specified status_code
        Args:
            callback_id (int):  id of the callback to remove
        """
        self.status_handler.remove_listener_by_id(callback_id)

    def get_state(self):
        """
        returns if process is running
        """
        if self.proc_instance is not None and self.proc_instance.poll() is None:
            return NodeState.RUNNING

        return NodeState.NOT_RUNNING

    def start_process(self, timeout=None):  # pylint: disable=too-many-return-statements
        """
        starts the given executable as a subprocess.
        Reads continuously stdout of the subprocess and updates it in the proc_output queue
        Waits till self.init_status_code code is sent (in case it's specified)
        Args:
            timeout (int): time in seconds to wait for specified init finished status code.
                           In case of timeout return with return value 2.
            return (int):   0: successful
                            1: path to process doesn't exist
                            2: timeout
                            3: error during start of the process
                            4: Process is already running
                            5: Process is not started
        """
        if NodeState.RUNNING == self.get_state():
            LOG.error(
                "[Node]::start_process: Process is still running. Close process first before calling start_process"
            )
            return StartProcessState.ERROR_START_PROCESS_PROCESS_IS_ALREADY_RUNNING

        if self.init_status_code is not None:
            # add listener hook instead of using "wait_for_code" function of status handler to be able to abort
            # immediately in case process crashed during startup
            self.id_init_status_code_callback = self.add_listener(self.__check_process_started_hook)

        path_proc = os.path.join(self.exec_dir, self.proc_name)
        if not os.path.exists(path_proc):
            LOG.error("[Node]::start_process: Path to process doesn't exist: %s. Process not started.", path_proc)
            return StartProcessState.ERROR_START_PROCESS_PATH_TO_PROCESS_DOES_NOT_EXIST

        start_command = [path_proc]
        for arg in self.proc_args:
            start_command.append(arg)

        try:
            self.process_starting = True
            orig_cwd = os.getcwd()
            # changing the directory to the folder with the executables in it
            os.chdir(self.exec_dir)
            if "Windows" in platform.platform():
                self.proc_instance = subprocess.Popen(
                    start_command, creationflags=subprocess.CREATE_NEW_CONSOLE, shell=False
                )
            elif "Linux" in platform.platform():
                # pylint: disable-next=consider-using-with
                self.proc_instance = subprocess.Popen(
                    start_command, stdout=subprocess.PIPE, shell=False, cwd=self.exec_dir
                )
            os.chdir(orig_cwd)

            self.thread_check_processed_finished = Thread(target=self.__check_process_stopped_hook)
            self.thread_check_processed_finished.start()

            start_time = time.time()
            # get notified either when process is closed or init status coded is send by the process
            with self.condition_wait_for_process_started:
                self.condition_wait_for_process_started.wait(timeout)
            self.process_starting = False

            state_running = False
            for _ in range(self.iterations_check_node_started):
                if NodeState.RUNNING == self.get_state():
                    state_running = True
                    break
                time.sleep(1)

            if not state_running:
                LOG.error(
                    "Error [Node]::start_process: Process %s is not running after trying to start it.", self.comp_name
                )
                self.close_process(3)
                return StartProcessState.ERROR_START_PROCESS_PROCESS_IS_NOT_STARTED

            if timeout is not None and time.time() - start_time > timeout:
                LOG.error(
                    "Error [Node]::start_process: %s: Timeout occurred for: %s. Process not started.",
                    self.comp_name,
                    path_proc,
                )
                self.close_process(3)
                return StartProcessState.ERROR_START_PROCESS_TIMEOUT

        except OSError as e:
            LOG.error("ERROR [Node]::start_process: %s: OSError errno: %s", self.comp_name, e.errno)
            LOG.error("ERROR [Node]::start_process: %s: OSError strerror: %s", self.comp_name, e.strerror)
            LOG.error("ERROR [Node]::start_process: %s: OSError filename: %s", self.comp_name, e.filename)
            self.close_process(3)
            return StartProcessState.ERROR_START_PROCESS_ERROR_DURING_START_PROCESS
        except Exception:  # noqa: E722 # pylint:disable=broad-exception-caught
            LOG.error("ERROR [Node]::start_process: %s: %s", self.comp_name, sys.exc_info()[0])
            self.close_process(3)
            return StartProcessState.ERROR_START_PROCESS_ERROR_DURING_START_PROCESS

        return StartProcessState.SUCCESS

    def start_process_extended_return_infos(self, timeout=None):
        """
        starts the given executable as a subprocess.
        Reads continuously stdout of the subprocess and updates it in the proc_output queue
        Waits till self.init_status_code code is sent (in case it's specified)
        Return the status and the process name as a tuple
        Args:
            timeout (int): time in seconds to wait for specified init finished status code.
                           In case of timeout return with return value 2.
            return (int, comp_name):    0: successful
                                        1: path to process doesn't exist
                                        2: timeout
                                        3: error during start of the process
                                        4: Process is already running
                                        5: Process is not started
        """
        ret = self.start_process(timeout)
        return ret, self.comp_name

    def close_process(self, timeout=3):
        """
        Closes the given subprocess.
        First tries gracefully, then sends kill command
        Args:
            timeout (int): time in second to wait for process to close with graceful shutdown
            return (int): 0: successful,  1: shutdown failed
        """
        if self.init_status_code is not None:
            if None is not self.status_handler and None is not self.id_init_status_code_callback:
                self.remove_listener_by_id(self.id_init_status_code_callback)
                self.id_init_status_code_callback = None

        if None is self.proc_instance:
            LOG.info("[Node]::close_process: Process %s already terminated.", self.proc_name)
            return CloseProcessState.SUCCESS

        try:
            if hasattr(signal, "CTRL_C_EVENT"):
                self.proc_instance.send_signal(signal.CTRL_C_EVENT)
            else:
                self.proc_instance.send_signal(signal.SIGINT)
            self.proc_instance.communicate(timeout=timeout)
        except subprocess.TimeoutExpired:
            LOG.info(
                "[Node]::close_process: Graceful shutdown of process %s not successful, going to terminate",
                self.proc_name,
            )
            self.proc_instance.terminate()
            if self.proc_instance.poll() is not None:
                LOG.info("killing ControlBridge")
                self.proc_instance.kill()
        except ValueError:
            LOG.info("[Node]::close_process: Process %s already terminated.", self.proc_name)

        state_running = True
        # check if process is stopped
        for _ in range(self.shutdown_iterations):
            if NodeState.NOT_RUNNING == self.get_state():
                state_running = False
                break
            time.sleep(1)

        if state_running:
            LOG.error("[Node]::close_process: Shutdown node %s failed.", self.proc_name)
            self.proc_instance = None
            return CloseProcessState.CLOSE_PROCESS_SHUTDOWN_FAILED

        self.proc_instance = None

        return StartProcessState.SUCCESS

    def restart_process(self, timeout_start=None, timeout_shutdown=3):
        """
        Shutdown and start process again
        First tries gracefully, then sends kill command
        Args:
            timeout_start (int): time in seconds to wait for specified init finished status code.
            timeout_shutdown (int): time in second to wait for process to close with graceful shutdown
            return (int): 0: successful,  else: start process failed
        """
        self.close_process(timeout_shutdown)
        return self.start_process(timeout_start)

    def get_proc_instance(self):
        """
        Args:
            return: created instance of this process
        """
        return self.proc_instance

    # needed for status handler, might be used later
    # pylint: disable-next=unused-argument
    def __check_process_started_hook(self, status_code_obj):
        """
        Hook which can be added to the StatusHandler listener to get called if a certain status code was received.
        Checks if the status code is from the expected component and notify condition variable in this case
        """
        if self.process_starting is True:
            with self.condition_wait_for_process_started:
                self.condition_wait_for_process_started.notify_all()

    def __check_process_stopped_hook(self):
        """
        Function which can be used to notify the condition variable in case the process stopped
        """
        self.proc_instance.communicate()
        with self.condition_wait_for_process_started:
            self.condition_wait_for_process_started.notify_all()
