# Protocol Documentations

The protocol was design to provide secure way to exchange messages between peers using a central server as a relay to receive messages and broadcast them to the peers. It works above TCP/IP and convert the tcp stream to the concept of messages.

the protocol was designed to provide end-to-end encryption to the messages as well as provide authenticity and integrity of the data.

## How the protocol works

 - First step to create your network, contact with your central server to provide you with your "Network Identification" as well as a "Signing Key".
 - Choose your password and you MUST NOT share it with the server or anyone else, only your network should know this password.
 - In order to send a message, the payload and part of the header will be encrypted, then the message will be signed twice, once with the server's "Signing Key" and the other with your network's "Signing Key".
 - Then the message will be sent to the central server, the server can validate the sign without decrypting the data to assure this message sent from one of your network, then the message will be broadcast to every device connected that listen to that type of messages in your network.
 - Once the message arrive to your peers in the network, the peer can validate your sign with your network signing key, then decrypt the data and use it.

#### What is my network?
 - Simply, your network consists of each device will be connecting through the central server. That includes you as a client, the Arduino or any other hardware follow the protocol and also the servers you want to control.

#### how end-to-end encryption is achieved?
 - Through the encryption by the password which only the peers in each network knows.
 
#### how Authenticity is achieved?
 - As every one have to sign the message using the network key, you can check the sign to assure that the message sent by someone in your network. Also in every message there is timestamp which is signed also so you can know when exactly your peer did send the message.

#### how Integrity is achieved?
 - The sender's sign is used to validate the integrity of the data.

#### why I have to sign the message with the server's key?
 - To secure the server and the networks from flooding and DOS attacks, in order for the server to broadcast the message to certain network, the server needs to validate that the message sent from the network which it claim to be sent from. By signing the message with server's key, the server can validate then broadcast the message.
 
#### What if the server's key got leaked?
 - Any one knows the server's key to your network can generate any message and sign it with the server key, the message will success the validation process in the server but will fail in any if your device so the only attack here is to flood your network with useless messages.

#### What if my network key got leaked?
 - Any one can decrypt your messages but no one can send your network any message as the validation in the server side will fail before broadcasting the message.

#### What if both keys got leaked?
 - This is the worst case, the attacker can decrypt all your messages and can join your network and send/receive any message.

## Protocol documentations
 - You should visit [Protocol Specifications](./protocol-specifications.md) first.
 
 The following in the protocol specifications.

| Size | Data Type | Objective | information |
| ------ | ------ | ------ | ------ |
| 4 Bytes | uint32_t | HMAC with server key | first 4 bytes of HMAC-SHA256(server key, last 32 bytes of header + payload) |
| 4 Bytes | uint32_t | HMAC with network key | first 4 bytes of HMAC-SHA256(network key, last 32 bytes of header + payload) |
| 1 Byte | uint32_t | Payload Length | length of payload only without header |
| 4 Bytes | uint32_t | Network Identifier | identifier of your own network given by central server |
| 3 Bytes | uint8_t | Future Use | 3 bytes to be used later if needed - must be 0xFF |
| 8 Bytes | uint8_t[] | Initial Vector | the seed used to generate iv which used to encrypt the data - use first 16 bytes of SHA256(Initial Vector) as real iv |
| 8 Bytes | uint64_t | Unix Timestamp | the time of generating the message - messages are valid only if arrived within 10 seconds |
| 8 Bytes | char[] | Sender Identifier | the name of device, this should be unique for the network |
| 1 Byte | uint32_t | Message Type | type of the message |
| 7 Bytes | uint8_t | Future Use | 7 bytes to be used later if needed - must be 0xFF |
| ? | uint8_t[] | Payload | the payload will be encrypted with AES-CTR using the network key |
 
 #### comments
 - The first 32 bytes are not encrypted and the last 16 bytes are encrypted with the payload.
 - We have discussed the benefits of two signing above.
 - The max size of payload is 0xFF and the minimum is 0x00.
 - Network Identifier as well as the server key is provided by the central server.
 - Initial Vector is used by the receiver to generate the iv to decrypt the message and the last part of the header as real_iv = SHA256(initial vector in header)
 - Unix Timestamp is used to prevent the server or man-in-the-middle attacks to repeat some messages even if they don't know what inside the messages, so only messages within 10 seconds are considered valid. If there was an attack by the server or man-in-the-middle to repeat the message, the device can easily detect it by saving all the HMAC received in the last 10 seconds and compare it with them.
 - Sender Identifier is the name of the device, the network manager will pick names for devices, also this name is encrypted and should be unique, although easy mechanism can be implemented to detect uniquness in the network - if device receive message it did not send with it's identifier then someone else is using it.
 - Message Type	is 1 Byte used to identify the message, this field is also encrypted.
 - Payload can be [0x00,0xFF] in length and is also encrypted.
 
#### Why parts of the header not encrypted and others are?
 - The first portion of the header must be plain text because it's necessary for the server or for the receiving peer to validate or decrypt the message.
 - The last portion is not necessary so it's encrypted for confidentiality
