Publish Manager Architecture {#player_architecture_publish_manager}
=============

[TOC]

# Player Publish Manager

Package Publishing Execution

When the Player state machine is in the Play state it will publish data packages from the recording. This is done using the PublisherManager class.
When PublishDataPackage is called it will get data from the data BufferQueue (if data is not available at this time it will wait untill there is data or Play is interrupted from the GUI); This data package is passed to the udex service where it will be published using the UDex library.
If the publish is successfull it will update the current statistic.
After the publishing call (successfull or not) it will do the subscriber triggering. If it succedes it will increment the step counter and move to the next data, if this fails an error is reported, the step counter is not incremented and will move to the next data.
@startuml
[*] -> PublisherManager:PublishDataPackage
PublisherManager:PublishDataPackage --> GetDataFromBuffer
GetDataFromBuffer --> udex_service:publishData  : Succeeded
udex_service:publishData --> PublisherManager_TriggerOrchestrator
 
state PublisherManager_TriggerOrchestrator {
  state "udex_service_->getTopicName" as test
  [*] --> test
  test --> TriggerSubscribers : Success
  test --> Done <<exitPoint>>: Fail
  TriggerSubscribers --> Done <<exitPoint>>
}
Done --> ReturnTrue : Success
Done --> ReturnFalse : Fail

@enduml
