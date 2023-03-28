#ifndef __VOW_MIC_KCP_H__
#define __VOW_MIC_KCP_H__

#ifdef __VOW_MIC_KCP__
#define VOW_KCP_HEAD_LEN    24
int vow_kcp_init(void);
void vow_kcp_update_clock(uint8_t id);
int vow_parase_kcp_data(uint8_t* data, int len);
void vow_kcp_send(uint8_t id, uint8_t* buf, uint32_t len);
#endif

#endif
