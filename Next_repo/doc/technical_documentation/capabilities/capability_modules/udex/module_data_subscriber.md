Data Subscriber{#udex_module_data_subscriber}
==============

Module Overview Contents
* @ref udex_module_data_extractor
* @ref udex_module_data_publisher
* @ref udex_module_data_subscriber
* @ref udex_module_rawdata
* @ref udex_module_signal_explorer
* @ref udex_module_signals_description
___

[TOC]

Subscribing to data requires two modules. The extractor coupled to the subscriber and the subscriber itself.

To start up a subscription you need the following steps:
* Create a Extractor which will get coupled ```DataExtractor()``` and a Subscriber ```DataSubscriber(string data_subscriber_name)```
* Subscribe your URL ```subscribeToUrl(string url_1,shard_prt<DataExtractor> extractor);```
* Add a hook to get updates using the ```DataSubscriber(string data_subscriber_name)``` ```connectCallback(next::udex::UdexCallbackT cb, std::string signal_url);```

@startuml


(*) --> "eCAL"

partition Subscriber #Orange{
  "eCAL" --> "Subscriber hook"
  "Subscriber hook" ->[eCAL binary data package] "Push data"
}

partition Extractor #Orange {
"Push data" --> "Push to input buffer"
"Push to input buffer" --> ===S1===
===S1=== --> "further processing"
}
@enduml
