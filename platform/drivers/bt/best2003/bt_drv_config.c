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
#include <besbt_string.h>
#include <string.h>
#include "plat_types.h"
#include "tgt_hardware.h"
#include "hal_i2c.h"
#include "hal_uart.h"
#include "bt_drv.h"
#include "hal_timer.h"
#include "hal_chipid.h"
#include "hal_psc.h"
#include "bt_drv_2003_internal.h"
#include "bt_drv_interface.h"
#include "bt_drv_reg_op.h"
#include "bt_drv_internal.h"
#ifdef __BT_RAMRUN__
#include "bt_drv_ramrun_symbol_2003_t0.h"
#else
#include "bt_drv_symbol_2003_t0.h"
#include "bt_drv_symbol_2003_t1.h"
#include "bt_drv_symbol_2003_t2.h"
#include "bt_drv_symbol_2006_t0.h"
#endif
#include "factory_section.h"
extern void btdrv_send_cmd(uint16_t opcode,uint8_t cmdlen,const uint8_t *param);
extern void btdrv_write_memory(uint8_t wr_type,uint32_t address,const uint8_t *value,uint8_t length);

typedef struct
{
    uint8_t is_act;
    uint16_t opcode;
    uint8_t parlen;
    const uint8_t *param;

} BTDRV_CFG_TBL_STRUCT;


#define BTDRV_CONFIG_ACTIVE   1
#define BTDRV_CONFIG_INACTIVE 0
#define BTDRV_INVALID_TRACE_LEVEL  0xFF
/*
[0][0] = 63, [0][1] = 0,[0][2] = (-80),           472d
[1][0] = 51, [2][1] = 0,[2][2] = (-80),          472b
[2][0] = 42, [4][1] = 0,[4][2] = (-75),           4722
[3][0] = 36, [6][1] = 0,[6][2] = (-55),           c712
[4][0] = 30, [8][1] = 0,[8][2] = (-40),           c802
[5][0] = 21,[10][1] = 0,[10][2] = 0x7f,         c102
[6][0] = 12,[11][1] = 0,[11][2] = 0x7f,       c142
[7][0] = 3,[13][1] = 0,[13][2] = 0x7f,        c1c2
[8][0] = -3,[14][1] = 0,[14][2] = 0x7f};      c0c2
*/
static uint8_t g_controller_trace_level = BTDRV_INVALID_TRACE_LEVEL;
static bool  g_lmp_trace_enable = false;
const int8_t btdrv_rf_env_2003[]=
{
    0x01,0x00,  //rf api
    0x01,   //rf env
    185,     //rf length
#ifdef PWR_CTRL_8DBM
    0x3,     //txpwr_max
#else
    0x5,     //txpwr_max
#endif
    -1,    ///rssi high thr
    -2,   //rssi low thr
    -100,  //rssi interf thr
    0xf,  //rssi interf gain thr
    2,  //wakeup delay
    0xe, 0, //skew
    0xe8,0x3,    //ble agc inv thr
#ifdef __HW_AGC__
    0x1,//hw_sw_agc_flag
#else
    0x0,//
#endif
    0xff,//sw gain set
    0xff,    //sw gain set
    -85,//bt_inq_page_iscan_pscan_dbm
    0x7f,//ble_scan_adv_dbm
    1,      //sw gain reset factor
    1,    //bt sw gain cntl enable
    1,   //ble sw gain cntl en
    1,  //bt interfere  detector en
    0,  //ble interfere detector en

    0,0,0,
    3,3,12,
    6,6,28,
    9,9,28,
    12,12,28,
    15,15,28,
    18,18,28,
    21,21,28,
    0x7f,24,0x7f,
    0x7f,27,0x7f,
    0x7f,30,0x7f,
    0x7f,33,0x7f,
    0x7f,36,0x7f,
    0x7f,39,0x7f,
    0x7f,42,0x7f,  //rx hwgain tbl ptr

#ifdef BT_IF_750K
    56,0,-80,
    49,0,-80,
    38,0,-80,
    32,0,-80,
    30,0,-80,
    24,0,-80,
    14,0,-80,
    0,0,-80,
#elif defined(BT_IF_1P05M)
    56,0,-80,
    53,0,-80,
    51,0,-80,
    39,0,-80,
    36,0,-80,
    30,0,-80,
    5,0,-80,
    -6, 0,-80,
#endif

    0x7f,0x7f,0x7f,
    0x7f,0x7f,0x7f,
    0x7f,0x7f,0x7f,
    0x7f,0x7f,0x7f,
    0x7f,0x7f,0x7f,
    0x7f,0x7f,0x7f,
    0x7f,0x7f,0x7f,  //rx gain tbl ptr

    -85,-85,
    -82,-82,
    -73,-73,
    -69,-69,
    -63,-63,
    -40,-40,
    -23,-23,
    0x7f,0x7f,

    //  0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,    //rx gain ths tbl ptr

    0,0,
    0,0,
    0,0,
    0,0,
    0,0,
    0,1,
    0,2,
    0,2,
    0,2,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,    //flpha filter factor ptr
    -23,-20,-17,-14,-11,-8,-5,-2,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,   //tx pw onv tbl ptr
};

const int8_t btdrv_rf_env_2006[]=
{
    0x01,0x00,  //rf api
    0x01,   //rf env
    185,     //rf length
#ifdef PWR_CTRL_8DBM
    0x3,     //txpwr_max
#else
    0x5,     //txpwr_max
#endif
    -1,    ///rssi high thr
    -2,   //rssi low thr
    -100,  //rssi interf thr
    0xf,  //rssi interf gain thr
    2,  //wakeup delay
    0xe, 0, //skew
    0xe8,0x3,    //ble agc inv thr
#ifdef __HW_AGC__
    0x1,//hw_sw_agc_flag
#else
    0x0,//
#endif
    0xff,//sw gain set
    0xff,    //sw gain set
    -85,//bt_inq_page_iscan_pscan_dbm
    0x7f,//ble_scan_adv_dbm
    1,      //sw gain reset factor
    1,    //bt sw gain cntl enable
    1,   //ble sw gain cntl en
    1,  //bt interfere  detector en
    0,  //ble interfere detector en

    0,0,0,
    3,3,12,
    6,6,28,
    9,9,28,
    12,12,28,
    15,15,28,
    18,18,28,
    21,21,28,
    0x7f,24,0x7f,
    0x7f,27,0x7f,
    0x7f,30,0x7f,
    0x7f,33,0x7f,
    0x7f,36,0x7f,
    0x7f,39,0x7f,
    0x7f,42,0x7f,  //rx hwgain tbl ptr

#ifdef BT_IF_750K
    56,0,-80,
    49,0,-80,
    38,0,-80,
    32,0,-80,
    30,0,-80,
    24,0,-80,
    14,0,-80,
    0,0,-80,
#elif defined(BT_IF_1P05M)
    56,0,-80,
    53,0,-80,
    51,0,-80,
    41,0,-80,
    36,0,-80,
    35,0,-80,
    5,0,-80,
    -6, 0,-80,
#endif

    0x7f,0x7f,0x7f,
    0x7f,0x7f,0x7f,
    0x7f,0x7f,0x7f,
    0x7f,0x7f,0x7f,
    0x7f,0x7f,0x7f,
    0x7f,0x7f,0x7f,
    0x7f,0x7f,0x7f,  //rx gain tbl ptr

    -85,-85,
    -82,-82,
    -73,-73,
    -69,-69,
    -63,-63,
    -40,-40,
    -23,-23,
    0x7f,0x7f,

    //  0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,    //rx gain ths tbl ptr

    0,0,
    0,0,
    0,0,
    0,0,
    0,0,
    0,1,
    0,2,
    0,2,
    0,2,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,    //flpha filter factor ptr
    -23,-20,-17,-14,-11,-8,-5,-2,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,   //tx pw onv tbl ptr
};

const int8_t btdrv_rxgain_gain_ths_3m[] = {
    -85,-85,
    -82,-82,
    -73,-73,
    -69,-69,
    -63,-63,
    -40,-40,
    -23,-23,

    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,
    0x7f,0x7f,    //rx gain ths tbl ptr
};

const int8_t btdrv_rxgain_ths_tbl_le[0xf * 2] = {
    -85,-85,
    -82,-82,
    -73,-73,
    -69,-69,
    -63,-63,
    -40,-40,
    -23,-23,
    0x7f, 0x7f,
    0x7f, 0x7f,
    0x7f, 0x7f,
    0x7f, 0x7f,
    0x7f, 0x7f,
    0x7f, 0x7f,
    0x7f, 0x7f,
    0x7f, 0x7f, //ble rx gain ths tbl ptr
};

const int8_t btdrv_rxgain_ths_tbl_le_2006[0xf * 2] = {
    -85,-85,
    -82,-82,
    -73,-73,
    -69,-69,
    -63,-63,
    -40,-40,
    -23,-23,
    0x7f,0x7f,
    0x7f, 0x7f,
    0x7f, 0x7f,
    0x7f, 0x7f,
    0x7f, 0x7f,
    0x7f, 0x7f,
    0x7f, 0x7f,
    0x7f, 0x7f, //ble rx gain ths tbl ptr
};

const int8_t btdrv_afh_env[] =
{
    0x02,0x00,   //afh env
    0x00,      //ignore
    33,          //length
    5,   //nb_reass_chnl
    10,  //win_len
    -70,  //rf_rssi_interf_thr
    10,  //per_thres_bad
    20,  //reass_int
    20,   //n_min
    20,   //afh_rep_intv_max
    96,    //ths_min
    2,   //chnl_assess_report_style
    15,  //chnl_assess_diff_thres
    60, // chnl_assess_interfere_per_thres_bad
    9,  //chnl_assess_stat_cnt_max
    -9,  //chnl_assess_stat_cnt_min
    1,2,3,2,1,   //chnl_assess_stat_cnt_inc_mask[5]
    1,2,3,2,1,    //chnl_assess_stat_cnt_dec_mask
    0xd0,0x7,      //chnl_assess_timer
    -48,        //chnl_assess_min_rssi
    0x64,0,   //chnl_assess_nb_pkt
    0x32,0,     //chnl_assess_nb_bad_pkt
    6,    //chnl_reassess_cnt_val
    0x3c,0,     //chnl_assess_interfere_per_thres_bad
};

const uint8_t lpclk_drift_jitter[] =
{
    0xfa,0x00,  //  drift  250ppm
    0x0a,0x00    //jitter  +-10us

};

const uint8_t  wakeup_timing[] =
{
    0xe8,0x3,   //twrm
    0xe8,0x3,    //twosc
    0xe8,0x3,    //twext
    0x3,           //rwip_prog_delay
    0x2,         //clk_corr
    0x90,0x01,//sleep_algo
    0x1,          //cpu_idle_en
    0xc,//wait_26m_cycle
    0x0,0x0,0x0,0x0//poweroff_flag
};


uint8_t  sleep_param[] =
{
    0,    // sleep_en;
    1,    // exwakeup_en;
    0xd0,0x7,    //  lpo_calib_interval;   lpo calibration interval
    0x32,0,0,0,    // lpo_calib_time;  lpo count lpc times
};

uint8_t  unsleep_param[] =
{
    0,    // sleep_en;
    0,    // exwakeup_en;
    0xc8,0,    //  lpo_calib_interval;   lpo calibration interval
    0x32,0,0,0,    // lpo_calib_time;  lpo count lpc times
};

static const uint32_t me_init_param[][2] =
{
    {0xffffffff, 0xffffffff},
};

const uint16_t me_bt_default_page_timeout = 0x2000;

const uint8_t  sync_config[] =
{
    1,1,   //sco path config   0:hci  1:pcm
    0,      //sync use max buff length   0:sync data length= packet length 1:sync data length = host sync buff len
    0,        //cvsd bypass     0:cvsd2pcm   1:cvsd transparent
};


//pcm general ctrl
#define PCM_PCMEN_POS            15
#define PCM_LOOPBCK_POS          14
#define PCM_MIXERDSBPOL_POS      11
#define PCM_MIXERMODE_POS        10
#define PCM_STUTTERDSBPOL_POS    9
#define PCM_STUTTERMODE_POS      8
#define PCM_CHSEL_POS            6
#define PCM_MSTSLV_POS           5
#define PCM_PCMIRQEN_POS         4
#define PCM_DATASRC_POS          0


