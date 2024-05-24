"""
    Copyright 2022 Continental Corporation

    This file contains functionality to subscribe and react on status codes.

    :file: status_code_handler.py

    :author:
        - Jannik Fritz (uidr9189) <jannik.fritz@continental-corporation.com>
"""

import json
import logging
import queue
import threading
import time
from enum import IntEnum

# pylint: disable=import-error
import ecal.core.core as ecal_core
from ecal.core.subscriber import StringSubscriber
from next_py_api.api_variables.next_status_code import StatusOrigin

# pylint: enable=import-error
LOG = logging.getLogger("__main__")


# pylint: disable-next=missing-class-docstring
class Severity(IntEnum):
    UNDEFINED = 0
    INFO = 1
    SUCCESS = 2
    WARNING = 3
    ERROR = 4
    FAILURE = 5


class StatusCodeMessage:
    """
    Messages that are received on the StatusCodeMessage Topic
    """

    def __init__(self, comp_name=None, origin=None, status_code=None, status_message=None):
        self.comp_name = comp_name
        self.origin = None
        if origin is not None:
            self.origin = int(origin)
        self.status_code = None
        if status_code is not None:
            # Hint: leading zeros are removed by converting str to int
            self.status_code = int(status_code)
        self.severity = self.get_severity()
        self.status_message = status_message

    # pylint: disable-next=too-many-return-statements
    def get_severity(self):
        """
        Returns the severity of the Message

        :return: Severity of Status Code Message
        :rtype: Severity Enum
        """
        status_code_str = self.get_status_code_str()
        if status_code_str is None:
            return Severity.UNDEFINED

        second_digit = int(status_code_str[1])

        if second_digit == Severity.INFO:
            return Severity.INFO
        if second_digit == Severity.SUCCESS:
            return Severity.SUCCESS
        if second_digit == Severity.WARNING:
            return Severity.WARNING
        if second_digit == Severity.ERROR:
            return Severity.ERROR
        if second_digit == Severity.FAILURE:
            return Severity.FAILURE
        return Severity.UNDEFINED

    def compatible(self, status_code_message):
        """
        Compares the received Status message with the provided and returns whether it is similar

        :param status_code_message: Status code message to compare against
        :type status_code_message: StatusCodeMessage
        :return: True or False if messages are similar
        :rtype: bool
        """
        # ignore all properties which are set to None
        return bool(
            (self.comp_name is None or self.comp_name == status_code_message.comp_name)
            and (self.origin is None or self.origin == status_code_message.origin)
            and (self.status_code is None or self.status_code == status_code_message.status_code)
            and (self.status_message is None or self.status_message == status_code_message.status_message)
        )

    def __str__(self):
        if None is not self.origin:
            try:
                origin = StatusOrigin(self.origin)
            except ValueError:
                origin = "undefined"
        else:
            origin = "undefined"

        if None is not self.comp_name:
            comp_name = self.comp_name
        else:
            comp_name = "undefined"

        status_code_str = self.get_status_code_str()
        if None is status_code_str:
            status_code_str = "undefined"

        if None is not self.status_message:
            status_message = self.status_message
        else:
            status_message = "undefined"

        return (
            f"[SCM] Received Status Message: Origin: {origin}, Component name: {comp_name}, "
            f" Status Code: {status_code_str}, Status Message: {status_message}"
        )

    def __eq__(self, other):
        return (self.comp_name, self.origin, self.status_code, self.status_message) == (
            other.comp_name,
            other.origin,
            other.status_code,
            other.status_message,
        )

    def __hash__(self):
        return hash((self.comp_name, self.origin, self.status_code, self.status_message))

    def get_status_code_str(self):
        """

        :return:
        :rtype:
        """
        if self.status_code is None or self.status_code < 0:
            return None

        str_status_code = str(self.status_code)
        # leading zeros are removed in the integer representation of the status code
        # make sure that the status code has always 4 digits
        while len(str_status_code) < 4:
            str_status_code = "0" + str_status_code

        return str_status_code


