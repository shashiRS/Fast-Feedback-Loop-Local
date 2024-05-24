How to configure NEXT {#howto_configure_next}
=====

[TOC]

---

# Overview {#howto_configure_next_overview}

This tutorial will present how to configure Next to work properly with the Algo Node.

There is also an outdated method with ecal_composition.ini which is **not recommended** (may be informative for legacy systems): @ref howto_configure_next_outdated

---

# References {#howto_configure_next_references}

* @ref howto_algo_node_main
* @ref howto_integrate_into_cobolt
* @ref howto_set_triggers
* @ref howto_export_algo_data

---

## Prerequisites
-Next integrated into Algo Node (@ref howto_integrate_into_cobolt)

---

# Steps {#howto_configure_next_steps}
Steps required to configure Next.

##1. Start Next
Start Next Client (GUI) and start from your repository(where your node is located) **next_player**, **next_controlbridge**, **next_databridge** and the <B>node executable with -n</B> (ex. in cmd: next_ecal_swc_nextsync_0.exe -n).<br>

##2. Save global configuration file
Go to <B>Global Simulation Configuration</B> -> click on <B>Fetch Configuration</B> -> <B>wait until the configuration diagram is created</B> -> click on <B>Save Configuration</B> button -> <B>Give a name</B> (ex. user_tutorial_config.json) and <B>Save it somewhere on your disk</B> (ex. where your node is located): <br>
<img src="global_config_initial.png"> <br>

##2. Edit configuration file
Next step is to <B>open the saved configuration file (.json)</B> from your disk in a text editor (ex. Notepad) and add `"composition"` to it, after the node's `"out_port"` section:
```
"composition": {
    "instance_name": {
      "subscriber_portname": {
        "url": "URL_from_rec_you_want_to_connect"
      }
    }
}
```
Where you need to replace **instance_name**, **subscriber_portname** and **URL_from_rec_you_want_to_connect** based on your configuration: <br>
 - **instance_name**: is defined in the **project.plantuml** (ex. BEGIN_INSTANCE(NextSync_0, next_test.NextForward, 1))
 - **subscriber_portname**: is the defined in the **component.plantuml** (ex. ADD_PORT_REPLYTO_ASYNC_PUSH(NextForward, requestPort_VehDyn_sync_last, next_test.VehDyn))
 - **URL_from_rec_you_want_to_connect**: URL coming from a **recording** or Carmaker output in `<publisher_instance>.<publisher_portname>` format (ex. ARS5xx.AlgoVehCycle.VehDyn) <br>

Here is a concrete **example** with four subscriber ports, the first with two URLs:
```
"composition": {
    "NextSync_0": {
     "requestPort_VehDyn_no_sync": {
        "url": [
                "ARS5xx.AlgoVehCycle.VehDyn_non_existent",
                "ARS5xx.AlgoVehCycle.VehDyn"
            ]
     },
     "requestPort_VehDyn_sync_last": {
        "url": "ARS5xx.AlgoVehCycle.VehDyn"
     },
     "requestPort_VehDyn_sync_exact": {
        "url": "ARS5xx.AlgoVehCycle.VehDyn"
     },
     "requestPort_VehDyn_sync_syncref": {
        "url": "ARS5xx.AlgoVehCycle.VehDyn"
     }
    }
}
```

Here this `"ARS5xx.AlgoVehCycle.VehDyn_non_existent",` line is just an **example** that if a URL is **not available**, it will **choose the next one**, in this case `"ARS5xx.AlgoVehCycle.VehDyn"`. This is how you can **define more URLs**.

Here is an **example** how it was added:
<img src="composition_added.png"> <br>

To inspire from a **working example** you can have a look into this file: [mf_sil/conf/next/ADC5/adc5_connection.json](https://github-am.geo.conti.de/ADAS/mf_sil/blob/45a0173e3cbe175eb809de7d9e323718fe0e2779/conf/next/ADC5/adc5_connection.json)

After the modifications are done, <B>save the changes</B> in the file. <br>

##3. Load configuration file
From Next Client (GUI) in Global Simulation Configuration tab press <B>Load Configuration</B> button and select your **modified configuration file** (user_tutorial_config.json). You should be able to see the **modifications on the diagram**: <br>
<img src="loaded_config_recording_not_loaded.png">

##4. Load recording
Load the recording, which **contains the URL configured** as input.<br>
In this example <B>Cut_00.00.05.042.703.zrec</B> was used from `cobolt_test\conan_workarea\package.cobolt_test.generic.btest.GENERIC.1.0.0-fallback.vs2017_debug\tests\cases\NextSyncTest`

##5. Open eCal Monitor
Open **eCal Monitor** and right click somewhere on tabs header (ex. Topic), then mark **Loc.Connections** checkbox to **activate it**:<br>
<img src="activate_loc_connections_ecal.png"><br><br>
**Check your node**, it should have **Loc.Connections** more than **0** if is configured **correctly**:
<img src="json_configured_rec_loaded.png"><br>

##6. Play recording
Start playing the recording and check eCal Monitor to see **if data is being received**:
<img src="json_configured_play_data_received.png"><br>

## Conclusion
If you **arrived here**:
 - At this moment Next has been <span style="color:GREEN">configured</span>.
 - Node is able to <span style="color:GREEN">receive data</span> from the recording.

**To continue this tutorial, next step would be to set trigger(s)**: @ref howto_set_triggers

---

# Tips and known problems {#howto_configure_next_tips}
 - This is a general tutorial, it means **needs to be adapted** for every project individually as **filenames**, **paths**, **signal names**, **node name** etc. differs.

---
