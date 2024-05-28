"""
    Copyright 2022 Continental Corporation

    This file contains the next class to interact with the NEXT backend.
    It provides methods to:
        Start Backend Processes
        Start Simulation Nodes
        Start Websockets to Backend
        Communicate with the backend

    :file: next_api.py

    :author:
        - Jannik Fritz (uidr9189) <jannik.fritz@continental-corporation.com>
"""

# Standard library imports
import argparse
import datetime
import logging
import logging.handlers
import os.path
import sys
import time
import traceback
# parent_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
# print(parent_dir)

# Local application imports
from api_utils import (
    flow_processor,
    next_config_handler,
    next_execution,
    next_process_handler,
)
from api_utils.config_classes import path_parsing
from api_utils.helper_classes import helper_functions
from api_utils.next_node_process import StartProcessState
from api_utils.status_code_handler import StatusHandler
from api_variables import global_vars

# from api_variables import global_vars
from api_websocket import next_websocket

API_FILE_PATH = os.path.dirname(os.path.realpath(__file__))
DEFAULT_LOGS_DIR = os.path.join(API_FILE_PATH, "Logs")
DEFAULT_LOGS_JSON_CONFIG = os.path.join(API_FILE_PATH, "conf", "next_py_api_logs_config.json")
MODULES_LIST = ["ecal", "websocket", "jsonschema"]
DESC_STRING = "Next Simulation Framework Python API"
JSON_SCHEMA_FILE = os.path.join(API_FILE_PATH, "conf", "python_api_schema.json")

ERROR_SET_ECAL_DOMAIN_DIRECTORY_DO_NOT_EXIST = 1
ERROR_SET_ECAL_DOMAIN_ECAL_INI_FILE_DO_NOT_EXIST = 2
ERROR_SET_ECAL_DOMAIN_GENERAL_ERROR = 3


