How to visualize Base Control Data {#howto_visualize_base_control_data}
=====

[TOC]

---

# References {#howto_visualize_base_control_data_references}

* @ref howto_configure_base_control_data_main
* @ref howto_configure_base_control_data
* @ref howto_adapt_base_control_sdl_for_different_data_generation
* @ref howto_verify_trigger_setup_with_ecal

---

# Overview {#howto_visualize_base_control_data_overview}

This tutorial will present how to configure Next in order to visualize the Base Control Data.

---

# Steps

> 📝 Make sure to follow the **steps** in this **exact order**, as the **json config** needs to be loaded **before loading the recording**.

> 📝 Before beginning, make sure that the **json** and **sdl** files are in the **same directory as the player**.

**1.** Open Next (player, data_bridge, control_bridge and whatever node is needed). Make sure everthing is running properly.
For more information, please check the tutorial below:

@ref howto_use_gui

<br><img src="visualize_bcd.png"><br>

**2.** Go to **Backend Configuration**.

<br><img src="visualize_bcd_backend_config.png"><br>

**3.** Go to **Global Simulation Configuration**.

<br><img src="visualize_bcd_backend_config2.png"><br>

**4.** Click on **Load Configuration**.

<br><img src="visualize_bcd_backend_config3.png"><br>

**5.** Load configuration .json file. Your window should look something like this: 

<br><img src="visualize_bcd_backend_config4.png"><br>

**6.** Click on the drop down for **next_player** and also select the **checkbox**. You should see **Orchestrator** and your **config**.

<br><img src="visualize_bcd_backend_config5.png"><br>

**7.** Load a recording.

**8.** Go to table view.

<br><img src="visualize_bcd_backend_config6.png"><br>

**9.** Click on **Add Signals**.

<br><img src="visualize_bcd_backend_config7.png"><br>

**10.** You should see **Orchestrator** in the available data sources. Select the structure from the your configured **flow**.

<br><img src="visualize_bcd_backend_config8.png"><br>

**11.** Play the recording and you should see data in **Table View**.

<br><img src="visualize_bcd_backend_config9.png"><br>

---