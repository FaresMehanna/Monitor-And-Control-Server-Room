#include "MotionHandler.h"

MotionHandler::MotionHandler(ErrorHandler& _errorHandler, uint8_t _pin) : errorHandler(_errorHandler) {
	motion = false;
	pin = _pin;
	timer = millis();
}

void MotionHandler::setup(void) {
	pinMode(pin, INPUT);
}

void MotionHandler::loop(void) {
	if(digitalRead(pin) == HIGH) {
		motion = true;
		timer = millis();
	} else if (millis() - timer > PIR_SENSOR_UP_TIME) {
		motion = false;
	}
}

boolean MotionHandler::isMotion(void) {
	return motion;
}