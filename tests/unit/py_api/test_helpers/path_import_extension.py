"""Path hack to make tests work."""

import os
import sys


def set_import_path():
    """
    Sets the root path for the Next API
    """
    dir_path = os.path.dirname(os.path.realpath(__file__))
    print(f"PATH_IMPORT_EXTENSION  DIR_PATH: {dir_path}")
    next_api_dir = os.path.join(dir_path, "..", "..", "..", "..", "src", "py_api", "next_py_api")
    next_api_root_dir = os.path.join(dir_path, "..", "..", "..", "..", "src", "py_api")
    print(f"PATH_IMPORT_EXTENSION  NEXT_API_DIR: {next_api_dir}")
    if os.path.isdir(next_api_dir):
        sys.path.append(next_api_root_dir)
        sys.path.append(next_api_dir)
    else:
        print(f"Next api directory is not found. Dir checked: '{next_api_dir}'")
