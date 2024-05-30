"""
    Copyright 2022 Continental Corporation

    This file contains the next websocket class to communicate with the NEXT backend.

    :file: next_websocket.py

    :author:
        - Jannik Fritz (uidr9189) <jannik.fritz@continental-corporation.com>
"""

# Local application imports
from next_py_api.api_variables import global_vars
from next_py_api.api_websocket import websocket_sync


class NextWebsocket:
    """
    Class to store the functionality for the websockets used in the Next API
    """

    def __init__(self):
        self.databridge = None  # websocket_sync.Handler()
        self.controlbridge = None  # websocket_sync.Handler()
        self.databridge_broadcast = None  # websocket_sync.Handler()

    def start_next_websockets(self, ports=global_vars.Ports()):
        """
        Starts a websocket with the provided port settings
        :param ports: Port settings that shall be used for the websocket
        :type ports: Ports
        """
        self.databridge = self.__start_websocket(global_vars.DATABRIDGE_IP_ADDRESS, ports.databridge)
        self.databridge_broadcast = self.__start_websocket(
            global_vars.DATABRIDGE_IP_ADDRESS, ports.databridge_broadcast
        )
        self.controlbridge = self.__start_websocket(global_vars.CONTROLBRIDGE_IP_ADDRESS, ports.controlbridge)

    def close_next_websockets(self):
        """
        Closes all available websockets
        """
        if self.databridge:
            self.__close_websocket(self.databridge)
        if self.databridge_broadcast:
            self.__close_websocket(self.databridge_broadcast)
        if self.controlbridge:
            self.__close_websocket(self.controlbridge)

    def dump_cache(self):
        """
        Clears the cache of all websockets
        """
        self.databridge.dump_cache()
        self.databridge_broadcast.dump_cache()
        self.controlbridge.dump_cache()

    @staticmethod
    def __start_websocket(ip_address: str, port: str):
        return websocket_sync.Handler(ip_address, port)

    @staticmethod
    def __close_websocket(websocket):
        websocket.close()
