"""
    Copyright 2022 Continental Corporation

    This file contains helper functions for the Next API.

    :file: helper_functions.py

    :author:
        - Jannik Fritz (uidr9189) <jannik.fritz@continental-corporation.com>
        - Alexandru Buraga <alexandru.2.buraga@continental-corporation.com>
"""

import json
import logging
import os
import re
import typing

import jsonschema  # pylint: disable=import-error
from next_py_api.api_utils.config_classes import path_parsing
from next_py_api.api_variables import global_vars

LOG = logging.getLogger("__main__")


def return_json_content(input_json_file: str):
    """
    Function to return the content of json file

    :param str input_json_file: path to the json file
    """
    if os.path.isfile(input_json_file):
        try:
            with open(input_json_file, encoding="utf-8") as json_file:
                json_content = json.load(json_file)
                return json_content
        except FileNotFoundError as e:
            LOG.error(e)
            return None
        except Exception as e:  # pylint: disable=broad-exception-caught
            LOG.error("Error in read json content of %s file. Error: %s", json_file, e)
            return None
    elif isinstance(input_json_file, str):
        try:
            json_content = json.loads(input_json_file)
            return json_content
        except Exception as e:  # pylint: disable=broad-exception-caught
            LOG.error("Error in reading json content of this input: %s.\n Error: %s", input_json_file, e)
            return None
    else:
        LOG.error("Input JSON file %s is not found.", input_json_file)
        return None


def validate_json_with_schema(
    input_json_file: str = "", input_dict=None, json_schema_file: str = global_vars.API_CONFIG_SCHEMA
):
    """
    Function to check if the content of json is valid, need a json_schema

    :param str input_json_file: path to the json file
    :param dict input_dict: json config already formatted as a dict
    :param str json_schema_file: path to the json schema file
    """
    if input_dict is None:
        input_dict = {}
    schema_json_content = return_json_content(json_schema_file)
    if input_json_file:
        input_json_content = return_json_content(input_json_file)
    elif input_dict:
        input_json_content = input_dict
    else:
        return False
    try:
        jsonschema.validate(instance=input_json_content, schema=schema_json_content)
        return True
    except jsonschema.exceptions.SchemaError as e:
        LOG.error("Error in Schema:\n%s", e)
        return False
    except jsonschema.exceptions.ValidationError as e:
        LOG.error("The provided configuration is not valid here: %s", list(e.absolute_path))
        LOG.error("The instance that caused the error is: %s", e.instance)
        LOG.error("The reason for the failed validation is: %s", e.message)
        LOG.error("Description for the Object:\n%s", e.schema["description"])
        return False


def return_env_variable_name(var_str: typing.Any):
    """
    Function to return the environment variable name
    var should match "$ENV(NAME)$"

    :param any var_str: string that contains environment variable name; should match  "$ENV(NAME)$"
    :return : return the name of environment variable
    :rtype: Success: str
    :rtype: Fail: None
    """
    if var_str:
        if "$ENV" in var_str:
            env_var_name = re.findall(r"\$ENV\((.+?)\)\$", var_str)
            if env_var_name:
                env_variable_name = env_var_name[0]
                return env_variable_name
            LOG.warning("ERROR: Could not extract the environment variable. Variable used: %s", var_str)
            return None
    return var_str


def unwrap_env_variable(env_var_str, return_unsuccessful_case_value=None):
    """
    Function to unwrap the environment variable if it's present
    var should match "$ENV(NAME)$"

    :param any env_var_str: entry from config that get´s checked for environment variables
    :param any return_unsuccessful_case_value: the value returned in case couldn't unwrap
    """
    if env_var_str == "" or env_var_str is None:
        return env_var_str
    if isinstance(env_var_str, list):
        env_var_list = []
        for i_env_var in env_var_str:
            env_var_list.append(unwrap_env_variable(i_env_var))
        return env_var_list
    if isinstance(env_var_str, str):
        env_var_name = return_env_variable_name(env_var_str)
        if env_var_name:
            env_var_value = os.getenv(env_var_name)
            if env_var_value:
                if os.path.isdir(env_var_value):
                    # convert to raw string
                    env_var_new_value = path_parsing.unwrap_relative_path(env_var_value)
                    env_var_value_raw = os.path.normpath(env_var_new_value).replace("\\", "\\\\")
                    return env_var_value_raw
                return env_var_value
            LOG.error("Could not find the value of the environment variable.")
        LOG.error(
            "The given variable will not be unwrapped and will return the return_unsuccessful_case_variable: %s.",
            return_unsuccessful_case_value,
        )
        LOG.error("Variable used: %s", env_var_str)

        return return_unsuccessful_case_value

    return env_var_str


