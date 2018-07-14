#ifndef SERVERS_HANDLER_H
#define SERVERS_HANDLER_H

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "ErrorHandler.h"
#define NO_OP (0)
#define SERVER_TURN_ON_OP (1)
#define SERVER_TURN_OFF_OP (2)
#define SERVER_RESTART_OP (3)
#define SERVER_RESTART_OP2 (4)
#define SERVER_PING_TIME (60000)
#define TURN_ON_DELAY (2000)
#define TURN_OFF_DELAY (5000)
#define RESTART_OFF_ON_DELAY (5000)

class ServersHandler {
private:
	//arrays of server data
	uint8_t *serversRelayPins;
	uint8_t *serversPushButtonPins;
	uint8_t *serversOperation;
	uint32_t *serversTimers;
	uint32_t *serversLastPing;
	//data to all servers
	uint8_t serversCount;
	boolean inOperation;
	//error handler
    ErrorHandler& errorHandler;

protected:
	void connect(uint8_t _serverIndex);
	void disconnect(uint8_t _serverIndex);
	boolean isServerOn(uint8_t _serverIndex);
	boolean isServerOff(uint8_t _serverIndex);

public:
    ServersHandler(ErrorHandler& _errorHandler, uint8_t _serversCount);
    void setup(void);
    void loop(void);

    boolean setPushButtonPin(uint8_t _serverIndex, uint8_t _pushButtonPin);
    boolean setRelayPin(uint8_t _serverIndex, uint8_t _relayPin);

    boolean turnOn(uint8_t _serverIndex);
    boolean turnOff(uint8_t _serverIndex);
    boolean restart(uint8_t _serverIndex);

    void pingFromServer(uint8_t _serverIndex);
};

#endif