//pcm phy ctrl
#define PCM_LRCHPOL_POS     15
#define PCM_CLKINV_POS      14
#define PCM_IOM_PCM_POS     13
#define PCM_BUSSPEED_LSB    10
#define PCM_SLOTEN_MASK     ((uint32_t)0x00000380)
#define PCM_SLOTEN_LSB      7
#define PCM_WORDSIZE_MASK   ((uint32_t)0x00000060)
#define PCM_WORDSIZE_LSB    5
#define PCM_DOUTCFG_MASK    ((uint32_t)0x00000018)
#define PCM_DOUTCFG_LSB     3
#define PCM_FSYNCSHP_MASK   ((uint32_t)0x00000007)
#define PCM_FSYNCSHP_LSB    0

/// Enumeration of PCM status
enum PCM_STAT
{
    PCM_DISABLE = 0,
    PCM_ENABLE
};

/// Enumeration of PCM channel selection
enum PCM_CHANNEL
{
    PCM_CH_0 = 0,
    PCM_CH_1
};

/// Enumeration of PCM role
enum PCM_MSTSLV
{
    PCM_SLAVE = 0,
    PCM_MASTER
};

/// Enumeration of PCM data source
enum PCM_SRC
{
    PCM_SRC_DPV = 0,
    PCM_SRC_REG
};

/// Enumeration of PCM left/right channel selection versus frame sync polarity
enum PCM_LR_CH_POL
{
    PCM_LR_CH_POL_RIGHT_LEFT = 0,
    PCM_LR_CH_POL_LEFT_RIGHT
};

/// Enumeration of PCM clock inversion
enum PCM_CLK_INV
{
    PCM_CLK_RISING_EDGE = 0,
    PCM_CLK_FALLING_EDGE
};

/// Enumeration of PCM mode selection
enum PCM_MODE
{
    PCM_MODE_PCM = 0,
    PCM_MODE_IOM
};

/// Enumeration of PCM bus speed
enum PCM_BUS_SPEED
{
    PCM_BUS_SPEED_128k = 0,
    PCM_BUS_SPEED_256k,
    PCM_BUS_SPEED_512k,
    PCM_BUS_SPEED_1024k,
    PCM_BUS_SPEED_2048k
};

/// Enumeration of PCM slot enable
enum PCM_SLOT
{
    PCM_SLOT_NONE = 0,
    PCM_SLOT_0,
    PCM_SLOT_0_1,
    PCM_SLOT_0_2,
    PCM_SLOT_0_3
};

/// Enumeration of PCM word size
enum PCM_WORD_SIZE
{
    PCM_8_BITS = 0,
    PCM_13_BITS,
    PCM_14_BITS,
    PCM_16_BITS
};

/// Enumeration of PCM DOUT pad configuration
enum PCM_DOUT_CFG
{
    PCM_OPEN_DRAIN = 0,
    PCM_PUSH_PULL_HZ,
    PCM_PUSH_PULL_0
};

/// Enumeration of PCM FSYNC physical shape
enum PCM_FSYNC
{
    PCM_FSYNC_LF = 0,
    PCM_FSYNC_FR,
    PCM_FSYNC_FF,
    PCM_FSYNC_LONG,
    PCM_FSYNC_LONG_16
};

const uint32_t pcm_setting[] =
{
//pcm_general_ctrl
    (PCM_DISABLE<<PCM_PCMEN_POS) |                      //enable auto
    (PCM_DISABLE << PCM_LOOPBCK_POS)  |                 //LOOPBACK test
    (PCM_DISABLE << PCM_MIXERDSBPOL_POS)  |
    (PCM_DISABLE << PCM_MIXERMODE_POS)  |
    (PCM_DISABLE <<PCM_STUTTERDSBPOL_POS) |
    (PCM_DISABLE <<PCM_STUTTERMODE_POS) |
    (PCM_CH_0<< PCM_CHSEL_POS) |
    (PCM_MASTER<<PCM_MSTSLV_POS) |                      //BT clock
    (PCM_DISABLE << PCM_PCMIRQEN_POS) |
    (PCM_SRC_DPV<<PCM_DATASRC_POS),

//pcm_phy_ctrl
    (PCM_LR_CH_POL_RIGHT_LEFT << PCM_LRCHPOL_POS) |
    (PCM_CLK_FALLING_EDGE << PCM_CLKINV_POS) |
    (PCM_MODE_PCM << PCM_IOM_PCM_POS) |
    (PCM_BUS_SPEED_2048k << PCM_BUSSPEED_LSB) |         //8k sample rate; 2048k = slot_num * sample_rate * bit= 16 * 8k * 16
    (PCM_SLOT_0_1 << PCM_SLOTEN_LSB) |
    (PCM_16_BITS << PCM_WORDSIZE_LSB) |
    (PCM_PUSH_PULL_0 << PCM_DOUTCFG_LSB) |
    (PCM_FSYNC_LF << PCM_FSYNCSHP_LSB),
};

#if 1
const uint8_t local_feature[] =
{
#if defined(__3M_PACK__)
    0xBF, 0xeE, 0xCD,0xFe,0xdb,0xFd,0x7b,0x87
#else
    0xBF, 0xeE, 0xCD,0xFa,0xdb,0xbd,0x7b,0x87

    //0xBF,0xFE,0xCD,0xFa,0xDB,0xFd,0x73,0x87   // disable simple pairing
#endif
};

#else
// disable simple pairing
uint8_t local_feature[] =
{
    0xBF,0xFE,0xCD,0xFE,0xDB,0xFd,0x73,0x87
};
#endif
const uint8_t local_ex_feature_page1[] =
{
    1,   //page
    0,0,0,0,0,0,0,0,   //page 1 feature
};

const uint8_t local_ex_feature_page2[] =
{
    2,   //page
    0x1f,0x03,0x00,0x00,0x00,0x00,0x00,0x00,   //page 2 feature
};

const uint8_t bt_rf_timing[] =
{
    0x42,// rxpwrupct;
    0x0c,// txpwrdnct;
    0x3c,// txpwrupct;
    0x00,// rxpathdly;
    0x12,// txpathdly;
    0x00,// sync_position;
    0x12,// edr_rxgrd_timeout;
};

const uint8_t ble_rl_size[] =
{
    10,    //rl size
};

//#define ACCEPT_NEW_MOBILE_EN

uint8_t bt_common_setting_2003[64] =
{
    0x03, //trace_level
    0x01, //trace_output
    0x00,0x01, //tports_level
    0x01, //power_control_type
    0x00, //close_loopbacken_flag
    0x00, //force_max_slot
    0x04, //wesco_nego
    0x05, //force_max_slot_in_sco
    0x01, //esco_retx_after_establish
    0x01,0x00, //sco_start_delay
    0x01, //msbc_pcmdout_zero_flag
    0x01, //master_2_poll
    0x01, //pca_disable_in_nosync
    0x0b, //version_major
    0x01, //version_minor
    0x15, //version_build
    0xb0,0x02, //comp_id
    0x00, //ptt_check
    0x00, //address_reset
    0x00,0x00, //hci_timeout_sleep
    0x00, //key_gen_after_reset
    0x00, //intersys_1_wakeup
    0xf4,0x01, //lmp_to_before_complete
    0x08, //fastpcm_interval
    0x01, //lm_env_reset_local_name
    0x01, //detach_directly
    0x00, //clk_off_force_even
    0x32, //seq_error_num
    0x03, //delay_process_lmp_type
    0x05, //delay_process_lmp_to 5*100 halt slot
    0x01, //ignore_pwr_ctrl_sm_state
    0x00, //lmp_task_trace_via_uart0
    0x01, //pscan_coex_en
    0x01, //iscan_coex_en
    0x00, //drift_limited
    0x00,0x7d,0x00,0x00, //init_lsto
    0x1e,0x00,0x00,0x00, //lmp_rsp_to
    0x01, //hec_error_no_update_last_ts
    0x96, //acl_rssi_avg_nb_pkt
    0xe2,0x04, //sniff_win_lim
    0x01, //iso_host_to_controller_flow
    0x01, //enable_assert
    0x00, //sw_seq_filter_en
    0x00, //ble_aux_adv_ind_update
    0x00, //sco_start_delay_flag
    0x00, //send_evt_when_wakeup
    0x00, //signal_test_en
    0x06, //ble_cis_conn_event_cnt_distance (in connect interval)
    0x0c, //ble_ci_alarm_init_distance (in half slots)
    0x01, //sync_force_max_slot
    0x00,0x08, //max_hdc_adv_dur in slots
};

uint8_t bt_sche_setting_2003[35] =
{
    0x14, //ld_sco_switch_timeout
    0x01, //two_slave_sched_en
    0xff, //music_playing_link
    0x00, //dual_slave_tws_en
    0x00, //reconnecting_flag
    0x28, //default_tpoll
    0x08, //acl_slot_in_ibrt_mode
    0x34, //ibrt_start_estb_interval
    0x9c,0x00, //acl_interv_in_ibrt_sco_mode
    0x68,0x00, //acl_interv_in_ibrt_normal_mode
    0x40,0x06, //acl_switch_to_threshold
    0x90,0x01, //stopbitoffthd
    0x0a, //delay_schd_min
    0x02, //stopthrmin
    0x78, //acl_margin
    0x50, //sco_margin
    0x01, //adv_after_sco_retx
    0x01, //ble_wrong_packet_lantency
    0x60,0x00, //sniff_priority_interv_thd
    0x0c, //scan_hslot_in_sco 12 half slot in 2ev3
    0x02, //double_pscan_in_sco
    0x06, //reduce_rext_for_sniff_thd
    0x06, //reduce_att_for_sco_thd
    0x01, //reduce_att_space_adjust
    0x01, //bandwidth_check_ignore_retx
    0x00, //check_acl_ble_en
    0x01, //check_sco_ble_en
    0x01, //sch_plan_slave_disable
    0x0a, //sco_anchor_start_add
    0x02, //ble_slot
};

uint8_t bt_ibrt_setting_2003[29] =
{
    0x00, //hci_auto_accept_tws_link_en
    0x00, //send_profile_via_ble
    0x00, //force_rx_error
    0x6c, //sync_win_size hus
    0x16, //magic_cal_bitoff
    0x3f, //role_switch_packet_br
    0x0b, //role_switch_packet_edr
    0x01, //only_support_slave_role
    0x01, //msg_filter_en
    0x00, //relay_sam_info_in_start_snoop
    0x00, //snoop_switch_interval_num
    0x02, //slave_rx_traffic_siam
    0x02, //ibrt_lmp_to
    0x00, //fa_use_twslink_table
    0x00, //fa_use_fix_channel
    0x06, //ibrt_afh_instant_adjust
    0x06, //ibrt_detach_send_instant
    0x06, //ibrt_detach_receive_instant
    0x03, //ibrt_detach_txcfm_instant
    0x01, //mobile_enc_change
    0x01, //ibrt_auto_accept_sco
    0x01, //ibrt_second_sco_decision
    0x02, //fa_ok_thr_in_sco
    0x02, //fa_ok_thr_for_acl
    0x01, //accep_remote_bt_roleswitch
    0x01, //accept_remote_enter_sniff
    0x0f, //start_ibrt_tpoll
    0x00, //update_all_saved_queue
    0x00, //not_pro_saved_msg_in_ibrt_switch
};

