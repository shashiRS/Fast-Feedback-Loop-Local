How to start a node on Linux {#howto_start_node_linux}
=====

[TOC]

---

# Overview

This tutorial will present how to start a node on Linux properly.

---

# Steps
Steps required to start a node.

##1. How to build a node on Linux

For example, if you want to build the NEXT nodes, all you have to do is open a terminal in the main repository folder and
add this commands:
> 'python3 ./scripts/cip.py sync'<br>
> 'python3 ./scripts/cip.py build --platform platform_name --variant variant_name'<br>

Platform name can be: ubuntu_20_04_x86_64-release, ubuntu_20_04_x86_64, linux64_x86_gcc_7-v2-release, linux64_x86_gcc_7-v2;
Variant name can be: generic, normal_debug;

<img src="command_for_next_build.jpg"><br>

<span style="color:RED">If you don't have access to the docker please access this tutorial</span> @ref next_linux <br>

#### If you want to build an external node you just have to include the "NEXT" package build in your repository build.yml folder and the commands remain the same.

##2. How can a node be started
Here we can start a node in 2 ways.

###1. With the console

First you have to go to this path "your_next_repo_path/conan_workarea/package_name/bin".

After that open a terminal in the BIN folder and open for example the player executable(in the console you should write "./next_player").
<img src="start_node_terminal.jpg"><br>

<span style="color:RED">NOTE: !!!!!!!!!!!!!!! if you get an error that says that it can't find some lib file

<img src="LD_library_error.jpg"><br>

you should use this command in the BIN folder: 
> 'export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../lib'</span><br>

<span style="color:RED">To add this command you should open a terminal inside your "bin" folder and write it down!</span>

###2. With the GUI

If you want to start the nodes with the GUI you need to have a "GUI_config_file.json"
After you have this configuration done, you just have to load it into the GUI.
Example: In the <B>GUI go to File -> Import -> GUI & Widget Configuration</B> and select "GUI_config_file.json"

Now you can start the nodes:
From Process Manager <B>select all executables (1)</B> and <B>start processes (2)</B>
<img src="testcases_set_relative_path.png"><br>

###3. How to open an external node

If you want to open an external node the only thing different from the first point wold be to add the "-n" argument after the executable name.

Example: I want to open the si_core node.
<img src="open_si_code_external_node.jpg"><br>