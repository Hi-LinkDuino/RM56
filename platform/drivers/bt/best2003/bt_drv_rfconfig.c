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
#include "plat_types.h"
#include "hal_i2c.h"
#include "hal_uart.h"
#include "bt_drv.h"
#ifdef RTOS
#include "cmsis_os.h"
#endif
#include "cmsis.h"
#include "hal_cmu.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "tgt_hardware.h"
#include "bt_drv_internal.h"
#include "bt_drv_2003_internal.h"
#include "hal_chipid.h"
#include "bt_drv_interface.h"
#include "bt_drv_reg_op.h"
#include "pmu.h"
#include "iqcorrect.h"
#include "hal_btdump.h"
#include "bt_drv_iq_common.h"
#include "factory_section.h"

#define AUTO_CAL                                0

#ifndef BT_RF_MAX_XTAL_TUNE_PPB
// Default 10 ppm/bit or 10000 ppb/bit
#define BT_RF_MAX_XTAL_TUNE_PPB                 10000
#endif

#ifndef BT_RF_XTAL_TUNE_FACTOR
// Default 0.2 ppm/bit or 200 ppb/bit
#define BT_RF_XTAL_TUNE_FACTOR                  200
#endif


#define XTAL_FCAP_NORMAL_SHIFT                  0
#define XTAL_FCAP_NORMAL_MASK                   (0x1FF << XTAL_FCAP_NORMAL_SHIFT)
#define XTAL_FCAP_NORMAL(n)                     BITFIELD_VAL(XTAL_FCAP_NORMAL, n)

#define BT_XTAL_CMOM_DR                         (1 << 9)

#define RF_REG_XTAL_FCAP                        0x95
#define RF_REG_XTAL_CMOM_DR                     0x95

#define SPI_TRIG_RX_NEG_TIMEOUT                 MS_TO_TICKS(3)

#define SPI_TRIG_NEG_TIMEOUT                    SPI_TRIG_RX_NEG_TIMEOUT


enum BIT_OFFSET_CMD_TYPE_T
{
    BIT_OFFSET_CMD_STOP = 0,
    BIT_OFFSET_CMD_START,
    BIT_OFFSET_CMD_ACK,
};

static uint16_t xtal_fcap = DEFAULT_XTAL_FCAP;
static uint16_t init_xtal_fcap = DEFAULT_XTAL_FCAP;

struct bt_drv_tx_table_t
{
    uint16_t tbl[16][3];
};


struct RF_SYS_INIT_ITEM
{
    uint16_t reg;
    uint16_t set;
    uint16_t mask;
    uint16_t delay;
};

static const struct RF_SYS_INIT_ITEM rf_sys_init_tbl[] =
{
    {0x00E9,0x0000,0x000F,1},
};
static const struct RF_SYS_INIT_ITEM rf_sys_init_tbl_1[] =
{
};
#if 0
static const struct RF_SYS_INIT_ITEM rf_sys_init_tbl_1[] =
{
};
#endif
#define REG_EB_VAL 0x083f
#define REG_181_VAL (0x00bf)
#define REG_EC_VAL 0x081f
#define REG_182_VAL 0x00bf
#define REG_ED_VAL 0x091f
#define REG_183_VAL 0x00bf
#define REG_EE_VAL 0x0b1f
#define REG_184_VAL 0x00bf
#define REG_EF_VAL 0x00c7
#define REG_185_VAL 0x00bf
#define REG_F0_VAL 0x0147
#define REG_186_VAL 0x00bf
#define REG_F1_VAL 0x0347
#define REG_187_VAL 0x00af
#define REG_F2_VAL 0x0347
#define REG_188_VAL 0x008d

