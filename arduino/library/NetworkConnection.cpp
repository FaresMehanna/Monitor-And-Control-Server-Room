#include "NetworkHandler.h"

void NetworkConnection::keep_connection(){
  //make sure the arduino is connected to the server
  if(!network_info.client.connected()){
    network_info.client.stop();    
    network_info.client.connect(network_info.server, PROTOCOl_SERVER_PORT);
    network_info.last_poke_to_server = millis();
    network_info.last_poke_from_server = millis();
    network_info.connection_number++;
  }
}

void NetworkConnection::close_and_connect() {
  network_info.client.stop();    
  network_info.client.connect(network_info.server, PROTOCOl_SERVER_PORT);
  network_info.last_poke_to_server = millis();
  network_info.last_poke_from_server = millis();
  network_info.connection_number++;
}

NetworkConnection::NetworkConnection(ErrorHandler& _errorHandler, NetworkInfo& _network_info, NetworkHandler& _network_handler) :
 errorHandler(_errorHandler), network_info(_network_info), network_handler(_network_handler){

}

boolean NetworkConnection::is_connected(void) {
  return millis() - network_info.last_poke_from_server <= PROTOCOl_RECIEVE_POKE_TIME;
}

void NetworkConnection::keep_connected(void) {
  keep_connection();

  //make sure you heared from the server since at most PROTOCOl_RECIEVE_POKE_TIME
  if(millis() - network_info.last_poke_from_server > PROTOCOl_RECIEVE_POKE_TIME) {
    close_and_connect();
  }

  //make sure you send the server poke message periodically.
  if(millis() - network_info.last_poke_to_server > PROTOCOl_SEND_POKE_TIME) {
    //try to send the server poke message
    if(network_handler.send(0x00,(uint8_t *)"",0)) {
      network_info.last_poke_to_server = millis();
    } else {
      close_and_connect();
    }
  }
}