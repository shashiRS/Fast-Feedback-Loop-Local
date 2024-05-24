"""
    Copyright 2022 Continental Corporation
    :file: precondition.py
    :platform: Windows, Linux
    :synopsis:
        Script containing postconditions for the different software tests
    :author:
        - Jannik Fritz (uidr9189) <jannik.fritz@continental-corporation.com>
"""
# standard Python import area
import signal
import subprocess


def close_backend_process(process_name, communicate_timeout=3):
    """
    Closes the given subprocess.
    First tries gracefully, then sends kill command
    Args:
        process_name (str): name of process to be closed
        communicate_timeout (int): time in second to wait for process to close with graceful shutdown
    """
    try:
        if hasattr(signal, "CTRL_C_EVENT"):
            process_name.send_signal(signal.CTRL_C_EVENT)
        else:
            process_name.send_signal(signal.SIGINT)
        process_name.communicate(timeout=communicate_timeout)
    except subprocess.TimeoutExpired:
        print("Graceful shutdown not successful, going to terminate")
        process_name.terminate()
        if process_name.poll() is not None:
            print("killing ControlBridge")
            process_name.kill()
    except ValueError:
        print("Process already terminated.")
