#include "TemperatureHumidityHandler.h"

TemperatureHumidityHandler::TemperatureHumidityHandler(ErrorHandler& _errorHandler, uint8_t _pin) : errorHandler(_errorHandler) , dht(_pin) {

}

void TemperatureHumidityHandler::setup(void) {
    dht.begin();
    timer = millis();
}

void TemperatureHumidityHandler::loop(void) {
  if(millis() - timer >= DHT22_SENSOR_INTERVAL_TIME) {
    //update the timer
    timer = millis();
    //check validity of the data
    if (isnan(dht.humidity) || isnan(dht.temperature_C)) {
      errorHandler.report("DHT sensor read failure!");
      return;
    }
    //read data from the sensor
    dht.readHumidity();
    this->humidity = dht.humidity;
    
    dht.readTemperature();
    this->temperature = dht.temperature_C;
  }
}

float TemperatureHumidityHandler::getCurrentTemperature() {
    return this->temperature;
}

float TemperatureHumidityHandler::getCurrentHumidity() {
    return this->humidity;
}
