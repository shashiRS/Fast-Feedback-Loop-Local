How to use eCal {#howto_use_ecal}
=====

[TOC]

---

# Overview {#howto_use_ecal_overview}

This tutorial will present <B>eCAL (enhanced Communication Abstraction Layer)</B> usage. Communication between Next components is by events and between publishers and subscribers. eCAL is useful to monitor this data exchange.

---

# References {#howto_use_ecal_references}

* [How to Debug Next components on Windows ](@ref next_debug_windows)
* [How to Debug Next components on Linux](@ref next_debug_linux)

---
# Install {#next_debug_linux_steps}
Go to <B>Downloading and installing eCAL</B> page:
https://eclipse-ecal.github.io/ecal/index.html#downloading-and-installing-ecal

Follow the instructions and </B>install eCAL</B> for Windows or Ubuntu.

---

# Steps {#howto_use_ecal_steps}
Steps to use eCAL and most common usage example.

##1. Start Next
Start next_controlbridge.exe, next_databridge.exe, next_player.exe and Next Client (GUI).

##2. Launch eCAL Monitor
After installing eCAL, start eCAL Monitor.

##3. Check processes
From <B>Group by</B> list select Process. If Next was started correctly, under <B>Host/Process tab</B> the three main components will show up:
<img src="ecal_windows.png"><br>

##4. Process related topics
If you <B>expand one of the processes</B>, for example <B>next_player</B>, it will show all the events related to the component and the details about them:
<img src="ecal_events_windows.png"><br>

A component can be subscriber or publisher to a Topic. 

##5. Raw Data
<B>Double click</B> on entry to see <B>Raw Data</B>of that topic or right click and "Inspect topic...":

<img src="ecal_raw_data.png">

This can be useful if you are interested to see what data exchange is happening on a specific topic.

---

## eCAL config file
Configuration of eCAL can be done through a file named ecal.ini. The configuration file eCAL.ini is located with the following priority: current directory of the executable, default ecal.ini after installation (<B>C:\ProgramData\eCAL\ecal.ini</B>), default configuration will be used if no ecal.ini is found.

---

# Tips and known problems {#howto_use_ecal_tips}
 - In some cases **eCAL will not start** (newer versions), because Next uses its own ecal.ini, it is recommended to **delete it** (conan_workarea/package/bin folder), so the default will be used.

---