const uint16_t rf_init_tbl_1[][3] =
{
    #if 0
    {0x0081,0x0104,1},
    {0x0082,0x0a10,1},
    {0x0083,0x0a0a,1},
    {0x0084,0x1320,1},//delay pa on time to cover locking ripple by walker mail 20201020
    {0x008b,0x0806,1},
    {0x008f,0x0004,1},
    {0x009e,0x0052,1},//reg_bt_rfpll_cnt_time_tx[7:5] by walker mail 20201016
    {0x009f,0x8000,1},

    {0x00a0,0x2040,1},
    {0x00a1,0x0052,1},//reg_bt_rfpll_cnt_time_rx[7:5] by walker mail 20201016
    {0x00a5,0x8400,1},//reg_bt_rfpll_dither_bypass_tx by xulicheng 20201106
    {0x00b7,0x20bf,1},//tx_gain_sel_en
    {0x00c0,0x2607,1},//i2v gain =7
    {0x00E8,0x13FF,1},

    {0x01E4,0x0009,1},

    {0x0283,0x0910,1},
    {0x0297,0x3F10,1},//Increase pll bandwidth and speed up locking by Xulilcheng mail 20210104
    {0x0299,0x1C14,1},
    {0x029e,0x3848,1},
    {0x029f,0x0040,1},// improve ACP by walker mail 20201208
    {0x02a0,0x7230,1},// tmx bsp ictrl
    {0x02a2,0x2800,1},//decrease pa current for better MC by walker mail 20201020
    {0x02b6,0x48A8,1},
    {0x02b9,0x1950,1},
    {0x02bb,0x1540,1},

    {0x0389,0x1ac0,1},///padrvgain=0
    {0x03ad,0x0038,1},//lna_ic=110

    {0x03b0,0x1796,1},//strengthen the dac clock drive capability by xulicheng 20201106
#if 0
    {0x00E9,0x110d,1},//lna itxb=1; rfflt_buf_pu=0
    {0x03ad,0x0038,1},//lna_ic=110
    {0x00c0,0x2607,1},//i2v gain =7
#endif

    {0x00E8,0x0000,1},//release lna dr
    {0x00c0,0x0007,1},//i2v gain =7
    {0x03ad,0x0000,1},//lna_ic=110

    {0x01E4,0x0000,1},
    {0x01E7,0x2494,1},
    {0x01E8,0x0049,1},
    {0x01Ed,0x9248,1},
    {0x01E4,0x0024,1},

    {0x2C1,0x117F,1},   //increase sar adc voltage by Fangfei
    //[vco pnoise]
    //{0x2C2,0x6768,1},
    //{0x2C3,0x23C0,1},
    //{0x2CB,0x8174,1},

    //[lowpower pll] by xwy
    {0x3ca,0x3900,1},
    {0x2c2,0x4728,1},//slightly increase the current of rfpll IF divider and rxvco by xulicheng 20201109
    {0x2c4,0x0582,1},//improve driving ability at low temperature by xulicheng mail 20201221
    {0x2c7,0x2924,1},//slightly increase the current of the rfpll IF divider to improve reliability by xulicheng 20201118
    {0x2c8,0x1971,1},
    {0x2c9,0x8294,1},//for stable trx pll
    {0x2cb,0x2124,1},

    {0x2a5,0x0101,1},///iqcal pu =0
    {0x1e7,0x2492,1},////set by luobin
    {0x1ed,0x9249,1},
    {0xbc,0x4e10,1},

#ifdef BT_I2V_VAL_RD
////fangfei add for block det
    {0x8c,0x1000,1},
    {0xb6,0x4c82,1},//enable hwagc en
    {0xb7,0x28b1,1},//tx_gain_sel_en
    {0x0181,0x4436,1},//smp_sel=010
    {0x0182,0x803e,1},
    {0x0183,0xc001,1},//stop_en_1=111; restart_disable=0000
    {0x0189,0x0310,1},//burst_mode_en=1
    {0x018a,0x1010,1},//burst_mode_en=0
    {0x018b,0x1f1f,1},//burst_mode_en=0
    {0x018e,0x0305,1},//lna_threshold
    {0x018f,0x0306,1},//i2v threshold
    {0x0195,0x04bb,1},//i2v threshold
    {0x01a1,0x101c,1},//lna_gain_value
    {0x01a2,0x2f38,1},//lna_rssi_lim
    {0x01a3,0x5f65,1},//i2v_rssi_lim
    {0x01a4,0x0075,1},//i2v_max gain rssi lim
    {0x01a6,0x0006,1},//rssi_lim_rxpwr
    {0x01a8,0x0003,1},//max_gain_rssi_lim_en
    {0x02be,0x1138,1},//abb_pdt_vcm
    {0x03ab,0x0280,1},//abb_pdt_op_gain
#endif

#ifdef BT_IF_750K
    {0x00b3,0x0300,1},//bt_rfpll_lo_freq
    //{0x00bb,0x4500,1},//filter cap * 1.25; flt_bw=001
    {0x00bd,0x0311,1},
    //{0x02b9,0x1958,1},//filter cplx res=1.25M
#elif defined(BT_IF_1M)
    {0x00b3,0x0400,1},
    {0x00bd,0x0311,1},//filter lowpass
#elif defined(BT_IF_1P05M)
    {0x00b3,0x0433,1},
    {0x00bd,0x0311,1},//filter lowpass
#endif

    {0x00b6,0x4c80,1},  //disable hwagc en
#endif

    {0x0001,0x0101},  //Modify the timing, advance the pll start time

    //rx gain
    {0x0039,0X6878,1},//gain idx 0
    {0x0041,0X034B,1},

    {0x003a,0X485C,1},//gain idx 1
    {0x0042,0X01C6,1},

    {0x003b,0X285C,1},//gain idx 2
    {0x0043,0X01CD,1},

    {0x003c,0X2835,1},//gain idx 3
    {0x0044,0X01C4,1},

    {0x003d,0X2905,1},//gain idx 4
    {0x0045,0X02CD,1},

    {0x003e,0X0115,1},//gain idx 5
    {0x0046,0X01CB,1},

    {0x003f,0X0305,1},//gain idx 6
    {0x0047,0X01B3,1},

    {0x0040,0X0305,1},//gain idx 7
    {0x0048,0X019A,1},

    {0x0023,0X28BF,1},
    {0x002E,0XC0E0,1},

    {0x004B,0X0190,1},

    //[tstbuf]
    {0x00a6,0x1e4a,1},
    //{0x002b,0X0F00,1},
    //{0x0051,0x27f9,1},
    {0x002b,0x0000,1},
    {0x0051,0x0000,1},
    //{0x00bf,0x3c03,1},
    {0x008f,0xE732,1},
    {0x0090,0x3848,1},
    {0x0092,0x8310,1},
    {0x0095,0x0030,1},
    {0x0097,0x1578,1},
    {0x009e,0x160C,1},
    {0x0081,0x0703,1},

    {0x001f,0x0433,1},  //BT_IF_1P05M
    {0x0027,0x2A00,1},
    {0x0028,0x6E10,1},
    {0x0029,0x0311,1},

#ifdef PWR_CTRL_8DBM
    {0x00CB,0X3F22,1},
    {0x00CC,0xE113,1},
#else
    {0x00CB,0X3F72,1},
    {0x00CC,0xE173,1},
#endif
    //{0x002C,0X2657,1},
    {0x002C,0X0006,1},
    {0x00C1,0x1AC0,1},
    {0x00BF,0X1002,1},
    {0x00C4,0X5C80,1},
    {0x00CF,0XA61B,1},
    {0x00D1,0X0200,1},
    {0x002D,0x3B20,1},
    //[bt rfpll type1]
    {0x0012,0x0092,1},
    {0x00CA,0x0000,1},  //Change Bt's rfpll from typeI to typeII to improve its performance by XuliCheng
    {0x00CE,0x00A4,1},  //Change Bt's rfpll from typeI to typeII to improve its performance by XuliCheng
    {0x00A0,0x1908,1},  //Reduce the bandwidth of txpll and improve the harmonic point evm
    {0x00A2,0x0294,1},
    {0x00A3,0x23ff,1},
    {0x00A4,0x8584,1},
    {0x0013,0x8000,1},
    {0x0014,0x2040,1},
    {0x0017,0x4400,1},
};

