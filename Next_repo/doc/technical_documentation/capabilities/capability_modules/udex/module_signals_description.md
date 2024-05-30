Signals Description{#udex_module_signals_description}
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

When data is recorded each algo component can do a snapshot of it's input/output structures, when triggered, and stores it in the recording, for historycal reasons this is calld measfreeze, freezing a measurement data value/sample. 

This data is handed over to the recording entity that is in charge of storing it in a rec file, MTSi, it gives the binary blob a timestamp, adds some header information so it can be identified later, and writes it to the file exactly, for components that provide the output to the bus, the data is written in the recording exactly as the ECU would write to the bus in the vehicle, same format.

Some data buses have some protocol limitations, for example CAN, where the maximum CAN payload is normally 8 bytes, and 64 bytes for CAN FD, some extended version of CAN. Within these 64 bytes the protocol can store more values than actually fit in 64 bytes on a x86 machine, by doing some compression based on some formulas and coefficients specified by the CAN standard. This information is stored also in a DBC file that can later be used to do the reverse, to decompress the data from a 64 byte CAN frame, to an actual C properly aligned structure that sizeof(said_structure) can be 160 bytes. Other example would be different endianness when storing the structure in the recording, that needs to be converted for that structure to make sense when the bytes are copied on top of a C structure.

This process of decompression/conversion from a data bus structure to a C structure is called **data extraction**. After data extraction, the structure can be copied around and sent over different components, and all of them have the same understanding and can memcpy and it makes sense when debugging on a x86 machine.

For easier understanding we will use the SDL format as a data description file, as it's more user friendly and easy to understand. This format is usually used for data that is not sent on the bus, but it's data that internal algo components pass between themselves to produce the final output that is eventually sent on the bus. We will call this ECU data.

SDL sample to discuss on
```
<SdlFile xmlns:xsd="http://www.w3.org/2001/XMLSchema" ByteAlignment="1" Version="2.0">
	<View Name="AlgoVehCycle" CycleID="207">
		<Group Name="VehDyn" Address="20350000" ArrayLen="1" Size="160">
			<Signal Name="uiVersionNumber" Offset="0" ArrayLen="1" Type="ulong" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
			<SubGroup Name="sSigHeader" Offset="4" ArrayLen="1" Size="12">
				<Signal Name="uiTimeStamp" Offset="0" ArrayLen="1" Type="ulong" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
				<Signal Name="uiMeasurementCounter" Offset="4" ArrayLen="1" Type="ushort" Bitmask="ffff" ByteOrder="big-endian" Size="2"/>
				<Signal Name="uiCycleCounter" Offset="6" ArrayLen="1" Type="ushort" Bitmask="ffff" ByteOrder="big-endian" Size="2"/>
				<Signal Name="eSigStatus" Offset="8" ArrayLen="1" Type="uchar" Bitmask="ff" ByteOrder="big-endian" Size="1"/>
				<Signal Name="a_reserve" Offset="9" ArrayLen="3" Type="uchar" Bitmask="ff" ByteOrder="big-endian" Size="1"/>
			</SubGroup>
		</Group>
	</View>
</SdlFile>
``` 
the C structure that is described in the upper SDL<br>
```
  typedef uint8_t a_reserve_array_t[3];
  
  typedef struct
  {
    uint32_t uiTimeStamp;
    uint16_t uiMeasurementCounter;
    uint16_t uiCycleCounter;
    uint8_t eSigStatus;
    a_reserve_array_t a_reserve;
  } SignalHeader_t;
  
  typedef struct
  {
    uint32_t uiVersionNumber;
    SignalHeader_t sSigHeader;
  } VehDyn;
```
memory layout of the structure
<img src="memory_layout.png">

One **ECU** contains multiple algo components that can run in different timing, for example every 20ms or 60ms, these components are running in different **cycles**. Each component contains multiple **ports**, that are used to get and sends data, according to AUTOSAR standard.

When we translate this in data description language it goes like this:

One **Device** contains multiple **views**. Each view contains multiple **groups**. The group is the granularity level that we send data around, and it makes sense in the code as a C structure.

Based on this we can identify each group by forming a unique URL: **Device.View.Group**<br>
Device.View.Group - this is a group URL<br>
Device.View.Group.Subgroup0.Subgroup1.Atomic_signal - this is a signal URL, if we want to access a single atomic signal (member) out of the data structure

The <**View**> contains a **CycleID**, this is used to group multiple components in the same cycle, those will be triggered at the same time, or in a defined order, but in that specified cycle.<br>
The <**Group**> is basically data that is send over to ports, and is contains a unique Virtual **Address** that is defined by the project architecture, virtual address + size of the group can not overlap with other virtual addresses as these are used in the ECU as memory layout.<br>
When the upper mentioned VehDyn structure is measfreezed, it will be wrapped in a dataPacket, and this data packet will contain beside the payload and the size, also the Cycle ID and the Virtual Address.

Having these pieces of information we can already identify the structure that is contained in that data packet and to map it to a data description that offers us the layout of the data members inside the structure, and based on the offsets we can access any of the internal members like uiTimeStamp or uiMeasurementCounter<br>
**Device.View.Group** can be mapped using **CycleID** and **Address**

In the vehicle there can be multiple ECUs of the same type, for example short range radars on each corner of the vehicle, a long range radar in the front and back, we will refer to them as device, from data read/write perspective. Each of those devices is a data source, so each of them has a unique Source ID, and if there are multiple devices of the same Source ID, they will have a different Instance Number. So the 4 short range radars will have the same Source ID and Instance Number acting as a counter.

Now going back to data measfreeze, beside the cycleID and Address to identify the structure in the payload, the data packet also contains the Source ID and the Instance Number, so we don't mix up structure coming from different devices.

This is how data is stored in the recording and how different devices, views and groups are handled. <br>
Some protocols might have some differences, for example Ethernet signals do not have a Virtual Address but they can be uniquely identified by ServiceID + MethodID that are part of the SomeIP protocol.

Note: When we are going to simulate the behavior of one ECU, all the views are grouped under one virtual device, called SIM VFB (virtual function bus) for historical reasons. 

Vehicle recording setup
@startuml
node "ECU1" {
[component11]
[component12]
}

node "ECU2"{
[component21]
[component22]
}

[component11] -> [component12] 
[component11] .> [MTSi] : measfreeze
[component12] ..> [MTSi] : measfreeze
[component12] -u-> [Vehicle bus] : output

[component21] -> [component22] 
[component21] .> [MTSi] : measfreeze
[component22] ..> [MTSi] : measfreeze
[component22] -u-> [Vehicle bus] : output
@enduml


From data perspective
@startuml
node "Device1: {Source ID, Instance Number}" {
[View11:{CycleID}]
[View12:{CycleID}]
}

node "Device2: {Source ID, Instance Number}"{
[View21:{CycleID}]
[View22:{CycleID}]
}

[View11:{CycleID}] -> [View12:{CycleID}] : VehDyn
[View11:{CycleID}] -> [Recording] : VehDyn:{vaddr}
[View12:{CycleID}] -d-> [Recording] : ObjList:{vaddr}

[View21:{CycleID}] -> [View22:{CycleID}] : VehDyn
[View21:{CycleID}] -> [Recording] : VehDyn:{vaddr}
[View22:{CycleID}] -d-> [Recording] : ObjList:{vaddr}
@enduml
