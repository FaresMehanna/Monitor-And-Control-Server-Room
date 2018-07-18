#include "DefinedPins.h"
#include <ErrorHandler.h>
#include <TemperatureHumidityHandler.h>
#include <MotionHandler.h>
#include <FlameHandler.h>
#include <SmokeHandler.h>
#include <ServersHandler.h>
#include <NetworkHandler.h>

//Error handler
ErrorHandler errorHandler;

//DHT22 for temperature and humidity
TemperatureHumidityHandler temperatureHumidityHandler(errorHandler, DHT22_PIN);

//PIR for motion detection
MotionHandler motionHandler(errorHandler, PIR_SENSOR_PIN);

//flame sensore for file detection
FlameHandler flameHandler1(errorHandler, FLAME_SENSOR_1_PIN);
FlameHandler flameHandler2(errorHandler, FLAME_SENSOR_2_PIN);

//Smoke sensor for fire detection
SmokeHandler smokeHandler(errorHandler, SMOKE_SENSOR_PIN);

//Network Handler to recieve/send messages to peers.
#define SERVER_KEY  "FFFFFFFFFFFFFFFF"
#define NETWORK_KEY "FFFFFFFFFFFFFFFF"
#define NETWORK_ID (0xFFFFFFFF)
NetworkInfo networkInfo(NETWORK_ID, SERVER_KEY, NETWORK_KEY);
NetworkHandler networkHandler(errorHandler, networkInfo);

//Serves handler to turn on/off servers.
#define NUM_OF_SERVERS (3)
ServersHandler serverHandler(errorHandler, NUM_OF_SERVERS);

//Calibrating time
#define CALIBRATING_TIME (1000)

//data to server
#define POST_TO_SERVER_INTERVAL (5000)
uint32_t sending_timer;

void setup_serial(){
    Serial.begin(9600); 
}

void print_to_serial(String message){
  Serial.println(message);
}

void report_data_to_server() {
  if(millis() - sending_timer < POST_TO_SERVER_INTERVAL) {
    return;
  }
  sending_timer = millis();
  String payload = "{";
  payload += "\"temperature\":" + String(temperatureHumidityHandler.getCurrentTemperature()) + ",";
  payload += "\"humidity\":" + String(temperatureHumidityHandler.getCurrentHumidity()) + ",";
  payload += "\"flame\":" + String(flameHandler1.flameDetected() || flameHandler2.flameDetected()) + ",";
  payload += "\"smoke\":" + String(smokeHandler.smokeDetected()) + ",";
  payload += "\"motion\":" + String(motionHandler.isMotion());
  payload += "}";
  networkHandler.send(0x20, (uint8_t *)payload.c_str(), payload.length());
}

void processing_unit(struct Message order){
  int server_number = atoi(order.payload);
  switch(order.message_type) {
    case 0x50:  //start server
      serverHandler.turnOn(server_number);
    break;
    case 0x51:  //restart server
      serverHandler.turnOff(server_number);
    break;
    case 0x52:  //shutdown server
      serverHandler.restart(server_number);
    break;
    case 0x30:  //data from server = server is on
      serverHandler.pingFromServer(server_number);
    break;
    default:
      Serial.println(order.sender_identifier);
      Serial.println(String(order.payload));
    break;
  }
}

void setup() {
  
  setup_serial();
  print_to_serial("SETUP");
  temperatureHumidityHandler.setup();
  motionHandler.setup();
  flameHandler1.setup();
  flameHandler2.setup();
  smokeHandler.setup();
  networkHandler.setup();

  serverHandler.setup();
  serverHandler.setPushButtonPin(0,SERVER1_PUSH_BUTTON_PIN);
  serverHandler.setRelayPin(0,SERVER1_RELAY_PIN);
  serverHandler.setPushButtonPin(1,SERVER2_PUSH_BUTTON_PIN);
  serverHandler.setRelayPin(1,SERVER2_RELAY_PIN);
  serverHandler.setPushButtonPin(2,SERVER3_PUSH_BUTTON_PIN);
  serverHandler.setRelayPin(2,SERVER3_RELAY_PIN);
  
  while(millis() <= CALIBRATING_TIME);
  Serial.flush();
  sending_timer = millis();
}

void loop() {
  temperatureHumidityHandler.loop();
  motionHandler.loop();
  flameHandler1.loop();
  flameHandler2.loop();
  smokeHandler.loop();
  networkHandler.loop();
  serverHandler.loop();
  report_data_to_server(); 
  if(networkHandler.is_received()){
    struct Message data = networkHandler.receive();
    processing_unit(data);
  }
}
