#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include "device.h"
#include "memory.h"

void acquire(DeviceData* device_data) {
    printf("acquire.\n");
    pthread_mutex_lock(&(device_data->itmes_mutex));
	(device_data->memory_pointers)++;
    pthread_mutex_unlock(&(device_data->itmes_mutex));
}

void release(DeviceData* device_data) {
    printf("release.\n");
    pthread_mutex_lock(&(device_data->itmes_mutex));
	(device_data->memory_pointers)--;
	if(0 == device_data->memory_pointers) {
		close(device_data->sockfd);
		delete(device_data);
    	printf("delete.\n");
	}
    pthread_mutex_unlock(&(device_data->itmes_mutex));
}