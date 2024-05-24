"""
    Copyright 2022 Continental Corporation

    :file: global_Variables.py
    :platform: Windows, Linux
    :synopsis:
        Script containing comparison functions for next sync test

    :author:
        - Bacila Emil-Florin (uib49077) <emil-florin.bacila@continental-corporation.com>
"""

# 100% identical, no samples should be different
THRESHOLD = 100
TOLERANCE = 0

# Define the three lists of column names
list0 = [
    "ARS5xx.AlgoVehCycle.VehDyn.sSigHeader.uiMeasurementCounter",
    "ARS5xx.AlgoVehCycle.VehDyn.sSigHeader.uiTimeStamp",
]

list1 = [
    "ARS5xx.RawDataCycle.ALN_SyncRef.VehDyn.uiMeasurementCounter",
    "ARS5xx.RawDataCycle.ALN_SyncRef.VehDyn.uiTimeStamp",
]

list2 = [
    "SIM VFB.NextSync_0.providePort_VehDyn_no_sync.sSigHeader.uiMeasurementCounter",
    "SIM VFB.NextSync_0.providePort_VehDyn_no_sync.sSigHeader.uiTimeStamp",
]

list3 = [
    "SIM VFB.NextSync_0.providePort_VehDyn_sync_exact.sSigHeader.uiMeasurementCounter",
    "SIM VFB.NextSync_0.providePort_VehDyn_sync_exact.sSigHeader.uiTimeStamp",
]

list4 = [
    "SIM VFB.NextSync_0.providePort_VehDyn_sync_last.sSigHeader.uiMeasurementCounter",
    "SIM VFB.NextSync_0.providePort_VehDyn_sync_last.sSigHeader.uiTimeStamp",
]

list5 = [
    "SIM VFB.NextSync_0.providePort_VehDyn_sync_syncref.sSigHeader.uiMeasurementCounter",
    "SIM VFB.NextSync_0.providePort_VehDyn_sync_syncref.sSigHeader.uiTimeStamp",
]


def compare_columns(column1_name, column2_name, column3_name, column4_name, csv_data):
    """Compares two columns based on their values(for no_sync, exact, last)"""
    # Initialize empty lists to store data from the selected columns
    data = []
    no_sync = []
    exact = []
    last = []

    # Read the most recent CSV file and extract data from the selected columns
    for row in csv_data:
        data.append(row[column1_name])
        no_sync.append(row[column2_name])
        exact.append(row[column3_name])
        last.append(row[column4_name])

    # Calculate the percentage of identical values element by element for all three columns
    identical_count = 0
    total_elements = len(data)

    # Check if rows contain data
    if total_elements == 0:
        return 0

    # implement comparison rule
    for i in range(total_elements):
        if (data[i] == exact[i]) and (no_sync[i] == last[i]) and (no_sync != exact):
            identical_count += 1

    # Calculate the overall similarity percentage
    overall_similarity_percentage = (identical_count / total_elements) * 100

    return overall_similarity_percentage


def validate_rule_for_sync_last(column1_name, column2_name, csv_data):
    """Checks if the rule is valid (for sync last)"""
    # initialize empty lists to store data from the selected columns
    column1_data = []
    column2_data = []

    # read the most recent CSV file and extract data from the selected columns
    for row in csv_data:
        column1_data.append(row[column1_name])
        column2_data.append(row[column2_name])

    invalid = 0

    # get unique elements in the original list
    unique_elements = sorted(set(column2_data))

    # parse the lists of unique items but skip the first, due to data alignment
    for item in unique_elements[1:]:
        first_aparition_of_value = item
        index_of_first_aparition = column2_data.index(first_aparition_of_value)

        data_to_check = column1_data[index_of_first_aparition - 1]

        if data_to_check != item:
            invalid = invalid + 1

    return invalid


def syncref_compare_columns(column1_name, column3_name, column5_name, csv_data):
    """Compares two columns based on their values (for exact, syncref)"""
    # Initialize empty lists to store data from the selected columns
    data = []
    exact = []
    syncref = []

    # Read the most recent CSV file and extract data from the selected columns
    for row in csv_data:
        data.append(row[column1_name])
        exact.append(row[column3_name])
        syncref.append(row[column5_name])

    # Calculate the percentage of identical values element by element for all three columns
    identical_count = 0
    total_elements = len(data)

    # Check if rows contain data
    if total_elements == 0:
        return 0

    # implement comparison rule
    for i in range(total_elements):
        if data[i] == exact[i] == syncref[i]:
            identical_count += 1

    # Calculate the overall similarity percentage
    overall_similarity_percentage = (identical_count / total_elements) * 100

    return overall_similarity_percentage
