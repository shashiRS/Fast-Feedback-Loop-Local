SCP test case {#manual_test_scp}
=====

[TOC]

---

# Overview {#manual_test_scp_overview}

SCP test case is used to ensure the proper functionality of Next (ex. CAN). 

---

# References {#manual_test_scp_references}

* [Manual Integration Test Cases Main Page ](@ref manual_test_main)
* [Parking and SI Core test case](@ref manual_test_parking_and_si_core)
* [Raw Image test case](@ref manual_test_raw_image)

---

## Prerequisites
Make sure you have installed all the <B>prerequisites</B>:
 - Next Client (GUI)
 - Next repository cloned and built (release is recommended)

---

# Steps {#manual_test_scp_steps}
Steps to perform SCP manual integration test.

##1. Copy test case to your local drive
Go to <B>\\\\cw01.contiwan.com\Root\Loc\ulm2\didt6448\Recordings\sim_team_share\Next Feature Integration Test\TestCases\reworked_tests\ </B>and copy <B>SCP</B> folder to your local drive (ex. C:\TestCases\SCP).

##2. Start Next
Start <B>next_controlbridge.exe</B>, <B>next_databridge.exe</B>, <B>next_player.exe</B>.

##3. Start Next Client (GUI)
Start <B>Next Client (GUI)</B> and select <B>"start new session"</B> for <B>both backend and widgets</B>.<br>

##4. Load configuration
In the GUI go to <B>File -> Import -> GUI & Widget Configuration</B> and select <B>1_GUI_and_Widgets_Config.json</B> from the <B>SCP</B> folder on your disk.<br>

##5. Load the recording
In the GUI go to <B>File -> Load Recording</B> and select <B>2022.06.09_at_08.31.46_radar-mi_5175_ext_09.07.16_DropIn perceptible_SHORT.rrec</B> from the <B>SCP</B> folder on your disk.<br>

##6. Signal Exporter configuration
In the GUI select the <B>Signal Exporter</B> tab and open the config file <B>2_Signal_Exporter_Config.json</B> via the <B>Load</B> button.<br>
In the <B>Export Settings</B> you can change the Output directory if you want.

##7. Start Exporting
In Signal Exporter press <B>Start Export</B> and <span style="color:RED">WAIT!!! until in Log_view "Full subscribe message appears"</span> (this initialization may take a <B>few minutes</B>):
<img src="testcases_scp_wait_exporter.png" width="1250"><br>

##8. Start Playing the recording
Click on <B>play recording</B> button.

##9. Stop Exporting
When playing finished press <B>Stop Exporting</B> in Signal Exporter.

##10. Rename exported file
<B>Rename</B> your exported <B>*.bsig</B> file to <B>exported_file.bsig</B> and <B>copy</B> to your local <B>SCP\bin_cmp\Test_BSIG</B> folder (overwrite the existing one).

##11. Compare with the reference
Double click on <B>bin_cmp.bat</B> from <B>Parking_and_SI_Core\bin_cmp folder</B>.

##12. Comparison results
Open <B>diff.txt</B> from <B>SCP\bin_cmp folder</B>.<br>
You must see the latest log on the bottom with <B>Files match</B> result to confirm that the exported files is identical with the reference:
<img src="testcases_diff_txt_scp.png" width="750"><br>

##13. Visually 3D View chart
Visually check the 3D View chart, you should see how the car moves and detects different objects and  road lanes are shown. At the end it should look like the following:
<img src="testcases_scp_visual_results.png" width="1100"><br>

---

# SCP test results {#manual_test_scp_expectation}
SCP test is considered <span style="color:GREEN">PASSED</span> <B>if you see how the car moves and detects different objects and road lanes are shown. The comparison result is "Files match" from diff.txt .</B> <br>
SCP test considered <span style="color:RED">FAILED</span> <B>if the results differs or if you have related errors during the test.</B>

---

# Tips and known problems {#manual_test_scp_tips}
 - In some cases <B>next_databridge</B> crashes if <B>Signal Exporter</B> wants to export to a folder that doesn't exists. <B>Solution:</B> Change the Output directory to a valid one.
 
---
