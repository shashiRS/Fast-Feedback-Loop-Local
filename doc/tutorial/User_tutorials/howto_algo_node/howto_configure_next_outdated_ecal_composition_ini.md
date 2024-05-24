How to configure NEXT (Outdated) {#howto_configure_next_outdated}
=====

[TOC]

---

# Overview {#howto_configure_next_outdated_overview}

<span style="color:RED">This is an **outdated tutorial**</span> which describes the old method how to configure Next using **ecal_composition.ini**. 

There is a <span style="color:GREEN">**newer tutorial**</span> to configure Next, <span style="color:GREEN">this one is **recommended**</span>: @ref howto_configure_next

---

# References {#howto_configure_next_outdated_references}

* @ref howto_algo_node_main
* @ref howto_integrate_into_cobolt
* @ref howto_set_triggers
* @ref howto_export_algo_data

---

## Prerequisites
-Next integrated into Algo Node (@ref howto_integrate_into_cobolt)

---

# Steps {#howto_configure_next_outdated_steps}
Steps required to configure Next to work properly with the Algo Node.

##1. Configuration file
The generation process creates a **ecal_composition.ini** file. This file **defines the connections** between the **node** and the **environment** (ex. output from a recording).<br>
To configure Next to work properly with the node, you need to modify **ecal_composition.ini**, which is located in the same folder where the **node executable** (ex. next_ecal_swc_nextsync_0.exe) is.<br>
Usually is in <B>conan_workarea\package\bin</B> folder. In this example **ecal_composition.ini** is located in `cobolt_test\conan_workarea\package.cobolt_test.generic.btest.GENERIC.1.0.0-fallback.vs2017_debug\tests\cases\NextSyncTest`.

##2. Modify ecal_composition.ini
Open **ecal_composition.ini** in text editor(ex. Notepad++):<br>
<img src="node_configure_ecal.png"><br>
The ports can either be connected to Carmaker outputs or to URLs coming from a **recording**. <br>
Format of connections is `<publisher_instance>.<publisher_portname> > <subscriber_instance>.<subscriber_portname>`<br>
Which in this example is `ARS5xx.AlgoVehCycle.VehDyn > NextSync_0.requestPort_VehDyn_no_sync`<br>
You need to <span style="color:RED"><B>modify the left side</B></span> (ex. ARS5xx.AlgoVehCycle.VehDyn) based on **what URL from you recording you want to use as input**.

##3. Start Next
Start Next Client (GUI) and start from your repository(where your node is located) **next_player**, **next_controlbridge**, **next_databridge** and the <B>node executable with -n</B> (ex. in cmd: next_ecal_swc_nextsync_0.exe -n).<br>

##4. Load recording
Load the recording, which **contains the URL configured** in ecal_composition.ini as input.<br>
In this example <B>Cut_00.00.05.042.703.zrec</B> was used from `cobolt_test\conan_workarea\package.cobolt_test.generic.btest.GENERIC.1.0.0-fallback.vs2017_debug\tests\cases\NextSyncTest`

##5. Open eCal Monitor
Open **eCal Monitor** and right click somewhere on tabs header (ex. Topic), then mark **Loc.Connections** checkbox to **activate it**:<br>
<img src="activate_loc_connections_ecal.png"><br><br>
**Check your node**, it should have **Loc.Connections** more than **0** if is configured **correctly**:
<img src="node_configured.png"><br>

##6. Play recording
Start playing the recording and check eCal Monitor to see **if data is being received**:
<img src="node_configured_play_data_received.png"><br>

## Conclusion
If you **arrived here**:
 - At this moment Next has been <span style="color:GREEN">configured</span>.
 - Node is able to <span style="color:GREEN">receive data</span> from the recording.

**To continue this tutorial, next step would be to set trigger(s)**: @ref howto_set_triggers

---

# Tips and known problems {#howto_configure_next_outdated_tips}
 - This is a general tutorial, it means **needs to be adapted** for every project individually as **filenames**, **paths**, **signal names**, **node name** etc. differs.

---
