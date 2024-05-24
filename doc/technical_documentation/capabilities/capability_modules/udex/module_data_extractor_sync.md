Synchronization {#module_data_extractor_sync}
==============

* different modes are there on how to sync stuff in the extractor

@startuml

state buffer {
state el_t_1 {
state timestampe1
state mysignal1
state cycleId1 : 3
}
state el_t_2{
state cycleId2 : 6
}

state el_t_3 {
state timestampe3
state cycleId3 : 7

}
state el_t_4 {
state timestampe4
state cycleId4 : 9

}
}

state el_t_1 : mts_timestamp 1
state el_t_2 : mts_timestamp 2
state el_t_3 : mts_timestamp 3
state el_t_4 : mts_timestamp 4

DataPublisher --> buffer

buffer --> DataExtractor : want to get some data (GetExtractedQueueWithTimestamp)
timestampe3 --> DataExtractor : want to get signal (GetValue)

DataExtractor --> SyncAlgo : give me the correct value
buffer --> SyncAlgo : sorts and selects the proper value
SyncAlgo --> el_t_2 : algorithm () + reference value +

state SyncAlgo: latest, timestamp, exact, nearest

SyncAlgo -> method
SyncAlgo -> SyncValue
SyncAlgo -> SyncUrl

state method : exact, closet
state SyncValue : an acutal value to compare agains i.E. 6 (implicit also mts_timestamp = 3)
state SyncUrl : cycleId

el_t_2 --> DataExtractor

@enduml