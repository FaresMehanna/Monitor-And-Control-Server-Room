#ifndef NETWORK_HANDLER_H
#define NETWORK_HANDLER_H

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <SHA256.h>
#include <AES.h>
#include <CTR.h>
#include <SPI.h>
#include <Ethernet.h>
#include <utility/util.h>
#include "ErrorHandler.h"

#define PROTOCOL_SEND_TRIES ((uint16_t)3)
#define PROTOCOL_KEY_LENGTH ((uint16_t)16)
#define PROTOCOl_RECIEVE_POKE_TIME ((uint32_t)12000)
#define PROTOCOl_SEND_POKE_TIME ((uint32_t)5000)
#define PROTOCOl_MESSAGE_HEADER_LENGTH ((uint16_t)48)
#define PROTOCOl_MAX_PAYLOAD_LENGTH ((uint16_t)255)
#define PROTOCOl_SERVER_PORT ((uint32_t)33331)
#define PROTOCOL_MAX_MESSAGE_LENGTH ((uint16_t)PROTOCOl_MAX_PAYLOAD_LENGTH + PROTOCOl_MESSAGE_HEADER_LENGTH)

#define htonll(x) ( ((x)<< 56 & 0xFF00000000000000ULL) | \
                   ((x)<<  40 & 0x00FF000000000000ULL) | \
                   ((x)<<  24 & 0x0000FF0000000000ULL) | \
                   ((x)<<  8  & 0x000000FF00000000ULL) | \
                   ((x)>>  8  & 0x00000000FF000000ULL) | \
                   ((x)>>  24 & 0x0000000000FF0000ULL) | \
                   ((x)>>  40 & 0x000000000000FF00ULL) | \
                   ((x)>>  56 & 0x00000000000000FFULL) )
#define ntohll(x) htonll(x)


struct NetworkMessageHeader{
  uint32_t HMAC_for_server;
  uint32_t HMAC_for_network;
  uint8_t payload_length;
  uint32_t network_id;
  uint8_t future_use1[3];
  uint8_t aes_vector[8];
  uint64_t unix_timestamp;
  char sender_identifier[8];
  uint8_t message_type;
  uint8_t future_use2[7];
}__attribute__((packed,aligned(1)));

struct NetworkMessage{
	struct NetworkMessageHeader message_header;
	uint8_t payload[PROTOCOl_MAX_PAYLOAD_LENGTH];
}__attribute__((packed,aligned(1)));

struct Message{
	char sender_identifier[9];
	uint8_t message_type;
	uint8_t payload_length;
	char payload[PROTOCOl_MAX_PAYLOAD_LENGTH+1];
};

class NetworkHandler;
class NetworkInfo;
class NetworkConnection;
class NetworkReceiver;
class NetworkSender;

class NetworkInfo{
public:
	uint32_t network_id;
    //for encryption
    CTR<AES128> encryption_engine;
    //for hashing
    SHA256 hash_generator;
    //connection number, used for synchronization
    uint8_t connection_number;
    //keys
	char server_key[16];
	char network_key[16];
	//ethernet module
	byte mac[6] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
	IPAddress server;
	EthernetClient client;
	//connection state
	boolean connection_state;
	//Timers
	uint32_t last_poke_to_server;
	uint32_t last_poke_from_server;

	//constructor to set the data
	NetworkInfo(uint32_t _network_id, const char *_server_key, const char *_network_key) : server(139,162,224,96){
		network_id = _network_id;
		connection_state = false;
		memcpy(server_key,_server_key,16);
		memcpy(network_key,_network_key,16);
		last_poke_to_server = millis();
		last_poke_from_server = millis();
		connection_number = 0;
	}
};

class NetworkConnection {
private:
    ErrorHandler& errorHandler;
    NetworkInfo& network_info;
    NetworkHandler& network_handler;
	//connection handlers
	void keep_connection();
	void close_and_connect();
public:
    NetworkConnection(ErrorHandler& _errorHandler, NetworkInfo& _network_info, NetworkHandler& _network_handler);
    boolean is_connected(void);
    void keep_connected(void);
};

class NetworkReceiver {
private:
    ErrorHandler& errorHandler;
    NetworkInfo& network_info;
    //buffer to recieve one message.
    struct NetworkMessage received_message;
    boolean is_message_received;
    //connection number, used for synchronization
    uint8_t connection_number;
    //detect current reading.
    uint16_t index;
    //methods to handle message in buffer.
    boolean is_valid_message();	//validate the sign.
    void ntoh_handling();		//do network to host conversion.
    void decrypt_message();		//decrypt the header+payload.
    boolean handle_ping_message();		//handle received ping message.
public:
    NetworkReceiver(ErrorHandler& _errorHandler, NetworkInfo& _network_info);
    void loop(void);
    boolean is_received();
    struct NetworkMessage get_received_message(void);
};

class NetworkSender {
private:
    ErrorHandler& errorHandler;
    NetworkInfo& network_info;
    //methods to handle message to be sent.
    void hton_handling(struct NetworkMessage *_send_message);		//do host to network conversion.
    void encrypt_message(struct NetworkMessage *_send_message);	//encrypt the header+payload.
    void sign_message(struct NetworkMessage *_send_message);		//sign the message with HMAC256
    boolean ethernet_send_message(struct NetworkMessage *_send_message);	//send full message
public:
    NetworkSender(ErrorHandler& _errorHandler, NetworkInfo& _network_info);
    boolean send(uint8_t _message_type, uint8_t *_payload, uint8_t _payload_length);
};

class NetworkHandler {
private:
    ErrorHandler& errorHandler;
    NetworkInfo network_info;
    NetworkConnection network_connection;
    NetworkReceiver network_receiver;
    NetworkSender network_sender;
public:
    NetworkHandler(ErrorHandler& _errorHandler, NetworkInfo& _network_info);
    void setup(void);
    void loop(void);
    boolean send(uint8_t _message_type, uint8_t *_payload, uint8_t _payload_length);
    boolean is_received();
    struct Message receive(void);
};

#endif
