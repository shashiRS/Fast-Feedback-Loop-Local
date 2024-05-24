"""
    Copyright 2022 Continental Corporation

    This file contains the config class to store Simulation Input entries.

    :file: simulation_input.py

    :author:
        - Jannik Fritz (uidr9189) <jannik.fritz@continental-corporation.com>
        - Alexandru Buraga <alexandru.2.buraga@continental-corporation.com>
"""

import json
import logging
import os

from next_py_api.api_utils.config_classes import path_parsing
from next_py_api.api_utils.helper_classes import helper_functions

LOG = logging.getLogger("__main__")


class SimulationInput:  # pylint: disable=too-few-public-methods
    """
    Contains all available Config Settings
    """

    def __init__(self, input_dict):
        self.recording_batches = []
        self.time_intervals = []
        self._recording_type = []
        self._split = None

        if "Split" in input_dict:
            self._split = input_dict["Split"]

        if "RecordingType" in input_dict:
            self._fill_recording_type(input_dict["RecordingType"])

        # RecordingLocation and SimulationInputFiles are given
        if "RecordingLocation" in input_dict and "SimulationInputFile" in input_dict:
            all_inputs = []
            all_inputs.extend(parse_run_input_files(input_dict["SimulationInputFile"]))
            all_inputs.extend(parse_recording_locations(input_dict["RecordingLocation"]))
            self._fill_recording_batches(all_inputs)
        # Only SimulationInputFiles is given
        elif "RecordingLocation" not in input_dict and "SimulationInputFile" in input_dict:
            all_inputs = parse_run_input_files(input_dict["SimulationInputFile"])
            self._fill_recording_batches(all_inputs)
        # Only RecordingLocation is given
        elif "RecordingLocation" in input_dict and "SimulationInputFile" not in input_dict:
            all_inputs = parse_recording_locations(input_dict["RecordingLocation"])
            self._fill_recording_batches(all_inputs)

        if "TimeInterval" in input_dict:
            self._fill_time_intervals(input_dict["TimeInterval"])

    def __eq__(self, other):
        # workaround because in the next_config_handler the simulation_input is imported different from unit py test
        # if isinstance(other, SimulationInput):
        return self.__dict__ == other.__dict__
        # return False

    def _fill_recording_batches(self, recording_locations):  # pylint: disable=too-many-branches
        # Stores individual files, if split option Folder is given.
        file_folder_split = []
        # Stores all files when split option none is given
        file_none_split = []
        for recording_location in recording_locations:  # pylint: disable=too-many-nested-blocks
            clean_recording_location = []
            clean_locations = path_parsing.get_path(recording_location)
            if isinstance(clean_locations, list):
                clean_recording_location.extend(clean_locations)
            else:
                clean_recording_location.append(clean_locations)
            # Check if location exists
            if clean_recording_location:
                for location in clean_recording_location:
                    if os.path.isdir(location):
                        # Gets all File paths from Folder
                        # TODO: Add recursive option  # pylint: disable=fixme
                        files_from_folder = self._get_files_from_folder(location)
                        if self._split == "File":
                            # Add all files from folder as individual entries
                            self.recording_batches.extend(files_from_folder)
                        elif self._split == "Folder":
                            # Put all single files that are given up until here in the batch and resets list
                            if file_folder_split:
                                self.recording_batches.append(file_folder_split)
                                file_folder_split = []
                            # Add all files from folder as list
                            self.recording_batches.append(files_from_folder)
                        elif self._split is None:
                            # Add all files from folder to temporary list.
                            # Will be added to batch after all locations are parsed
                            file_none_split.extend(files_from_folder)
                    elif os.path.isfile(location):
                        if self._split == "File":
                            # Adds file directly to recording batch
                            self.recording_batches.append(os.path.normpath(location))
                        elif self._split == "Folder":
                            # Adds file to temporary folder split list
                            # Will be added to batch if folder is found or if all locations are parsed
                            file_folder_split.append(os.path.normpath(location))
                        elif self._split is None:
                            # Add file to temporary none split list.
                            # Will be added to batch after all locations are parsed
                            file_none_split.append(os.path.normpath(location))
                    else:
                        LOG.info("%s not found!", location)
        if file_folder_split:
            self.recording_batches.append(file_folder_split)
        if file_none_split:
            self.recording_batches.append(file_none_split)

    def _get_files_from_folder(self, folder_path):
        available_recordings = []
        all_instances = os.listdir(folder_path)
        for entry in all_instances:
            if os.path.isfile(os.path.join(folder_path, entry)):
                file_type = "." + entry.split(".")[-1]
                # if no recording type is given add all entries
                # if recording types are given they need to match the file_type
                if (self._recording_type and file_type in self._recording_type) or (not self._recording_type):
                    available_recordings.append(os.path.normpath(os.path.join(folder_path, entry)))
        return available_recordings

    def _fill_time_intervals(self, time_intervals):
        if isinstance(time_intervals, list):
            for interval in time_intervals:
                self.time_intervals.append(add_time_interval(interval))
        else:
            self.time_intervals.append(add_time_interval(time_intervals))

    def _fill_recording_type(self, recording_types):
        if isinstance(recording_types, list):
            for it_type in recording_types:
                self._recording_type.append(it_type)
        else:
            self._recording_type.append(recording_types)


