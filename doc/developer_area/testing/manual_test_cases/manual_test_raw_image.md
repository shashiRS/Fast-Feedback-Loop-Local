Raw Image test case {#manual_test_raw_image}
=====

[TOC]

---

# Overview {#manual_test_raw_image_overview}

Raw Image test case is used to ensure the proper functionality of <B>video player</B>, <B>playback of recordings</B> and support of <B>.zrec format</B>.

---

# References {#manual_test_raw_image_references}

* [Manual Integration Test Cases Main Page ](@ref manual_test_main)
* [Parking and SI Core test case](@ref manual_test_parking_and_si_core)
* [SCP test case](@ref manual_test_scp)

---

## Prerequisites
Make sure you have installed all the <B>prerequisites</B>:
 - Next Client (GUI)
 - Next repository cloned and built (release is recommended)

---

# Steps {#manual_test_raw_image_steps}
Steps to perform Raw Image manual integration test.

##1. Copy test case to your local drive
Go to <B>\\\\cw01.contiwan.com\Root\Loc\ulm2\didt6448\Recordings\sim_team_share\Next Feature Integration Test\TestCases\reworked_tests\ </B>and copy <B>Raw_Image_Test</B> folder to your local drive (ex. C:\TestCases\Raw_Image_Test).

##2. Start Next
Start <B>next_controlbridge.exe</B>, <B>next_databridge.exe</B>, <B>next_player.exe</B>.

##3. Start Next Client (GUI)
Start <B>Next Client (GUI)</B> and select <B>"start new session"</B> for <B>both backend and widgets</B>.<br>

##4. Load configuration
In the GUI go to <B>File -> Import -> GUI & Widget Configuration</B> and select <B>1_GUI_and_Widgets_Config.json</B> from the <B>Raw_Image_Test</B> folder on your disk.<br>

##5. Load the recording
In the GUI go to <B>File -> Load Recording</B> and select <B>adc5xx_ref_cam_9_22.05.17_at_11.23.49_svu-mi5_128_SHORT.zrec</B> from the <B>Raw_Image_Test</B> folder on your disk.<br>

##6. Play recording
Click on <B>play recording</B> button.

##7. Expected results
All video frames should be visible in both widgets.<br>

<B>First Video Player tab</B>: <br>
<img src="manual_test_raw_image_results_tab1.png" width="1200"><br>

<B>Second Video player tab</B>: <br>
<img src="manual_test_raw_image_results_tab2.png" width="1200"><br>

---

## Raw Image test results
Raw Image test is considered <span style="color:GREEN">PASSED</span> <B>if you see the same images as specified in step "7. Expected results".</B> <br>
Raw Image test considered <span style="color:RED">FAILED</span> <B>if the results differs or if you are not able to load the recording (.zrec).</B>

---

# Tips and known problems {#manual_test_raw_image_tips}

---
