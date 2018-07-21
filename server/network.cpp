#include <vector>
#include <algorithm>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include "network.h"
#include "device.h"
#include "utility.h"
#include "message.h"
#include "memory.h"

/**
 * Global data and strucutres needed to keep track of each netwrok
 * and each device connected to each network.
 */
struct network_devices_hash{
    uint64_t operator() (const DeviceData* data) const {
        return data->sockfd;
    }
};
struct network_devices_equal{
    bool operator() (const DeviceData* data1, const DeviceData* data2) const {
        return data1->sockfd == data2->sockfd;
    }
};
pthread_mutex_t network_devices_mutex = PTHREAD_MUTEX_INITIALIZER;
std::unordered_map<uint32_t, std::unordered_set<DeviceData*, network_devices_hash, network_devices_equal>> network_devices;


void add_device_to_network(DeviceData* device_data) {
    printf("Add to network acquire.\n");
    acquire(device_data);
    pthread_mutex_lock(&network_devices_mutex);
	network_devices[device_data->network_id].insert(device_data);
    pthread_mutex_unlock(&network_devices_mutex);
}

void remove_device_from_network(DeviceData* device_data) {
    pthread_mutex_lock(&network_devices_mutex);
	network_devices[device_data->network_id].erase(device_data);
    pthread_mutex_unlock(&network_devices_mutex);
    printf("Remove from network release.\n");
    release(device_data);
}

void broadcast_to_network(NetworkMessage message, uint32_t network_id) {
    pthread_mutex_lock(&network_devices_mutex);
    // get list of devices inside the network
    std::vector<DeviceData*> current_network_devices;
    for(DeviceData* device_data : network_devices[network_id]){
        printf("Broadcast acquire.\n");
        acquire(device_data);
    	current_network_devices.push_back(device_data);
    }
    pthread_mutex_unlock(&network_devices_mutex);

    //send to every device connected to the network
    for(int i=0; i<current_network_devices.size(); i++) {
    	pthread_mutex_lock(&(current_network_devices[i]->new_message_mutex));
        current_network_devices[i]->sending_buffer.push_back(message);
        printf("Broadcast release.\n");
        release(current_network_devices[i]);
        pthread_cond_signal(&(current_network_devices[i]->new_message_condition));  
    	pthread_mutex_unlock(&(current_network_devices[i]->new_message_mutex));
    }

}