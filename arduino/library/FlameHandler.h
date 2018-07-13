#ifndef FLAME_HANDLER_H
#define FLAME_HANDLER_H

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "ErrorHandler.h"
#define FLAME_SENSOR_READ_TIME (100)

class FlameHandler {
private:
    boolean flame;
    uint8_t pin;
    uint32_t timer;
    ErrorHandler& errorHandler;

public:
    FlameHandler(ErrorHandler& _errorHandler, uint8_t _pin);
    void setup(void);
    void loop(void);
    boolean flameDetected(void);
};

#endif