#ifdef __HW_AGC__
const uint16_t rf_init_tbl_1_hw_agc[][3] = //hw agc table
{
   //{0x00b6,0x4c82,1},//enable hwagc en
    //rxgain
    {0x00cd,0x03c8,1},
    {0x00ce,0x00c8,1},
    {0x00cf,0x481c,1},

    {0x01e7,0x0121,1},///should check by luobin andn fangfei
//    {0x00E8,0x0000,1},//release lna dr
//    {0x00E9,0x1100,1},//release sw itxb dr
//    {0x03ad,0x0000,1},//release lna_ic
//    {0x00bf,0x0000,1},//release filter dr
    {0x00c0,0x0000,1},//release adc sel gain
    {0x01E4,0x0020,1},//release i2v rin


    {0x0181,0x4436,1},//smp_sel=010
    {0x0182,0x803f,1},
    {0x0183,0xe001,1},//stop_en_1=111; restart_disable=0000
    {0x0189,0x0010,1},//burst_mode_en=0
    {0x018e,0x0305,1},//lna_threshold
    {0x018f,0x0306,1},//i2v threshold
    {0x0195,0x04bb,1},//i2v threshold
    {0x01a1,0x101c,1},//lna_gain_value
    {0x01a2,0x2f38,1},//lna_rssi_lim
    {0x01a3,0x5f65,1},//i2v_rssi_lim
    {0x01a4,0x0075,1},//i2v_max gain rssi lim
    {0x01a6,0x0006,1},//rssi_lim_rxpwr
    {0x01a8,0x0003,1},//max_gain_rssi_lim_en
    {0x02be,0x1138,1},//abb_pdt_vcm
    {0x03ab,0x0280,1},//abb_pdt_op_gain
};
#endif //__HW_AGC__

void bt_drv_rf_set_bt_hw_agc_enable(bool enable)
{
    uint16_t val_e2 = 0;
    btdrv_read_rf_reg(0x22,&val_e2);
    if(enable)
    {
        //open rf bt hw agc mode
        val_e2 |= (1<<1);
    }
    else
    {
        //close rf bt hw agc mode
        val_e2 &= ~(1<<1);
    }
    btdrv_write_rf_reg(0x22,val_e2);
}

void bt_drv_rf_set_ble_hw_agc_enable(bool enable)
{
    uint16_t val_e2 = 0;
    btdrv_read_rf_reg(0x23,&val_e2);
    if(enable)
    {
        //open rf ble hw agc mode
        val_e2 |= (1<<11);
    }
    else
    {
        //close rf ble hw agc mode
        val_e2 &= ~(1<<11);
    }
    btdrv_write_rf_reg(0x23,val_e2);
}

void bt_drv_rf_set_afh_monitor_gain(void)
{
    return;
}

uint32_t btdrv_rf_get_max_xtal_tune_ppb(void)
{
    return BT_RF_MAX_XTAL_TUNE_PPB;
}

uint32_t btdrv_rf_get_xtal_tune_factor(void)
{
    return BT_RF_XTAL_TUNE_FACTOR;
}

void btdrv_rf_init_xtal_fcap(uint32_t fcap)
{
    xtal_fcap = SET_BITFIELD(xtal_fcap, XTAL_FCAP_NORMAL, fcap);
    btdrv_write_rf_reg(RF_REG_XTAL_FCAP, xtal_fcap);
    init_xtal_fcap = xtal_fcap;
}

uint32_t btdrv_rf_get_init_xtal_fcap(void)
{
    return GET_BITFIELD(init_xtal_fcap, XTAL_FCAP_NORMAL);
}

uint32_t btdrv_rf_get_xtal_fcap(void)
{
    return GET_BITFIELD(xtal_fcap, XTAL_FCAP_NORMAL);
}

void btdrv_rf_set_xtal_fcap(uint32_t fcap, uint8_t is_direct)
{
    xtal_fcap = SET_BITFIELD(xtal_fcap, XTAL_FCAP_NORMAL, fcap);
    btdrv_write_rf_reg(RF_REG_XTAL_FCAP, xtal_fcap);
}

int btdrv_rf_xtal_fcap_busy(uint8_t is_direct)
{
    return 0;
}

void btdrv_rf_bit_offset_track_enable(bool enable)
{
    return;
}

uint32_t btdrv_rf_bit_offset_get(void)
{
    return 0;
}

uint16_t btdrv_rf_bitoffset_get(uint8_t conidx)
{
    uint16_t bitoffset = 0;
    bitoffset = bt_drv_reg_op_bitoff_getf(conidx);
    return bitoffset;
}

void btdrv_rf_log_delay_cal(void)
{
    unsigned short read_value;
    unsigned short write_value;
    BT_DRV_TRACE(1,"%s", __func__);
    BTDIGITAL_REG(0xd0340020) = 0x010e01c0;
    BT_DRV_TRACE(1,"0xd0340020 =%x\n",BTDIGITAL_REG(0xd0340020) );

    btdrv_write_rf_reg(0xd4, 0x000f);
    btdrv_write_rf_reg(0xd5, 0x4000);
    btdrv_write_rf_reg(0xd2, 0x1003);
    btdrv_write_rf_reg(0xa7, 0x004e);
    btdrv_write_rf_reg(0xd4, 0x0000);
    btdrv_write_rf_reg(0xd5, 0x4002);

    BTDIGITAL_REG(0xd0340020) = 0x030e01c1;
    BT_DRV_TRACE(1,"0xd0340020 =%x\n",BTDIGITAL_REG(0xd0340020) );

    btdrv_delay(1);

    btdrv_write_rf_reg(0xd2, 0x5003);

    btdrv_delay(1);

    btdrv_read_rf_reg(0x1e2, &read_value);
    BT_DRV_TRACE(1,"0x1e2 read_value:%x\n",read_value);
    if(read_value == 0xff80)
    {
        btdrv_write_rf_reg(0xd3, 0xffff);
    }
    else
    {
        write_value = ((read_value>>7)&0x0001) | ((read_value & 0x007f)<<1) | ((read_value&0x8000)>>7) | ((read_value&0x7f00)<<1);
        BT_DRV_TRACE(1,"d3 write_value:%x\n",write_value);
        btdrv_write_rf_reg(0xd3, write_value);
    }
    btdrv_delay(1);

    BTDIGITAL_REG(0xd0340020) = 0x010e01c0;
    BT_DRV_TRACE(1,"0xd0340020 =%x\n",BTDIGITAL_REG(0xd0340020) );


    btdrv_write_rf_reg(0xd4, 0x000f);
    btdrv_write_rf_reg(0xd2, 0x1003);
    btdrv_write_rf_reg(0xd5, 0x4000);

}