uint8_t bt_hw_feat_setting_2003[43] =
{
    0x01, //rxheader_int_en
    0x01, //rxdone_bt_int_en
    0x01, //txdone_bt_int_en
    0x01, //rxsync_bt_int_en
    0x01, //rxsync_ble_int_en
    0x00, //pscan_hwagc_flag
    0x00, //iscan_hwagc_flag
    0x00, //sniff_hwagc_flag
    0x00, //blescan_hwagc_flag
    0x00, //bleinit_hwagc_flag
#ifdef __NEW_SWAGC_MODE__
    0x01, //bt_sync_swagc_en
#else
    0x00, //bt_sync_swagc_en
#endif
#ifdef __BLE_NEW_SWAGC_MODE__
    0x01, //le_sync_swagc_en
#else
    0x00, //le_sync_swagc_en
#endif
    0x00, //fa_to_en
    0x00, //fa_dsb_en
    0x0F, //fa_to_num
    0xff, //fa_rxgain
    0x55,0x00,0x00,0x00, //fa_to_type
    0x50,0x00,0x00,0x00, //fa_disable_type
    0xff, //fa_txpwr
    0x00, //ecc_data_flag
    0x01, //softbit_data_flag
    0x00, //rx_noise_chnl_assess_en
    0x14, //rx_noise_thr_good
    0x0a, //rx_noise_thr_bad
    0x00, //snr_chnl_assess_en
    0x14, //snr_good_thr_value
    0x28, //snr_bad_thr_value
    0x00, //rssi_maxhold_record_en
    0xff, //ble_adv_bis_txpwr
    0x7f, //ble_adv_bis_rxgain_dbm
    0xff, //ble_con_txpwr
    0x7f, //ble_con_rxgain_dbm
    0xff, //ble_scan_con_req_txpwr
    0x7f, //ble_scan_con_rxgain_dbm
    0x0a, //new_agc_adjust_dbm
    0x0f, //ble_rssi_noise_thr
    0x01, //ble_rxgain_adjust_once
};


uint8_t bt_common_setting_t1_2003[11] =
{
    16,//tws_acl_prio_in_sco
    20,//tws_acl_prio_in_normal
    10,//scan_evt_win_slot_in_a2dp
    40,//pscan_gap_slot_in_a2dp
    20,//page_gap_slot_in_a2dp
    1,//ble_eff_time_config
    0,//send_tws_interval_to_peer
#ifdef __BT_RAMRUN__
    0,//softbit_enable
#else
    1,//softbit_enable
#endif
    1,//ble_adv_buf_malloc
#ifdef __AFH_ASSESS__
    1,//afh_assess_old
#else
    1,//afh_assess_old
#endif
    0,//lld_ip_ci_bi_compute_params
};


bool btdrv_get_accept_new_mobile_enable(void)
{
    bool ret = false;
    return ret;
}

bool btdrv_get_page_pscan_coex_enable(void)
{
    bool ret = false;

    BT_DRV_TRACE(1,"BT_DRV_CONFIG:page_pscan_coex enable=%d", bt_common_setting_2003[37]);
    ret = bt_common_setting_2003[37];

    return ret;
}

const uint8_t bt_edr_thr[] =
{
    30,0,60,0,5,0,60,0,0,1,
    30,0,60,0,5,0,60,0,1,1,
    30,0,60,0,5,0,60,0,1,1,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
};

const uint8_t bt_edr_algo[]=
{
    0,0,1,
    8,0,3,
    16,0,0,
    0xff,0xff,0xff,
    0xff,0xff,0xff,
    0xff,0xff,0xff,
    0xff,0xff,0xff,
};

const uint8_t bt_rssi_thr[]=
{
    -1, //high
    -2, //low
    -100, //interf
};


const uint8_t ble_dle_dft_value[]=
{
    0xfb,0x00, ///tx octets
    0x48,0x08, ///tx time
    0xfb,0x00, ///rx octets
    0x48,0x08, ///rx time
};

uint8_t bt_lmp_record[]=
{
    0,  //en
    0,  //only opcode
};
const uint8_t bt_peer_txpwr_dft_thr[]=
{
    0x64,00,//uint16_t rssi_avg_nb_pkt;
    -1,//rssi_high_thr;
    -2,//rssi_low_thr;
    5,//rssi_below_low_thr;
    50,//unused rssi_interf_thr;
};

const uint8_t bt_txrx_gain_setting[]=
{
   0,//bt_inq_rxgain;
   0,//bt_page_rxgain;
   6,//bt_page_txpwr;
   0,//bt_iscan_rxgain;
   0xff,//bt_iscan_txpwr;
   0,//bt_pscan_rxgain;
   0xff,//bt_pscan_txpwr;
   0,//bt_ibrt_rxgain;
   0xff,//ble_adv_txpwr;
   0,//ble_adv_rxgain;
   0xff,//ble_bis_txpwr;
   0,//ble_bis_rxgain;
   0xff,//ble_adv_per_txpwr;
   0,//ble_adv_per_rxgain;
   0xff,//ble_testmode_txpwr;
   0,//ble_testmode_rxgain;
   0xff,//ble_con_txpwr;
   0,//ble_con_rxgain;
   0xff,//ble_con_init_txpwr;
   0,//ble_con_init_rxgain;
   0xff,//ble_scan_txpwr;
   3,//ble_scan_rxgain;
};

const uint8_t dbg_ble_scan_rxgain_changed_cmd[]=
{
    1,//enable;
    0x3c,//rssi_max_default;
    3,//gain_count;
    0,//ble_scan_rxgain_0;
    3,//ble_scan_rxgain_1;
    6,//ble_scan_rxgain_2;
    0,//ble_scan_rxgain_3;
    0,//ble_scan_rxgain_4;
    0,//ble_scan_rxgain_5;
    0,//ble_scan_rxgain_6;
};

const struct rssi_txpower_link_thd tws_link_txpwr_thd =
{
    0x64,//uint16_t rssi_avg_nb_pkt;
    -40,//rssi_high_thr;
    -50,//rssi_low_thr;
    5,//rssi_below_low_thr;
    50,//unused rssi_interf_thr;
};
uint8_t bt_set_transq_flag[26] =
{
    1,//wlan_on_off
    1,//bt_sniff
    1,//bt_pscan
    1,//bt_rsw
    1,//bt_sco
    1,//ble_init
    0,//ble_scan
    1,//ble_adv
    1,//ble_conn
    0,//ble_adv_aux
    0,//ble_per_adv
    0,//ble_audio_bis
    0,//ble_audio_cis
    0,//start_link
    0,//stop_link
    0,//new_role
    0,0,0,0,//transq_timmer_extra
    1,//ble_adv_rand_delay_en
    17,//ble_adv_max_rand_delay
    0,//sigle_adv_flag
    6,//ble_adv_prio
    0,//transq_sco_att
    1,//ble_sync
};
static BTDRV_CFG_TBL_STRUCT  btdrv_cfg_tbl[] =
{
    {BTDRV_CONFIG_INACTIVE,HCI_DBG_LMP_MESSAGE_RECORD_CMD_OPCODE,sizeof(bt_lmp_record),(uint8_t *)&bt_lmp_record},
    {BTDRV_CONFIG_ACTIVE,HCI_DBG_SET_LOCAL_FEATURE_CMD_OPCODE,sizeof(local_feature),local_feature},
    {BTDRV_CONFIG_ACTIVE,HCI_DBG_SET_BT_SETTING_CMD_OPCODE,sizeof(bt_common_setting_2003),bt_common_setting_2003},
    {BTDRV_CONFIG_ACTIVE,HCI_DBG_SET_BT_SCHE_SETTING_CMD_OPCODE,sizeof(bt_sche_setting_2003),bt_sche_setting_2003},
    {BTDRV_CONFIG_ACTIVE,HCI_DBG_SET_BT_IBRT_SETTING_CMD_OPCODE,sizeof(bt_ibrt_setting_2003),bt_ibrt_setting_2003},
    {BTDRV_CONFIG_ACTIVE,HCI_DBG_SET_BT_HW_FEAT_SETTING_CMD_OPCODE,sizeof(bt_hw_feat_setting_2003),bt_hw_feat_setting_2003},
    {BTDRV_CONFIG_ACTIVE,HCI_DBG_BT_COMMON_SETTING_T1_CMD_OPCODE,sizeof(bt_common_setting_t1_2003),bt_common_setting_t1_2003},
    {BTDRV_CONFIG_ACTIVE,HCI_DBG_SET_TRANSQ_FLAG_CMD_OPCODE,sizeof(bt_set_transq_flag),bt_set_transq_flag},

//    {BTDRV_CONFIG_ACTIVE,HCI_DBG_SET_SLEEP_SETTING_CMD_OPCODE,sizeof(sleep_param),sleep_param},
    {BTDRV_CONFIG_ACTIVE,HCI_DBG_SET_CUSTOM_PARAM_CMD_OPCODE,189,(uint8_t *)&btdrv_rf_env_2003},
    //{BTDRV_CONFIG_ACTIVE,HCI_DBG_SET_CUSTOM_PARAM_CMD_OPCODE,sizeof(btdrv_afh_env),(uint8_t *)&btdrv_afh_env},
    // {BTDRV_CONFIG_INACTIVE,HCI_DBG_SET_LPCLK_DRIFT_JITTER_CMD_OPCODE,sizeof(lpclk_drift_jitter),(uint8_t *)&lpclk_drift_jitter},
    // {BTDRV_CONFIG_ACTIVE,HCI_DBG_SET_WAKEUP_TIME_CMD_OPCODE,sizeof(wakeup_timing),(uint8_t *)&wakeup_timing},
#ifdef _SCO_BTPCM_CHANNEL_
    {BTDRV_CONFIG_INACTIVE,HCI_DBG_SET_SYNC_CONFIG_CMD_OPCODE,sizeof(sync_config),(uint8_t *)&sync_config},
    {BTDRV_CONFIG_INACTIVE,HCI_DBG_SET_PCM_SETTING_CMD_OPCODE,sizeof(pcm_setting),(uint8_t *)&pcm_setting},
#endif
    {BTDRV_CONFIG_INACTIVE,HCI_DBG_SET_RSSI_THRHLD_CMD_OPCODE,sizeof(bt_rssi_thr),(uint8_t *)&bt_rssi_thr},
    {BTDRV_CONFIG_ACTIVE,HCI_DBG_SET_LOCAL_EX_FEATURE_CMD_OPCODE,sizeof(local_ex_feature_page2),(uint8_t *)&local_ex_feature_page2},
    {BTDRV_CONFIG_ACTIVE,HCI_DBG_SET_BT_RF_TIMING_CMD_OPCODE,sizeof(bt_rf_timing),(uint8_t *)&bt_rf_timing},
    {BTDRV_CONFIG_ACTIVE,HCI_DBG_SET_RSSI_TX_POWER_DFT_THR_CMD_OPCODE,sizeof(bt_peer_txpwr_dft_thr),(uint8_t *)&bt_peer_txpwr_dft_thr},
    {BTDRV_CONFIG_ACTIVE,HCI_DBG_SET_BT_BLE_TXRX_GAIN_CMD_OPCODE,sizeof(bt_txrx_gain_setting),(uint8_t *)&bt_txrx_gain_setting},
    //if use HCI_DBG_BLE_SCAN_RXGAIN_CHANGED_CMD_OPCODE,HCI_DBG_SET_BT_BLE_TXRX_GAIN_CMD_OPCODE should be BTDRV_CONFIG_INACTIVE
    {BTDRV_CONFIG_INACTIVE,HCI_DBG_BLE_SCAN_RXGAIN_CHANGED_CMD_OPCODE,sizeof(dbg_ble_scan_rxgain_changed_cmd),(uint8_t *)&dbg_ble_scan_rxgain_changed_cmd},
};

static BTDRV_CFG_TBL_STRUCT  btdrv_cfg_tbl_2003 =
{
    BTDRV_CONFIG_ACTIVE,
    HCI_DBG_SET_CUSTOM_PARAM_CMD_OPCODE,
    189,
    (uint8_t *)&btdrv_rf_env_2003,
};

static BTDRV_CFG_TBL_STRUCT  btdrv_cfg_tbl_2006 =
{
    BTDRV_CONFIG_ACTIVE,
    HCI_DBG_SET_CUSTOM_PARAM_CMD_OPCODE,
    189,
    (uint8_t *)&btdrv_rf_env_2006,
};

void btdrv_fa_txpwrup_timing_setting(uint8_t txpwrup)
{
    BTDIGITAL_REG_SET_FIELD(BT_BES_CNTL2_ADDR, 0xff, 24, txpwrup);
}

void btdrv_fa_rxpwrup_timig_setting(uint8_t rxpwrup)
{
    BTDIGITAL_REG_SET_FIELD(BT_BES_CNTL2_ADDR, 0xff, 16, rxpwrup);
}

