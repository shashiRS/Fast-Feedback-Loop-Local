DataExtractor {#udex_module_data_extractor}
==============

Module Overview Contents
* @ref udex_module_data_extractor
* @ref udex_module_data_publisher
* @ref udex_module_data_subscriber
* @ref udex_module_rawdata
* @ref udex_module_signal_explorer
* @ref udex_module_signals_description
___

[TOC]

The extractor manages the incoming data flow and processes and prepares all incoming data to be used either
* buffer based (```GetExtractedQueueWithTimestamp```)
* block based (```GetExtractedData```, ```GetRawData```, ```GetSyncExtractedData```)
* signal based (```GetValue```, ```GetVectorValue```, ```GetInfo```, ```GetValueAndInfo```)

Additionally, there are several functions for setting up utilities
* add and removing hooks (```connectCallback```, ```disconnectCallback```, ```getCallbacksInfo```)
* setting extraction specifications (```SetExtractionVersion```)
* setting synchronization methods (```SetSyncAlgo```)
* setting data flow management (```SetDropExtraInput```, ```SetBlockingExtraction```, ```SetBlockingExtractionForUrl```)


It needs to be connected to the ```DataSubscriber``` to receive data

@startuml
partition Subscriber #Orange{
"eCAL" --> "Subscriber hook"
}

partition Extractor #Orange {

partition udex_queue #grey {
"Subscriber hook" ->[eCAL binary data package] "Push data to queue"
"Push data to queue" --> "Background Thread"
"Background Thread" -[#red]->[Data extraction] "Background Thread"
--> "Set data to extraction buffer"
--> === S1 ===
"Set data to extraction buffer" --> "call hooks"
--> === S2 ===

}

partition Public #orange {
=== S1 === --> GetValue
=== S1 === --> isDataAvailable
=== S1 === --> GetExtractedQueueWithTimestamp
=== S1 === --> GetExtractedData
=== S1 === --> GetRawData
=== S2 === --> connectCallback
}
}
@enduml

___

# Buffer based input

If an application or user is interested in buffered data and has a event-driven application instead of a data-driven one \named{NEXT-UDex} provides multiple functions to interact with buffered incoming data.
Per extractor and per subscription to a \named{Signal-URL}\glos{url} a isolated and separated buffer is created to allow for multiple use-cases even if the one application uses the same \named{Signal-URL}\glos{url}.

There are functions to
* request a copy of the buffer to look for data (```GetExtractedQueueWithTimestamp```)
* clear the current entries in the queue (```ClearExtractedQueue```)
* clear the top elements of the queue (```RemoveElementsFromQueue```)

# Block based input

Additionally most of the applications want to separate concerns of handling the buffered queue and selecting the correct \named{Binary blob}\glos{binary_blob}. 
This leads to speed-ups when using a simple cast to a c-struct instead of manually assigning all values separately.
To allow such a data-treatment the memory block has to be extracted and transformed to fit the architecture of the CPU (byte-alignment, endianness) and match the actual struct with its specified interface version. 
Both of those challenges can be handled by the ```DataExtractor``` using low-level extraction methods for example for CAN-data or Ethernet-packages as well as introducing \named{GIA}\glos{gia}.
Block based input can be either used as a raw, un-processed packages, so called \named{Raw data blobs}\glos{raw_data_blob} using 
* ```GetRawData```

Or via processed packages, so called \named{Binary blobs}\glos{binary_blob}
* ```GetExtractedData```

Additionally, a customizable synchronization algorithm can be integrated to use not the latest received packages as done with (```GetExtractedData```). 

## Extraction specifications

For the accurate extraction with the Block based input additionally parameters can be set to ensure a proper extraction.
This is especially important if GIA-based extraction is required.

* ```SetExtractionVersion``` enables version-specified extraction on a block based level.

# Signal based input

In contrast to the Block based input often the exact structure of data is not known at compile-time. Therefor a dynamic typing has to be used. This is available with the Signal based input.
All signals in the \named{NEXT Simulation Network}\glos{simulation_network} are identifiable via a unique \named{Signal-URL}\glos{signal_url}, 
which consists of the component name prefixed with "SIM_VFB." or the recorded device name (i.E. "ADC4xx"). 
The registered structure data (```RegisterDataDescription```) or port name are then used to extend the \named{Signal-URL}\glos{signal_url}.
As recording device names and components are required to have a unique name all \named{Signal-URL}\glos{signal_url} are uniquely identifiable.

