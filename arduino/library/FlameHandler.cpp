#include "FlameHandler.h"

FlameHandler::FlameHandler(ErrorHandler& _errorHandler, uint8_t _pin) : errorHandler(_errorHandler) {
	flame = false;
	pin = _pin;
	timer = millis();
}

void FlameHandler::setup(void) {
	pinMode(pin, INPUT);
}

void FlameHandler::loop(void) {
	if(millis() - timer < FLAME_SENSOR_READ_TIME){
		return;
	}
	timer = millis();

	int flameSensorRead = analogRead(pin);
	if (flameSensorRead < 950){
		flame = true;
	}
}

boolean FlameHandler::flameDetected(void) {
	return flame;
}