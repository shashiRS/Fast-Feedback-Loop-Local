How to export data from NEXT {#howto_export_data}
=====

[TOC]

---

# Overview {#howto_export_data_overview}

This tutorial will present how to **export data** and how to use the **Signal Exporter**.

---

# References {#howto_export_data_references}

* @ref howto_use_ecal
* @ref howto_use_gui
* @ref howto_use_logs
* @ref howto_use_python_api

---

## Prerequisites
Make sure you have installed all the **prerequisites**:
 - Next Client (GUI)
 - Next repository cloned and built (release is recommended)

---

# Steps {#howto_export_data_steps}
Steps required to export signals data in Next and how to configure the Signal Exporter.

##1. Start Next
Start **next_controlbridge.exe**, **next_databridge.exe**, **next_player.exe**.

##2. Start Next Client (GUI)
Start **Next Client (GUI)** and select <B>"start new session"</B> for **both backend and widgets**.<br>

##4. Load a recording
In the GUI go to **File -> Load Recording -> select a recording from your disk (.rrec)**. <br>
If you don't have a recording, you can copy **4_parallel_park_in_succ.rrec** from `cw01.contiwan.com\Root\Loc\ulm2\didt6448\Recordings\sim_team_share\Next Feature Integration Test\TestCases\reworked_tests\Parking_and_SI_Core` to your local drive (ex. C:\Recordings).

##5. Add a Signal Exporter
In the GUI go to **Tools -> select Signal Exporter**. <br>

##6. Open Signal Explorer
To explore the available signals, **click on Select signals button**:
<img src="exporter_select_signals.png"> <br>

##7. Select signals to export
In the Signal Explorer window **Expand camera-mi_gps_nmea_0 dropdown -> expand GPSCycle -> double click on NMEA_GPRMC** to add all signals from **NMEA_GPRMC** section or you can add them one by one (double click), then press Confirm button:
<img src="signals_to_export.png"> <br>

##8. Export settings
In Signal Exporter, from the dropdown list choose the exported file type, in this example **Csv Exporter** and in the other combo box **set the path** where the file will be generated, ex. D:\Next_tutorial:
<img src="export_settings.png"> <br>

##9. Trigger settings
In Signal Exporter, from the dropdown list choose the **trigger type**, in this example **Signal** and from the Signals list select **camera-mi_gps_nmea_0_0.GPSCycle.NMEA_GPRMC.UTCTime** and press **Set as Trigger** button:
<img src="export_trigger.png"> <br>

##10. Start export
In Signal Exporter press <B>Start Export</B> and <span style="color:RED">**WAIT!!!** until in **next_databridge** "Full subscribe message appears"</span> (this initialization may take a <B>few minutes</B>):
<img src="export_wait.png"> <br>

##11. Play recording
Click on <B>play recording</B> button.

##12. Stop export
When playing finished press <B>Stop Exporting</B> in Signal Exporter.

##13. Open file
Open the exported **exported_file__D2023_10_20_T12_41_54.csv** file:
<img src="export_open_file.png"> <br>

##14. Results
**Results** should be similar:
<img src="export_results.png"> <br>

---

# Tips and known problems {#howto_export_data_tips}

---
