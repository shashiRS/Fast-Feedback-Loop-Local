WSL: Linux Subsystem {#howto_linux_subsystem}
==============

[TOC]

# Overview

Windows Subsystem for Linux (WSL) is a feature of Microsoft Windows that allows developers to run a Linux environment without the need for a separate virtual machine or dual booting.
It supports as well to run Linux Gui apps. However running the NEXT Gui on Linux doesn't seems to work properly. Therefore it's recommended to run the NEXT GUI on windows and execute only the \named{NEXT} backend on Linux. 

# Install Linux Subsystem

Follow the tutorial from here:
https://github.geo.conti.de/pages/CCIF-Packages/choco-wsl-xde/usage/install/

Check as well this [video](https://github.geo.conti.de/pages/CCIF-Packages/choco-wsl-xde/usage/) about installation and usage of WSL.

Consider *xde-20* stand for ubuntu 20.04 and *xde-22* for ubuntu 22.04 distribution.

See [Tips and known problems](#see-tips-and-known-problems) in case of any issues. 

---

# Using VSCode as IDE in Linux Subsystem

To start using VSCode as your IDE in the Linux Subsystem, follow these steps:

1. **Install VSCode on Windows**: Download and install Visual Studio Code from the official site.

2. **Install Remote Development Extension**: In VSCode, go to the Extensions view (`Ctrl+Shift+X`), search for "Remote Development", and install the extension.

## Connecting to Your Distribution with VSCode

You can connect to your Linux distribution from VSCode using the following methods:

- Press `F1`, then select `WSL: Connect to WSL using Distro` and choose your distribution.

- Alternatively, you can connect directly from the VSCode Remote Explorer.
![WSL: Connect to WSL using Distro](tools_linux_subsystem.png)

## Additional Extensions for C++ Development

For enhanced C++ development experience in VSCode, consider installing these extensions:

- **CMake Tools** (`ms-vscode.cmake-tools`)

- **C/C++** (`ms-vscode.cpptools`)

- **C/C++ Extension Pack** (`ms-vscode.cpptools-extension-pack`)

---

# How to move your distribution to another drive

1. Check your distribution:
   ```powershell
   wsl -l -v
   ```

2. Run your distribution:
   ```powershell
   wsl --distribution <your_distribution>
   ```

3. Check your user, you will need this later, usually your uid:
   ```bash
   whoami
   ```

4. Exit WSL:
   ```powershell
   exit
   ```

5. Shutdown WSL:
   ```powershell
   wsl --shutdown
   ```

6. Create backup:
   ```powershell
   mkdir D:\backup
   ```

7. Export distribution:
   ```powershell
   wsl --export <your_distribution> D:\backup\ubuntu.tar
   ```

8. Unregister distribution:
   ```powershell
   wsl --unregister <your_distribution>
   ```

9. Import distribution:
   ```powershell
   mkdir <your_workspace>
   wsl --import <your_distribution> <your_workspace> D:\backup\ubuntu.tar
   ```

10. Run distribution and set your user:
   ```powershell
   cd $env:USERPROFILE\AppData\Local\Microsoft\WindowsApps
   <your_distribution>.exe config --default-user <user>
   ```

---

# Run NEXT backend together with NEXT-GUI

1. Start Linux distribution, e.g. xde-20
```
wsl --distribution xde-20
```
2. Connect VSCode to WSL: Press F1 and selecting "WSL: Connect to WSL using Distro ..."

3. Setup linux debugging as specified in the (debug tutorials)[(@ref next_debug_linux_overview)
   * **HINT** 
      * Don't build the project with docker by bricks (use **--no-docker** option) because docker use by default root rights and building with VSCode, which is using your user account, can lead to access permission errors. 
      * If needed install gcc compiler, e.g. with `sudo apt install build-essential`

4. Execute \named{NEXT} nodes.  
   * **HINT**
      * All nodes needs to run on the same Linux distribution.
      * In case of missing libraries set LD_LIBRARY_PATH: `LD_LIBRARY_PATH=./../lib ./<next_node>`
      * In case of issues during startup of the nodes, check access rights of the folder where the \named{NEXT} nodes are placed (via `ll`) so that the \named{NEXT} nodes can create log-files. Access rights can be set via `sudo chmod -R 777 <NEXT_BINARY_FOLDER>`

5. Start NEXT-GUI on windows machine. 
   * **Hint:** 
      * All paths like the export folder of NEXT Exporter has to be set according to the Linux distribution where the backend is running. 

6. Load Recording. 
   * NEXT player needs to get the path to the mounted windows folder in the linux distribution (/mnt/<drive>/...) which is not accessible via the file explorer in the NEXT-GUI. Possible solution: In the NEXT-GUI open the "Web socket" widget, add the port 8200 and send the following recording load command to the NEXT player:
```
{"channel": "player","event": "UserRequestedInputOpen","source": "NextGUI","payload": {"fileName": "<PATH TO RECORDING IN LINUX>", "mode": "OLS"}}
```
e.g. 
```
{"channel": "player","event": "UserRequestedInputOpen","source": "NextGUI","payload": {"fileName": "/mnt/e/Workspace/next/Recordings/Parking/ADC5xx_new/4_parallel_park_in_succ.rrec", "mode": "OLS"}}
```

**General hints:**
* A shared folder of windows is mounted in Linux under "/mnt/<drive>" and in Windows under "Linux" (see Windows Explorer) as explained in this [video](https://github.geo.conti.de/pages/CCIF-Packages/choco-wsl-xde/usage/)
* The integration test folder can be mounted with the following commands in linux
   ```
   cd /
   sudo mkdir -p /mnt/cw01
   sudo mount -t drvfs '\\cw01.contiwan.com\Root\Loc\ulm2\didt6448\Recordings\sim_team_share\Next Feature Integration Test\TestCases\reworked_tests' /mnt/cw01
   ```
   * A symbolic link to the folder can be create with the command `ln -s /mnt/cw01 ~/<your_folder>`


# Tips and known problems
## How to enable subsytem feature from Windows
1. **Open PowerShell as administrator**: Right-click the Start button and select "Windows PowerShell (Admin)".

2. **Enable Windows Subsystem for Linux (WSL)**:
   ```powershell
   dism.exe /online /enable-feature /featurename:Microsoft-Windows-Subsystem-Linux /all /norestart
   ``` 

## Issues installing WSL packages

### WSL package can't be installed

![WSL package installation error](wsl_installation_issue.png)

Execute:

```powershell
irm https://artifactory.geo.conti.de/artifactory/vni_pmt_ccif_nuget_l/install.ps1 | iex
``` 

Check as well bricks WSL [installation guide](https://cip-docs.cmo.conti.de/static/docfiles/cip_bricks_user_doc/v4.24.0/getting_started/prepare_machine_WSL.html)

### WSL-XDE package can't be installed

Dependency error during installation of wsl-xde

```
Failures
2024-04-17 14:24:41,676 7076 [ERROR] -  - wsl-xde-22 - Unable to resolve dependency 'chocolatey': Unable to resolve dependencies. 'chocolatey 2.2.2' is not compatible with 'chocolatey.extension 4.2.0 constraint: chocolatey (>= 1.0.0 && < 2.0.0)'.
```

Update the following packages

```
choco upgrade chocolatey-agent
choco upgrade chocolatey.extension
```

### Invalid credentials during package installation

Error message: 

```
Invalid credentials specified.
```

Add artifactory source again as specified as first step in the bricks WSL [installation guide](https://cip-docs.cmo.conti.de/static/docfiles/cip_bricks_user_doc/v4.24.0/getting_started/prepare_machine_WSL.html)



## Network issues

Make sure **wsl-vpnkit** is running:

```powershell
wsl -l -v
```

If the state of wsl-vpnkit is not running, use the following command:

```powershell
wsl -d wsl-vpnkit vpnkit
```

If you still encounter network issues, check your DNS address:

```powershell
nslookup www.google.com
```

Copy the address from the conti server.

Run your distribution:

```powershell
wsl --d <your_distribution>
```

Add your address as a fallback option to **resolv.conf**:

```bash
sudo nano /etc/resolv.conf
```

Press `Ctrl+S`, then `Ctrl+X`.

In case it doesn't save, you might need to remove the immutable attribute:

```bash
sudo chattr -i /etc/resolv.conf
```