# pylint: disable-next=too-many-instance-attributes
class StatusHandler:
    """
    Class to store functionality to work with status codes
    """

    def __init__(
        self, ecal_args, topic_name="next::sdk::events::StatusCodeMessage_", unit_name="Session Control Module"
    ):
        self.subscriber = None
        self.status_history_last_codes = []  # contains the latest codes
        self.status_history_complete = []  # contains all codes since start of status handler
        self.condition_new_data = threading.Condition()
        self.listeners = {}
        self.listener_id = 0
        self.listener_thread = None
        self.stop_event = threading.Event()
        self.lock_update_listener = threading.Lock()
        self.lock_update_data = threading.Lock()
        self.new_status_queue = queue.Queue()
        self.wait_for_code_cond = {}
        self.wait_for_code_results = {}
        self.wait_for_code_cond_lock = threading.Lock()
        self.wait_for_code_counter = 0
        self.wait_for_code_lock = threading.Lock()

        self.__initialize_ecal(ecal_args, topic_name, unit_name)

    def __del__(self):
        self.shutdown()

    def shutdown(self):
        """
        Destructor

        :return: None
        """
        ecal_core.finalize()
        self.stop_event.set()
        with self.condition_new_data:
            self.condition_new_data.notify_all()
        for _, event_wait_for_code in self.wait_for_code_cond.values():
            if event_wait_for_code is not None:
                event_wait_for_code.set()

        self.wait_for_code_cond.clear()

        self.listener_thread.join()

    def add_listener_by_statuscodemessage(self, callback, expected_status_code_message=StatusCodeMessage()):
        """
        Function to add callback in case the specified status code messsage is published
        In case a status message fulfills the "compatible" function of "expected_status_code_message",
        the hook is called.
        Args:
            callback (func): Hook to be called in case any of the filtering conditions is fulfilled
            expected_status_code_message (obj StatusCodeMessage):  expected status code message
                                                                   properties which are set to "None" are ignored
                                                                   for checking if the published status code message
                                                                   fits.
            return (int):  Internally created ID for this callback which can be used for removal.


        """
        with self.lock_update_listener:
            if expected_status_code_message not in self.listeners:
                self.listeners[expected_status_code_message] = {}

            self.listener_id += 1
            self.listeners[expected_status_code_message][self.listener_id] = callback

        return self.listener_id

    def add_listener(self, callback, comp_name=None, comp_type=None, status_code=None):
        """
        Function to be called in case the specified status code is published by the specified component
        In case of "None" this attribute is ignored for checking if the received status code fits.
        Args:
            callback (func): function to be called in case status code appears
            comp_name (str) (optional): internal component/instance name of this executable
            comp_type (enum StatusOrigin)(optional): enum specifying the type of the component
            status_code (int)(optional): 4 digit status code which will trigger calling of the callback
                                         In case of "None"" it will be called for every status message
            return (bool):  True: registration was successful
                            False: not successful
        """
        return self.add_listener_by_statuscodemessage(callback, StatusCodeMessage(comp_name, comp_type, status_code))

    def remove_all_listener(self):
        """
        Function to remove all callbacks which are registered.
        """
        with self.lock_update_listener:
            self.listeners.clear()

    def remove_listener(self, expected_status_code_message):
        """
        Function to remove all callbacks which are registered for the specified status_code. If several callbacks
        are registered for this status code, all will be removed.
        Args:
            expected_status_code_message (obj StatusCodeMessage):  expected status code message
        """
        with self.lock_update_listener:
            self.listeners.pop(expected_status_code_message, None)

    def remove_listener_by_id(self, callback_id):
        """
        Function to remove a specific callback.
        Args:
            callback_id (int):  id of the callback to removed
        """
        with self.lock_update_listener:
            for status_code, callbacks in self.listeners.items():
                if callback_id in callbacks:
                    callbacks.pop(callback_id, None)
                    if 0 == len(callbacks):
                        # remove entry if no callbacks are registered for this status code
                        self.listeners.pop(status_code, None)
                        return

    def wait_for_code_list(self, status_code_message_list=None, timeout=None, consider_history=False):
        """
        Wait till any of the specified codes are published
        Function call is thread safe
        Args:
             status_code_message_list( list of obj StatusCodeMessage): List with expected status code message.
                                                                   Properties which are set to "None" are ignored
                                                                   for checking if the published status code message
                                                                   fits.
            timeout (int): time in seconds to wait for status code.
            consider_history (bool): check if the status codes was already published before the start of this call
            return (bool):  True: status code received
                            False: status code not received
        """
        if status_code_message_list is None:
            status_code_message_list = []
        with self.wait_for_code_lock:
            self.wait_for_code_counter += 1
            counter = self.wait_for_code_counter

        wait_for_code_event = threading.Event()

        start_time = time.time()

        received_status_message = None
        status_message_received = False

        with self.wait_for_code_cond_lock:
            self.wait_for_code_cond[counter] = (status_code_message_list, wait_for_code_event)

        if consider_history:
            status_message_received, received_status_message = self.check_status_history_for_code(
                status_code_message_list
            )

        if not status_message_received:
            wait_for_code_event.wait(timeout)
            if not (timeout is not None and time.time() - start_time > timeout):
                status_message_received = True

        with self.wait_for_code_cond_lock:
            del self.wait_for_code_cond[counter]
            if counter in self.wait_for_code_results:
                received_status_message = self.wait_for_code_results[counter]
                del self.wait_for_code_results[counter]

        if received_status_message and status_message_received:
            return True, received_status_message

        return False, None

    def wait_for_code_by_statuscodemessage(self, status_code_message, timeout=None, consider_history=False):
        """
        Wait till specified code are published
        Function call is thread safe
        Args:
             status_code_message(obj StatusCodeMessage): Expected status code message.
                                                         Properties which are set to "None" are ignored for
                                                         checking if the published status code message fits.
            timeout (int): time in seconds to wait for status code.
            consider_history (bool): check if the status codes was already published before the start of this call
            return (bool):  True: status code received
                            False: status code not received
        """
        return self.wait_for_code_list([status_code_message], timeout, consider_history)

    # pylint: disable-next=too-many-arguments
    def wait_for_code(self, comp_name=None, comp_type=None, status_code=None, timeout=None, consider_history=False):
        """
        Wait till specified code are published
        In case of "None" this attribute is ignored for checking if the received status code fits.
        Function call is thread safe
        Args:
            comp_name (str) (optional): internal component/instance name of this executable
            comp_type (enum StatusOrigin)(optional): enum specifying the type of the component
            status_code (int)(optional): 4 digit status code which will trigger calling of the callback
                                         In case of "None"" it will be called for every status message
            timeout (int): time in seconds to wait for status code.
            consider_history (bool): check if the status codes was already published before the start of this call
            return (bool):  True: status code received
                            False: status code not received
        """
        return self.wait_for_code_by_statuscodemessage(
            StatusCodeMessage(comp_name, comp_type, status_code), timeout, consider_history
        )

    def get_last_codes(self):
        """
        Get all status codes which were received since the last call of "get_last_codes".
        status_history_last_codes will be cleared after the call.
        Args:
            return (list obj StatusCodeMessage):  List of status code message which were received since
                                                 the last call of "get_last_codes".
        """
        with self.lock_update_data:
            status_history = self.status_history_last_codes.copy()
            self.status_history_last_codes.clear()

        return status_history

    def cleanup_status_history(self):
        """
        Clears history of status codes.
        """
        self.status_history_last_codes.clear()
        self.status_history_complete.clear()

    def check_status_history_for_code(self, status_code_message_list):
        """
        Checks if any of the specified status code messages appeared in the full status code history
        Args:
            status_code_message_list (list obj StatusCodeMessage):  List of status code message which shall be
                                                                    searched in the full status code history
            return (bool, obj StatusCodeMessage): (True, StatusCodeMessage): first status code message from the
                                                    history which fulfills the compatible condition of one of
                                                    the status_code_message_list
                                                    (False, None): No compatible status code found

        """
        with self.lock_update_data:
            for status_code_message in status_code_message_list:
                for history_status_message in self.status_history_complete:
                    if status_code_message.compatible(history_status_message):
                        return True, history_status_message
        return False, None

    def __callback(self, topic_name, msg, timestamp):
        deserialized_message = json.loads(msg)
        status_message = StatusCodeMessage(
            deserialized_message["component_name"],
            deserialized_message["origin"],
            deserialized_message["status_code"],
            deserialized_message["status_msg"],
        )
        LOG.info(status_message)

        self.__push_status(status_message)

        # Placeholder for unused parameters (topic_name and timestamp)
        _ = topic_name
        _ = timestamp

    def __start_listening(self):
        while not self.stop_event.is_set():
            with self.condition_new_data:
                self.condition_new_data.wait()

                while not self.new_status_queue.empty():
                    status_message = self.new_status_queue.get()

                    with self.lock_update_listener:
                        for expected_status_code_message, callback_list in self.listeners.items():
                            # use compatible function for comparison to ignore the "None" parameters
                            if expected_status_code_message.compatible(status_message):
                                for callback in callback_list.values():
                                    callback(status_message)

                    with self.wait_for_code_cond_lock:
                        for index, (status_code_list, event_wait_for_code) in self.wait_for_code_cond.items():
                            if status_code_list is not None and event_wait_for_code is not None:
                                for expected_status_code_message in status_code_list:
                                    if expected_status_code_message.compatible(status_message):
                                        self.wait_for_code_results[index] = status_message
                                        self.wait_for_code_cond[index] = (None, None)

                                        event_wait_for_code.set()

    def __push_status(self, status_code_message):
        self.new_status_queue.put(status_code_message)
        with self.lock_update_data:
            self.status_history_last_codes.append(status_code_message)
            self.status_history_complete.append(status_code_message)
            with self.condition_new_data:
                self.condition_new_data.notify_all()

    def __initialize_ecal(self, ecal_args, topic_name, unit_name):
        # TODO: add if isEcalInitialized when it's available  # pylint: disable=fixme
        ecal_core.initialize(ecal_args, unit_name)
        self.subscriber = StringSubscriber(topic_name)
        self.subscriber.set_callback(self.__callback)

        self.listener_thread = threading.Thread(target=self.__start_listening)
        self.listener_thread.start()
