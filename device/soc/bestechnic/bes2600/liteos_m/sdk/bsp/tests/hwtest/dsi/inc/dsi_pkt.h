#ifndef __DSI_PACKET_H__
#define __DSI_PACKET_H__

#include <stdio.h>
#include <string.h>

enum DSI_VC_ID {
    DSI_VC_ID_0,
    DSI_VC_ID_1,
    DSI_VC_ID_2,
    DSI_VC_ID_3,
};
#define DSI_ITF_CMD_TYPE 0x04 //LPT
#define DSI_ITF_CMD_ESC  0xE1


#define DSI_DEFAULT_VC 0x0

enum DSI_PKT_ID {
    // short packet
    DSI_PID_V_SYNC_START = 0x01,
    DSI_PID_V_SYNC_END = 0x11,
    DSI_PID_H_SYNC_START = 0x21,
    DSI_PID_H_SYNC_END = 0x31,
    DSI_PID_EOTP = 0x08,
    DSI_PID_COLOR_MODE_OFF = 0x02,
    DSI_PID_COLOR_MODE_ON = 0x12,
    DSI_PID_SHUTDOWN_PERIPH = 0x22,
    DSI_PID_TURN_ON_PERIPH = 0x32,
    DSI_PID_GENERIC_SHORT_WRITE0 = 0x03,
    DSI_PID_GENERIC_SHORT_WRITE1 = 0x13,
    DSI_PID_GENERIC_SHORT_WRITE2 = 0x23,
    DSI_PID_GENERIC_SHORT_READ0 = 0x04,
    DSI_PID_GENERIC_SHORT_READ1 = 0x14,
    DSI_PID_GENERIC_SHORT_READ2 = 0x24,
    DSI_PID_DCS_SHORTW0 = 0x05,
    DSI_PID_DCS_SHORTW1 = 0x15,
    DSI_PID_DCS_READ_REQUEST = 0x06,
    DSI_PID_DCS_SET_MAX_RSP_SIZE = 0x37,
    DSI_PID_DCS_LONGW = 0x39,
    // long packet
    DSI_PID_NULL_PKT = 0x09,
    DSI_PID_BLANK_PKT = 0x19,
    DSI_PID_GENERIC_LONG_WRITE = 0x29,
    DSI_PID_PIXEL_STREAM_RGB565 = 0x0E,
};

struct dsi_pkt_header {
    uint32_t di_type :6; //[5:0]
    uint32_t di_vc   :2; //[7:6]
    uint32_t wc      :16;//[23:8]
    uint32_t ecc     :8; //[31:24]
};

struct dsi_pkt_short {
    uint32_t di_type :6; //[5:0]
    uint32_t di_vc   :2; //[7:6]
    uint32_t data    :16;//[23:8]
    uint32_t ecc     :8; //[31:24]
};

struct dsi_pkt_long {
    struct dsi_pkt_header header; //PH
    uint8_t *payload; //PL
    uint16_t crc; //PF=packet footer
};

typedef struct dsi_pkt_header dsi_pkt_header_t;
typedef struct dsi_pkt_short dsi_pkt_short_t;
typedef struct dsi_pkt_long dsi_pkt_long_t;

struct pixel_data_rgb565 {
    uint16_t r :5; //[4:0]
    uint16_t g :6; //[10:5]
    uint16_t b :5; //[15:11]
};

struct pixel_data_rgb666 {
    uint32_t r :6; //[5:0]
    uint32_t g :6; //[11:6]
    uint32_t b :6; //[17:12]
    uint32_t rsd :14; //[31:18]
};

void dsi_pkt_eotp(struct dsi_pkt_short *p);
uint8_t dsi_pkt_gen_ecc(struct dsi_pkt_short *p);
void dsi_pkt_short_write(struct dsi_pkt_short *p, uint16_t param, uint32_t num);

void dsi_cmd_shortw0(uint8_t cmd);
void dsi_cmd_shortw1(uint8_t cmd, uint8_t data);
int dsi_cmd_longw(uint8_t cmd, uint8_t *data, uint32_t len);
int dsi_pixel_data_longw(uint8_t cmd, uint8_t *data, uint32_t len);
void dsi_dlyms(uint32_t ms);

#endif