void btdrv_rf_log_delay_cal_init(void)
{
    unsigned short read_value;
    uint16_t rf_f5_13_7, rf_f5_6_0 = 0;

    BT_DRV_TRACE(1,"%s", __func__);
    //open RX ON for calabration
    BTDIGITAL_REG(0xd0220c00) = 0x000A002A;
    //delay to preventing BT/MCU RF SPI conflict
    btdrv_delay(10);

    //init
    btdrv_write_rf_reg(0x29c, 0xffb3);
    btdrv_write_rf_reg(0x29d, 0x001f);

    //cal flow
    btdrv_write_rf_reg(0xc2, 0x0);
    //bit9 : reg_rstn_log_cal
    btdrv_read_rf_reg(0xbe, &read_value);
    read_value |= (1 << 9);
    btdrv_write_rf_reg(0xbe, read_value);
    //reg_bt_log_cal_en
    btdrv_read_rf_reg(0x3af, &read_value);
    read_value |= (1 << 0);
    btdrv_write_rf_reg(0x3af, read_value);

    //log_cal_value[14:0]
    btdrv_read_rf_reg(0xf5, &read_value);
    BT_DRV_TRACE(0,"0xf5=0x%x",read_value);
    rf_f5_13_7 = (read_value & 0x3f80) >> 7;
    rf_f5_6_0  = (read_value & 0x7f);

    //0xc2 reg_fdata1[7:1] | 0xc2 reg_fdata2[15:9] | bit0:reg_fdata1_dr | bit8:reg_fdata2_dr
    btdrv_read_rf_reg(0xc2, &read_value);
    read_value = 0x0;
    read_value |= ((rf_f5_6_0 << 1) | (rf_f5_13_7 << 9) | (1<<0) | (1<<8));
    btdrv_write_rf_reg(0xc2, read_value);
    BT_DRV_TRACE(0,"0xc2=0x%x",read_value);

    //cal done
    btdrv_read_rf_reg(0xbe, &read_value);
    read_value &= ~(1 << 9);
    btdrv_write_rf_reg(0xbe, read_value);

    btdrv_read_rf_reg(0x3af, &read_value);
    read_value &= ~(1 << 0);
    btdrv_write_rf_reg(0x3af, read_value);

    //delay to preventing BT/MCU RF SPI conflict
    btdrv_delay(10);
    BTDIGITAL_REG(0xd0220c00) = 0x0;
}

void btdrv_tx_iq_manual_cal(void)
{
#if 0
    uint16_t read_val;
    uint8_t EFUSE_F_BT_IMAGE, RF_C2_7_1, RF_C2_15_9 = 0;

    pmu_get_efuse(PMU_EFUSE_PAGE_RESERVED_15, &read_val);
    TRACE(2, "%s efuse=0x%x",__func__, read_val);
    EFUSE_F_BT_IMAGE = read_val & 0xf;          //get[3:0]

    EFUSE_F_BT_IMAGE |= (((read_val & 0x20) >> 5) << 4);    //get bit5
    TRACE(1, "EFUSE_F_BT_IMAGE:0x%x",EFUSE_F_BT_IMAGE);

    BTRF_REG_GET_FIELD(0xC2, 0x7f, 1, RF_C2_7_1);
    BTRF_REG_GET_FIELD(0xC2, 0x7f, 9, RF_C2_15_9);
    TRACE(1, "original RF_C2_7_1=0x%x , RF_C2_15_9=0x%x",RF_C2_7_1,RF_C2_15_9);

    if(EFUSE_F_BT_IMAGE >= RF_C2_7_1)
        return;

    if(0x10 & read_val)
    {
        RF_C2_7_1 = RF_C2_7_1 - EFUSE_F_BT_IMAGE;
        RF_C2_15_9 = RF_C2_15_9 - EFUSE_F_BT_IMAGE;
    }
    else
    {
        RF_C2_7_1 = RF_C2_7_1 + EFUSE_F_BT_IMAGE;
        RF_C2_15_9 = RF_C2_15_9 + EFUSE_F_BT_IMAGE;
    }

    BTRF_REG_SET_FIELD(0xC2, 0x7f, 1, RF_C2_7_1);
    BTRF_REG_SET_FIELD(0xC2, 0x7f, 9, RF_C2_15_9);
#endif
}

void btdrv_rf_rx_gain_adjust_req(uint32_t user, bool lowgain)
{
    return;
}

//rf Image calib
void btdtv_rf_image_calib(void)
{
    // uint16_t read_val = 0;
    // //read calibrated val from efuse 0x05 register
    // pmu_get_efuse(PMU_EFUSE_PAGE_SW_CFG, &read_val);
    // //check if bit 11 has been set
    // uint8_t calb_done_flag = ((read_val &0x800)>>11);
    // if(calb_done_flag)
    // {
    //     BT_DRV_TRACE(1,"EFUSE REG[5]=%x",read_val);
    // }
    // else
    // {
    //     BT_DRV_TRACE(0,"EFUSE REG[5] rf image has not been calibrated!");
    //     return;
    // }
    // //[bit 12] calib flag
    // uint8_t calib_val = ((read_val &0x1000)>>12);
    // btdrv_read_rf_reg(0x9b,&read_val);
    // read_val&=0xfcff;

    // if(calib_val==0)
    // {
    //     read_val|= 1<<8;
    // }
    // else if(calib_val== 1)
    // {
    //     read_val|= 1<<9;
    // }

    // BT_DRV_TRACE(1,"write rf image calib val=%x in REG[0x9b]", read_val);
    // btdrv_write_rf_reg(0x9b,read_val);
}

#ifdef TX_PULLING_CAL

const uint16_t tx_cal_rfreg_set[][2] =
{
    //[tstbuf]
    {0x2b, 0x0700},
    {0x2c, 0x2676},
    {0x51, 0x27f9},
    {0x81, 0x0009},
    {0x95, 0x0031},
    {0xbf, 0x3c32},
    {0xa6, 0x1e6a},

    //[ddrpll prepare]
    {0xe7, 0xe04e},
    {0xe8, 0x4030},
    {0xdd, 0x0000},
    {0xde, 0x9800},
    {0xdf, 0x0003},
    {0xe0, 0x0110},

    //[pulling mixer]
    {0xd1, 0x02c8},
    {0xcf, 0xaf1B},
    {0xd0, 0x0224},

    //[adc rst]
    {0x0f, 0x0030},
    {0xc1, 0x1bc3},
    {0x2b, 0x0710},
    {0x2b, 0x0730},
#ifdef PWR_CTRL_8DBM
    {0x2C, 0x2616},
    {0xC1, 0x1B83},
#endif
    {0x23, 0x28BE},
    {0xC8, 0x5B0D},
    {0x27, 0x2A10},
    {0x27, 0x2A30},
};

