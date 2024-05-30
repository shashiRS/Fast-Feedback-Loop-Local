How to adapt Base Control SDL for different data generation {#howto_adapt_base_control_sdl_for_different_data_generation}
=====

[TOC]

---

# References {#howto_adapt_base_control_sdl_for_different_data_generation_references}

* @ref howto_configure_base_control_data_main
* @ref howto_configure_base_control_data
* @ref howto_visualize_base_control_data
* @ref howto_verify_trigger_setup_with_ecal

---

# Overview {#howto_adapt_base_control_sdl_for_different_data_generation_overview}

This part of the tutorial will describe how the .sdl file is used in order to generate the Base Control Data.

---

# The .sdl file

The .sdl file is used by the Orchestrator in order to put the generated base control data in the structure defined by the user.

> 📝 **IMPORTANT** The .sdl file needs to be located in the **same directory as the player**.

The content can looks something like this:

<br><img src="sdl_bcd.png"><br>

The first thing to take into consideration is the **View name**.

<br><img src="sdl_bcd2.png"><br>

**?ViewName?** is just a placeholder, the Orchestrator will create new View name by combining the **flow name** and **component name** defined in the .json file (naming format is **[flow name]_[component name]**).
For more information on the .json file check the tutorial below:  
@ref howto_configure_base_control_data

<br><img src="sdl_bcd3.png"><br>

Based on the above image, the resulting View name will be **flow1_sicoregeneric**.
This can be seen in the image taken from Table View.
More on how to visualize the Base Control Data here:  
@ref howto_visualize_base_control_data

<br><img src="sdl_bcd4.png"><br>

The structure of the .sdl file also defines the how the generated Base Control Data will look.

SDL:
```xml
<SdlFile xmlns:xsd="http://www.w3.org/2001/XMLSchema" ByteAlignment="1" Version="2.0">
  <View Name="?ViewName?" CycleID="207">
    <Group Name="SigHeader" Address="20350000" ArrayLen="1" Size="12">
      <Signal Name="uiTimeStamp" Offset="0" ArrayLen="1" Type="ulong" Bitmask="ffffffff" ByteOrder="little-endian"
              Size="4"/>
      <Signal Name="uiMeasurementCounter" Offset="4" ArrayLen="1" Type="ushort" Bitmask="ffff" ByteOrder="little-endian"
              Size="2"/>
      <Signal Name="uiCycleCounter" Offset="6" ArrayLen="1" Type="ushort" Bitmask="ffff" ByteOrder="little-endian"
              Size="2"/>
      <Signal Name="eSigStatus" Offset="8" ArrayLen="1" Type="uchar" Bitmask="ff" ByteOrder="little-endian" Size="1"/>
      <Signal Name="a_reserve" Offset="9" ArrayLen="3" Type="uchar" Bitmask="ff" ByteOrder="little-endian" Size="1"/>
    </Group>
  </View>
</SdlFile>

```
JSON:
```json
{
   "next_player":{
      "orchestrator":{
         "flow1":{
            "meta":{
               "delay":0,
               "sync":true,
               "control_data_generation":{
                  "sdl_file":"sig_header.sdl",
                  "member_data":{
                     "SigHeader.uiTimeStamp":"ADC5xx_Device.VD_DATA.IuVehDynamicsPort.sSigHeader.uiTimeStamp",
                     "SigHeader.uiMeasurementCounter":"ADC5xx_Device.VD_DATA.IuVehDynamicsPort.sSigHeader.uiMeasurementCounter",
                     "SigHeader.uiCycleCounter":"ADC5xx_Device.VD_DATA.IuVehDynamicsPort.sSigHeader.uiCycleCounter",
					 "SigHeader.eSigStatus":"ADC5xx_Device.VD_DATA.IuVehDynamicsPort.sSigHeader.eSigStatus",
					 "SigHeader.a_reserve":[1]
                  }
               }
            },
            "type":"signal_name",
            "value":"ADC5xx_Device.EM_DATA.EmApParkingBoxPort",
            "clients":{
               "sicoregeneric":{
                  "SiCoreGeneric_simpleTrigger":{
				  
                  }
               }
            }
         }
      }
   }
}
```

If we take a close look at the .sdl file and the .json config, we can see that the defined member data and the .sdl, we can see that they match. Each Signal defined in the .sdl file can also be found in the .json configuration.
The end result can been seen in Table view, where the structure of the data reflects the structure in the .sdl file

<br><img src="visualize_bcd_backend_config9.png"><br>

---

# Adding signals {#howto_adapt_base_control_sdl_for_different_data_generation_adding_signals}

When adding new member data in the configuration file, it is important that the .sdl is update accordingly (group, signal, offset, size etc.)

For simplicity, I have created a **member_data** section for the trigger **SiCoreGeneric_simpleTrigger** and copied the **SigHeader.uiTimeStamp** from the fallback data. Finally I have renamed it to **SigHeader.uiTimeStamp2**, but the URL will stay the same, so to verify that data is actually correct.

JSON:
<br><img src="sdl_bcd_extra2.png"><br>

```json
{
   "next_player":{
      "orchestrator":{
         "flow1":{
            "meta":{
               "delay":0,
               "sync":true,
               "control_data_generation":{
                  "sdl_file":"sig_header.sdl",
                  "member_data":{
                     "SigHeader.uiTimeStamp":"ADC5xx_Device.VD_DATA.IuVehDynamicsPort.sSigHeader.uiTimeStamp",
                     "SigHeader.uiMeasurementCounter":"ADC5xx_Device.VD_DATA.IuVehDynamicsPort.sSigHeader.uiMeasurementCounter",
                     "SigHeader.uiCycleCounter":"ADC5xx_Device.VD_DATA.IuVehDynamicsPort.sSigHeader.uiCycleCounter",
					 "SigHeader.eSigStatus":"ADC5xx_Device.VD_DATA.IuVehDynamicsPort.sSigHeader.eSigStatus",
					 "SigHeader.a_reserve":[1]
                  }
               }
            },
            "type":"signal_name",
            "value":"ADC5xx_Device.EM_DATA.EmApParkingBoxPort",
            "clients":{
               "sicoregeneric":{
                  "SiCoreGeneric_simpleTrigger":{
					"member_data":{
						"SigHeader.uiTimeStamp2":"ADC5xx_Device.VD_DATA.IuVehDynamicsPort.sSigHeader.uiTimeStamp"
					}	
                  }
               }
            }
         }
      }
   }
}
```

The changes have to be added to the .sdl as well.

SDL:
<br><img src="sdl_bcd_extra3.png"><br>

```xml
<SdlFile xmlns:xsd="http://www.w3.org/2001/XMLSchema" ByteAlignment="1" Version="2.0">
  <View Name="?ViewName?" CycleID="207">
    <Group Name="SigHeader" Address="20350000" ArrayLen="1" Size="16">
      <Signal Name="uiTimeStamp" Offset="0" ArrayLen="1" Type="ulong" Bitmask="ffffffff" ByteOrder="little-endian"
              Size="4"/>
      <Signal Name="uiMeasurementCounter" Offset="4" ArrayLen="1" Type="ushort" Bitmask="ffff" ByteOrder="little-endian"
              Size="2"/>
      <Signal Name="uiCycleCounter" Offset="6" ArrayLen="1" Type="ushort" Bitmask="ffff" ByteOrder="little-endian"
              Size="2"/>
      <Signal Name="eSigStatus" Offset="8" ArrayLen="1" Type="uchar" Bitmask="ff" ByteOrder="little-endian" Size="1"/>
      <Signal Name="a_reserve" Offset="9" ArrayLen="3" Type="uchar" Bitmask="ff" ByteOrder="little-endian" Size="1"/>
	  <Signal Name="uiTimeStamp2" Offset="12" ArrayLen="1" Type="ulong" Bitmask="ffffffff" ByteOrder="little-endian"
              Size="4"/>
    </Group>
  </View>
</SdlFile>
```

Close attention to the **name** and especially to the **signal offset** and **group size**, these need to be correctly defined, failing to do so will result faulty memory coping. 

The result can be seen in Table view:

<br><img src="sdl_bcd_extra.png"><br>

We can see that the signal **SigHeader.uiTimeStamp2** is now available and since it uses the same URL as **SigHeader.uiTimeStamp**, both signals have the same value, which is the behaviour we are expecting.

---