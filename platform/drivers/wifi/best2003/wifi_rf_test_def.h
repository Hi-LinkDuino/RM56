/***************************************************************************
 *
 * Copyright 2015-2019 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#ifndef __WIFI_RF_TEST_DEF_H__
#define __WIFI_RF_TEST_DEF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "wifi_def.h"

#if 1
#define DPD_TEST  0
#define DPD_TRACK_TEST  0
//#define SMTH_TEST  0

//#define PATCH_TEST 1

#define AON_SLEEP_TEST  0
#define WIFI_CPU_USE_320M  0


//test MICRO
#define USE_24M 0
#define TX_TEST 1
#define TX_TEST_BUF 0
#define RX_TEST_BUF 0

#define VCO_TEST 0


//#define AX_MEM_TEST 1

// #define SUPPORT_40M 0


//test MICRO end

#endif

#define USE_GPIO21_LMAC_LOG 0


#define USE_DPD  0
#define USE_PATCH  1
#define SIG_EQ_CHE_EN  1
#define USE_RANDOM_MAC 1

//select BGA or QFN

#ifndef BGA_TX_TEST

#ifndef WIFI_QFN_RF_REG
#define BGA_TX_TEST                  1 //BGA only by zhengtao and LiuShaoxiong
#define WIFI_QFN_RF_REG              0 //QFN only by ZhengTao and LiuShaoxiong
#endif

#endif

//caliration switch

#define WIFI_DAC_CEIL_CALI           1 //dac ceil calibration

#define WIFI_IQ_CALI                 1 //625k tx iq calibration

#define WIFI_DC_CALI                 1 //312.5k tx dc calibration

#define WIFI_DPD_CALI                1 //dpd calibration

#define WIFI_RF_TEMPERATURE_CALI     1 //rfpll temperature compensation calibration

#define WIFI_TX_TEMPERATURE_CALI_V1  0 //tx pa temperature compensation calibration v1

#define WIFI_TX_TEMPERATURE_CALI_V2  1 //tx pa temperature compensation calibration v2

#define WIFI_BBPL_TEMPTR_CALI_V1     0 //bbpl temperature compensation calibration v1

#define WIFI_BBPL_TEMPTR_CALI_V2     1 //bbpl temperature compensation calibration v2

#define WIFI_IQ_TEMPTR_CALI_V1       1 //iq temperature compensation calibration V1

#define WIFI_RX_DC_CALI              0 //rx dc calibration

#define WIFI_RCOSC_CALI              1 //RC calibration

#define WIFI_RCOSC_CALI_V2           1 //RC calibration v2

//other funtion
#define PMU_DCDC_POWER_SUPPLY        1 //the mode reduce current 

#define WIFI_40M_PERFORMANCE_ENHANCE 1 //40M performance enhance

#define WIFI_RF_XIERENZHONG          1 //add by XieRenzhong

#define IC_CALIB_EXT                 1 //for 2nd iq calibration (RongZecun add for iq cali)

#define WIFI_5G_ZHENGTAO             1 //add by ZhengTao

#define WIFI_RF_PA_THERMAL_RESP      1 //pa thermal response comp by ZhengTao

#define WIFI_DPD_LOOP_CALI           1 //dpd large freq calibration

#define WIFI_EVB_SWITCH_5G           1 //EVB switch 5G

#define WIFI_ECO_CHIP_XIERZ          0 //eco chip need reg by XieRZ

#define WIFI_ECO_CHIP_ZHENGT         0 //eco chip need reg by ZhengT

#define WIFI_FEM_TEST                0 //eco chip need function by YangL and WuYS

#define WIFI_XTAL_SELECT             0 //0: 24m xtal; 1: 48m xtal

#define WIFI_VCO_RFPLL_1_3_V         0 //vco and rfpll low power, 0: default, 1.8v; 1: 1.3v; ZhengT

#ifndef WIFI_2003_ANA_LOW_POWER
#define WIFI_2003_ANA_LOW_POWER      0 //2003/2006 ana low power config
#endif

#define WIFI_FT_TEMPRETURE_OFFSET    0 //tx pa temperature compensation offset by factory

#define WIFI_IQ_TEST                 0 //iq test, 0: default !!, 1: test only

#define WIFI_DPD_TEST                0 //dpd test, 0: default !!, 1: test only

#ifndef WIFI_USE_FLASH_DC_DATA
#define WIFI_USE_FLASH_DC_DATA       0 //use flash data of dc cali
#endif

#ifndef WIFI_CALI_CHINA_ONLY
#define WIFI_CALI_CHINA_ONLY         0 //ch100 - ch144 cancal cali
#endif

#ifndef WIFI_RF_TEMPRTR_PROTECT
#define WIFI_RF_TEMPRTR_PROTECT      0 //rf temperature protection
#endif


//function select
#define WIFI_RF_SWITCH               0 //eco chip need; rf switch: 0: yunque(14) or Bailu(gpio24),select by  g_wifi_rf_tx_io ; 1: bes EVB gpio20;

//signal mode micro

//debug log macro
#define WIFI_IQ_DEBUG                0 //IQ debug

#define WIFI_DPD_DEBUG               0 //dpd debug

#define WIFI_QFN_TEMPERATURE_DEBUG   0 //temperature compensation debug

#define WIFI_RX_DC_DEBUG             0 //rx dc debug

#define WIFI_TX_TEMPERATURE_V1_DEBUG 0 //tx pa temperature compensation calibration v1 debug

#define WIFI_TX_TEMPERATURE_V2_DEBUG 0 //tx pa temperature compensation calibration v2 debug

#define WIFI_BBPL_TEMPERT_V1_DEBUG   0 //bbpl temperature compensation calibration v1 debug

#define WIFI_BBPL_TEMPERT_V2_DEBUG   0 //bbpl temperature compensation calibration v2 debug

#define WIFI_IQ_TEMPERT_V1_DEBUG     1 //iq temperature compensation calibration v1 debug

#define WIFI_DC_IQ_LINE_FIT_DEBUG    0 //dc iq line fit debug

#define WIFI_RCOSC_DEBUG             1 //RC calibration debug


#ifdef __cplusplus
}
#endif

#endif

