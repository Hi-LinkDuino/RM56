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
#ifndef __BT_DRV_H__
#define  __BT_DRV_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "cmsis.h"
#ifdef RTOS
#include "cmsis_os.h"
#endif
#include "hal_timer.h"

#include "hal_analogif.h"
#include "hal_trace.h"

#define BT_DRV_REG_OP_ENTER()    do{ uint32_t stime, spent_time; stime= hal_sys_timer_get();
#define BT_DRV_REG_OP_EXIT()     spent_time = TICKS_TO_US(hal_sys_timer_get()-stime);if (spent_time>500)TRACE(2,"%s exit, %dus",__func__, spent_time);}while(0);
#define btdrv_read_wifi_reg(reg,val)            hal_analogif_reg_read(ISPI_WFRF_REG(reg),val)
#define btdrv_write_wifi_reg(reg,val)           hal_analogif_reg_write(ISPI_WFRF_REG(reg),val)

#define SBC_PKT_TYPE_DM1   0x3
#define SBC_PKT_TYPE_2EV3  0x6
#define SBC_PKT_TYPE_2DH5  0xe

#define BT_ACL_CONHDL_BIT       (0x80)

#if defined(CHIP_BEST1400) || defined(CHIP_BEST1402) || defined(CHIP_BEST2001) || defined(CHIP_BEST2002) || defined(CHIP_BEST2003) || defined(CHIP_BEST1501) || defined(CHIP_BEST1305)
#define BTDRV_ISPI_RF_REG(reg)                  (((reg) & 0xFFF) | 0x2000)
#else
#define BTDRV_ISPI_RF_REG(reg)                  (reg)
#endif

// NOTE: Measure this value for every CHIP Version
#if defined(CHIP_BEST2300A) || defined(CHIP_BEST1305) || defined(CHIP_BEST1501) || defined(CHIP_BEST2002) || defined(CHIP_BEST2003)
#define CVSD_OFFSET_BYTES (120)
#else
#define CVSD_OFFSET_BYTES (120 - 2)
#endif

// NOTE: Measure this value for every CHIP Version
#if defined(CHIP_BEST2300A) || defined(CHIP_BEST1305) || defined(CHIP_BEST1501) || defined(CHIP_BEST2002) || defined(CHIP_BEST2003)
#define MSBC_OFFSET_BYTES (0)
#else
#define MSBC_OFFSET_BYTES (1)
#endif

#if defined(CHIP_BEST1400) || defined(CHIP_BEST1402) || defined(CHIP_BEST2300P) || defined(CHIP_BEST2300A) || defined(CHIP_BEST1305) || defined(CHIP_BEST2001) ||\
       defined(CHIP_BEST1501) || defined(CHIP_BEST2002) || defined(CHIP_BEST2003)

#define MSBC_MUTE_PATTERN (0x55)
#else
#define MSBC_MUTE_PATTERN (0x00)
#endif

#define btdrv_read_rf_reg(reg,val)              hal_analogif_reg_read(BTDRV_ISPI_RF_REG(reg),val)
#define btdrv_write_rf_reg(reg,val)             hal_analogif_reg_write(BTDRV_ISPI_RF_REG(reg),val)

#define btdrv_delay(ms)                         hal_sys_timer_delay(MS_TO_TICKS(ms))
#define btdrv_delay_us(us)                         hal_sys_timer_delay(US_TO_TICKS(us))


#define BTDIGITAL_REG(a)                        (*(volatile uint32_t *)(a))
#define BTDIGITAL_REG_WR(addr, value)         (*(volatile uint32_t *)(addr)) = (value)

#define BTDIGITAL_BT_EM(a)                      (*(volatile uint16_t *)(a))
/// Macro to write a BT control structure field (16-bit wide)
#define BTDIGITAL_EM_BT_WR(addr, value)         (*(volatile uint16_t *)(addr)) = (value)

#define BTDIGITAL_REG_SET_FIELD(reg, mask, shift, v)\
                                                do{ \
                                                    volatile unsigned int tmp = *(volatile unsigned int *)(reg); \
                                                    tmp &= ~(mask<<shift); \
                                                    tmp |= (v<<shift); \
                                                    *(volatile unsigned int *)(reg) = tmp; \
                                                }while(0)

#define BTDIGITAL_REG_GET_FIELD(reg, mask, shift, v)\
                                                do{ \
                                                    volatile unsigned int tmp = *(volatile unsigned int *)(reg); \
                                                    v = (tmp>>shift)&mask; \
                                                }while(0)

