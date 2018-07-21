#ifndef UTILITY_H
#define UTILITY_H

#define SEND_TRY_TIME (2)

/**
 * Function : send_message
 * -------------------------------
 *
 * send a message to sockfd in max time of [SEND_TRY_TIME, SEND_TRY_TIME+1]
 */
bool send_message(int sockfd, uint8_t *message, uint16_t message_length);

/**
 * Function : receive_message
 * -------------------------------
 *
 * recieve a message from sockfd, must be equal to recv_length or -1
 */
int16_t receive_message(int sockfd, void *buffer, uint16_t recv_length);

#endif