# pylint: disable=too-many-instance-attributes
class NextAPI:
    """
    Class to store an API object
    Contains all methods needed to simulate Next in headless mode
    """

    def __init__(self, api_config=None, args=None, backend_config=None):
        self.websocket = next_websocket.NextWebsocket()
        self.file_name_ecal_ini = "ecal.ini"
        self.process_handler = None
        self.status_handler = None
        self._previous_backend_settings = None
        if backend_config is not None:
            full_path_ecal_ini = os.path.realpath(
                os.path.join(backend_config.backend_directory, self.file_name_ecal_ini)
            )
            self.status_handler = StatusHandler(["--ecal-ini-file", full_path_ecal_ini])
            self.process_handler = next_process_handler.ProcessHandler(backend_config, self.status_handler)
            self._previous_backend_settings = backend_config

        self.execute = None
        self.api_config = api_config
        self.args = args
        self.config_manager = next_config_handler.ConfigHandler()

    def __del__(self):
        if "LOG" in globals():
            LOG.info("Destructor called, API Closed.")
        else:
            print("[INFO]: Destructor called, API Closed.")
        if self.status_handler:
            self.status_handler.shutdown()

    # pylint: disable-next=too-many-arguments
    def add_sim_node(self, comp_name, comp_type, exec_dir, proc_name, proc_args=None, init_status_code=""):
        """
        Add a simulation node.
        Args:
            comp_name (str): internal component/instance name of this executable
            comp_type (enum StatusOrigin): enum specifying the type of the component
            exec_dir (str): directory of the node executable to start
            proc_name (str): name of the node executable to start
            proc_args (list): list of command line arguments for starting the process
            init_status_code (str): name of the status code which indicates that the initialization of the process
                                    is finished during process startup
            return (bool):  True: successful
                            False: start of node failed
        """
        if proc_args is None:
            proc_args = []
        if not self.process_handler:
            LOG.error("Couldn't add a new simulation node. Variable process_handler is not set")
            sys.exit(global_vars.GENERAL_ERR)

        return self.process_handler.add_sim_node(comp_name, comp_type, exec_dir, proc_name, proc_args, init_status_code)

    def remove_sim_node(self, comp_name, timeout=3):
        """
        Function to stop and remove a simulation node
        Args:
            comp_name (str): internal component/instance name of this executable
            timeout(int): time in seconds to wait for stop. In case of timeout kill process.
            return (bool):   True: successful
                             False: node(s) couldn't be stopped successfully
        """
        if not self.process_handler:
            LOG.error("Couldn't stop and remove a simulation node. Variable process_handler is not set")
            sys.exit(global_vars.GENERAL_ERR)

        return self.process_handler.remove_sim_node(comp_name, timeout)

    def start_backend(self, backend_config=None, timeout=45):
        """
        Function to start all NEXT backend components and the websocket
        Will wait until all components are fully started.
        Args:
            backend_config:settings of the backend
            timeout (int): time in seconds to wait for init status code. In case of timeout abort.
                            None: no timeout
            return (bool):  True: succesful
                            False: error: starting backend not successful
        """
        if not self.process_handler or not self.websocket or not self.status_handler:
            LOG.error("Couldn't start the backend. Variable process_handler, websocket or status_handler are not set")
            sys.exit(global_vars.GENERAL_ERR)

        ret, result_list = self.process_handler.start_backend(timeout=timeout)
        if not ret:
            for result in result_list:
                if StartProcessState.SUCCESS != result[0]:
                    sys.exit(global_vars.COMPONENT_CRASHED)
        if backend_config is not None:
            self.websocket.start_next_websockets(backend_config.ports)
        elif self._previous_backend_settings is not None:
            self.websocket.start_next_websockets(self._previous_backend_settings.ports)
        else:
            self.websocket.start_next_websockets()
        self.execute = next_execution.NextExecution(self.websocket, self.status_handler)
        return True

    def start_all_sim_nodes(self, timeout=None):
        """
        Function to start all simulation nodes.
        Will wait until components are started in case init_status_code is specified in list_node_process_prop.
        Args:
            timeout(int): time in seconds to wait for init status code. In case of timeout abort.
            return (bool):  True: successful
                            False: node(s) couldn't be started successfully
        """
        if not self.process_handler:
            return self.process_handler.start_all_sim_nodes(timeout)
        return False

    def start_simulation(self, backend_config=None, timeout=None):
        """
        Function to start all NEXT backend components, sim nodes and the websocket
        Will wait until all components are fully started.
        Args:
            backend_config: settings of the backend
            timeout (int): time in seconds to wait for init status code. In case of timeout abort.
                            None: no timeout
            return (bool):  True: successful
                            False: error: starting backend not successful
        """
        if not self.process_handler or not self.websocket or not self.status_handler:
            LOG.error(
                "Couldn't start the simulation. Variable process_handler, websocket or status_handler are not set"
            )
            sys.exit(global_vars.GENERAL_ERR)

        # will start backend and simulation nodes in parallel
        if not self.process_handler.start_simulation(timeout):
            LOG.error("Couldn't start the simulation. Function process_handler.start_simulation returned false")
            sys.exit(global_vars.GENERAL_ERR)

        if backend_config is not None:
            self.websocket.start_next_websockets(backend_config.ports)
        elif self._previous_backend_settings is not None:
            self.websocket.start_next_websockets(self._previous_backend_settings.ports)
        else:
            self.websocket.start_next_websockets()

        self.execute = next_execution.NextExecution(self.websocket, self.status_handler)
        return True

    def stop_backend(self, timeout=3):
        """
        Function to stop all backend nodes in parallel
        Args:
            timeout(int):   time in seconds to wait for stop. In case of timeout kill process.
            return (bool):  True: successful
                            False: node(s) couldn't be stopped successfully
        """
        ret = False
        if self.process_handler:
            # will stop backend and simulation nodes in parallel
            ret = self.process_handler.stop_backend(timeout)

        if self.websocket:
            self.websocket.close_next_websockets()

        return ret

    def stop_all_sim_nodes(self, timeout=3):
        """
        Function to stop all sim nodes in parallel
        Args:
            timeout(int): time in seconds to wait for stop. In case of timeout kill process.
            return (int):   True: successful
                            False: backend node(s) couldn't be stopped successfully
        """
        if not self.process_handler:
            LOG.error("Couldn't stop all simulation nodes. Variable process_handler is not set")
            sys.exit(global_vars.GENERAL_ERR)

        return self.process_handler.stop_all_sim_nodes(timeout=timeout)

    def stop_simulation(self, timeout=3):
        """
        Function to stop all backend and sim nodes
        Args:
            timeout(int): time in seconds to wait for stop. In case of timeout kill process.
            return (int):   True: successful
                            False: node(s) couldn't be stopped successfully
        """
        ret = False
        if self.process_handler:
            # will stop backend and simulation nodes in parallel
            ret = self.process_handler.stop_simulation(timeout)

        if self.websocket:
            self.websocket.close_next_websockets()

        return ret

    def restart_all_sim_nodes(self, timeout_start=None, timeout_stop=3):
        """
        Function to restart existing simulation components.
        Clean up history of status handler.
        Will wait until components are started in case init_status_code is specified.
        Args:
            timeout_start(int): time in seconds to wait for init status code. In case of timeout abort.
            timeout_stop(int): time in seconds to wait for stop. In case of timeout kill process.
            return (bool):  True: successful
                            False: node(s) couldn't be started successfully
        """
        if not self.process_handler:
            LOG.error("Couldn't restart all sim nodes. Variable process_handler is not set")
            sys.exit(global_vars.GENERAL_ERR)

        # will stop backend and simulation nodes in parallel
        return self.process_handler.restart_all_sim_nodes(timeout_start, timeout_stop)

    def restart_simulation(self, timeout_start=None, timeout_stop=3):
        """
        Function to shut down all backend and sim nodes and restart them
        timeout_start(int): time in seconds to wait for init status code. In case of timeout abort.
        timeout_stop(int): time in seconds to wait for stop. In case of timeout kill process.
        Args:
           return (int):    True: successful
                            False: node(s) couldn't be restarted successfully
        """
        ret = False
        if self.process_handler:
            ret = self.process_handler.restart(timeout_start, timeout_stop)
        return ret

    def shutdown(self, timeout=3):
        """
        Function to stop all backend and sim nodes and shutdown process handler and close the websockets
        Afterward simulation can't be started again anymore. process_config needs to be called first again
        Args:
            timeout(int): time in seconds to wait for shutdown. In case of timeout kill process.
            return (bool):  True: successful
                            False: node(s) couldn't be shutdown successfully
        """
        ret = False
        if self.process_handler:
            ret = self.process_handler.shutdown(timeout)
        if self.websocket:
            self.websocket.close_next_websockets()

        return ret

    def load_configuration(self, cfg):
        """
        Function used to load a config into the API.
        Can be either a file, a string or a dict.
        :param cfg: Config that shall be used by the api
        :type cfg: str or dict
        :return: True or False if config was loaded
        :rtype: bool
        """
        self.config_manager.load_configuration(cfg)

    def update_configuration(self, args):
        """
        Updates the existing configuration based on arguments provided to the API.

        :param args: Command line arguments for the API
        :type args: argparser
        :return: True or False if config was updated
        :rtype: bool
        """
        self.config_manager.update_configuration(args)

    def overwrite_configuration(self, args):
        """
        Overwrites the existing configuration based on arguments provided to the API.

        :param args: Command line arguments for the API
        :type args: argparser
        :return: None
        :rtype: None
        """
        self.config_manager.overwrite_configuration(args)

    def process_config(self):
        """
        Executes the steps defined in the config
        :return: None
        :rtype: None
        """
        if not self.api_config:
            error_message = "Failed to load configuration. Variable api_config is not set"
            LOG.error(error_message)
            raise RuntimeError(error_message)

        self.load_configuration(self.api_config)
        flows_to_execute = self.__get_flows_to_execute()
        if self.args:
            self.overwrite_configuration(self.args)
            if self.args.f:
                flows_to_execute = helper_functions.return_matched_patterns(
                    re_pattern=self.args.f, config=flows_to_execute
                )
            if self.args.rf:
                flows_to_execute = helper_functions.return_unmatched_patterns(
                    re_pattern=self.args.rf, config=flows_to_execute
                )
        flows_to_execute = self.__get_flows_to_execute()

        for flow_key in flows_to_execute:
            if None is not self.status_handler:
                self.status_handler.cleanup_status_history()

            flow_entry = self.config_manager.sim_flows[flow_key]
            run_input = self.__get_sim_input(flow_entry)
            run_export = self.__get_sim_export(flow_entry)
            sim_nodes = self.__get_sim_nodes(flow_entry)
            sim_runs = self.__get_sim_runs(flow_entry)

            if global_vars.dryrun:
                LOG.info("Dryrun selected. Only printing what would happen")
                LOG.info("Selected Flow: %s", flow_key)
                LOG.info("Selected Inputs: %s", flow_entry.sim_input)
                LOG.info("Selected Exports: %s", flow_entry.sim_export)
                LOG.info("Selected Sim Nodes: %s", flow_entry.sim_nodes)
                LOG.info("Selected Executions: %s", flow_entry.sim_run)
                flow_processor.process_flow(run_input, run_export, sim_nodes, sim_runs, self, self.process_handler)
                LOG.info("End of execution.\n")
            else:
                if self._previous_backend_settings != flow_entry.backend_settings:
                    if None is not self.status_handler:
                        self.status_handler.shutdown()

                    if None is not self.process_handler:
                        self.process_handler.shutdown()

                    full_path_ecal_ini = os.path.realpath(
                        os.path.join(flow_entry.backend_settings.backend_directory, self.file_name_ecal_ini)
                    )
                    self.status_handler = StatusHandler(["--ecal-ini-file", full_path_ecal_ini])

                    self.process_handler = next_process_handler.ProcessHandler(
                        flow_entry.backend_settings, self.status_handler
                    )
                    if not self.start_backend(flow_entry.backend_settings, 180):
                        continue
                    self._previous_backend_settings = flow_entry.backend_settings

                flow_processor.process_flow(
                    run_input, run_export, sim_nodes, sim_runs, self.execute, self.process_handler
                )

    def __get_sim_input(self, flow_entry):
        run_input = []
        for selected_input in flow_entry.sim_input:
            run_input.append((self.config_manager.sim_input[selected_input], selected_input))
        return run_input

    def __get_sim_export(self, flow_entry):
        run_export = []
        for selected_export in flow_entry.sim_export:
            run_export.append((self.config_manager.sim_export[selected_export], selected_export))
        return run_export

    def __get_sim_nodes(self, flow_entry):
        sim_nodes = []
        for selected_sim_nodes in flow_entry.sim_nodes:
            sim_nodes.append((self.config_manager.sim_nodes[selected_sim_nodes], selected_sim_nodes))
        return sim_nodes

    def __get_sim_runs(self, flow_entry):
        sim_runs = []
        for selected_executions in flow_entry.sim_run:
            sim_runs.append((self.config_manager.sim_runs[selected_executions], selected_executions))
        return sim_runs

    def __get_flows_to_execute(self):
        flows_to_execute = self.config_manager.sim_flows.keys()
        if self.args:
            self.overwrite_configuration(self.args)
            if self.args.f:
                flows_to_execute = helper_functions.return_matched_patterns(
                    re_pattern=self.args.f, config=flows_to_execute
                )
            if self.args.rf:
                flows_to_execute = helper_functions.return_unmatched_patterns(
                    re_pattern=self.args.rf, config=flows_to_execute
                )
        return flows_to_execute

    def wait_for_code(self, status_code_message, timeout=None, consider_history=False):
        """
        Wait till any of the specified codes are published
        Args:
             status_code_message(obj StatusCodeMessage): expected status code message
                                                        properties which are set to "None" are ignored
                                                        for checking if the published status code message fits.
            timeout (int): time in seconds to wait for status code.
            consider_history (bool): check if the status codes was already published before the start of this call
            return (bool):  True: status code received
                            False: status code not received
        """
        return self.wait_for_code_list([status_code_message], timeout, consider_history)

    def wait_for_code_list(self, status_code_message_list=None, timeout=None, consider_history=False):
        """
        Wait till any of the specified codes are published
        Args:
             status_code_message_list( list of obj StatusCodeMessage): list with expected status code message
                                                                   properties which are set to "None" are ignored
                                                                   for checking if the published status code message
                                                                   fits.
            timeout (int): time in seconds to wait for status code.
            consider_history (bool): check if the status codes was already published before the start of this call
            return (bool):  True: status code received
                            False: status code not received
        """
        if status_code_message_list is None:
            status_code_message_list = []
        return self.status_handler.wait_for_code_list(status_code_message_list, timeout, consider_history)

    def add_status_code_listener(self, callback, comp_name=None, comp_type=None, code=None):
        """
        Function to be called in case the specified status code is published by the specified component
        In case "code" is defined, "comp_type" has to be defined as well.
        In case of "None" this attribute is ignored for checking if the received status code fits.
        Args:
            callback (func): function to be called in case status code appears
            comp_name (str) (optional): internal component/instance name of this executable
            comp_type (enum StatusOrigin)(optional): enum specifying the type of the component
            code (enum ResultStatusCode)(optional): status code which will trigger calling of the callback
                                         In case of "None"" it will be called for every status message
            return (bool):  True: registration was successful
                            False: not successful
        """
        status_code = None
        if code and not comp_type:
            return False
        if code and comp_type:
            status_code = comp_type * 1000 + code
        return self.status_handler.add_listener(callback, comp_name, comp_type, status_code)

    # Will be used later
    def __set_new_ecal_domain(self, exec_dir):  # pylint: disable=W0238
        """
        Function to start new ecal domain to prevent interference with other instances using ecal

        :param exec_dir: directory of ecal.ini file
        :type exec_dir: str
        :return: 0: successful,  1: error directory doesn't exist, 2: error ecal.ini file doesn't exist,
                          3: general error during setting of new ecal domain
        :rtype: int
        """
        orig_cwd = os.getcwd()

        if not os.path.exists(exec_dir):
            LOG.error(
                "[ProcessHandler]::set_new_ecal_domain: Folder doesn't exist: %s.\nBackend processes not started.",
                exec_dir,
            )
            return ERROR_SET_ECAL_DOMAIN_DIRECTORY_DO_NOT_EXIST

        os.chdir(exec_dir)

        # set a new ecal domain
        if not os.path.exists("ecal.ini"):
            LOG.error(
                "[ProcessHandler]::set_new_ecal_domain: ecal.ini file doesn't exist in folder: %s.\n"
                "Backend processes not started.",
                exec_dir,
            )
            return ERROR_SET_ECAL_DOMAIN_ECAL_INI_FILE_DO_NOT_EXIST

        try:
            with open("ecal.ini", encoding="utf-8") as fin, open("ecal2.ini", "wt", encoding="utf-8") as fout:
                for line in fin:
                    if line.startswith("shm_monitoring_domain"):
                        line = "shm_monitoring_domain       = ecal_monitoring" + str(time.time()) + "\n"

                        fout.write(line)
            os.remove(os.path.join(exec_dir, "ecal.ini").replace("\\", "\\\\"))
            os.rename("ecal2.ini", "ecal.ini")
        except IOError:
            LOG.error(
                "[ProcessHandler]::set_new_ecal_domain: I/O error during setting new ecal domain: %s", sys.exc_info()[0]
            )
            return ERROR_SET_ECAL_DOMAIN_GENERAL_ERROR
        except Exception:  # noqa: E722 pylint: disable=W0718
            LOG.error(
                "[ProcessHandler]::set_new_ecal_domain: Exception during setting new ecal domain: %s", sys.exc_info()[0]
            )
            return ERROR_SET_ECAL_DOMAIN_GENERAL_ERROR

        os.chdir(orig_cwd)

        return global_vars.SUCCESS


