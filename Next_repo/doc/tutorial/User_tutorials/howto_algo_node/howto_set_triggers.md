How to set up triggers in NEXT {#howto_set_triggers}
=====

[TOC]

---

# Overview {#howto_set_triggers_overview}

This tutorial will present how to set triggers.

---

# References {#howto_set_triggers_references}

* @ref howto_algo_node_main
* @ref howto_integrate_into_cobolt
* @ref howto_configure_next
* @ref howto_export_algo_data

---

## Prerequisites
-Next integrated into Algo Node (@ref howto_integrate_into_cobolt)<br>
-Next configured (@ref howto_configure_next)

---

# Steps {#howto_set_triggers_steps}
Steps required to set triggers in Next for the Algo Node.

##1. Verify configuration files
Verify if you have **playerdefaultconfig.json** file in the same folder where your **node executable** (ex. next_ecal_swc_nextsync_0.exe) is located.<br>
In this example **playerdefaultconfig.json** would be located in `cobolt_test\conan_workarea\package.cobolt_test.generic.btest.GENERIC.1.0.0-fallback.vs2017_debug\tests\cases\NextSyncTest`.<br>
In the same folder check if you have **trigger_config.json** file.<br>
**If there is no such file there, then you can create it manually.**<br>

##2. Modify trigger_config.json
The json file allows to define different flows that can consist of several components which will be triggered in the order defined in the JSON file.<br>
For more details on the **trigger_config.json**, see the link below:<br>
@ref howto_configure_base_control_data <br>
Open **trigger_config.json** in text editor and modify it, based on this example: <br>

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
                     
                  }
               }
            },
            "type":"signal_name",
            "value":"ADC4xx_Filter.EMF.EmfGlobalTimestamp",
            "clients":{
               "NextChain_0":{
                  "CopyEmfGlobalTimesatamp":{
                     
                  }
               },
               "NextChain_1":{
                  "CopyEmfGlobalTimesatamp":{
                     
                  }
               }
            }
         }
      },
      "flow2":{
         "meta":{
            "delay":0,
            "sync":true,
            "control_data_generation":{
               "sdl_file":"sig_header.sdl",
               "member_data":{
                  
               }
            }
         },
         "type":"signal_name",
         "value":"ARS5xx.RawDataCycle.ALN_SyncRef",
         "clients":{
            "NextSync_0":{
               "CopySynchedVehDyn":{
                  
               }
            }
         }
      }
   }
}

```
In this example we have **two triggers**, where `ADC4xx_Filter.EMF.EmfGlobalTimestamp` and `ARS5xx.RawDataCycle.ALN_SyncRef` are the **URLs from recording** which will trigger the **callback function** of the nodes: <br>
<img src="trigger_callback.png">

About **type**: What kind of flow it is? It has these possible values: **time_stamp**, **cycle_start**, **cycle_end** and **signal_name**. What does each value mean to flow?<br>
 - if value is `time_stamp`: execute the flow when time value is more than the specified one from the component.
 - if value is `signal_name`: execute the flow when component specifies the same signal_name.
 - if value is `cycle_start` or `cycle_end`: execute the flow when cycle_id specified from the component matches with either `cycle_start` or `cycle_end`.

##3. Modify playerdefaultconfig.json
Next player component (next_player) uses <B>playerdefaultconfig.json</B> configuration file, where we can specify to use <B>trigger_config.json</B> settings for trigger.<br>
Open **playerdefaultconfig.json**, which is located in the same folder where the **node executable** (ex. next_ecal_swc_nextsync_0.exe) is .<br>
Add `"config_file": "trigger_config.json"` to it based on this **example**: <br>
```
{
  "next_player": {
    "orchestrator": {
      "config_file": "trigger_config.json"
    },
    "stepping": {
      "time_step": "40000"
    },
    "realtime_factor": "10",
    "filters": {
      "url": [
      ],
      "expression": [
      ]
    }
  }
}
```
In this case the settings defined in **trigger_config.json** will be loaded by **playerdefaultconfig.json** which is loaded by **next_player**.

##3. Start Next
Start Next Client (GUI) and start from your repository(where your node is located) **next_player**, **next_controlbridge**, **next_databridge** and the <B>node executable with -n</B> (ex. in cmd: next_ecal_swc_nextsync_0.exe -n).<br>

##4. Load recording
Load the recording, which <B>contains the signal(s) for trigger</B> defined in trigger_config.json.<br>
In this example <B>Cut_00.00.05.042.703.zrec</B> was used from `cobolt_test\conan_workarea\package.cobolt_test.generic.btest.GENERIC.1.0.0-fallback.vs2017_debug\tests\cases\NextSyncTest` <br>
<img src="trigger_init_player.png"><br>

##5. Open eCal Monitor
Open eCal Monitor, check if your node is **initialized**.

##6. Play recording
Start playing the recording and check eCal Monitor to see **if data is being received**: <br>
<img src="trigger_after_play.png"><br>

In this example there were **252 data packets** from which **86 triggers**, the **first one was the init trigger** (check the callback function image) and the **other 85** triggers are displayed under **Data Clock** tab.<br>
Check the **Services tab** also: <br>
<img src="triggers_ecal_services.png"><br>

## Conclusion
If you **arrived here**:
 - At this moment <span style="color:GREEN">triggers(s)</span> has been set up for Next.
 - Orchestrator is able to <span style="color:GREEN">trigger</span> Algo Node.

**To continue this tutorial, next step would be to export Algo Node data from NEXT**: @ref howto_export_algo_data

---

# Tips and known problems {#howto_set_triggers_tips}
 - This is a general tutorial, it means **needs to be adapted** for every project individually as **filenames**, **paths**, **signal names**, **node name**, **recording**, **trigger** etc. differs.
 
---
