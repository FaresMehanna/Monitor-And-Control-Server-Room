#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/socket.h>
#include "utility.h"

bool send_message(int sockfd, uint8_t *message, uint16_t message_length) {
	uint16_t sent = 0;
	time_t start_time = time(NULL);
	//I added 1 to SEND_TRY_TIME to give a period between [SEND_TRY_TIME, SEND_TRY_TIME+1]
	while(sent < message_length && (time(NULL)-start_time) < (SEND_TRY_TIME+1)) {
	    int16_t sent_temp = send(sockfd, message+sent, message_length-sent, 0);
	    if (sent_temp != -1) {
	    	sent += sent_temp;
	    }
	}
	return sent == message_length;
}

int16_t receive_message(int sockfd, void *buffer, uint16_t recv_length) {
	uint16_t recv_counter = 0;
	int16_t recv_temp = 1;
	time_t start_time = time(NULL);
	while(recv_temp > 0 && (time(NULL)-start_time) < 13) {
		recv_temp = recv(sockfd, ((char *)buffer) + recv_counter, recv_length-recv_counter, 0);
		recv_counter += recv_temp;

		//check full recv_length already received?
		if(recv_counter == recv_length) {
			return recv_length;
		}
	}
	return -1;
}