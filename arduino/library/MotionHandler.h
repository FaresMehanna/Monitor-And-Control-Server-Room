#ifndef MOTION_HANDLER_H
#define MOTION_HANDLER_H

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "ErrorHandler.h"
#define PIR_SENSOR_UP_TIME (10000)

class MotionHandler {
private:
    boolean motion;
    uint8_t pin;
    uint32_t timer;
    ErrorHandler& errorHandler;

public:
    MotionHandler(ErrorHandler& _errorHandler, uint8_t _pin);
    void setup(void);
    void loop(void);
    boolean isMotion(void);
};

#endif
