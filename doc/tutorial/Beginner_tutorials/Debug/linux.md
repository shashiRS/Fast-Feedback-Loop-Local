How to debug Next applications and tests on Linux. {#debug_linux}
=====

[TOC]

---

# Overview {#next_debug_linux_overview}

This tutorial will show the needed steps to debug **Next applications** and the **tests** under **Visual Studio Code** on **Ubuntu 20.04**.

\remark This tutorial can also be used for Ubuntu 18.04. In this case in the .vscode/tasks.json the platform *ubuntu_20_04_x86_64* must be replaced by *linux64_x86_gcc_7-v2*. Same for *ubuntu_20_04_x86_64-release*, which must be replaced by *linux64_x86_gcc_7-v2-release*. A simple search and replace will do.

---

# References {#next_debug_linux_references}

* [How to debug on Windows](@ref debug_windows)
* [How to use eCal](@ref howto_use_ecal)

---

# Steps {#next_debug_linux_windows}
Step by step guide to debug Next applications or tests in Visual Studio Code.

## 1. Prepare the environment 
Make sure you have installed all the **prerequisites**:
 - Visual Studio Code
 - Next Client (GUI)
 - Next repository cloned and built for tests
 - conan 1.60.1
 - CMake 3.25.0
 - gdb
 
To check the version of conan and CMake run in a terminal `conan --version` or `cmake --version`. If needed you can update conan: `sudo pip3 install --force-reinstall -v conan==1.60.1`.
gdb can be installed via `sudo apt-get install gdb` if missing.

In Visual Studio Code these four extensions are needed (for installing press Ctrl+Shift+X): <B>CMake Tools</B>, <B>C/C++</B>, <B>C/C++ Extension Pack</B>, <B>C/C++ Themes</B>.

<img src="linux_four_extensions.png" width="250">

## 2. Prepare the build environment
Open Visual Studio Code, select File -> Open Folder and choose the cloned **Next** repository folder. 

<img src="open_folder_linux.png" width="300"  height="200"><br>

After that, press <B>Ctrl + Shift + B</B>. If it is the very first run it would be good to run **Sync database** first.

<img src="build_dbg_solution_linux.png">

Press <B>Ctrl + Shift + B</B> and select **Setup local conan for bricks**.

In the next step you have to build the tests (yes, the tests, even if you want to debug one of the applications). Press again <B>Ctrl + Shift + B</B> and select **Build solution (debug)**. Then choose you want to build the tests by choosing **--test-tpye GENERIC**.

<img src="build_tests_linux.png">

You do not have to wait until the build finished. As soon something started to compile you can press <B>Ctrl + C</B> in the Terminal to save time.

Press again <B>Ctrl + Shift + B</B> and this time select **Setup CMAKE environment**. Afterwards select the correct folder. If in the previous step the debug solution was built, then the correct folder would be the one with *normal_debug* in its name, and without the *-release* for the platform (in the picture the first entry).

<img src="linux_cmake_environment.png" width="500">

## 3. Build and debug a specific target

In the bottom of Visual Studio instead of <B>No active kits</B> select <B>GCC 9.4.0</B>. Select <B>Build target</B> also (what component you want to build):

<img src="linux_build_target.png" width="700">

\remark If this options are not shown, open the settings by pressing <B>Ctrl + ,</B>. Search for *cmake.options* and set the *Status Bar Visibility* to *visible*.

\remark Even if the correct kit is already selected, klick again on it to trigger CMake. Otherwise no targets would be selectable.


Press <B>Ctrl+Shift+B</B> and select <B>CMake: build</B> or <B>CMake: clean rebuild</B> to build only the selected target component.

## 4. Place a breakpoint (Optional)
If you want to place a breakpoint, then go to <B>Explorer</B> tab and find the source code (<B>can_package_test.cpp</B> in this case) of the unit test and place a breakpoint or more:

<img src="unittest_breakpoint_linux.png" width="1100"><br>

---

# Tips and known problems {#debug_unittest_linux_tips}
 - From Visual Studio Code the **root Next repository** folder needs to be opened, not the conan folder.
 
---