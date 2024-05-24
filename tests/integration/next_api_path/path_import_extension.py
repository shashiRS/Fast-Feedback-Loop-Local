"""Path hack to make tests work."""
import os
import sys

DIR_PATH = os.path.dirname(os.path.realpath(__file__))
print(f"PATH_IMPORT_EXTENSION  DIR_PATH: {DIR_PATH}")
NEXT_API_DIR = os.path.join(DIR_PATH, "..", "..", "..", "src", "py_api", "next_py_api")
NEXT_API_ROOT_DIR = os.path.join(DIR_PATH, "..", "..", "..", "src", "py_api")
print(f"PATH_IMPORT_EXTENSION  NEXT_API_DIR: {NEXT_API_DIR}")
if os.path.isdir(NEXT_API_DIR):
    sys.path.append(NEXT_API_ROOT_DIR)
    sys.path.append(NEXT_API_DIR)
else:
    print(f"Next api directory is not found. Dir checked: '{NEXT_API_DIR}'")
