#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


class ErrorHandler {
public:
    ErrorHandler(void);
    void report(const String& _error);
    void report(const char* _error);
};

#endif
