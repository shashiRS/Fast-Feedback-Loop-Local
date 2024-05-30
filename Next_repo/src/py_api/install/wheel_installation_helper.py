"""
    Copyright 2022-2023 Continental Corporation
    :file: wheel_installation_helper.py
    :platform: Windows, Linux
    :synopsis:
        Script containing common helper functions
    :author:
        - Shubham Deshpande<shubham.abhay.2.deshpande@continental-corporation.com>
"""

import glob
import os
import shutil

import yaml  # pylint: disable=import-error


def delete_wheel_build_metadata_files(util_folder):
    """
    Function to delete metadata folders 'build' and 'egg-info' after bdist_wheel command
    :param string util_folder: Location containing metadata folders
    """
    delete_folders = "./build ./*.egg-info".split(" ")
    for folder in delete_folders:
        absolute_path = glob.glob(os.path.normpath(os.path.join(util_folder, folder)))
        for path in [str(p) for p in absolute_path]:
            print(f"removing {path}")
            shutil.rmtree(path)


def get_info_yml_content(util_info_yml_file):
    """
    Function to read info.yml file and return its content
    :param string util_info_yml_file: Path to a particular utility info.yml file
    :return: info.yml file content
    :rtype: Dictionary
    """
    with open(util_info_yml_file, encoding="utf-8") as f:
        config = yaml.safe_load(f)
    return config


def check_folders_and_create(folders_list):
    """
    Function to check if the folders exist, if not create it
    :param list folders_list: list of folders path
    :return: return True if not error raised
    :rtype: bool
    """
    try:
        for folder in folders_list:
            if not os.path.isdir(folder):
                print(f"Folder {folder} doesn't exist. Created...")
                os.mkdir(folder)
    except Exception as e:  # pylint: disable=broad-exception-caught
        print("Error in check or creation of folder. Exception: %s", e)
        return False
    return True