def convert_to_int(config_entry):
    """
    Tries to convert the provided variable into an integer

    :param config_entry: Value to convert
    :type config_entry: any
    :return: Converted Integer, original value when not possible
    :rtype: int or any
    """
    if isinstance(config_entry, str):
        config_entry = config_entry.strip("\"'")
    try:
        return int(config_entry)
    except ValueError as e:
        LOG.warning("Could not convert %s to integer. Using it unchanged.", config_entry)
        LOG.warning(e)
        return config_entry


def get_matched_list(re_pattern: str, config: list) -> list:
    """
    Function to return a list of matched patterns
    Use regex expressions

    :param str re_pattern: pattern string
    :param list config: the list of configuration entries
    :return the list of matched patterns
    """
    list_matched_patterns = []
    if re_pattern:
        # split the flows
        pattern_list = re_pattern.split(" ")
        if pattern_list:
            for element in pattern_list:
                # for the case when more than one space is between the parameters
                if element:
                    re_compiled = re.compile(element)
                    list_matched_patterns.extend(list(filter(re_compiled.match, config)))
        if not list_matched_patterns:
            LOG.warning(
                "The selected pattern from command line doesn't match the list of keys."
                "pattern: '%s'; "
                "list keys %s",
                re_pattern,
                list(config),
            )

    return list_matched_patterns


def return_matched_patterns(re_pattern: str, config: list) -> list:
    """
    Function to return a list of matched patterns
    Use regex expressions

    :param str re_pattern: pattern string
    :param list config: the list of configuration entries
    :return the list that contains only the config matched patterns elements
    """
    list_matched_pattern = get_matched_list(re_pattern=re_pattern, config=config)
    set_matched_pattern = set(list_matched_pattern)
    if config:
        return [x for x in config if x in set_matched_pattern]
    return []


def return_unmatched_patterns(re_pattern: str, config: list) -> list:
    """
    Function to return a list which would not have the matched patterns
    Use regex expressions

    :param str re_pattern: pattern string
    :param list config: the list of configuration entries
    :return the list that contains only the config unmatched(filtered) patterns elements
    """
    list_matched_pattern = get_matched_list(re_pattern=re_pattern, config=config)
    set_matched_pattern = set(list_matched_pattern)
    if config:
        return [x for x in config if x not in set_matched_pattern]
    return []


def save_env_var(var_name):
    """
    Save the current value of an environment variable.
    """
    prev_var_name = f"PREV_{var_name}"
    # Check if environment variable "PREV_var_name" has a value (Can occure in case last processing crashed.
    # In this case "var_name" won't be reset to "PREV_var_name" and "PREV_var_name" won't be deleted).
    # In this case first restore "var_name" to "PREV_var_name"
    if os.getenv(prev_var_name):
        LOG.info(
            "%s has a value. Previous run wasn't cleaned up properly. Restore %s env variable to previous value: %s",
            prev_var_name,
            var_name,
            os.getenv(prev_var_name),
        )
        os.environ[var_name] = os.getenv(prev_var_name)
    os.environ[prev_var_name] = ""
    # ensure that the env variable is set before
    if os.getenv(var_name):
        LOG.info(
            "Save the environment variable %s to %s to be able to restore it at the end. The saved value is: %s",
            var_name,
            prev_var_name,
            os.getenv(var_name),
        )
        os.environ[prev_var_name] = os.getenv(var_name)


def restore_env_var(var_name):
    """
    Restore the previous value of an environment variable.
    """
    prev_var_name = f"PREV_{var_name}"
    if os.getenv(prev_var_name):
        # if the env variable was not set before
        if os.getenv(prev_var_name) == "":
            LOG.info("Delete the next environment variable: %s", prev_var_name)
            del os.environ[var_name]
        else:
            os.environ[var_name] = os.getenv(prev_var_name)
            LOG.info(
                "Restore the next environment variable: %s to the value from the beginning (%s) ",
                prev_var_name,
                os.getenv(prev_var_name),
            )
            LOG.info("Delete the next environment variable: %s", prev_var_name)
            del os.environ[prev_var_name]