const uint16_t tx_cal_rfreg_store[][1] =
{
    {0x2b},
    {0x2c},
    {0x51},
    {0x81},
    {0x95},
    {0xbf},
    {0xa6},
    {0xe7},
    {0xe8},
    {0xdd},
    {0xde},
    {0xdf},
    {0xe0},
    {0xd1},
    {0xcf},
    {0xd0},
    {0x0f},
    {0xc1},
    {0x2b},
    {0x2b},
#ifdef PWR_CTRL_8DBM
    {0x2C},
    {0xC1},
#endif
    {0x23},
    {0xC8},
    {0x27},
    {0x27},
};

int bt_iqimb_test_ex (int mismatch_type);
extern int iq_gain;
extern int iq_phy;

void btdrv_tx_pulling_cal(void)
{
    int i = 0;
    uint16_t value = 0;
    uint32_t tx_cal_digreg_store[5];
    const uint16_t (*tx_cal_rfreg_set_p)[2];
    const uint16_t (*tx_cal_rfreg_store_p)[1];
    uint32_t tx_reg_set_tbl_size = 0;

    tx_cal_rfreg_store_p = &tx_cal_rfreg_store[0];
    uint32_t rx_reg_store_tbl_size = ARRAY_SIZE(tx_cal_rfreg_store);
    uint16_t tx_rf_local[rx_reg_store_tbl_size];
    BT_DRV_TRACE(0,"rx reg_store:\n");
    for(i=0; i< rx_reg_store_tbl_size; i++)
    {
        btdrv_read_rf_reg(tx_cal_rfreg_store_p[i][0],&value);
        tx_rf_local[i] = value;
        BT_DRV_TRACE(2,"rx reg=%x,v=%x",tx_cal_rfreg_store_p[i][0],value);
    }

    tx_cal_digreg_store[0] = BTDIGITAL_REG(0xd0350248);
    tx_cal_digreg_store[1] = BTDIGITAL_REG(0xd0340020);
    tx_cal_digreg_store[2] = BTDIGITAL_REG(0xd0220c00);
    tx_cal_digreg_store[3] = BTDIGITAL_REG(0xD0330020);
    tx_cal_digreg_store[4] = BTDIGITAL_REG(0xD0350218);

    BT_DRV_TRACE(1,"0xd0350248:%x\n",tx_cal_digreg_store[0]);
    BT_DRV_TRACE(1,"0xd0340020:%x\n",tx_cal_digreg_store[1]);
    BT_DRV_TRACE(1,"0xd0220c00:%x\n",tx_cal_digreg_store[2]);
    BT_DRV_TRACE(1,"0xD0330020:%x\n",tx_cal_digreg_store[3]);
    BT_DRV_TRACE(1,"0xD0350218:%x\n",tx_cal_digreg_store[4]);

    enum HAL_CHIP_METAL_ID_T metal_id = hal_get_chip_metal_id();
    if (metal_id >= HAL_CHIP_METAL_ID_6) {
        BTRF_REG_SET_FIELD(0xD9, 7, 0, 7);          //reg_bbpll_iofst[2:0]
    }

    tx_cal_rfreg_set_p = &tx_cal_rfreg_set[0];
    tx_reg_set_tbl_size = ARRAY_SIZE(tx_cal_rfreg_set);
    BT_DRV_TRACE(0,"rx reg_set:\n");
    for(i=0; i< tx_reg_set_tbl_size; i++)
    {
        btdrv_write_rf_reg(tx_cal_rfreg_set_p[i][0],tx_cal_rfreg_set_p[i][1]);
        BTRF_REG_DUMP(tx_cal_rfreg_set_p[i][0]);
    }
    if (metal_id >= HAL_CHIP_METAL_ID_8) {
        btdrv_write_rf_reg(0x2c, 0x2656);
        btdrv_write_rf_reg(0xd0, 0x03a4);
        btdrv_write_rf_reg(0xc1, 0x1b83);
        BTRF_REG_DUMP(0x2c);
        BTRF_REG_DUMP(0xd0);
        BTRF_REG_DUMP(0xc1);          
    }
    BTRF_REG_SET_FIELD(0x22, 0x1, 1, 0);
    //[tx2448 & rx modem on]
    BTDIGITAL_REG_WR(0xd0350248, 0x80BE6E6E);
    BTDIGITAL_REG_WR(0xd0340020, 0x070E01C0);
    BTDIGITAL_REG_WR(0xd0220c00, 0x800a001f);
    BTDIGITAL_REG_WR(0xD0330020, 0x00200002);
    BTDIGITAL_REG_WR(0xD0350218, 0x583);

    btdrv_delay(1);
    bt_iqimb_test_ex(1);

    BTRF_REG_SET_FIELD(0x22, 0x1, 1, 1);
    BTRF_REG_SET_FIELD(0x24, 0x1, 15, 1);

    BT_DRV_TRACE(0,"reg_reset:\n");
    for(i=0; i< rx_reg_store_tbl_size; i++)
    {
        btdrv_write_rf_reg(tx_cal_rfreg_store_p[i][0],tx_rf_local[i]);
        BTRF_REG_DUMP(tx_cal_rfreg_store_p[i][0]);
    }

    BTDIGITAL_REG_WR(0xd0350248, tx_cal_digreg_store[0]);
    BTDIGITAL_REG_WR(0xd0340020, tx_cal_digreg_store[1]);
    BTDIGITAL_REG_WR(0xd0220c00, tx_cal_digreg_store[2]);
    BTDIGITAL_REG_WR(0xD0330020, tx_cal_digreg_store[3]);
    BTDIGITAL_REG_WR(0xD0350218, tx_cal_digreg_store[4]);

    BT_DRV_TRACE(1,"0xd0350248:%x\n",tx_cal_digreg_store[0]);
    BT_DRV_TRACE(1,"0xd0340020:%x\n",tx_cal_digreg_store[1]);
    BT_DRV_TRACE(1,"0xd0220c00:%x\n",tx_cal_digreg_store[2]);
    BT_DRV_TRACE(1,"0xD0330020:%x\n",tx_cal_digreg_store[3]);
    BT_DRV_TRACE(1,"0xD0350218:%x\n",tx_cal_digreg_store[4]);
}
#endif

