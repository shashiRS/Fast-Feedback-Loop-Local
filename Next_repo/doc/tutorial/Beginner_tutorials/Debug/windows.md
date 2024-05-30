How to debug Next applications and tests on Windows. {#debug_windows}
=====

[TOC]

---

# Overview {#debug_windows_overview}

This tutorial will show the needed steps to debug **Next applications** and the **tests** under **Visual Studio** on **Windows 10 or higher**.

Two ways are explained. First setting the application, which shall be debugged, as start up project. Second attaching to an already running process. Second one is usefull if you already have everything running.

---

# References {#debug_windows_references}

* [How to debug on Linux ](@ref debug_linux)
* [How to use eCal](@ref howto_use_ecal)

---

# Steps {#debug_windows_steps}
Step by step guide to debug one application or a test.

## Prepare the environment
Make sure you have installed all the **prerequisites**:
- Visual Studio 2017 or higher
- Next Client (GUI)
- Next repository cloned and built for debug

\remark The debug build of the variant `generic` is a debug build, but with optimization turned on. It is not meant to be used for debugging the Next executables, libraries and tests. Either disable the optimizations by editing the *conf/build.yml*  (setting the option `OPTIMIZEDDEBUG` to `false`) and rerun the bricks build. Or build the libraries and executables using the variant `normal_debug` (option `--variant normal_debug` for the build command).

## Build the project

For building the libraries and executables run
```
python .\scripts\cip.py build --variant normal_debug --platform vs2017_debug
```
For building and running the tests run
```
python .\scripts\cip.py build --platform vs2017_debug --test-type GENERIC
```
In second case the variant `normal_debug` does not have to be given explicitly. It is the only variant having tests for the debug platform.

Depending on whether the applications or the tests shall be debugged, the directory <b>next\conan_workarea\build.next.normal_debug.0.0.0-fallback.vs2017_debug</b> (applications and libraries) or the directory <b>next\conan_workarea\build.next.normal_debug.btest.GENERIC.0.0.0-fallback.vs2017_debug</b> (tests) has to be used later. The directory names are not given again. It is simply referred as **build folder**.

## A) Start debugging out of Visual Studio

### 1. Open Visual Studio solution
Visual Studio solution needs to be opened from your local build folder. Double click on the **vs_sln.bat**:

<img src="unittest_solution_windows.png" width="600" height="200"><br>

### 2. Set as Startup project
In <B>Solution explorer</B> right click on the unit test project, where you want to debug and select <B>"Set as StartUp Project"</B>:

<img src="unittest_startupproject_windows.png" width="500"  height="500">

\remark If a plugin shall be debugged, then the databridge shall be selected as StartUp Project. No need to select the plugin itself and configure something special. During runtime the plugin is loaded and such it can be debugged. Same for plugins of the player, but here the player has to be selected as StartUp Project.

### 3. Place breakpoint and start debugging
Pleace a breakpoint in the code if you want, so the execution will be stopped when it reaches. 
To start the debugging press the <B>"Local Windows Debugger"</B> button:

<img src="unittest_start_debug_windows.png" width="1000"><br>

## B) Start applications and attach to running process

### 1. Start all three applications individually
To start environment, in your locally built (Debug) Next repository go to the subfolder *bin* of the build folder.
There are three executables that needs to be started individually: <B>next_controlbridge.exe, next_databridge.exe, next_player.exe</B>.<br>
Start all three of them <B>one by one</B> with <B>double click</B> or right click and Execute (order doesn't matter):

<img src="three_executables.png" width="600"><br>

The <B>Command Prompts can be minimized</B> to taskbar:<br>

<img src="dbg_windows_minimize_cmds.png" width="700"><br>

Please, <B>avoid clicking in the Command Prompts</B> because that will <B>pause the execution</B>. If accidentally click was made in the Command Prompt just press Enter to continue the execution.

### 2. Start Next Client (GUI)
After the three Main executables were started, next step is to <B>launch the Next Client</B> (GUI):
<img src="next_client_shortcut.png" width="100" height="100"><br>
Next Client (GUI) will ask for a configuration,  for this example you can select <B>Start New Session</B>.
The most important is after Next Client (GUI) window opens, check the three main <B>components connection status:</B>

<img src="components_connected_to_GUI.png" width="750"><br>

If the three applications are connected successfully to Next Client (GUI), then <B>green and blue colours</B> will appear in the <B>Connections</B> tab. 
But, you can hover over the tab, with the mouse to check the status description.

### 3. Open Visual Studio solution
Visual Studio solution needs to be opened from your local build folder. Double click on the <B>vs_sln.bat</B> executable:
<img src="solution_bat.png" width="600" height="200"><><br>

### 4. Attach to process
After the solution was opened, in you Visual Studio headline click <B>Debug</B>, and select <B>"Attach to process..."</B>: 
<img src="vs_attach_to_process.png" width="800" height="700"><br>
Attach to process window will pop up and here in the <B>Available processes</B> search box write <B>next</B> keyword to find the three applications.<BR>
In this example select <B>next_player.exe</B> and press <B>Attach</B> button. 
In this case Debug will be made on the players process and the code behind, but the same can be done for the other executables.

### 5. Breapkoint example
To set a breakpoint for a specific line of code, you just need to left click on the left side of the line's number:
<img src="breakpoint_player_example.png" width="1100"><br>
In this example when the <B>next_player.exe</B> process reaches <B>line 28</B> the execution will pause. 
This is a good example because this part of code is executed periodically and the breakpoint will be hit. 
This is just a simple example, other breakpoints can be placed also, it depends on the specific task. 
Usually some trigger should be done on GUI side to execute a piece of code, for example clicking on buttom to execute the code behind.

---
 
# Tips and known problems {#debug_windows_tips}
 - In some cases Visual Studio may be needed to be started in Administrator mode.
 - Clicking in the Command Prompts window will pause the execution. If accidentally click was made in the Command Prompt just press Enter to continue the execution. This is called QuickEdit mode and can be turned off, right click the icon in the upper-left corner of the Command Prompt window->Properties->Options->QuickEditMode checkbox.

---
