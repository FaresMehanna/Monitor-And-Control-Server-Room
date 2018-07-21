#ifndef MEMORY_H
#define MEMORY_H

/**
 * Function : acquire, release
 * -------------------------------
 *
 * These functions will keep track of usage of the device, in cas the device
 * no longer in use it will be freed.
 * This allow the main thread to be terminated before it's data be freed.
 */
void acquire(DeviceData* device_data);
void release(DeviceData* device_data);

#endif