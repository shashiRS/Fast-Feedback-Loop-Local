"""
    Copyright 2022 Continental Corporation

    This file contains a class to measure timings of execution commands for NEXT.

    :file: time_measurement.py

    :author:
        - Jannik Fritz (uidr9189) <jannik.fritz@continental-corporation.com>
"""

import logging
import time

LOG = logging.getLogger("__main__")


# pylint: disable-next=too-few-public-methods
class Timings:  # pylint: disable=missing-class-docstring
    def __init__(self):
        self.elapsed_time = 0
        self.last_checked = time.time()


# pylint: disable-next=too-few-public-methods
class TimeMeasurement:  # pylint: disable=missing-class-docstring
    def __init__(self, name="default"):
        self.times = {}
        self.measure_time(name)
        if name != "default":
            self.measure_time("default")

    def measure_time(self, name="default"):
        """
        Returns the time difference for the given name(s)
        If no name is given, the time difference from the last default call is returned.
        If a name is given, the time difference from the last call with this name is returned.
        if a list of names is given, a dict of time difference from last call of each name is returned.
        :param name: name(s) of the time difference(s) that shall be checked
        :type name: str or list
        :return: elapsed time or dict with key = name and value = elapsed time
        :rtype: float or dict
        """
        current_time = time.time()
        if isinstance(name, list):
            elapsed_times = {}
            for item in name:
                elapsed_times[item] = self.measure_time(item)
            return elapsed_times

        if name in self.times:
            elapsed_time = current_time - self.times[name].last_checked
            LOG.info("The operation '%s' took %s seconds.", name, elapsed_time)
            self.times[name].elapsed_time = elapsed_time
            self.times[name].last_checked = current_time
        else:
            LOG.debug("Creating entry for operation '%s'.", name)
            self.times[name] = Timings()

        return self.times[name].elapsed_time
