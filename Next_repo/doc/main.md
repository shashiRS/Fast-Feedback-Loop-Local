@mainpage
Enabling simulation from Prototype to Validation {#main_doc}
==========================

[TOC]

<img src="next_logo_light.gif">

==========================

\named{NEXT} is a simulation framework to support a wide range of projects and systems with the goal of simulating virtual or recorded data.

It is separated into two category of features:

- @ref CapabilityModules \glos{capability_module} providing low-level functions
- @ref CapabilitySolution \glos{capability_solution} which provide high level user value for specific use cases

The main focus lies on:

- Flexibility for project specific adaptions
- Multi-platform support for Windows and Linux
- Scalability for cloud computation
- Simple low-level modules for rapid prototyping

To start the NEXT simulation framework you have to download the [GUI](https://github-am.geo.conti.de/ADAS/mts-gui/releases/latest) as well as the latest [NEXT backend](https://github-am.geo.conti.de/ADAS/Next/releases/latest).
Additionally, a BRICKS package called \named{Next} is available which will include all the required binaries.

<img src="Next3DViewGIF.gif" alt="Demo of the Next visualization features" width="800">

==========================

# Questions? Feature Requests? Want to get into contact?

\named{NEXT} provides two communication channels towards the team:

* A [User-Group with Teams](https://teams.microsoft.com/l/team/19%3AQkCvNyEg1bFSE4S5rNXKL_rnvK8FbQQCJ5hshpBfkBU1%40thread.tacv2/conversations?groupId=17bfae8d-b1d9-43fa-9b33-65c6614d0bef&tenantId=8d4b558f-7b2e-40ba-ad1f-e04d79e6265a) for a direct contact via chat
* [Service Desk](https://jira.auto.continental.cloud/plugins/servlet/desk/portal/1) for problem reports or concrete feature requests

[More details](@ref contact_next)

==========================

# Overview {#main_doc_overview}

In this document you can find information about

* [Overview of the Simulation Framework Next](@ref overview_main)
* [How to start Next](@ref howto_main)
* [Setting up Next](@ref workspace_main)
* [Integrating Next into your project](@ref integration_main)
* [Testing with Next](@ref next_testing_main)
* [Version and licenses](@ref licenses_main)
* [Architecture](@ref architecture_main)
* [Release strategy](@ref versioning_main)

A short demonstration [video](https://web.microsoftstream.com/video/88d52252-c7ed-4b28-b197-327836d6e9f4) showing the main capabilities is also available.

# Compatibilty with eco version

| eco version | next version     | known issues              |
|-------------|------------------|---------------------------|
| 10.2.0      | 0.11.0 - current |                           |
| 10.2.0      | 0.12.0 - current | sdl byte alignement issue |
| 10.4.1      | 0.13.0 - current |                           |
