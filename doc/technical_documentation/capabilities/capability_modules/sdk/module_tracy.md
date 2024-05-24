Tracy Profiler {#sdk_module_tracy}
=============

Module Overview Contents
* @ref sdk_module_main
* @ref sdk_module_config
* @ref sdk_module_lifecycle
* @ref sdk_module_initchain
* @ref sdk_module_event_system
* @ref sdk_module_ecalsingleton
___

# General description

[Tracy](https://github.com/wolfpld/tracy) is a real-time frame and sampling profiler with a nanosecond resolution.<br>
The profiler supports a central processing unit (CPU) and graphics processing unit (GPU) profiling and can follow function blocks within the code via tracy markers and visualize it.<br>
Profiling means the analysis of the program to be executed.

# Integration
## Cmake

Enable tracy in the CmakeLists.txt with following command:

```cmake
target_compile_definitions(${TARGET_NAME} 
  PRIVATE
    TRACY_ENABLE
)
```

## Header

Include the profiler header into the source file which shall use the tracy marker

``` c++
#include <next/sdk/profiler/profiler.hpp>
```

## Init

To enable the profiler during start up, include the following lines into the @ref sdk_module_initchain :

```
next::sdk::logger::register_to_init_chain();
```
```
initmap.emplace(next_profiling::getProfileLevelCfgKey(), profiling_level);
```

# Configuration
## Profiling Level

Setting the profiling level can be done in the GUI through the @ref sdk_module_config.
The used key is "'component_name'.profiling.level". The value can be edited in the global simulation configuration tab.<br>

```
component: {
  "profiling": {
    "level": "OFF"
  }
}
```

The profiler supports four levels:
- OFF
- OVERVIEW
- DETAIL
- FULL

The default value for a release-build is OFF, the default value for debug-build is DETAIL.<br>
If an unsupported value is given in the configuration, the current value will be kept and an information is displayed in the console log.

# Tracy Marker

| level | next_sdk | next_udex | next_control | next_bridges | next_player |
|---|---|---|---|---|---|
| overview | ProfileCore_OV<br>ProfileCoreN_OV(name)<br>ProfileCoreNC_OV(name, color) | ProfileUdex_OV<br>ProfileUdexN_OV(name)<br>ProfileUdexNC_OV(name, color) | ProfileControl_OV<br>ProfileControlN_OV(name)<br>ProfileControlNC_OV(name, color) | ProfileBridges_OV<br>ProfileBridgesN_OV(name)<br>ProfileBridgesNC_OV(name, color) | ProfilePlayer_OV<br>ProfilePlayerN_OV(name)<br>ProfilePlayerNC_OV(name, color) |
| detail | ProfileCore_DTL<br>ProfileCoreN_DTL(name)<br>ProfileCoreNC_DTL(name, color) | ProfileUdex_DTL<br>ProfileUdexN_DTL(name)<br>ProfileUdexNC_DTL(name, color) | ProfileControl_DTL<br>ProfileControlN_DTL(name)<br>ProfileControlNC_DTL(name, color) | ProfileBridges_DTL<br>ProfileBridgesN_DTL(name)<br>ProfileBridgesNC_DTL(name, color) | ProfilePlayer_DTL<br>ProfilePlayerN_DTL(name)<br>ProfilePlayerNC_DTL(name, color) |
| full | ProfileCore_FLL<br>ProfileCoreN_FLL(name)<br>ProfileCoreNC_FLL(name, color) | ProfileUdex_FLL<br>ProfileUdexN_FLL(name)<br>ProfileUdexNC_FLL(name, color) | ProfileControl_FLL<br>ProfileControlN_FLL(name)<br>ProfileControlNC_FLL(name, color) | ProfileBridges_FLL<br>ProfileBridgesN_FLL(name)<br>ProfileBridgesNC_FLL(name, color) | ProfilePlayer_FLL<br>ProfilePlayerN_FLL(name)<br>ProfilePlayerNC_FLL(name, color) |

Tracy Marker can be set with three different functions:<br>
The functions without a N or NC should be used in the beginning of the traced function and traces the complete function. The name used in the Tracy GUI is the function name with namespaces.<br>
The function with a N at the end should be used in the function itself, to trace for example a while loop. Here a name must be provided as parameter (e.g.: ProfileBridgesN_FLL("next::udex::DataExtractor::PushData call Push")). This name will be used in the Tracy GUI<br>
The function with a NC at the end can be used similar to the one with only an N. This call includes the Tracy Marker name and a color, provided in the tracy::Color namespace (e.g.: ProfileBridgesNC_FLL("next::udex::DataExtractor::PushData call Push", tracy::Color::Red)).

In the table the colors for the already provided tracy marker can be found:

| next_sdk | next_udex | next_control | next_bridges | next_player |
|:--------:|:---------:|:------------:|:------------:|:-----------:|
| blue     | green     | yellow       | orange       | brown       |

# Examples

``` c++
void function() { 
  ProfileUdex_DTL; // profiling marker for the whole function. Name of the function and default udex color (green) will be displayed in Tracy GUI
  bool job_finished = false; 
  // now start a background job; the creation takes a while and the execution takes even more time
  {
   // profiling marker for the scope, in this case waiting to start the job. 
   // Tracy will use the given name and the default udex color
    ProfileUdexN_OV("function waiting for job to start");
    start_job(&task_finished); 
  } 
  
  { 
    // profiling marker for the scope, with name and color
    ProfileUdexNC_OV("function waiting for job to finish", tracy::Color::Blue); 
    
    // wait until the job finished 
    while(!task_finished) { 
      std::this_thread::sleep_for(std::chrono::milliseconds(50)); 
    }
  }
}
```