# pylint: enable=too-many-instance-attributes


def modules_checker():
    """
    Function to find out ptf required modules are installed

    :returns: list of not found modules
    """
    not_found_module_list = []
    for module in MODULES_LIST:
        try:
            __import__(module)
        except ImportError:
            not_found_module_list.append(module)
    return not_found_module_list


# disabling 'too-many-branches' warning from pylint because we need to check each cli option
# and raise relevant error(s)
# pylint: disable=too-many-branches
def verify_args(args):
    """
    Function for checking arguments and raising errors in-case of wrong arguments

    :param obj args: Command line arguments
    """
    # check if cfg file exists
    if args.c is not None:
        if not os.path.exists(args.c):
            error_message = (
                "Wrong -c option detected: \n"
                "Location (" + args.c + ") doesn't exist. Please check if config file exists"
            )
            LOG.error(error_message)
            raise RuntimeError(error_message)
        # check if input location exists
        if args.i is not None:
            args.i = path_parsing.get_path(args.i)
            if not os.path.exists(args.i):
                error_message = (
                    "Wrong Simulation Input config file detected: \n"
                    "File (" + args.i + ") doesn't found. Please enter correct file location"
                )
                LOG.error(error_message)
                raise RuntimeError(error_message)
        # check if flow(s) is selected to be executed
        if args.f is not None:
            if not isinstance(args.f, str):
                error_message = (
                    "Wrong selected flows data type detected \n"
                    "Argument -f (" + args.f + ") should be a string. Please enter a string"
                )
                LOG.error(error_message)
                raise RuntimeError(error_message)
        # check if EXCLUDE flow(s) from execution argument is given
        if args.rf is not None:
            if not isinstance(args.rf, str):
                error_message = (
                    "Wrong selected excluding flows data type detected \n"
                    "Argument -rf (" + args.rf + ") should be a string. Please enter a string"
                )
                LOG.error(error_message)
                raise RuntimeError(error_message)
        # check if output location exists
        if args.o is not None:
            if not os.path.exists(args.o):
                error_message = (
                    "Wrong output folder detected: \n"
                    "Location (" + args.o + ") doesn't exist. Please enter correct base location"
                )
                LOG.error(error_message)
                raise RuntimeError(error_message)
        # check if logs directory exist
        if args.ld is not None:
            if not os.path.isdir(args.ld):
                error_message = (
                    "Wrong logs folder detected: \n"
                    "Location (" + args.ld + ") doesn't exist. Please enter correct base location"
                )
                LOG.error(error_message)
                raise RuntimeError(error_message)
        if args.dryrun > 0:
            global_vars.dryrun = True


