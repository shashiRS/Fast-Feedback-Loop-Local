"""
    Copyright 2022 Continental Corporation

    This file contains a class providing the Process Handler for the Next API.

    :file: next_process_handler.py

    :author:
        - Dirk Koltermann (uif75429) <dirk.koltermann@continental-corporation.com>
"""

# Standard library imports
import logging
import os
import platform
import threading
from multiprocessing.pool import ThreadPool

from next_py_api.api_utils.helper_classes import helper_functions
from next_py_api.api_utils.next_node_process import (
    CloseProcessState,
    NodeProcess,
    NodeState,
    StartProcessState,
)

# Local application imports
from next_py_api.api_variables import global_vars
from next_py_api.api_variables.next_status_code import (
    NodeNames,
    ResultStatusCode,
    StatusOrigin,
)

LOG = logging.getLogger("__main__")


class ProcessHandler:
    """
    Process handler to handle the Simulation Nodes
    """

    def __init__(self, backend_config=global_vars.BackendConfig(), status_handler=None):
        """
        Constructor
        The backend_directory in backend_config folder will be used as working directory for
        starting up all processes
        Args:
            backend_config (struct): struct containing the backend config settings
                                    (see function __fill_backend_settings in simulation_flow)
            status_handler (obj StatusHandler): handler for getting status codes
        """
        self.backend_dir_path = os.path.realpath(backend_config.backend_directory)
        # export the env variable for ecal.ini folder
        if self.backend_dir_path:
            print("ECAL starting")

            print("Ecal succuss")
            helper_functions.save_env_var("ECAL_DATA")
            if os.getenv("ECAL_DATA"):
                del os.environ["ECAL_DATA"]
            os.environ["ECAL_DATA"] = self.backend_dir_path
            print("Ecal second started")

            print("Ecal succussfull")
        self.status_handler = status_handler

        self.lock_shutdown = threading.Lock()
        self.backend_nodes = {}
        if platform.system() == "Windows":
            self.backend_nodes[NodeNames.NEXT_CONTROLBRIDGE] = NodeProcess(
                self.status_handler,
                NodeNames.NEXT_CONTROLBRIDGE,
                StatusOrigin.NEXT_CONTROLBRIDGE,
                self.backend_dir_path,
                "next_controlbridge.exe",
                [],
                ResultStatusCode.SUCC_COMP_INIT_FINISHED,
            )
            self.backend_nodes[NodeNames.NEXT_DATABRIDGE] = NodeProcess(
                self.status_handler,
                NodeNames.NEXT_DATABRIDGE,
                StatusOrigin.NEXT_DATABRIDGE,
                self.backend_dir_path,
                "next_databridge.exe",
                [],
                ResultStatusCode.SUCC_COMP_INIT_FINISHED,
            )
            self.backend_nodes[NodeNames.NEXT_PLAYER] = NodeProcess(
                self.status_handler,
                NodeNames.NEXT_PLAYER,
                StatusOrigin.NEXT_PLAYER,
                self.backend_dir_path,
                "next_player.exe",
                [],
                ResultStatusCode.SUCC_COMP_INIT_FINISHED,
            )
        else:
            self.backend_nodes[NodeNames.NEXT_CONTROLBRIDGE] = NodeProcess(
                self.status_handler,
                NodeNames.NEXT_CONTROLBRIDGE,
                StatusOrigin.NEXT_CONTROLBRIDGE,
                self.backend_dir_path,
                "next_controlbridge",
                [],
                ResultStatusCode.SUCC_COMP_INIT_FINISHED,
            )
            self.backend_nodes[NodeNames.NEXT_DATABRIDGE] = NodeProcess(
                self.status_handler,
                NodeNames.NEXT_DATABRIDGE,
                StatusOrigin.NEXT_DATABRIDGE,
                self.backend_dir_path,
                "next_databridge",
                [],
                ResultStatusCode.SUCC_COMP_INIT_FINISHED,
            )
            self.backend_nodes[NodeNames.NEXT_PLAYER] = NodeProcess(
                self.status_handler,
                NodeNames.NEXT_PLAYER,
                StatusOrigin.NEXT_PLAYER,
                self.backend_dir_path,
                "next_player",
                [],
                ResultStatusCode.SUCC_COMP_INIT_FINISHED,
            )

        self.sim_nodes = {}  # key = name, value = object(NodeProcess)

    # pylint: disable=too-many-arguments
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
        if comp_name in self.sim_nodes:
            LOG.info("[ProcessHandler]::add_sim_node: %s exist already. No new node is added.", comp_name)
            return False

        self.sim_nodes[comp_name] = NodeProcess(
            self.status_handler, comp_name, comp_type, exec_dir, proc_name, proc_args, init_status_code
        )

        return True

    def start_backend_node(self, comp_name, timeout=None):
        """
        Function to start a backend component.
        Args:
            comp_name (str): internal component/instance name of this executable
            timeout(int): time in seconds to wait for init status code. In case of timeout abort.
            return (bool):  True: successful
                            False: start of node failed
        """
        if comp_name not in self.backend_nodes:
            LOG.warning(
                "[ProcessHandler]::start_backend_node: %s doesn't exist in list of backend nodes. "
                "Node is not started.",
                comp_name,
            )
            return False

        orig_cwd = self.__change_to_backend_dir()
        ret = self.backend_nodes[comp_name].start_process(timeout)
        os.chdir(orig_cwd)

        if StartProcessState.SUCCESS != ret:
            return False

        return True

    def start_backend(self, timeout=25):
        """
        Function to start all NEXT backend components.
        Will wait until all components are fully started.
        Args:
            timeout (int): time in seconds to wait for init status code. In case of timeout abort.
                            None: no timeout
            return (bool, result_list): The first tuple entry contains the overall status
                                            (True: successful; False: error: starting backend not successful)
                                        The second tuple entry contains for each process the status
                                            (StartProcessState enum value)
        """
        return self.__start_nodes_in_parallel(self.backend_nodes, timeout)

    def start_sim_node(self, comp_name, timeout=None):
        """
        Function to start a simulation components.
        Will wait until init_status_code of the node is received (in case it's specified).
        Args:
            comp_name (str): internal component/instance name of this executable
            timeout(int): time in seconds to wait for init status code. In case of timeout abort.
            return (bool):  True: successful
                            False: start of node failed
        """
        if comp_name not in self.sim_nodes:
            LOG.warning("[ProcessHandler]::start_sim_node: %s doesn't exit. Node can't be started.", comp_name)
            return False

        node = self.sim_nodes[comp_name]

        if NodeState.RUNNING == node.get_state():
            LOG.info(
                "[ProcessHandler]::start_sim_node: Node is already running. Node %s is not started again.", comp_name
            )
            return True

        orig_cwd = self.__change_to_backend_dir()
        ret = node.start_process(timeout)
        os.chdir(orig_cwd)

        if StartProcessState.SUCCESS != ret:
            return False

        return True

    def start_all_sim_nodes(self, timeout=None):
        """
        Function to start all simulation nodes.
        Will wait until all init_status_code of the nodes are received (in case it's specified).
        Args:
            timeout(int): time in seconds to wait for init status code. In case of timeout abort.
            return (bool, result_list): The first tuple entry contains the overall status.
                                        The second tuple entry contains for each process the status
        """

        succ, result_list = self.__start_nodes_in_parallel(self.sim_nodes, timeout)
        for res, comp_name in result_list:
            if StartProcessState.SUCCESS != res:
                LOG.warning("[ProcessHandler]::start_sim_node_list: Node %s not started.", comp_name)

        return succ, result_list

    def start_simulation(self, timeout=None):
        """
        Function to start all simulation components and backend in parallel
        Will wait until all init_status_code of the nodes are received (in case it's specified).
        Args:
            timeout(int): time in seconds to wait for init status code. In case of timeout abort.
            return (bool):  True: successful
                            False: node(s) couldn't be started successfully
        """
        pool_start_sim = ThreadPool(processes=2)

        result_list = []

        def log_result(result):
            result_list.append(result)

        pool_start_sim.apply_async(self.start_backend, args=(timeout,), callback=log_result)
        pool_start_sim.apply_async(self.start_all_sim_nodes, args=(timeout,), callback=log_result)

        # wait till all nodes are finished with start up and get return value
        pool_start_sim.close()
        pool_start_sim.join()

        # abort in case of error during startup
        for result in result_list:
            if result[0] is False:
                self.stop_simulation(3)
                return False

        return True

    def stop_backend_node(self, comp_name, timeout=3):
        """
        Function to stop a specific backend node
        Args:
            comp_name (str): internal component/instance name of this executable
            timeout(int):   time in seconds to wait for stop. In case of timeout kill process.
            return (int):   True: successful
                            False: backend node couldn't be stopped successfully
        """
        return self.__stop_node(self.backend_nodes, comp_name, False, timeout)[0]

    def stop_backend(self, timeout=3):
        """
        Function to stop all backend nodes in parallel
        Args:
            timeout(int):   time in seconds to wait for stop. In case of timeout kill process.
            return (bool, result_list): The first tuple entry contains the overall status
                                (True: successful; False: error: backend node(s) couldn't be stopped successfully)
                            The second tuple entry contains for each process the status
                                (StartProcessState enum value)
        """
        return self.__stop_nodes_in_parallel(self.backend_nodes, False, timeout)

    def stop_sim_node(self, comp_name, remove_process=False, timeout=3):
        """
        Function to stop a simulation node and remove it, if configured
        Args:
            comp_name (str): internal component/instance name of this executable
            remove_process (bool):   True: remove process from node list.
                                     False: Keep process in process list, e.g. for restart.
            timeout(int): time in seconds to wait for stop. In case of timeout kill process.
            return (bool):   True: successful
                             False: node(s) couldn't be stopped successfully
        """
        return self.__stop_node(self.sim_nodes, comp_name, remove_process, timeout)[0]

    def remove_sim_node(self, comp_name, timeout=3):
        """
        Function to stop and remove a simulation node
        Args:
            comp_name (str): internal component/instance name of this executable
            timeout(int): time in seconds to wait for stop. In case of timeout kill process.
            return (bool):   True: successful
                             False: node(s) couldn't be stopped successfully
        """
        return self.__stop_node(self.sim_nodes, comp_name, True, timeout)[0]

    def stop_all_sim_nodes(self, timeout=3):
        """
        Function to stop all sim nodes in parallel
        Args:
            timeout(int): time in seconds to wait for stop. In case of timeout kill process.
            return (bool, result_list): The first tuple entry contains the overall status
                                (True: successful; False: error: backend node(s) couldn't be stopped successfully)
                            The second tuple entry contains for each process the status
                                (StartProcessState enum value)
        """
        return self.__stop_nodes_in_parallel(self.sim_nodes, False, timeout)

    def stop_simulation(self, timeout=3):
        """
        Function to stop all backend and sim nodes
        Args:
            timeout(int): time in seconds to wait for stop. In case of timeout kill process.
            return (int):   True: successful
                            False: node(s) couldn't be stopped successfully
        """
        pool_stop_sim = ThreadPool(processes=2)

        result_list = []

        def log_result(result):
            result_list.append(result)

        pool_stop_sim.apply_async(self.stop_all_sim_nodes, args=(timeout,), callback=log_result)
        pool_stop_sim.apply_async(self.stop_backend, args=(timeout,), callback=log_result)

        # wait till all nodes are stopped and get return value
        pool_stop_sim.close()
        pool_stop_sim.join()

        stop_succ = True
        for result in result_list:
            if CloseProcessState.SUCCESS != result[0]:
                stop_succ = False

        # remove history of status codes
        self.status_handler.cleanup_status_history()

        return stop_succ

    def restart_all_sim_nodes(self, timeout_start=None, timeout_stop=3):
        """
        Function to restart existing simulation components.
        Clean up history of status handler.
        Will wait until all init_status_code of the nodes are received (in case it's specified).
        Args:
            timeout_start(int): time in seconds to wait for init status code. In case of timeout abort.
            timeout_stop(int): time in seconds to wait for stop. In case of timeout kill process.
            return (bool, result_list): The first tuple entry contains the overall status.
                                        The second tuple entry contains for each process the status
        """
        self.stop_all_sim_nodes(timeout_stop)

        # remove history of status codes
        self.status_handler.cleanup_status_history()

        return self.start_all_sim_nodes(timeout_start)

    def restart(self, timeout_start=None, timeout_stop=3):
        """
        Function to shut down all backend and sim nodes and restart them
        timeout_start(int): time in seconds to wait for init status code. In case of timeout abort.
        timeout_stop(int): time in seconds to wait for stop. In case of timeout kill process.
        Args:
           return (int):    True: successful
                            False: node(s) couldn't be restarted successfully
        """
        ret = self.stop_simulation(timeout_stop)
        return ret & self.start_simulation(timeout_start)

    def shutdown(self, timeout=3):
        """
        Function to stop all backend and sim nodes and shutdown process handler.
        Afterward simulation can't be started again anymore. ProcessHandler needs to be created again.
        Args:
            timeout(int): time in seconds to wait for shutdown. In case of timeout kill process.
            return (bool):  True: successful
                            False: node(s) couldn't be shutdown successfully
        """
        with self.lock_shutdown:
            stop_succ = True
            if not self.stop_simulation(timeout):
                stop_succ = False

        self.status_handler.shutdown()

        self.sim_nodes.clear()

        return stop_succ

    def get_node(self, comp_name, sim_node=True):
        """
        Function to return a specific node.
        Args:
            comp_name (str): internal component/instance name of this executable
            sim_node (bool): type of the node.
                             True: simulation node
                             False: backend node
            return (bool, result_list):  The first tuple entry contains the overall status.
                                         The second tuple entry contains for each process the status
        """
        if sim_node:
            if comp_name in self.sim_nodes:
                return self.sim_nodes[comp_name]
        elif comp_name in self.backend_nodes:
            return self.backend_nodes[comp_name]

        return None

    def get_all_sim_nodes(self):
        """
        Function to return all sim nodes
        Args:
            return (dict of NodeProcess):  list of all sim nodes
        """
        return self.sim_nodes

    def __start_nodes_in_parallel(self, node_list, timeout):
        """
        Function to start list of nodes in parallel.
        Will wait until all init_status_code of the nodes are received (in case it's specified).
        Args:
            node_list (dict of NodeProcess): list of node processes to start in parallel
            timeout(int): time in seconds to wait for init status code of each node. In case of timeout abort.
            return (bool, result_list):  The first tuple entry contains the overall status.
                                         The second tuple entry contains for each process the status
        """
        result_list = []

        if not node_list:
            return True, result_list

        orig_cwd = self.__change_to_backend_dir()

        pool_start_nodes = ThreadPool(processes=len(node_list))

        def log_result(result):
            result_list.append(result)

        for node_name, node in node_list.items():
            if NodeState.RUNNING == node.get_state():
                LOG.info(
                    "[ProcessHandler]::__start_nodes_in_parallel: Node is already running."
                    "Node %s is not started again.",
                    node_name,
                )
            else:
                pool_start_nodes.apply_async(
                    node.start_process_extended_return_infos, args=(timeout,), callback=log_result
                )

        # wait till all nodes are started and get return value
        pool_start_nodes.close()
        pool_start_nodes.join()

        os.chdir(orig_cwd)

        nodes_started = True
        for result in result_list:
            if StartProcessState.SUCCESS != result[0]:
                LOG.error(
                    "[ProcessHandler]::__start_nodes_in_parallel: Start process %s failed with return value %s",
                    result[1],
                    result[0],
                )
                nodes_started = False

        if nodes_started is False:
            return False, result_list

        # *********** check node status *******************
        # check if all nodes are running
        for node_name, node in node_list.items():
            if NodeState.NOT_RUNNING == node.get_state():
                LOG.error("[ProcessHandler]::__start_nodes_in_parallel: Node %s isn't started", node_name)
                nodes_started = False

        return nodes_started, result_list

    def __change_to_backend_dir(self):
        orig_cwd = os.getcwd()

        if not os.path.exists(self.backend_dir_path):
            LOG.warning(
                "[ProcessHandler]::__start_nodes_in_parallel: Backend directory doesn't exist: "
                "%s. "
                "Backend directory is not used as working directory or starting the nodes.",
                self.backend_dir_path,
            )
        else:
            os.chdir(self.backend_dir_path)

        return orig_cwd

    def __stop_node(self, list_node_processes, comp_name, remove_process=False, timeout=3):
        stop_succ = True
        if comp_name in list_node_processes:
            node = list_node_processes[comp_name]
            if NodeState.RUNNING == node.get_state():
                ret = node.close_process(timeout)
                if ret != CloseProcessState.SUCCESS:
                    stop_succ = False
                if remove_process is True:
                    list_node_processes.pop(comp_name, None)
        else:
            LOG.warning("[ProcessHandler]::__stop_node: Following node not found for stopping: %s", comp_name)

        return stop_succ, comp_name

    def __stop_nodes_in_parallel(self, list_node_processes, remove_process=False, timeout=3):
        result_list = []
        if not list_node_processes:
            return True, result_list

        pool_stop_nodes = ThreadPool(processes=len(list_node_processes))

        def log_result(result):
            result_list.append(result)

        for comp_name, _ in list_node_processes.items():
            pool_stop_nodes.apply_async(
                self.__stop_node,
                args=(
                    list_node_processes,
                    comp_name,
                    remove_process,
                    timeout,
                ),
                callback=log_result,
            )

        # wait till all nodes are stopped and get return value
        pool_stop_nodes.close()
        pool_stop_nodes.join()

        stop_succ = True
        for result in result_list:
            if CloseProcessState.SUCCESS != result[0]:
                stop_succ = False

        return stop_succ, result_list
