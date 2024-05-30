Parking and SI Core test case {#manual_test_parking_and_si_core}
=====

[TOC]

---

# Overview {#manual_test_parking_and_si_core_overview}

The main functionalities tested here: <B>point clouds</B>, <B>parking</B>, <B>GPS</B>, <B>signal exporter</B>, <B>log view</B>.

---

# References {#manual_test_parking_and_si_core_references}

* [Manual Integration Test Cases Main Page ](@ref manual_test_main)
* [Raw Image test case](@ref manual_test_raw_image)
* [SCP test case](@ref manual_test_scp)

---

## Prerequisites
Make sure you have installed all the <B>prerequisites</B>:
 - Next Client (GUI)
 - Next repository cloned and built (release is recommended)

---

## Prepare the SI Core environment
To test that Next is working in the projects we need to integrate it and let the project components run.
To do so follow the steps explained here:

<B>1.</B> <B>Clone si_core</B> from: https://github-am.geo.conti.de/ADAS/si_core <br>

<B>2.</B> Checkout the branch: <B>feature/SIMEN-9089/test_next_ct</B> <br>

<B>3.</B> Replace the <B>next_package version</B> in the <B>build_uses</B> section of <B>si_core/conf/build.yml</B> with your <B>local next release</B> or with the version you want to test (ex. 0.12.0-rc4): <br>
<img src="testcases_si_core_build_yml.png" width="600" height="450"><br>

> **Note**: To do a local release of Next to be used in other projects you need to build it with a specified version argument and then do the actual release. A local release will only contain the variant that was pointed to during the release command <br>
> **Build and local release for a version that doesn't exist (ex. 0.12.123):**<br>
> Build vs2017 platform with version 0.12.123, folder with specified version will be created in conan_workarea:<br>
> `py .\scripts\cip.py build --platform vs2017 --version 0.12.123`<br>
> Localy release version 0.12.123 from the conan_workarea according build folder generated in build step:<br>
> `py .\scripts\cip.py entity release --entity-file conan_workarea/build.next.generic.0.12.123.vs2017/selected_entity.json`<br>
> Last line in console after successful local release:<br>
> `INFO - Entity next.generic/0.12.123@cip/releases[vs2017] inserted into the local database`<br>

<B>4.</B> Open <B>Power shell</B> or command window in the <B>si_core folder</B> and run: <B>py .\scripts\cip.py sync</B> command, after that <B>build</B> the generic variant with: `py .\scripts\cip.py build --platform vs2017`<br>

> **Note**: If no `--variant` argument is specified then all the variants in the build.yml are built. You can save some build time by specifically building `--variant generic`<br>
> Full command: `py .\scripts\cip.py build --platform vs2017 --variant generic`<br>
> By specifying the `--platform` it will only build that specific platform, in this case the Next local release only contains the vs2017 platform. If we build other platforms, the build for the ones not released will fail as the dependency platform is not found.

> **Hint**: Might be that there is an issue on Linux when building the SI Core "Failed to convert PlantUML to xmi"
> Check the @ref common_pitfalls to see the solution for this.

<B>5.</B> Find <B>next_plugins</B> package on <B>Report Overview</B>: https://report-overview.cmo.conti.de/?project_id=next_plugins&tag_name= <br>

<B>6.</B> Choose the <B>latest version</B> and <B>click on it</B> (ex. 0.12.1 in this case): <br>
<img src="testcases_next_plugins_overview.png" width="700" height="400"><br>

<B>7.</B> Click on <B>ART</B> button to access the Artifacts (enter your credentials if necessary): <br>
<img src="testcases_next_plugins_artifactory.png" width="1100" height="400"><br>

<B>8.</B> Click on <B>conan_workarea/</B> folder (link): <br>
<img src="testcases_next_plugins_conan.png" width="1100"><br>

<B>9.</B> Find the package which <B>matches your Operating System</B> and configiguration, then <B>click on it</B> to download: 
<img src="testcases_next_plugins_download.png" width="1200" height="550"><br>

<B>10.</B> After downloading the next_plugins package, <B>unzip</B> the zipped folders and <B>copy the bin/plugins/project folder</B> from next_plugins to your SI_Core package <B>bin/plugins</B> to make the project plugins available. 
<br>

<img src="testcases_next_plugins_copy.png" width="600" height="150"><br>

---

# Steps {#manual_test_parking_and_si_core_steps}
Steps to perform Parking and SI Core test.

##1. Copy test case to your local drive
Go to <B>\\\\cw01.contiwan.com\Root\Loc\ulm2\didt6448\Recordings\sim_team_share\Next Feature Integration Test\TestCases\reworked_tests\ </B>and copy <B>Parking_and_SI_Core</B> folder to your local drive (ex. C:\TestCases\Parking_and_SI_Core).

##1.5 Copy the backend config
Copy <B>1_Backend_Config.json configuration</B> from <B>\reworked_tests\Parking_and_SI_Core</B> (from 1st step) to the si_core package bin folder: <B>si_core\conan_workarea\package.si_core.generic.13.0.0-fallback.vs2017\bin</B>

##2. Start Next Client (GUI)
Start <B>Next Client (GUI)</B> and select <B>"start new session"</B> for <B>both backend and widgets</B>.<br>

## WORKAROUND - Adapt byte alignement
open sicoregeneric.sdl and change the `ByteAlignment` to 1. This workaround is needed until the eco release will be updated.

##3. Load backend configuration
In the GUI go to <B>File -> Import -> Backend Configuration</B> and select <B>1_Backend_Config.json</B> from your disk. (<B>si_core\conan_workarea\package.si_core.generic.13.0.0-fallback.vs2017\bin</B>)<br>

##4. Load GUI & Widgets configuration
In the GUI go to <B>File -> Import -> GUI & Widget Configuration</B> and select <B>2_GUI_and_Widgets_Config.json</B>

##5. Signal Exporter configuration
In the GUI select the <B>Signal Exporter</B> tab and open the config file <B>3_Signal_Exporter_Config.json</B> via the <B>Load</B> button.<br>
In the <B>Export Settings</B> you can change the Output directory if you want.

##6. Start-up the nodes
From Process Manager <B>select all executables (1)</B> and <B>start processes (2)</B> :
<img src="testcases_set_relative_path.png" width="850"><br>

##6.5 Load config
load si_core_config.json config in global config from left side menu

##7. Load recording
In the GUI go to <B>File -> Load Recording</B> and select <B>4_parallel_park_in_succ.rrec</B> from the <B>Parking_and_SI_Core</B> folder on your disk.<br>

##8. Start Exporting
In Signal Exporter press <B>Start Export</B> and <span style="color:RED">WAIT!!! until in Log_view "Full subscribe message appears"</span> (this initialization may take a <B>few minutes</B>):
<img src="testcases_wait_for_exporter.png" width="1200"><br>

> **Note**: The Log widget can be found in the left side widgets menu. !NOT the one in the bottom bar! that one is not functional yet<br>

##9. Start Playing the recording
Click on <B>play recording</B> button.

##10. Stop Exporting
When playing finished press <B>Stop Exporting</B> in Signal Exporter.

##11. Rename exported file
<B>Rename</B> your exported <B>*.bsig</B> file to <B>exported_file.bsig</B> and <B>copy</B> to your local <B>Parking_and_SI_Core\bin_cmp\Test_BSIG</B> folder (overwrite the existing one).

##12. Compare with the reference
Double click on <B>bin_cmp.bat</B> from <B>Parking_and_SI_Core\bin_cmp folder</B>.

##13. Comparison results
Open <B>diff.txt</B> from <B>Parking_and_SI_Core\bin_cmp folder</B>.<br>
You must see the latest log on the bottom with <B>Files match</B> result to confirm that the exported files is identical with the reference:
<img src="testcases_diff_txt_parking.png" width="750"><br>

##14. Visually check 2D Line charts
Visually check the 2D Line charts with this reference, should be similar:
<img src="manual_test_parking_results_2dview1.png" width="1000"><br>
<img src="manual_test_parking_results_2dview2.png" width="1000"><br>

##15. Visually 3D View chart
Visually check the 3D View chart with this reference (should contain an moving ego vehicle and its trajectory, point cloud points, free space around the ego and the parking spot). At the end of the trace it should look like the following:
<img src="manual_test_parking_results_3dview.png" width="1300"><br>

---

# Parking and SI Core test results {#manual_test_parking_and_si_core_expectation}
Parking and SI Core test is considered <span style="color:GREEN">PASSED</span> <B>if the 3D visualization visualize the parking algo while trace is playing, the diagrams are identical and log messages are shown. The comparison result is "Files match" from diff.txt.</B> <br>
Parking and SI Core test considered <span style="color:RED">FAILED</span> <B>if the results differs or if you have related errors during the test.</B>

---

# Tips and known problems {#manual_test_parking_and_si_core_tips}
 - In some cases <B>next_databridge</B> crashes if <B>Signal Exporter</B> wants to export to a folder that doesn't exists: solution is to change the Output directory to a valid one.
 
---
