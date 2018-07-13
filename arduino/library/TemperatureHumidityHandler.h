#ifndef TEMPERATURE_HUMIDITY_HANDLER_H
#define TEMPERATURE_HUMIDITY_HANDLER_H

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "cactus_io_DHT22.h"
#include "ErrorHandler.h"

#define DHT22_SENSOR_INTERVAL_TIME (3000)

class TemperatureHumidityHandler {

private:
    float temperature;
    float humidity;
    uint32_t timer;
    ErrorHandler& errorHandler;
    DHT22 dht;
    
public:
    TemperatureHumidityHandler(ErrorHandler& _errorHandler, uint8_t _pin);
    void setup(void);
    void loop(void);
    float getCurrentTemperature();
    float getCurrentHumidity();
};

#endif