void btdrv_fa_margin_timig_setting(uint8_t margin)
{
    BTDIGITAL_REG_SET_FIELD(BT_BES_CNTL2_ADDR, 0x1f, 11, margin);
}

void bt_fa_sync_invert_en_setf(uint8_t faacinven)
{
    BTDIGITAL_REG_WR(BT_BES_FACNTL0_ADDR,
                     (BTDIGITAL_REG(BT_BES_FACNTL0_ADDR) & ~((uint32_t)0x00020000)) | ((uint32_t)faacinven << 17));
}

void btdrv_fast_lock_en_setf(uint8_t rxonextenden)
{
    BTDIGITAL_REG_WR(BT_BES_FACNTL0_ADDR,
                     (BTDIGITAL_REG(BT_BES_FACNTL0_ADDR) & ~((uint32_t)0x40000000)) | ((uint32_t)rxonextenden << 30));
}

void btdrv_fatx_pll_pre_on_en(uint8_t en)
{
    BTDIGITAL_REG_WR(BT_BES_CNTL5_ADDR,
                     (BTDIGITAL_REG(BT_BES_CNTL5_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)en << 31));
}

void btdrv_fa_xsco_wrbuf_mask_en_setf(uint8_t xscowrbufmasken)
{
    //BTDIGITAL_REG_WR(BT_FASTACK_ADDR,
    //                 (BTDIGITAL_REG(BT_FASTACK_ADDR) & ~((uint32_t)0x10000000)) | ((uint32_t)xscowrbufmasken << 28));
}

void btdrv_btradio_set_tx_delay(uint8_t tx_delay)
{
    BTDIGITAL_REG_SET_FIELD(BT_RADIOTXRXTIM_ADDR, 0x7f, 0, (tx_delay & 0x7f));
}
void btdrv_btradiowrupdn_set_rxpwrup(uint8_t rxpwrup)
{
    BTDIGITAL_REG_SET_FIELD(BT_RADIOPWRUPDN_ADDR, 0xff, 16, (rxpwrup & 0xff));
}
void btdrv_btradiowrupdn_set_txpwrdn(uint8_t txpwrdn)
{
    BTDIGITAL_REG_SET_FIELD(BT_RADIOPWRUPDN_ADDR, 0x7f, 8, (txpwrdn & 0x7f));
}
void btdrv_btradiowrupdn_set_txpwrup(uint8_t txpwrup)
{
    BTDIGITAL_REG_SET_FIELD(BT_RADIOPWRUPDN_ADDR, 0xff, 0, (txpwrup & 0xff));
}

void btdrv_bleradiowrupdn_set_rtrip_delay(uint8_t rtrip_delay)
{
    //BTDIGITAL_REG_SET_FIELD(0xd0220280, 0x7f, 24, (rtrip_delay & 0x7f));
}
void btdrv_bleradiowrupdn_set_rxpwrup(uint8_t rxpwrup)
{
    //BTDIGITAL_REG_SET_FIELD(0xd0220280, 0xff, 16, (rxpwrup & 0xff));
}
void btdrv_bleradiowrupdn_set_txpwrdn(uint8_t txpwrdn)
{
    //BTDIGITAL_REG_SET_FIELD(0xd0220280, 0x1f, 8, (txpwrdn & 0x1f));
}
void btdrv_bleradiowrupdn_set_txpwrup(uint8_t txpwrup)
{
    //BTDIGITAL_REG_SET_FIELD(0xd0220280, 0xff, 0, (txpwrup & 0xff));
}


void ble_radiopwrupdn0_pack(uint8_t syncposition0, uint8_t rxpwrup0, uint8_t txpwrdn0, uint8_t txpwrup0)
{
    BTDIGITAL_REG_WR(BLE_RADIOPWRUPDN0_ADDR,  ((uint32_t)syncposition0 << 24) | ((uint32_t)rxpwrup0 << 16) | ((uint32_t)txpwrdn0 << 8) | ((uint32_t)txpwrup0 << 0));
}

void ble_radiopwrupdn1_pack(uint8_t syncposition1, uint8_t rxpwrup1, uint8_t txpwrdn1, uint8_t txpwrup1)
{
    BTDIGITAL_REG_WR(BLE_RADIOPWRUPDN1_ADDR,  ((uint32_t)syncposition1 << 24) | ((uint32_t)rxpwrup1 << 16) | ((uint32_t)txpwrdn1 << 8) | ((uint32_t)txpwrup1 << 0));
}

void ble_radiopwrupdn2_pack(uint8_t syncposition2, uint8_t rxpwrup2, uint8_t txpwrdn2, uint8_t txpwrup2)
{
    BTDIGITAL_REG_WR(BLE_RADIOPWRUPDN2_ADDR,  ((uint32_t)syncposition2 << 24) | ((uint32_t)rxpwrup2 << 16) | ((uint32_t)txpwrdn2 << 8) | ((uint32_t)txpwrup2 << 0));
}

void ble_radiopwrupdn3_pack(uint8_t txpwrdn3, uint8_t txpwrup3)
{
    BTDIGITAL_REG_WR(BLE_RADIOPWRUPDN3_ADDR,  ((uint32_t)txpwrdn3 << 8) | ((uint32_t)txpwrup3 << 0));
}

void ble_radiotxrxtim0_pack(uint8_t rfrxtmda0, uint8_t rxpathdly0, uint8_t txpathdly0)
{
    BTDIGITAL_REG_WR(BLE_RADIOTXRXTIM0_ADDR,  ((uint32_t)rfrxtmda0 << 16) | ((uint32_t)rxpathdly0 << 8) | ((uint32_t)txpathdly0 << 0));
}
void ble_radiotxrxtim1_pack(uint8_t rfrxtmda1, uint8_t rxpathdly1, uint8_t txpathdly1)
{
    BTDIGITAL_REG_WR(BLE_RADIOTXRXTIM1_ADDR,  ((uint32_t)rfrxtmda1 << 16) | ((uint32_t)rxpathdly1 << 8) | ((uint32_t)txpathdly1 << 0));
}
void ble_radiotxrxtim2_pack(uint8_t rxflushpathdly2, uint8_t rfrxtmda2, uint8_t rxpathdly2, uint8_t txpathdly2)
{
    BTDIGITAL_REG_WR(BLE_RADIOTXRXTIM2_ADDR,  ((uint32_t)rxflushpathdly2 << 24) | ((uint32_t)rfrxtmda2 << 16) | ((uint32_t)rxpathdly2 << 8) | ((uint32_t)txpathdly2 << 0));
}
void ble_radiotxrxtim3_pack(uint8_t rxflushpathdly3, uint8_t rfrxtmda3, uint8_t txpathdly3)
{
    BTDIGITAL_REG_WR(BLE_RADIOTXRXTIM3_ADDR,  ((uint32_t)rxflushpathdly3 << 24) | ((uint32_t)rfrxtmda3 << 16) | ((uint32_t)txpathdly3 << 0));
}

uint8_t ble_radiotxrxtim0_rxpathdly0_getf(void)
{
    uint32_t localVal = BTDIGITAL_REG(BLE_RADIOTXRXTIM0_ADDR);
    return ((localVal & ((uint32_t)0x00007F00)) >> 8);
}