#define BTRF_REG_SET_FIELD(reg, mask, shift, v)\
                                                do{ \
                                                    unsigned short tmp; \
                                                    btdrv_read_rf_reg(reg,&tmp); \
                                                    tmp &= ~(mask<<shift); \
                                                    tmp |= (v<<shift); \
                                                    btdrv_write_rf_reg(reg,tmp); \
                                                }while(0)

#define BTRF_REG_GET_FIELD(reg, mask, shift, v)\
                                                do{ \
                                                    unsigned short tmp; \
                                                    btdrv_read_rf_reg(reg,&tmp); \
                                                    v = (tmp>>shift)&mask; \
                                                }while(0)

#define getbit(x,y)   ((x) >> (y)&1)

#define BT_DRV_DEBUG  1
#if BT_DRV_DEBUG
#define BT_DRV_TRACE(n, fmt, ...) TRACE(n, fmt, ##__VA_ARGS__)
#define BT_DRV_DUMP(s,buff,len) DUMP8(s,buff,len)
#ifndef TOTA_CRASH_DUMP_TOOL_ENABLE
#define BT_DRV_TRACE_CRASH_DUMP(n, fmt, ...)  TRACE(n, fmt, ##__VA_ARGS__)
#else
#define BT_DRV_TRACE_CRASH_DUMP(n, fmt, ...)  REL_TRACE_IMM(n, fmt, ##__VA_ARGS__)
#endif
#else
#define BT_DRV_TRACE(n, fmt, ...) hal_trace_dummy(NULL, ##__VA_ARGS__)
#define BT_DRV_DUMP(s,buff,len)
#define BT_DRV_TRACE_CRASH_DUMP(n, fmt, ...) hal_trace_dummy(NULL, ##__VA_ARGS__)

#endif

#define HCI_HOST_NB_CMP_PKTS_CMD_OPCODE         0x0C35
#define HCI_NB_CMP_PKTS_EVT_CODE                0x13
#define MAX_NB_SYNC (2)

#if defined(CHIP_BEST2300) || defined(__FPGA_BT_2300__)
#define BT_EM_ADDR_BASE (0xD021114A)
#define BT_EM_SIZE (110)
#define BLE_EM_CS_SIZE      (90)
#define EM_BT_PWRCNTL_ADDR (BT_EM_ADDR_BASE + 0x16)
#define EM_BT_BT_EXT1_ADDR (BT_EM_ADDR_BASE + 0x66)
#define EM_BT_BITOFF_ADDR   (BT_EM_ADDR_BASE + 0x02)
#define EM_BT_CLKOFF0_ADDR   (BT_EM_ADDR_BASE + 0x04)
#define EM_BT_CLKOFF1_ADDR   (BT_EM_ADDR_BASE + 0x06)
#define EM_BT_WINCNTL_ADDR   (BT_EM_ADDR_BASE + 0x1A)
#define EM_BT_RXBIT_ADDR    (BT_EM_ADDR_BASE + 0x56)
#define EM_BT_BTADDR1_ADDR    (BT_EM_ADDR_BASE + 0xC)
#define EM_BT_LINKCNTL_ADDR    (BT_EM_ADDR_BASE + 0x8)
#define EM_BT_RXDESCCNT_ADDR    (BT_EM_ADDR_BASE + 0x5A)

#define EM_BT_AUDIOBUF_OFF    0xd021449c
#define EM_BT_RXACLBUFPTR_ADDR  0xd02115a0
#define REG_EM_BT_RXDESC_SIZE 14

#define LBRT_TX_PWR_FIX     (3)
#define BT_CONTROLLER_CRASH_DUMP_ADDR_BASE  (0xc0000050)
#define DEFAULT_XTAL_FCAP                       0x8080

#define BT_ERRORTYPESTAT_ADDR   (0xd0220060)
#define MAX_NB_ACTIVE_ACL                   (3)

