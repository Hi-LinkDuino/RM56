#ifndef _VOW_DATA_H_
#define _VOW_DATA_H_
#include "plat_types.h"

#define STATION_INDEX       0
#define ETH_P_VOW           0xBABA
#define MIC_PAYLOAD         1280
#define SOFTAP_DEV_NUM      1
#define STATION_DEV_NUM     0
#define MAX_STA             3

#ifndef MAC2STR
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#endif

enum mic_data_type {
    MIC_TYPE_DATA,
    MIC_TYPE_CMD,
    MIC_TYPE_QTY,
};

typedef struct {
    uint16_t recv_flags;
    uint8_t peermac[6];
}peer_info;

typedef struct _mid_data_head {
    enum mic_data_type cmd_type;
    uint32_t  data_len;
}MIC_HEAD_T;

typedef struct _mic_data {
    enum mic_data_type cmd_type;
    uint32_t  data_len;
    uint8_t data[MIC_PAYLOAD];
} MIC_DATA_T;

peer_info* vow_get_peer_info();
void vow_send_data(uint8_t* data , size_t len);
void vow_send_cmd(uint8_t* data , size_t len);
uint8_t* vow_get_goup_mac(uint8_t id);
uint8_t vow_get_peer_num();
void vow_data_handle_init();
int vow_data_xmit_v2(u8 dev_idx, const u8 *dest, u16 proto, const u8 *data, size_t data_len);

typedef int (*vow_kcp_callback__t)(uint8_t* data, int len);
void vow_reg_kcp_recv_callbak(vow_kcp_callback__t cb);

#endif//_VOW_DATA_H_