#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <pthread.h>
#include <arpa/inet.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <unistd.h>
#include "network.h"
#include "device.h"
#include "utility.h"
#include "memory.h"
#include "message.h"

static void handle_recvd_message (NetworkMessage message, DeviceData *device_data) {

	//do ntoh conversions
	message.message_header.network_id = ntohl(message.message_header.network_id);
	message.message_header.HMAC_for_server = ntohl(message.message_header.HMAC_for_server);

	//currently this function only supports network id = 0
	if(message.message_header.network_id != 0) {
		return;
	}

    const uint8_t key[] = {
      0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,
      0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46
    };

	unsigned char *server_sign = HMAC(EVP_sha256(), key, sizeof(key), ((uint8_t *)(&message)) + 8, message.message_length - 8, NULL, NULL);
    if(memcmp(server_sign, &(message.message_header.HMAC_for_server), 4) != 0) {
    	return;	//not properly signed message
    }

    printf("message arrived!.\n");
    printf("message length: %d.\n\n",message.message_length);

    /* Now I verified the message */
	pthread_mutex_lock(&(device_data->itmes_mutex));
	uint8_t connection_state = device_data->connection_state;
	device_data->last_message_from_client = time(NULL);
	pthread_mutex_unlock(&(device_data->itmes_mutex));

    if(connection_state == CONNECTION_INTIALIZATION) {

 		pthread_mutex_lock(&(device_data->itmes_mutex));
    	device_data->network_id = message.message_header.network_id;
    	device_data->connection_state = CONNECTION_WORKING;
		pthread_mutex_unlock(&(device_data->itmes_mutex));

    	add_device_to_network(device_data);
    }

    //do hton conversions
   	message.message_header.network_id = htonl(message.message_header.network_id);
	message.message_header.HMAC_for_server = htonl(message.message_header.HMAC_for_server);

	//handle if it's ping message
	if(message.message_header.payload_length == 0x0 &&
	   message.message_header.HMAC_for_network == 0xFFFFFFFF &&
	   *(uint64_t *)message.message_header.aes_vector == 0xFFFFFFFFFFFFFFFF) {
		printf("PING RECV!\n");
		return;
	}

    //else, broadcast to network
    broadcast_to_network(message, ntohl(message.message_header.network_id));
}

static void *handle_device_receiver(void *device_data) {
	
	//aquire the data
	DeviceData *device_data_ptr = (DeviceData *)device_data;
    printf("acquire - receiver.\n");
	acquire(device_data_ptr);

	//read from the client untill the connection closes
	NetworkMessage message;

	
	pthread_mutex_lock(&(device_data_ptr->itmes_mutex));
	time_t last_recv = device_data_ptr->last_message_from_client = time(NULL);
	pthread_mutex_unlock(&(device_data_ptr->itmes_mutex));

	while(time(NULL) - last_recv < 30) {
		//read the header
		int16_t header_len = receive_message(device_data_ptr->sockfd, &message, PROTOCOl_MESSAGE_HEADER_LENGTH);
		if(header_len == -1) {
			break;
		}

		//read the payload
		int16_t payload_len = receive_message(device_data_ptr->sockfd, ((char *)&message) + PROTOCOl_MESSAGE_HEADER_LENGTH, message.message_header.payload_length);
		if(payload_len == -1) {
			break;
		}

		message.message_length = PROTOCOl_MESSAGE_HEADER_LENGTH + message.message_header.payload_length;
		handle_recvd_message(message, device_data_ptr);

		pthread_mutex_lock(&(device_data_ptr->itmes_mutex));
		last_recv = device_data_ptr->last_message_from_client;
		pthread_mutex_unlock(&(device_data_ptr->itmes_mutex));
	}

	//remove the device from the network
	pthread_mutex_lock(&(device_data_ptr->itmes_mutex));
	uint8_t connection_state = device_data_ptr->connection_state;
	pthread_mutex_unlock(&(device_data_ptr->itmes_mutex));
	if(connection_state == CONNECTION_WORKING) {
    	remove_device_from_network(device_data_ptr);
	}

	//set the connection state to CONNECTION_ENDED
	pthread_mutex_lock(&(device_data_ptr->itmes_mutex));
	device_data_ptr->connection_state = CONNECTION_ENDED;
	pthread_mutex_unlock(&(device_data_ptr->itmes_mutex));

	//release and exit the thread
    pthread_mutex_lock(&(device_data_ptr->new_message_mutex));
	pthread_cond_signal(&(device_data_ptr->new_message_condition));  
    pthread_mutex_unlock(&(device_data_ptr->new_message_mutex));
    printf("release - recv.\n");
	release(device_data_ptr);
	pthread_exit(NULL);
}

