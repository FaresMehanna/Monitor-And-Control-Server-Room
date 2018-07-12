# Protocol Specifications

## Message Header
This header will be appended before the start of any message.

```
<--------------------------------+ 8 Bytes +---------------------------------->

+-------------------------------------+---------------------------------------+
|                                     |                                       |
|        HMAC with server key         |         HMAC with network key         |
|                                     |                                       |
+------------------+------------------+---------------------------------------+
|                  |                  |                                       |
|  Message Length  |   Message Type   |           Network Identifier          |
|                  |                  |                                       |
+------------------+------------------+---------------------------------------+
|                                                                             |
|                             Sender Identifier                               |
|                                                                             |
+-----------------------------------------------------------------------------+
|                                                                             |
|                      8 Bytes Reserved For Future Use                        |
|                                                                             |
+-----------------------------------------------------------------------------+
|                                                                             |
|                              Initial Vector #1                              |
|                                                                             |
+-----------------------------------------------------------------------------+
|                                                                             |
|                              Initial Vector #2                              |
|                                                                             |
+-----------------------------------------------------------------------------+
|                                                                             |
|                                                                             |
|                                   Payload                                   |
|                                                                             |
|                                                                             |
v                                                                             v

```

| Size | Data Type | Objective | information |
| ------ | ------ | ------ | ------ |
| 4 Bytes | uint32_t | HMAC with server key | first 4 bytes of HMAC-SHA256(server key, last 32 bytes of header + payload) |
| 4 Bytes | uint32_t | HMAC with network key | first 4 bytes of HMAC-SHA256(network key, last 32 bytes of header + payload) |
| 2 Bytes | uint32_t | Payload Length | length of payload only without header |
| 2 Bytes | uint32_t | Message Type | type of the message |
| 4 Bytes | uint32_t | Network Identifier | identifier of your own network given by central server |
| 8 Bytes | char[] | Sender Identifier | the name of device, this should be unique |
| 16 Bytes | uint8_t[] | Initial Vector | the iv used to encrypt the data to be used again to decrypt the data |
| ? | uint8_t[] | Payload | the payload will be encrypted with AES-CTR using the network key |

## Messages Types

| Type | From | To | Message |
| ------ | ------ | ------ | ------ |
| 0x0000 | any device / central server | central server / any device | ping |
| 0x001x | arduino or client | all clients | message |
| 0x002x | arduino | all clients | sensors data |
| 0x003x | controlled server | arduino + clients | server data |
| 0x004x | client | controlled server | server executive order |
| 0x005x | client | arduino | arduino executive order |

### Message Subtypes

#### ping
| Type | Message | Payload length |
| ------ | ------ | ------ |
| 0x0000 | ping | 0 |

#### message
| Type | Message | Payload length |
| ------ | ------ | ------ |
| 0x0010 | information | ? |
| 0x0011 | success | ? |
| 0x0012 | warning | ? |
| 0x0013 | error | ? |

#### sensor data
| Type | Message | Payload length |
| ------ | ------ | ------ |
| 0x0020 | json sensor data | ? |

#### server data
| Type | Message | Payload length |
| ------ | ------ | ------ |
| 0x0030 | json server data | ? |

#### client to server executive order
| Type | Message | Payload length |
| ------ | ------ | ------ |
| 0x0040 | restart | 0 |
| 0x0041 | shutdown | 0 |
| 0x0042 | close miner | 0 |
| 0x0043 | start miner | 0 |
| 0x0044 | restart miner | 0 |

#### client to arduino executive order
| Type | Message | Payload length |
| ------ | ------ | ------ |
| 0x0050 | start server | 8 |
| 0x0051 | restart server | 8 |
| 0x0052 | shutdown server | 8 |

