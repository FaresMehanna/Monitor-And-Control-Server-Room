#ifndef SERVER_H
#define SERVER_H

#define PORT 	"33331"	// the port users will be connecting to
#define BACKLOG 10		// how many pending connections queue will hold

#include <pthread.h>
#include <forward_list>
#include <stdint.h>
#include "device.h"

struct ServerData{
	std::forward_list<DeviceData> connected_devices;
	// synchroization tools
	pthread_mutex_t vector_mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t  new_connection_condition = PTHREAD_COND_INITIALIZER;
};

/**
 * Function : start_server
 * -------------------------------
 *
 * This fucntion will start a server listening to PORT.
 * the function will append the sockfd of the connected devidces to
 * the list given in the struct
 */
void *start_server(void* server_data);

#endif