uint8_t ble_radiotxrxtim1_rxpathdly1_getf(void)
{
    uint32_t localVal = BTDIGITAL_REG(BLE_RADIOTXRXTIM1_ADDR);
    return ((localVal & ((uint32_t)0x00007F00)) >> 8);
}
uint8_t ble_radiotxrxtim2_rxpathdly2_getf(void)
{
    uint32_t localVal = BTDIGITAL_REG(BLE_RADIOTXRXTIM2_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}


uint8_t ble_radiotxrxtim2_rxflushpathdly2_getf(void)
{
    uint32_t localVal = BTDIGITAL_REG(BLE_RADIOTXRXTIM2_ADDR);
    return ((localVal & ((uint32_t)0xFF000000)) >> 24);
}

uint8_t ble_radiotxrxtim3_rxflushpathdly3_getf(void)
{
    uint32_t localVal = BTDIGITAL_REG(BLE_RADIOTXRXTIM3_ADDR);
    return ((localVal & ((uint32_t)0xFF000000)) >> 24);
}

void ble_radiocntl3_pack(uint8_t rxrate3cfg, uint8_t rxrate2cfg, uint8_t rxrate1cfg, uint8_t rxrate0cfg, uint8_t getrssidelay, uint8_t rxsyncrouting, uint8_t rxvalidbeh, uint8_t txrate3cfg, uint8_t txrate2cfg, uint8_t txrate1cfg, uint8_t txrate0cfg, uint8_t txvalidbeh)
{
    BTDIGITAL_REG_WR(BLE_RADIOCNTL3_ADDR,  ((uint32_t)rxrate3cfg << 30) | ((uint32_t)rxrate2cfg << 28) | ((uint32_t)rxrate1cfg << 26) | ((uint32_t)rxrate0cfg << 24) | ((uint32_t)getrssidelay << 20) | ((uint32_t)rxsyncrouting << 18) | ((uint32_t)rxvalidbeh << 16) | ((uint32_t)txrate3cfg << 14) | ((uint32_t)txrate2cfg << 12) | ((uint32_t)txrate1cfg << 10) | ((uint32_t)txrate0cfg << 8) | ((uint32_t)txvalidbeh << 0));
}

#if (defined(SOFTBIT_EN))
void btdrv_softbit_config(uint16_t connhdl, uint8_t type1,uint8_t type2,uint8_t type3, uint8_t num)
{
#if 0
    uint8_t link_id = 0;
    if(num == 1)
    {
        BTDIGITAL_REG_SET_FIELD(0xd0220464, 0xf, 0, (type1 & 0xf));
        BTDIGITAL_REG(0xd0220464) |= (1<<12);
    }
    else if(num == 2)
    {
        BTDIGITAL_REG_SET_FIELD(0xd0220464, 0xf, 0, (type1 & 0xf));
        BTDIGITAL_REG(0xd0220464) |= (1<<12);
        BTDIGITAL_REG_SET_FIELD(0xd0220464, 0xf, 4, (type2 & 0xf));
        BTDIGITAL_REG(0xd0220464) |= (1<<13);
    }
    else if(num == 3)
    {
        BTDIGITAL_REG_SET_FIELD(0xd0220464, 0xf, 0, (type1 & 0xf));
        BTDIGITAL_REG(0xd0220464) |= (1<<12);
        BTDIGITAL_REG_SET_FIELD(0xd0220464, 0xf, 4, (type2 & 0xf));
        BTDIGITAL_REG(0xd0220464) |= (1<<13);
        BTDIGITAL_REG_SET_FIELD(0xd0220464, 0xf, 8, (type3 & 0xf));
        BTDIGITAL_REG(0xd0220464) |= (1<<14);
    }
    if(connhdl & (0xff00))//sco
    {
        link_id = ((connhdl & 0xff00) >> 8) + 1;
        BTDIGITAL_REG_SET_FIELD(0xd022047c, 3, 24, (link_id & 3));
        BTDIGITAL_REG(0xd022046c) |= (1<<5);
    }
    else if(connhdl >= HCI_HANDLE_MIN && connhdl <= HCI_HANDLE_MAX) //acl
    {
        link_id = connhdl - HCI_HANDLE_MIN +  1;
        BTDIGITAL_REG_SET_FIELD(0xd022047c, 3, 24, (link_id & 3));
        BTDIGITAL_REG(0xd022046c) |= (1<<6);
    }
    BTDIGITAL_REG_SET_FIELD(0xd0220028, 0x1ff, 0, 0xe1);//rxguard time for sbc

    //[18]1:SNR weighting mode
#ifdef __SNR_WEIGHTING_MODE__
    BTDIGITAL_REG(0xd03503d8) |= (1<<18);
#else
    //[18]0:equally weighted mode
    BTDIGITAL_REG(0xd03503d8) &= ~(1<<18);
#endif
#endif
}
#endif

void btdrv_2m_band_wide_sel(uint8_t fa_2m_mode)
{
#ifdef BT_SYSTEM_52M
    //only BT system 52M can use fastack 2M mode
    BTDIGITAL_REG_WR(BT_BES_FACNTL0_ADDR,
                     (BTDIGITAL_REG(BT_BES_FACNTL0_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)fa_2m_mode << 0));
#endif
}

void btdrv_48m_sys_enable(void)
{
    hal_cmu_bt_sys_set_freq(HAL_CMU_FREQ_52M);
    BTDIGITAL_REG(CMU_CLKREG_ADDR)|=(1<<7)|(1<<19);//BT select 52M system
}

void btdrv_24m_sys_enable(void)
{
//   hal_cmu_bt_sys_set_freq(HAL_CMU_FREQ_52M);
    BTDIGITAL_REG(CMU_CLKREG_ADDR) &= ~((1<<7)|(1<<19));//BT select 52M system
}


void btdrv_new_mode_bt_corr_thr(uint8_t corr_thr)
{
    //BTDIGITAL_REG_SET_FIELD(0xd03503a4, 0x7f, 0, (corr_thr & 0x7f));
}

void btdrv_new_mode_ble_corr_thr(uint8_t corr_thr)
{
    //BTDIGITAL_REG_SET_FIELD(0xd03503a4, 0x7f, 8, (corr_thr & 0x7f));
}

void btdrv_old_mode_bt_corr_thr(uint8_t corr_thr)
{
    //BTDIGITAL_REG_SET_FIELD(0xd0220000, 0xf, 0, (corr_thr & 0xf));
}

void btdrv_old_mode_ble_corr_thr(uint8_t corr_thr)
{
    //BTDIGITAL_REG_SET_FIELD(0xd0220200, 0x7, 0, (corr_thr & 0x7));
}

void btdrv_set_fa_use_old_swagc(void)
{
#if 0
    //second rf spi
#ifdef __NEW_SWAGC_MODE__
    BTDIGITAL_REG_SET_FIELD(0xd0220484, 0xffff, 0, 0x25c);
#else
    BTDIGITAL_REG_SET_FIELD(0xd0220484, 0xffff, 0, 0x21c);
#endif
    BTDIGITAL_REG_SET_FIELD(0xd0220480, 0xff, 0, 0xe2);
#endif
}

void btdrv_fa_corr_mode_setting(bool is_new_corr)
{
    //[1]: corr_preamble
    if(is_new_corr == true)
    {
        BTDIGITAL_REG(0xd0350228) |= (1<<24);//enable new corr
    }
    else
    {
        BTDIGITAL_REG(0xd0350228) &= ~(1<<24);//enable old corr
    }
}

void btdrv_fa_new_mode_corr_thr(uint8_t corr_thr)
{
    BTDIGITAL_REG_SET_FIELD(0xd0350228, 0x7f, 16, (corr_thr & 0x7f));
}

void btdrv_fa_old_mode_corr_thr(uint8_t corr_thr)
{
    BTDIGITAL_REG_SET_FIELD(0xd0350368, 0xf, 4, (corr_thr & 0xf));
}


void btdrv_fa_config_vendor_syncword_en(bool enable)
{
    if(enable)
    {
        BTDIGITAL_REG(BT_BES_FACNTL1_ADDR) |= (1<<31);
    }
    else
    {
        BTDIGITAL_REG(BT_BES_FACNTL1_ADDR) &= ~(1<<31);
    }
}

void btdrv_set_fa_redundancy_time(uint8_t time)
{
    //BTDIGITAL_REG_SET_FIELD(0xd0220488, 0x1f, 21, (time & 0x1f));
}

void btdrv_fa_config_vendor_syncword_content(uint32_t syncword_high, uint32_t syncword_low, uint8_t syncword_len)
{
    if(syncword_low == INVALID_SYNC_WORD || syncword_high == INVALID_SYNC_WORD)
    {
        BT_DRV_TRACE(0,"BT_DRV:vendor syncword invalid");
        return;
    }

    if(syncword_len == FA_SYNCWORD_32BIT)
    {
        BTDIGITAL_REG(BT_BES_FA_SWREG1_ADDR) = syncword_low;
    }
    else if(syncword_len == FA_SYNCWORD_64BIT)
    {
        BTDIGITAL_REG(BT_BES_FA_SWREG1_ADDR) = syncword_high;
        BTDIGITAL_REG(BT_BES_FA_SWREG0_ADDR) = syncword_low;
    }
}

void btdrv_fa_config_syncword_mode(uint8_t syncword_len)
{
    if(syncword_len == FA_SYNCWORD_32BIT)
    {
        BTDIGITAL_REG(0xd0220c8c) &= ~(1<<28);//syncword 32bit
    }
    else if(syncword_len == FA_SYNCWORD_64BIT)
    {
        BTDIGITAL_REG(0xd0220c8c) |= (1<<28);//syncword 64bit
    }
}

void btdrv_fa_config_tx_gain(bool ecc_tx_gain_en, uint8_t ecc_tx_gain_idx)//false :disable tx gain
{
    if(ecc_tx_gain_en == true)
    {
        BTDIGITAL_REG(BT_BES_CNTL3_ADDR) |= (1<<29);
        BTDIGITAL_REG_SET_FIELD(BT_BES_CNTL3_ADDR, 0xf, 25, (ecc_tx_gain_idx & 0xf));
    }
    else
    {
        BTDIGITAL_REG(BT_BES_CNTL3_ADDR) &= ~(1<<29);
    }
}

void btdrv_fa_config_rx_gain(bool ecc_rx_gain_en, uint8_t ecc_rx_gain_idx)//false: disable rx gain
{
    if(ecc_rx_gain_en == true)
    {
        BTDIGITAL_REG(BT_BES_CNTL3_ADDR) |= (1<<30);
        BTDIGITAL_REG_SET_FIELD(BT_BES_CNTL3_ADDR, 0xf, 21, (ecc_rx_gain_idx & 0xf));
    }
    else
    {
        BTDIGITAL_REG(BT_BES_CNTL3_ADDR) &= ~(1<<30);
    }
}

void bt_drv_enhance_fa_mode(bool enable)
{
}

void btdrv_fa_rx_winsz(uint8_t ecc_rx_winsz)
{
    BTDIGITAL_REG_SET_FIELD(BT_BES_FACNTL1_ADDR, 0x7f, 24, (ecc_rx_winsz & 0x7f));
}

void btdrv_fa_tx_preamble_enable(bool fa_tx_preamble_en)
{
    if(fa_tx_preamble_en)
    {
        BTDIGITAL_REG(0xD0220C18) |= (1<<16); // fa tx 8 bit preamble
    }
    else
    {
        BTDIGITAL_REG(0xD0220C18) &= ~(1<<16);
    }
}

void btdrv_fa_freq_table_enable(bool fa_freq_table_en)
{
    if(fa_freq_table_en)
    {
        BTDIGITAL_REG(0xD0220C80) |= (1<<14);
    }
    else
    {
        BTDIGITAL_REG(0xD0220C80) &= ~(1<<14);
    }
}

void btdrv_fa_multi_mode0_enable(bool fa_multi_mode0_en)
{
    if(fa_multi_mode0_en)
    {
        BTDIGITAL_REG(0xD0220C18) |= (1<<24);
        BTDIGITAL_REG_SET_FIELD(BT_BES_FACNTL1_ADDR, 0x7f, 24, (0x66 & 0x7f));
    }
    else
    {
        BTDIGITAL_REG(0xD0220C18) &= ~(1<<24);
    }
}

void btdrv_fa_multi_mode1_enable(bool fa_multi_mode1_en, uint8_t fa_multi_tx_count)
{
    if(fa_multi_mode1_en)
    {
        BTDIGITAL_REG(0xD0220C18) |= (1<<25);
        if(fa_multi_tx_count == 2)
        {
            BTDIGITAL_REG(0xD0220C18) |= 0x00800000;
            BTDIGITAL_REG_SET_FIELD(BT_BES_FACNTL1_ADDR, 0x7f, 24, (0x31 & 0x7f));
        }

        if(fa_multi_tx_count == 3)
        {
            BTDIGITAL_REG(0xD0220C18) |= 0x00c00000;
            BTDIGITAL_REG_SET_FIELD(BT_BES_FACNTL1_ADDR, 0x7f, 24, (0x5b & 0x7f));
        }
    }
    else
    {
        BTDIGITAL_REG(0xD0220C18) &= ~(1<<25);
    }
}

void btdrv_fa_freq_table_config(uint32_t fa_freq_table0,uint32_t fa_freq_table1,uint32_t fa_freq_table2)
{
    BTDIGITAL_REG(0xD0220C40)  = fa_freq_table0; // freq 0~31
    BTDIGITAL_REG(0xD0220C44)  = fa_freq_table1; // freq 32~63
    BTDIGITAL_REG(0xD0220C48) |= fa_freq_table2; // freq 64~78
}

void btdrv_fa_basic_config(btdrv_fa_basic_config_t* p_fa_basic_config)
{
    if(p_fa_basic_config != NULL)
    {
        //fast ack TX power gain set
        btdrv_fa_config_tx_gain(p_fa_basic_config->fa_tx_gain_en, p_fa_basic_config->fa_tx_gain_idx);
#ifdef __FIX_FA_RX_GAIN___
        //fix fast ack rx gain
        btdrv_fa_config_rx_gain(p_fa_basic_config->fa_rx_gain_en, p_fa_basic_config->fa_rx_gain_idx);
        bt_drv_reg_op_set_fa_rx_gain_idx(p_fa_basic_config->fa_rx_gain_idx);
#endif
        // fa 2M mode select
        btdrv_2m_band_wide_sel(p_fa_basic_config->fa_2m_mode);
        //fa syncword mode
        btdrv_fa_config_syncword_mode(p_fa_basic_config->syncword_len);
        //ECC vendor syncword mode
        if(p_fa_basic_config->fa_vendor_syncword_en)
        {
            btdrv_fa_config_vendor_syncword_en(true);
            btdrv_fa_config_vendor_syncword_content(p_fa_basic_config->syncword_high,
                                                    p_fa_basic_config->syncword_low,
                                                    p_fa_basic_config->syncword_len);
        }
        else
        {
            btdrv_fa_config_vendor_syncword_en(false);
        }
        //fa win size
        btdrv_fa_rx_winsz(p_fa_basic_config->fa_rx_winsz);
        //fa corr mode
        btdrv_fa_corr_mode_setting(p_fa_basic_config->is_new_corr);

        if(p_fa_basic_config->is_new_corr)
        {
            btdrv_fa_new_mode_corr_thr(p_fa_basic_config->new_mode_corr_thr);
        }
        else
        {
            btdrv_fa_old_mode_corr_thr(p_fa_basic_config->old_mode_corr_thr);
        }

        if(p_fa_basic_config->fa_tx_preamble_en)
        {
            btdrv_fa_tx_preamble_enable(true);
        }
        else
        {
            btdrv_fa_tx_preamble_enable(false);
        }

        if(p_fa_basic_config->fa_freq_table_en)
        {
            btdrv_fa_freq_table_enable(true);
            btdrv_fa_freq_table_config(p_fa_basic_config->fa_freq_table0,
                                       p_fa_basic_config->fa_freq_table1,
                                       p_fa_basic_config->fa_freq_table2);
        }
        else
        {
            btdrv_fa_freq_table_enable(false);
        }

        if(p_fa_basic_config->fa_multi_mode0_en)
        {
            btdrv_fa_multi_mode0_enable(true);
        }
        else
        {
            btdrv_fa_multi_mode0_enable(false);
        }

        if(p_fa_basic_config->fa_multi_mode1_en)
        {
            btdrv_fa_multi_mode1_enable(true, p_fa_basic_config->fa_multi_tx_count);
        }
        else
        {
            btdrv_fa_multi_mode1_enable(false,p_fa_basic_config->fa_multi_tx_count);
        }

        //set enhance fa mode
        bt_drv_enhance_fa_mode(p_fa_basic_config->enhance_fa_mode_en);
    }

    //fa agc config
    btdrv_set_fa_use_old_swagc();
    //enable fa invert
    bt_fa_sync_invert_en_setf(BT_FA_INVERT_EN);
}

#ifdef __FASTACK_ECC_ENABLE__
//ecc usert data
void btdrv_ecc_config_len_mode_sel(uint8_t ecc_len_mode) //only for ecc 1 block
{
    if(ecc_len_mode == ECC_8_BYTE_MODE)
    {
        BTDIGITAL_REG_SET_FIELD(BT_BES_FACNTL1_ADDR, 0x7, 0, 1);
    }
    else if(ecc_len_mode == ECC_16_BYTE_MODE)
    {
        BTDIGITAL_REG_SET_FIELD(BT_BES_FACNTL1_ADDR, 0x7, 0, 0);
    }
    else if(ecc_len_mode == ECC_12_BYTE_MODE)
    {
        BTDIGITAL_REG_SET_FIELD(BT_BES_FACNTL1_ADDR, 0x7, 0, 2);
    }
}

void btdrv_ecc_config_usr_tx_dat_set(ecc_trx_dat_t* p_ecc_trx_dat)
{
    BTDIGITAL_REG(0xd0220cd0) = p_ecc_trx_dat->trx_dat.dat_arr[0];
    BTDIGITAL_REG(0xd0220cd4) = p_ecc_trx_dat->trx_dat.dat_arr[1];
    BTDIGITAL_REG(0xd0220cd8) = p_ecc_trx_dat->trx_dat.dat_arr[2];
    BTDIGITAL_REG(0xd0220cdc) = p_ecc_trx_dat->trx_dat.dat_arr[3];
}

void btdrv_ecc_config_usr_rx_dat_get(ecc_trx_dat_t* p_ecc_trx_dat)
{
    p_ecc_trx_dat->trx_dat.dat_arr[0] = BTDIGITAL_REG(0xd0220cd0);
    p_ecc_trx_dat->trx_dat.dat_arr[1] = BTDIGITAL_REG(0xd0220cd4);
    p_ecc_trx_dat->trx_dat.dat_arr[2] = BTDIGITAL_REG(0xd0220cd8);
    p_ecc_trx_dat->trx_dat.dat_arr[3] = BTDIGITAL_REG(0xd0220cdc);
}


void btdrv_ecc_disable_spec_pkt_type(uint32_t ptk_type) // 1 -> disable FA, ptk type enum in bt_drv_2003_internal.h
{
    BTDIGITAL_REG(0xd0220c30) |= (1<<20);
    BTDIGITAL_REG(0xd0220c30) = (ptk_type & 0xfffff);
}


void btdrv_ecc_config_blk_mode(uint8_t ecc_blk_mode)
{
    if (ecc_blk_mode == ECC_1BLOCK)
    {
        BTDIGITAL_REG_SET_FIELD(BT_BES_FACNTL1_ADDR, 0x7ff, 5, 0xef);
    }
    else if (ecc_blk_mode == ECC_2BLOCK)
    {
        BTDIGITAL_REG_SET_FIELD(BT_BES_FACNTL1_ADDR, 0x7ff, 5, 0x1de);
    }
    else if (ecc_blk_mode == ECC_3BLOCK)
    {
        BTDIGITAL_REG_SET_FIELD(BT_BES_FACNTL1_ADDR, 0x7ff, 5, 0x2cd);
    }
}

void btdrv_ecc_config_modu_mode_acl(uint8_t ecc_modu_mode_acl)
{
    if(ecc_modu_mode_acl == ECC_8PSK)
    {
        BTDIGITAL_REG_SET_FIELD(BT_BES_CNTLX_ADDR, 3, 20, 3); //ECC 8PSK
    }
    else if(ecc_modu_mode_acl == ECC_DPSK)
    {
        BTDIGITAL_REG_SET_FIELD(BT_BES_CNTLX_ADDR, 3, 20, 2); //ECC DPSK
    }
    else if(ecc_modu_mode_acl == ECC_GFSK)
    {
        BTDIGITAL_REG_SET_FIELD(BT_BES_CNTLX_ADDR, 3, 20, 1); //ECC GFSK
    }
}

void btdrv_ecc_config_modu_mode_sco(uint8_t ecc_modu_mode_sco)
{
    if(ecc_modu_mode_sco == ECC_8PSK)
    {
        BTDIGITAL_REG_SET_FIELD(BT_BES_FACNTL0_ADDR, 3, 1, 3); //ECC 8PSK
    }
    else if(ecc_modu_mode_sco == ECC_DPSK)
    {
        BTDIGITAL_REG_SET_FIELD(BT_BES_FACNTL0_ADDR, 3, 1, 2); //ECC DPSK
    }
    else if(ecc_modu_mode_sco == ECC_GFSK)
    {
        BTDIGITAL_REG_SET_FIELD(BT_BES_FACNTL0_ADDR, 3, 1, 1); //ECC GFSK
    }
}

void btdrv_ecc_enable(bool enable)
{
    if(enable == true)
    {
        BTDIGITAL_REG(BT_BES_CNTL5_ADDR) |= 1;//ecc enable
    }
    else
    {
        BTDIGITAL_REG(BT_BES_CNTL5_ADDR) &= ~1;//disable
    }
}

void btdrv_ecc_basic_config(btdrv_ecc_basic_config_t* p_ecc_basic_config)
{
    if((p_ecc_basic_config != NULL) && (p_ecc_basic_config->ecc_mode_enable == true))
    {
        btdrv_ecc_enable(true);
        btdrv_ecc_config_modu_mode_acl(p_ecc_basic_config->ecc_modu_mode_acl);
        btdrv_ecc_config_modu_mode_sco(p_ecc_basic_config->ecc_modu_mode_sco);
        btdrv_ecc_config_blk_mode(p_ecc_basic_config->ecc_blk_mode);
        btdrv_ecc_config_len_mode_sel(p_ecc_basic_config->ecc_len_mode_sel);
    }
}


void btdrv_bid_ecc_2dh5_enable(void)
{
    BTDIGITAL_REG_SET_FIELD(BT_TRIGREG_ADDR, 1, 15, 1);
}

void btdrv_bid_ecc_cnt_act_1(uint8_t regcnteccact1)
{
    BTDIGITAL_REG_SET_FIELD(BT_BES_CNTL3_ADDR, 0xff, 0, regcnteccact1);
}

void btdrv_bid_ecc_cnt_act_2(uint8_t regcnteccact2)
{
    BTDIGITAL_REG_SET_FIELD(BT_BES_CNTL3_ADDR, 0xff, 8, regcnteccact2);
}

void btdrv_bid_ecc_frame_len(uint16_t regeccframelen)
{
    BTDIGITAL_REG_SET_FIELD(BT_BES_FACNTL1_ADDR, 0x7ff, 5, regeccframelen);
}

void btdrv_bid_ecc_bid_ecc_blk_en(uint8_t bideccen)
{
    BTDIGITAL_REG_SET_FIELD(BT_BES_ENHPCM_CNTL_ADDR, 1, 5, bideccen);
}

void btdrv_ecc_bid_enable(void)
{
    BTDIGITAL_REG_SET_FIELD(0xd0220c8c, 1, 29, 1);
}

void btdrv_ecc_bid_pkt_2dh_5_reduce_byte(uint8_t pkt2dh5reducebyte)
{
    BTDIGITAL_REG_SET_FIELD(BT_TRIGREG_ADDR, 0x7f, 18, pkt2dh5reducebyte);
}


void btdrv_ecc_bid_fa_rate_mode_acl(uint8_t regfaratemodeacl)
{

    BTDIGITAL_REG_SET_FIELD(BT_BES_CNTLX_ADDR, 3, 20, regfaratemodeacl);
}


void btdrv_bid_ecc_basic_config(void)
{
    btdrv_bid_ecc_2dh5_enable();
    btdrv_bid_ecc_cnt_act_1(205);
    btdrv_bid_ecc_cnt_act_2(250);
    btdrv_bid_ecc_frame_len(690);
    btdrv_ecc_bid_pkt_2dh_5_reduce_byte(78);
    btdrv_bid_ecc_bid_ecc_blk_en(1);
    btdrv_ecc_bid_fa_rate_mode_acl(3);
    btdrv_ecc_bid_enable();
}




void btdrv_ecc_content_config(void)
{
    btdrv_ecc_basic_config_t ecc_config;
    //ECC  config
    ecc_config.ecc_mode_enable = true;
    ecc_config.ecc_modu_mode_acl = ECC_MODU_MODE;
    ecc_config.ecc_modu_mode_sco = ECC_8PSK;
    ecc_config.ecc_blk_mode = ECC_BLK_MODE;
    ecc_config.ecc_len_mode_sel = ECC_8_BYTE_MODE;
    //setting
    btdrv_ecc_basic_config(&ecc_config);

#if __BT_BID_ECC_EN__
    btdrv_bid_ecc_basic_config();
#endif
}
#endif

void btdrv_fast_lock_config(bool fastlock_on)
{
    //useless for 2003
    return;
}

void btdrv_fast_ack_config(void)
{
    btdrv_fa_basic_config_t fa_config;
    //fast ack config
    fa_config.fa_2m_mode = false;
    fa_config.fa_vendor_syncword_en = false;
    fa_config.syncword_high = INVALID_SYNC_WORD;
    fa_config.syncword_low = INVALID_SYNC_WORD;
    fa_config.syncword_len = FA_SYNCWORD_32BIT;
    fa_config.is_new_corr = false;

    fa_config.old_mode_corr_thr = FA_OLD_CORR_VALUE;
    fa_config.new_mode_corr_thr = FA_NEW_CORR_VALUE;
    fa_config.fa_tx_gain_en = true;
    fa_config.fa_tx_gain_idx = FA_FIX_TX_GIAN_IDX;
    fa_config.fa_rx_winsz = FA_RX_WIN_SIZE;
#ifdef __FIX_FA_RX_GAIN___
    fa_config.fa_rx_gain_en = true;
    fa_config.fa_rx_gain_idx = FA_FIX_RX_GIAN_IDX;
#endif
    fa_config.enhance_fa_mode_en = false;
    fa_config.fa_tx_preamble_en = true;
    fa_config.fa_freq_table_en = false;
    fa_config.fa_freq_table0 = INVALID_FA_FREQ_TABLE;
    fa_config.fa_freq_table1 = INVALID_FA_FREQ_TABLE;
    fa_config.fa_freq_table2 = INVALID_FA_FREQ_TABLE;
    fa_config.fa_multi_mode0_en = false;
    fa_config.fa_multi_mode1_en = false;
    fa_config.fa_multi_tx_count = FA_MULTI_TX_COUNT;

    //setting
    btdrv_fa_basic_config(&fa_config);
}

void btdrv_ecc_config(void)
{
    btdrv_fast_ack_config();
#ifdef __FASTACK_ECC_ENABLE__
    btdrv_ecc_content_config();
#endif
}

void btdrv_fa_config(void)
{
    //FA trx pwrup/dn
    btdrv_fa_txpwrup_timing_setting(0x28);
    btdrv_fa_rxpwrup_timig_setting(FA_RXPWRUP_TIMING);
    btdrv_fa_margin_timig_setting(0);
    BTDIGITAL_REG(BT_BES_CNTL5_ADDR) &= ~1;//disable
    BTDIGITAL_REG_SET_FIELD(BT_TRIGREG_ADDR, 7, 15, 0);//clear bid ecc en
    BTDIGITAL_REG_SET_FIELD(BT_TRIGREG_ADDR, 7, 9, 0);//clear fa mt en
    BTDIGITAL_REG_SET_FIELD(BT_TRIGREG_ADDR, 1, 1, 1);

    BTDIGITAL_REG_SET_FIELD(BT_BES_FACNTL0_ADDR, 1, 29, 0);//clear bid ecc en
    BTDIGITAL_REG_SET_FIELD(BT_BES_CNTL5_ADDR, 1, 8, 0);//clear sbc en
}

void btdrv_afh_monitor_config(void)
{
    BTDIGITAL_REG_SET_FIELD(0xD035035C,3,0,1);//afh_avg_window[1:0]
    BTDIGITAL_REG_SET_FIELD(0xD0350240,1,12,1);//cpx_bypass[12]

    BTDIGITAL_REG_SET_FIELD(0xD035034C,0xff,0,0x40);//afh_settle_down0[7:0]
    BTDIGITAL_REG_SET_FIELD(0xD0350350,0xff,0,0x40);//afh_settle_down1[7:0]

    BTDIGITAL_REG_SET_FIELD(0xD0350354,0xff,0,0x80);//The front_end latency time, bit[8:5] which unit is us,and bit[4:0] which unit is 1/overramp_rate
    BTDIGITAL_REG_SET_FIELD(0xD0350358,0xf,0,6);//The number of channel which estimate the RSSI afh_chan_num[3:0]
    //BTDIGITAL_REG_SET_FIELD(0xD0220C04,1,23,1); //bit23 afh en
    //BTDIGITAL_REG_SET_FIELD(0xD0220C04,1,22,1); //bit22 rxgain dr
    //BTDIGITAL_REG_SET_FIELD(0xD0220C04,0xf,12,3); //bit22 rxgain dr

    //BTDIGITAL_REG_SET_FIELD(0xD0350348,0x3ff,0,0x1a4);//afh ramp up delay
    //BTDIGITAL_REG_SET_FIELD(0xD035020C,0xff,0,0x2e);//rx_startupdel[7:0]
    //btdrv_btradiowrupdn_set_rxpwrup(0x5a);
    //btdrv_fa_rxpwrup_timig_setting(0x5a);
}

void btdrv_digital_config_init_2003(void)
{
    //[21] tx pwr dr
    BTDIGITAL_REG_SET_FIELD(0xD0220C04,1,21,0);//disable tx power dr
    //[22] swagc gain dr
    BTDIGITAL_REG_SET_FIELD(0xD0220C04,1,22,0);//disable swagc gain dr

    //BT trx pwrup/dn
    btdrv_btradio_set_tx_delay(0xd);
    btdrv_48m_sys_enable();

#ifdef BT_LOG_POWEROFF
    //hal_psc_bt_enable_auto_power_down();
    //[3]:mcu2bt1 isr en
    BTDIGITAL_REG_SET_FIELD(0xD03300a0,1,3,1);
#endif

#if 0
    //close last 8k ram of bt
    hal_cmu_bt_ram_cfg();
#endif

#if 0
    btdrv_btradiowrupdn_set_rxpwrup(0x57);
    btdrv_btradiowrupdn_set_txpwrdn(0x14);
    btdrv_btradiowrupdn_set_txpwrup(0x39);
    //BLE trx pwrup/dn
    btdrv_bleradiowrupdn_set_rtrip_delay(0x0e);
    btdrv_bleradiowrupdn_set_rxpwrup(0x47);
    btdrv_bleradiowrupdn_set_txpwrdn(0x14);
    btdrv_bleradiowrupdn_set_txpwrup(0x45);
#endif

    ble_radiopwrupdn2_pack(/*uint8_t syncposition2*/ 0,
            /*uint8_t rxpwrup2*/      0x50,
            /*uint8_t txpwrdn2*/      0x07,
            /*uint8_t txpwrup2*/      0x32);
    BTDIGITAL_REG_SET_FIELD(BLE_RADIOCNTL3_ADDR,3,14,0);
    BTDIGITAL_REG_SET_FIELD(BLE_RADIOCNTL3_ADDR,3,12,0);

#ifdef __AFH_ASSESS__
    btdrv_afh_monitor_config();
#endif
}


void btdrv_ble_modem_config(void)
{
    //BLE IM txramp
    BTDIGITAL_REG_SET_FIELD(0xD0350254, 0x1f, 0, 0x1D);
    // 1M
    BTDIGITAL_REG_SET_FIELD(BLE_RADIOPWRUPDN0_ADDR, 0x7f, 8, 0xd);
    //BTDIGITAL_REG_SET_FIELD(0xD0220C14, 0x7f, 0, 0xc);
    BTDIGITAL_REG_SET_FIELD(0xD0220C14, 0x7f, 0, 0xA);
    // 2M
    BTDIGITAL_REG_SET_FIELD(BLE_RADIOPWRUPDN1_ADDR, 0x7f, 8, 0xd);
    // 2M_long_S8
    BTDIGITAL_REG_SET_FIELD(BLE_RADIOPWRUPDN2_ADDR, 0x7f, 8, 0xd);
    // 2M_long_S2
    BTDIGITAL_REG_SET_FIELD(BLE_RADIOPWRUPDN3_ADDR, 0x7f, 8, 0xd);

    /* BLE RADIOTXRXTIM0 */
    ble_radiotxrxtim0_pack(/*uint8_t rfrxtmda0*/   0,
                           /*uint8_t rxpathdly0*/  4,
                           /*uint8_t txpathdly0*/  10);

    /* BLE RADIOTXRXTIM1 */
    ble_radiotxrxtim1_pack(/*uint8_t rfrxtmda0*/   0,
                           /*uint8_t rxpathdly0*/  2,
                           /*uint8_t txpathdly0*/  10);
}

void btdrv_tx_pwr_decrease_2dbm(void)
{
    BTDIGITAL_REG_SET_FIELD(0xd0350300, 0x7, 0, 5);
    BTDIGITAL_REG_SET_FIELD(0xd0350308, 0x3ff, 0, 0x1b);
    BTDIGITAL_REG_SET_FIELD(0xd0350308, 0x3ff, 10, 0x1b);
}

void btdrv_bt_modem_config(void)
{
    BT_DRV_TRACE(1,"%s",__func__);
    //[16]:iqswap
    BTDIGITAL_REG_SET_FIELD(0xd0350214, 0x1, 16, 1);
    //[18]:txqsigned
    BTDIGITAL_REG_SET_FIELD(0xd0350214, 0x1, 18, 0);
    //[19]:txisigned
    BTDIGITAL_REG_SET_FIELD(0xd0350214, 0x1, 19, 0);
    BTDIGITAL_REG_SET_FIELD(0xd0340020, 0x1, 26, 1);
    //[2]:iqswap
    BTDIGITAL_REG_SET_FIELD(0xd0350240, 0x1, 2, 1);
    //[12]:cpx_bypass
    BTDIGITAL_REG_SET_FIELD(0xd0350240, 0x1, 12, 1);

    //[0:15]:fe_ifshift
    BTDIGITAL_REG_SET_FIELD(0xd0350244, 0xffff, 0, 0x2000);
    //[16:31]:fe_ifshift1
    BTDIGITAL_REG_SET_FIELD(0xd0350244, 0xffff, 16, 0x2ab);
    //[0:7]:tx_startupdel
    BTDIGITAL_REG_SET_FIELD(0xd0350210, 0xff, 0, 0x28); //set modem tx startup

#ifdef BT_IF_750K
    //fe_ifshift1[31:16]
    BTDIGITAL_REG_SET_FIELD(0xd0350240, 0xffff, 16, 0x1414);
    BTDIGITAL_REG_SET_FIELD(0xd0350244, 0xffff, 0, 0x1000);
#elif defined(BT_IF_1M)
    BTDIGITAL_REG_SET_FIELD(0xd0350240, 0xffff, 16, 0x1AC5);
    BTDIGITAL_REG_SET_FIELD(0xd0350244, 0xffff, 0, 0x1555);
#elif defined(BT_IF_1P05M)
    BTDIGITAL_REG_SET_FIELD(0xd0350240, 0xffff, 16, 0x1C1C);
    BTDIGITAL_REG_SET_FIELD(0xd0350240, 0xffff, 0, 0xB825);
    BTDIGITAL_REG_SET_FIELD(0xd0350244, 0xffff, 0, 0x1666);
#endif

#ifndef BT_RF_OLD_CORR_MODE
    //[12]:cpx_bypass
    //BTDIGITAL_REG_SET_FIELD(0xd0350240, 0x1, 12, 0);
    //[0]:corr_new_en
    BTDIGITAL_REG_SET_FIELD(0xd0350230, 0x1, 0, 1);
    //[12:25]:lau_h
    BTDIGITAL_REG_SET_FIELD(0xd03502ec, 0x3fff, 12, 0x94d);

    BTDIGITAL_REG_SET_FIELD(0xd0350224, 0xff, 0, 0x60);
    BTDIGITAL_REG_SET_FIELD(0xd0350224, 0xff, 8, 0x60);
    BTDIGITAL_REG_SET_FIELD(0xd0350224, 0xff, 16, 0x80);
    BTDIGITAL_REG_SET_FIELD(0xd0350224, 0xff, 24, 0x62);
#endif

    BTDIGITAL_REG_SET_FIELD(0xD0350014, 0xffff, 0, 0x48C0);
    BTDIGITAL_REG_SET_FIELD(0xD0350014, 0xffff, 16, 0xE1C0);

#ifdef LAURENT_ALGORITHM

    //[0:10]te_timeinit
    //[11:18]before_timeinit
    BTDIGITAL_REG_SET_FIELD(0xd03502c4, 0x7ff, 0, 0x336);
    BTDIGITAL_REG_SET_FIELD(0xd03502c4, 0xff, 11, 0x94);
    //[0]lau_enable
    //[5:6]lau_gain
    BTDIGITAL_REG_SET_FIELD(0xd03502e8, 1, 0, 1);
    BTDIGITAL_REG_SET_FIELD(0xd03502e8, 3, 5, 2);
#endif

    BTDIGITAL_REG_SET_FIELD(0xd03502c4, 0xffff,  0, 0xFAE8);
    BTDIGITAL_REG_SET_FIELD(0xd03502c4, 0xffff, 16, 0x08E1);

    //BTDIGITAL_REG_SET_FIELD(BT_RADIOPWRUPDN_ADDR,0x1f, 0, 0x1A);// tx off ramping
    //BTDIGITAL_REG_SET_FIELD(BT_RADIOPWRUPDN_ADDR,0x1f, 8, 0x1f);

    btdrv_btradiowrupdn_set_rxpwrup(0x35);

    btdrv_btradiowrupdn_set_txpwrdn(0x0d);
    btdrv_btradiowrupdn_set_txpwrup(0x30);
    BTDIGITAL_REG_SET_FIELD(0xd0350340, 0xf, 0, 5);  //dsg_den[3:0]

    //decrease EDR gain for more BDR pwr wo EDR ACP violation
    BTDIGITAL_REG_SET_FIELD(0xD0350344, 0x1f, 0, DIGITAL_DEC_EDR_POWER_VAL);

    BTDIGITAL_REG_SET_FIELD(0xD0350308, 0xffff, 16, 0x4010);

    BTDIGITAL_REG_SET_FIELD(0xD0350300, 0xffff, 0, 0x0404);
    BTDIGITAL_REG_SET_FIELD(0xD0350300, 0xffff, 16, 0x3FF0);
#ifdef PWR_CTRL_8DBM
    BTDIGITAL_REG_SET_FIELD(0xD0350308, 0xffff, 0, 0x4812);
    BTDIGITAL_REG_SET_FIELD(0xD0350300, 0xffff, 0, 0x0405);
#else
    BTDIGITAL_REG_SET_FIELD(0xD0350308, 0xffff, 0, 0x2C0B);
#endif

#ifdef BT_1M_N98
    BTDIGITAL_REG_SET_FIELD(0xD035000C, 0xffff, 0, 0x0c03);
    BTDIGITAL_REG_SET_FIELD(0xD035000C, 0xffff, 16, 0x0200);

    BTDIGITAL_REG_SET_FIELD(0xD0350004, 0xffff, 0, 0x0045);
    BTDIGITAL_REG_SET_FIELD(0xD0350004, 0xffff, 16, 0x0291);
#endif

    //guard time time out : 512->51E
    //3M packet Loopback Test TX will have a probability of TX underdrver, and RX will have PER packet loss under a large signal.
    BTDIGITAL_REG_SET_FIELD(0xD0220534, 0xffff, 0, 0x051E);

    BTDIGITAL_REG_SET_FIELD(0xD0350044, 0xffff, 0, 0x8032);
    BTDIGITAL_REG_SET_FIELD(0xD0350044, 0xffff, 16, 0x0325);

    BTDIGITAL_REG_SET_FIELD(0xD03502C8, 0xffff, 0, 0x584C);
    BTDIGITAL_REG_SET_FIELD(0xD03502C8, 0xffff, 16, 0x0160);

    BTDIGITAL_REG_SET_FIELD(0xD035036C, 0xffff, 0, 0x1000);
    BTDIGITAL_REG_SET_FIELD(0xD035036C, 0xffff, 16, 0x0300);

    BTDIGITAL_REG_SET_FIELD(0xD0350010, 0xffff, 0, 0x0064);
    BTDIGITAL_REG_SET_FIELD(0xD0350010, 0xffff, 16, 0x0010);

    factory_section_wifi_t* factory_data = factory_section_get_wifi_data();
    if(factory_data)
    {
        TRACE(0, "burn the factory,power from factory");
        BTDIGITAL_REG(0xd0350300)= factory_data->data.bt_tx_power[0];
        BTDIGITAL_REG(0xd0350308)= factory_data->data.bt_tx_power[1];//basic digital gain
        BTDIGITAL_REG(0xd0350340)= factory_data->data.bt_tx_power[2];
        BTDIGITAL_REG(0xd0350344)= factory_data->data.bt_tx_power[3];//edr digital gain
    }
    if(hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_7)
    {
        BTDIGITAL_REG_SET_FIELD(0xD0350328, 0x1, 15, 1);//3m_independent_en gain table 0xd035_0328 bit15=1
        BTDIGITAL_REG_SET_FIELD(0xD03502C0, 0x1, 7, 1);//3m_independent_en rrc gain 0xd035_02C0 bit7=1
    }

}

void btdrv_ibrt_dual_ant_ctrl_config(void)
{
    // TODO: Change the iomux in the following function
    hal_iomux_set_bt_rf_sw(0, 1);

    //P0_1 = fa flag
    BTDIGITAL_REG_SET_FIELD(0xd0220050, 0xffff,  0, 0xb4b4);
    BTDIGITAL_REG(0xd0340000) |= (1<<9);
    BTDIGITAL_REG_SET_FIELD(0xd0340000, 0xffff,  16, 0xA362);

    /*     //P1_1 = fa flag
        BTDIGITAL_REG_SET_FIELD(0xd0220050, 0xffff,  0, 0xb4b4);
        BTDIGITAL_REG(0xd0340000) |= (1<<9);
        BTDIGITAL_REG_SET_FIELD(0xd0340000, 0xffff,  16, 0x2362);*/
}

void bt_drv_config_lmp_filter_table(void)
{
}

uint32_t data_backup_tbl[] =
{
    0xD0350014,
    0xd0350210,
//  0xd0350214,
    0xd0350224,
    0xd0350228,
//  0xd0350230,
//  0xd0350240,
//  0xd0350244,
//  0xd03502c4,
    0xd03502e8,
    0xd03502ec,
    0xd0220540,
    0xd0220548,
    0xd0220550,
    0xD0350254,
    0xD035025c,
    0xd0350340,
    0xD0350344,
    0xD0350004,
    0xD035000C,
    0xd0350368,

    //afh
    //0xD0220C04,backup in controller
    0xD035020C,
    //0xD0350240,
    0xD0350348,
    0xD035034C,
    0xD0350350,
    0xD0350354,
    0xD0350358,
    0xD035035C,
};


void btdrv_config_end(void)
{
#ifdef __CLK_GATE_DISABLE__
    BTDIGITAL_REG(0xD0330024) &= (~(1<<5));
    BTDIGITAL_REG(0xD0330024) |= (1<<18);
    //BTDIGITAL_REG(0xD0330038) |= (1<<20);
#else
    BTDIGITAL_REG(0xD0330024) |= (1<<5);
    BTDIGITAL_REG(0xD0330024) &= ~(1<<18);
    //BTDIGITAL_REG(0xD0330038) &= ~(1<<20);
#endif
    bt_drv_reg_op_data_bakeup_init();
    bt_drv_reg_op_data_backup_write(&data_backup_tbl[0],ARRAY_SIZE(data_backup_tbl));
}

extern void bt_drv_reg_op_controller_mem_log_config(void);
void btdrv_config_init(void)
{
    BT_DRV_TRACE(1,"%s",__func__);
#ifdef BT_LOG_POWEROFF
    enum HAL_CHIP_METAL_ID_T metal_id = hal_get_chip_metal_id();
#endif

    if (btdrv_get_lmp_trace_enable())
    {
        //enable lmp trace
        bt_lmp_record[0] = 1;
        bt_lmp_record[1] = 1;
        btdrv_cfg_tbl[0].is_act= BTDRV_CONFIG_ACTIVE;
        ASSERT((btdrv_cfg_tbl[0].opcode == HCI_DBG_LMP_MESSAGE_RECORD_CMD_OPCODE), "lmp config not match");
    }

    if (btdrv_get_controller_trace_level() != BTDRV_INVALID_TRACE_LEVEL)
    {
        //enable controller trace
        bt_common_setting_2003[0] = btdrv_get_controller_trace_level();
    }
#ifdef BT_LOG_POWEROFF
    if(metal_id >= HAL_CHIP_METAL_ID_0)
    {
        bt_common_setting_2003[25] = 1;
    }
#endif

    for(uint8_t i=0; i<ARRAY_SIZE(btdrv_cfg_tbl); i++)
    {
        //BT other config
        if(btdrv_cfg_tbl[i].is_act == BTDRV_CONFIG_ACTIVE)
        {
            btdrv_send_cmd(btdrv_cfg_tbl[i].opcode,btdrv_cfg_tbl[i].parlen,btdrv_cfg_tbl[i].param);
            btdrv_delay(1);
        }
    }

    if (g_metal_id < HAL_CHIP_METAL_ID_8) {
        if (btdrv_cfg_tbl_2003.is_act == BTDRV_CONFIG_ACTIVE) {
            btdrv_send_cmd(btdrv_cfg_tbl_2003.opcode,btdrv_cfg_tbl_2003.parlen,btdrv_cfg_tbl_2003.param);
            btdrv_delay(1);
        }
    } else {
        if (btdrv_cfg_tbl_2006.is_act == BTDRV_CONFIG_ACTIVE) {
            btdrv_send_cmd(btdrv_cfg_tbl_2006.opcode,btdrv_cfg_tbl_2006.parlen,btdrv_cfg_tbl_2006.param);
            btdrv_delay(1);
        }
    }

    bt_drv_reg_op_init_rssi_setting();
    bt_drv_reg_op_init_sleep_para();
    btdrv_digital_config_init_2003();
#ifdef __HW_AGC__
    bt_drv_reg_op_init_hwagc_corr_gain();
#endif

    btdrv_bt_modem_config();
    btdrv_ble_modem_config();

#ifdef IBRT_DUAL_ANT_CTRL
    btdrv_ibrt_dual_ant_ctrl_config();
#endif

    bt_drv_reg_op_ble_rx_gain_thr_tbl_set();
#if 0
    if(btdrv_get_controller_trace_dump_enable())
    {
        bt_drv_reg_op_controller_mem_log_config();
    }

    bt_drv_config_lmp_filter_table();

#ifdef __EBQ_TEST__
    bt_drv_reg_op_ebq_test_setting();
#endif
#endif
#ifdef __SW_TRIG__
    btdrv_play_trig_mode_set(1);
#endif

}

bool btdrv_is_ecc_enable(void)
{
    bool ret = false;
#ifdef  __FASTACK_ECC_ENABLE__
    ret = true;
#endif
    return ret;
}

////////////////////////////////////////test mode////////////////////////////////////////////




void btdrv_sleep_config(uint8_t sleep_en)
{
    sleep_param[0] = sleep_en;
    btdrv_send_cmd(HCI_DBG_SET_SLEEP_SETTING_CMD_OPCODE,8,sleep_param);
    btdrv_delay(1);
}

void btdrv_feature_default(void)
{
#ifdef __EBQ_TEST__
    const uint8_t feature[] = {0xBF, 0xFE, 0xCF,0xFe,0xdb,0xFF,0x5b,0x87};
#else
    const uint8_t feature[] = {0xBF, 0xeE, 0xCD,0xFe,0xc3,0xFf,0x7b,0x87};
#endif
    btdrv_send_cmd(HCI_DBG_SET_LOCAL_FEATURE_CMD_OPCODE,8,feature);
    btdrv_delay(1);
}

const uint8_t test_mode_addr[6] = {0x77,0x77,0x77,0x77,0x77,0x77};
void btdrv_test_mode_addr_set(void)
{
    return;

    btdrv_send_cmd(HCI_DBG_SET_BD_ADDR_CMD_OPCODE,sizeof(test_mode_addr),test_mode_addr);
    btdrv_delay(1);
}

uint8_t meInit_param_get_entry_idx = 0;

int btdrv_meinit_param_init(void)
{
    int size = 0;
    if ((me_init_param[0][0] == 0xffffffff) &&
        (me_init_param[0][1] == 0xffffffff))
    {
        size = -1;
    }
    meInit_param_get_entry_idx = 0;
    return size;
}

int btdrv_meinit_param_remain_size_get(void)
{
    int remain_size;
    if ((me_init_param[0][0] == 0xffffffff) &&
        (me_init_param[0][1] == 0xffffffff))
    {
        return -1;
    }
    remain_size = ARRAY_SIZE(me_init_param) - meInit_param_get_entry_idx;
    return remain_size;
}

int btdrv_meinit_param_next_entry_get(uint32_t *addr, uint32_t *val)
{
    if (meInit_param_get_entry_idx > (ARRAY_SIZE(me_init_param) - 1))
        return -1;
    *addr = me_init_param[meInit_param_get_entry_idx][0];
    *val = me_init_param[meInit_param_get_entry_idx][1];
    meInit_param_get_entry_idx++;
    return 0;
}

enum
{
    SYNC_IDLE,
    SYNC_64_ORG,
    SYNC_68_ORG,
    SYNC_72_ORG,
    SYNC_64_NEW,
    SYNC_68_NEW,
    SYNC_72_NEW,
};

enum
{
    SYNC_64_BIT,
    SYNC_68_BIT,
    SYNC_72_BIT,
};
bool btdrv_get_lmp_trace_enable(void)
{
    return g_lmp_trace_enable;
}
void btdrv_set_lmp_trace_enable(void)
{
    g_lmp_trace_enable = true;
}
void btdrv_set_controller_trace_enable(uint8_t trace_level)
{
    g_controller_trace_level = trace_level;
}

uint8_t btdrv_get_controller_trace_level(void)
{
    return g_controller_trace_level;
}

const struct rssi_txpower_link_thd* btdrv_get_tws_link_txpwr_thd_ptr(void)
{
    return &tws_link_txpwr_thd;
}

static uint32_t StringToHex(char *str, unsigned char *out, unsigned int *outlen)
{
    char *p = str;
    char high = 0, low = 0;
    int tmplen = strlen(p), cnt = 0;
    uint32_t result = 0;
    tmplen = strlen(p);
    while(cnt < (tmplen / 2))
    {
        high = ((*p > '9') && ((*p <= 'F') || (*p <= 'f'))) ? *p - 48 - 7 : *p - 48;
        low = (*(++ p) > '9' && ((*p <= 'F') || (*p <= 'f'))) ? *(p) - 48 - 7 : *(p) - 48;
        out[cnt] = ((high & 0x0f) << 4 | (low & 0x0f));
        p ++;
        result |= out[cnt] << (cnt *8);
        cnt ++;
    }

    if(tmplen % 2 != 0)
    {
        out[cnt] = ((*p > '9') && ((*p <= 'F') || (*p <= 'f'))) ? *p - 48 - 7 : *p - 48;
        result |= out[cnt] << (cnt *8);
    }

    if (outlen != NULL)
        *outlen = tmplen / 2 + tmplen % 2;

    return result;
}

#define BTDIGITAL_REG(a) (*(volatile uint32_t *)(a))
void btdrv_set_tport(int argc, char *argv[])
{
    uint8_t hexval[8];
    uint32_t val = 0, reg = 0;
    val = StringToHex(argv[1], hexval, NULL);
    TRACE(1,"0xd0220050:0x%08x",val);
    BTDIGITAL_REG(0x40086014) = 0x00000F0F;
    BTDIGITAL_REG(0x40086018) = 0x00000005;
    reg = BTDIGITAL_REG(0x40086008);
    BTDIGITAL_REG(0x40086008) = (reg&0xFFFF0000)|0xeeee;
    BTDIGITAL_REG(0x40086004) = 0x0FFFeeee;
    BTDIGITAL_REG(0xd0340000) = 0x23620200;
    BTDIGITAL_REG(0xd0220050) = val;
}