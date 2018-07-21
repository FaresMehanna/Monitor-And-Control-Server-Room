#include <stdio.h>
#include <forward_list>
#include <pthread.h>
#include <vector>
#include "server.h"
#include "device.h"

int main(void) {
	ServerData server_data;
	pthread_t server_thread;
	int server_thread_state = pthread_create(&server_thread, NULL, start_server, (void *)&server_data);
	if(server_thread_state) {
		perror("Can't launch server thread.");
		return 1;
	}
	while(1) {
		//wait to read the new connected devices
		pthread_mutex_lock(&server_data.vector_mutex);
		while(server_data.connected_devices.begin() == server_data.connected_devices.end()) {
 			pthread_cond_wait(&server_data.new_connection_condition, &server_data.vector_mutex);
		}
		//read the new connected devices fd
		for(const auto& device : server_data.connected_devices) {
        	printf("Handling new device.\n");
			DeviceData* new_device = new DeviceData(device.sockfd);
			pthread_t device_handler_thread;
			pthread_create(&device_handler_thread, NULL, handle_device, (void *)new_device);
		}
		//allow more data to be stored
		server_data.connected_devices.clear();
		pthread_mutex_unlock(&server_data.vector_mutex);
	}
}