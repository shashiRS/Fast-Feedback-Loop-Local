Configure for Windows {#next_windows}
=====

[TOC]

---

# Overview {#next_windows_overview}

This tutorial will present how to install **Next** environment for **development** on **Windows** operating system.

---

# References {#next_windows_references}

* @ref next_linux
* @ref howto_use_gui
* @ref howto_use_ecal

---

## Prerequisites
Make sure you have installed all the **prerequisites**:
 - Windows 10 or newer\n
 - Visual studio 2017\n 
 - Python 3.6 or greater version\n 
 - Git application\n 

---

# Steps {#next_windows_steps}

Needed steps to prepare the system for Next.

##1. Access rights:
https://github-am.geo.conti.de/ADAS\n
https://eu.artifactory.conti.de/ui/packages\n
https://jira.auto.continental.cloud/secure/RapidBoard.jspa?rapidView=44399&projectKey=SIMEN
You can **request** all them **here**: https://jira.auto.continental.cloud/plugins/servlet/desk/portal/1\n 
\n

##2. Creat workspace and run Git Bash
**Create a new folder** for repositories, for example: <B>C:\Next_Repos</B> \n
Make sure you have <span style="color:RED">**at least 30 GB free space**</span> on your drive. This folder can be created on other drives also (ex. **D:**), if there is more free space available. \n
**Right click** in your repositories folder and select **Git bash here**:\n
   <img src="repositories_folder.png"><br>

##3. Create your SSH-key for git
More **detailed guide** available here: https://docs.github.com/en/authentication/connecting-to-github-with-ssh/generating-a-new-ssh-key-and-adding-it-to-the-ssh-agent \n
 -# Generate **new SSH-key**, replace with your email and run: `ssh-keygen -t ed25519 -C "your_email@example.com"`
 -# Open the file where that key is generated and **copy it**
 -# **Add** the SSH-key to GitHub on: https://github-am.geo.conti.de/settings/ssh/new

##4. Clone NEXT git repository
Open **Git bash** in your repository folder (ex. C:\Next_Repos) and run: `git clone git@github-am.geo.conti.de:ADAS/next.git` \n 

##5.Prepare for bricks:
 -# Open **powershell/cmd** in the cloned **next** folder (ex. C:\Next_Repos\next)
 -# Run the **sync** command: `py .\scripts\cip.py sync`
 -# Install the newest **NEXT-GUI** (Next-Client-Setup-X.XX.XXX.exe): https://github-am.geo.conti.de/ADAS/Next-GUI/releases 
 -# Install **eCAL** : https://eclipse-ecal.github.io/ecal/index.html
 -# Install **Doxygen** : https://confluence.auto.continental.cloud/display/public/ToolHMISWProcess/Doxygen%2B%7C%2BInstallation%2Band%2BUsage%2BInstructions\n 
\n 

##6. Build with bricks:
Open **powershell/cmd** in the cloned **next** folder (ex. C:\Next_Repos\next) and run these commands:
 - Create a **release** build: `py .\scripts\cip.py build --platform vs2017`
 - Create a **debug** build: `py .\scripts\cip.py build --platform vs2017_debug`
 - Build **unit tests** (only debug version): `py .\scripts\cip.py build --platform vs2017_debug --test-type GENERIC`
 - Build the **documentation**: `py .\scripts\cip.py build --build-type documentation`

---

# GitHub and Jira {#next_windows_github_jira}
Here are some **informations** how to use GitHub and Jira:
 - Template for creating a branch based on the JIRA ID: **feature/SIMEN-XXX/issue_description**
 - https://confluence.auto.continental.cloud/display/SIMEN/HowTo+contribute+into+the+NEXT+project
 - https://confluence.auto.continental.cloud/display/SIMEN/NEXT+-+Run+Integrationtest+in+Jenkins+Pipeline

---

# Other useful tools {#next_windows_other_tools}
Some other tools that are **not mandatory** but recommended to have:
 - VMware Horizon client
 - BinViewer\n 
 - Dependency Walker\n 

---

# Tips and known problems {#next_windows_tips}
 - eCAL configuration can be done through a file named **ecal.ini**. Next uses its own ecal.ini, it is placed in the current directory from where the executables (components or tests) are run
 - **eCAL.ini is located** with the following priority: current directory of the executable, default ecal.ini after installation (C:\ProgramData\eCAL\ecal.ini), default configuration if no ecal.ini is found
 - When **creating a Pull Request**, set it as a Draft PR at first. You must run the manual testing, copy the screenshots with the results for the tests as a comment and then set it as Ready for Review. Besides the description, the message used when committing into the branch should also contain a blank line and then "Related: SIMEN-XXX"

---
