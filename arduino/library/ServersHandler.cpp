#include "ServersHandler.h"

ServersHandler::ServersHandler(ErrorHandler& _errorHandler, uint8_t _serversCount) : errorHandler(_errorHandler) {
	serversRelayPins = (uint8_t *)malloc(sizeof(uint8_t)*_serversCount);
	serversPushButtonPins = (uint8_t *)malloc(sizeof(uint8_t)*_serversCount);
	serversOperation = (uint8_t *)malloc(sizeof(uint8_t)*_serversCount);
	serversTimers = (uint32_t *)malloc(sizeof(uint32_t)*_serversCount);
	serversLastPing = (uint32_t *)malloc(sizeof(uint32_t)*_serversCount);

	for(uint8_t i = 0; i < _serversCount; i++){
		serversOperation[i] = NO_OP;
		serversTimers[i] = serversLastPing[i] = millis();
	}

	serversCount = _serversCount;
	inOperation = false;
}

void ServersHandler::setup(void) {

}

void ServersHandler::loop(void) {

	for(uint8_t i = 0; i < serversCount; i++) {
		//handle push buttons
		boolean buttonFlag = false;
		while (digitalRead(serversPushButtonPins[i]) == LOW) {
			delay(50);
			if(digitalRead(serversPushButtonPins[i]) == LOW) {
				connect(i);
				buttonFlag = true;
			}
		}
		if(buttonFlag) {
			disconnect(i);
		}
		//handle operation
		switch(serversOperation[i]) {
			case SERVER_TURN_ON_OP:
				if(millis() - serversTimers[i] > TURN_ON_DELAY) {
					disconnect(i);
					serversOperation[i] = NO_OP;
				}
			break;
			
			case SERVER_TURN_OFF_OP:
				if(millis() - serversTimers[i] > TURN_OFF_DELAY) {
					disconnect(i);
					serversOperation[i] = NO_OP;
				}
			break;
			
			case SERVER_RESTART_OP:
				if(millis() - serversTimers[i] > TURN_OFF_DELAY) {
					disconnect(i);
					serversOperation[i] = SERVER_RESTART_OP2;
					serversTimers[i] = millis();
				}
			break;

			case SERVER_RESTART_OP2:
				if(millis() - serversTimers[i] > RESTART_OFF_ON_DELAY) {
					connect(i);
					serversTimers[i] = millis();
					serversOperation[i] = SERVER_TURN_ON_OP;
				}
			break;

			default:
			break;
		}
	}
}

void ServersHandler::connect(uint8_t _serverIndex) {
	digitalWrite(serversRelayPins[_serverIndex],LOW);
}

void ServersHandler::disconnect(uint8_t _serverIndex) {
	digitalWrite(serversRelayPins[_serverIndex],HIGH);
}

boolean ServersHandler::isServerOn(uint8_t _serverIndex) {
	return ( millis() - serversLastPing[_serverIndex] < SERVER_PING_TIME );
}

boolean ServersHandler::isServerOff(uint8_t _serverIndex) {
	return !isServerOn(_serverIndex);
}

boolean ServersHandler::setPushButtonPin(uint8_t _serverIndex, uint8_t _pushButtonPin) {
	if(_serverIndex >= serversCount){
		return false;
	}
	serversPushButtonPins[_serverIndex] = _pushButtonPin;
	pinMode(_pushButtonPin, INPUT_PULLUP);
	return true;
}

boolean ServersHandler::setRelayPin(uint8_t _serverIndex, uint8_t _relayPin) {
	if(_serverIndex >= serversCount){
		return false;
	}
	serversRelayPins[_serverIndex] = _relayPin;
	pinMode(_relayPin, OUTPUT);
	return true;
}

boolean ServersHandler::turnOn(uint8_t _serverIndex) {
	if(_serverIndex >= serversCount){
		return false;
	}
	if(serversOperation[_serverIndex] != NO_OP){
		return false;
	}
	if(isServerOn(_serverIndex)){
		return false;
	}
	serversOperation[_serverIndex] = SERVER_TURN_ON_OP;
	connect(_serverIndex);
	serversTimers[_serverIndex] = millis();

}

boolean ServersHandler::turnOff(uint8_t _serverIndex) {
	if(_serverIndex >= serversCount){
		return false;
	}
	if(serversOperation[_serverIndex] != NO_OP){
		return false;
	}
	if(isServerOff(_serverIndex)){
		return false;
	}
	serversOperation[_serverIndex] = SERVER_TURN_OFF_OP;
	connect(_serverIndex);
	serversTimers[_serverIndex] = millis();
}

boolean ServersHandler::restart(uint8_t _serverIndex) {
	if(_serverIndex >= serversCount){
		return false;
	}
	if(serversOperation[_serverIndex] != NO_OP){
		return false;
	}
	if(isServerOff(_serverIndex)){
		return false;
	}
	serversOperation[_serverIndex] = SERVER_RESTART_OP;
	connect(_serverIndex);
	serversTimers[_serverIndex] = millis();
}

void ServersHandler::pingFromServer(uint8_t _serverIndex) {
	if(_serverIndex >= serversCount){
		return;
	}
	serversLastPing[_serverIndex] = millis();
}