# pylint: enable=too-many-branches


def verify_config(args):
    """
    Verifies if the config file provided with cmd line args is matching our schema.

    :param args: Cmd line arguments
    :type args: argpaser
    :return: True or false if config is valid
    :rtype: bool
    """
    try:
        if args.c is not None:
            config_file = os.path.realpath(args.c)
        else:
            config_file = os.path.join(API_FILE_PATH, "conf", "next_api_full_example_config.json")
        if args.dryrun > 0:
            global_vars.dryrun = True
        cfg_json_valid = helper_functions.validate_json_with_schema(
            input_json_file=config_file, json_schema_file=JSON_SCHEMA_FILE
        )
        if cfg_json_valid:
            return helper_functions.return_json_content(config_file)
    except Exception as e:  # pylint: disable=broad-exception-caught
        LOG.error("Error in Schema:\n%s", e)
    return False


def set_logger(logs_dir=None):
    """
    Function to update the log levels and directory based on config values
    :param logs_dir: path to folder were files should be logged
    :type logs_dir: str
    :return: None
    :rtype: None
    """
    if logs_dir:
        log_dir_path = os.path.realpath(logs_dir)
    else:
        log_dir_path = os.path.realpath(DEFAULT_LOGS_DIR)
    # check if the Logs folder exist, if not created
    if not os.path.isdir(log_dir_path):
        os.makedirs(log_dir_path)
    log_file_path = os.path.join(
        log_dir_path, rf"log_file_next_api_py_{datetime.datetime.now().strftime('%d_%m_%Y_%H_%M_%S')}.txt"
    )
    log_level_console = logging.INFO
    log_level_file = logging.ERROR
    log_format = "%(asctime)s[%(filename)s][%(levelname)-2s] %(message)s"
    log_datefmt = "[%Y-%m-%d][%H:%M:%S]"
    log_formatter = logging.Formatter(fmt=log_format, datefmt=log_datefmt)

    # Also, log to the console:
    console_log_handler = logging.StreamHandler(sys.stdout)
    console_log_handler.setLevel(log_level_console)
    console_log_handler.setFormatter(log_formatter)
    file_log_handler = logging.handlers.TimedRotatingFileHandler(filename=log_file_path)
    file_log_handler.setLevel(log_level_file)
    file_log_handler.setFormatter(log_formatter)

    LOG.addHandler(file_log_handler)
    LOG.addHandler(console_log_handler)


