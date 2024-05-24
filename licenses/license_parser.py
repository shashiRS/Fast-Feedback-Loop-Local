#!/usr/bin/env python3

import csv
import os

# dictionary with libname as key and lib version as value
markdown_header = [
    "Licenses {#licenses_main}\n",
    "======\n",
    "\n",
    "[TOC]\n",
    "\n",
    "# Overview {#licenses_overview}\n",
    "\n",
    "Here you can find all used third-party libraries.\n",
    "\n",
    "| Library Name | Version | License|\n",
    "|--------------|---------|--------|\n",
]
# dictionary with libname as key and lib version as value
used_libraries = {}

# Sort the input csv file alphabetically
with open(r"thirdparty-licenses.csv", "r") as libraries:
    unsorted_libraries = libraries.readlines()
    unsorted_libraries.sort(key=str.lower)

with open(r"thirdparty-licenses.csv", "w") as libraries:
    libraries.write("".join(unsorted_libraries))

with open(r"thirdparty-licenses.csv", newline="") as csvfile:
    license_reader = csv.reader(csvfile, delimiter=",")
    with open(r"../doc/licenses.md", "w") as licenses_md:
        licenses_md.write("".join(markdown_header))
        # Creates all files for the libraries mentioned
        for row in license_reader:
            used_libraries[row[0].replace(" ", "")] = row[1].replace(".", "_")
            filename = (
                f"{row[0].replace(' ', '')}-{row[1].replace('.', '_')}-{row[2].replace(' ', '_').replace('.', '_')}.md"
            )

            # If the file already exists, check if its also in the Markdown.
            # If not, add it there but skip file creation
            if os.path.isfile(filename):
                licenses_md.write(f"|{row[0]}|{row[1]}|[{row[2]}](licenses/{filename})|\n")
                continue

            with open(filename, "w") as f:
                f.write(f"Library Name: {row[0]} <br>\n")
                f.write(f"Library Version: {row[1]} <br>\n")
                f.write(f"Library License: {row[2]} <br>\n")
                f.write(f"Library License Link: {row[3]} <br>\n<br>")
                f.write("```text\n\n```")

            # Update markdown file to include licenses in Doxygen Docu if not already existing
            licenses_md.write(f"|{row[0]}|{row[1]}|[{row[2]}](licenses/{filename})|\n")

# Check library name and version number and removed unused files
files = [f for f in os.listdir(".") if os.path.isfile(f)]
for file in files:
    if file.endswith(".md"):
        if file.split("-")[0] not in used_libraries.keys() or used_libraries[file.split("-")[0]] != file.split("-")[1]:
            os.remove(file)
