Contact the NEXT team {#contact_next}
==============

[TOC]

# Teams Chat group

You have questions, struggle with the UI or are not sure where to start at with NEXT.
Feel free to join our [NEXT User-group via Teams](https://teams.microsoft.com/l/team/19%3AQkCvNyEg1bFSE4S5rNXKL_rnvK8FbQQCJ5hshpBfkBU1%40thread.tacv2/conversations?groupId=17bfae8d-b1d9-43fa-9b33-65c6614d0bef&tenantId=8d4b558f-7b2e-40ba-ad1f-e04d79e6265a).

There are multiple channels for your specific problems.

@startuml
hide empty description
(*) --> "Is NEXT crashing?" #lightgrey
if "Crash" then
-left->[yes] use "NEXT Crashes" or create a service desk ticket #lightgreen
else
-right->[no] Questions
if "How to use next"
-left>[new user] check out the "NEXT Shorts" #lightblue
else
-down>[different usecase] checkout the "NEXT How To" #lightblue
else
-right>[questions or struggles] checkout the "NEXT Struggles" #lightblue
endif
else
-down->[no] "Share Experience"
if "Finished Project"
-down->[success] feel free to add it to the "NEXT Success Stories" #orange
else
-left->[request] add your feature request into "NEXT Feature Requests" #orange
endif
endif
@enduml

# Service Desk Ticket System

For crashes or new feature requests it might be best to create a ticket so all the information is well stored.
To enable this a Service Desk for NEXT has been implemented. Feel free to add tickets [here](https://jira.auto.continental.cloud/plugins/servlet/desk/portal/1) as well.
<img src="service_desk_create_ticket.png" width="800">

Process Requirements:

* Tool/Process Area should be set to: **NEXT Simulation Framework (ADAS)**

Example:
<img src="problem_report_example.png" width="800">

If you have an attachment, that is bigger than what Jira allows (100 MB), you can add your attachments into our shared folder:
\\cw01.contiwan.com\Root\Loc\ulm2\didt6448\Recordings\sim_team_share\NEXT\ProblemReportAttachments

Please create a new folder there with a name matching your Jira Ticket ID.
If you don´t have access to the location, you can access the Self Service Page here:
https://rrm001.cw01.contiwan.com/SMTWebNextGen/de/AllSharesFolders/DIDT6448


