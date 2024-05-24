SyncRef Synchronization {#port_synchronization_syncref}
==============

The SyncRef synchronization uses SyncRef information from the recording for port level synchronization. The SyncRef itself contains a signal header
which holds the general information to detect the SyncRef again and sort it to a cycle/flow of execution. The payload of the SyncRef holds the information
for each port which data package was used. To detect these information it holds the complete sigHeader and uses two kinds of information which can be
both used for uniquely identifying the package:

* Timestamp: during the measfreezing the current timestamp of the signal header from the incoming package is set into the SyncRef
* Measurement Counter: the scheduler on the ECU increments the measurement counter and each component measfreezes them for the incoming packages

The SyncRef is measfrozen during the start of the execution of a component. The purpose of it is to allow synchronization in the simulation
environment to enable the algo developers to resimulate errors or wrong behavior exactly as it happened on the ECU.

## Basic Synchronization Overview

@startuml
state SyncManager : 1. GetSyncValueFromSyncRef(), 2. FillPortSyncedData()
state SyncRefHandler : getSyncValue()
state SyncAlgoCfg : FIND_EXACT_HEADERTIMESTAMP, FIND_EXACT_VALUE
state DataExtractorSyncRef : extractor only for sync ref related data
state NextComponentInterfaceImpl: OnTrigger()

[*] --> NextComponentInterfaceImpl : incoming trigger(mts_header_timestamp)
NextComponentInterfaceImpl -down-> SyncManager: FillReferenceValueAndData()

SyncManager --> SyncRefHandler: step1: GetSyncValueFromSyncRef()
note left of SyncRefHandler
- sets SyncAlgoCfg
- first extracts syncref timestamp 
- second port level timestamp
end note
SyncRefHandler --> SyncAlgoCfg: sets sync modes (Exact Value)
SyncRefHandler --> DataExtractorSyncRef : sets SyncAlgoCfg and extracts syncref timestamp + port level timestamp

SyncManager --> DataExtractor : step2: FillPortSyncedData uses sync_value from sync_ref

@enduml

* trigger is incoming with headertimestamp (trigger needs to be fired by sync ref)
* calls SyncRefHandler for extracting the timestamp from sigHeader
* extracts the timestamp in the payload of the SyncRef for the current port as sync_value


## Port Level Synchronization

@startuml

state UdexQueue {
state el_1 {
state timestamp1 : 1000
state cycleId1 : 3
}
state el_2 #orange {
state timestamp2 #orange : 1050
state cycleId2 : 4
}

state el_3{
state timestamp3 : 1100
state cycleId3 : 5
}
}

state el_1 : mts_timestamp 1020
state el_2 #orange : mts_timestamp 1075
state el_3 : mts_timestamp 1115

state SyncAlgo : method(exact, ...), SyncValue, SyncURL

DataPublisher -down-> UdexQueue : incoming data packages

[*] --> DataExtractor : FillPortSyncedData(sync_value=1050, type: timestamp)
DataExtractor --> UdexQueue : GetExtractedQueueWithTimestamp(signal_URL)
DataExtractor --> SyncAlgo : CallSyncAlgo()
DataExtractor -down-> [*]	: Return value el_2

@enduml

* the port uses the sync_value from the step before
* the complete input queue is requested in the data extractor
* the given sync_value is used to call the sync algo with exact value (input is the resolved timestamp)

## Workflow for Configuring SyncRef Synchronization

The setup phase prepares the synchronization configuration with the following steps:
	* prepare SyncRef with information from trigger signal
	* prepare the sync ref reference value (e.g. if the synchronized value from the trigger differs)
	* prepare the SyncRef port data with reference to the value inside of the SyncRef used for port level synchroization
@startuml
participant Sync_Manager
group SetSyncConfig
	Sync_Manager -> Sync_Manager: SetSyncConfig()
	note over Sync_Manager
	sets up the configuration for the synchronization
	end note
	Sync_Manager -> Sync_Manager: step 1: set sync mode
	note over Sync_Manager
	sets the configuration mode (e.g. SyncMode_SyncRef)
	end note
		group SetupSyncRef
		Sync_Manager -> Sync_Manager: step 2: SetupSyncRef()
		note over Sync_Manager
		gets the flow ID and syncref URL from the orchestrator and stores it in the syncref_handler_map
		end note
		end
	
		group SetupSyncRefPortInfo
		Sync_Manager -> Sync_Manager: step 3: SetupSyncRefPortInfo()
		note over Sync_Manager
		gets the sync ref reference value for the component level syncref synchronization
		depending on the sync ref mode it gets the sync->timestamp->name or sync->sync_id->name value and concatenates it to either 
		sync->syncref_URL on component level or
		sync->syncref->URL on port level
		end note
		Sync_Manager -> Sync_Manager: step 4: syncref_handler_map_[flow.id]SetupPortSyncData()
		note over Sync_Manager
		- creates sync ref extractor and subscribes to sync ref URL
		- gets package URL from signal_URL (sync_ref) + port_sync_info (from sync_name section)
		- sets sync algo to exact value
		- sets signal URL to ref_value_sync_cfg.signal_URL from orchestrator (TODO recheck with debugging in working variant)
		end note
		end
	note over Sync_Manager
		- sets the sync type to exact value
		- depending on syncref_mode sets signal URL to Timestamp/SyncId
	end note