#elif defined(CHIP_BEST2300P) || defined(CHIP_BEST2300A) || defined(CHIP_BEST1400) || defined(CHIP_BEST1402) || defined(__FPGA_BT_1400__) || defined(CHIP_BEST2001) || defined(CHIP_BEST1305) || defined(CHIP_BEST1600SIMU)
#define BT_EM_ADDR_BASE (0xD02111A2)
#define BT_EM_SIZE (110)
#define BLE_EM_CS_SIZE      (90)
#define EM_BT_PWRCNTL_ADDR (BT_EM_ADDR_BASE + 0x16)
#define EM_BT_BT_EXT1_ADDR (BT_EM_ADDR_BASE + 0x66)
#define EM_BT_BT_EXT2_ADDR (BT_EM_ADDR_BASE + 0x68)
#define EM_BT_BITOFF_ADDR   (BT_EM_ADDR_BASE + 0x02)
#define EM_BT_CLKOFF0_ADDR   (BT_EM_ADDR_BASE + 0x04)
#define EM_BT_CLKOFF1_ADDR   (BT_EM_ADDR_BASE + 0x06)
#define EM_BT_WINCNTL_ADDR   (BT_EM_ADDR_BASE + 0x1A)
#define EM_BT_RXBIT_ADDR    (BT_EM_ADDR_BASE + 0x56)
#define EM_BT_BTADDR1_ADDR    (BT_EM_ADDR_BASE + 0xC)
#define EM_BT_LINKCNTL_ADDR    (BT_EM_ADDR_BASE + 0x8)
#define BLE_CRCINIT1_ADDR (0xd02100c2)
#define EM_BT_RXDESCCNT_ADDR    (BT_EM_ADDR_BASE + 0x5A)

#define BLE_MAXEVTIME_ADDR  (0xd02100d0)

#define EM_BT_AUDIOBUF_OFF    0xd02144fc
#define EM_BT_RXACLBUFPTR_ADDR  0xd02115f8
#define REG_EM_BT_RXDESC_SIZE 16

#define LBRT_TX_PWR_FIX     (3)
#define BT_CONTROLLER_CRASH_DUMP_ADDR_BASE  (0xc0000050)
#define BT_ERRORTYPESTAT_ADDR   (0xd0220060)
#define MAX_NB_ACTIVE_ACL                   (3)

#if defined(CHIP_BEST2300A) || defined(CHIP_BEST1305)
#define DEFAULT_XTAL_FCAP                       0x80ad      //8pf crstal No cap by luobin
#else
#define DEFAULT_XTAL_FCAP                       0x8080
#endif

#elif defined(__FPGA_BT_1500__) || defined(CHIP_BEST1501) || defined(CHIP_BEST2002) || defined(CHIP_BEST2003)
#if defined(CHIP_BEST1501SIMU) || defined(CHIP_BEST1600SIMU)
#ifdef __FPGA_1501P__
#define BT_EM_ADDR_BASE (0xD0216F34)
#else
#define BT_EM_ADDR_BASE (0xD0216800)
#endif//fpga_1501p
#else
#if defined(CHIP_BEST2003) && defined __BT_RAMRUN__
#define BT_EM_ADDR_BASE (0xD0215ee0)
#else
#define BT_EM_ADDR_BASE (0xD0215000)
#endif

#endif
#define BT_EM_SIZE (104)
#define BLE_EM_CS_SIZE      (112)
#define EM_BLE_CS_OFFSET    (0x178)
#define EM_BT_PWRCNTL_ADDR (BT_EM_ADDR_BASE + 0x14)
#define EM_BT_BT_EXT1_ADDR (BT_EM_ADDR_BASE + 0x60)
//#define EM_BT_BITOFF_ADDR  //1501 don't have bitoff reg
#define EM_BT_CLKOFF0_ADDR   (BT_EM_ADDR_BASE + 0x02)
#define EM_BT_CLKOFF1_ADDR   (BT_EM_ADDR_BASE + 0x04)
#define EM_BT_WINCNTL_ADDR   (BT_EM_ADDR_BASE + 0x18)
#define EM_BT_RXCLKN0_ADDR   (BT_EM_ADDR_BASE + 0x52)
#define EM_BT_RXCLKN1_ADDR   (BT_EM_ADDR_BASE + 0x54)
#define EM_BT_RXBIT_ADDR    (BT_EM_ADDR_BASE + 0x56)
#define EM_BT_BTADDR1_ADDR    (BT_EM_ADDR_BASE + 0x8)
#define EM_BT_LINKCNTL_ADDR    (BT_EM_ADDR_BASE + 0x6)
#define EM_BT_RXDESCCNT_ADDR    (BT_EM_ADDR_BASE + 0x5A)
#define BLE_MAXEVTIME_ADDR   (0xD0210030 + EM_BLE_CS_OFFSET)
#define BLE_CRCINIT1_ADDR    (0xD0210014 + EM_BLE_CS_OFFSET)

#define EM_BT_AUDIOBUF_OFF    0xd0218a70
#define EM_BT_RXACLBUFPTR_ADDR  0xD0215482
#define REG_EM_BT_RXDESC_SIZE 20

