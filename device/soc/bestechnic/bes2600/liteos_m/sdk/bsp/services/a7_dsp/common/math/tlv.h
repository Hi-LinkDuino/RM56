#ifndef _TLV_H_
#define _TLV_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>

#define MATH_HTONS(x) ((uint16_t)((((x) & (uint16_t)0x00ffU) << 8) | (((x) & (uint16_t)0xff00U) >> 8)))
#define MATH_NTOHS(x) MATH_HTONS(x)
#define MATH_HTONL(x) ((((x) & (uint32_t)0x000000ffUL) << 24) | \
                     (((x) & (uint32_t)0x0000ff00UL) <<  8) | \
                     (((x) & (uint32_t)0x00ff0000UL) >>  8) | \
                     (((x) & (uint32_t)0xff000000UL) >> 24))
#define MATH_NTOHL(x) MATH_HTONL(x)


typedef struct { // 10 bytes
    uint32_t magic;
	uint8_t type;
    uint8_t tlv_count;
	uint32_t bodylen;
} __attribute__((packed)) cmd_header_t;

typedef struct { // 3 bytes
    uint8_t tag;
    uint16_t len;
    uint8_t value[0];
} __attribute__((packed)) TLV;

typedef struct {
    cmd_header_t header;
    uint8_t body[0];
} __attribute__((packed)) pkg_t;

#define TLV_MAGIC 0x20220629
#define INIT_HEADER(header, cmd, count, body) \
        do { \
            memset(&header, 0, sizeof(cmd_header_t)); \
            header.magic = TLV_MAGIC; \
            header.type = cmd; \
            header.tlv_count = count; \
            header.bodylen = MATH_HTONL(body); \
        } while(0)

TLV* tlv_get(uint8_t tag, uint8_t* data, int datalen);
int tlv_get_tlvs_sum(TLV** tlvs, int tlv_count);
pkg_t* tlv_get_pkg(int type, TLV** tlvs, int tlv_count, int* outlen);

#ifdef __cplusplus
}
#endif

#endif // _TLV_H_
