#include "FlameHandler.h"

FlameHandler::FlameHandler(ErrorHandler& _errorHandler, uint8_t _pin) : errorHandler(_errorHandler) {
	flame = false;
	pin = _pin;
	useTimer = millis();
	flameTimer = millis();
}

void FlameHandler::setup(void) {
	pinMode(pin, INPUT);
}

void FlameHandler::loop(void) {
	if(millis() - useTimer < FLAME_SENSOR_READ_TIME){
		return;
	}
	useTimer = millis();

	int flameSensorRead = analogRead(pin);
	if (flameSensorRead < 950){
		flame = true;
		flameTimer = millis();
	} else if (millis() - flameTimer > FLMAE_SENSOR_UP_TIME) {
		flame = false;
	}
}

boolean FlameHandler::flameDetected(void) {
	return flame;
}