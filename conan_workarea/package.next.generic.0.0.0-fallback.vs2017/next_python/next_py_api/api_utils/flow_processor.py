"""
    Copyright 2022 Continental Corporation

    This file contains the functions to process the set up flows from the Next API config.

    :file: flow_processor.py

    :author:
        - Jannik Fritz (uidr9189) <jannik.fritz@continental-corporation.com>
"""

import logging

LOG = logging.getLogger("__main__")


# pylint: disable-next=too-many-arguments
def process_flow(sim_input, sim_export, sim_nodes, sim_run, next_execution, process_handler):
    """
    Processes the input from a flow and iterates over the configuration and executes it

    :param sim_input: List of all Simulation Inputs that shall be used during the full flow.
    :type sim_input: List[Tuple(SimulationInput, Str)]
    :param sim_export: List of all Simulation Outputs that shall be used during the full flow.
    :type sim_export: List[Tuple(SimulationExport, Str)]
    :param sim_nodes: List of all Simulation Nodes configurations that shall be used during the full flow.
    :type sim_nodes: List[Tuple(SimulationNodes, Str)]
    :param sim_run: List of all execution steps that shall be used during the full flow.
    :type sim_run: List[Tuple(Execution, Str)]
    :param next_execution: Object containing all available functions to be executed
    :type next_execution: NextExecution
    :param process_handler: Object containing all available functions to control processes
    :type process_handler: ProvessHandler
    :return: None
    :rtype: None
    """
    for it_run_input in sim_input:
        for it_run_export in sim_export:
            if sim_nodes:
                for it_sim_nodes in sim_nodes:
                    for it_run in sim_run:
                        process_run(it_run, it_sim_nodes, it_run_export, it_run_input, next_execution, process_handler)
            else:
                for it_run in sim_run:
                    process_run(it_run, None, it_run_export, it_run_input, next_execution, process_handler)


# pylint: disable-next=too-many-arguments
def process_run(sim_run, nodes, output, inputs, next_execution, process_handler):
    """
    Iterates over the execution steps.
    Invokes each step with all possible arguments

    :param sim_run: List of all execution steps that should be executed
    :type sim_run: List[Tuple(Name, ExecutionClassObj)]
    :param nodes: List of Simulation Nodes that shall be started
    :type nodes: SimulationNodes
    :param output: List of Exporter settings
    :type output: SimulationExport
    :param inputs: List of Recordings that shall be loaded
    :type inputs: SimulationInputs
    :param next_execution: Class storing all execution classes that can be executed
    :type next_execution: NextExecution
    :param process_handler: Process handler of the API used to control nodes
    :type process_handler: ProcessHandler
    :return: None
    :rtype: None
    """
    LOG.info("Starting Startup")
    for execution_step in sim_run[0].setup:
        LOG.info("[Setup] Starting step: %s, with args: %s", execution_step[0].get_name(), execution_step[1])
        execution_step[0].next_execution = next_execution
        execution_step[0].process_handler = process_handler
        execution_step[0].invoke(execution_step[1], None, nodes, output, inputs)
        LOG.info("[Setup]Finished step %s", execution_step[0].get_name())
    LOG.info("Finished Startup")

    LOG.info("Starting Run")
    if inputs[0].recording_batches:
        for batch in inputs[0].recording_batches:
            for execution_step in sim_run[0].run:
                LOG.info("[Run] Starting step: %s, with args: %s", execution_step[0].get_name(), execution_step[1])
                execution_step[0].next_execution = next_execution
                execution_step[0].process_handler = process_handler
                execution_step[0].invoke(execution_step[1], batch, nodes, output, inputs)
                LOG.info("[Run] Finished step: %s.", execution_step[0].get_name())
    else:
        for execution_step in sim_run[0].run:
            LOG.info("[Run] Starting step: %s, with args: %s", execution_step[0].get_name(), execution_step[1])
            execution_step[0].next_execution = next_execution
            execution_step[0].invoke(execution_step[1], None, nodes, output, inputs)
            LOG.info("[Run] Finished step: %s.", execution_step[0].get_name())
    LOG.info("Finished run")

    LOG.info("Starting Teardown")
    for execution_step in sim_run[0].teardown:
        LOG.info("[Teardown] Starting step: %s, with args: %s", execution_step[0].get_name(), execution_step[1])
        execution_step[0].next_execution = next_execution
        execution_step[0].process_handler = process_handler
        execution_step[0].invoke(execution_step[1], None, nodes, output, inputs)
        LOG.info("[Teardown] Starting step: %s, with args: ", execution_step[0].get_name())
    LOG.info("Finished Teardown")