#define LBRT_TX_PWR_FIX     (3)
#define BT_CONTROLLER_CRASH_DUMP_ADDR_BASE  (0xc0004050)
#define BT_ERRORTYPESTAT_ADDR   (0xd0220460)
#define DEFAULT_XTAL_FCAP                       0x1300
#define MAX_NB_ACTIVE_ACL                   (4)
#else
#define BT_EM_ADDR_BASE (0xd0210190)
#define BT_EM_SIZE (96)
#define BT_CONTROLLER_CRASH_DUMP_ADDR_BASE  (0xc00064cc)
#endif


#if defined(__FPGA_BT_1500__) || defined(CHIP_BEST1501) || defined(CHIP_BEST2002) || defined(CHIP_BEST2003)
//bt max slot clock
#define MAX_SLOT_CLOCK      ((1L<<28) - 1)
#else
//bt max slot clock
#define MAX_SLOT_CLOCK      ((1L<<27) - 1)
#endif
 // A slot is 625 us
#define SLOT_SIZE           625
#define HALF_SLOT_SIZE       625                    // A half slot is 312.5 us (in half us)

//#define __PASS_CI_TEST_SETTING__
#define  BT_LOW_POWER_MODE  1
#define  BT_HIGH_PERFORMANCE_MODE   2
//#define __ENABLE_LINK_POWER_CONTROL__

#define BT_RFMODE BT_LOW_POWER_MODE
//#define BT_RFMODE BT_HIGH_PERFORMANCE_MODE
//#define BT_50_FUNCTION

#define BT_POWERON            1
#define BT_POWEROFF           0

/// 8 bit access types
#define _8_Bit                              8
/// 16 bit access types
#define _16_Bit                             16
/// 32 bit access types
#define _32_Bit                             32

#define BTDRV_PATCH_WRITING  0x0
#define BTDRV_PATCH_DONE         0x1

#define BTDRV_CFG_WRITING   0x0
#define BTDRV_CFG_DONE      0x1

#define HCI_DBG_RD_MEM_CMD_OPCODE                     0xFC01
#define HCI_DBG_WR_MEM_CMD_OPCODE                     0xFC02
#define HCI_DBG_DEL_PAR_CMD_OPCODE                    0xFC03
#define HCI_DBG_FLASH_ID_CMD_OPCODE                   0xFC05
#define HCI_DBG_FLASH_ER_CMD_OPCODE                   0xFC06
#define HCI_DBG_FLASH_WR_CMD_OPCODE                   0xFC07
#define HCI_DBG_FLASH_RD_CMD_OPCODE                   0xFC08
#define HCI_DBG_RD_PAR_CMD_OPCODE                     0xFC09
#define HCI_DBG_WR_PAR_CMD_OPCODE                     0xFC0A
#define HCI_DBG_WLAN_COEX_CMD_OPCODE                  0xFC0B
#define HCI_DBG_ENTER_TEST_MODE_CMD_OPCODE            0xFC0C
#define HCI_DBG_WLAN_COEXTST_SCEN_CMD_OPCODE          0xFC0D
#define HCI_DBG_SEND_LMP_CMD_OPCODE                   0xFC0E
#define HCI_DBG_WR_SYNC_DATA_CFG_CMD_OPCODE           0xFC0F
#define HCI_DBG_RD_KE_STATS_CMD_OPCODE                0xFC10
#define HCI_DBG_PLF_RESET_CMD_OPCODE                  0xFC11
#define HCI_DBG_RD_MEM_INFO_CMD_OPCODE                0xFC12
#define HCI_DBG_EMUL_TESTER_CMD_OPCODE                0xFC2D
#define HCI_DBG_SCATT_IMPROV_CMD_OPCODE               0xFC2E
#define HCI_DBG_RF_REG_RD_CMD_OPCODE                  0xFC39
#define HCI_DBG_RF_REG_WR_CMD_OPCODE                  0xFC3A
#define HCI_DBG_HW_REG_RD_CMD_OPCODE                  0xFC30
#define HCI_DBG_HW_REG_WR_CMD_OPCODE                  0xFC31
#define HCI_DBG_SET_BD_ADDR_CMD_OPCODE                0xFC32
#define HCI_DBG_SET_TYPE_PUB_CMD_OPCODE               0xFC33
#define HCI_DBG_SET_TYPE_RAND_CMD_OPCODE              0xFC34
#define HCI_DBG_SET_CRC_CMD_OPCODE                    0xFC35
#define HCI_DBG_LLCP_DISCARD_CMD_OPCODE               0xFC36
#define HCI_DBG_RESET_RX_CNT_CMD_OPCODE               0xFC37
#define HCI_DBG_RESET_TX_CNT_CMD_OPCODE               0xFC38
#define HCI_DBG_SET_TX_PW_CMD_OPCODE                  0xFC3B
#define HCI_DBG_SET_SYNCWORD_CMD_OPCODE               0xFC3C


