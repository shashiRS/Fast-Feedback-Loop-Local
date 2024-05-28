import logging

from next_py_api.api_utils.execution_classes.next_execution_classes import ExecutionStep

LOG = logging.getLogger("__main__")


class CustomLogIfConnected(ExecutionStep):
    def __init__(self):
        ExecutionStep.__init__(self, "custom_log_if_connected")

    def invoke_dryrun(self, args="", recording_batch="", sim_nodes="", export_settings="", run_input_settings=""):
        LOG.info("This is a custom execution dryrun!")

    def invoke_step(self, args, recording_batch, sim_nodes, export_settings, run_input_settings):
        if self.next_execution.websocket.connected():
            LOG.info("Still connected")
