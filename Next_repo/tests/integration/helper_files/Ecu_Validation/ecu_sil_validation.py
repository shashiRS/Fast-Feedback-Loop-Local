"""
    Copyright 2022 Continental Corporation

    :file: global_Variables.py
    :platform: Windows, Linux
    :synopsis:
        Script containing only global variables shared for the softwaretests

    :author:
        - Bacila Emil-Florin (uib49077) <emil-florin.bacila@continental-corporation.com>
"""

import csv
import os

import validate_chain_test
import validate_sync_test


def read_csv_into_dict_list(csv_path):
    """Read the CSV file into a list of dictionaries"""
    csv_data = []
    with open(csv_path, mode="r", newline="", encoding="utf-8") as csv_file:
        for row in csv.DictReader(csv_file):
            csv_data.append(row)

    return csv_data


def print_content_of_file(file_name):
    """Reads the file and prints the content"""
    print(f"reading {file_name}")
    with open(file_name, "r", encoding="utf-8") as file:
        for line in file:
            print(line)


def validate_chain_test_output(csv_path):
    """Read the csv and then checks the file for specific values in columns for chain testing logic"""
    csv_data = read_csv_into_dict_list(csv_path)

    file_name = "validation_output_nextchain.txt"
    if os.path.exists(file_name):
        os.remove(file_name)

    columns_not_identical = 0
    # Open the file in write mode ('w')
    with open(file_name, "w", encoding="utf-8") as file:
        # Iterate through the column triplets and compare them
        for column1, column2, column3 in zip(
            validate_chain_test.list1, validate_chain_test.list2, validate_chain_test.list3
        ):
            result = validate_chain_test.compare_columns(column1, column2, column3, csv_data)
            if result < validate_chain_test.THRESHOLD:
                line = f"{column1}, {column2}, {column3} - {result}% \n"
                print(line)
                file.write(line)
                columns_not_identical += 1
        line = f"{columns_not_identical} not above the threshold {validate_chain_test.THRESHOLD}"
        print(line)
        file.write(line)

    print_content_of_file(file_name)

    if columns_not_identical == 0:
        print("TEST - chain - PASS")
        return True

    print("TEST - chain - FAIL")
    return False


def validate_sync_test_output(nextsync_mode, csv_path):
    """Read the csv and then checks the file for specific values in columns for sync testing logic"""
    print(f"Analyzing output: {csv_path} with {nextsync_mode}")

    file_name = f"validation_output_{nextsync_mode}.txt"
    if os.path.exists(file_name):
        os.remove(file_name)

    columns_not_identical = 0
    rules_not_met = 0
    # Open the file in write mode ('w')
    with open(file_name, "w", encoding="utf-8") as file:
        # Iterate through the column and compare them
        for column0, column1, column2, column3, column4, column5 in zip(
            validate_sync_test.list0,
            validate_sync_test.list1,
            validate_sync_test.list2,
            validate_sync_test.list3,
            validate_sync_test.list4,
            validate_sync_test.list5,
        ):
            if nextsync_mode in ["nextsync_cycleid", "nextsync_timestamp"]:
                # comparison matters for: no_sync, exact, last
                result = validate_sync_test.compare_columns(
                    column1, column2, column3, column4, read_csv_into_dict_list(csv_path)
                )
                sync_last_valid = validate_sync_test.validate_rule_for_sync_last(
                    column0, column4, read_csv_into_dict_list(csv_path)
                )

                if result < validate_sync_test.THRESHOLD:
                    line = f"{column0}, {column1}, {column2}, {column3}, {column4} - {result}% \n"
                    print(line)
                    file.write(line)
                    columns_not_identical += 1

                if sync_last_valid != validate_sync_test.TOLERANCE:
                    line = f"{column0}, {column4} - {sync_last_valid} rules not met \n"
                    print(line)
                    file.write(line)
                    rules_not_met += 1

            elif nextsync_mode == "nextsync_syncref":
                # comparison matters for: syncref
                result = validate_sync_test.syncref_compare_columns(
                    column1, column3, column5, read_csv_into_dict_list(csv_path)
                )

                if result < validate_sync_test.THRESHOLD:
                    line = f"{column1}, {column3}, {column5} - {result}% \n"
                    print(line)
                    file.write(line)
                    columns_not_identical += 1
            else:
                print("ERROR: Can not identify the right sync mode, cases are not reached properly.")
                return False

        line = (
            f"{columns_not_identical} not above the threshold {validate_sync_test.THRESHOLD} and "
            f"{rules_not_met} validation rules fails"
        )
        print(line)
        file.write(line)

    print_content_of_file(file_name)

    if (columns_not_identical == 0) and (rules_not_met == 0):
        print(f"TEST - {nextsync_mode} - PASS")
        return True

    print(f"TEST - {nextsync_mode} - FAIL")
    return False