//encrytion min and max key size
#define HCI_DBG_SET_ENCRYPTION_KEY_SIZE_CMD_OPCODE    0xFC41
//preferred key type default :combine key
#define HCI_DBG_SET_PREFERRED_KEY_TYPE_CMD_OPCODE     0xFC42
//creat unit key
#define HCI_DBG_CREAT_UNIT_KEY_CMD_OPCODE             0xFC43
//set clk drift and jitter
#define HCI_DBG_SET_LPCLK_DRIFT_JITTER_CMD_OPCODE     0xFC44

//change uart buadrate
#define HCI_DBG_CHANGE_UART_BAUDRATE_CMD_OPCODE       0xFC46

//set sleep enable and external wakeup enable
#define HCI_DBG_SET_SLEEP_EXWAKEUP_EN_CMD_OPCODE      0xFC47

//set private key
#define HCI_DBG_SET_SP_PRIVATE_KEY_CMD_OPCODE         0xFC48

//set public key
#define HCI_DBG_SET_SP_PUBLIC_KEY_CMD_OPCODE          0xFC49

//set errdata adopted
#define HCI_DBG_SET_ERRDATA_ADOPTED_CMD_OPCODE        0xFC4A

//set basic threshold
#define HCI_DBG_SET_BASIC_THRESHOLD_CMD_OPCODE        0xFC4B

//set edr threshold
#define HCI_DBG_SET_EDR_THRESHOLD_CMD_OPCODE          0xFC4C

//set basic algorithm
#define HCI_DBG_SET_BASIC_ALGORITHM_CMD_OPCODE        0xFC4D

//set edr alorithm
#define HCI_DBG_SET_EDR_ALGORITHM_CMD_OPCODE          0xFC4E

//set basic packet lut
#define HCI_DBG_SET_BASIC_PKT_LUT_CMD_OPCODE          0xFC4F

//set edr packet lut
#define HCI_DBG_SET_EDR_PKT_LUT_CMD_OPCODE            0xFC50

//set diag_bt_hw
#define HCI_DBG_SET_DIAG_BT_HW_CMD_OPCODE             0xFC54
//set diag ble hw
#define HCI_DBG_SET_DIAG_BLE_HW_CMD_OPCODE            0xFC55
//set diag sw
#define HCI_DBG_SET_DIAG_SW_CMD_OPCODE                0xFC56

//set ble channel assessment parameter
#define HCI_DBG_SET_BLE_CA_PARA_CMD_OPCODE            0xFC57

//set  ble rf timing

//set ble rf timig

//set ble rl size
#define HCI_DBG_SET_RL_SIZE_CMD_OPCODE                0xFC5D

///set hostwake
#define HCI_DBG_SET_HOSTWAKE_CMD_OPCODE               0xFC5E
//reserved for VCO test
#define HCI_DBG_BT_VCO_TEST_CMD_OPCODE                0xFCAA

#if defined(CHIP_BEST2300) || defined(CHIP_BEST2300P) || \
    defined(CHIP_BEST2300A) || defined(CHIP_BEST2001) || \
    defined(CHIP_BEST1400) || defined(CHIP_BEST1402) || \
    defined(__FPGA_BT_2300__) ||defined(__FPGA_BT_1400__) || \
    defined(__FPGA_BT_1500__) || defined(CHIP_BEST1501) || defined(CHIP_BEST2003) || \
    defined(CHIP_BEST2002) || defined(CHIP_BEST1305) || defined(CHIP_BEST1600SIMU)

///set dle dft value
#define HCI_DBG_WR_DLE_DFT_VALUE_CMD_OPCODE           0xFC41

//set exernal wake up time oscillater wakeup time and radio wakeup time
#define HCI_DBG_SET_WAKEUP_TIME_CMD_OPCODE            0xFC71

#define HCI_DBG_SET_SLEEP_SETTING_CMD_OPCODE          0xFC77

