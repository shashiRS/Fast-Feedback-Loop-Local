How to integrate NEXT into my cobolt node {#howto_integrate_into_cobolt}
=====

[TOC]

---

# Overview {#howto_integrate_into_cobolt_overview}

This tutorial will present how to integrate Next into cobolt node.<br>
It is recommended to **check the main page first**, to get a **high level overview** of this tutorial: @ref howto_algo_node_main .<br>
Here are some **outdated articles about Cobolt**, but some details might be useful(recommended for **advanced users**): [Cobolt Initial Setup](https://confluence.auto.continental.cloud/display/SIMEN/Cobolt+Initial+Setup), [Next - Project Integration](https://confluence.auto.continental.cloud/display/SIMEN/Next+-+Project+Integration), [Synchronisation Concept](https://confluence.auto.continental.cloud/pages/viewpage.action?spaceKey=SIMEN&title=Synchronisation+Concept)

---

# References {#howto_integrate_into_cobolt_references}

* @ref howto_algo_node_main
* @ref howto_configure_next
* @ref howto_set_triggers
* @ref howto_export_algo_data

---

# Steps {#howto_integrate_into_cobolt_steps}
For this tutorial we will use **cobolt_test repository** as example: https://github-am.geo.conti.de/ADAS/cobolt_test.

##1. Add packages to build.yml
To integrate Next into a Cobolt Node, three packages are mandatory to be added into build.yml: **eco_package**, **next_package**, **adas_platforms_package**.<br>
Check your build.yml and add these packages if needed. This is an example, **newer versions might be available**:
<table>
<caption id="multi_row">Add this packages to build.yml</caption>
<tr><th>eco_package                         <th>next_package           <th>adas_platforms_package<tr>
<td>eco: &eco_package  <br>
    release: &eco_package_version "10.4.0-dev8"<br>
    variant: generic<br>
<td>next: &next_package     <br>
    release: "0.12.0"<br>
    variant: generic<br>
    platforms:<br>
    -- vs2017<br>
    -- vs2017_debug<br>
<td>adas_platforms: &adas_platforms_package<br>
    release: "6.0.38"<br>
    variant: generic<br>
</table>
To **check if newer versions are available**, visit the repositories: [eco/tags](https://github-am.geo.conti.de/ADAS/eco/tags), [next/tags](https://github-am.geo.conti.de/ADAS/next/tags), [adas_platforms/tags](https://github-am.geo.conti.de/ADAS/adas_platforms/tags).<br>
To inspire from a **working example** you can have a look to this file: [cobolt_test/conf/build.yml](https://github-am.geo.conti.de/ADAS/cobolt_test/blob/master/conf/build.yml)

##2. Add find package to cmake 
Open your **root CMakeLists.txt** file, and **add these queries** if they are not there: <br>
```
find_package(adas_platforms REQUIRED)
find_package(eco REQUIRED)
find_package(next QUIET)
```
To inspire from a **working example** you can have a look into this file:<br>
[cobolt_test/CMakeLists.txt](https://github-am.geo.conti.de/ADAS/cobolt_test/blob/806e29a97b475050072c3a70499b61e7cd0a5d5a/CMakeLists.txt)

##3. Create three PlantUML files
Three <B>.plantuml</B> files needed, they can be generated or created manually:<br>
 -# **types.plantuml** includes from Cobolt primitive types and continues with the definition of a package, also defines signal description, here is an **example**:
[cobolt_test\doc\arch\types\next_types.plantuml](https://github-am.geo.conti.de/ADAS/cobolt_test/blob/806e29a97b475050072c3a70499b61e7cd0a5d5a/doc/arch/types/next_types.plantuml)

 -# **component.plantuml** defines the component and also the input/output ports, here is an **example**:
[cobolt_test\doc\arch\components\next_forward.plantuml](https://github-am.geo.conti.de/ADAS/cobolt_test/blob/806e29a97b475050072c3a70499b61e7cd0a5d5a/doc/arch/components/next_forward.plantuml)

 -# **project.plantuml** defines the instance of the component for a specific project and defines the port in more detail by adding properties and connection details, here is an **example**:
[cobolt_test\doc\arch\project_config\next\sync\next_configuration.plantuml](https://github-am.geo.conti.de/ADAS/cobolt_test/blob/806e29a97b475050072c3a70499b61e7cd0a5d5a/doc/arch/project_config/next/sync/next_configuration.plantuml)<br>

**Info:** In general the **macros for PlantUml** are described here: https://github-am.geo.conti.de/ADAS/eco/blob/master/tools2build/PlantUML/macros/

##4. Add generator functions if not exist
  -# The **convertPlantUmlToCompletePlatform** function runs the conversion from plantuml files to the complete platform. This means the types,
the component interfaces, the glue code and the composition is generated all together for a given platform. It is a kind of convenience function which conbines all other functions.<br>
Open your **CMakeLists.txt** file, and **add this function** if it's not already there: <br>
```
convertPlantUmlToCompletePlatform(
  INPUTPLANTUMLFILES
    ${TEST_TYPES_PLANTUMLS} ${TEST_INTERFACE_PLANTUMLS} ${TEST_PROJECT_PLANTUMLS}
  CODEGENFOLDER
    ${CMAKE_CURRENT_BINARY_DIR}
  PLATFORM
    ecal
  TARGETPREFIX
    next
  LOGFILE
    ${CMAKE_CURRENT_BINARY_DIR}/log/log_next.log
)
```
To inspire from a **working example** you can have a look into this file:[cobolt_test/src/scenarios/next/CMakeLists.txt](https://github-am.geo.conti.de/ADAS/cobolt_test/blob/806e29a97b475050072c3a70499b61e7cd0a5d5a/src/scenarios/next/CMakeLists.txt)

  -# The **compileEcalGlueCode** function supports a compilation of glue code of all simulation Platforms (MTS, eCal, Autosar, Dpuff, Apex, ...).<br>
Open your **CMakeLists.txt** file, and **add this function** if it's not already there: <br>
```
compileEcalGlueCode(
  COMPONENT_NAME NextForward
  GLUECODE_LINK_LIBRARIES next_forward
  TARGET_PREFIX "next"
  GLUECODE_DIRECTORY ${CMAKE_BINARY_DIR}/src/scenarios/next/src/ecal/next_test
)
```
To inspire from a **working example** you can have a look to this file:[cobolt_test/tests/integration/next/CMakeLists.txt](https://github-am.geo.conti.de/ADAS/cobolt_test/blob/806e29a97b475050072c3a70499b61e7cd0a5d5a/tests/integration/next/CMakeLists.txt)

##5. Post build commands
Post build commands are needed to copy the executables, libraries etc. into the package folder.<br>
Open your **CMakeLists.txt** file, and add similar **post build events**, adjusted for your project: <br>
```
#copy the directory content, needed next libraries
add_custom_command(TARGET next_sync_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${next_PACKAGE_PATH}/bin/ ${next_sync_test_FOLDER}
)

# copy the directory content, nodes
add_custom_command(TARGET next_sync_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_BINARY_DIR}/../nextforward/${CMAKE_BUILD_TYPE}/ ${next_sync_test_FOLDER}
)

# copy the directory content, SDLs
add_custom_command(TARGET next_sync_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/src/scenarios/next/src/simulation/next_test/after/ ${next_sync_test_FOLDER}
)
```
To inspire from a **working example** you can have a look to this file:[cobolt_test/tests/integration/next/sync/CMakeLists.txt](https://github-am.geo.conti.de/ADAS/cobolt_test/blob/806e29a97b475050072c3a70499b61e7cd0a5d5a/tests/integration/next/sync/CMakeLists.txt) <br>

##6. Run cip sync
Open <B>powershell/cmd</B> in your repository folder and run the specific sync command for your project, usually: `py .\scripts\cip.py sync`

##7. Run cip build
Open <B>powershell/cmd</B> in your repository folder and run the specific build command for your project, usually: `py .\scripts\cip.py build --platform vs2017`<br>
In this example `py .\scripts\cip.py build --platform vs2017_debug --test-type GENERIC` was used to build cobolt_test repository.

##8. Start up your node
Open <B>powershell/cmd</B> in the folder where your node executable is located, usually <B>conan_workarea\package\bin</B> folder. In this example **next_ecal_swc_nextsync_0.exe** is located in `cobolt_test\conan_workarea\package.cobolt_test.generic.btest.GENERIC.1.0.0-fallback.vs2017_debug\tests\cases\NextSyncTest`.<br><br>
**To start your node**, write the executable name and <B>-n</B> argument after and press enter. For example: <B>next_ecal_swc_nextsync_0.exe -n</B>:
<img src="cobolt_node_started.png"><br>

##9. Open eCal Monitor
Open eCal Monitor to **check your Node**, at this point it should be there **without any connection**:
<img src="node_started_not_configured.png"><br>

## Conclusion
If you **arrived here**:
 - At this moment Next has been <span style="color:GREEN">integrated</span> into your project.
 - Node is able to <span style="color:GREEN">start up</span>.

**To continue this tutorial, next step would be to configure your node**: @ref howto_configure_next

---

# Tips and known problems {#howto_integrate_into_cobolt_tips}
 - This is a general tutorial, it means **needs to be adapted** for every project individually as **file names/paths** etc. differs.
 
---
