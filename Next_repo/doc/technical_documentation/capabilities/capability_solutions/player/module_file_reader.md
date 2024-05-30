File Reader{#player_module_file_reader}
==============

Module Overview Contents
* @ref player_module_state_machine
* @ref player_module_play_manager
* @ref player_module_player_plugin
* @ref player_module_publisher_manager
* @ref player_module_resource_manager

___

[TOC]

# Overview

This page explains the Next-Player FileReader module.

Main functionalities:
<ul>
  <li>open file</li>
  <li>start read ahead thread</li>
  <li>jump to timestamp</li>
</ul>

___

# Open File
<p>The Command Handler Client will notify the Command Handler Server that the command OPEN was published. The PlayManager will then handle the requested message, change the current state to Opening and process the event.</p>
<p>The recording file will be opened with replay proxy. Then the data sources and data source provider will be registered with UdexService. If the registration is successful, it will change to state Ready. If the registration is unsuccessful, it will change state back to New.</p>

@startuml
left to right direction
state StateOpening #LightBlue {
    ReplayProxy : mts::runtime::offline::open_recording(filePath, {}, 500ms)
    ReplayProxy : mts::runtime::offline::get_data_source_provider()
    ReplayProxy : mts::runtime::offline::get_data_sources()
    UdexService : RegisterDataSources(data_sources, data_source_provider)
}
state StateNew
state StateReady
StateNew --> StateOpening  : Open Recording Event
StateOpening --> ReplayProxy #LightBlue : Open File
ReplayProxy --> UdexService #LightBlue : Get Data Sources
StateOpening --> StateNew #LightBlue : Recoring Loading Failed
StateOpening -u-> StateReady #LightBlue : Recording Loading Success 
note left of StateOpening
This state represents the Next-Player opening the recording
end note

note left of ReplayProxy
Proxy for offline recording replay (reader and formatter access)
end note

note left of UdexService
Wrapper for access to the Udex library for data publishing
end note
@enduml

<br>

> 📝Replay Proxy is not an actual proxy for a specific C API but a layer that interconnects several component proxies and provides a unified method for reading recordings with raw or formatted data.

## Reader Filter Expressions

Filter expressions serve as a whitelist, functioning as a filtering mechanism to exclusively permit specific, predetermined topics. These expressions can be established using URLs or expression filters. Binary expressions support identifiers, numerical/string constants, equality operators, as well as logical AND/OR operators. The primary objective of filter expressions is to enable efficient data filtering from recording readers. In future versions, all readers will support filter expressions, extending their usage beyond just zrec. Hence, filter expressions can be employed on any recording, as long as the reader supports this functionality.

@startuml
left to right direction
state StateOpening #LightBlue {
    ReplayProxy : mts::runtime::offline::set_filter_expression(filter_expressions)
    UdexService : GetFilterExpressionFromURL(url)
    PublisherManager : filter_expressions
}
state ConfigFile #LightBlue{

}

ConfigFile --> PublisherManager #LightBlue : SettingUpFilters(expressions)
ConfigFile --> UdexService #LightBlue : SettingUpFilters(urls)
UdexService --> PublisherManager #LightBlue : UpdateFilterExpression(urls)
PublisherManager --> ReplayProxy #LightBlue : GetFilterExpressions()
@enduml

Filters entry example in the config file:
\code{.json}
"filters": {
  "url": [
    "url1",
    "url2"
  ],
"expression": [
    expression1,
    expression2
    ]
}
\endcode

More information can be found here:
* [Read optimized data format](https://confluence.auto.continental.cloud/display/SIMEN/Read+optimized+data+format)
* [Reader Filter Expressions](https://confluence.auto.continental.cloud/display/SIMEN/Reader+Filter+Expressions)

___

# Start Read Ahead Thread
@startuml
left to right direction
state StateOpening #LightBlue {
    state ReadAheadThread{
         ReplayProxy : mts::runtime::offline::replay_proxy::read()
         ReplayProxy : mts::runtime::offline::replay_proxy::get_packages()
         state ResourceManager #LightBlue {
            BufferQueue : pushBuffer(package)
         } 
    }
    ReplayProxy #LightBlue --> BufferQueue #LightBlue : Insert package into buffer
}

[*] --> StateOpening  : Open Recording Event
StateOpening --> ReadAheadThread #LightBlue : Start Read Ahead Thread

note right of ReadAheadThread
The read ahead thread is a thread that will read packages and insert them into a buffer
end note

@enduml
___

# Jump to Timestamp
<p>Jump to timestamp it's a file reader functionality used to jump through the recording using replay proxy module.</p>
<p>The read ahead thread will stop, clear the buffer and jump to the specific timestamp without streaming the frames in between. It is also responsible for handling jump to start and jump to end events.</p>
@startuml
left to right direction
state StateSkipTo #LightBlue {
    ReplayProxy : mts::runtime::offline::replay_proxy::jump_to_timestamp(timestamp)
}