Examples for \named{Signal-URL}\glos{signal_url} are
* "ADC4xx.EMF.EmfGlobalTimestamp.SigHeader.uiTimeStamp" (standard c-struct \named{Signal-URL}\glos{signal_url})
* "Local_SRR_FR_CANFD.SRR520HA22_FR_Private.SRR_Debug.E2E_CRC16_Debug" (CAN-data \named{Signal-URL}\glos{signal_url})
* "AXIS_CAM_Left.Video.Frame.FrameHeader.BitsPerPixel" (Reference Camera \named{Signal-URL}\glos{signal_url})
* "GPS_Mi.GPSCycle.NMEA_GPGGA.LatitudeHemi" (GPS-data \named{Signal-URL}\glos{signal_url})

To receive signals based on a \named{Signal-URL}\glos{signal_url} you can call 

* ```GetValue``` to receive a single value which is automatically cast and reformated to your requested type using ```SignalType```
* ```GetVectorValue``` to receive a array value which is automatically cast and reformated to your requested type using ```SignalType```
* ```GetValueAndInfo``` to receive a value together with its respective ```SignalInfo```
* ```GetInfo``` to receive just the ```SignalInfo```

All those examples can be seen and evaluated with help of the ```SignalExplorer```. This is also used when looking through the \named{Signal Tree}\glos{signal_tree} with the NEXT-GUI
<img src="signal_tree.png" width="800">

___

# Hooks

As the eCAL input is decoupled from the extractor to ensure a loss-less data-flow in the \named{NEXT Simulation Network}\glos{simulation_network}.
To check if new data is available you can use 

* ```isDataAvailable``` providing a check per port
* ```connectCallback``` to connect a callback notifying when new data is available. The Signal based input is kept frozen until the callback is returned.
* ```disconnectCallback``` to disconnect a callback
* ```getCallbacksInfo``` returns all currently connected callbacks


# Data flow management

Especially for value-based and data-driven subscriptions more control over the data-flow is needed. There are two main concerns which require balance.
* performance
* synchronization

To allow for a synchronization even with single value extractions a blocking mechanism is introduced which blocks the further processing of the respective ```DataPublisher```
and therefore synchronizes the \named{NEXT Simulation Framework}\glos{simulation_framework} from back to front. 
To enable this blocking use 

* ```SetBlockingExtraction``` to ensure the respective ```DataExtractor``` couples the connected callback attached with ```connectCallback``` directly with the eCAL callback. Therefore slowing down the whole \named{NEXT Simulation Framework}\glos{simulation_framework}
* ```SetBlockingExtractionForUrl``` allows for \named{Signal-URL}\glos{signal_url} specific blocking

If synchronization is not a concern and only performance of the backend is required one is also able to drop packages inside the ```DataExtractor```-queue to ensure just working on the latest received data.
This still allows for slow processing in the attached callbacks without stacking up the Queue inside the ```DataExtractor``` and limits the processed data to the latest available package. All packages in between are lost.

* ```SetDropExtraInput``` sets this package-drop property on the ```DataExtractor```-queue for all callbacks of the ```DataExtractor```.

@startuml
partition "Decoupled: SetBlockingExtraction(off)" #orange {
"eCAL Callback (DataSubscriber)" --> "PushData (DataExtractor)"
"PushData (DataExtractor)" -->[Add to buffer (decoupling)] ===S1===
===S1=== -> "Data extraction"
"Data extraction" -> ===S1===

"Data extraction" --> "User Callback"

===S1=== -->[Get value from buffer] "GetValue"
"User Callback" --> "GetValue"
}
@enduml

@startuml
partition "Blocked: SetBlockingExtraction(on)" #ff8080 {
"eCAL Callback (DataSubcsriber)" --> "PushData (DataExtractor)"
"PushData (DataExtractor)" -->[Add to buffer] ===S1===
"PushData (DataExtractor)" -> "Data extraction"