//bt address not ble address
#define HCI_DBG_SET_BT_ADDR_CMD_OPCODE                0xFC72
//set pcm setting
#define HCI_DBG_SET_PCM_SETTING_CMD_OPCODE            0xFC74

#define HCI_DBG_SET_RSSI_THRHLD_CMD_OPCODE            0xFC76


//set sync buff size
#define HCI_DBG_SET_SYNC_BUF_SIZE_CMD_OPCODE          0xFC7F
//set afh algorithm
#define HCI_DBG_SET_AFH_ALGORITHM_CMD_OPCODE          0xFC80
//set local feature
#define HCI_DBG_SET_LOCAL_FEATURE_CMD_OPCODE          0xFC81

#define HCI_DBG_SET_BT_RF_TIMING_CMD_OPCODE           0xFC83

//set local extend feature
#define HCI_DBG_SET_LOCAL_EX_FEATURE_CMD_OPCODE       0xFC82

#define HCI_DBG_SET_2300_BT_RF_TIMING_CMD_OPCODE           0xFC83


#define HCI_DBG_SET_BLE_RF_TIMING_CMD_OPCODE  0xfc84

//bt setting interface
#define HCI_DBG_SET_BT_SETTING_CMD_OPCODE             0xFC86
// xiao add for nonsignaling test mode
#define HCI_DBG_NONSIG_TESTER_SETUP_CMD_OPCODE        0xFC87
/* xiao add for custom set param*/
#define HCI_DBG_SET_CUSTOM_PARAM_CMD_OPCODE           0xFC88

#define HCI_DBG_SET_SCO_SWITCH_CMD_OPCODE             0xFC89

#define HCI_DBG_SET_SNIFFER_ENV_CMD_OPCODE            0xFC8E
//set sco path
#define HCI_DBG_SET_SYNC_CONFIG_CMD_OPCODE            0xFC8F

#define HCI_DBG_START_TWS_EXCHANGE_CMD_OPCODE         0xFC91

#define HCI_DBG_BTADDR_EXCHANGE_CMD_OPCODE            0xFC92

#define HCI_DBG_SEND_DATA_TO_PEER_DEV_CMD_OPCODE      0xFC93

#define HCI_DBG_SCO_TX_SILENCE_CMD_OPCODE             0xFC94

#define HCI_DBG_SNIFFER_CMD_OPCODE                    0xFC95

#define HCI_DBG_CSB_UPDATE_LINK_PARAM_CMD_OPCODE      0xFC96

#define HCI_DBG_SET_LINK_LBRT_CMD_OPCODE              0xFC97

#define HCI_DBG_SET_CON_SLV_BCST_DATA_CMD_OPCODE      0xFC98
#define HCI_DBG_SET_ROLE_SWITCH_INSTANT_CMD_OPCODE    0xFC99
#define HCI_DBG_LOW_LAYER_METRICS_CMD_OPCODE          0xFC9B
#define HCI_DBG_LMP_MESSAGE_RECORD_CMD_OPCODE         0xFC9C


#define HCI_DBG_SET_LOCAL_FEATURE_50_CMD_OPCODE       0xFC72
#define HCI_DBG_SET_SLEEP_SETTING_50_CMD_OPCODE       0xFC70
#define HCI_DBG_SET_BT_SETTING_50_CMD_OPCODE          0xFC71

#define HCI_DBG_SET_CUSTOM_PARAM_50_CMD_OPCODE        0xFC73
#define HCI_DBG_SET_FIX_TWS_INTERVAL_CMD_OPCODE       0xFCAB
#define HCI_DBG_SET_BT_SETTING_EXT1_CMD_OPCODE        0xFCAE
#define HCI_DBG_SET_BT_TWS_LINK_CMD_OPCODE            0xFCAF
#define HCI_DBG_SET_AFH_FOLLOW_CMD_OPCODE             0xFCB0
#define HCI_DBG_SET_RF_RX_GAIN_THS_TBL_CMD_OPCODE     0xFCB1
#define HCI_DBG_SET_RF_RX_GAIN_FIXED_CMD_OPCODE       0xFCB2

