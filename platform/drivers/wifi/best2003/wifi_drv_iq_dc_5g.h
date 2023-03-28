#ifndef __WIFI_DRV_IQ_DC_5G_H__
#define  __WIFI_DRV_IQ_DC_5G_H__

#include "hal_analogif.h"

void iq_cali_new_5g(uint8_t band, uint8 freq_index);//625K
void iq_cali_new_2nd_5g(uint8_t band);//625K
void DC_cali_new_5g(uint8_t band, uint8 freq_index);//312.5K
void DC_cali_new_2nd_5g(uint8_t band);//312.5K

#endif

