"""
    File for path handling of the Next API Config.
    Copyright 2023 Continental Corporation

    :file: path_parsing.py
    :synopsis:
        This file contains functions to handle paths given in the configuration.

    :author:
        - Jannik Fritz <jannik.fritz@continental-corporation.com>
"""

import glob
import logging
import os

from next_py_api.api_variables import global_vars

LOG = logging.getLogger("__main__")


def get_path(path_entry, create=False):
    """
    Takes a path from the config and checks:
    1. if path is valid
    2. if ENV variables need to be resolved
    3. if relative paths need to be resolved
    :param create: If this flag is provided, the given path will be created if it is not yet available
    :type create: Boolean
    :param path_entry: Path provided in the config file
    :type path_entry: str
    :return: Returns usable path if it´s available
    :rtype: SUCCESS: str
    :rtype: FAIL: None
    """
    if path_entry:
        if path_entry.startswith("./") or path_entry.startswith(".\\"):
            path_entry = unwrap_relative_path(path_entry)
        if "*" in path_entry:
            replaced_wildcard_path_entry = replace_wildcard_in_path(path_entry)
            path_entry = []
            for entry in replaced_wildcard_path_entry:
                if not (os.path.isdir(entry) or os.path.isfile(entry)):
                    LOG.debug("Could not find path: %s", entry)
                    if create:
                        LOG.debug("Creating path %s", entry)
                        os.makedirs(os.path.normpath(entry))
                path_entry.append(os.path.normpath(entry))
        else:
            if not (os.path.isdir(path_entry) or os.path.isfile(path_entry)):
                LOG.debug("Could not find path: %s", path_entry)
                if create:
                    LOG.debug("Creating path %s", path_entry)
                    os.makedirs(os.path.normpath(path_entry))
            path_entry = os.path.normpath(path_entry)
    return path_entry


def unwrap_relative_path(filepath):
    """
    Takes a relative path from the configuration and tries to resolve it.
    Order of check:
    1. Check if Environment variable "NEXT_API_PATH" exists. If so, take this as root
    2. Check if the config contains the GlobalSetting "ApiRoot". If so, take this as root
    3. Take the current working directory as root
    :param filepath: Path from config that should be handled as relative path
    :type filepath: str
    :return: Full normpath of the provided relative path
    :rtype: str
    """
    next_env_var = "NEXT_API_ROOT"
    # get rid of leading level
    if filepath.startswith(".\\") or filepath.startswith("./"):
        filepath = filepath[2:]
    if os.getenv(next_env_var):
        return os.path.normpath(os.path.join(os.getenv(next_env_var), filepath))
    if global_vars.root_dir:
        return os.path.normpath(os.path.join(global_vars.root_dir, filepath))
    return os.path.normpath(os.path.join(os.getcwd(), filepath))


def replace_wildcard_in_path(filepath):
    """
    Takes a path and replaces all wildcards (*) in it.
    All valid entries will be returned as a list.
    :param filepath: Path to replace wildcards in
    :type filepath: str
    :return: All files with replaced wildcard
    :rtype: list
    """
    full_path = glob.glob(filepath)
    all_files = [os.path.normpath(x) for x in full_path]
    return all_files
