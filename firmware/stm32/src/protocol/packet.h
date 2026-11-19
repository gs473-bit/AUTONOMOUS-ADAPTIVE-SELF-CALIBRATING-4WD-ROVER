#ifndef AETHER_PACKET_H
#define AETHER_PACKET_H
#include <stdint.h>
#include <stddef.h>
#define PROTO_SOF0 0xA5u
#define PROTO_SOF1 0x5Au
#define PROTO_MAX_PAYLOAD 128u
#define PROTO_MAX_FRAME (9u + PROTO_MAX_PAYLOAD)
typedef enum {
  PKT_DRIVE=1, PKT_STOP=2, PKT_ARM=3, PKT_DISARM=4,
  PKT_SET_PID=5, PKT_SET_MODE=6, PKT_STATUS_REQ=7, PKT_HEARTBEAT=8,
  PKT_TELEMETRY=0x81, PKT_ACK=0x82, PKT_NACK=0x83, PKT_FAULT_LOG=0x84
} packet_type_t;
typedef struct {uint16_t sequence; uint8_t type; uint16_t length; uint8_t payload[PROTO_MAX_PAYLOAD];} packet_t;
typedef struct {
  uint8_t data[PROTO_MAX_FRAME]; size_t used; uint16_t expected;
  uint32_t bad_crc, bad_length, bad_sequence, noise;
  uint8_t synchronized;
} packet_parser_t;
uint16_t packet_crc16(const uint8_t *data, size_t length);
int packet_encode(const packet_t *packet, uint8_t *out, size_t capacity, size_t *written);
void packet_parser_init(packet_parser_t *parser);
int packet_parser_feed(packet_parser_t *parser, uint8_t byte, packet_t *packet);
/* 1: valid; 0: incomplete/noise; -1: bad CRC/length; -2: out-of-sequence. */
#endif
