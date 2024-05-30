Capabilities {#udex_capabilities_main}
==============

# DataSubscriber 

* input buffer queue
* low-latency non-loss input
* parallelized data extraction for enhanced performance
* data driven hooks for efficient response times when new data is available

# DataExtractor

* supports data extraction for multiple formats
  * CAN-data
  * ECU-data
  * Ethernet-data
  * GPS-data
* raw binary-block extraction when using known structures defined at compile time of the receiver and transmitter
* Signal interpretation for fully dynamic type-casting of arbitrary data structures using the signal-name
* synchronization of the input buffer shared with ```DataSubscriber```

# DataPublisher

* Inject data into the simulation network using binary data
* Define the published datastructures using
  * sdl-filenames
  * binary descriptions for
    * dbc
    * sdl 
    * cdl
    * xml (fibex)
  * raw blob description only providing a name for a binary data block
* retrieve current publisher information 
* reset functionality to reset backend data structures

# Signal Explorer

* Search for available signals
* retrieve information about the children of a signal
* provide Signal description for a specific URL