#if defined(CHIP_BEST1501) ||defined(CHIP_BEST2002) || defined(CHIP_BEST2003)
#define HCI_DBG_SET_RSSI_TX_POWER_DFT_THR_CMD_OPCODE     0xFCB3
#define HCI_DBG_SET_RSSI_TX_POWER_LINK_THR_CMD_OPCODE    0xFCB4
#define HCI_DBG_SET_CLK_DRAGGING_CMD_OPCODE              0xFCB5
#define HCI_DBG_SET_BLE_TXPWR_CMD_OPCODE                 0xFCB6
#define HCI_DBG_SET_BT_BLE_PARA_CMD_OPCODE               0xFCB7
#define HCI_DBG_SET_FUNC_PATCH_CMD_OPCODE                0xFCB8
//#define HCI_DBG_SET_IBRT_TEST_MODE_CMD_OPCODE            0xfcb9
//#define HCI_DBG_SET_ECC_DATA_TEST_CMD_OPCODE             0xfcbb
#define HCI_DBG_SET_IBRT_DATA_TEST_CMD_OPCODE            0xfcbc
#define HCI_DBG_GET_TX_RX_BUF_CNT_CMD_OPCODE             0xfcbd
//#define HCI_DBG_IBRT_SWITCH_CMD_OPCODE                 0xfcbe
//#define HCI_DBG_STOP_MULTI_POINT_IBRT_CMD_OPCODE       0xfcbf
#define HCI_DBG_SET_TXPWR_MODE_CMD_OPCODE                0xfcc0
#define HCI_DBG_SET_LOW_LATENCY_MODE_CMD_OPCODE          0xfcc1
#define HCI_DBG_SET_SW_RSSI_CMD_OPCODE                   0xfcc2
#define HCI_DBG_SET_BT_SCHE_SETTING_CMD_OPCODE           0xfcc3
#define HCI_DBG_SET_BT_IBRT_SETTING_CMD_OPCODE           0xfcc4
#define HCI_DBG_SET_BT_HW_FEAT_SETTING_CMD_OPCODE        0xfcc5
#define HCI_DBG_SIGNAL_TEST_ACTIVATE_CMD_OPCODE          0xfcc6
#define HCI_DBG_SIGNAL_TEST_CONTROL_CMD_OPCODE           0xfcc7
#define HCI_DBG_SIGNAL_TEST_PWR_CTRL_REQ_CMD_OPCODE      0xfcc8
#ifndef HCI_DBG_SET_TRANSQ_FLAG_CMD_OPCODE
#define HCI_DBG_SET_TRANSQ_FLAG_CMD_OPCODE               0xfcc9//transq flag
#endif
#define HCI_DBG_BT_COMMON_SETTING_T1_CMD_OPCODE          0xfcca
#define HCI_DBG_SET_AFH_ASSESS_CMD_OPCODE                0xfccb
#define HCI_DBG_BT_COMMON_SETTING_T2_CMD_OPCODE          0xfccf //bt setting interface
#define HCI_DBG_SET_BT_BLE_TXRX_GAIN_CMD_OPCODE          0xfccd
#define HCI_DBG_BLE_SCAN_RXGAIN_CHANGED_CMD_OPCODE       0xfcd0
#else //!defined(CHIP_BEST1501)
#define HCI_DBG_SET_BT_SETTING_EXT2_CMD_OPCODE        0xFCB3
#define HCI_DBG_SET_IBRT_TEST_MODE_CMD_OPCODE         0xFCB4
#define HCI_DBG_SET_BT_LOCAL_CLK_CMD_OPCODE           0xFCB5
#define HCI_DBG_SET_FUNC_PATCH_CMD_OPCODE             0xFCB8
#define HCI_DBG_SET_TXPWR_MODE_CMD_OPCODE             0xFCB9
#define HCI_DBG_SET_SW_RSSI_CMD_OPCODE                0xFCBA
#define HCI_DBG_SET_ECC_DATA_TEST_CMD_OPCODE          0xFCBB
#define HCI_DBG_SET_IBRT_DATA_TEST_CMD_OPCODE         0xFCBC
#define HCI_DBG_SET_NWINSZ_RXGRN_TO_CMD_OPCODE        0xFCBD
#define HCI_DBG_ENABLE_SOFTBIT_CMD_OPCODE             0xFCBE
#define HCI_DBG_SET_BT_SCHE_SETTING_CMD_OPCODE        0xFCC3
#define HCI_DBG_SET_BT_IBRT_SETTING_CMD_OPCODE        0xFCC4
#define HCI_DBG_SET_BT_HW_FEAT_SETTING_CMD_OPCODE     0xFCC5
#endif//defined(CHIP_BEST1501)

#if defined(CHIP_BEST1305) || defined(CHIP_BEST1600SIMU)
#define HCI_DBG_SET_BT_SETTING_EXT3_CMD_OPCODE        0xfcbf
#endif

