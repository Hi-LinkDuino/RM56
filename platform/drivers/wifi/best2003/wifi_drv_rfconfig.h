#ifndef __WIFI_DRV_RFCONFIG_H__
#define  __WIFI_DRV_RFCONFIG_H__

#include "hal_analogif.h"

#define ISPI_PMU_REG(reg)                   (((reg) & 0xFFF) | 0x0000)
#define ISPI_ANA_REG(reg)                   (((reg) & 0xFFF) | 0x1000)
#define ISPI_RF_REG(reg)                    (((reg) & 0xFFF) | 0x2000)
#define ISPI_WFRF_REG(reg)                  (((reg) & 0xFFF) | 0x3000)

#define wifidrv_read_rf_reg(reg,val)  hal_analogif_reg_read(ISPI_WFRF_REG(reg),val)
#define wifidrv_write_rf_reg(reg,val) hal_analogif_reg_write(ISPI_WFRF_REG(reg),val)
#define wifidrv_read_pmu_reg(reg,val)  hal_analogif_reg_read(ISPI_PMU_REG(reg),val)
#define wifidrv_write_pmu_reg(reg,val) hal_analogif_reg_write(ISPI_PMU_REG(reg),val)




#define HW_READ_REG(a) *(volatile uint32_t *)(a)
#define HW_WRITE_REG(a,v) *(volatile uint32_t *)(a) = v

#define min(a,b)    (((a) < (b)) ? (a) : (b))

void wifi_rfconfig( void);
void wifi_switch_freq(uint16_t freq);
int config_wf_dac_upsample_mode(int mode);
int config_wf_adc_downsample_mode(int mode);

int config_wf_adc_clk_div(int div);
int config_wf_dac_clk_div(int div);

int config_wf_dpd_coef();

int config_wf_5g_mode();
void wifi_rf_config_5G(void);


int wifi_rfpll_pu();
int wifi_rfpll_pu_init();
int wifi_rfpll_reset();

void wifi_rf_temperature_compensation(void);
void wifi_rfconfig( void);
//uint8_t wifidrv_rf_init(void);
uint8_t wifidrv_rf_init(uint8 band);
void wifi_set_xtal_cal_val(uint16 freq_cal);
void logen_cali(void);

void logen_cali_5G(uint16 freq);

// void DC_cali(void);
// void DC_cali_5G(void);

int config_wf_dpd(void);
void enable_rfpll_auto_ctrl(void);
void xtal_osc_amp_calib(void);
void wifi_set_agc_ext(uint8 id);
void bandWidth_limitation(uint8 limit);
void xtal_rc_osc_calib(void);
void rx_agc_calib(uint32* reg_10d, uint32* reg_10e);

void wifi_rf_hw_agc(uint32 reg_10d, uint32 reg_10e);
void config_rfpll_lpf_cal(uint16 temp);
void config_rfpll_dly_cap(int16 tpt_tmp);
void rx_soft_agc_calib(void);

void wifi_switch_freq_5g(uint16_t freq);
void wifi_rf_config_5G(void);
void wifi_switch_freq(uint16_t freq);

void rf_tx_tone_enable(bool enable);
void rf_tx_tone_enable_ana(enum ENUM_WIFI_BAND band, bool enable);

void RX_DC_cali_new(uint8_t band);
void iq_cali_new(uint8_t band);//625K
void iq_cali_new_2nd(uint8_t band);//625K
void DC_cali_new(uint8_t band);//312.5K
void DC_cali_new_2nd(uint8_t band);//312.5K
void rfpll_config_rf(uint8 band);
void xtal_48M_config_rf(void);

int config_dpd(uint16_t channel_id);

extern uint16_t use_freq;
extern uint8 test_ch;


#endif

