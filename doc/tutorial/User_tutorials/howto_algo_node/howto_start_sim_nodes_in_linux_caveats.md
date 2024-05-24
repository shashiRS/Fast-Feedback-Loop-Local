Start SiCore in Linux Caveats {#next_linux}
=====

[TOC]

---

# Overview {#next_linux_overview}

This tutorial will present how to handle the struggles during starting the the Sicore using a specic version of **Next** for **development** on **Linux** operating system.

---

# References {#next_linux_references}

* @ref howto_configure_next
* @ref howto_set_triggers

---

## Prerequisites
Make sure you have all the **prerequisites**:

 - Ubuntu 18 or newer
 - Specific version of NEXT is available(released version or local release)
 
---

# Steps {#next_linux_steps}

The steps for manual integration tests.

##1. Build the Sicore repository using 
**python3 scripts/cip.py build --variant generic --platform linux64_x86_gcc_7-v2-release --no-docker**

##2 Start the nodes(player, bridges):

##3 Start the sim node using argument using **-n**

##4. If gettign error when starting the node **error while loading shared libraries: libnext.so: cannot open shared object file: No such file or directory** export the library path using **export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../lib**

##5 If no data available in ecal monitor , changed the 'shm_monitoring_domain' to 'ecal_monitoring' in ecal.ini file 

##6 Signal Exporter : Make sure the output directory specified is present in the Export settings

