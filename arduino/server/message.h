#ifndef MESSAGE_H
#define MESSAGE_H

#define PROTOCOl_MESSAGE_HEADER_LENGTH ((uint16_t)48)
#define PROTOCOl_MAX_PAYLOAD_LENGTH ((uint16_t)255)
#define PROTOCOL_MAX_MESSAGE_LENGTH ((uint16_t)PROTOCOl_MAX_PAYLOAD_LENGTH + PROTOCOl_MESSAGE_HEADER_LENGTH)

#include <stdint.h>

struct NetworkMessageHeader{
  uint32_t HMAC_for_server;
  uint32_t HMAC_for_network;
  uint8_t payload_length;
  uint32_t network_id;
  uint8_t future_use1[3];
  uint8_t aes_vector[8];
  uint64_t unix_timestamp;
  char sender_identifier[8];
  uint8_t message_type;
  uint8_t future_use2[7];
}__attribute__((packed,aligned(1)));

struct NetworkMessage{
	struct NetworkMessageHeader message_header;
	uint8_t payload[PROTOCOl_MAX_PAYLOAD_LENGTH];
  uint16_t message_length;
}__attribute__((packed,aligned(1)));

#endif