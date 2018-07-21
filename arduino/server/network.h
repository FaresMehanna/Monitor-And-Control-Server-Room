#ifndef NETWORK_H
#define NETWORK_H

#include <unordered_map>
#include <unordered_set>
#include <set>
#include <pthread.h>
#include "device.h"


/**
 * Function : add_device_to_network
 * -------------------------------
 *
 * This fucntion will add the given device to it's network to be used
 * in the future in any broadcast to it's network.
 */
void add_device_to_network(DeviceData* device_data);


/**
 * Function : remove_device_from_network
 * -------------------------------
 *
 * This fucntion will remove the device from it's network and make it
 * can't recieve any future broadcast.
 */
void remove_device_from_network(DeviceData* device_data);


/**
 * Function : broadcast_to_network
 * -------------------------------
 *
 * broadcast the message to every device connected to the network with
 * network_id identifier.
 */
void broadcast_to_network(NetworkMessage message, uint32_t network_id);


#endif