#else
//bt address not ble address
#define HCI_DBG_SET_BT_ADDR_CMD_OPCODE                0xFC40
//set exernal wake up time oscillater wakeup time and radio wakeup time
#define HCI_DBG_SET_WAKEUP_TIME_CMD_OPCODE            0xFC45
//set sco path
#define HCI_DBG_SET_SYNC_CONFIG_CMD_OPCODE            0xFC51
//set pcm setting
#define HCI_DBG_SET_PCM_SETTING_CMD_OPCODE            0xFC52
//set sync buff size
#define HCI_DBG_SET_SYNC_BUF_SIZE_CMD_OPCODE          0xFC53
//set local feature
#define HCI_DBG_SET_LOCAL_FEATURE_CMD_OPCODE          0xFC59
//bt setting interface
#define HCI_DBG_SET_BT_SETTING_CMD_OPCODE             0xFC5F
//set afh algorithm
#define HCI_DBG_SET_AFH_ALGORITHM_CMD_OPCODE          0xFC58
//set local extend feature
#define HCI_DBG_SET_LOCAL_EX_FEATURE_CMD_OPCODE       0xFC5A

#define HCI_DBG_SET_BT_RF_TIMING_CMD_OPCODE           0xFC5B


#define HCI_DBG_SET_BLE_RF_TIMING_CMD_OPCODE          0xFC5C


// xiao add for nonsignaling test mode
#define HCI_DBG_NONSIG_TESTER_SETUP_CMD_OPCODE        0xFC60
/* xiao add for custom set param*/
#define HCI_DBG_SET_CUSTOM_PARAM_CMD_OPCODE           0xFC61

#define HCI_DBG_SET_SCO_SWITCH_CMD_OPCODE             0xFC62

#define HCI_DBG_SET_SNIFFER_ENV_CMD_OPCODE            0xFC67

#define HCI_DBG_START_TWS_EXCHANGE_CMD_OPCODE         0xFC69

#define HCI_DBG_BTADDR_EXCHANGE_CMD_OPCODE            0xFC6A

#define HCI_DBG_SEND_DATA_TO_PEER_DEV_CMD_OPCODE      0xFC6B

#define HCI_DBG_SCO_TX_SILENCE_CMD_OPCODE             0xFC6C

#define HCI_DBG_SNIFFER_CMD_OPCODE                    0xFC6D

#define HCI_DBG_BT_VCO_TEST_CMD_OPCODE                0xFCAA

// Only new controller IP has this funciton,it is a error opcode
#define HCI_DBG_SET_ROLE_SWITCH_INSTANT_CMD_OPCODE    0xFCFF
#define HCI_DBG_SET_LINK_LBRT_CMD_OPCODE              0xFCFF
#define HCI_DBG_SET_BT_SETTING_EXT1_CMD_OPCODE        0xFCFF
#define HCI_DBG_SET_BT_TWS_LINK_CMD_OPCODE            0xFCFF
#define HCI_DBG_SET_AFH_FOLLOW_CMD_OPCODE             0xFCFF
#define HCI_DBG_SET_RF_RX_GAIN_THS_TBL_CMD_OPCODE     0xFCFF
#define HCI_DBG_SET_RF_RX_GAIN_FIXED_CMD_OPCODE       0xFCFF
#define HCI_DBG_SET_BT_SETTING_EXT2_CMD_OPCODE        0xFCFF
#define HCI_DBG_SET_IBRT_TEST_MODE_CMD_OPCODE         0xFCFF
#define HCI_DBG_SET_BT_LOCAL_CLK_CMD_OPCODE           0xFCFF
#define HCI_DBG_SET_FUNC_PATCH_CMD_OPCODE             0xFCFF
#define HCI_DBG_SET_TXPWR_MODE_CMD_OPCODE             0xFCFF
#define HCI_DBG_SET_SW_RSSI_CMD_OPCODE                0xFCFF
#define HCI_DBG_SET_ECC_DATA_TEST_CMD_OPCODE          0xFCFF
#define HCI_DBG_SET_IBRT_DATA_TEST_CMD_OPCODE         0xFCFF
#define HCI_DBG_SET_NWINSZ_RXGRN_TO_CMD_OPCODE        0xFCFF
#define HCI_DBG_ENABLE_SOFTBIT_CMD_OPCODE             0xFCFF

#endif // 2300, 2300p, 1400,1402,1501

extern void btdrv_poweron(uint8_t en);
extern void btdrv_hciopen(void);
#ifdef __cplusplus
}
#endif

#endif

