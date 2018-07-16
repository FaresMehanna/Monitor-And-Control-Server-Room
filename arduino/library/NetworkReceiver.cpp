#include "NetworkHandler.h"

NetworkReceiver::NetworkReceiver(ErrorHandler& _errorHandler, NetworkInfo& _network_info) : 
errorHandler(_errorHandler), network_info(_network_info) {
  index = 0;
  connection_number = network_info.connection_number;
}

void NetworkReceiver::ntoh_handling(void) {
  received_message.message_header.HMAC_for_server = ntohl(received_message.message_header.HMAC_for_server);     //Network HMAC server
  received_message.message_header.HMAC_for_network = ntohl(received_message.message_header.HMAC_for_network);     //Network HMAC network
  received_message.message_header.network_id = ntohl(received_message.message_header.network_id);     //Network Identifier as uint32_t
  received_message.message_header.unix_timestamp = ntohll(received_message.message_header.unix_timestamp);  //unix timestamp as uint64_t
}

boolean NetworkReceiver::handle_ping_message() {
  if(received_message.message_header.message_type != 0x00) {
    return false;
  }
  network_info.last_poke_from_server = millis();
  return true;
}

void NetworkReceiver::decrypt_message(void) {
  //create aes iv
  uint8_t aes_vector[16];
  network_info.hash_generator.clear();
  network_info.hash_generator.resetHMAC(network_info.network_key, PROTOCOL_KEY_LENGTH);
  network_info.hash_generator.update(received_message.message_header.aes_vector, 8); //the Initial Vector part of the header
  network_info.hash_generator.finalizeHMAC(network_info.network_key, PROTOCOL_KEY_LENGTH, aes_vector, 16);

  uint8_t *ptr = (uint8_t *)&received_message;

  //do the decryption
  network_info.encryption_engine.clear();
  network_info.encryption_engine.setKey((uint8_t *) (network_info.network_key), 16);
  network_info.encryption_engine.setIV(aes_vector,16);
  network_info.encryption_engine.decrypt(ptr+32,ptr+32,received_message.message_header.payload_length+16);
}

boolean NetworkReceiver::is_valid_message() {
  
  //check full header read
  if(index < PROTOCOl_MESSAGE_HEADER_LENGTH) {
    return false;
  }

  //test for network id
  if(network_info.network_id != received_message.message_header.network_id){
    errorHandler.report("Error in received message, Not my network id.");
    return false;
  }

  //pointer to the received message in buffer
  uint8_t *ptr = (uint8_t *)&received_message;

  //validate the message by using hash values
  char hash_buffer[4];
  char* received_hash = (char*) received_message.message_header.HMAC_for_network;

  //generate HMAC for network validation.
  network_info.hash_generator.clear();
  network_info.hash_generator.resetHMAC(network_info.network_key, PROTOCOL_KEY_LENGTH);
  network_info.hash_generator.update(ptr+8, PROTOCOl_MESSAGE_HEADER_LENGTH+received_message.message_header.payload_length-8);
  network_info.hash_generator.finalizeHMAC(network_info.network_key, PROTOCOL_KEY_LENGTH, hash_buffer, 4);
  *(uint32_t *)hash_buffer = ntohl(*(uint32_t *)hash_buffer);     //Network HMAC server

  //if the message signing is not valid then report error and return false
  if(!memcmp(hash_buffer,received_hash,4) == 0) {
    errorHandler.report("FAILED IN MESSAGE HASH RECEIVED!!");
    return false;
  }

  return true;
}


void NetworkReceiver::loop(void) {

  //if the connections change, reset the buffer
  if(connection_number != network_info.connection_number) {
    connection_number = network_info.connection_number;
    is_message_received = false;
    index = 0;
  }

  //if no data to read then return
  if(0 == network_info.client.available()) {
    return;
  }

  //if there is already message in the buffer, reset to read new one.
  if(is_message_received) {
    is_message_received = false;
    index = 0;
  }

  //read the data in the buffer
  uint8_t *ptr = (uint8_t *)&received_message;
  uint16_t max_value_to_be_read = PROTOCOL_MAX_MESSAGE_LENGTH;

  //reaching here that mean we are in a very new message or in the middle of a message.
  while(network_info.client.available() && index < max_value_to_be_read) {
    ptr[index++] = network_info.client.read();
    
    //if the header was already read, then set the max_value_to_be_read = payload_length+header_length
    if(index >= PROTOCOl_MESSAGE_HEADER_LENGTH) {
      max_value_to_be_read = PROTOCOl_MESSAGE_HEADER_LENGTH + received_message.message_header.payload_length;
    }
  }

  //if not all the message is read, then return and continue next time
  if(index != max_value_to_be_read) {
    return;
  }

  //test the validity of the message by validating the hash.
  if(is_valid_message()) {
    //if all message is read, endianess handling.
    ntoh_handling();
    //if the message is valid then decrypt it and set received_message flag to true.
    decrypt_message();
    
    //handle if the message is ping
    if(handle_ping_message()) {
      is_message_received = false;
      index = 0;
    } else {
      is_message_received = true;
    }
  } else {
    errorHandler.report("NOT VALID MESSAGE");
    is_message_received = false;
    index = 0;
  }
}

boolean NetworkReceiver::is_received() {
  return is_message_received;
}

struct NetworkMessage NetworkReceiver::get_received_message(void) {
  is_message_received = false;
  index = 0;
  return this->received_message;
}