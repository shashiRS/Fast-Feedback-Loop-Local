Configure for Linux {#next_linux}
=====

[TOC]

---

# Overview {#next_linux_overview}

This tutorial will present how to install **Next** environment for **development** on **Linux** operating system.

---

# References {#next_linux_references}

* @ref next_windows
* @ref howto_use_gui
* @ref howto_use_ecal

---

## Prerequisites
Make sure you have installed all the **prerequisites**:
 - Ubuntu 18.04 or newer\n
 - Setup Linux on VDI: https://github-am.geo.conti.de/pages/ADAS/mts-docs/onboarding-guide/devbox_setup/vdi.html
 - Install VMware Horizon Client from Portal Manager
 - Request admin rights for the Linux machine

\warning The support of Ubuntu 18.04 will end in December 2023. From 2024 on only Ubuntu 20.04 is supported.

---

# Steps {#next_linux_steps}

Needed steps to prepare the system for Next.

##1. Get your Linux machine ready
Update and upgrade your Ubuntu. Open a **Terminal** and run these commands: \n 
 -# **sudo apt-get clean**
 -# **sudo apt-get update && sudo apt-get upgrade**

##2 Check if everything is set properly for your machine:
 -# Open **CMI Ubuntu -> Tools and press Parse CMI** (it will propagate the sudo rights you requested) and then press **Run Puppet** (it will check the configuration of the system)
 -# Finally, open **CMI Ubuntu -> System Check** and press **Run checks**
 -# You **shouldn't get any errors** while updating and checking the system. If you get errors, **try to fix** them before moving on to the next steps. There are two errors that in most cases **can be ignored**: **Checking mcaffe**, **Checking shortcuts**.

##3 Setup the needed environment
**Install** the needed software: https://confluence.auto.continental.cloud/pages/viewpage.action?pageId=598941366 \n
<span style="color:RED">After installing docker, the machine might become inaccessible, and a support ticket needs to be created in order to regain access.</span> \n
Support ticket can be created from https://vdissp.conti.de/. Go to **Virtual machines -> Click on the three dots -> Create support ticket**.

##4. Create your SSH-key for git
More **detailed guide** available here: https://docs.github.com/en/authentication/connecting-to-github-with-ssh/generating-a-new-ssh-key-and-adding-it-to-the-ssh-agent?platform=linux \n
 -# Open a **Terminal**
 -# Generate **new SSH-key**, replace with your email and run: `ssh-keygen -t ed25519 -C "your_email@example.com"`
 -# Open the file where that key is generated and **copy it**
 -# **Add** the SSH-key to GitHub on: https://github-am.geo.conti.de/settings/ssh/new

##5 Clone NEXT git repository
**Create a new folder** for repositories, for example: <B>/home/Next_Repos</B> \n
Make sure you have <span style="color:RED">**at least 30 GB free space**</span> on your drive. \n
Open **Terminal** in that folder and run: `git clone git@github-am.geo.conti.de:ADAS/next.git`

##6 Prepare for bricks:
 -# Open **Terminal** in the cloned **next** folder (ex. /home/Next_Repos/next)
 -# Grant **execute** permission `chmod +x ./scripts/cip.py`
 -# Run the **sync** command: `python3 ./scripts/cip.py sync`
 -# Install the newest **NEXT-GUI** (Next-Client-X.XX.XXX.AppImage): https://github-am.geo.conti.de/ADAS/Next-GUI/releases

##7 Build with bricks:
 -# Open **Terminal** in the cloned **next** folder (ex. /home/Next_Repos/next)
 -# In your terminal run a **build command** depending on your needs (if unsure then choose release for your Ubuntu version):
 - **Ubuntu 18.04 release** build: `python3 ./scripts/cip.py build --platform linux64_x86_gcc_7-v2-release`
 - **Ubuntu 18.04 debug** build: `python3 ./scripts/cip.py build --platform linux64_x86_gcc_7-v2`
 - **Ubuntu 20.04 release** build: `python3 ./scripts/cip.py build --platform ubuntu_20_04_x86_64-release`
 - **Ubuntu 20.04 debug** build: `python3 ./scripts/cip.py build --platform ubuntu_20_04_x86_64`
 - **Ubuntu 18.04 unit test** build: `python3 ./scripts/cip.py build --platform linux64_x86_gcc_7-v2 --test-type GENERIC`
 - **Ubuntu 20.04 unit test** build: `python3 ./scripts/cip.py build --platform ubuntu_20_04_x86_64 --test-type GENERIC`

---

# Good to know {#next_linux_good_to_know}

 - **Ubuntu 18.04** platform names (only supported until end of 2023): \n
   linux64_x86_gcc_7-v2-release \n
   linux64_x86_gcc_7-v2 \n \n

 - **Ubuntu 20.04** platform names: \n
   ubuntu_20_04_x86_64 \n
   ubuntu_20_04_x86_64-release \n \n

 - **VDI machines** can be managed (start, stop, reload, create tickets etc.) from here: https://vdissp.conti.de/


# Tips and known problems {#next_linux_tips}
- It is recommended to not use **--no-docker**
**Docker** simplifies application packaging by creating standardized containers, which encapsulate both the application and its dependencies.
---