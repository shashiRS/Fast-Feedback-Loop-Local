How to use Logs {#howto_use_logs}
=====

[TOC]

---

# Overview {#howto_use_logs_overview}

This tutorial will present where <B>logs</B> cand be found and how to set different <B>log levels</B>. 

---

# References {#howto_use_logs_references}

* @ref howto_use_gui
* @ref howto_use_ecal
* @ref debug_windows

---

# Logging channels {#howto_use_logs_channels}
There are three channels (sinks) where logs are posted: <B>console</B>, <B>log files</B> and <B>router</B>.

##1. Console logs
<B>Console logs</B> can be viewed from the <B>Next Client (GUI)</B> and from the <B>command windows</B> also.
To check the logs from the GUI, on the left side scroll down to <B>Log View</B> tab and click on it. You have different filters available: <B>log level</B> (severity level ex. ERROR only), <B>components</B> (ex. next_player only), <B>timestamp interval</B>, <B>channel</B>. In the example above, both <B>INFO</B> and <B>ERROR</B> messages are <B>included in the filter</B>: <br>
<img src="log_viewer_gui.png"> <br>
In the command window the log messages are displayed with <B>different colours for each log level</B>, error with red, info with green in this case: <br>
<img src="console_logs.png"> <br>

##2. Log files
<B>Log files</B> are <B>generated automatically</B> when Next is used. There are logs generated for each component separately: <B>next_player.log</B>, <B>next_databridge.log</B>, <B>next_controlbridge.log</B> . But also there are <B>combined logs</B> generated (ex. Combined_log), to see the most recent logs you can <B>order</B> the files <B>by Data modified</B>: <br>
<img src="log_files.png"> <br>
In the generated Combined_log, there are <B>log messages from different components</B> in one place, for example from next_databridge and next_player in this case: <br>
<img src="log_files_opened.png">

##3. Router logs
In <B>eCal Monitor</B> it is shown which component is <B>publisher</B>(sending) or <B>subscriber</B>(receiving) to logging topics, for example to <B>LogEventData</B>. 
<img src="logs_ecal.png">

---

# Configure log levels {#howto_use_logs_levels}
Next has <B>five</B> different <B>log levels</B>: <B>debug</B>, <B>info</B>, <B>warn</B>, <B>error</B>, <B>off</B>. <br>
Each component can be configured separately with specific log level for the three channels (sinks): <B>file</B>, <B>console</B>, <B>router</B> .
There are two methods how to change the log level of components:

##1. Run with arguments
Open command window in <B>next\conan_workarea\package.next.generic.0.0.0-fallback.vs2017\bin</B> folder and type: <B>next_controlbridge.exe --lc ERR --lr WARN --lf DEBUG</B> :
<img src="set_log_level_arguments.png"> <br>
In this example <B>next_controlbridge</B> was configured with <B>--lc</B> (log_level_console) set to <B>ERR</B>, <B>--lr</B> (log_level_router) set to <B>WARN</B>, <B>--lf</B> (log_level_file) set to <B>DEBUG</B>. <br>
Same arguments can be set in <B>Process Manager -> Backend configuration -> set relative path to bin folder -> click on next_controlbridge and press Edit button</B>: <br>
<img src="set_log_level_arguments_cfg.png"> <br>

##2. Edit global configuration manually
Other workaround is, that <B>global configuration(.json)</B> can be <B>edited manually</B>. <br>
First go to <B>Global Simulation Configuration</B> -> click on <B>Fetch Configuration</B> -> <B>wait until the configuration diagram is created</B> -> click on <B>Save Configuration</B> button -> <B>Give a name</B> (ex. scretch_cfg.json) and <B>Save it somewhere on your disk</B> (ex. D:\Next_tutorial\): <br>

<img src="global_cfg_usage.png"> <br>
Next step is to <B>open the saved configuration file (.json)</B> from your disk in a text editor (ex. Notepad) and edit the log levels how you want</B> (ex. "ERR" instead of "WARN"): <br>
<img src="set_log_level_manually_json.png"> <br>
Next step is to <B>save the modifications</B> in the file. <br>
Last step is from Next Client (GUI) in Global Simulation Configuration tab press <B>Load Configuration</B> button and select your modified configuration file (scretch_cfg.json). You should be able to <B>see the modified values in the diagram</B> and in the <B>console</B> (ex. next_controlbridge) there should be some similar message: <B>Logger Console level is now set to ERR</B> .

---

# Tips and known problems {#howto_use_logs_tips}


---
