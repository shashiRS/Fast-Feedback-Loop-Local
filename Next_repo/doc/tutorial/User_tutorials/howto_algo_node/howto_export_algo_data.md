How to export Algo Node data from NEXT {#howto_export_algo_data}
=====

[TOC]

---

# Overview {#howto_export_algo_data_overview}

This tutorial will present how to **export data** and how to use the **Signal Exporter**.

---

# References {#howto_export_algo_data_references}

* @ref howto_algo_node_main
* @ref howto_integrate_into_cobolt
* @ref howto_configure_next
* @ref howto_set_triggers

---

## Prerequisites
-Next integrated into Algo Node (@ref howto_integrate_into_cobolt)<br>
-Next configured (@ref howto_configure_next)<br>
-Next trigger(s) set up (@ref howto_set_triggers)

---

# Steps {#howto_export_algo_data_steps}
Steps required to export signals data from Algo Node in Next.

##1. Start Next
Start **Next Client (GUI)**, **next_controlbridge.exe**, **next_databridge.exe**, **next_player.exe**.

##2. Start up your node
**Start your node** with <B>-n</B> argument. In this example: next_ecal_swc_nextsync_0.exe -n.

##4. Load a recording
In the GUI go to **File -> Load Recording -> select a recording from your disk**. This recording should contain the **signals configured** in **ecal_composition.ini** and in **trigger_config.json**. <br>
In this example <B>Cut_00.00.05.042.703.zrec</B> was used from `cobolt_test\conan_workarea\package.cobolt_test.generic.btest.GENERIC.1.0.0-fallback.vs2017_debug\tests\cases\NextSyncTest` <br>

##5. Add a Signal Exporter
In the GUI go to **Tools -> select Signal Exporter**. <br>

##6. Open Signal Explorer
To explore the available signals, **click on Select signals button**:
<img src="exporter_select_signals.png"> <br>

##7. Select signals to export
**Select** the signals you want to **export**. It is recommended to export from the **recording** (ARS5XX) and from the **Node** (SIM VFB) as well. <br>
In this **example**: `ARS5XX -> AlgoVehCycle -> VehDyn -> sSigHeader -> uiMeasurementCounter` and `SIM VFB -> NextSync_0 -> providePort_VehDyn_no_sync -> sSigHeader -> uiMeasurementCounter`.
<img src="export_first_signal.png"> <br>
<img src="export_second_signal.png"> <br>

##8. Exporter settings
In Signal Exporter, from the dropdown list choose the **exported file type** (ex. Csv Exporter) and in the other combo box set the **path for the exported file** location (ex. D:\export_folder).<br>
In **Trigger** section set the **trigger type** (ex. Signal) and from the Signals list **select the signal** you want use as trigger for the exporter (ex. SIM VFB.NextSync_0.providePort_VehDyn_no_sync.sSigHeader.uiMeasurementCounter) and press **Set as Trigger** button:<br>
<img src="export_set_trigger.png"> <br>

<img src="export_trigger_set.png"> <br>

##10. Start exporting
In Signal Exporter press <B>Start Export</B> and <span style="color:RED">WAIT!!! until in **next_databridge** "Full subscribe message appears"</span> (this initialization may take a <B>few minutes</B>): <br>
<img src="export_start.png"> <br>

##11. Play recording
Click on <B>play recording</B> button and wait until it finishes: <br>
<img src="export_play_rec.png"> <br>

##12. Stop export
When playing finished press <B>Stop Exporting</B> in Signal Exporter.

##13. Open file
Open the exported file (ex. exported_file__D2023_11_01_T13_21_46.csv): <br>
<img src="open_exported_file.png"> <br>
Here we can see the **timestamp** and the **two exported signals**, one from the **recording** (ARS5XX.AlgoVehCycle.VehDyn.sSigHeader.uiMeasurementCounter) and one from the **Algo node** (SIM VFB.NextSync_0.providePort_VehDyn_no_sync.sSigHeader.uiMeasurementCounter).

---

# Tips and known problems {#howto_export_algo_data_tips}
 - This is a general tutorial, it means **needs to be adapted** for every project individually as  **signal names**, **node name**, **recording**, **trigger** etc. differs.

---
