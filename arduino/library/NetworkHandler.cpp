#include "NetworkHandler.h"

NetworkHandler::NetworkHandler(ErrorHandler& _errorHandler, NetworkInfo& _network_info)
 : errorHandler(_errorHandler), network_info(_network_info), network_connection(_errorHandler,_network_info,*this), 
 network_receiver(_errorHandler,_network_info), network_sender(_errorHandler,_network_info) {

}

void NetworkHandler::setup(void) {
  //these steps are required for ethernet shield
  pinMode(53,OUTPUT);
  if (Ethernet.begin(network_info.mac) == 0) {
    errorHandler.report("Failed to configure Ethernet using DHCP");
    while(true);
  }
  delay(1000);
}

void NetworkHandler::loop(void) {
  network_connection.keep_connected();
  network_receiver.loop();
}

boolean NetworkHandler::send(uint8_t _message_type, uint8_t *_payload, uint8_t _payload_length) {
  return network_sender.send(_message_type, _payload, _payload_length);
}

boolean NetworkHandler::is_received() {
  return network_receiver.is_received();
}

struct Message NetworkHandler::receive(void) {
  struct Message to_return;
  memset(&to_return,0,sizeof(struct Message));
  if(!network_receiver.is_received()) {
    return to_return;
  }
  struct NetworkMessage received_message = network_receiver.get_received_message();
  //set message data
  memcpy(to_return.sender_identifier, received_message.message_header.sender_identifier, 8);
  memcpy(to_return.payload, received_message.payload, received_message.message_header.payload_length);
  to_return.message_type = received_message.message_header.message_type;
  to_return.payload_length = received_message.message_header.payload_length;
  return to_return;
}
