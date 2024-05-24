Event System {#sdk_module_event_system}
=============

Module Overview Contents
* @ref sdk_module_main
* @ref sdk_module_config
* @ref sdk_module_lifecycle
* @ref sdk_module_initchain
* @ref sdk_module_ecalsingleton
* @ref sdk_module_tracy

___

# Overview

The eventsystem is a small framework which helps to create events which can be subscribed to or published.
The interface is based on C++ template inheritance.
If blocking events are needed inherit from next::sdk::events::BaseEventBinaryBlocking otherwise from next::sdk::events::BaseEventBinary

# Usage

first create your own message type, e.g. a struct
```cpp
struct MyMessage {
  int counter;
};
```
then create your event class by deriving from the right event type, in this example we will use non blocking events. And implement the serialize and deserialize methods according to your own message.

```cppp
class MyEvent : public next::sdk::events::BaseEventBinary<MyMessage> {
  public:
  next::sdk::events::payload_type serializeEventPayload(const MyMessage &message) const {
    // serialize here
    return ;
  }
  MyMessage deserializeEventPayload(next::sdk::events::payload_type &strbuf) const {
    MyMessage message{};
    // deserialize here
    return message;
  }
}
```
Now you can create an instance of your new event, register hooks for received messages, publish and/or subscribe your events.

\note
**the eventsystem is not initializing eCal, this must be done before the eventsystem can be used.**

# Share

The events can be shared via (static or dynamic) libs or source code. If one app is subscribed to an event, another can simply publish the event and all subscribed apps will automatically receive the message.

# Frontend

Currently two types of events are supported. Blocking and non blocking eCal based events.
The architecture is based on C++ template classes

@startuml

class BaseEventTemplate<PayloadType, UserType> {
-- constructor --
BaseEventTemplate(std::string event_name, std::string event_class)
-- virtual methods ---
{abstract} void subscribe() = 0
{abstract} void publish(const UserType&) = 0
{abstract} PayloadType serializeEventPayload(const UserType &) = 0
{abstract} UserType deserializeEventPayload(PayloadType &) = 0
-- implemented --
 UserType getEventData()
-- hooks --
{abstract} void addEventHook(const notify_hook& hook)
{abstract} void addSyncEventHook(const notify_sync_hook &hook)
}

class BaseEventBinary<UserType> {
-- constructor --
BaseEventBinary(std::string event_name)
-- implements --
void subscribe()
void publish(const UserType&)
--implemented--
void requestEventUpdate()
-- hooks --
void addOnEventRequestedHook(subscribe_event_callback &hook) 
}

class BaseEventBinaryBlocking<UserType> {
-- constructor --
BaseEventBinaryBlocking(std::string event_name)
-- implements --
void subscribe()
void publish(const UserType&)
}

class VersionInfoPackage {
 std::string component_name
  std::string documentation_link
  std::string component_version
}

class VersionInfo {
-- constructor --
VersionInfo(std::string event_name)
-- implements --
payload_type serializeEventPayload(const VersionInfoPackage &message)
VersionInfoPackage deserializeEventPayload(const payload_type &buf)
}
BaseEventTemplate <|--- BaseEventBinary : PayloadType = next::sdk::events::payload_type, event_class = typeid(UserType).name
BaseEventTemplate <|--- BaseEventBinaryBlocking : PayloadType = next::sdk::events::payload_type, event_class = typeid(UserType).name
BaseEventBinary <|--- VersionInfo : UserType = VersionInfoPackage
VersionInfo ---> VersionInfoPackage
@enduml

The first base class *BaseEventTemplate* is mainly taking care of hook management. Therefor it implements the basic *addHook* methods. 

It provides 2 template parameters:

**PayloadType**: this represents the serialized Usertype message, e.g. a BLOB, a packed protobuf buffer, a simple byte array or a simple string buffer.

**UserType**: the interpreted message. A struct/class that represents the message we want to publish or receive. This will be serialized or deserialized from/to PayloadType.

The constructor takes also 2 parameters:

**event_name**: the name to create subchannels for this event

**event_class**: the topic name of this event

internally the eventsystem uses those two parameters to create the topic name. 

The example from Usage chapter would look like this: `MyEvent event("subchannel1");` which would create the topic name `struct MyMessage_subchannel1`.

### Non blocking Events

This event type is represented by the class *BaseEventBinary*. It uses as
*PayloadType* a simple `next::sdk::events::payload_type`.
The event_class constructor parameter is set to the *UserType* name.

As backend it utilizes the eCal CSubscriber and CPublisher, which means it can have for the same event name multiple subscribers and publishers.
### Blocking Events

This event type is represented by the class *BaseEventBinaryBlocking*. Similar to non blocking events it uses as
*PayloadType* a simple `next::sdk::events::payload_type`and the event_class constructor parameter is set to the
*UserType* name.

But as backend it utilizes the eCal CServer and CClient, which means it can have for the same event name only one subscriber (server) but multiple publishers (clients).

A publish (client) call is blocking and will only return when the subscriber (server) sends a response or an error.

This event type is used by e.g. the orchestrator mechanism in the next_control repository.
## backend
The backend is taking care of the network part in the eventsystem. In the current version eCal is used to send or receive BLOBs (as string buffer) either in a blocking or in a non blocking way. It does not know what is inside the message.

The architecture is based on a singleton class *EventManager* which distributes the needed functionality to implemented subclasses.

@startuml

class EventManager<<Singleton>>
class EventClient
class EventServer
class EventSubscriber
class EventPublisher

EventManager --> "0..*" EventPublisher
EventManager --> "0..*" EventSubscriber
EventManager --> "0..*" EventClient
EventManager --> "0..*" EventServer

@enduml

# Known Issues

- due to name mangling in linux GCC the topic names are different in windows than in linux. 
- not all return values, states, errors are propagated from backend to frontend.
- unit tests with events involved can be problematic, since the backend is a singleton and keeps the state of a previous executed testcase, therefor the *EventManager* class provides a helper method `resetAll()`.


