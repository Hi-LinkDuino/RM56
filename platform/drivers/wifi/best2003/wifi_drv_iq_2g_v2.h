#ifndef __WIFI_DRV_IQ_2G_V2_H__
#define  __WIFI_DRV_IQ_2G_V2_H__

#include "hal_analogif.h"

#define WIFI_IQ_CALI_V2                     1 //wifi iq cali v2

#define WIFI_IQ_CALI_DEBUG_V2               0 //wifi iq debug v2

void wifi_iq_cali_2g_v2(uint8 freq_index, uint8 band, uint8 step, uint16 gain, uint32 len);//625K


#endif