#ifdef BT_BLE_USING_SAME_TX_LEVEL
#ifdef TX_PWR_LOWER_2DBM
//idx 0: -4.4dbm
#define TX_PWR_IDX_0_VAL (0x0000)
//idx 1: -0.1dbm
#define TX_PWR_IDX_1_VAL (0x0104)
//idx 2: 3.8dbm
#define TX_PWR_IDX_2_VAL (0x0410)
//idx 3: 8.1dbm
#define TX_PWR_IDX_3_VAL (0x04d3)
//idx 4: 11dbm
#define TX_PWR_IDX_4_VAL (0x0820)
//idx 5: 13.7dbm
#define MAX_BT_TX_PWR_VAL (0x31)
#define MAX_BLE_TX_PWR_VAL (0x31)
#else
//idx 0: -4.4dbm
#define TX_PWR_IDX_0_VAL (0x0000)
//idx 1: -0.1dbm
#define TX_PWR_IDX_1_VAL (0x0104)
//idx 2: 3.8dbm
#define TX_PWR_IDX_2_VAL (0x0410)
//idx 3: 8.1dbm
#define TX_PWR_IDX_3_VAL (0x0514)
//idx 4: 12.6dbm
#define TX_PWR_IDX_4_VAL (0x0c30)
//idx 5: 15.7dbm
#define MAX_BT_TX_PWR_VAL (0x34)
#define MAX_BLE_TX_PWR_VAL (0x34)
#endif

#else //!BT_BLE_USING_SAME_TX_LEVEL
#ifdef TX_PWR_LOWER_2DBM

//idx 0:BT -4.4dbm|BLE -4.4dbm
#define TX_PWR_IDX_0_VAL (0x0000)
//idx 1:BT -0.1dbm|BLE -4.4dbm
#define TX_PWR_IDX_1_VAL (0x0004)
//idx 2:BT 3.8dbm|BLE -0.1dbm
#define TX_PWR_IDX_2_VAL (0x0110)
//idx 3:BT 7.4dbm|BLE 4.1dbm
#define TX_PWR_IDX_3_VAL (0x0413)
//idx 4:BT 11dbm|BLE 8.1dbm
#define TX_PWR_IDX_4_VAL (0x0520)
//idx 5:BT 13.7dbm|BLE 8.1dbm
#define MAX_BT_TX_PWR_VAL (0x31)
#define MAX_BLE_TX_PWR_VAL (0x14)

#else

#ifdef PWR_CTRL_8DBM
//idx 0:BT -5.7dbm|BLE -6.19dbm
#define TX_PWR_IDX_0_VAL (0x0000)
//idx 1:BT -2.43dbm|BLE 5.39dbm
#define TX_PWR_IDX_1_VAL (0x04c3)
//idx 2:BT 4.5dbm|BLE 6.09dbm
#define TX_PWR_IDX_2_VAL (0x0512)
//idx 3:BT 8.41dbm|BLE 7.95dbm
#define TX_PWR_IDX_3_VAL (0x0520)

#else
//idx 0:BT -4.4dbm|BLE -4.4dbm
#define TX_PWR_IDX_0_VAL (0x0000)
//idx 1:BT -0.1dbm|BLE -4.4dbm
#define TX_PWR_IDX_1_VAL (0x0183)
//idx 2:BT 3.8dbm|BLE -0.1dbm
#define TX_PWR_IDX_2_VAL (0x0187)
//idx 3:BT 8.1dbm|BLE 3.88dbm
#define TX_PWR_IDX_3_VAL (0x0193)
//idx 4:BT 12.6dbm|BLE 8.1dbm
#define TX_PWR_IDX_4_VAL (0x01A1)
//idx 5:BT 15.7dbm|BLE 8.1dbm
#define MAX_BT_TX_PWR_VAL (0x33)
#define MAX_BLE_TX_PWR_VAL (0x6)
#endif

#endif
#endif

#define TX_PWR_16dbm_VAL (0x34)
#define TX_PWR_15dbm_VAL (0x32)
#define TX_PWR_14dbm_VAL (0x31)
#define TX_PWR_13dbm_VAL (0x30)
#define TX_PWR_12dbm_VAL (0x21)
#define TX_PWR_11dbm_VAL (0x20)
#define TX_PWR_10dbm_VAL (0x16)
#define TX_PWR_9dbm_VAL (0x15)
#define TX_PWR_8dbm_VAL (0x14)
#define TX_PWR_7dbm_VAL (0x13)
#define TX_PWR_6dbm_VAL (0x12)
#define TX_PWR_5dbm_VAL (0x11)
#define TX_PWR_4dbm_VAL (0x18)
#define TX_PWR_3dbm_VAL (0x7)
#define TX_PWR_2dbm_VAL (0x5)
#define TX_PWR_1dbm_VAL (0x4)
#define TX_PWR_0dbm_VAL (0x3)
#define TX_PWR_N1dbm_VAL (0x2)
#define TX_PWR_N2dbm_VAL (0x1)
#define TX_PWR_N3dbm_VAL (0x0)

//Warning!!! Please call this function only in this file!
static void bt_drv_rf_config_tx_pwr_for_page(void)
{
    btdrv_write_rf_reg(0xe3, TX_PWR_IDX_3_VAL);//only used for page
}

//Warning!!! Please call this function only in this file!
#ifndef PWR_CTRL_8DBM
static void bt_drv_rf_config_tx_max_pwr(void)
{
    TX_PWR_REG_T reg_4E;
    reg_4E.u.reg_bit.bt_pwr = MAX_BT_TX_PWR_VAL;
    reg_4E.u.reg_bit.ble_pwr = MAX_BLE_TX_PWR_VAL;
    BT_DRV_TRACE(1,"RF 4E=0x%x",reg_4E.u.reg_val);

    btdrv_write_rf_reg(0x4E, reg_4E.u.reg_val);//only used for max tx pwr
}
#endif

