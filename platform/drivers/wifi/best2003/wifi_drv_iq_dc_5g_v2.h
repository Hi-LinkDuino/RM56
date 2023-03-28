#ifndef __WIFI_DRV_IQ_DC_5G_V2_H__
#define  __WIFI_DRV_IQ_DC_5G_V2_H__

#include "hal_analogif.h"

void iq_cali_new_5g_v2(uint8_t band, uint8 freq_index);//625K
void DC_cali_new_5g_v2(uint8_t band, uint8 freq_index);//312.5K
int iq_compensate_by_temperature_2003(uint8_t band, uint16_t ch_id, uint16 mode);
#endif