[*] --> StateSkipTo  : Jump to Timestamp Event
StateSkipTo --> ReplayProxy #LightBlue : Jump to Timestamp

note left of StateSkipTo
This state represents the Next-Player skipping to a timestamp
end note

@enduml
___

# Package Structure
Structure fields of the packages that Next supports:
<br>

<table>
    <tr>
      <th>Type</th>
      <th>Package Structure</th>
      <th>ECU SW</th>
      <th>ECU HW</th>
      <th>CAN</th>
      <th>Ethernet</th>
      <th>RefCam</th>
      <th>GPS</th>
      <th>FlexRay</th>
    </tr>
    <tr>
      <td>uint16_t</td>
      <td>source_id</td>
      <td>SourceID</td>
      <td>SourceID</td>
      <td>SourceID</td>
      <td>SourceID</td>
      <td>SourceID</td>
      <td>SourceID</td>
      <td>SourceID</td>
    </tr>
    <tr>
      <td>uint16_t</td>
      <td>source_location</td>
      <td>SourceLocationID</td>
      <td>SourceLocationID</td>
      <td>SourceLocationID</td>
      <td>SourceLocationID</td>
      <td>SourceLocationID</td>
      <td>SourceLocationID</td>
      <td>SourceLocationID</td>
    </tr>
    <tr>
      <td>uint32_t</td>
      <td>instance_number</td>
      <td>InstanceNumber</td>
      <td>InstanceNumber</td>
      <td>InstanceNumber</td>
      <td>InstanceNumber</td>
      <td>InstanceNumber</td>
      <td>InstanceNumber</td>
      <td>InstanceNumber</td>
    </tr>
    <tr>
        <td>char[16]</td>
        <td>format_type</td>
        <td>"mts.mta.sw"</td>
        <td>"mts.mta.hw"</td>
        <td>"mts.can"</td>
        <td>"mts.eth"</td>
        <td>"mts.refcam"</td>
        <td>"mts.gpsnmea"</td>
        <td>"mts.flexray"</td>
    </tr>
    <tr>
        <td>uint64_t</td>
        <td>hardware_timestamp.id</td>
        <td>HWTimestampID</td>
        <td>HWTimestampID</td>
        <td>HWTimestampID</td>
        <td>HWTimestampID</td>
        <td>HWTimestampID</td>
        <td>HWTimestampID</td>
        <td>HWTimestampID</td>
    </tr>
    <tr>
        <td>uint64_t</td>
        <td>hardware_timestamp.time</td>
        <td>Computed HW Timestamp</td>
        <td>Computed HW Timestamp</td>
        <td>Computed HW Timestamp</td>
        <td>Computed HW Timestamp</td>
        <td>Computed HW Timestamp</td>
        <td>Computed HW Timestamp</td>
        <td>Computed HW Timestamp</td>
    </tr>
    <tr>
        <td>uint64_t</td>
        <td>reference_timestamp</td>
        <td>Computed Timestamp</td>
        <td>Computed Timestamp</td>
        <td>Computed Timestamp</td>
        <td>Computed Timestamp</td>
        <td>Computed Timestamp</td>
        <td>Computed Timestamp</td>
        <td>Computed Timestamp</td>
    </tr>
    <tr>
        <td>uint32_t</td>
        <td>cycle_id</td>
        <td>from SW header</td>
        <td>from SWC</td>
        <td>CAN Message ID</td>
        <td>0</td>
        <td>0</td>
        <td>0</td>
        <td>communication cycle</td>
    </tr>
    <tr>
        <td>uint16_t</td>
        <td>cycle_counter</td>
        <td>from SW header</td>
        <td>MTAHEADER.FrameCounter</td>
        <td>0</td>
        <td>0</td>
        <td>0</td>
        <td>0</td>
        <td>0-63(communication cycle)</td>
    </tr>
        <tr>
        <td>uint8_t</td>
        <td>cycle_state</td>
        <td>BODY</td>
        <td>BODY</td>
        <td>START|BODY|END</td>
        <td>0</td>
        <td>0</td>
        <td>0</td>
        <td>BODY(computed by reader)</td>
    </tr>
    </tr>
        <tr>
        <td>uint64_t</td>
        <td>size</td>
        <td>computed</td>
        <td>computed</td>
        <td>computed</td>
        <td>computed</td>
        <td>computed</td>
        <td>computed</td>
        <td>computed</td>
    </tr>
    <tr>
        <td>*</td>
        <td>payload</td>
        <td>ECU SW Package</td>
        <td>ECU HW Package</td>
        <td>CAN Frame + Payload</td>
        <td>Ethernet Data + Payload</td>
        <td>Video Frame Header + Payload</td>
        <td>NMEA Sentence</td>
        <td>FlexRay Data</td>
    </tr>
</table>
