Plugins {#module_plugins}
======

[TOC]

# Overview {#plugins_overview}

Plugins provide the possibility to add project-specific functionality to the \named{NEXT} simulation framework and allows for a complete separation of development from other individual plugins and the \named{NEXT Bridges} core.
Plugins are able to communicate with \named{NEXT UDex} for data exchange and to provide data for the \named{GUI} via websocket. 
An overview of the \named{NEXT} simulation framework is depicted in the following picture. 

<img src="howto_overview_next_framework.PNG" width="640">

The \named{DataBridge} provides a Plugin SDK that allows developers to test and deploy project-specific plugins during runtime. 
<br>
Plugins are shared library packages that can be loaded dynamically during runtime.
<br>
A Plugin consists of 
- a shared library file *.dll* containing the plugins functionality
- a static library file *.lib* for implicit loading of the shared library
- a [flatbuffer](@ref flatbuffer) schema file *.fbs* to describe the output data of the plugin

An example use case would be to provide a project-specific object list of a sensor for the 3D View widget in the \named{GUI}.
<br>
Plugins are handled by the PluginManager in the \named{DataBridge} and can be started from the \named{GUI}. 
<br>
Developers are able to create their own plugins and see the data in the \named{GUI} by just following a few steps that are explained in the chapter [Plugin Creation](@ref plugin_creation). 
<br>
Project specific plugins are stored in the next_plugins repository.

# Create your own plugin {#plugin_creation}
## Folder Structure and Needed Files {#plugin_folderstructure}

```
D:\WORKSPACE/next_bridges/src
├───BridgeSDK
│   └───interface/next/bridgesdk
│       ├───plugin_addons           // header files for general purpose utility classes for your plugin
│       └───schema                  // location of the available flatbuffer schema
└───Plugins
    └───core                        //general core plugins
        └───Plugins                 // Each plugin is stored in it´s own folder
            ├───CMakeLists.txt      // CMake configuration for your plugin 
            ├───<your_plugin>.h     // header file for your plugin
            └───<your_plugin>.cpp   // source file for your plugin              
```
For project specific plugins the next_plugins repository is used.

```
D:\WORKSPACE/next_plugins/src/next_bridges
└─── <project_name>                  //project names where the plugins belong to
        └───Plugins                 // Each plugin is stored in it´s own folder
            ├───CMakeLists.txt      // CMake configuration for your plugin 
            ├───<your_plugin>.h     // header file for your plugin
            └───<your_plugin>.cpp   // source file for your plugin              
```

## Mandatory Functions of a Plugin {#plugin_functions}

### init() {#plugin_function_init}

This function will be called when a widget requests the tag of the plugin (see [Step 2] (@ref databridge_plugin_init)). It should be used to initialize all needed variables and setups for the plugin, e.g. general purpose utility classes of the plugin.
<br>
**Hint:** General purpose utility classes of the plugin can be found in the header files of folder [BridgeSDK/interface/next/bridgesdk/plugin_addons](@ref plugin_folderstructure)

### getDescription() {#plugin_function_getDescription}

This function will be called in [Step 2](@ref databridge_plugin_init) when a widget requests the tag of the plugin. It should contain all configuration parameters for the plugin including their default values. To set a configuration parameter, use the function:
<br>
`config_fields.insert({"<config_field_name>", {next::bridge_sdk::ConfigType::CONFIGTYPE, "<config_field_default_value>"}});` 
<br>
The following properties are available for the configuration parameter:

| OPTION            | Explanation |
|-------------------|-----------  |
| EDITABLE          | Text field to be filled by user      |
| FIXED             | Text field with predefined value. no input by user possible |
| SELECTABLE        | Single select item. Possible to add more options in GUI |
| SELECTABLE_FIXED  | Single select item. Not Possible to add more options |
| MULTISELECT       | Multi select items. Possible to add more options in GUI|
| MULTISELECT_FIXED | Multi select items. Not possible to add more options in GUI|

---

**Hint:** To set a list of default values for the options SELECTABLE, SELECTABLE_FIXED, MULTISELECT and MULTISELECT_FIXED call `config_fields.insert` for each value. An example is shown in the following pictures:

```
config_fields.insert({"url", {next::bridge_sdk::ConfigType::SELECTABLE, "ADC4xx.FDP_CSB.m_tpfOutput"}});
config_fields.insert({"url", {next::bridge_sdk::ConfigType::SELECTABLE, "ADC4xx.FDP_Base.p_TpfOutputIf"}});
config_fields.insert({"url", {next::bridge_sdk::ConfigType::SELECTABLE, "SIM VFB.CEM200_FDP_SYS100.m_tpfOutput"}});
```

<img src="plugins_config_properties_visualization.png" width="639">

---

### addConfig() {#plugin_function_addConfig}

This function will be called in [Step 4](@ref databridge_plugin_add) when a configuration has been confirmed in the GUI. This function should receive the plugin configuration set in the widget and make them available for the plugin. 
<br>
A configuration parameter can be extracted by using the function:
<br>
 `GetValueFromPluginConfig(config, "<config_field_name>", <config_field_value>)`
<br>
The *config* parameter of the function contains all configuration parameters of the plugin. The `<config_field_name>` is the name of a configuration parameter and has to match with the `<config_field_name>` that was added by the [getDescription()](@ref plugin_function_getDescription) function. The `<config_field_value>` is the output parameter and will be filled with the value of configuration parameter set in the widget.
<br>
**Hint:**
<br>
One mandatory config field in *config* parameter is the **DATA_VIEW** field. This is set by the widget and contains the value that is written in the data stream configuration field. The following picture shows this field at the 3D View widget.

<img src="howto_3dview_data_stream_configuration.png" width="639">

This value will be used by the GUI to differentiate data from the plugin if multiple widgets are run in parallel.<br>
By default the GUI will add a number to the **DATA_VIEW** value when multiple widgets are opened.

---

**Hint**: All values in `<config_field_value>` will be of data type `std::string`! If you are expecting numbers you need to convert them in your code.

---

### removeConfig() {#plugin_function_removeConfig}

This function will be called in [Step 4](@ref databridge_plugin_add) when a configuration has been changed or removed in the GUI.
This function should handle the closing of the plugin and all internal processes.

---

### updateConfig() {#plugin_function_updateConfig}

This function will be called when a plugin update is requested.

---

### enterReset() {#plugin_function_enterReset}

This function will be called in case a new recording is loaded or a recording is reset to the beginning. 

---

### exitReset() {#plugin_function_exitReset}

This function will be called after the enterReset() function to signal that the reset of the plugin is finished.

---

## Data Subscription to UDex

Plugins can subscribe a callback to a URL from the \named{NEXT UDex} to receive the data from it. To do this, the plugins can use the ```subscribeUrl(const std::string &url_name, bridge_sdk::pluginCallbackT &sub_callback)```. 

Here *url_name* is the URL that should be subscribed to.
<br>
 *sub_callback* is the function that will be executed in the callback.

<div class="note"></div>       
Be aware that the function *sub_callback* needs to be thread-safe. Avoid using member variables and global variables in this function to prevent race conditions.

The data which shall be send to the \named{GUI} from the callback, need to be stored in [Flatbuffers](https://google.github.io/flatbuffers/) before sending it out.
The following chapters describes the flatbuffer structure, how to create a flatbuffer message and sending it out.

### Structure of a Flatbuffer Schema {#plugin_flatbuffer_structure}

To make sure that your data is formated correctly and understood by the \named{GUI} it is recommended to use the existing flatbuffer schemas to store your information (they can be found in the [BridgeSDK/Interface/Schema](@ref plugin_folderstructure) folder).

Flatbuffer schemas define the format of a message through tables with objects, like this:
```
namespace geometry;

struct Vertex (version: "0.0.1") {
      point: [float:2];
}

table FreeGeometry (version: "0.0.1") {
      id:int;
      sensorUid:int;
      vertices:[Vertex];
      height:float;
      color:string;
}

root_type FreeGeometry;
```
In this example the message is defined in the table *FreeGeometry*, which consists of 5 fields, defined through pairs of name and datatype.

`id` and `sensorUid` are defined as **integers**, `height` as **float**, `color` as **string**. `vertices` is defined as a **list of Vertex objects** by the square brackets. *Vertex* itself is a struct defined above the table in the schema. 
<br>
The *root_type* describes the root table of this schema.
More information on writing Flatbuffer schemas can be found on the [official web page](https://google.github.io/flatbuffers/flatbuffers_guide_writing_schema.html).

---

### Creating a Flatbuffer Message {#plugin_create_flatbuffer}

To fill in the data in the table of flatbuffer you need a `flatbuffers::FlatBufferBuilder` object. You can set an initial size of the buffer which will grow automatically if needed (default: 1024). This flatbuffer builder will use functions from the autogenerated header file of the schema. 
To ensure that all fields of the flatbuffer, which are set, are created, call the function *ForceDefaults* from the flatbuffer object with the parameter *true*. Otherwise all fields which are set to default values are omitted. 

---

**Hint**: The header files need to be generated manually by the [flatc flatbuffer schema compiler](https://google.github.io/flatbuffers/flatbuffers_guide_using_schema_compiler.html) each time the schema file changes. (That build step is currently not supported by Bricks). Please be aware that not all version of flatc might be compatible to the flatbuffer schema files. Try to use the flatc version included in the *flatbuffers* package referenced in the build.yml file. Otherwise try another version from [flatbuffer github] (https://github.com/google/flatbuffers) <br>
[TODO: Create and describe process to get a compatible version of flatc]

---

A table from the schema can be serialized and filled in with one command. The command consists of the namespace followed by the function `Create<table_name>` and can be found in the generated header files from the schema. For the example above it would be `geometry::CreateFreeGeometry(<flatbuffer_builder>, id, sensorUid, vertices, height, color)`. The return value of the function is of type `flatbuffers::Offset`, which indicates where the data is stored in the `<flatbuffer_builder>`. You need to store this value in a variable, as this will be the input for finishing the final flatbuffer object.

---

**Hint:** Numeric types can be used directly in the `Create<table_name>` function. Strings or lists need to be created a priori with seperate functions to serialize them. For strings use the function `builder.CreateString(string_var)` and for lists `builder.CreateVector(vector_var)`. The return values can then be used in the `Create<table_name>` function.

---

More information on how to fill in data into a flatbuffer format can be found on the [flatbuffer tutorial](https://google.github.io/flatbuffers/flatbuffers_guide_tutorial.html)

Once all data is stored in `flatbuffers::Offset` variables, the function `builder.Finish(FreeGeometry)` is used to finalize the flatbuffer data for transmission.

The databridge provides an additional class as wrapper to handle the created flatbuffer in a way that can be shared with the \named{GUI}. This class is the `bridge_sdk::plugin::FlatbufferPackage`. To create a FlatbufferPackage containing the flatbuffer data, the following lines can be used:

```
  bridge_sdk::plugin::FlatbufferPackage flatbuffer;
  flatbuffer.fbs_binary = builder.GetBufferPointer();
  flatbuffer.fbs_size = builder.GetSize();
```

---

### Sending the Flatbuffer Message to the GUI {#plugin_send_flatbuffer}

To send the flatbuffer message to the \named{GUI} your plugin needs a `std::shared_ptr<next::bridge_sdk::plugin::PluginDataPublisher>` object.
The PluginDataPublisher has a function 
```
PluginDataPublisher::sendFlatbufferData(const std::string &path_to_fbs,
                                             const FlatbufferPackage &flatbuffer,
                                             const SensorInfoForFbs &sensor_info,
                                             const std::pair<std::string, std::string> &structure_version_pair)
```

The path to the *.fbs file needs to be given so that the file can later be parsed and the message can be properly received. As the *.fbs file needs to be copied to the folder of the plugin to be detected by the \named{GUI}, you can use the path of the plugin and just add the name of the *.fbs file. The path of the plugin can be taken from the plugin constructor function.

The finished flatbuffer is contained in the flatbuffer package class from the \named{DataBridge} to make it compatible with the \named{GUI} (see [Creating a flatbuffer message](@ref plugin_create_flatbuffer)).

The *SensorInfoForFbs* is a struct that can be filled with additional information that should be shared with the \named{GUI}. The data_view will be taken from the DATA_VIEW field in the [addConfig()](@ref plugin_function_addConfig) function.
The remaining fields are optional to be filled.

```
struct SensorInfoForFbs {
  std::string data_view;
  std::string cache_name;
  int sensor_id;
  uint64_t timestamp_microsecond = 0;
};
```

The structure version pair *structure_version_pair* is the combination of the used flatbuffer schema and the version of it. In our example it would be `{"FreeGeometry", "0.0.1"}`.