def parse_run_input_files(run_input_file_dict):
    """
    Function that extracts information from provided file

    :param run_input_file_dict: content form Input File
    :type run_input_file_dict:
    :return:
    :rtype:
    """
    all_inputs = []
    # Gather all recording locations from all available run Input files
    if isinstance(run_input_file_dict, list):
        # Get the locations from all provided Input files
        for input_file in run_input_file_dict:
            locations_from_file = get_recordings_from_file(input_file)
            # Wrong if SimulationInputFile could not be found
            if locations_from_file:
                all_inputs.extend(locations_from_file)
    else:
        locations_from_file = get_recordings_from_file(run_input_file_dict)
        # Wrong if SimulationInputFile could not be found
        if locations_from_file:
            all_inputs.extend(locations_from_file)
    return all_inputs


def parse_recording_locations(recording_locations_dict):
    """
    Gather all recording locations from recording location config

    :param recording_locations_dict: Config Entry for the Simulation Input
    :type recording_locations_dict: dict
    :return: All available recording locations
    :rtype: List
    """
    all_inputs = []
    if isinstance(recording_locations_dict, list):
        for recording in recording_locations_dict:
            all_inputs.append(path_parsing.get_path(recording))
    else:
        all_inputs.append(path_parsing.get_path(recording_locations_dict))
    return all_inputs


def get_recordings_from_file(filepath):
    """
    Function to get available recording locations from a file

    :param filepath: Path to config file containing the input data
    :type filepath: (list of) str
    :return: Available recordings or None
    :rtype: list of str
    """
    try:
        filepath = path_parsing.get_path(filepath)
        if isinstance(filepath, list):
            recording_locations_list = []
            for clean_file in filepath:
                with open(clean_file, encoding="utf-8") as run_input_file:
                    input_file_dict = json.load(run_input_file)
                    # Gather all recording locations from all available run Input files
                    if "RecordingLocation" in input_file_dict:
                        recording_locations_list.extend(parse_recording_locations(input_file_dict["RecordingLocation"]))
                    else:
                        LOG.error("No valid Recordings described in %s", filepath)
            if recording_locations_list:
                return recording_locations_list
            return None

        with open(filepath, encoding="utf-8") as run_input_file:
            input_file_dict = json.load(run_input_file)
            # Gather all recording locations from all available run Input files
            if "RecordingLocation" in input_file_dict:
                return parse_recording_locations(input_file_dict["RecordingLocation"])
            LOG.error("No valid Recordings described in %s", filepath)
            return None
    except FileNotFoundError:
        LOG.error("Run Input File not available with this path %s", filepath)
        return None


def add_time_interval(interval: dict):
    """
    Checks the available time interval, and if available converts it into integer
    :param interval: Time interval entry to be checked
    :type interval: dict
    :return: Time interval created from the config
    :rtype: TimeInterval
    """
    tmp_interval = TimeInterval(0, -1, False)
    if "StartTimeUs" in interval:
        try:
            tmp_interval.start_time = interval["StartTimeUs"]
            tmp_interval.start_time = helper_functions.convert_to_int(tmp_interval.start_time)
        except ValueError as e:
            LOG.error(e)
            LOG.error("Could not add StartTimeUs.")
            LOG.error("Given Value: %s", interval["StartTimeUs"])
    if "EndTimeUs" in interval:
        try:
            tmp_interval.end_time = interval["EndTimeUs"]
            tmp_interval.end_time = helper_functions.convert_to_int(tmp_interval.end_time)
        except ValueError as e:
            LOG.error(e)
            LOG.error("Could not add EndTimeUs.")
            LOG.error("Given Value: %s", interval["EndTimeUs"])
    if "Relative" in interval:
        tmp_interval.relative = interval["Relative"]
    return tmp_interval


class TimeInterval:  # pylint: disable=too-few-public-methods, missing-class-docstring
    def __init__(self, start_time, end_time, relative):
        self.start_time = start_time
        self.end_time = end_time
        self.relative = self.fill_relative(relative)

    def __eq__(self, other):
        # workaround because in the next_config_handler the simulation_input is imported different from unit py test
        # if isinstance(other, TimeInterval):
        return self.__dict__ == other.__dict__
        # return False

    @staticmethod
    def fill_relative(relative):
        """
        Checks if the provided value is valid.
        Transforms string into proper bool representation.

        :param relative: Config Entry
        :type relative: str or bool
        :return: Valid entry
        :rtype: bool
        """
        if isinstance(relative, str):
            if relative.lower() == "false":
                return False
            return bool(relative)
        if isinstance(relative, bool):
            return relative
        LOG.error("Could not evaluate Relative entry '%s'", relative)
        LOG.error("Set relative to None")
        return None
