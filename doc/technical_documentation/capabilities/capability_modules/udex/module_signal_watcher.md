SignalWatcher {#udex_module_signal_watcher}
==============
Module Overview Contents
* @ref SignalWatcher
* @ref SignalTree
___

[TOC]

# SignalWatcher {#SignalWatcher}

## Description
The SignalWatcher module is used to track all the available next publisher topics that are present in the system.
It will keep track only of next publisher topics. Next publishers are identified by the topic type name which shoud start with
<strong>"next:"</strong>

The SignalWacther has a background thread which is responsible for updating its internal topic cache. 
The thread is only active in the following 2 cases:
- when an instance of the SignalWatcher is created the thread is activated and runs 5 times the check for new data
- when the Signalwacther instance receives an UpdateTopicCache event for adding new topics

## Updating the topic cache:

@startuml
  "eCAL::Utils" ->[get all topics from eCAL] "vector<topic_name> topic_name_list"
  "vector<topic_name> topic_name_list" -->[check for new entries] "map<topic_name, TopicInfo> topic_info_cache_"
  "map<topic_name, TopicInfo> topic_info_cache_" -->[check for topics to delete with tick or if t_type old -> update signal_list ] "new topics"
  "new topics" -->[parse schema and extract SignalTree] "ThreadPool"
  "ThreadPool" -->[send SignalTree description] "map<URL,description> topic_description_info_"
  "new topics" -->[add new entries]  "map<URL,description> topic_description_info_"
  "map<URL,description> topic_description_info_" -> "external API (URL based)"
@enduml

## UpdateTopicCache event:

This event is used to signal the SignalWatcher that new topics are available in the system or that topics have been removed from the system.

@startuml
  "UpdateTopicCache" ->[receive event message] "Handle topic event"
  "Handle topic event" -->[check if msg topic size is 0] "Check for new topics"
  "Check for new topics" --> "run background thread for 3 seconds"
  "Handle topic event" -->[check if msg topic size is larger than 0] "Delete the topics"
@enduml

# SignalTree {#SignalTree}

The SignalTree module is used to parse the schema of a next publisher in order to identify the containing signals.

The signal information (schema) will be stored as a boost::ptree for ease of acces to the structure. The SignalTree will 
also create a set contaioning all signals which will be used in the search functionality.

Since the creation of an instance may take some time since the schema of the topic is parsed the SignalWacther uses a boost::asio::thread_pool
to create the SignalTree instances


@startuml
"Parse description" --> "fill signal ptree"
  "Parse description" --> "fill search set"
  "fill signal ptree" -->["is node signal node"] "Save it inside ptree"
  "fill signal ptree" --->["is node struct"] "create struct node and save it in ptree"
   "create struct node and save it in ptree" --> "fill signal ptree"
  "fill search set" --> "Read children of node"
  "Read children of node" ---> "create current URL"
  "create current URL" --->["is node struct node"] "Read children of node"
  "create current URL" --->["is node signal node"] "save data in search set"
@enduml
