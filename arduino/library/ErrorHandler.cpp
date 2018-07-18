#include "ErrorHandler.h"

ErrorHandler::ErrorHandler(void) {

}

void ErrorHandler::report(const String& _error) {
	Serial.println(_error);
}

void ErrorHandler::report(const char* _error) {
	Serial.println(_error);
}