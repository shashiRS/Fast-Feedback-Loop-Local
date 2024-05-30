"""
    Copyright 2022 Continental Corporation

    :file: validate_chain_test.py
    :platform: Windows, Linux
    :synopsis:
        Script containing comparison functions for next chain test

    :author:
        - Geleriu Ioan Ovidiu (uie72136) <ioan-ovidiu.geleriu@continental-corporation.com>
"""

# 100% identical, no samples should be different
THRESHOLD = 100

# Define the three lists of column names
list1 = [
    "ADC4xx.EMF.EmfGlobalTimestamp.u_VersionNumber",
    "ADC4xx.EMF.EmfGlobalTimestamp.SigHeader.uiTimeStamp",
    "ADC4xx.EMF.EmfGlobalTimestamp.SigHeader.uiMeasurementCounter",
    "ADC4xx.EMF.EmfGlobalTimestamp.SigHeader.uiCycleCounter",
    "ADC4xx.EMF.EmfGlobalTimestamp.SigHeader.eSigStatus",
]

list2 = [
    "SIM VFB.NextChain_0.providePort_EmfGlobalTimestamp.versionNumber",
    "SIM VFB.NextChain_0.providePort_EmfGlobalTimestamp.sSigHeader.uiTimeStamp",
    "SIM VFB.NextChain_0.providePort_EmfGlobalTimestamp.sSigHeader.uiMeasurementCounter",
    "SIM VFB.NextChain_0.providePort_EmfGlobalTimestamp.sSigHeader.uiCycleCounter",
    "SIM VFB.NextChain_0.providePort_EmfGlobalTimestamp.sSigHeader.eSigStatus",
]

list3 = [
    "SIM VFB.NextChain_1.providePort_EmfGlobalTimestamp.versionNumber",
    "SIM VFB.NextChain_1.providePort_EmfGlobalTimestamp.sSigHeader.uiTimeStamp",
    "SIM VFB.NextChain_1.providePort_EmfGlobalTimestamp.sSigHeader.uiMeasurementCounter",
    "SIM VFB.NextChain_1.providePort_EmfGlobalTimestamp.sSigHeader.uiCycleCounter",
    "SIM VFB.NextChain_1.providePort_EmfGlobalTimestamp.sSigHeader.eSigStatus",
]


def compare_columns(column1_name, column2_name, column3_name, csv_data):
    """Compares three columns based on their names"""
    # Initialize empty lists to store data from the selected columns
    column1_data = []
    column2_data = []
    column3_data = []

    # Read the most recent CSV file and extract data from the selected columns
    for row in csv_data:
        column1_data.append(row[column1_name])
        column2_data.append(row[column2_name])
        column3_data.append(row[column3_name])

    # Calculate the percentage of identical values element by element for all three columns
    identical_count = 0
    total_elements = len(column1_data)

    # Check if rows contain data
    if total_elements == 0:
        return 0

    for i in range(total_elements):
        if column1_data[i] == column2_data[i] == column3_data[i]:
            identical_count += 1

    # Calculate the overall similarity percentage
    overall_similarity_percentage = (identical_count / total_elements) * 100

    return overall_similarity_percentage
