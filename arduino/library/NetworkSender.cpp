#include "NetworkHandler.h"

void NetworkSender::hton_handling(struct NetworkMessage *send_message) {
  send_message->message_header.network_id = htonl(send_message->message_header.network_id);     //Network Identifier as uint32_t
  send_message->message_header.unix_timestamp = htonll(send_message->message_header.unix_timestamp);  //unix timestamp as uint64_t
}

void NetworkSender::encrypt_message(struct NetworkMessage *send_message) {
  //create aes iv
  uint8_t aes_vector[16];
  network_info.hash_generator.clear();
  network_info.hash_generator.resetHMAC(network_info.network_key, PROTOCOL_KEY_LENGTH);
  network_info.hash_generator.update(send_message->message_header.aes_vector, 8); //the Initial Vector part of the header
  network_info.hash_generator.finalizeHMAC(network_info.network_key, PROTOCOL_KEY_LENGTH, aes_vector, 16);

  //do the encryption
  uint8_t* ptr = (uint8_t*) send_message;
  network_info.encryption_engine.clear();
  network_info.encryption_engine.setKey((uint8_t *) (network_info.network_key), 16);
  network_info.encryption_engine.setIV(aes_vector, 16);
  network_info.encryption_engine.encrypt(ptr+32,ptr+32,send_message->message_header.payload_length+16);
}

void NetworkSender::sign_message(struct NetworkMessage *send_message) {

  //ptr to the message
  uint8_t* ptr = (uint8_t*) send_message;

  //generate HMAC for server validation.
  network_info.hash_generator.clear();
  network_info.hash_generator.resetHMAC(network_info.server_key, PROTOCOL_KEY_LENGTH);
  network_info.hash_generator.update(ptr+8, PROTOCOl_MESSAGE_HEADER_LENGTH+send_message->message_header.payload_length-8);
  network_info.hash_generator.finalizeHMAC(network_info.server_key, PROTOCOL_KEY_LENGTH, ptr, 4);
  *(uint32_t *)ptr = htonl(*(uint32_t *)ptr);

  //generate HMAC for network validation.
  network_info.hash_generator.clear();
  network_info.hash_generator.resetHMAC(network_info.network_key, PROTOCOL_KEY_LENGTH);
  network_info.hash_generator.update(ptr+8, PROTOCOl_MESSAGE_HEADER_LENGTH+send_message->message_header.payload_length-8);
  network_info.hash_generator.finalizeHMAC(network_info.network_key, PROTOCOL_KEY_LENGTH, ptr+4, 4);
  *(uint32_t *)(ptr+4) = htonl(*(uint32_t *)(ptr+4));
}

boolean NetworkSender::ethernet_send_message(struct NetworkMessage *send_message) {

  //message data
  uint8_t* ptr = (uint8_t*) send_message;
  uint16_t message_length = send_message->message_header.payload_length + PROTOCOl_MESSAGE_HEADER_LENGTH;

  //if the client not connected, return.
  if(!network_info.client.connected()) {
    return false;
  }

  //send the message upto PROTOCOL_SEND_TRIES*message_length times.
  int tries = 0;
  int sent = 0;
  while(sent < message_length && tries < PROTOCOL_SEND_TRIES*message_length){
    sent += network_info.client.write(ptr[sent]);
    tries++;
  }

  //return based on if all the message sent
  if(sent == message_length) {
    return true;
  }
  return false;
}


NetworkSender::NetworkSender(ErrorHandler& _errorHandler, NetworkInfo& _network_info) : 
errorHandler(_errorHandler), network_info(_network_info) {

}

boolean NetworkSender::send(uint8_t _message_type, uint8_t *_payload, uint8_t _payload_length) {

  //check length
  if(_payload_length > PROTOCOl_MAX_PAYLOAD_LENGTH) {
    return false;
  }

  //header + payload
  struct NetworkMessage message_send;
  struct NetworkMessage *ptr = &message_send;

  //generate header
  message_send.message_header.payload_length = _payload_length;
  message_send.message_header.network_id = network_info.network_id;
  message_send.message_header.message_type = _message_type;
  message_send.message_header.unix_timestamp = 0xFFFFFFFFFFFFFFFF;
  memcpy(message_send.message_header.sender_identifier,"ARDUINO",8);
  memset(message_send.message_header.future_use1,0xFF,3);
  memset(message_send.message_header.aes_vector,0xFF,8);
  memset(message_send.message_header.future_use2,0xFF,7);

  //generate payload
  memcpy(message_send.payload, _payload, _payload_length);

  //handle ping messages
  uint8_t message_type_temp = _message_type;

  hton_handling(ptr);
  encrypt_message(ptr);
  sign_message(ptr);
  if(message_type_temp == 0) {
    message_send.message_header.HMAC_for_network = 0xFFFFFFFF;
  }
  return ethernet_send_message(ptr);
}