int bt_drv_rf_ble_power_control( unsigned char ble_config)
{
    int ret = 0;
    int i =0 ;
    uint16_t tx_pwr_val[4]={TX_PWR_IDX_0_VAL,TX_PWR_IDX_1_VAL,TX_PWR_IDX_2_VAL,TX_PWR_IDX_3_VAL};

    for(i = 0; i< 4; i++){
        tx_pwr_val[i] = (tx_pwr_val[i] & 0xF03F)|( ble_config << 6);
    }
    tx_pwr_val[3] = (tx_pwr_val[3] & 0xFFC0)|0x20;

    btdrv_write_rf_reg(0x49, tx_pwr_val[0] );
    btdrv_write_rf_reg(0x4A, tx_pwr_val[1] );
    btdrv_write_rf_reg(0x4B, tx_pwr_val[2] );
    btdrv_write_rf_reg(0x4C, tx_pwr_val[3] );
    ret = factory_section_set_ble_power_control(ble_config);
    return ret;
}
void bt_drv_tx_pwr_init(void)
{
    uint16_t reg_ble_value = factory_section_get_ble_power_control();   //change for harmonyOS
    uint16_t tx_pwr_val[4]={TX_PWR_IDX_0_VAL,TX_PWR_IDX_1_VAL,TX_PWR_IDX_2_VAL,TX_PWR_IDX_3_VAL};
    if(!reg_ble_value){
        tx_pwr_val[3] = (tx_pwr_val[3] & 0xF000)|(0x0014<<6)|0x20;
        btdrv_write_rf_reg(0x49, tx_pwr_val[0]);
        btdrv_write_rf_reg(0x4A, tx_pwr_val[1]);
        btdrv_write_rf_reg(0x4B, tx_pwr_val[2]);
        btdrv_write_rf_reg(0x4C, tx_pwr_val[3]);
    }else{
        //read max txpwr from factory
        tx_pwr_val[3] = (tx_pwr_val[3] & 0xF000)|(reg_ble_value<<6)|0x20;
        btdrv_write_rf_reg(0x49, tx_pwr_val[0]);
        btdrv_write_rf_reg(0x4A, tx_pwr_val[1]);
        btdrv_write_rf_reg(0x4B, tx_pwr_val[2]);
        btdrv_write_rf_reg(0x4C, tx_pwr_val[3]);
    }
#ifndef PWR_CTRL_8DBM
    btdrv_write_rf_reg(0x4D, TX_PWR_IDX_4_VAL);
    //idx5 max TX pwr
    bt_drv_rf_config_tx_max_pwr();
#endif
    //for page
    bt_drv_rf_config_tx_pwr_for_page();
}


uint8_t rf_txpwr_reg_get(uint8_t idx)
{
    uint8_t rf_reg = 0;
    switch(idx)
    {
        case 0:
            rf_reg = 0x49;
            break;
        case 1:
            rf_reg = 0x4A;
            break;
        case 2:
            rf_reg = 0x4B;
            break;
        case 3:
            rf_reg = 0x4C;
            break;
        case 4:
            rf_reg = 0x4D;
            break;
        default:
            rf_reg = 0x4E;
            break;
    }

    return rf_reg;
}

uint8_t rf_reg_txpwr_val_get(int8_t txpwr_dbm)
{
    uint8_t txpwr_val = 0;
    switch(txpwr_dbm)
    {
        case 0:
            txpwr_val = TX_PWR_0dbm_VAL;
            break;
        case 1:
            txpwr_val = TX_PWR_1dbm_VAL;
            break;
        case 2:
            txpwr_val = TX_PWR_2dbm_VAL;
            break;
        case 3:
            txpwr_val = TX_PWR_3dbm_VAL;
            break;
        case 4:
            txpwr_val = TX_PWR_4dbm_VAL;
            break;
        case 5:
            txpwr_val = TX_PWR_5dbm_VAL;
            break;
        case 6:
            txpwr_val = TX_PWR_6dbm_VAL;
            break;
        case 7:
            txpwr_val = TX_PWR_7dbm_VAL;
            break;
        case 8:
            txpwr_val = TX_PWR_8dbm_VAL;
            break;
        case 9:
            txpwr_val = TX_PWR_9dbm_VAL;
            break;
        case 10:
            txpwr_val = TX_PWR_10dbm_VAL;
            break;
        case 11:
            txpwr_val = TX_PWR_11dbm_VAL;
            break;
        case 12:
            txpwr_val = TX_PWR_12dbm_VAL;
            break;
        case 13:
            txpwr_val = TX_PWR_13dbm_VAL;
            break;
        case 14:
            txpwr_val = TX_PWR_14dbm_VAL;
            break;
        case 15:
            txpwr_val = TX_PWR_15dbm_VAL;
            break;
        case 16:
            txpwr_val = TX_PWR_16dbm_VAL;
            break;
        case -1:
            txpwr_val = TX_PWR_N1dbm_VAL;
            break;
        case -2:
            txpwr_val = TX_PWR_N2dbm_VAL;
            break;
        case -3:
            txpwr_val = TX_PWR_N3dbm_VAL;
            break;
        default:
            if(txpwr_dbm < -3)
            {
                txpwr_val = TX_PWR_N3dbm_VAL;
            }
            else
            {
                txpwr_val = TX_PWR_16dbm_VAL;
            }
            break;
    }

    return txpwr_val;
}

void bt_drv_ble_rf_reg_txpwr_set(uint8_t rf_reg, uint8_t val)
{
    TX_PWR_REG_T reg_txpwr;

    btdrv_read_rf_reg(rf_reg, &reg_txpwr.u.reg_val);

    reg_txpwr.u.reg_bit.ble_pwr = val;

    btdrv_write_rf_reg(rf_reg, reg_txpwr.u.reg_val);
}

