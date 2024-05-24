"""
    Copyright 2022 Continental Corporation

    :file: eCal_handler.py
    :platform: Windows, Linux
    :synopsis:
        Script containing the APIs of the eCal function for mockComponent

    :author:
        - Fabian Janker (uif41320) <fabian.janker@continental-corporation.com>
"""
# standard Python import area
import os
import subprocess
import sys
import time
from threading import Thread

# custom import area
import ecal.core.core as ecal_core
from ecal.core.publisher import StringPublisher
from helper_files.DataBridge import databridge_Variables
from helper_files.General import global_Variables


class Handler:
    """
    This is a class to create an eCal handler for the mockComponent.

    external functions:
        Handler(sdl_file_name),
        startSignalinThread(self, delay_s=0),
        startSignal(self),
        stopSignal(self),
        closeSignal(self),
    """

    def __init__(self, sdl_file_name):
        """
        internal initialization function of the eCal handler

        :param sdl_file_name: contains the sdl file name
        :type sdl_file_name: string
        """

        self.SDL_FILE_NAME = sdl_file_name

        current_path = os.getcwd()

        os.chdir(global_Variables.DUMMY_COMPONENT_DIR)

        self.dummyComponent_proc = subprocess.Popen(
            databridge_Variables.exe_dummy_component(self.SDL_FILE_NAME),
            creationflags=subprocess.CREATE_NEW_CONSOLE,
            shell=False,
        )
        # waiting that the subprocess has fully started
        time.sleep(5)
        os.chdir(current_path)

        ecal_core.initialize(sys.argv, "dummy_component_control")
        self.start_signal = StringPublisher("startPublishData")
        self.stop_signal = StringPublisher("stopPublishData")

        # waiting that the topics has fully published
        time.sleep(5)

    def startSignalinThread(self, delay_s=0):
        """
        external function to start the publisher of a signal with a delay in sec on a thread

        :param delay_s: delay time in seconds, defaults to 0
        :type delay_s: int, optional
        """
        t = Thread(target=self.__startSignalinThread, args=[delay_s])
        t.start()

    def __startSignalinThread(self, delay_s):
        """
        internal function to start the publisher of a signal with a delay in sec on a thread

        :param delay_s: delay time in seconds
        :type delay_s: int
        """
        time.sleep(delay_s)
        self.start_signal.send("s")

    def startSignal(self):
        """
        external function to start the publisher of a signal whithout a thread
        """
        self.__startSignal()

    def __startSignal(self):
        """
        internal function to start the publisher of a signal whithout a thred
        """
        self.start_signal.send("s")

    def stopSignal(self):
        """
        external function to stop the publisher of the signal
        """
        self.__stopSignal()

    def __stopSignal(self):
        """
        internal function to stop the publisher of the signal
        """
        self.stop_signal.send("a")

    def closeSignal(self):
        """
        external function to close the mockComponent
        """
        self.__closeSignal()

    def __closeSignal(self):
        """
        internal functon to close the mockComponent
        """
        pub = StringPublisher("waitingForKill")
        time.sleep(5)
        pub.send("Kill")
        time.sleep(5)
        # finalize eCAL API
        ecal_core.finalize()

        # waiting a short time to finalize the dummy signal
        time.sleep(5)
        self.dummyComponent_proc.terminate()

        if self.dummyComponent_proc is not None:
            self.dummyComponent_proc.kill()
