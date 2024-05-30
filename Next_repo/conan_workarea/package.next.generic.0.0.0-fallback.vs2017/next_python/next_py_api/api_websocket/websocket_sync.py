"""
    Copyright 2022 Continental Corporation

    :file: websocket_sync.py
    :platform: Windows, Linux
    :synopsis:
        Script containing the APIs for a websocket

    :author:
        - Fabian Janker (uif41320) <fabian.janker@continental-corporation.com>
"""

# standard Python import area
import json
import logging
import time
from threading import Thread

# pylint: disable=import-error
import websocket

# pylint: enable=import-error
LOG = logging.getLogger("__main__")


class Handler:
    """
    This is a class to create a synchronous, websocket based connection to a server.
    The class creates a connection with the initialization function

    external functions:
        Handler(server_address),
        close(server_address),
        send_recv(msg),
        send(msg),
        recv(),
        dump_cache()
    """

    def __init__(self, ip_address, port, timeout_s=60):
        """
        internal initialization function of the websocket handler

        :param ip_address: The ip address of the server
        :type ip_address: string
        :param port: The port of the server
        :type port: string
        :param timeout_s: timeout in seconds for receiving and sending of a message, defaults to 60
        :type timeout_s: int, optional
        """
        # creating a websocket object
        self.ws = websocket.WebSocket()
        # creating the server_address
        self.server_address = f"ws://{ip_address}:{port}"
        # saving the timeout for the dump_cache
        self.timeout_s = timeout_s
        # perform a synchronous connect
        self.__sync_connect(timeout_s)

    def __sync_connect(self, timeout_s):
        """internal function to connect to a server through the initialization function

        :param timeout_s: timeout in seconds for receiving and sending of a message, defaults to 5
        :type timeout_s: int
        """
        LOG.info("attempting connection to %s", self.server_address)
        # store the connected WebSocketClientProtocol
        # object, for later reuse for send & recv
        try:
            self.ws.connect(self.server_address, timeout=timeout_s)
            LOG.info("connected to %s", self.server_address)
        except websocket._exceptions.WebSocketException as e:  # pylint: disable=protected-access
            LOG.error("exception: %s", e)
            LOG.error("attempting connection to %s failed!", self.server_address)

    def close(self):
        """
        external function to close the connection through the websocket
        """
        self.__sync_close()

    def __sync_close(self):
        """
        internal function to close the connection through the websocket
        """
        LOG.info("attempting disconnecting from %s", self.server_address)
        try:
            self.ws.close()
            LOG.info("connection closed")
        except websocket._exceptions.WebSocketException as e:  # pylint: disable=protected-access
            LOG.error("exception: %s", e)
            LOG.error("attempted disconnect from %s failed!", self.server_address)

    def send_recv(self, msg, receive_channel="", time_out=60):
        """
        external function to send and receive a message from the websocket

        :param msg: the message to send through the websocket to the server
        :type msg: string
        :param receive_channel: Channel to listen to for the responses.
        :type receive_channel: str
        :return: the message received from the server through the websocket
                 when timeout occurred returning: False
        :time_out time in seconds waiting for the receiving data
        :type time_out: int
        :rtype: string
        """
        return self.__sync_send_recv(msg, receive_channel, time_out)

    def __sync_send_recv(self, msg, receive_channel, time_out):
        """
        internal function to send and receive a message from the websocket

        :param msg: the message to send through the websocket to the server
        :type msg: string
        :param receive_channel: Channel to listen to for the responses.
        :type receive_channel: str
        :time_out time in seconds waiting for the receiving data
        :type time_out: int
        :return: the message received from the server through the websocket
                 when timeout occurred returning: False
        :rtype: string
        """
        try:
            self.ws.send(msg)
            if receive_channel:
                return self.__receive_msg_on_channel(receive_channel, time_out)
            return self.ws.recv()
        except websocket._exceptions.WebSocketTimeoutException as e:  # pylint: disable=protected-access
            LOG.error("Exception: %s", e)
            return False

    # pylint: disable-next=too-many-arguments
    def send(
        self,
        msg,
        wait_response=True,
        expected_event="",
        expected_payload="",
        receive_channel="",
        get_response=False,
        timeout=60,
    ):
        """
        external function to send a message from the websocket
        if the flag wait_response true the function waits for a receiving
        but without returning the response.
        if the flag wait_response false the function does not wait for a response

        :param msg: the message to send through the websocket to the server
        :type msg: string
        :param wait_response: waiting of the response without returning, defaults to True
        :type wait_response: bool, optional
        :param expected_event: String to search in the response. If nothing is given will take first response
        :type expected_event: str
        :param expected_payload: String to search in the response Payload.
        :type expected_payload: str
        :param receive_channel: Channel to listen to for the responses.
        :type receive_channel: str
        :param get_response: return the message response of command, defaults to False
        :type get_response: bool
        :param timeout: Time in seconds to wait for a response
        :type timeout: int
        :return: if get_response is False: when timeout occurred returning: False
                 if get_response is True: the message response
        :rtype: bool or str
        """

        if wait_response:
            return self.__sync_send_with_response(
                msg=msg,
                expected_event=expected_event,
                expected_payload=expected_payload,
                receive_channel=receive_channel,
                get_response=get_response,
                timeout=timeout,
            )
        return self.__sync_send_without_response(msg)

    # pylint: disable-next=too-many-arguments
    def __sync_send_with_response(self, msg, expected_event, expected_payload, receive_channel, get_response, timeout):
        """
        internal function to send a message from the websocket
        with waiting but not returning of the response

        :param msg: the message to send through the websocket to the server
        :type msg: string
        :return: when timeout occurred returning: False
        :rtype: bool
        """
        try:
            self.ws.send(msg)
            if expected_event == "" and expected_payload == "" and receive_channel == "":
                self.ws.recv()
            elif expected_event == "" and expected_payload == "" and receive_channel != "":
                message_received = self.__receive_msg_on_channel(receive_channel, timeout)
                if get_response and message_received:
                    return message_received
            else:
                return self.__wait_for_expected_response(
                    expected_event, expected_payload, receive_channel, get_response, timeout
                )
            return True
        except websocket._exceptions.WebSocketTimeoutException as e:  # pylint: disable=protected-access
            LOG.error("Exception: %s", e)
            return False

    # pylint: disable-next=too-many-arguments
    def __wait_for_expected_response(self, expected_event, expected_payload, receive_channel, get_response, timeout):
        if expected_event != "" and expected_payload != "":
            return self.__wait_for_expected_event_and_payload(
                expected_event=expected_event,
                expected_payload=expected_payload,
                receive_channel=receive_channel,
                get_response=get_response,
                timeout=timeout,
            )
        if expected_event != "" and expected_payload == "":
            return self.__wait_for_expected_event(expected_event, receive_channel, get_response, timeout)
        if expected_event == "" and expected_payload != "":
            return self.__wait_for_expected_payload(expected_payload, receive_channel, get_response, timeout)
        return False

    def __wait_for_expected_event(self, expected_event, receive_channel, get_response, timeout):
        start_time = time.time()
        if receive_channel:
            received_msg = self.__receive_msg_on_channel(receive_channel, timeout)
        else:
            received_msg = self.ws.recv()
        if not bool(received_msg):
            LOG.warning("Timeout occurred in recv function while waiting for Event: %s", expected_event)
            return False
        received_json = json.loads(received_msg)
        while received_json["event"] != expected_event:
            if receive_channel:
                received_msg = self.__receive_msg_on_channel(receive_channel, timeout)
            else:
                received_msg = self.ws.recv()
            # Check if recv message exits with timeout
            if not bool(received_msg):
                LOG.warning("Timeout occurred in recv function while waiting for Event: %s", expected_event)
                return False
            # check if no valid message is received in time
            if start_time + timeout < time.time():
                LOG.warning("Timeout occurred while waiting for matching response. Expected Event: %s", expected_event)
                return False
            received_json = json.loads(received_msg)
        if get_response:
            return received_json
        return True

    def __wait_for_expected_payload(self, expected_payload, receive_channel, get_response, timeout):
        start_time = time.time()
        if receive_channel:
            received_msg = self.__receive_msg_on_channel(receive_channel, timeout)
        else:
            received_msg = self.ws.recv()
        if not bool(received_msg):
            LOG.warning("Timeout occurred in recv function while waiting for Event: %s", expected_payload)
            return False
        received_json = json.loads(received_msg)
        while received_json["payload"] != expected_payload:
            if receive_channel:
                received_msg = self.__receive_msg_on_channel(receive_channel, timeout)
            else:
                received_msg = self.ws.recv()
            # Check if recv message exits with timeout
            if not bool(received_msg):
                LOG.warning("Timeout occurred in recv function while waiting for Event: %s", expected_payload)
                return False
            # check if no valid message is received in time
            if start_time + timeout < time.time():
                LOG.warning(
                    "Timeout occurred while waiting for matching response. Expected Event: %s", expected_payload
                )
                return False
            received_json = json.loads(received_msg)
        if get_response:
            return received_json
        return True

    # pylint: disable-next=too-many-arguments
    def __wait_for_expected_event_and_payload(
        self, expected_event, expected_payload, receive_channel, get_response, timeout
    ):
        start_time = time.time()
        if receive_channel:
            received_msg = self.__receive_msg_on_channel(receive_channel, timeout)
        else:
            received_msg = self.ws.recv()
        if not bool(received_msg):
            LOG.warning("Timeout occurred in recv function while waiting for Event: %s", expected_event)
            return False
        received_json = json.loads(received_msg)
        while received_json["event"] != expected_event or received_json["payload"] != expected_payload:
            if receive_channel:
                received_msg = self.__receive_msg_on_channel(receive_channel, timeout)
            else:
                received_msg = self.ws.recv()
            # Check if recv message exits with timeout
            if not bool(received_msg):
                LOG.warning("Timeout occurred in recv function while waiting for Event: %s", expected_event)
                return False
            # check if no valid message is received in time
            if start_time + timeout < time.time():
                LOG.warning("Timeout occurred while waiting for matching response. Expected Event: %s", expected_event)
                return False
            received_json = json.loads(received_msg)
        if get_response:
            return received_json
        return True

    def __receive_msg_on_channel(self, channel, time_out):
        start_time = time.time()
        received_msg = self.ws.recv()
        received_json = json.loads(received_msg)
        while received_json["channel"] != channel:
            received_msg = self.ws.recv()
            received_json = json.loads(received_msg)
            # check if no valid message is received in time
            if start_time + time_out < time.time():
                LOG.warning("Timeout occurred while waiting for message receive in Channel -> %s", channel)
                return False
        return received_msg

    def __sync_send_without_response(self, msg):
        """
        internal function to send a message from the websocket
        without waiting and returning of the response

        :param msg: the message to send through the websocket to the server
        :type msg: string
        :return: when timeout occurred returning: False
        :rtype: bool
        """
        try:
            self.ws.send(msg)
            return True
        except websocket._exceptions.WebSocketTimeoutException as e:  # pylint: disable=protected-access
            LOG.error("Exception: %s", e)
            return False

    def send_with_delay(self, msg, delay_s=5):
        """
        Sends the message after the provided delay

        :param msg: msg to be sent
        :type msg: str
        :param delay_s: time in seconds to wait before sending
        :type delay_s: int
        """
        t = Thread(target=self.__send_with_delay, args=(msg, delay_s))
        t.start()

    def __send_with_delay(self, msg, delay_s):
        time.sleep(delay_s)
        self.ws.send(msg)
        self.ws.recv()

    def recv(self, receive_channel="", time_out=60):
        """
        external function to receive a message from the websocket

        :param receive_channel: Channel to listen to for the responses.
        :type receive_channel: str
        :time_out time in seconds waiting for the receiving data
        :type time_out: int
        :return: the message received from the server through the websocket
                 when timeout occurred returning: False
        :rtype: string
        """
        return self.__sync_recv(receive_channel, time_out)

    # pylint: disable-next=too-many-arguments
    def wait_until_recv(
        self, expected_event="", expected_payload="", receive_channel="", get_response=False, time_out=60
    ):
        """
        Wait until a message is received with the matching arguments

        :param expected_event: Event that the message shall have
        :type expected_event: str
        :param expected_payload: payload that the message shall have
        :type expected_payload: str
        :param receive_channel: channel that the message shall be received on
        :type receive_channel: str
        :param get_response: If set, the received message will be returned, Default False
        :type get_response: bool
        :param time_out: Time in seconds to wait for the expected message
        :type time_out: int
        :return: True or False and message
        :rtype: bool and str
        """
        return self.__wait_for_expected_response(
            expected_event=expected_event,
            expected_payload=expected_payload,
            receive_channel=receive_channel,
            get_response=get_response,
            timeout=time_out,
        )

    def __sync_recv(self, receive_channel, time_out):
        """
        internal function to receive a message from the websocket
        :param receive_channel: Channel to listen to for the responses.
        :type receive_channel: str
        :time_out time in seconds waiting for the receiving data
        :type time_out: int
        :return: the message received from the server through the websocket
                 when timeout occurred returning: False
        :rtype: string
        """

        try:
            if receive_channel:
                return self.__receive_msg_on_channel(receive_channel, time_out)
            return self.ws.recv()
        except websocket._exceptions.WebSocketTimeoutException as e:  # pylint: disable=protected-access
            LOG.error("Exception in websocket.__sync_recv: %s", e)
            return False

    def dump_cache(self, receive_channel=""):
        """
        external function to clean up the cache of the recv function
        """
        self.__dump_cache(receive_channel)

    def __dump_cache(self, receive_channel=""):
        """
        internal function to clean up the cache of the recv function
        """
        old_timeout = self.timeout_s
        self.ws.timeout = 5
        websocket_is_sending = True
        while websocket_is_sending:
            websocket_is_sending = self.recv(receive_channel, self.ws.timeout)

        self.ws.timeout = old_timeout
