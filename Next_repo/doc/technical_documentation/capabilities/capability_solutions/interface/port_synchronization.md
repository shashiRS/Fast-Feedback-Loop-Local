NEXT Interface Synchronization {#port_synchronization}
==========================

Synchronization:
* @ref sync_modes
* @ref sync_algos
* @ref ts_sync_mode
* @ref cycleid_sync_mode
* @ref syncref_sync_mode

Goal is to sync up all different ports as close as possible to the recording

* per trigger all ports of all triggered components need to be sorted (synchronized)

There are currently 3 synchronization modes and 4 synchronization algorithms which are used to get port data.

# Synchronization algorithms {#sync_algos}

- **GET_LAST_ELEMENT** - gets the latest data package from the Udex output Queue.
- **FIND_EXACT_VALUE** - gets the data package which has the same signal value(from inside the package) as a given input value
- **FIND_NEAREST_VALUE** - gets the data package which has the nearest signal value(from inside the package) as a given input value
- **FIND_EXACT_HEADER_TIMESTAMP** - get the data package which has the same mts timestamp as a given input timestamp

# Synchronizing modes {#sync_modes}

- **Timestamp**
- **CycleId**
- **SyncRef**

# Timestamp synchronization mode {#ts_sync_mode}

The user specifies inside the *ComponentInputPortConfig.sync_mapping.signal_url_timestamp.synchronized_port_member_name* a URL to 
a timestamp member which will be used as the compare signal inside the selected synchronization algorithm.  
When we want to select data for that specific port we use the above mentioned signal and compare it with a given value using the 
selected synchronization algorithm


# CycleId synchronization mode {#cycleid_sync_mode}

The user specifies inside the *ComponentInputPortConfig.sync_mapping.signal_url_syncid.synchronized_port_member_name* a URL to 
a measurement counter member which will be used as the compare signal inside the selected synchronization algorithm.  
When we want to select data for that specific port we use the above mentioned signal and compare it with a given value using the 
selected synchronization algorithm


# SyncRef synchronization mode {#syncref_sync_mode}

In the previous methods we cannot ensure that the data is fed to the algorithms like it was on the ECU. For this we 
need some extra input in order to correctly select the port data. For this a special package is measfreezed which 
helps to identify which port data should be selected for this specific cycle. 
This mode ignores the selected synchronziation algorithm since it is a 3 step process.  
1. Select data which will be used to extract the correct sync ref package. The trigger timestamp is used as a comparison value with 
the mts package timestamp to correctly select the sync ref selection data.
2. Extract the correct syncref package. This is done with data from point 1 and compared to the URL specified by the developer 
3. Compare the value from point 3 with a specific signal from inside the package. The developer can define this signal in 2 ways. 
using the configuration system:
    - at component level using the following key: <component>.sync.syncref_url
    - at port level using the following key: <component>.in_port.<port_name>.sync.syncref_url
If the values match then the package is extracted.
 The specific signal from inside the package can be specified by the component developer by setting one of these 2 values:
    - ComponentInputPortConfig.sync_mapping.signal_url_timestamp.synchronized_port_member_name
    - ComponentInputPortConfig.sync_mapping.signal_url_syncid.synchronized_port_member_name

The developer has the possibility to specify if he wants to use the timestamp or the sync ID URLs for syncronization by 
setting the following configuration key:
 -  *<component_name>.in_port.<port_name>.sync.syncref.mode* and they can set one of the folowing values:
    -  "Timestamp" the synchronization algo will use the following 2 variables when comparing values:
       - ComponentInputPortConfig.sync_mapping.signal_url_timestamp.synchronized_port_member_name
       - ComponentInputPortConfig.sync_mapping.signal_url_timestamp.syncref_signal_url
    -  "SyncId" the synchronization algo will use the following 2 variables when cpomapring values:
       -  ComponentInputPortConfig.sync_mapping.signal_url_syncid.synchronized_port_member_name
       -  ComponentInputPortConfig.sync_mapping.signal_url_syncid.syncref_signal_url

## Workflow for selecting data using a Syncref package  
@startuml
participant Trigger
collections SyncRefSelectionData
collections SyncRefPackage
Trigger -> SyncRefSelectionData: trigger timestamp
note over SyncRefSelectionData
based on the trigger timestamp
select the correct
sync ref selection data
end note
SyncRefSelectionData -> SyncRefSelectionData: select entry based on input
SyncRefSelectionData-> SyncRefPackage: sync ref selection ts
note over SyncRefPackage
based on the timestamp
select the correct
sync ref selection data
end note
SyncRefPackage -> SyncRefPackage: select entry based on input
SyncRefPackage -> Port1 : selection ts Port1
SyncRefPackage -> Port2 : selection ts Port2
SyncRefPackage -> Port3 : selection ts Port3
@enduml
  
## Example of setting up the full chain for sync ref usage

### Step 1: Update player configuration

The users have to update the configuration for the player. They have to update the orchestrator flow configuration with info regarding the 
syncref selection data URL. They have to add the **meta_info** node which includes a **ref_value** subnode which holds the actual URL:
```
"orchestrator": {
      "flow1": {
        "meta": {
          "delay": 0,
          "sync": true
        },
        "type": "signal_name",
        "value": "ARS620.AlignmentCycle.ALN_SyncRefALNMeas",
        "meta_info":{
            "ref_value" : "ARS620.AlignmentCycle.ALN_SyncRefALNMeas.sigHeader.uiTimeStamp"
        },
        "clients": [
          [
            "EmfNodeTriggering",
            "runSequencer_response"
          ]
        ]
      }
    }
```

### Step2: Setting up the component configuration for SyncRef synchronization

The user can define inside the component configuration all the details for using the SyncRef syncrhonization.
Besides the normal configuration the user has to add/modify the following keys:
- **in_port.<port_name>.sync.syncref.mode** - specifies what member of the Sync mapping to use: Timestamp/SyncId
- **in_port.<port_name>.sync.syncref.URL** [optional] - if the user wants to use a specific URL for the syncref package which is different 
from the component set syncref URL he can use this key. Setting the same URL as the component sync.syncref_url will not influence in any way 
the synchronization. Here we specify the exact signal that will be compared to the SyncRef selection data.
- **in_port.<port_name>.sync.mode** - setting the synmchronization mode. Values: **SyncRef**/CycleID/Timestamp
- **in_port.<port_name>.sync.sync_id** 
  - **name** - the internal URL for the timestamp which will be used as a comparison value to synchronize the port. 
This URL will be appended to the port URL and the value will be extracted for comparison when synchronizing the port.
  - **URL** - the internal URL for the value that will be extracted from the sync ref package which will be used 
- **in_port.<port_name>.sync.timestamp**
  - **name** - the internal URL for the timestamp which will be used as a comparison value to synchronize the port. 
This URL will be appended to the port URL and the value will be extracted for comparison when synchronizing the port.
  - **URL** - the internal URL for the value that will be extracted from the sync ref package which will be used 
- **in_port.<port_name>.sync.algo** - not used by the SyncRef synchronization mode
- **sync.syncref_url** - the syncref URL which is specified at component level. Can be overwritten at port level. Here we specify 
the exact signal that will be compared to the syncRef selection data.

Here is an example of component:
```
"component_name": {
    "composition": {
        "synced_port": {
            "url": "ARS620.RSP_Cycle.RSP_ClusterListMeas"
        }
    },
    "in_port": {
        "synced_port": {
            "sync": {
                "syncref": {
                    "mode": "Timestamp",
                    "url": "ARS620.AlignmentCycle.ALN_SyncRefALNMeas.sigHeader.uiTimeStamp"
                },
                "mode": "SyncRef",
                "sync_id": {
                    "name": "sigHeader.uiMeasurementCounter",
                    "url": "sig_clusterList.uiMeasurementCounter"
                },
                "timestamp": {
                    "name": "sigHeader.uiTimeStamp",
                    "url": "sig_clusterList.uiTimeStamp"
                },
                "algo": "GET_LAST_ELEMENT"
            },
            "port": {
                "version": "1",
                "size": "100000"
            },
            "data": {
                "type": "some_type"
            }
        }
    },
    "sync": {
        "syncref_url": "ARS620.AlignmentCycle.ALN_SyncRefALNMeas.sigHeader.uiTimeStamp"
    },
    "system_services": {
        "timestamp": {
            "mode": "signal",
            "signal_url": "ADC4xx.EMF.EmfGlobalTimestamp.SigHeader.uiTimeStamp",
            "signal_unit": "uns"
        }
    }
}
```

### Step3: Setting up the code inside the component (optional can be done in the configuration)

In the code snippet bellow we set the following:
```
1. SyncMapping sync = {};
2a. sync.signal_url_timestamp.synchronized_port_member_name = "sigHeader.uiTimeStamp";
3a. sync.signal_url_timestamp.syncref_signal_url = "sig_clusterList.uiTimeStamp";
2b. sync.signal_url_syncid.synchronized_port_member_name = "sigHeader.uiMeasurementCounter";
3b. sync.signal_url_syncid.syncref_signal_url = "sig_clusterList.uiMeasurementCounter";
4. ComponentInputPortConfig synced_port = {"synced_port", port_version_number, 100000, "some_type", sync};
5. next::appsupport::ConfigClient::getConfig()->put(
       next::appsupport::configkey::getCompositionPortUrlKey(component_name_, "synced_port"),
       "ARS620.RSP_Cycle.RSP_ClusterListMeas");
6. next::appsupport::ConfigClient::getConfig()->put(
        next::appsupport::configkey::getSyncModeKey(component_name_, "synced_port"), "SyncRef");
7. next::appsupport::ConfigClient::getConfig()->put(
         next::appsupport::configkey::getSyncRefSelectionMode(component_name_, "synced_port"), "Timestamp");
8. std::vector<ComponentInputPortConfig> input_v;
9. input_v.push_back(synced_port);
10. SetupInput("", input_v);
```
- line 1: declare a SyncMapping object
- The user has to use the following lines based type of selection he wants. The user of course can define both.
- line 2a: set the internal URL for the timestamp which will be used as a comparison value to synchronize the port. 
This URL will be appended to the port URL and the value will be extracted for comparison when synchronizing the port.
- line 3a: set the internal URL for the value that will be extracted from the sync ref package which will be used 
- line 2b: set the internal URL for the syncid which will be used as a comparison value to synchronize the port. 
This URL will be appended to the port URL and the value will be extracted for comparison when synchronizing the port.
- line 3b: set the internal URL for the value that will be extracted from the sync ref package (syncid) which will be used 
to compare against the actual data package value for synchronizing
- line 4: fill the SyncMapping object
- line 5: set the subscription URL of the port
- line 6: set the synchronization method for the port to SyncRef
- line 7: specify that we use the signal_url_timestamp member of the SyncMapping object (URLs) for synchronizing. We have 
the option to use also "SyncId" which will use the signal_url_syncid member of the SyncMapping object.
- line 8: create the ComponentInputPortConfig vector used for setting up the triggers inside Next
- line 9: push the newly created port inside the vector
- line 10: call the SetupInput method from inside the Next interface

# Software architecture technical overview {#architecture_overview}

Here is a **detailed technical overview** about how it is the **package extracted, stored and removed from Udex Queue** based on the **received triggers**: 
<img src="archtiecture_from_orchestrator_to_udex.png">

