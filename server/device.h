#ifndef DEVICE_H
#define DEVICE_H

#define CONNECTION_INTIALIZATION	(1)
#define CONNECTION_WORKING 			(2)
#define CONNECTION_ENDED 			(3)

#include <vector>
#include "message.h"
#include <pthread.h>

struct DeviceData{
	int sockfd;
	uint32_t network_id;
	uint8_t connection_state = CONNECTION_INTIALIZATION;
	uint32_t memory_pointers;
	std::vector<NetworkMessage> sending_buffer;

	//server ping
	time_t last_message_from_client;

	// synchroization tools
	pthread_mutex_t new_message_mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t  new_message_condition = PTHREAD_COND_INITIALIZER;
	pthread_mutex_t itmes_mutex = PTHREAD_MUTEX_INITIALIZER;

	DeviceData() = delete;
	DeviceData(int _sockfd) {
		memory_pointers = 0;
		sockfd = _sockfd;
	}
};

void *handle_device(void *device_data);

#endif