"Data extraction" --> "User Callback"
===S1=== -->[Get value from buffer] "GetValue"
"User Callback" --> "GetValue"
}
@enduml


# Synchronization

To enable this synchronization use
* ```GetSyncExtractedData``` to receive the sorted and filtered \named{Binary blobs}\glos{binary_blob}
* ```SetSyncAlgo``` provides functions to set a predefined sorting algorithm or a custom one
* ```setDefaultSync``` sets a default syncing algorithm which takes the latest received element.

To see the general flow of synchronization you can follow the diagram below

@startuml
partition "Synchronization" #orange {
"eCAL Callback (DataSubscriber)" --> "PushData (DataExtractor)"
"PushData (DataExtractor)" -->[Add to raw buffer (decoupling)] ===raw===

===raw=== -> "Data extraction"
"Data extraction" ->[background thread] "Data extraction"

"Data extraction" --> ===S1===

(*) -->[Orchestrator Trigger] "Trigger Callback"
"Trigger Callback" --> "GetSyncExtractedData"

"GetSyncExtractedData" -> "Synchronization Algorithm"
===S1=== ->[Select from buffer] "Synchronization Algorithm"
"Synchronization Algorithm" ->[Clear elements] ===S1===
"Synchronization Algorithm" -> "GetSyncExtractedData"
"GetSyncExtractedData" --> "Binary blob"
}
@enduml

In general the Synchronization Algorithms work on two input arguments which need to be provided

* A Comparison \named{Signal-URL}\glos{signal_url} to see which part of the \named{Binary blob}\glos{binary_blob} needs to be compared against the reference value
* Reference Value itself which is defined by its Type (```SignalType```) and its value (```dataType```)

Internally the buffer of the extracted data is provided and each element in the buffer can be evaluated against
the reference value with help of the ```dataType``` function delivering byte-offset and type of the requested Comparison-\named{Signal-URL}\glos{signal_url}

@startuml

(*) -> "GetSyncExtractedData"

"UDex Extraction Queue"#orange --> ===Buffer===

partition SyncManager #orange {
"GetSyncExtractedData" ->[Reference Value] "Synchronization Manager"
"GetSyncExtractedData" ->[Comparison SignalURL] "Synchronization Manager"
"Configuration Manager"#lightyellow -->[SyncMode] "Synchronization Manager"
"Synchronization Manager" ->[SyncMode] "set up Sync Algo"
"Synchronization Manager" -->[Reference Value and Comparison SignalURL]  ===SyncAlgo===
"set up Sync Algo" -->[SyncMode]  ===SyncAlgo===
}
partition iSyncAlgo #grey {
===SyncAlgo=== -->[Comparison SignalURL] "Filter Buffer for synchronized Data"


"Filter Buffer for synchronized Data" -->[Reference Value] "Analyse single buffer element"
"Filter Buffer for synchronized Data" -->[Comparison SignalURL] "GetInfo"
"Analyse single buffer element" ->[repeat until match found] "Analyse single buffer element"
"GetInfo" -->[Byte offset and Signal Type] "Analyse single buffer element"
===Buffer=== -->[get Buffer Element] "Analyse single buffer element"

"Analyse single buffer element" -->[current best guess] ===BinaryBlob===
===BinaryBlob=== -->[returned binary blob] "GetSyncExtractedData"

}

@enduml



___


A more complicated Use case can be seen for example in the \named{NEXT Component Interface}\glos{component_interface}
In this example the trigger is done via the \named{Orchestrator} of \named{NEXT-Control}\glos{control} and uses the trigger data to extract some synchronization information.
Then the User code is called and when a request for new data is received the \named{NEXT Component Interface}\glos{component_interface} sorts and synchronizes automatically in the backend.

@startuml
partition "DataExtractor" #orange {
"eCAL Callback (DataSubscriber)" --> "PushData (DataExtractor)"
"PushData (DataExtractor)" -->[Add to raw buffer (decoupling)] ===raw===

===raw=== -> "Data extraction"
"Data extraction" ->[background thread] "Data extraction"

"Data extraction" --> ===S1===

"GetSyncExtractedData" --> "Synchronization Algorithm"
===S1=== ->[Select from buffer] "Synchronization Algorithm"
"Synchronization Algorithm" ->[Clear elements] ===S1===
"Synchronization Algorithm" -> "GetSyncExtractedData"

}

