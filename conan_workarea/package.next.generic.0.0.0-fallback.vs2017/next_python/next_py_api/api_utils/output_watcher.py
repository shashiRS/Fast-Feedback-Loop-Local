"""
    Copyright 2023 Continental Corporation
    :file: output_watcher.py
    :platform: Windows, Linux
    :synopsis:
        Script containing helper functions to read the output of subprocesses
    :author:
        - Jannik Fritz (uidr9189) <jannik.fritz@continental-corporation.com>
"""

import logging
import time
from queue import Empty, Queue
from threading import Thread

LOG = logging.getLogger("__main__")


def put_output_in_queue(out, in_queue):
    """
    Function that will run in the threads.
    Will iterate over the provided stdout and put it into the provided queue
    :param out: stdout of the process that shall be watched
    :type out: subprocess.PIPE
    :param in_queue: queue that shall be filled with stdout
    :type in_queue: queue.Queue
    """
    try:
        for line in iter(out.readline, b""):
            try:
                in_queue.put(line.decode("utf-8"))
            except UnicodeDecodeError:
                LOG.error("Output watcher could not decode %s", line)
    except ValueError:
        LOG.error("Output Watcher was called on empty queue. Continuing.")
    out.close()


def get_output_queue(proc):
    """
    Function to create and return the queue to watch the stdout of the given process
    Background thread will be started to fill the queue with new data as soon as it´s available.
    :param proc: process that shall be watched
    :type proc: subprocess.Popen
    :return: Queue that will be filled with stdout if available
    :rtype: queue.Queue
    """
    output_queue = Queue()
    output_queue_thread = Thread(target=put_output_in_queue, args=(proc.stdout, output_queue), daemon=True)
    output_queue_thread.start()
    return output_queue


def read_output_until(output_queue, keyword, timeout=10):
    """
    reads the provided output queue until the keyword is found or until timeout is reached
    this function will block the execution until it is finished
    :param output_queue: Queue that will be read
    :type output_queue: queue.Queue
    :param keyword: words that shall be found in the output
    :type keyword: str
    :param timeout: Timeout in seconds
    :type timeout: int
    :return: True for found keyword, false for timeout
    :rtype: bool
    """
    start_time = time.time()
    keyword_found = False
    while not keyword_found:
        if start_time + timeout < time.time():
            LOG.warning("Timeout occurred while waiting for shell output. Expected response: %s", keyword)
            break
        try:
            line = output_queue.get_nowait()
        except Empty:
            continue  # no output available
        else:
            if keyword in line:
                keyword_found = True
                break

    return keyword_found


def read_output(output_queue, keyword):
    """
    Reads one line of the queue.
    Checks if the given keyword is in the line.
    Hint: Will only read one line of the queue. It might be possible that lines are missed when this function is used.
    :param output_queue: Queue that will be read
    :type output_queue: queue.Queue
    :param keyword: words that shall be found in the output
    :type keyword: str
    :return: True for found keyword, false for timeout
    :rtype: bool
    """
    keyword_found = False
    try:
        line = output_queue.get_nowait()
    except Empty:
        pass  # no output available
    else:
        if keyword in line:
            keyword_found = True
    return keyword_found


def read_all_output(output_queue, keyword):
    """
    Reads all currently available lines in the queue.
    Checks, if the given keyword is in any of those lines.
    Hint: The function will not break when the keyword is found. It will empty the complete available queue
    :param output_queue: Queue that will be read
    :type output_queue: queue.Queue
    :param keyword: words that shall be found in the output
    :type keyword: str
    :return: True for found keyword, false for timeout
    :rtype: bool
    """
    keyword_found = False
    while not output_queue.empty():
        line = output_queue.get_nowait()
        if keyword in line:
            keyword_found = True
    return keyword_found