end
@enduml

Example Configuration with comments (no JSON valid code, the comments are just for additional explanations):

```
{
    "nextsync_0": {
        "logging": {
            "file": {
                "loglevel": "INFO"
            },
            "console": {
                "loglevel": "INFO"
            },
            "router": {
                "loglevel": "INFO"
            }
        },
        "system_services": {
            "timestamp": {
                "mode": "signal",
                "signal_URL": "undefined",
                "signal_unit": "us"
            }
        },
        "dataAvailabilityMode": "DATA_BASED",
        "in_port": {
            "NextSync_0.requestPort_VehDyn_sync_syncref": {
                "port": {
                    "version": "0",
                    "size": "160"
                },
                "data": {
                    "type": "VehDyn"
                },
                "sync": {
                    "sync_id": {
                        "name": "sSigHeader.uiMeasurementCounter", #part added to port level subscription to resolve final value with sync value after sync ref sync
                        "url": "VehDyn.uiMeasurementCounter" #part added to sync ref port level URL
                    },
                    "timestamp": {
                        "name": "sSigHeader.uiTimeStamp", #part added to port level subscription to resolve final value with sync value after sync ref sync
                        "url": "VehDyn.uiTimeStamp" #part added to sync ref port level URL
                    },
                    "mode": "SyncRef", # mode for synchronization
                    "algo": "FIND_EXACT_VALUE", #mode for single value synchroization (will be overwritten for port level data, maybe can be used for sync ref timestamp/sync id synch
                    "syncref": {
                        "mode": "Timestamp", #mode for sync ref (timestamp or sync_id) -> determines which sync will be used and which options
						"url": "ARS5xx.RawDataCycle.ALN_SyncRef.sSigHeader.uiTimeStamp" # URL to timestamp used for syncref synchronization -> I think is overwritten by port level URL
                    }
                }
            }
        },
		"sync": {
			"syncref_url": "ARS5xx.RawDataCycle.ALN_SyncRef.sSigHeader.uiTimeStamp" # URL valid for all ports if set -> sets the URL to the timestamp which should be used for sync ref synchronization
		},
        "composition": {
            "NextSync_0.requestPort_VehDyn_sync_syncref": {
                "url": "ARS5xx.AlgoVehCycle.VehDyn" # sets first part of last step sync ref synchronization (URL to the value used for the port data timestamp after getting the value from the sync ref payload)
            }
        },
		"next_player": {
		"orchestrator": {
		  "flow1": {
			"meta": {
			  "delay": 3,
			  "sync": true
			},
			"type": "signal_name",
			"value": "ARS5xx.RawDataCycle.ALN_SyncRef",
			"meta_info":{
                "ref_value" : "ARS5xx.RawDataCycle.ALN_SyncRef.sSigHeader.uiTimeStamp" # value extracted for sync ref synchronization -> used in setup sync ref call
            },
			"clients": [
			  [
			    "nextsync_0",
			    "NextSync_0_CopySynchedVehDyn"
			  ]
			]
		  }
		},
        "realtime_factor": "10"
    }
}
```

## Workflow for SyncRef Synchronization 
@startuml
participant Trigger
collections SyncRefSelectionData
collections SyncRefPackage
Trigger -> SyncRefSelectionData: 1. trigger timestamp
note over SyncRefSelectionData
based on the trigger timestamp
select the correct
sync ref selection (ref) data
end note
SyncRefSelectionData -> SyncRefSelectionData: 2. select entry based on input (sync value)
SyncRefSelectionData-> SyncRefPackage: 3. sync ref selection ts from URL set as sync ref URL
note over SyncRefPackage
based on the timestamp
select the correct
sync ref selection data
end note
SyncRefPackage -> SyncRefPackage: 4. select entry based sync ref URL
SyncRefPackage -> Port1 : 5. select timestamp from port specific URL for Port1
Port1 -> Port1			: 6. find correct package by port sync URL
SyncRefPackage -> Port2 : 5. select timestamp from port specific URL for Port2
Port2 -> Port2			: 6. find correct package by port sync URL
@enduml

* 1. a trigger comes in from the orchestrator master
* 2. the SyncRef is detected by the header timestamp from the trigger signal (the trigger needs to be done with the SyncRef of this flow)
* 3. internally the signal to the SyncRef URL is resolved and could be used for sync value
* 4. the SyncRef extracts the payload to the URL given for the current port (timestamp/sync ID)
* 5. the sync reference value for the current input port is set (this will be compared to the extracted SyncRef port sync value)
* 6. the SyncAlgo checks the given values (SyncRef sync value and port sync value) for exact matches
* 7. step 5 and 6 are repeated for each port