# pylint: disable=R0915


def main():
    """
    Main function for handling input arguments
    """
    global LOG  # pylint: disable=W0601
    # The exit code of the program.
    # Default is 0 (= success), on errors this will be changed to values != 0
    exit_code = global_vars.SUCCESS
    parser = argparse.ArgumentParser(description=DESC_STRING, formatter_class=argparse.RawTextHelpFormatter)
    # adding Command Line Options
    parser.add_argument("-c", help="Enter path of configuration file", type=str, required=False, metavar="cfg_file")
    parser.add_argument(
        "--dryrun", help="When provided a dryrun will be executed", action="count", default=0, required=False
    )
    parser.add_argument(
        "-i",
        help="Enter path of Simulation Input configuration file."
        " The provided configuration file will overwrite all Simulation Input"
        " provided in the config file provided by the -c option.",
        required=False,
        metavar="input_sim_config_file",
    )
    parser.add_argument(
        "-o", help="Location where the API exports all output files to.", required=False, metavar="output_dir"
    )
    parser.add_argument("-ld", help="Enter path of logs directory", required=False, metavar="logs_dir")
    parser.add_argument(
        "-f",
        help="Select which flow(s) from the config file (provided by the -c option) shall be executed. "
        "Only the provided flow(s) will be executed"
        "The value should be a string (CASE SENSITIVE) and can be a single name, a regular expression "
        "or a list of names/regular expressions"
        "the separator will be an empty space (' ').",
        required=False,
        metavar="flows_selected",
    )
    parser.add_argument(
        "-rf",
        help="Select which flow(s) from the config file (provided by the -c option) shall be EXCLUDED from execution. "
        "Only the provided flow(s) will be excluded from flow(s) execution list "
        "The value should be a string(CASE SENSITIVE) and can be a single name, a regular expression "
        "or a list of names/regular expressions "
        "the separator will be an empty space (' ')",
        required=False,
        metavar="remove_flows_selected",
    )

    args = parser.parse_args()
    api = None

    # Logging
    LOG = logging.getLogger(__name__)
    LOG.setLevel(logging.INFO)
    set_logger(args.ld)

    try:
        # verifies that args are valid before passing them to next stage
        verify_args(args)

        LOG.info("Checking required Python modules ...")
        not_found_modules = modules_checker()
        if not not_found_modules:
            LOG.info("All Python modules exists for running framework")
        else:
            error_message = "Following Python module(s) required to run api not found:\n%s", not_found_modules
            LOG.error(error_message)
            raise RuntimeError(error_message)

        api = NextAPI(
            api_config=args.c,
            args=args,
        )

        api.process_config()

    # raise run time error
    except RuntimeError as error:
        LOG.error("-" * 100)
        LOG.error("RuntimeError Occurred:\n%s", error)
        LOG.error(traceback.format_exc())
        LOG.error("-" * 100)
        exit_code = global_vars.GENERAL_ERR

    # raise basic exception
    except Exception as error:  # pylint: disable=broad-except
        LOG.error("-" * 100)
        LOG.error("Exception Occurred: %s", error)
        LOG.error(traceback.format_exc())
        LOG.error("-" * 100)
        exit_code = global_vars.GENERAL_ERR
    except SystemExit as system_exit_code:
        LOG.error("-" * 100)
        LOG.error("SystemExit Occurred: %s", system_exit_code)
        LOG.error(traceback.format_exc())
        LOG.error("-" * 100)
        exit_code = system_exit_code.code

    # things to do before exiting
    finally:
        # print help
        LOG.info("Exiting %s Platform", global_vars.TOOL_NAME)
        if "api" in locals() and api:
            api.shutdown()
        # reset ECAL DATA environment variable
        helper_functions.restore_env_var("ECAL_DATA")
        # exit the system
        sys.exit(exit_code)


if __name__ == "__main__":
    main()