partition "NEXT Client Interface" #orange {

"Trigger Callback" -.>[provide sync info] "GetSyncExtractedData"

partition "User Execute" #grey {
"Trigger Callback" -> "Component Execute hook"


"GetAsyncPushData(port 2)" --> "GetSyncExtractedData"
"GetAsyncPushData(port 1)" --> "GetSyncExtractedData"


"Component Execute hook" --> "GetAsyncPushData(port 2)"
"GetAsyncPushData(port 1)" -> "Component Execute hook"
"GetAsyncPushData(port 2)" -> "Component Execute hook"
"Component Execute hook" --> "GetAsyncPushData(port 1)"
"Component Execute hook" -> "Trigger Callback"
}
(*) -->[Orchestrator Trigger] "Trigger Callback"
}
@enduml

# Extraction Threading 

Currently the extraction is coupled via a mutex and condition variables in a separate thread. 
When data is received or pulled the extraction thread is being triggered via a condition variable and will run until all data has been extracted.

@startuml

participant Node_Extraction_Thread

Node_Extraction_Thread->Node_Extraction_Thread: getting triggered by buffering_queue_cv_

Node_Extraction_Thread->Node_Extraction_Thread: check raw_input_queue size
alt extract data
	Node_Extraction_Thread->Node_Extraction_Thread: push data to temp_queue_ptr
	Node_Extraction_Thread->Node_Extraction_Thread: reset raw_input_queue_ptr_
	loop while !temp_queue_ptr->empty()
		Node_Extraction_Thread->Node_Extraction_Thread: extract package
		Node_Extraction_Thread->Node_Extraction_Thread: push package to pull queue
	end
	Node_Extraction_Thread->Node_Extraction_Thread: notify main thread with pull queue condition variable
	Node_Extraction_Thread->Node_Extraction_Thread: sleep
else no data available
	Node_Extraction_Thread->Node_Extraction_Thread: notify main thread with pull queue condition variable
	Node_Extraction_Thread->Node_Extraction_Thread: sleep
end
	
@enduml

The graph below shows an example flow when data is received on a subscriber to visualize the interaction between the threads.
The simulation node is left out to simplify the graph. Usually there is one participant in the network using the udex library and requests data from it.

@startuml

participant Player
participant Node_Extraction_Thread
participant Node_Main_Thread

Player-> Node_Extraction_Thread: publish data to input ports
Node_Extraction_Thread->Node_Main_Thread: push call
Node_Main_Thread->Node_Main_Thread: copy data to buffer queue
Node_Main_Thread->Node_Extraction_Thread: notify with buffering queue cv
Node_Main_Thread->Player: return
par extraction
	Node_Extraction_Thread->Node_Extraction_Thread: acquire lock for pull queue
	Node_Extraction_Thread->Node_Extraction_Thread: extract
	Node_Extraction_Thread->Node_Extraction_Thread: sleep
end
Player->Node_Main_Thread: pull queue called via sim node
Node_Main_Thread->Node_Extraction_Thread: triggers via buffering_queue_cv_
Node_Main_Thread->Node_Main_Thread: wait for extracted_output_queue_cv
Node_Extraction_Thread->Node_Extraction_Thread: obtain lock for pull queue
alt extraction done and queue empty	
	Node_Extraction_Thread->Node_Main_Thread: notify main thread with pull queue cv
else extraction not done
	Node_Extraction_Thread->Node_Extraction_Thread: continue running extraction
	Node_Extraction_Thread->Node_Extraction_Thread: check if buffering queue is empty
	alt buffering queue != empty
		Node_Extraction_Thread->Node_Extraction_Thread: extract data
		Node_Extraction_Thread->Node_Main_Thread: notify main thread with pull queue cv
	else
		Node_Extraction_Thread->Node_Main_Thread: notify main thread with pull queue cv
	end
else new data and no running extraction
	Node_Extraction_Thread->Node_Extraction_Thread: extraction started by buffering_queue_cv_
	Node_Extraction_Thread->Node_Main_Thread: notify main thread with pull queue cv
end
Node_Main_Thread->Player: deliver data and finish exec

@enduml