static void *handle_device_sender(void *device_data) {

	DeviceData *device_data_ptr = (DeviceData *)device_data;
    printf("acquire - sender.\n");
	acquire(device_data_ptr);
	uint8_t state = CONNECTION_INTIALIZATION;

	while(state != CONNECTION_ENDED) {
		//wait for new messages to be sent
		pthread_mutex_lock(&(device_data_ptr->new_message_mutex));
 		pthread_cond_wait(&(device_data_ptr->new_message_condition), &(device_data_ptr->new_message_mutex));
		
		//send all messages in the buffer
		for(NetworkMessage message : device_data_ptr->sending_buffer) {
			send_message(device_data_ptr->sockfd,(uint8_t *) &message, message.message_length);
		}

		//allow more messages to be stored
		(device_data_ptr->sending_buffer).clear();
		pthread_mutex_unlock(&(device_data_ptr->new_message_mutex));

		//get new state
		pthread_mutex_lock(&(device_data_ptr->itmes_mutex));
		state = device_data_ptr->connection_state;
		pthread_mutex_unlock(&(device_data_ptr->itmes_mutex));
	}
    printf("release - sender.\n");
	release(device_data_ptr);
	pthread_exit(NULL);
}

static void create_ping_message (NetworkMessage& message) {

	message.message_header.payload_length = 0x0;
	message.message_header.network_id = 0x0;
	message.message_header.unix_timestamp = 0xFFFFFFFFFFFFFFFF;
	message.message_header.message_type = 0xFF;
	message.message_header.HMAC_for_network = 0xFFFFFFFF;
	message.message_length = PROTOCOl_MESSAGE_HEADER_LENGTH;
	memset(message.message_header.future_use1,0xFF,3);
	memset(message.message_header.future_use2,0xFF,7);
	memset(message.message_header.aes_vector,0xFF,8);
	memset(message.message_header.sender_identifier,0xFF,8);

    const uint8_t key[] = {
      0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,
      0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46
    };
	unsigned char *server_sign = HMAC(EVP_sha256(), key, sizeof(key), ((uint8_t *)(&message)) + 8, message.message_length - 8, NULL, NULL);
	message.message_header.HMAC_for_server = htonl( *(uint32_t *) server_sign);
}

void *handle_device(void *device_data) {
	DeviceData *device_data_ptr = (DeviceData *)device_data;
    printf("acquire - main.\n");
	acquire(device_data_ptr);
	//start sender thread
	pthread_t device_sender_thread;
	int device_sender_thread_state = pthread_create(&device_sender_thread, NULL, handle_device_sender, device_data);
	if(device_sender_thread_state) {
		perror("Can't launch handle_device_sender.");
		return NULL;
	}
	//start receiver thread
	pthread_t device_receiver_thread;
	int device_receiver_thread_state = pthread_create(&device_receiver_thread, NULL, handle_device_receiver, device_data);
	if(device_receiver_thread_state) {
		perror("Can't launch device_receiver_thread.");
		return NULL;
	}
	uint8_t state = CONNECTION_INTIALIZATION;
	while(state != CONNECTION_ENDED) {
		NetworkMessage message;
		create_ping_message(message);
		//send ping message
		pthread_mutex_lock(&(device_data_ptr->new_message_mutex));
        device_data_ptr->sending_buffer.push_back(message);
		pthread_cond_signal(&(device_data_ptr->new_message_condition));  
		pthread_mutex_unlock(&(device_data_ptr->new_message_mutex));

		sleep(5);
		//get new state
		pthread_mutex_lock(&(device_data_ptr->itmes_mutex));
		state = device_data_ptr->connection_state;
		pthread_mutex_unlock(&(device_data_ptr->itmes_mutex));
	}
    printf("release - main.\n");
	release(device_data_ptr);
	pthread_exit(NULL);
}