# Server Room Monitor And Control

I own and operate a small scale mining farm / server room since December 2017.
Operating the room manually is a pain, so generally the objective of the project is to monitor and control the room and all the servers from website or mobile application.

##### The detailed objectives:

 - Monitor the room 24/7 using several sensors.
 - Take action based on the monitored data.
 - Implement safety measurements and actions based on the data.
 - Control the servers in the room softly by software in each server.
 - Control the servers externally by hardware using Arduino or Raspberry Pi.
 - Allow Monitoring data and control signals to flow between servers and owners and monitoring servers.

## How to achieve these objectives - Software:

 To answer this question we have to know the architecture of the system.
 there are four parts :
   - Servers in the room to be controlled and to provide data.
   - Clients to monitor and control the servers by website or mobile application.
   - Arduino/s to control the servers externally and implement safety measurements.
   - Server/s to manage and route the monitored data and the control signals.

##### It look something like this

```
+---------+        +--------+        +---------+
|         |        |        |        |         |
+---------+        |        |        |         |
|         |        |        |        |         |
+---------+<-------+        +<-------+         |
|         |        |        |        |         |
| Clients |        | Server |        | Arduino |
|         |        |        |        |         |
+---------+------->+        +------->+         |
|         |        |        |        |         |
+---------+        |        |        |         |
|         |        |        |        |         |
+---------+        +-+----+-+        +---------+
                     ^    v
        +------------+----+------------+
        |                              |
        |     Controlled servers       |
        |                              |
        +------------------------------+

```
As we see, I didn't implement peer-to-peer network and preferred server-client model because of simplicity. And also as the servers in the room behind NAT and the clients as well, building peer-to-peer network adds un-necessary complications.
But to guarantee the best level of security even if the server is compromised, the server's only job is to broadcast the messages, the system is end-to-end encryption and compromising the server can only yield to prevention of service, in this case the solution is easy by mirroring the server and use different server in case of compromising.

Also I should note that only controlling signals and data is going to this network, all safety measurements are implemented inside the Arduino.

#####  To answer the original question.

 - the Arduino will gather the data from the sensor periodically.
 - the Arduino will control the servers by Relay connected to the power switch of each server.
 - the Arduino will broadcast the data gathered from the server periodically.
 - the Arduino will listen to any message from server which mean it's order from the client.
 - the server will receive the messages from clients or from Arduino, then validate the message and broadcast the message to the network.
 - the client will receive the messages from the Arduino to display it to the user.
 - the client can make any order which will sent to the server to broadcast to the network. 
 - Design protocol which is end-to-end encrypted and also can be verified by the server to prevent DOS attacks - [Protocol Specifications](protocol/protocol-specifications.md) - - [Protocol Documentations](protocol/protocol-documentations.md).


## How to achieve these objectives - Hardware:
 
 - Using Arduino as the main part of the system with ethernet shield to provide internet connectivity.
 - Monitor the temperature and humidity using DHT22 sensor.
 - Detect Motion by PIR sensor, Flame by flame sensor, Smoke by Qt2 sensor, electricity by hall effect sensor.
 - control start and shut down servers by relay controlled by the Arduino.

##### Hardware parts used :
 
  - 1x Arduino Mega.
  - 1x Ethernet shield V1 W5100.
  - 1x DHT22 sensor for temperature and humidity.
  - 1x PIR sensor for motion detection.
  - 2x flame sensor for flame detection.
  - 1x QT2 sensor for smoke detection.
  - 1x Hall effect sensor for electricity leaks detection.
  - 1x 4-channel relay.