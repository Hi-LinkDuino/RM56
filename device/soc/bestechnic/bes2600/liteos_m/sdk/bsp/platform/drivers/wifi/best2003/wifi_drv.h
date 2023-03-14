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
#ifndef __WIFI_DRV_H__
#define __WIFI_DRV_H__

#ifdef __cplusplus
extern "C" {
#endif

#if 0
#define DPD_TEST  0
#define DPD_TRACK_TEST  0
#define SMTH_TEST  1

#define PATCH_TEST 1

#define AON_SLEEP_TEST  0
#define WIFI_CPU_USE_320M  0


//test MICRO
#define USE_24M 0
#define TX_TEST 1


//test MICRO
#endif


#include "plat_addr_map.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "hal_analogif.h"

#include "patch.h"
#include "wifi_rf_test_def.h"
typedef short int16;
typedef unsigned short uint16;
typedef int   int32;
typedef unsigned int   uint32;

typedef unsigned char   uint8;

#define wifidrv_read_rf_reg(reg,val)  hal_analogif_reg_read(ISPI_WFRF_REG(reg),val)
#define wifidrv_write_rf_reg(reg,val) hal_analogif_reg_write(ISPI_WFRF_REG(reg),val)
#define HW_READ_REG(a) *(volatile uint32_t *)(a)
#define HW_WRITE_REG(a,v) *(volatile uint32_t *)(a) = v

#define ISPI_PMU_REG(reg)                   (((reg) & 0xFFF) | 0x0000)
#define ISPI_ANA_REG(reg)                   (((reg) & 0xFFF) | 0x1000)
#define ISPI_RF_REG(reg)                    (((reg) & 0xFFF) | 0x2000)
#define ISPI_WFRF_REG(reg)                  (((reg) & 0xFFF) | 0x3000)

#define ZERO_JUDJMENT         1.0e-4


int config_wf_dac_upsample_mode(int mode);
int config_wf_adc_downsample_mode(int mode);

int config_wf_adc_clk_div(int div);
int config_wf_dac_clk_div(int div);

int config_epta(void);



#ifdef __WIFI_FW_RAMRUN__
#define WIFI_PATCH_ADDR  0x80100000     //use sram1
#else
#define WIFI_PATCH_ADDR  0x80010000
#endif

#ifdef __WIFI_RAMRUN__
#define wifidrv_delay(ms) hal_sys_timer_delay(MS_TO_TICKS(ms))
#else
#define wifidrv_delay(ms) osDelay(ms)
#endif

#define wifidrv_cali_delay_us(us) hal_sys_timer_delay(US_TO_TICKS(us))

#define WIFIDIGITAL_REG(a) (*(volatile uint32_t *)(a))
#define WIFIDIGITAL_REG_SET_FIELD(reg, mask, shift, v)\
                                                do{ \
                                                    volatile unsigned int tmp = *(volatile unsigned int *)(reg); \
                                                    tmp &= ~(mask<<shift); \
                                                    tmp |= (v<<shift); \
                                                    *(volatile unsigned int *)(reg) = tmp; \
                                                }while(0)

#define WIFIDIGITAL_REG_GET_FIELD(reg, mask, shift, v)\
                                                do{ \
                                                    volatile unsigned int tmp = *(volatile unsigned int *)(reg); \
                                                    v = (tmp>>shift)&mask; \
                                                }while(0)

#define DRV_DEBUG  0
#if DRV_DEBUG
#define DRV_PRINT(fmt, ...) TRACE(fmt, ##__VA_ARGS__)
#define DRV_DUMP(s,buff,len) DUMP8(s,buff,len)
#else
#define DRV_PRINT(fmt, ...)
#define DRV_DUMP(s,buff,len)
#endif

#define PHY_BASE                                0x90100000
#define NRDT_BASE                               PHY_BASE
#define NTSG_BASE                               (PHY_BASE + 0x00020000)
#define BBDIG_BASE                              (PHY_BASE + 0x00038000)

#define PHY_RDT_NRDTCTRL (0x90100000)
//#define PHY_RDT_NRDTCTRL            (NRDT_BASE + 0x0000)   //
#define PHY_TMP_40M_MODE   0x9011809c   //(NTMP_BASE + 0x009c)


#define PHY_TSG_NTONT               (NTSG_BASE + 0x0010) // Radio On Counter Register
#define PHY_TSG_NTENT               (NTSG_BASE + 0x0014) // PHY Enable Counter Register

#define PHY_BBDIG_RICTRL            (BBDIG_BASE + 0x0004) // Radio Interface control register
#define PHY_BBDIG_RITONECTL         (BBDIG_BASE + 0x005c) // Tone Generator  Control register
#define PHY_BBDIG_RITONEGEN_0       (BBDIG_BASE + 0x0060) // Tone Generator 1 Configuration register
#define PHY_BBDIG_RITONEGEN_1       (BBDIG_BASE + 0x0064) // Tone Generator 2 Configuration register

#define PHY_RI_FEMPAEN2GHZ_ADDR     (BBDIG_BASE + 0x02C4)//0x901382c4
#define PHY_RI_FEMPAEN5GHZ_ADDR     (BBDIG_BASE + 0x02C8) //0x901382c8
#define PHY_RI_TXGAIN_0_ADDR        (BBDIG_BASE + 0x001C)//0x9013801c


#define PAC_PTA_SW_OVERRIDE                     (0x82c0104c )
#define PAC_RXC_RX_BUFFER_IN_POINTER            (0x82c00600 + 0x04)
#define PAC_RXC_RX_BUFFER_OUT_POINTER           (0x82c00600 + 0x08)


//rate: 0 ... 21, 22  (1m ... 65m, mcs32)
//#define RATE_INDEX_B_1M           0
//#define RATE_INDEX_B_2M           1
//#define RATE_INDEX_B_5_5M         2
//#define RATE_INDEX_B_11M          3
//#define RATE_INDEX_PBCC_22M       4     // not supported/unused
//#define RATE_INDEX_PBCC_33M       5     // not supported/unused
//OFDM legacy
//#define RATE_INDEX_A_6M           6
//#define RATE_INDEX_A_9M           7
//#define RATE_INDEX_A_12M          8
//#define RATE_INDEX_A_18M          9
//#define RATE_INDEX_A_24M          10
//#define RATE_INDEX_A_36M          11
//#define RATE_INDEX_A_48M          12
//#define RATE_INDEX_A_54M          13
//HT-OFDM
//#define RATE_INDEX_N_6_5M         14
//#define RATE_INDEX_N_13M          15
//#define RATE_INDEX_N_19_5M        16
//#define RATE_INDEX_N_26M          17
//#define RATE_INDEX_N_39M          18
//#define RATE_INDEX_N_52M          19
//#define RATE_INDEX_N_58_5M        20
//#define RATE_INDEX_N_65M          21
//40M
//#define RATE_INDEX_N_MCS32        22

//mode: 0: legacy; 1: mix-mode; 2: green field
//test_mode: 4bit, bit0: shortgi; bit1: 40m half-band; bit2: non-ht dup; bit3: 40m frame
//length: 12bit
//mode_40m: static 40m mode
//below_20m:
//random_data:




#define  WIFI_LOW_POWER_MODE  1
#define  WIFI_HIGH_PERFORMANCE_MODE   2

#define WIFI_RFMODE WIFI_LOW_POWER_MODE
//#define WIFI_RFMODE WIFI_HIGH_PERFORMANCE_MODE

#define WIFI_POWERON            1
#define WIFI_POWEROFF           0



/// 8 bit access types
#define _8_Bit                              8
/// 16 bit access types
#define _16_Bit                             16
/// 32 bit access types
#define _32_Bit                             32

#define WIFIDRV_PATCH_WRITING  0x0
#define WIFIDRV_PATCH_DONE         0x1


#define WIFIDRV_CFG_WRITING   0x0
#define WIFIDRV_CFG_DONE      0x1
typedef int (*HOOK_FUNC_T)(void *);


#define HOOK_TABLE_MAGIC                        0xBE578006

//rom_run_flag

#define  WIFI_RUN_BAND                     (1<<0)
#define  WIFI_BT_COEXIST                   (1<<1)
#define  WIFI_USE_DPD                      (1<<2)
#define  WIFI_USE_TRIPCAPBANDCAL           (1<<3)
#define  WIFI_USE_EPTA                     (1<<4)
#define  WIFI_USE_160M_CPU_FREQ            (1<<5)
#define  WIFI_USE_40M_BANDWIDTH            (1<<6)
#define  WIFI_USE_SGI                      (1<<7)
#define  WIFI_40M_2ND_SEL                  (1<<8)
#define  WIFI_SIGNAL_MODE_SNIFFER_OPEN     (1<<9)
#define  WIFI_NO_SLEEP                     (1<<10)
#define  WIFI_CLOSE_PA            	   (1<<20)
#define  WIFI_SET_PMU_VPA_CLOSE            (1<<25)

#define  SET_CHIP_FLAG(chip_id)  ((chip_id& 0xf) <<16)  //for lmac 1:BGA, 0:QFN
#define BAND_MODE_40M_2ND_BELOW    1
#define BAND_MODE_40M_2ND_ABOVE    2


//new_run_flag

#define  IQ_CALIB_DONE                              (1<<0)
#define  IQ_CALIB_DONE_5G                           (1<<1)
#define  WIFI_RF_INIT_DONE                          (1<<2)
#define  WIFI_RF_INIT_DONE_5G                       (1<<3)
#define  USE_BANDWIDTH_40M                          (1<<4)
#define  USE_SGI                                    (1<<5)
#define  PLL_DLY_CAP_CALIB_DONE_5G                  (1<<6)

#define  ENABLE_DYNAMIC_FREQ_OFFSET_CALIB           (1<<8)
#define  LMAC_COEX_MODE_FDD                         (1<<10)
#define  RETRY_1M_RATE                              (1<<11)
#define  LMAC_TX_AGGR_ADJUST                        (1<<12)
//#define  ADDBA_ENHANCE    		(1<<13) //merge to 2003 fw rom
//#define  ANA_GAIN_CFG_EXT    	(1<<14)//2003 need new config
#define  USE_DUAL_BAND_TEST                         (1<<13)
#define  USE_CALIB_DATA_TEST                        (1<<14)

#define  LMAC_COEX_TDD_PS                           (1<<15)

#define  USE_B_VER                                  (1<<16)
#define  LOWER_POWER_MODE                           (1<<17)
#define  EPTA_CTS_PROT_EN                           (1<<18)
#define  FORMULA_POWERLEVEL_OFFSET_EN               (1<<19)
#define  LMAC_LOW_POWER_MODE_ANA                    (1<<20)
#define  LMAC_MASK_TRANSQ_INSCANNING                (1<<21)


#define  LMAC_SLEEP                                 (1<<24)
#define  LMAC_COEX_TDD_PS_EN                        (1<<25)
#define  LMAC_TDD_TSQ_WIN_PROCESS                   (1<<26)
#define  TEMPERATURE_OUT_OF_NORMAL_RANGE            (1<<27)
#define  LMAC_PATCH_LOG_EN                          (1<<28)
#define  LMAC_BT_TSQ_CFG_EN                         (1<<29)
#define  LMAC_WLAN_BCN_CFG_EN                       (1<<30)
#define  RCOSC_CALI_DONE                            (1<<31)
//new_run_flag end

#define WIFI_DPD_CALI_NUM_5G            14 //5g, (influence factory_section_save_dc_cali_data)
#define WIFI_DPD_CALI_NUM_2G            3  //2g, (influence factory_section_save_dc_cali_data)

#if WIFI_OUT_FEM
    #define WIFI_CONFIG_REG_5G_NUM          28 //5g
#else
    #define WIFI_CONFIG_REG_5G_NUM          26 //5g
#endif

typedef struct HOOK_CFG_S
{

    uint32  pwrOscStableTimeCont; //PWR_OSC_STABLE_TIME_COUNT
    uint32  aonPwrDurtoTsfEn; ////from reset to tsf en, in us
    uint32  BeaconMargin;
    uint32  crystal_freq;
    //uint16_t cal_val[15];
    uint32 rom_run_flag; //used 5
    /*
       bit0 : run_band; 0: 2G4, 1:5G
       bit1 : wifi_bt_coexist; 0: false, 1: true

     */

    uint32 patch_en_flag;
    uint32 patch_g_en_flag;
    uint32 patch_2001_en_flag;
    uint32 HoldOnStart;
    uint32 HoldOnInitDone; //use 10
    uint32 TransqM_tx_num;
    uint32 max_chanTimeUs;
    uint32 digGain_ext;
    uint32 log_mask;
    //add 2020-2-15
    //new_run_flag:
    //bit[7:0], 1: iQ_calib done
    //bit[15:8], 1: enable  dynamically freq_offset claib
    //bit[23:16], 1:  use b version chip
    //bit[27:24], 1:  enable lmac sleep
    //bit[28], 1:  enable patch log
    //bit[29], 1:  enable bt transq config window
    //bit[30], 1:  enable config window according to BCN
    uint32  new_run_flag; //used 15
    //tx power cal
    /*
        14: channel
        3: b /g /n
      */
    uint16  new_cal_val[44];//14*3 +1 (new_cal_val_flag)+1(reserved)
    uint32 tx_ExpireTime;

    uint32 debug_config;
    //bit[7], 1 gpio test
    //bit[0] gpio34 //block/unblock bt
    //bit[1] gpio35 //bt sniffer req
    //bit[2] gpio20 // beacon time
    uint32 logen_calib_5g_count; //40
    uint16 dly_cap_done;
    uint16 dly_cap_done_5g;

    uint16 dly_cap_ft_2g ;
    uint16 dly_cap_ft_5g; //used 42

    uint16 resvd1[40];//tx_dig_gian_calib_5g[40]; //used 62
    uint32 resvd2[4];//iq_calib_rst_5g[4];        //used 66
    uint32 resvd3[4];//dc_calib_rst_5g[4];        //used 70
    uint32 resvd4;//iq_calib_rst_2g;
    uint32 resvd5;//dc_calib_rst_2g;              //used 72

    uint16 iq_temp;
    uint16 pll_dly_cap_temp;
    uint16 gain_temp;
    uint16 init_temp;
    uint16 reg_82_2g ;
    uint16 reg_15_2g ;//used 75

    uint16 reg_82_5g ;
    uint16 reg_15_5g ;//used 76
    
    uint32  cpu_freq; //used 77

    /********* can not modify the above symbol*********/

    uint32  dig_gain_cfg_2g; //function: 2g rate-power comp    used 78
    uint32  dig_gain_cfg_5g; //function: 5g rate-power comp    used 79

    uint8_t rcosc_flt_result[4]; //function: rcosc filter result; 
                                 //0:2g 20m 0x07; 1:2g 40m 0x07; 2:5g 20m 0x2d; 3:5g 40m 0x2d    used 80
    uint8_t rcosc_adc_result[4]; //function: rcosc adc result; 
                                 //0:20m 0x64; 1:40m 0x64; 2,3:reserve                           used 81
    uint32 metal_id;                                                                           //used 82

    uint16 dig_gain_cfg_skip_ch_disable; //0: skip special channel; 1: no skip;
    uint16 switch_control_io;                                                                  //used 83

    uint32 powerlevel_offset_val_en;   //84
    uint32 powerlevel_offset_val_addr; //85

    uint16 tx_dig_gian_calib_5g[52];   //used 111

    uint32 iq_calib_rst_068_5g[WIFI_DPD_CALI_NUM_5G]; //function: 0x90138068
    uint16 dc_calib_rst_11e_5g[WIFI_DPD_CALI_NUM_5G]; //function: 0x11e
    uint16 dc_calib_rst_11f_5g[WIFI_DPD_CALI_NUM_5G]; //function: 0x11f       used 139

    uint16 iq_calib_rst_118_2g[WIFI_DPD_CALI_NUM_2G]; //function: 0x118
    uint16 iq_calib_rst_119_2g[WIFI_DPD_CALI_NUM_2G]; //function: 0x119
    uint16 dc_calib_rst_11e_2g[WIFI_DPD_CALI_NUM_2G]; //function: 0x11e
    uint16 dc_calib_rst_11f_2g[WIFI_DPD_CALI_NUM_2G]; //function: 0x11f       used 145

    uint16 wifi_lp_mode_index;
    uint16 reserv16;           //used 146

    uint32 reserved32[110];//resevred
}HOOK_CFG;



typedef  struct HOOK_TABLE_S {
    uint32_t    magic;
    HOOK_FUNC_T system_init;
    HOOK_FUNC_T post_pas_init;
    HOOK_FUNC_T post_phy_init;
    HOOK_FUNC_T rf_init;
    HOOK_FUNC_T phy_switch_channel;
    HOOK_FUNC_T phy_Rssi_Update;
    HOOK_FUNC_T HiTrnq_OutputCml;
    HOOK_FUNC_T HiTrnq_RcvHandle;
    HOOK_FUNC_T HiTrnq_Send;
    HOOK_FUNC_T HiTrnq_Sent;
    HOOK_FUNC_T OS_ProcessPowerIdle;
    HOOK_FUNC_T OS_Wakeup;
    HOOK_FUNC_T Pas_PreTX;
    HOOK_FUNC_T Pas_PostTX;
    HOOK_FUNC_T Pas_PreRX;
    HOOK_FUNC_T Pas_PostRX;
    HOOK_FUNC_T LMC_RX_Indicate;
    HOOK_FUNC_T pre_sys_init;
    HOOK_FUNC_T post_sys_init;
    HOOK_FUNC_T pre_TX_SetupFrame;
    HOOK_FUNC_T post_TX_SetupFrame;
    HOOK_FUNC_T post_TX_PM_StartScan;
    HOOK_FUNC_T post_TX_PM_StartBss;
    HOOK_FUNC_T wsm_SetFW_Cfg_Ext;
    HOOK_FUNC_T wifi_test;
    HOOK_FUNC_T pre_go_sleep;
    HOOK_FUNC_T post_go_sleep;
    HOOK_FUNC_T wifidrv_rfpll_en;
    HOOK_FUNC_T PHY_BB_Configure;
    HOOK_FUNC_T LMC_HeartBeat;
    HOOK_FUNC_T PAS_RT_GetControl01Values;
    HOOK_FUNC_T wifi_config_40m;
    HOOK_FUNC_T Setup_LongFilter_A1_OwnMac;
    HOOK_FUNC_T Adjust_Bt_Window_for_BCN;

}HOOK_TABLE_T;

struct wifi_exec_struct_t {
    unsigned int entry;
    unsigned int patch_offset;
    unsigned int sp;
};

struct wifi_bin_struct_t {
    unsigned int stack_top;
    unsigned int reset_handler;
};

typedef struct _RICTRL {
    uint32  bandwidth       :1,
            channel_band    :1,
            mode            :1,
            receive_iq_inv  :1,
            transmit_iq_inv :1,
            cal_update      :1,
            tx_filter_bypass:1,
            ri_reset        :1,
            loop_back       :1,
            rx_on           :1,
            tx_on           :1,
            aux_adc_en      :1,
            sw_ctrl_en      :1,
            sw_ctrl         :1,
            iq_ofst_inv     :1,
            rsvd            :14,
            dc_track_fix	:1,
            rx_aux_adc_en   :1,
            reg_adc_signed  :1;


} RICTRL_T;

#define BITFIELD_VALUE(_bitfield)  *((uint32 *) &(_bitfield))
#define SET_REG32_VAL(_Val,_Struct) _Val = *(uint32*)&_Struct

enum ROM_PATCH_ID {
    HITRANSQ_PROGRAMOUTPUT = 0,
    HITRANSQ_PROCESSINPUTREADY,
    HI_TRANSQHANDLEINPROCESS,
    PAS_ACTIVATEPHY,
    PAS_DOZE,
    PHY_ACTIVATEPHY,
    PHY_RESETRFIP,
    PHY_RF_CONFIGRFIP,
    RTL_COPYMEMORY,
    OS_ENABLEFIQIRQ,
    PHY_INITIALISE,
    SNIFFER_FUNC,
    ROM_PATCH_NUM
};


enum ROM_PATCH_TO_G_VER_ID {
    HI_TRANSQHANDLEINPROCESS_G = 0,
    WSM_SNIFFERHANDLER_G,
    RX_GETNEXTRXBUFFERENHABCED_G,
    HOOK_EN_G,//use modifid  hook-func  for G ver chip
    EXT_MIB_G,//extend  MIB,  rf enable, for power save
    TIMER_HANDLER_G,
    SUPPORT_5G_G,//support default
    PWR_SAVE_CTRL_G,//
    EPTA_LM_ALLOCATE_ENTRY_G,//
    EPTA_LM_UPDATE_BT_RT_INFO_G,//
    PAC_PROCESSSEQUENCEREVENTTTCSEND_G,
    PS_TXCOMPLETED_G,
    PAS_STOP_G,
    PAS_DISABLEPACPHYIF_G,
    PAS_CHANGECHANNEL_G,
    PHY_RF_TX_WRITETOGAINTBL_G,
    HI_PROGRAMINPUT_G,
    HOOK_OSPROCESSPOWERIDLE_G,
    HOOK_LMC_RX_INDICATE_G,
    CSI_ENABLE_G,
    TSF_END_ENABLE_G,
    PHY_INFO_ENABLE_G,
    USE_HW_EPTA_G,
    USE_CFG_MAX_CHAN_TIME_G,
    HI_TRANSQ_RECEIVE_G,
    TX_ADD_CHANNEL_GAIN_G,

#if 0//!MERGE_PATCH_FROM_2001
    HOOK_WIFIDRVRFSWITCHCHANNEL_G,

    HOOK_OSPROCESSPOWERIDLE_ANA_G,
    TX_SIGNAL_SNIFFER_G,
#endif
    ROM_PATCH_NUM_G
};

enum ROM_PATCH_FOR_2001_ID {
    HI_TRANSQHANDLEINPROCESS_2001 = 0,
    HI_TRANSQ_RECEIVE_2001,
    BES_EPTA_2001,
    SNIFFER_PATCH_2001,
    PHY_BB_CONFIGURE_2001,
    HT40M_SUPPORT_2001,
    SHORT_GI_2001,
    IGNORE_ERP_IE_2001,
    SNIFFER_HANDLE_2001,
    //WAKEUP_CFG_2001,
    RX_FRAME_LOG_2001,
    SNIFFER_DEAGGREGATION_2001,
    CONFIG_TRSQ_TX_2001,
    DISABLE_SORT_AGGR_TBL_2001,
    BA_EXTEND_2001,
    HI_USE_DMA_2003,
    USE_AES2_2003,
	USE_AON_SLEEP,
	NULL_FRAME_WITHOUT_ACK_2001,

    ROM_PATCH_NUM_2001,
};


#define DBG_LMAC(x)    (x)

#define DBG_ALWAYS      DBG_LMAC(1<<0)
#define DBG_I_BUILD     DBG_LMAC(1<<1)
#define DBG_HIF         DBG_LMAC(1<<3)
#define DBG_I_SCAN      DBG_LMAC(1<<4)
#define DBG_I_TX        DBG_LMAC(1<<5)
#define DBG_I_HW        DBG_LMAC(1<<6)
#define DBG_I_PS        DBG_LMAC(1<<7)
#define DBG_I_Q         DBG_LMAC(1<<9)
#define DBG_I_BA2       DBG_LMAC(1<<10)
#define DBG_I_IBSS      DBG_LMAC(1<<11)
#define DBG_I_BA3       DBG_LMAC(1<<12)
#define DBG_I_SN        DBG_LMAC(1<<13)
#define DBG_F_LMC       DBG_LMAC(1<<14)
#define DBG_I_REPLAY	DBG_LMAC(1<<15) // added for debugging replay detection with swift/wispa
#define DBG_I_BUF       DBG_LMAC(1<<17)
#define DBG_I_RATE      DBG_LMAC(1<<19)
#define DBG_I_BA        DBG_LMAC(1<<20)
#define DBG_I_ADAPWAKE  DBG_LMAC(1<<21)
#define DBG_I_TXOP      DBG_LMAC(1<<22)
#define DBG_I_EPTA      DBG_LMAC(1<<23)
#define DBG_I_LMC_EPTA  DBG_LMAC(1<<24)
#define DBG_I_TSF       DBG_LMAC(1<<25)
#define DBG_I_BEACON    DBG_LMAC(1<<26)
#define DBG_I_BSCAN     DBG_LMAC(1<<27)
#define DBG_I_PIPE      DBG_LMAC(1<<28)
#define DBG_I_P2P       DBG_LMAC(1<<29)
#define DBG_I_MINIAP    DBG_LMAC(1<<30)
#define DBG_I_AMP       DBG_LMAC(1<<31)




typedef struct FW_RUN_CFG_S
{
    uint8  run_band;
    bool use_dpd; //for 5g rf
    bool tripCapbandCal;//for 5g rf
    bool  wifiBtCoexist;
    bool eptaBypass;
    uint8  bandWidth; // 0: 20M; 1: 40M
    uint8  sgi_en; // 0: disable; 1: enable
    uint8  bandwidth_2nd_sel; // 0: 20M; 1: below; 2: above
    uint8  signal_mode_sniffer_open; // 0: no open; 1: open;
    uint8 reserved;
    //uint8 reserved;
}FW_RUN_CFG_T;

typedef struct HI_SET_FWCFG_REQ_S {
    uint16 MsgLen;
	uint16 MsgId;
	uint16 flag; // bit 0, set logmask; bit1,set crystalFeq , bit2 Transq need wakeup
    uint16 run_cfg_flag;
    uint32 run_mode; //1, test mode; 0: normal
    uint32 logMask;
   //uint16 eptaBypass;
    //uint8  TrnqWakeupEn;
    FW_RUN_CFG_T run_cfg;
    //uint8  run_band;
    uint32 crystalFeq;


}HI_SET_FWCFG_REQ;
#if 1//DUAL_BAND_TEST
typedef struct WIFI_DUAL_BAND_BACK_S {
    uint16 reg_82_2g ;
    uint16 reg_15_2g ;

    uint16 reg_82_5g ;
    uint16 reg_15_5g ;

}WIFI_DUAL_BAND_BACK;


typedef struct WIFI_DRV_GLOBAL_S {

    uint32 new_run_flag;
    uint16 use_freq ;
    uint8 band ;
    uint8 test_ch ;
    bool iq_calied ;
    //uint8 tpt_en;
    bool wifi_rfinit_done;
    bool wifi_fw_inited;
    uint8 dpd_11b_config ;
    int16 dig_gain;

    //uint16 temperature_cal_vlue;
    int16 temp_last; //
    int16 evm_temp_last ; //
    int16 iq_temp_last ; //
    int16 pll_lpf_temp_last ; //
    int16 dly_cap_temp_last ; //
    uint8 ret_dpd_calib;

    uint8 use_wifi_band;
    uint8 cali_tx_power_type;
    uint16 cali_tx_power_val[14][3];
    uint16* cali_tx_power ;
    uint16 cali_tx_power_val_5g[19][2];
    uint16* cali_tx_power_5g ;
    //uint8 cali_tx_power_type_5g;

    uint8 dly_cap_done;

    uint8 dly_cap_done_5g;

    uint16 dly_cap_ft_2g ;
    uint16 dly_cap_ft_5g;

    uint32 iq_calib_rst_2g ;
    uint32 dc_calib_rst_2g;
    uint16 reg_27 ;
    uint16 reg_32 ;
    uint16 reg_34 ;
    uint32 iq_calib_rst_5g[4];
    uint32 dc_calib_rst_5g[4];
    WIFI_DUAL_BAND_BACK dual_band;
}WIFI_DRV_GLOBAL;
#endif

struct wifi_cali_record {
    uint32_t crc32;

    //5g dc iq dpd reg
    uint32 g_dc_iq_rst_u32_5g[WIFI_DPD_CALI_NUM_5G][2];      //2:(0:0x90138068; 1:reserve)
    uint16 g_dc_iq_rst_u16_5g[WIFI_DPD_CALI_NUM_5G][2];      //2:(0:0x11e; 1:0x11f)

    short g_dpd_rst_table_5g[WIFI_DPD_CALI_NUM_5G*3][128];   //WIFI_DPD_CALI_NUM_5G:cali freq num; 3:lut_amp, lut_ph, amp

    uint16 g_dpd_rst_reg_5g[WIFI_DPD_CALI_NUM_5G];           //reg_0x5c

    uint16 frequency_cfg_5G_reg[WIFI_CONFIG_REG_5G_NUM][6];

    uint16_t g_dpd_cali_freq_5g[WIFI_DPD_CALI_NUM_5G][5];
#if(WIFI_RF_LOWER_POWER_MODE == 2)
    uint32 g_iq_line_fit_rst_u32_5g[25];                    //iq line fit value
#endif

    //2g dc iq dpd reg
    uint16 g_dc_iq_rst_u16_2g[WIFI_DPD_CALI_NUM_2G][4];      //4:(0:0x118; 1:0x119; 2:0x11e; 3:0x11f)

    short g_dpd_rst_table_2g[WIFI_DPD_CALI_NUM_2G*3][128];   //WIFI_DPD_CALI_NUM_2G:cali freq num; 3:lut_amp, lut_ph, amp

    uint16 g_dpd_rst_reg_2g[WIFI_DPD_CALI_NUM_2G];           //reg_0x5b

    uint16_t g_dpd_cali_freq_2g[WIFI_DPD_CALI_NUM_2G][5];

    //rc
    uint8_t g_rcosc_flt_result[4];//0:2g 20m 0x07; 1:2g 40m 0x07; 2:5g 20m 0x2d; 3:5g 40m 0x2d
    uint8_t g_rcosc_adc_result[4];//0:20m 0x64; 1:40m 0x64; 2,3:reserve

    //dac
    uint16 dac_i_way_result[11][2];//11:reg num; 2(0:reg; 1:reg value)
    uint16 dac_q_way_result[11][2];

    //add new record after
};

typedef struct WIFI_INIT_PARAM_S {
    uint32 coex_mode; // bit0: 0 -TDD, 1 - FDD; bit1: HYBRID
    uint8 wifi_no_sleep; //1: wifi_no_slepp, 0:wifi(include LMAC,UMAC) can go to sleep,when wifi is disconnect
    uint8 ble_adv_cts_proction; //1: ble adv send cts protection frame, 0: no protection
}WIFI_INIT_PARAM;

int recover_wifi_cali_result(void);

extern const unsigned char __wifi_patch_code_start[];
extern const unsigned char __wifi_patch_code_end[];

extern HOOK_CFG* wifi_Hook_cfg;
extern WIFI_DRV_GLOBAL g_wfdrv;


void wifi_init( uint8 band,uint8 mode);
void wifi_init_dual_band(uint8_t band,uint8_t mode);
void wifi_band_switch(uint8_t band);
void wifi_coex_config(uint32 coex_mode);
void wifi_no_sleep_config(uint8 wifi_no_sleep);
void wifi_coex_ble_adv_cts_config(uint8 cts_prot);

//void dpd_2003_config(void);
void dpd_write_table(uint16_t luk_amp[128], uint16_t luk_pha[128], uint16_t amp[128]);

void phy_2003_config(void);
void dig_config(void);
void wifi_set_agc(uint8 agc_tbl_idx);
// void wifi_agc_config(uint8 band, uint8 tbl_idx);
void lmac_update_agc_config(uint8_t tbl_idx);
//void wifi_40M_test_config(void);
//void wifi_40M_rf_config(void);
void dpd_2003_track_config(void);

// void dpd_2003_config_new_2G(uint8 freq_index, uint8 band, uint8 step, uint16 gain, uint32 len);    //dpd 2G
// void dpd_2003_config_new_5G(uint8 freq_index, uint8 band, uint8 step, uint16 gain, uint32 len); //dpd 5G
// void dpd_2003_config_new_5G_no_table(uint8 freq_index, uint8 band, uint8 step, uint16 gain, uint32 len); //dpd no table 5G

// void dpd_2003_dump_data(uint8 band, uint8 step, uint16 gain, uint32 len);

void pa_thermal_response_comp(bool enable);
void iq_dc_calib_cfg_2g(void);
void iq_dc_calib_cfg_5g(void);
void config_tx_power(uint8 band);
void config_freq_calib();
uint16_t dig_gain_compensate_by_temperature_2003(uint8_t band);

// uint16_t ana_gain_compensate_by_temperature_2003(uint8_t band, uint16 ch_id);

void bbpl_compensate_init_v1_2003(void);
uint16_t bbpl_compensate_by_temperature_v1_2003(void);
void bbpl_compensate_by_temperature_step2_v2(void);
void iq_compensate_by_temperature_v1(uint16 mode, uint16 ch_id);

void temperature_comp_data_clear_signaling_mode(void);
void temperature_comp_signaling_mode(uint8 ch_id);
void wifi_rf_temperature_protection(void);

void rf_tx_tone_on_dc(void);

void set_Fem(uint8_t *sw_matrix);
void wifi_rf_slect_matrix(int matrix_index);

void wifidrv_set_rf_tx_ionumber(int rf_tx_io);

void wifi_rcosc_cali(void);
void wifi_write_rcosc_cali_res(uint8 band, uint8 band_width);

// void wifi_dc_iq_line_fit(void);
uint32_t *wifi_get_new_run_flag();
uint32_t wifi_get_lmac_sleep_lock_bit();

#ifdef __cplusplus
}
#endif

#endif