uint8_t btdrv_rf_init(void)
{
    uint16_t value = 0;
    const uint16_t (*rf_init_tbl_p)[3] = NULL;
    uint32_t tbl_size = 0;
    //uint8_t ret;
    uint8_t i;

    for (i = 0; i < ARRAY_SIZE(rf_sys_init_tbl); i++)
    {
        btdrv_read_rf_reg(rf_sys_init_tbl[i].reg, &value);
        value = (value & ~rf_sys_init_tbl[i].mask) | (rf_sys_init_tbl[i].set & rf_sys_init_tbl[i].mask);
        if (rf_sys_init_tbl[i].delay)
        {
            btdrv_delay(rf_sys_init_tbl[i].delay);
        }
        btdrv_write_rf_reg(rf_sys_init_tbl[i].reg, value);
    }

    if(hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_0)
    {
        for (i = 0; i < ARRAY_SIZE(rf_sys_init_tbl_1); i++)
        {
            btdrv_read_rf_reg(rf_sys_init_tbl_1[i].reg, &value);
            value = (value & ~rf_sys_init_tbl_1[i].mask) | (rf_sys_init_tbl_1[i].set & rf_sys_init_tbl_1[i].mask);
            if (rf_sys_init_tbl_1[i].delay)
            {
                btdrv_delay(rf_sys_init_tbl_1[i].delay);
            }
            btdrv_write_rf_reg(rf_sys_init_tbl_1[i].reg, value);
        }
    }
    rf_init_tbl_p = &rf_init_tbl_1[0];
    tbl_size = ARRAY_SIZE(rf_init_tbl_1);

    for(i=0; i< tbl_size; i++)
    {
        btdrv_write_rf_reg(rf_init_tbl_p[i][0],rf_init_tbl_p[i][1]);
        if(rf_init_tbl_p[i][2] !=0)
            btdrv_delay(rf_init_tbl_p[i][2]);//delay
        btdrv_read_rf_reg(rf_init_tbl_p[i][0],&value);
        BT_DRV_TRACE(2,"reg=%x,v=%x",rf_init_tbl_p[i][0],value);
    }

    bt_drv_tx_pwr_init();

#ifdef __HW_AGC__
    bt_drv_rf_set_bt_hw_agc_enable(1);
    bt_drv_rf_set_ble_hw_agc_enable(1);
    for(i=0; i< ARRAY_SIZE(rf_init_tbl_1_hw_agc); i++)
    {
        btdrv_write_rf_reg(rf_init_tbl_1_hw_agc[i][0],rf_init_tbl_1_hw_agc[i][1]);
        if(rf_init_tbl_1_hw_agc[i][2] !=0)
            btdrv_delay(rf_init_tbl_1_hw_agc[i][2]);//delay
    }
#else
#ifndef BT_I2V_VAL_RD
    bt_drv_rf_set_bt_hw_agc_enable(0);
    bt_drv_rf_set_ble_hw_agc_enable(0);
#endif
#endif

#ifdef __NEW_SWAGC_MODE__
    bt_drv_rf_set_bt_sync_agc_enable(true);
#else
    bt_drv_rf_set_bt_sync_agc_enable(false);
#endif

#ifdef __BLE_NEW_SWAGC_MODE__
    bt_drv_rf_set_ble_sync_agc_enable(true);
#else
    bt_drv_rf_set_ble_sync_agc_enable(false);
#endif

#ifdef __AFH_ASSESS__
    bt_drv_rf_set_afh_monitor_gain();
#endif
    //need before rf log delay cal
//    btdtv_rf_image_calib();
    //btdrv_rf_log_delay_cal_init();
    //btdrv_tx_iq_manual_cal();
    //btdrv_rf_log_delay_cal();

//    btdrv_spi_trig_init();

    return 1;
}

void bt_drv_rf_reset(void)
{
    btdrv_write_rf_reg(0x80,0xcafe);
    btdrv_write_rf_reg(0x80,0x5fee);
}

void bt_drv_rf_set_bt_sync_agc_enable(bool enable)
{
    uint16_t val_e2 = 0;
    btdrv_read_rf_reg(0x22,&val_e2);
    if(enable)
    {
        //open rf new sync agc mode
        val_e2 |= (1<<3);
    }
    else
    {
        //close rf new sync agc mode
        val_e2 &= ~(1<<3);
    }
    btdrv_write_rf_reg(0x22,val_e2);
}

void bt_drv_rf_set_ble_sync_agc_enable(bool enable)
{
    uint16_t val_e2 = 0;
    btdrv_read_rf_reg(0x22,&val_e2);
    if(enable)
    {
        //open rf new sync agc mode
        val_e2 |= (1<<2);
    }
    else
    {
        //close rf new sync agc mode
        val_e2 &= ~(1<<2);
    }
    btdrv_write_rf_reg(0x22,val_e2);
}

//switch old swagc and new sync swagc
void bt_drv_select_agc_mode(enum BT_WORK_MODE_T mode)
{
#ifdef __NEW_SWAGC_MODE__
    static uint8_t agc_mode_bak = SWAGC_INIT_MODE;
    uint8_t agc_mode = OLD_SWAGC_MODE;

    switch(mode)
    {
        case BT_A2DP_WORK_MODE:
            agc_mode = NEW_SYNC_SWAGC_MODE;
            break;
        case BT_IDLE_MODE:
        case BT_HFP_WORK_MODE:
            agc_mode = OLD_SWAGC_MODE;
            break;
        default:
            BT_DRV_TRACE(1,"BT_DRV:set error mork mode=%d",mode);
            break;
    }

    if(agc_mode_bak != agc_mode)
    {
        agc_mode_bak = agc_mode;
        BT_DRV_TRACE(1,"BT_DRV:use SW AGC mode=%d[2:OLD,1:NEW sync]",agc_mode);
        bt_drv_reg_op_swagc_mode_set(agc_mode);
    }
#endif
}

void btdrv_switch_agc_mode(enum BT_WORK_MODE_T mode)
{
}

void btdrv_txpower_calib(void)
{
    // uint16_t read_val;

    // pmu_get_efuse(PMU_EFUSE_PAGE_SW_CFG, &read_val);
    // TRACE(2, "%s PMU_EFUSE_PAGE_SW_CFG val=0x%x",__func__, read_val);

    // if(0x3 == (0x3 & read_val))
    // {
    //     BTRF_REG_SET_FIELD(0x2A0, 0x1,14,0);
    // }
}
