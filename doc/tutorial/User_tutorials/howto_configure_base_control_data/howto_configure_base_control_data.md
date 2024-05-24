How to configure Base Control Data {#howto_configure_base_control_data}
=====

[TOC]

---
# References {#howto_configure_base_control_data_references}

* @ref howto_configure_base_control_data_main
* @ref howto_visualize_base_control_data
* @ref howto_adapt_base_control_sdl_for_different_data_generation
* @ref howto_verify_trigger_setup_with_ecal

---

# Overview {#howto_configure_base_control_data_overview}

This tutorial will present how to configure the *.json file and it's options, such as fallback mechanism and fixed versus connected values.

---

# The .json file {#howto_configure_base_control_data_json_file}

---
> 📝 **IMPORTANT** The .json configuration file needs to be located in the **same directory as the player**.

<br><img src="example_trigger_config_json.png"><br>

Inside the .json file, a flow can can be found. The flow contains the information about the fallback data, connected signals, fixed values and .sdl file used for the Base Control Data generation.

<br><img src="example_trigger_config_json_flow.png"><br>

Multiple flows can be defined, but at least **one** flow is needed in order for the Base Control Data generation to work.

Inside a flow, we can find information about the **fallback data**:

<br><img src="example_trigger_config_json_fallback.png"><br>

As well as **trigger data**:

<br><img src="example_trigger_config_json_trigger.png"><br>

Each of which can contain information about connected signals and fixed values.

**Connected signals**

<br><img src="example_trigger_config_json_connected.png"><br>

**Fixed values**

<br><img src="example_trigger_config_json_fixed.png"><br>

---

# The fallback mechanism {#howto_configure_base_control_data_fallback_mechanism}

This allows the user to define default data that the Orchestrator will use to update the defined triggers.

There are several scenarios:
* Trigger has no member data
* Trigger has some member data
* Trigger has all member data available in fallback member data

> 📝 **IMPORTANT** This only happens in memory at runtime, **the configuration you have on the disk will not actually chage**. The following images are just there as an example to help you better imagine what is happening in the background.

## Trigger has no member data

In the current configuration, the trigger has no member data.

<br><img src="example_trigger_config_json_trigger.png"><br>

Because of this, all the member data from the fallback section will be copied over.

After the .json file has been proccessed, the trigger will look internally like this:

<br><img src="example_trigger_config_json_fallback_trigger.png"><br>

## Trigger has some member data

However, if the trigger has some available data, as seen below:

<br><img src="example_trigger_config_json_trigger_member_data.png"><br>

Since **SiCoreGeneric_simpleTrigger** has the member data **SigHeader.testSignal** and this member data is not found in the fallback data, all available fallback data will be copied over. The resulting structure will contain the original trigger data and the fallback data.

<br><img src="example_trigger_config_json_trigger_member_data2.png"><br>

In the example below, **SiCoreGeneric_simpleTrigger** has member data that coincides with a member from the fallback data,
**SigHeader.uiTimeStamp**, but the URL is different.

<br><img src="example_trigger_config_json_trigger_member_data3.png"><br>

In this case, the fallback mechanism will copy only the missing member data, as seen below.

<br><img src="example_trigger_config_json_trigger_member_data4.png"><br>

## Trigger has all member data available in fallback member data

I case **SiCoreGeneric_simpleTrigger** has all member data that is also available in the fallback info, such as seen here:

<br><img src="example_trigger_config_json_trigger_member_data5.png"><br>

In this situation, **nothing** from the fallback data will be copied over to **SiCoreGeneric_simpleTrigger**.

---

# Connected signals vs fixed values {#howto_configure_base_control_data_connected_vs_fixed}

**Connected signals** are defined as **member data** that have an associated **URL**:

<br><img src="example_trigger_config_json_connected_vs_fixed.png"><br>

The latest available value, for that speicifc **URL**, will be used for the Base Control Data generation.

**Fixed values** are defined as **member data** that have an associated **array of elements**:
The array must consist of **minimum one element**.

<br><img src="example_trigger_config_json_connected_vs_fixed2.png"><br>

In this scenario, for each trigger, data is selected, **in order**, from the array.
When triggerd after the last element is selected and there are no more values, the last value will be continously used.

**Example based on the above image:**

Trigger 1: selected value is **1**  
Trigger 2: selected value is **2**  
Trigger 3: selected value is **3**  
Trigger 4: selected value is **3**  
Trigger 5: selected value is **4**  
Trigger 6: selected value is **4**  
Trigger 7: selected value is **4**  

And so on...

---