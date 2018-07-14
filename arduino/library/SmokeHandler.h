#ifndef SMOKE_HANDLER_H
#define SMOKE_HANDLER_H

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "ErrorHandler.h"
#define SMOKE_SENSOR_READ_TIME (100)
#define SMOKE_SENSOR_UP_TIME (20000)

class SmokeHandler {
private:
    boolean smoke;
    uint8_t pin;
    uint32_t useTimer;
    uint32_t smokeTimer;
    ErrorHandler& errorHandler;

public:
    SmokeHandler(ErrorHandler& _errorHandler, uint8_t _pin);
    void setup(void);
    void loop(void);
    boolean smokeDetected(void);
};

#endif
