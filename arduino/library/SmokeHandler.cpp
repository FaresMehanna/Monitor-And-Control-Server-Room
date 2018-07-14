#include "SmokeHandler.h"

SmokeHandler::SmokeHandler(ErrorHandler& _errorHandler, uint8_t _pin) : errorHandler(_errorHandler) {
	smoke = false;
	pin = _pin;
	useTimer = millis();
	smokeTimer = millis();
}

void SmokeHandler::setup(void) {
	pinMode(pin, INPUT);
}

void SmokeHandler::loop(void) {
	if(millis() - useTimer < SMOKE_SENSOR_READ_TIME){
		return;
	}
	useTimer = millis();

	int smokeSensorRead = analogRead(pin);
	if (smokeSensorRead > 330){
		smoke = true;
		smokeTimer = millis();
	} else if (millis() - smokeTimer > SMOKE_SENSOR_UP_TIME) {
		smoke = false;
	}
}

boolean SmokeHandler::smokeDetected(void) {
	return smoke;
}