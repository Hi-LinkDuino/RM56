#ifndef _BES_SNIFFER_H_
#define _BES_SNIFFER_H_

#include "plat_types.h"

#define SNIFFER_STATE_ENABLE_MASK  0x001
#define SNIFFER_STATE_DISABLE      0x000
#define SNIFFER_STATE_ENABLE       0x001
#define SNIFFER_STATE_MGMT         0x002
#define SNIFFER_STATE_FROMDS       0x004
#define SNIFFER_STATE_TODS         0x008
#define SNIFFER_STATE_40M          0x010
#define SNIFFER_STATE_ACK          0x020
#define SNIFFER_STATE_RTSCTX       0x040
#define SNIFFER_STATE_NULL         0x080

#define SNIFFER_STATE_MGMT_MASK       0xfff000

#define SNIFFER_STATE_MGMT_ASRQ       0x001000
#define SNIFFER_STATE_MGMT_ASRSP      0x002000
#define SNIFFER_STATE_MGMT_RSRQ       0x004000
#define SNIFFER_STATE_MGMT_RSRSP      0x008000
#define SNIFFER_STATE_MGMT_PBRQ       0x010000
#define SNIFFER_STATE_MGMT_PBRSP      0x020000
#define SNIFFER_STATE_MGMT_BCN	      0x040000
#define SNIFFER_STATE_MGMT_ATIM       0x080000
#define SNIFFER_STATE_MGMT_DAS	      0x100000
#define SNIFFER_STATE_MGMT_AUTH       0x200000
#define SNIFFER_STATE_MGMT_DAUTH      0x400000
#define SNIFFER_STATE_MGMT_ACTION     0x800000

enum TYPESUBTYPE_T
{
    ASSOC_REQ             = 0x00,
    ASSOC_RSP             = 0x10,
    REASSOC_REQ           = 0x20,
    REASSOC_RSP           = 0x30,
    PROBE_REQ             = 0x40,
    PROBE_RSP             = 0x50,
    BEACON                = 0x80,
    ATIM                  = 0x90,
    DISASOC               = 0xA0,
    AUTH                  = 0xB0,
    DEAUTH                = 0xC0,
    ACTION                = 0xD0,
    PS_POLL               = 0xA4,
    RTS                   = 0xB4,
    CTS                   = 0xC4,
    ACK                   = 0xD4,
    CFEND                 = 0xE4,
    CFEND_ACK             = 0xF4,
    DATA                  = 0x08,
    DATA_ACK              = 0x18,
    DATA_POLL             = 0x28,
    DATA_POLL_ACK         = 0x38,
    NULL_FRAME            = 0x48,
    CFACK                 = 0x58,
    CFPOLL                = 0x68,
    CFPOLL_ACK            = 0x78,
    QOS_DATA              = 0x88,
    QOS_DATA_ACK          = 0x98,
    QOS_DATA_POLL         = 0xA8,
    QOS_DATA_POLL_ACK     = 0xB8,
    QOS_NULL_FRAME        = 0xC8,
    QOS_CFPOLL            = 0xE8,
    QOS_CFPOLL_ACK        = 0xF8,
    BLOCKACK_REQ          = 0x84,
    BLOCKACK              = 0x94
};

typedef int (*sniffer_handler_t)(const u8 *data, u16 data_len, int signal);

int bes_sniffer_start(sniffer_handler_t handler);
int bes_sniffer_stop(void);
int bes_sniffer_set_channel(u8 channel);
int bes_sniffer_set_filter(u8 f_mgmt, u8 f_fromds, u8 f_tods, u8 f_error, u8 f_ack, u8 f_rts, u8 f_null);
int bes_sniffer_send_mgmt_frame(u8 channel, const u8 *data, size_t len);

#endif
