How to verify trigger setup with eCAL Monitor {#howto_verify_trigger_setup_with_ecal}
=====

[TOC]

---
# References {#howto_verify_trigger_setup_with_ecal_references}

* @ref howto_configure_base_control_data_main
* @ref howto_configure_base_control_data
* @ref howto_visualize_base_control_data
* @ref howto_adapt_base_control_sdl_for_different_data_generation

---

# Overview {#howto_configure_base_control_data_overview}

This tutorial will show how to check if the triggers are setup properly using eCAL Monitor.

---

# Steps

> 📝 Make sure you also have your algo node started. For this example si_core will be used.

Follow the steps on how to visualize the base control data from below:  
@ref howto_visualize_base_control_data  
  
After performing the above steps, open **eCAL Monitor** and click on the **Services** tab.

<br><img src="trigg_ecal.png"><br>

The windows should look something like this:

<br><img src="trigg_ecal2.png"><br>

Notice the **Method** name: 

<br><img src="trigg_ecal3.png"><br>

If we look in our current config, we can se that the **Method** name is actually made up of the **component** name and **trigger** name.

<br><img src="trigg_ecal4.png"><br>

If we **step once** we can see the data in **Table view** getting updated.

<br><img src="trigg_ecal5.png"><br>

This is also reflected in eCAL Monitor as the **Call count** gets increased to one.

<br><img src="trigg_ecal6.png"><br>

This way we know that our trigger is configured properly.

---

# Example of a BAD configuration

We will be performing the same steps as before, but we will make one small change in the **json** config file.

<br><img src="trigg_ecal7.png"><br>

The **trigger** name has been changed from **SiCoreGeneric_simpleTrigger** to **SiCoreGeneric_simpleTrigger2**.  

When stepping once, we can see that **Call count does not increase**.

<br><img src="trigg_ecal8.png"><br>

---