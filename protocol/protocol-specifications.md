# Protocol Specifications

## Message Header
This header will be appended before the start of any message.

```
<--------------------------------+ 8 Bytes +---------------------------------->

+-------------------------------------+---------------------------------------+
|                                     |                                       |   ^
|        HMAC with server key         |         HMAC with network key         |   |
|                                     |                                       |   |
+--------+----------------------------+-------+-------------------------------+   |
|        |                                    |                               |   |
|p_length|         Network Identifier         |3 Bytes Reserved For Future Use|   |
|        |                                    |                               |   |
+--------+------------------------------------+-------------------------------+   | Not Encrypted
|                                                                             |   |
|                               Initial Vector                                |   |
|                                                                             |   |
+-----------------------------------------------------------------------------+   |
|                                                                             |   |
|                               UNIX Timestamp                                |   |
|                                                                             |   v
+-----------------------------------------------------------------------------+
|                                                                             |   ^
|                               Sender Identifier                             |   |
|                                                                             |   |
+-------+---------------------------------------------------------------------+   | Encrypted
|       |                                                                     |   |
|m_type |                   7 Bytes Reserved For Future Use                   |   |
|       |                                                                     |   v
+-------+---------------------------------------------------------------------+
|                                                                             |
|                                   Payload                                   |
|                                                                             |
|                                                                             |

```

| Size | Data Type | Objective | information |
| ------ | ------ | ------ | ------ |
| 4 Bytes | uint32_t | HMAC with server key | first 4 bytes of HMAC-SHA256(server key, last 32 bytes of header + payload) |
| 4 Bytes | uint32_t | HMAC with network key | first 4 bytes of HMAC-SHA256(network key, last 32 bytes of header + payload) |
| 1 Byte | uint32_t | Payload Length | length of payload only without header |
| 4 Bytes | uint32_t | Network Identifier | identifier of your own network given by central server |
| 3 Bytes | uint8_t | Future Use | 3 bytes to be used later if needed - must be 0xFF |
| 8 Bytes | uint8_t[] | Initial Vector | the seed used to generate iv which used to encrypt the data - use first 16 bytes of sha256(Initial Vector) as real iv |
| 8 Bytes | uint64_t | Unix Timestamp | the time of generating the message - messages are valid only if arrived within 10 seconds |
| 8 Bytes | char[] | Sender Identifier | the name of device, this should be unique for the network |
| 1 Byte | uint32_t | Message Type | type of the message |
| 7 Bytes | uint8_t | Future Use | 7 bytes to be used later if needed - must be 0xFF |
| ? | uint8_t[] | Payload | the payload will be encrypted with AES-CTR using the network key |

## Messages Types

| Type | From | To | Message |
| ------ | ------ | ------ | ------ |
| 0x00 | any device / central server | central server / any device | ping |
| 0x1x | arduino or client | all clients | message |
| 0x2x | arduino | all clients | sensors data |
| 0x3x | controlled server | arduino + clients | server data |
| 0x4x | client | controlled server | server executive order |
| 0x5x | client | arduino | arduino executive order |

### Message Subtypes

#### ping
| Type | Message | Payload length |
| ------ | ------ | ------ |
| 0x00 | ping | 0 |

#### message
| Type | Message | Payload length |
| ------ | ------ | ------ |
| 0x10 | information | ? |
| 0x11 | success | ? |
| 0x12 | warning | ? |
| 0x13 | error | ? |

#### sensor data
| Type | Message | Payload length |
| ------ | ------ | ------ |
| 0x20 | json sensor data | ? |

#### server data
| Type | Message | Payload length |
| ------ | ------ | ------ |
| 0x30 | json server data | ? |

#### client to server executive order
| Type | Message | Payload length |
| ------ | ------ | ------ |
| 0x40 | restart | 0 |
| 0x41 | shutdown | 0 |
| 0x42 | close miner | 0 |
| 0x43 | start miner | 0 |
| 0x44 | restart miner | 0 |

#### client to arduino executive order
| Type | Message | Payload length |
| ------ | ------ | ------ |
| 0x50 | start server | 8 |
| 0x51 | restart server | 8 |
| 0x52 | shutdown server | 8 |

