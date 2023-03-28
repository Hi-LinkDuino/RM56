#ifndef __WIFI_DRV_RC_H__
#define __WIFI_DRV_RC_H__



float wifi_rc_calculate_freq_m3dB(uint8_t band);

uint16 wifi_rc_adc_cap_cali(uint8_t band);

int wifi_rc_filter_adc_cap_cali_v2(uint8_t flt_result[4], uint8_t adc_result[4]);
#endif

