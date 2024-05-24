#!/usr/bin/python3

import os
import pathlib
import sys

###################################
# file suffixes which are checked #
###################################
suffixes = [".h", ".hpp", ".c", ".cpp"]


def trim_include(s):
    """
    trimming the given string (include statement line) to get only the file / path
    """
    s = s.lstrip("#cdeilnu ")  # "include" alphabetically sorted
    s = s.lstrip('<"')
    s = s.rstrip("\n\r ")
    s = s.rstrip('>"')
    return s


def get_includes(full_path):
    """
    searching all included files of a given file, ignoring commented out lines
    Warn: only line comments are recognized as comments!
    """
    includes = []
    with open(full_path, mode="r", encoding="utf-8") as f:
        while True:
            line = f.readline().lstrip(" ")
            if not line:
                break
            if line.startswith("//"):
                continue
            if "#include" in line:
                trimmed_line = trim_include(line)
                includes.append(trimmed_line)
    return includes


def check_include(src_root, full_path, include):
    """
    check whether the given include file is allowed within the SWC
    the src root dir is used to determine the SWC
    the path of the include file is needed to resolve relative include statements
    """
    d, _ = os.path.split(full_path)
    inc_full_path = os.path.join(d, include)
    norm_inc_full_path = os.path.normpath(inc_full_path)

    swc = full_path[len(src_root) + 1 : full_path.find(os.sep, len(src_root) + 1)]
    swc_path = os.path.join(src_root, swc)
    if not norm_inc_full_path.startswith(swc_path):
        return False
    return True


##############
# start of the main part
##############

if not len(sys.argv) == 2:
    print("Usage: " + sys.argv[0] + " <src dir>")
    sys.exit(1)

src_root = sys.argv[1]
if not os.path.isdir(src_root):
    print("the given path '" + src_root + "' is not a directory")
    sys.exit(1)
src_root = os.path.abspath(src_root)

error_occured = False

for root, subdirs, files in os.walk(src_root):
    for filename in files:
        if pathlib.Path(filename).suffix in suffixes:
            full_path = os.path.join(root, filename)
            includes = get_includes(full_path)
            for inc in includes:
                if not check_include(src_root, full_path, inc):
                    error_occured = True
                    print("file " + full_path + " has include '" + inc + "' which is not allowed")

if error_occured is True:
    sys.exit(1)
else:
    sys.exit(0)
