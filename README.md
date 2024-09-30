# GDNet

An [ENet](http://enet.bespin.org/) low level, non peer oriented abstraction for Godot 3.x

Utilises changes made by @jakobwinkler ( https://github.com/jakobwinkler/gdnet3 ) and 
@SimoneStorai ( https://github.com/SimoneStorai/gdnet3 ).

Older versions:

 - GDNet for Godot 3.2.4: https://github.com/krayon/godot3.module.enet/tree/v3.2.4
 - GDNet for Godot 3.2  : https://github.com/krayon/godot3.module.enet/tree/v3.2
 - GDNet for Godot 3.0  : https://github.com/perdugames/gdnet3
 - GDNet for Godot 2.1  : https://github.com/empyreanx/gdnet

## About

[ENet](http://enet.bespin.org/) is a library that provides a number of features
on top of UDP such as, connection handling, sequencing, reliability, channels,
bandwidth throttling, packet fragmentation and reassembly. GDNet provides a
(mostly) thin wrapper around ENet.

## Installation

Simply drop the code into a `gdnet3` directory in your `godot/modules`
directory and build for the platfom of your choice. GDNet has been verified to
build on Linux (64 bit), MacOS X (32/64 bit), and Windows (32/64 bit
cross-compiled using MinGW).

## Example

```python
extends Node

var client1 = null
var client2 = null
var peer1 = null
var peer2 = null
var server = null

var client1_connected = false
var client2_connected = false

func _init():
    var address = GDNetAddress.new()
    address.set_host("127.0.0.1")
    address.set_port(3000)

    server = GDNetHost.new()
    server.bind(address)

    client1 = GDNetHost.new()
    client1.bind()
    peer1 = client1.host_connect(address)

    client2 = GDNetHost.new()
    client2.bind()
    peer2 = client2.host_connect(address)

func _process(delta):
    if (server.is_event_available()):
        var event = server.get_event()

        if (event.get_event_type() == GDNetEvent.DISCONNECT):
            var peer = server.get_peer(event.get_peer_id())
            var address = peer.get_address();
            print(
                "[SERVER ] Peer ",
                event.get_peer_id(),
                " disconnected from ",
                address.get_host(), ":", address.get_port())

        elif (event.get_event_type() == GDNetEvent.CONNECT):
            var peer = server.get_peer(event.get_peer_id())
            var address = peer.get_address();
            print("[SERVER ] Peer connected from ", address.get_host(), ":", address.get_port())
            peer.send_var(str("Hello from server to peer ", event.get_peer_id()), 0)

        elif (event.get_event_type() == GDNetEvent.RECEIVE):
            print("[SERVER ] ", event.get_var())
            server.broadcast_var("Server broadcast, look what I got: " + event.get_var(), 0)

    # Disconnect client1 once we've connected and all our messages have been sent
    if (client1_connected and client1.get_message_count() == 0):
            client1_connected = false
            peer1.disconnect_later();

    if (client1.is_event_available()):
        var event = client1.get_event()

        if (event.get_event_type() == GDNetEvent.DISCONNECT):
            print("[CLIENT1] Client1 disconnected")

        elif (event.get_event_type() == GDNetEvent.CONNECT):
            print("[CLIENT1] Client1 connected")
            peer1.send_var("Hello from client 1", 0)
            client1_connected = true

        elif (event.get_event_type() == GDNetEvent.RECEIVE):
            print("[CLIENT1] ", event.get_var())

    if (client2.is_event_available()):
        var event = client2.get_event()

        if (event.get_event_type() == GDNetEvent.DISCONNECT):
            print("[CLIENT2] Client2 disconnected")

        elif (event.get_event_type() == GDNetEvent.CONNECT):
            print("[CLIENT2] Client2 connected")
            peer2.send_var("Hello from client 2", 0)

        elif (event.get_event_type() == GDNetEvent.RECEIVE):
            print("[CLIENT2] ", event.get_var())
```

**Sample Output:**
```
[SERVER ] Peer connected from 127.0.0.1:48530
[CLIENT1] Client1 connected
[CLIENT2] Client2 connected
[SERVER ] Peer connected from 127.0.0.1:37899
[CLIENT2] Hello from server to peer 0
[SERVER ] Hello from client 2
[CLIENT1] Client1 disconnected
[SERVER ] Hello from client 1
[CLIENT2] Server broadcast, look what I got: Hello from client 2
[SERVER ] Peer 1 disconnected from 127.0.0.1:37899
[CLIENT2] Server broadcast, look what I got: Hello from client 1
```

## API

#### GDNetAddress

- **set_port(port:Integer)**
- **get_port():Integer**
- **set_host(host:String)**
- **get_host():String**

#### GDNetEvent

- **get_event_type():Integer** - returns one of `GDNetEvent.CONNECT`, `GDNetEvent.DISCONNECT`, or `GDNetEvent.RECEIVE`
- **get_peer_id():Integer** - the peer associated with the event
- **get_channel_id():Integer** - only valid for `GDNetEvent.RECEIVE` events
- **get_packet():PoolByteArray** - only valid for `GDNetEvent.RECEIVE` events
- **get_var():Variant** - only valid for `GDNetEvent.RECEIVE` events
- **get_data():Integer** - only valid for `GDNetEvent.CONNECT` and `GDNetEvent.DISCONNECT` events

#### GDNetHost

- **get_peer(id:Integer):GDNetPeer**
- **set_event_wait(id:Integer)** - sets the duration of time the host thread will wait (block) for events (default: 1 ms)
- **set_max_peers(max:Integer)** - must be called before `bind` (default: 32)
- **set_max_channels(max:Integer)** - must be called before `bind` (default: 1)
- **set_max_bandwidth_in(max:Integer)** - measured in bytes/sec, must be called before `bind` (default: unlimited)
- **set_max_bandwidth_out(max:Integer)** - measured in bytes/sec, must be called before `bind` (default: unlimited)
- **bind(addr:GDNetAddress)** - starts the host (the system determines the interface/port to bind if `addr` is empty)
- **unbind()** - stops the host
- **host_connect(addr:GDNetAddress, data:Integer):GDNetPeer** - attempt to connect to a remote host (data default: 0)
- **broadcast_packet(packet:PoolByteArray, channel_id:Integer, type:Integer)** - type must be one of `GDNetMessage.UNSEQUENCED`, `GDNetMessage.SEQUENCED`, or `GDNetMessage.RELIABLE`
- **broadcast_var(var:Variant, channel_id:Integer, type:Integer)** - type must be one of `GDNetMessage.UNSEQUENCED`, `GDNetMessage.SEQUENCED`, or `GDNetMessage.RELIABLE`
- **is_event_available():Boolean** - returns `true` if there is an event in the queue
- **get_message_count():Integer** - returns the number of messages in the (outbound) queue
- **get_event_count():Integer** - returns the number of events in the queue
- **get_event():GDNetEvent** - return the next event in the queue

#### GDNetPeer

These methods should be called after a successful connection is established, that is, only after a `GDNetEvent.CONNECT` event is consumed.

- **get_peer_id():Integer**
- **get_address():GDNetAddress**
- **ping()** - sends a ping to the remote peer
- **set_ping_interval(pingInterval:Integer)** - Sets the interval at which pings will be sent to a peer
- **get_avg_rtt():Integer** - Average Round Trip Time (RTT). Note, this value is initially 500 ms and will be adjusted by traffic or pings.
- **reset()** - forcefully disconnect a peer (foreign host is not notified)
- **peer_disconnect(data:Integer)** - request a disconnection from a peer (data default: 0)
- **disconnect_later(data:Integer)** - request disconnection after all queued packets have been sent (data default: 0)
  - NOTE: Queued packets here refers to those queued in the ENet library itself, _**NOT**_ in the Godot ENet module. To account for these, you should first check that `get_message_count()` is returning zero before calling `disconnect_later`.
- **disconnect_now(data:Integer)** - forcefully disconnect peer (notification is sent, but not guaranteed to arrive) (data default: 0)
- **send_packet(packet:PoolByteArray, channel_id:int, type:int)** - type must be one of `GDNetMessage.UNSEQUENCED`, `GDNetMessage.SEQUENCED`, or `GDNetMessage.RELIABLE`
- **send_var(var:Variant, channel_id:Integer, type:Integer)** - type must be one of `GDNetMessage.UNSEQUENCED`, `GDNetMessage.SEQUENCED`, or `GDNetMessage.RELIABLE`
- **set_timeout(limit:int, min_timeout:Integer, max_timeout:Integer)**
    - **limit** - A factor that is multiplied with a value that based on the average round trip time to compute the timeout limit.
    - **min_timeout** - Timeout value, in milliseconds, that a reliable packet has to be acknowledged if the variable timeout limit was exceeded before dropping the peer.
    - **max_timeout** - Fixed timeout in milliseconds for which any packet has to be acknowledged before dropping the peer.

## License
Copyright (c) 2015 James McLean  
Licensed under the MIT license.
