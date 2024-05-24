Orchestrator Base Control Data Generation {#control_capabilities_orchestrator_base_ctrl_data}
==============
[TOC]

---

# Base Control Data Definiton

For more elaborate synchronization and triggering schemas projects often freeze metadata to recordings for latter
accurate resimulation.

This metadata often contains timestamps, cycle counter or similar properties to accurately map datablocks into resimulated
components. We call this meta information \named{Base Control Data}\glos{base_ctrl_data}

As not everytime a schedular is present as a simulation component the orchestrator provides a way of generating this matching
the architecture of the project.

This \named{Base Control Data}\glos{base_ctrl_data}

---

# Base Control Data Flow

Each simulation node has one data port for Base Control data. If multiple execution calls need to be acces the
base control data needs to be generated twice for the same publisher

---

# Base Control Data Configuration

Configurations are possible for:

* data description (via .sdl)
* member data connection (via .json)
  * via signals as a subscription (connected signals)
  * via defined value array (fixed values)

Those configurations can either be done generically for all flows or adapted and redefined for single flows.

An example of such configuration:

```json
{
    "next_player": {
        "orchestrator": {
            "flow1": {
                "meta": {
                    "delay": 0,
                    "sync": 1,
                    "control_data_generation": {
                        "sdl_file": "sig_header.sdl",
                        "member_data": {
                            "uiTimestamp": "ADC4xx.EMF.EmfGlobalTimestamp.SigHeader.uiTimeStamp",
                            "uiMeasurementCounter": "ADC4xx.EMF.EmfGlobalTimestamp.SigHeader.uiMeasurementCounter",
                            "sSigState": [
                                0,
                                1,
                                2,
                                3,
                                4,
                                4,
                                6,
                                1
                            ]
                        }
                    }
                },
                "type": "signal_name",
                "value": "ADC4xx.EMF.EmfGlobalTimestamp",
                "clients": {
                    "some_component": {
                        "runSequencer_response": {
                            "member_data": {
                                "uiTimestamp": "ARS5xx.AlgoSenCycle.ACDC_InterCycleDataMeas.sSigHeader.uiTimeStamp",
                                "uiMeasurementCounter": "ARS5xx.AlgoSenCycle.ACDC_InterCycleDataMeas.sSigHeader.uiMeasurementCounter",
                                "uiCycleCounter": "ARS5xx.AlgoSenCycle.ACDC_InterCycleDataMeas.sSigHeader.uiCycleCounter"
                            }
                        },
                        "runSequencer_response2": {}
                    }
                }
            }
        }
    }
}
```

* BaseContolDataGenerator is set up form 3 Modules
* One BaseControlDataGenerator per flow

* one module to read out config
* one moudle to publish data via tree Extractors
* one module to subscribe data and write static data called datapool

@startuml

state BaseControlDataGenerator {
state ControlDataInfo : output Information, data pool

StartUp --> ConfigReader : ControlDataInfo
ConfigReader -> ControlDataInfo
ControlDataInfo --> TreePublisher : Setup
ControlDataInfo --> DataPool : Setup
state ConfigReader
state TreePublisher : create publisher and prepares signal base publishing
state DataPool : create subscription and keeps static data

state SendBaseControlData

SendBaseControlData --> DataPool : request data for client
DataPool --> SendBaseControlData : request data for client

SendBaseControlData --> TreePublisher : update signals and publish

}

[*] -> SendBaseControlData : send data for client and execcall
@enduml

---

# Base Control Config Reader

The purpose of the Base Control Config Reader is to read the .json configuration and to create the ControlDataInfo.
Base Control Config Reader is able to provide ControlDataInfo / provided flow ID available in the configuration.

The Base Control Config Reader is where the fallback mechanism is implemented.
For more on the .json configuration and fallback mechanism, see the tutorial below:  
@ref howto_configure_base_control_data

The ControlDataInfo has the following structure:

```cpp
struct ControlDataInfo {
  std::string flow_name; //flow1
  std::string sdl_file;  //sig_header.sdl
  std::vector<TriggerDataInfo> contol_data_urls;
};
```

TriggerDataInfo contains information about the component, execution and member data.

```cpp
struct TriggerDataInfo {
  std::string component_name;       // sicoregeneric
  std::string execution_name;       // SiCoreGeneric_simpleTrigger
  std::vector<MemberDataInfo> info;
};
```

MemberDataInfo is just a description of the individual member data.

```cpp
struct MemberDataInfo {
  std::string control_data_url_name; // SigHeader.uiTimeStamp
  SignalType type;
  std::string connection_name;          // if fixed, string is empty,
  std::vector<float> fixed_value_array; // if connected, array is empty
};
```

SignalType is an enum, which has to values:
* 0 - signal is of type connected
* 1 - signal is of type fixed values

```cpp
enum class SignalType { connected = 0, fixed_values = 1 };
```

@startuml

->BaseDataReadOutConfig:CreateInfoFromFlowId
activate BaseDataReadOutConfig

BaseDataReadOutConfig -> BaseDataReadOutConfig:CreateFallbackTriggerDataInfoFromFlowId
activate BaseDataReadOutConfig

loop through all available fallback member data
    BaseDataReadOutConfig -> BaseDataReadOutConfig:CreateMemberInfo
    activate BaseDataReadOutConfig
    return
end
return

loop  through all available triggers
    BaseDataReadOutConfig -> BaseDataReadOutConfig:CreateTriggerDataInfo
    activate BaseDataReadOutConfig

    loop through all available member data
        BaseDataReadOutConfig -> BaseDataReadOutConfig:CreateMemberInfo
        activate BaseDataReadOutConfig
        return
    end
    return

    BaseDataReadOutConfig -> BaseDataReadOutConfig:MergeFallbackIntoTriggerDataInfo
    activate BaseDataReadOutConfig
    return
end

return

@enduml

---

# Base Control Data Pool

The Base Control Data Pool is responsible for creating and connecting the tree extrators and providing the the DataPool.
There are two main parts:
* The setup
* Updating the DataPool

## The setup

This part initializes the BaseControlDataPool.
It goes through all the available DataInfo info and tryies to find available tree extractors, if not, it tries to create them.

The main data structure is the DataPoolBackend.
The DataPoolBackend, contains information all fixed values, all connected values and all tree extractors.

```cpp
  struct DataPoolBackend {
    std::vector<FixedValues> fixed_values;
    std::vector<ConnectedValues> connected_values;
    std::unordered_map<std::string, std::shared_ptr<next::udex::struct_extractor::PackageTreeExtractor>>
        tree_extractors;
  };
```

The FixedValue structure contains information about the URL name (an example would be SigHeader.a_reserve),
the actuall array of values and a counter to indicate the index of the current selected value.

```cpp
  struct FixedValues {
    size_t current_value_counter;
    std::vector<float> values;
    std::string control_data_url_name;
  };
```

The ConnectedValues structure cotains the URL name and a shared pointer to an **AnyValue**.

```cpp
  struct ConnectedValues {
    std::shared_ptr<next::udex::AnyValue> value;
    std::string control_data_url_name;
  };
```
@startuml

->BaseControlDataPool:SetupInput 
activate BaseControlDataPool

    BaseControlDataPool->BaseControlDataPool:SearchOrAddTreeExtractorForSignal
    activate BaseControlDataPool

        BaseControlDataPool->BaseControlDataPool:findAnyValueInTreesList
        activate BaseControlDataPool
        return

        BaseControlDataPool->BaseControlDataPool:searchInSignalListOfTrees
        activate BaseControlDataPool
        return

        BaseControlDataPool->BaseControlDataPool:createTreeExtractorAndConnect
        activate BaseControlDataPool
        return

    return

return

@enduml

## Updating the DataPool

When GetNewData is called, the DataPool gets updated and is sent.
The DataPool structure contains information about the fixed signals and connected signals.

```cpp
struct DataPool {
  std::map<std::string, float> fixed_signal;
  std::map<std::string, ConnectedSignal> connected_signal;
};
```

The fixed signals are stored in a map, using the **control URL name as the key**.
Values are converted to type **float**.

When updating values for fixed signals, each value from the array is taken in order. If the last value is reached, but the GetNewData is still being called, the last value will be repeated.
For example, if we have the following array [1, 3, 5, 2, 4].
There are 5 elements in the array, however if the 7 triggers, the values will provided as follows:
1 3 5 2 4 **4 4**

Connected signals are also stored in a map, using the **control URL name as the key**, however they are of type 
ConnectedSignal.

The ConnectedSignal structure contains information about the actual value and te signal type.

When updating connected signals, whatever value is available, for that specific URL, at that moment in time will be used.

```cpp
struct ConnectedSignal {
  next::udex::ValueUnion value;
  next::udex::SignalType signal_type;
};
```

@startuml

->BaseControlDataPool:GetNewData
activate BaseControlDataPool

    BaseControlDataPool->BaseControlDataPool:createHashName
    activate BaseControlDataPool
    return
    
    BaseControlDataPool->BaseControlDataPool:updateResultFixValues
    activate BaseControlDataPool
    return

    BaseControlDataPool->BaseControlDataPool:updateConnectedValues
    activate BaseControlDataPool
    return

return

@enduml

BaseControlDataPool also provides the method IsConnected, that returns **true** if all the subscribers all connected.

```cpp
  bool IsConnected() {
    bool all_connected = true;

    for (auto sub : subscriber_->getSubscriptionsInfo()) {
      all_connected &= subscriber_->IsConnected(sub.second.topic);
    }

    return all_connected;
  }
```

---

# Tree Extractor Publisher

The Tree Extractor Publisher is responsible for creating the tree extractor and the publisher used for generating the Base Control Data.
The **sdl** provided in the **json** configuration is used for publishing.

# The Setup

The .sdl file is parsed and the placeholder **?ViewName?** is replaced with **[flow name]_[component name]**.  
The publisher is created.  
The tree extractor is created.

@startuml

->TreeExtractorPublisher:Setup
activate TreeExtractorPublisher

    TreeExtractorPublisher->TreeExtractorPublisher:ParseSdlToFlow
    activate TreeExtractorPublisher

        TreeExtractorPublisher->TreeExtractorPublisher:replace
        activate TreeExtractorPublisher
        return

    return

    TreeExtractorPublisher->TreeExtractorPublisher:CreatePublisher
    activate TreeExtractorPublisher
    return

    TreeExtractorPublisher->TreeExtractorPublisher:CreateTreeExtractor
    activate TreeExtractorPublisher

        TreeExtractorPublisher->TreeExtractorPublisher:ClearTreePackage
        activate TreeExtractorPublisher
        return

    return

return

@enduml

## SetValue

**SetValue** is used when setting data for signals with **fixed values**.
It will search for the base control URL in the tree extractor, if it's available, it will set the provided value.
Fixed values are of type **float**.

@startuml

->TreeExtractorPublisher:SetValue
activate TreeExtractorPublisher

    TreeExtractorPublisher->TreeExtractorPublisher:findMatchingValueInTreeExtractor
    activate TreeExtractorPublisher

        TreeExtractorPublisher->TreeExtractorPublisher:GetFullSignalName
        activate TreeExtractorPublisher
        return

    return

    TreeExtractorPublisher->TreeExtractorPublisher:SetAnyValueFromTemplate
    activate TreeExtractorPublisher
    return

return

@enduml

## SetValueFromValueUnion

**SetValueFromValueUnion** is used when setting data for **connected signals**.
It will search for the base control URL in the tree extractor, if it's available, it will set the provided value.
Connected signal values are of type **AnyValue**.

@startuml
->TreeExtractorPublisher:SetValueFromValueUnion
activate TreeExtractorPublisher

    TreeExtractorPublisher->TreeExtractorPublisher:findMatchingValueInTreeExtractor
    activate TreeExtractorPublisher

        TreeExtractorPublisher->TreeExtractorPublisher:GetFullSignalName
        activate TreeExtractorPublisher
        return

    return

    TreeExtractorPublisher->TreeExtractorPublisher:SetAnyValueFromValueUnion
    activate TreeExtractorPublisher
    return

return
@enduml

The Tree Extractor Publisher also provides a **Publish** method:

@startuml

->TreeExtractorPublisher:Publish
activate TreeExtractorPublisher

    TreeExtractorPublisher->TreeExtractorPublisher:ClearTreePackage
    activate TreeExtractorPublisher
    return

return

@enduml

**ClearTreePackage** will set to 0 the memory for the tree package pointer.

```cpp
  void ClearTreePackage() {
    if (tree_package_ptr_ == nullptr) {
      return;
    }
    memset(tree_package_ptr_, 0, tree_package_size_);
  }
```

---

# Control Data Generator

The Control Data Generator utilizes the **Base Control Config Reader**, **Base Control Data Pool** and the **Tree Extractor Publisher** in order to generate and send control data.

## Init

The init phase is where the Control Data Generator reads the .json configuration and prepares the the BaseControlDataPool and the TreeExatractorPublisher.

@startuml

->ControlDataGenerator:initControlDataGenerator
activate ControlDataGenerator

    ControlDataGenerator->BaseDataReadOutConfig:CreateInfoFromFlowId
    activate BaseDataReadOutConfig
    return

    ControlDataGenerator->BaseControlDataPool:SetupInput
    activate BaseControlDataPool
    return

    loop for each control data url
        ControlDataGenerator->TreeExtractorPublisher:Setup
        activate TreeExtractorPublisher
        return
    end

return

@enduml

## Generate and send Control Data

When **generateAndSendControlData** is called, the Control Data Generator will request new data from the data pool.
It will use the TreeExtractorPublisher to set values for each individual fixed value and connected value.
Finally it will use the TreeExactorPublisher to publish the previously set values.

@startuml

->ControlDataGenerator:generateAndSendControlData
activate ControlDataGenerator

    ControlDataGenerator->DataPool:GetNewData
    activate DataPool
    return

    loop for each fixed value
        ControlDataGenerator->TreeExtractorPublisher:SetValue
        activate TreeExtractorPublisher
        return
    end

    loop for each connected signal
        ControlDataGenerator->TreeExtractorPublisher:SetValueFromValueUnion
        activate TreeExtractorPublisher
        return
    end

    ControlDataGenerator->TreeExtractorPublisher:Publish
    activate TreeExtractorPublisher
    return
    
return

@enduml

The Control Data Generator also offers the method **isConnected** to see if all available subscribers are connected.

@startuml

->ControlDataGenerator:isConnected
activate ControlDataGenerator

    ControlDataGenerator->DataPool:IsConnected
    activate DataPool
    return

return 

@enduml

---