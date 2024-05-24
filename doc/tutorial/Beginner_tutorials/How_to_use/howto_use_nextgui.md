How to use NEXT-GUI {#howto_use_gui}
=====

[TOC]

---

# Overview {#howto_use_gui_overview}

This tutorial will show step by step how to run <B>NEXT-GUI, load/save a configuration, recording, add traffic objects and signals</B>.

---

# References {#howto_use_gui_references}

* @ref next_windows
* @ref howto_use_logs
* @ref howto_use_ecal

---

## Prerequisites
Make sure you have installed all the **prerequisites**:
 - Next Client (GUI) 
 - Next repository cloned and built (release recommended)

---
# Chapters {#howto_use_gui_chapters}
Each chapter will describe a functionality of Next. This will provide you a **basic knowledge** about how to use Next.

##1. How to start Next
<span style="text-decoration:underline">**Start all three components individually (backend):**</span><br>
To start the environment, in your locally built Next repository go to: <B>next\conan_workarea\package.next.generic.0.0.0-fallback.vs2017\bin</B><BR>
There are three executables that needs to be started individually: <B>next_controlbridge.exe, next_databridge.exe, next_player.exe</B> .<br>

Start all three of them <B>one by one</B> with <B>double click</B> or right click and Execute (order doesn't matter):
<img src="three_executables.png"><br>

The <B>Command Prompts can be minimized</B> to taskbar:<br>
<img src="dbg_windows_minimize_cmds.png"><br>
Please, <span style="color:RED"><B>avoid clicking in the Command Prompts</B></span> because that will <B>pause the execution</B>. If accidentally click was made in the Command Prompt just press Enter to continue the execution.

<span style="text-decoration:underline">**Start Next Client (GUI):**</span><br>
After the three Main components were started, next step is to <B>launch the Next Client</B> (GUI):
<img src="next_client_shortcut.png"><br>
Next Client (GUI) will ask for a configuration,  for this example you can select <B>Start New Session</B>.<br>
The most important is after Next Client (GUI) window opens, check the three main <B>components connection status:</B>
<img src="components_connected_to_GUI.png"><br>
If the three components are connected successfully to Next Client (GUI), then <B>green and blue colours</B> will appear in the <B>Connections</B> tab. 
But, you can hover over the tab, with the mouse to check the status description.

---

##2. Configuration loading
The Next GUI provides several configuration interfaces.

- GUI Widget Config
- **GUI Connection Settings**
- Algo Nodes

The **GUI Connection Settings** is where the Next endpoints can be configured (IP/Port). These can be configured manually, using the values provided above or loaded using a custom JSON file.

We provide a **configuration** JSON in the following shared drive, **copy it to your local drive** (ex. D:\recordings):
`\\cw01.contiwan.com\Root\Loc\ulm2\didt6448\Recordings\sim_team_share\Recordings\HA22.00.09.01.0.153\backend_config.json`

Now, going back to the GUI, select `GUI Connection Settings -> Load -> Open File...` and choose `backend_config.json` from your local drive:
<img src="howto_gui_config_loading.png"><br>
Afterwards, if the file is not moved, it should appear under RECENT. This should be selected every time the GUI is opened to force a reconnection to the correct IP/Ports of the Next.

After this is done, the GUI and both **Data Bridge** and **Control Bridge** attempt automatic re-connects regularly. The successful bridges connections can be checked on the top right corner, by 2 green lights.
<img src="howto_gui_endpoints_connected.png">

<span style="color:RED">Make sure only one instance of each bridge is running.</span>
All 3 main components for open-loop simulation are now running, connected and ready to simulate.

---

##3. Load a recording
We provide a **recording** in the following shared drive, **copy it to your local drive** (ex. D:\recordings):
`cw01.contiwan.com\Root\Loc\ulm2\didt6448\Recordings\sim_team_share\Recordings\HA22.00.09.01.0.153\2022.02.14_at_12.48.46_radar-mi_663_ext_00.01.29.491.027_00.05.29.999.022.rrec`

In the Next GUI, on the top left of the Control Toolbar, click on the file icon to **load a recording** and select the copied `2022.02.14_at_12.48.46_radar-mi_663_ext_00.01.29.491.027_00.05.29.999.022.rrec` file from your local drive:
<img src="howto_load_recording.png">

The GUI provides feedback when the recording will be loaded. In the meantime, check the output of both **Control Bridge** and **Next Player** to verify everything is going OK.

<img src="howto_recording_loaded_control_player.png">

In the GUI, the successful recording load can be checked by the message popup, and the green checksign in the Control Toolbar.

<img src="howto_recording_loaded_gui.png">

---

##4. Start widgets
The Next GUI has different widgets that provide different visualization capabilities.
The list can be accessed either by hovering the mouse on the rightmost side of the GUI and clicking the popup handle **Manage Widgets** or by selecting `GUI Widget Config -> Manage Widgets` on the top.
<img src="howto_gui_manage_widgets.png">
The Widgets menu appears on the right side.

---

##5. Table View widget
Select **Table View**:
<img src="howto_widgets_table_view_select.png">

Click on **Select Signals**:
<img src="howto_table_view_select_signals.png">

Click on the magnifying glass to open the **Signal Explorer** and search for the available signals:
<img src="howto_table_view_signal_search.png">

In the **Signal Explorer**, click on the update button next to **Available Signals**:
<img src="howto_table_view_refresh_signals.png">

Now we can select a couple of interesting **signals** for visualization:
- ADC4xx.EML.EmlOutput.egoVehKinematic.egoMotion.velocityTrans.x.mu
- GPS_Mi.GPSCycle.NMEA_GPGGA.Latitude
- GPS_Mi.GPSCycle.NMEA_GPGGA.Longitude

These can be selected by expanding each of the arrows in the **Signal Tree** in the list of **Available Signals**. Double click on each of the desired signals, and they will appear on the right side, under **Selected Signals**:
<img src="howto_signal_velocity.png">
<img src="howto_signal_gps.png">
Now click on **Confirm** at the bottom.

Verify that you have 3 Signal(s) selected in the **Signal Selection** popup and close it:
<img src="howto_signals_select_close.png">

Now, back in the **Table View** widget, nothing seems to be there. This is normal behaviour, as the table will only be populated once the data starts to come in.
Go ahead and press the **Play button** in the **Control Toolbar**:
<img src="howto_table_view_play.png">

If everything is running smoothly, you should see the 3 values changing. You can pause the playback anytime to analyse the values:
<img src="howto_gui_table_view_moving.png">

---

##6. 3D View widget
Repeat the process of opening the widgets menu **Manage Widgets** and this time select the **3D View**:
<img src="howto_widgets_3dview_select.png">

Resize and adjust the 3D View Widget as convenient. Click on the **Gear button** to configure:
<img src="howto_3dview_gear.png">

In the Widget Configuration menu, click on the **Magnifying glass button** next to **3D Visualization Data Stream Configuration**:
<img src="howto_3dview_glass.png">

This opens up the **Data Class Configuration** menu. Click on the **Refresh button** next to **Available Configurations**:
<img src="howto_3dview_config_update.png">
This makes a request to the configuration server, and the possible Data types for each signal available for 3D Visualization.

**Choose** the following:
- egoVehicle FdpBaseEgoVehicleKinematics
- groundLines fdp_base_roadmodel

They can be selected by expanding the arrow icons and double clicking. They should appear on the right side after expansion.
Scroll down and click on **Confirm**:
<img src="howto_3dview_config_select.png">
Close the **3D View Configuration** popup and go back to the main view of the Next GUI.

Press play again to visualize the Ego Vehicle in motion and the Road Model lines on the ground moving:
<img src="howto_openloop_full.png">

---

# Tips and known problems {#howto_use_gui_tips}
 - **Loading a recording** while Next initialization was not finished may throw some error, it is recommended to wait a few moments and **retry**
 - If a **recording has a bigger size** it may take a **bit more time to load** it
 - In **Signal Exporter** when **Start Exporting** is pressed, it takes some time initialize depenging on how many signals are selected. Solution is to check in the **Log View** filtered for **next_databridge** when <B>"Full subscribe"</B> message appears, it means initialization is done.

---
