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
#include <stdio.h>
#include <math.h>
#include "string.h"
#include "plat_types.h"
#include "plat_addr_map.h"
#include "hal_i2c.h"
#include "hal_uart.h"
#include "bt_drv.h"
#include "bt_drv_internal.h"
#include "bt_drv_2003_internal.h"
#include "bt_drv_reg_op.h"
#include "bt_drv_interface.h"
#include "hal_timer.h"
#include "hal_intersys.h"
#include "hal_trace.h"
#include "hal_psc.h"
#include "hal_cmu.h"
#include "hal_sysfreq.h"
#include "hal_chipid.h"
#include "hal_iomux.h"
#include "hal_gpio.h"
#include "pmu.h"
#include "nvrecord_dev.h"
#include "tgt_hardware.h"
#include "hal_btdump.h"
#include "iqcorrect.h"
#include "bt_drv_internal.h"
#include "factory_section.h"
#ifdef __BT_RAMRUN__
#include "bt_drv_ramrun_symbol_2003.h"

extern uint32_t __bt_ramrun_code_start_flash[];
extern uint32_t __bt_ramrun_code_end_flash[];

#endif

#include "factory_section.h"
#include "bwifi_interface.h"
enum HAL_CHIP_METAL_ID_T g_metal_id;

extern "C" void hal_iomux_set_controller_log(void);
bool btdrv_dut_mode_enable = false;

static volatile uint32_t btdrv_tx_flag = 1;
void btdrv_tx(const unsigned char *data, unsigned int len)
{
    BT_DRV_TRACE(0,"tx");
    btdrv_tx_flag = 1;
}

void btdrv_dut_accessible_mode_manager(const unsigned char *data);

static unsigned int btdrv_rx(const unsigned char *data, unsigned int len)
{
    hal_intersys_stop_recv(HAL_INTERSYS_ID_0);
    if(len>5 && data[0] == 0x04 && data[1] == 0xff &&
        data[3] == 0x01)
    {
        BT_DRV_TRACE(2,"%s", &data[4]);
    }
    else
    {
        BT_DRV_TRACE(2,"%s len:%d", __func__, len);
        BT_DRV_DUMP("%02x ", data, len>7?7:len);
    }
    btdrv_dut_accessible_mode_manager(data);
    hal_intersys_start_recv(HAL_INTERSYS_ID_0);

    return len;
}

void btdrv_SendData(const uint8_t *buff,uint8_t len)
{
    btdrv_tx_flag = 0;

    BT_DRV_TRACE(1,"%s", __func__);
    BT_DRV_DUMP("%02x ", buff, len);
    hal_intersys_send(HAL_INTERSYS_ID_0, HAL_INTERSYS_MSG_HCI, buff, len);

    while( (btdrv_dut_mode_enable==0) && btdrv_tx_flag == 0);
}

////open intersys interface for hci data transfer
static bool hci_has_opened = false;

void btdrv_hciopen(void)
{
    int ret = 0;

    if (hci_has_opened)
    {
        return;
    }

    hci_has_opened = true;

    ret = hal_intersys_open(HAL_INTERSYS_ID_0, HAL_INTERSYS_MSG_HCI, btdrv_rx, btdrv_tx, false);

    if (ret)
    {
        BT_DRV_TRACE(0,"Failed to open intersys");
        return;
    }

    hal_intersys_start_recv(HAL_INTERSYS_ID_0);
}

////open intersys interface for hci data transfer
void btdrv_hcioff(void)
{
    if (!hci_has_opened)
    {
        return;
    }
    hci_has_opened = false;

    hal_intersys_close(HAL_INTERSYS_ID_0,HAL_INTERSYS_MSG_HCI);
}

/*  btdrv power on or off the bt controller*/
void btdrv_poweron(uint8_t en)
{
    //power on bt controller
    if(en)
    {
        hal_psc_bt_enable();
        hal_cmu_bt_clock_enable();
        hal_cmu_bt_reset_clear();
        hal_cmu_bt_module_init();
        btdrv_delay(10);
        // BTDM mode 4.2
        BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE) = 0x42;
        btdrv_delay(100);
    }
    else
    {
        btdrv_delay(10);
        hal_cmu_bt_reset_set();
        hal_cmu_bt_clock_disable();
        hal_psc_bt_disable();
    }
}

void btdrv_rf_init_ext(void)
{
    unsigned int xtal_fcap;

    if (!nvrec_dev_get_xtal_fcap(&xtal_fcap))
    {
        btdrv_rf_init_xtal_fcap(xtal_fcap);
        btdrv_delay(1);
        BT_DRV_TRACE(2,"%s 0x95=0x%x", __func__, xtal_fcap);
    }
    else
    {
        btdrv_rf_init_xtal_fcap(DEFAULT_XTAL_FCAP);
        BT_DRV_TRACE(1,"%s failed", __func__);
    }
}


void bt_drv_extra_config_after_init(void)
{
#if (!defined(__BT_RAMRUN__) && defined(__AFH_ASSESS__) && !defined(__HW_AGC__))
    bt_drv_reg_op_afh_assess_init();
#endif
}

extern "C" int16_t pmu_get_rcosc_cal_cnt_initial(void);
uint16_t RF_I2V_CAP_2M, RF_I2V_CAP_3M = 0;
#define D_REF 0x102
static  uint16_t RF_FILTER_CAP = 0;
void btdrv_2003_rxbb_rccal(void)
{
    uint16_t read_value;
    uint8_t  cal_val = 0;

    int16_t rf_39f_ini = pmu_get_rcosc_cal_cnt_initial();

    TRACE(2, "D_REF=0x%x rf_39f_ini=0x%x",D_REF,rf_39f_ini);

    if((rf_39f_ini < 0x80) || (rf_39f_ini > 0x200 ))
    {
        TRACE(1, "rcosc cal val out of range[0x80,0x200]:0x%x \n filter adc cap use defalut value.",rf_39f_ini);
        return;
    }

    //3DH5
    TRACE(1, "D_REF*1000/rf_39f_ini :%d",D_REF*1000/rf_39f_ini);
    cal_val = (D_REF*1000/rf_39f_ini) * 0x3E/1000;
    TRACE(1, "i2v cap for 3DH=0x%x",cal_val);
    RF_I2V_CAP_3M = cal_val;
    BTRF_REG_SET_FIELD(0xbc,0x7f,8,cal_val);//0xbc[14:8]=cal_val
    btdrv_read_rf_reg(0xbc,&read_value);
    TRACE(1, "reg 0xbc=0x%x",read_value);

#if 0
    //2DH5
     cal_val = (D_REF*1000/rf_39f_ini) * 0x4E/1000;
    TRACE(1, "i2v cap for 2DH=0x%x",cal_val);
    RF_I2V_CAP_2M = cal_val;
    btdrv_read_rf_reg(0xbc,&read_value);
    read_value &= 0x80ff;       //clear [14:8]
    read_value |= (cal_val << 8);
    TRACE(1, "reg 0xbc=0x%x",read_value);
#endif

#ifdef BT_IF_750K
    cal_val = (D_REF*1000/rf_39f_ini) * 0x3B/1000;
    RF_FILTER_CAP = cal_val;
    TRACE(1, "filter cap=0x%x",cal_val);
    BTRF_REG_SET_FIELD(0xbb,0xff,8,cal_val);//0xbc[15:8]=cal_val
    BTRF_REG_SET_FIELD(0xbb,0x7,0,0x0);
#elif defined(BT_IF_1M)
    cal_val = (D_REF*1000/rf_39f_ini) * 0x47/1000;
    RF_FILTER_CAP = cal_val;
    TRACE(1, "filter cap=0x%x",cal_val);
    BTRF_REG_SET_FIELD(0xbb,0xff,8,cal_val);//0xbc[15:8]=cal_val
    BTRF_REG_SET_FIELD(0xbb,0x7,0,0x4);
#elif defined(BT_IF_1P05M)
    cal_val = (D_REF*1000/rf_39f_ini) * 0x46/1000;
    RF_FILTER_CAP = cal_val;
    TRACE(1, "filter cap=0x%x",cal_val);
    BTRF_REG_SET_FIELD(0xbb,0xff,8,cal_val);//0xbc[15:8]=cal_val
    BTRF_REG_SET_FIELD(0xbb,0x7,0,0x4);
#endif

    btdrv_read_rf_reg(0xbb,&read_value);
    TRACE(1, "reg 0xbb=0x%x",read_value);

    cal_val = (D_REF*1000/rf_39f_ini) * 0x50/1000;
    TRACE(1, "ADC cap 1st=0x%x",cal_val);
    BTRF_REG_SET_FIELD(0x38A,0xff,0,cal_val);//0x38A[7:0]
    btdrv_read_rf_reg(0x38A,&read_value);
    TRACE(1, "reg 0x38A=0x%x",read_value);

    cal_val = (D_REF*1000/rf_39f_ini) * 0x28/1000;
    TRACE(1, "ADC cap 2nd=0x%x",cal_val);
    BTRF_REG_SET_FIELD(0x38A,0x3f,8,cal_val);//0x38A[13:8]
    btdrv_read_rf_reg(0x38A,&read_value);
    TRACE(1, "reg 0x38A=0x%x",read_value);
}

#ifdef __PWR_FLATNESS__
#define PWR_FLATNESS_CONST_VAL                         0x11
void btdrv_2003_channel_pwr_flatness(void)
{
    uint8_t rf_8c_val = 0;
    uint16_t rf_ed_val = 0;
    uint8_t RF_ED_9_6[3] = {0};
    uint8_t RF_2A1_9_6 = 0;
    uint32_t Channel[3] = {0xA0000, 0xA0027, 0xA004E};
    uint8_t average_value = 0;

    BTRF_REG_GET_FIELD(0x8c, 0xffff,0,rf_8c_val);
    TRACE(2, "original 0x8C val=0x%x",rf_8c_val);
    BTRF_REG_SET_FIELD(0x8c, 0x1,0,1);          //reg_bt_pa_ldo_pu_dr

    for(int i=0; i<3; i++)
    {
        BTDIGITAL_REG(0xD0220C00)  = Channel[i];
        btdrv_delay(1);

        BTRF_REG_GET_FIELD(0xED, 0xffff, 0, rf_ed_val);
        if (0x0400 & rf_ed_val) //bit10 txpll_capbank_done
        {
            BTRF_REG_GET_FIELD(0xED, 0xf, 6, RF_ED_9_6[i]);
            TRACE(2, "RF_ED_9_6[%d]=0x%x",i,RF_ED_9_6[i]);
        }
        else
        {
            TRACE(2, "[%d]txpll capbank not done,and 0xED=0x%x", i, rf_ed_val);
            return;
        }
    }

    average_value = (RF_ED_9_6[0] + RF_ED_9_6[1] + RF_ED_9_6[2]) / 3 ;
    RF_2A1_9_6 = PWR_FLATNESS_CONST_VAL - average_value;

    if(average_value > PWR_FLATNESS_CONST_VAL)
        RF_2A1_9_6 = 8;

    TRACE(1, "average_value=0x%x and RF_2A1_9_6=0x%x.",average_value,RF_2A1_9_6);
    BTRF_REG_SET_FIELD(0x2A1, 0xf, 0,RF_2A1_9_6);

    BTRF_REG_SET_FIELD(0x8c, 0xffff,0,rf_8c_val);
    BTDIGITAL_REG(0xD0220C00) = 0;
}
#endif

void btdrv_2003_i2v_dccal(void)
{
    uint16_t read_value;
    uint8_t rf_f8_11_6, rf_f8_5_0 = 0;
    uint8_t reg_dc = 0;

    BTRF_REG_SET_FIELD(0xC4,0x3f,2,0x32);
    btdrv_read_rf_reg(0xC4, &read_value);
    BT_DRV_TRACE(1, "write reg 0xC4 val=%x", read_value);

    BTRF_REG_SET_FIELD(0xBD,0x3,4,0x2);
    btdrv_read_rf_reg(0xBD,&read_value);
    BT_DRV_TRACE(1,"write reg 0xBD val=%x",read_value);

    BTRF_REG_SET_FIELD(0x8C,0xf,6,0xD);
    btdrv_read_rf_reg(0x8C,&read_value);
    BT_DRV_TRACE(1,"write reg 0x8C val=%x",read_value);

    BTDIGITAL_REG(0xd0220c00) = 0x000A0080;
    btdrv_delay(1);

    //reg_bt_i2v_dccal_ready_dr
    BTRF_REG_SET_FIELD(0xBD,0x1,0,0);
    btdrv_read_rf_reg(0xBD,&read_value);
    BT_DRV_TRACE(1,"write reg 0xBD val=%x",read_value);

    //reg_rxgain_dr
    BTRF_REG_SET_FIELD(0xB8,0x1,0,1);
    btdrv_read_rf_reg(0xB8,&read_value);
    BT_DRV_TRACE(1,"write reg 0xB8 val=%x",read_value);

    //reg_bt_trx_cal_en
    BTRF_REG_SET_FIELD(0xBE,0x1,0,1);
    btdrv_read_rf_reg(0xBE,&read_value);
    BT_DRV_TRACE(1,"write reg 0xBE val=%x",read_value);

    for(int i=0; i<8; i++)
    {
        //reg_i2v_dc_cal_en
        BTRF_REG_SET_FIELD(0xC4,0x1,0,0);
        btdrv_read_rf_reg(0xC4, &read_value);
        BT_DRV_TRACE(1, "write reg 0xC4 val=%x", read_value);

        btdrv_delay(1);
        BTRF_REG_SET_FIELD(0xB8,0x7,1,i);
        btdrv_read_rf_reg(0xB8, &read_value);
        BT_DRV_TRACE(1, "write reg 0xB8 val=%x", read_value);

        //reg_bt_rxlowifdc_cal_resetn
        BTRF_REG_SET_FIELD(0xBE,0x1,8,1);
        btdrv_read_rf_reg(0xBE, &read_value);
        BT_DRV_TRACE(1, "write reg 0xBE val=%x", read_value);

        BTRF_REG_SET_FIELD(0xBE,0x1,8,0);
        btdrv_read_rf_reg(0xBE, &read_value);
        BT_DRV_TRACE(1, "write reg 0xBE val=%x", read_value);

        //reg_i2v_dc_cal_en
        BTRF_REG_SET_FIELD(0xC4,0x1,0,1);
        btdrv_read_rf_reg(0xC4, &read_value);
        BT_DRV_TRACE(1, "write reg 0xC4 val=%x", read_value);

        BTRF_REG_SET_FIELD(0xBE,0x1,8,1);
        btdrv_read_rf_reg(0xBE, &read_value);
        BT_DRV_TRACE(1, "write reg 0xBE val=%x", read_value);

        btdrv_delay(1);
        //i2v_dccal_value[12:0]
        btdrv_read_rf_reg(0xF8, &read_value);
        BT_DRV_TRACE(1, "read reg 0xF8 val=%x", read_value);
        if(read_value & 0x1000)
        {
            rf_f8_5_0 = (read_value & 0x3f);
            rf_f8_11_6 = ((read_value & 0xfc0) >> 6);
            BT_DRV_TRACE(2, "rf_f8_5_0=0x%x, rf_f8_11_6=0x%x", rf_f8_5_0,rf_f8_11_6);

            reg_dc = 0xc5 + i;
            BTRF_REG_SET_FIELD(reg_dc,0x3f,0,rf_f8_11_6);
            btdrv_read_rf_reg(reg_dc, &read_value);
            BT_DRV_TRACE(2, "write reg 0x%x[5:0] val=%x",reg_dc ,read_value);

            BTRF_REG_SET_FIELD(reg_dc,0x3f,6,rf_f8_5_0);
            btdrv_read_rf_reg(reg_dc, &read_value);
            BT_DRV_TRACE(2, "write reg 0x%x[11:6] val=%x",reg_dc ,read_value);
        }
    }

    //reg_bt_i2v_dccal_ready_dr
    BTRF_REG_SET_FIELD(0xBD,0x1,0,1);
    btdrv_read_rf_reg(0xBD,&read_value);
    BT_DRV_TRACE(1,"write reg 0xBD val=%x",read_value);

    BTRF_REG_SET_FIELD(0xB8,0xf,0,0);
    btdrv_read_rf_reg(0xB8,&read_value);
    BT_DRV_TRACE(1,"write reg 0xB8 val=%x",read_value);

    BTRF_REG_SET_FIELD(0xBE,0x1,8,0);
    btdrv_read_rf_reg(0xBE, &read_value);
    BT_DRV_TRACE(1, "write reg 0xBE val=%x", read_value);

    BTRF_REG_SET_FIELD(0xBE,0x1,0,0);
    btdrv_read_rf_reg(0xBE,&read_value);
    BT_DRV_TRACE(1,"write reg 0xBE val=%x",read_value);

    BTRF_REG_SET_FIELD(0x8C,0xf,6,0);
    btdrv_read_rf_reg(0x8C,&read_value);
    BT_DRV_TRACE(1,"write reg 0x8C val=%x",read_value);

    BTDIGITAL_REG(0xd0220c00) = 0;
}

void btdrv_enable_jtag(void)
{
    *(uint32_t*)0x400000F8 &= 0x7FFFFFFF;//clear bit31

    hal_iomux_set_jtag();
    hal_cmu_jtag_enable();
    hal_cmu_jtag_clock_enable();
}

void bt_drv_enable_transq_local_isr_mask(void)
{
    *(volatile uint8_t*)(0xd0380000) |= 1<<1;
    *(volatile uint8_t*)(0xd038000c)  = 0xff;
}

///start active bt controller
//#define BT_DRV_ENABLE_LMP_TRACE

/* config wifi/bt coex mode
@mode: coex mode, 0 - tdd, 1 - fdd
*/
extern "C" void config_coex_mode(uint8_t mode)
{
    TRACE(0, "config_coex_mode %d", mode);
    /* set bt traffic ignore pta, should be set after bt drv started */
    btdrv_reg_op_config_coex_mode(mode);
}

extern "C" void coex_mode_evt_handler(WIFI_USER_EVT_ID evt_id, void *arg)
{
    if (evt_id == WIFI_USER_EVT_COEX_MODE_CHANGE) {
        if (arg) {
            uint8_t mode = *(uint8_t *)arg;
            config_coex_mode(mode);
        }
    }
}


#ifdef __BT_COMP_BIN__
extern "C" int dsp_code_decom (unsigned char *outStream, unsigned int *uncompressedSize,
			      unsigned char *inStream,  unsigned int  length);
#endif
uint8_t btdrv_init_flag = 0;

const uint16_t dc_iq_rf_store[] =
{
    0x8F,
    0x0F,
    0xC1,
    0x27,
    0x2B,
    0x2C,
    0x92,
    0x93,
    0x94,
    0xBF,
    0x29,
    0x95,
    0xA6,
    0xA5,
    //0xCB,
    0x91,
    0x9B,
};

const uint32_t dc_iq_dig_store[] =
{
    0xD0350028,
    0xD035002C,
    0xD0340020,
    0xD0220C00,
};

void btdrv_tx_iq_manual_cal_2006(void)
{
    uint32_t rf_tbl_size = ARRAY_SIZE(dc_iq_rf_store);
    uint16_t local_rf_v[rf_tbl_size];
    for (uint32_t i=0; i<rf_tbl_size;i++) {
        btdrv_read_rf_reg(dc_iq_rf_store[i], &local_rf_v[i]);
    }

    uint32_t dig_tbl_size = ARRAY_SIZE(dc_iq_dig_store);
    uint32_t local_dig_v[rf_tbl_size];
    for (uint32_t i=0; i<dig_tbl_size;i++) {
        local_dig_v[i] = BTDIGITAL_REG(dc_iq_dig_store[i]);
    }

    //Step1: RF & DIG config
    //btdrv_write_rf_reg(0x8F, 0xE742);   //lobuf ldo
    btdrv_write_rf_reg(0x0F, 0x0030);   //pa on && adc pu
    btdrv_write_rf_reg(0xC1, 0x1B80);   //padrv gain dr max
    btdrv_write_rf_reg(0x27, 0x8a04);
    btdrv_write_rf_reg(0x2B, 0x0e30);   //adc rst release
    btdrv_write_rf_reg(0x2C, 0x02F7);   //txflt gain dr max adc gain dr 0db
    //btdrv_write_rf_reg(0x92, 0x729F);   //tmx ldo
    btdrv_write_rf_reg(0x93, 0x01e3);   //iqcal pu
    //dc
    BTRF_REG_SET_FIELD(0x94, 0x3F, 0, 0x10);   //reg_bt_iqcal_att_cs_ctl[5:0]
    BTRF_REG_SET_FIELD(0x94, 3, 6, 3);      //reg_bt_iqcal_load_res_sel[7:6]
    BTRF_REG_SET_FIELD(0x94, 3, 8, 3);      //reg_bt_iqcal_cap_amp_gain_ctl[9:8]
    BTRF_REG_SET_FIELD(0x94, 0xF, 10, 0xC);   //reg_bt_iqcal_i2v_gain_ctl[13:10]

    btdrv_write_rf_reg(0xBF, 0x3032);   //i2v pu
    btdrv_write_rf_reg(0x29, 0x0311);   //i2v low pass mode
    btdrv_write_rf_reg(0x95, 0x1006);   //filter sel iqcal & test en
    btdrv_write_rf_reg(0xA6, 0x1E6A);   //tstbuf sel flt & pu
    //btdrv_write_rf_reg(0xA5, 0x0076);   //increase bg current
    //btdrv_write_rf_reg(0xCB, 0x0140);   //increase padrv bias
    btdrv_write_rf_reg(0xCB, 0x3F20);   //add by caihejia
    btdrv_write_rf_reg(0xA0, 0x1568);   //add by caihejia
    btdrv_write_rf_reg(0x91, 0x0200);   //txflt capbank
    //btdrv_write_rf_reg(0x9B, 0x3748);   //lower vco ldo vref

    BTDIGITAL_REG_WR(0xD0340020, 0x030E01C0);   //txrx on pll
    BTDIGITAL_REG_SET_FIELD(0xD0350334, 0x1F, 25, 0xF);
    //Step2: rx modem on
    BTDIGITAL_REG_WR(0xD0220C00, 0x800A0000);

    dc_iq_calib_2006();

    for (uint32_t i=0; i<rf_tbl_size;i++) {
        btdrv_write_rf_reg(dc_iq_rf_store[i], local_rf_v[i]);
    }

    for (uint32_t i=0; i<dig_tbl_size;i++) {
        BTDIGITAL_REG_WR(dc_iq_dig_store[i], local_dig_v[i]);
    }
}

void btdrv_start_bt(void)
{
    if (btdrv_init_flag)
        return;

    hal_sysfreq_req(HAL_SYSFREQ_USER_BT, HAL_CMU_FREQ_26M);
    g_metal_id = hal_get_chip_metal_id();
    btdrv_init_flag = 1;
#if INTERSYS_DEBUG
#ifdef BT_DRV_ENABLE_LMP_TRACE
    btdrv_trace_config(BT_CONTROLER_TRACE_TYPE_INTERSYS   |
                       BT_CONTROLER_TRACE_TYPE_CONTROLLER |
                       BT_CONTROLER_FILTER_TRACE_TYPE_A2DP_STREAM |
                       BT_CONTROLER_TRACE_TYPE_LMP_TRACE |
                       BT_CONTROLER_TRACE_TYPE_SPUV_HCI_BUFF |
                       BT_CONTROLER_TRACE_TYPE_ACL_PACKET);
#else
    btdrv_trace_config(BT_CONTROLER_TRACE_TYPE_INTERSYS   |
                       BT_CONTROLER_TRACE_TYPE_CONTROLLER |
                       BT_CONTROLER_FILTER_TRACE_TYPE_A2DP_STREAM |
                       BT_CONTROLER_TRACE_TYPE_SPUV_HCI_BUFF |
                       BT_CONTROLER_TRACE_TYPE_ACL_PACKET);
#endif
#endif

#if defined(BLE_ONLY_ENABLED)
    btdrv_enable_sleep_checker(false);
#else
#ifndef IQ_CALI_TEST
    btdrv_enable_sleep_checker(false);
#endif
#endif
    hal_iomux_ispi_access_enable(HAL_IOMUX_ISPI_MCU_RF);

#ifndef NO_SLEEP
    pmu_sleep_en(0);
#endif

    bt_drv_reg_op_global_symbols_init();
#ifndef IQ_CALI_TEST
    btdrv_pm_register_notif_handler();
#endif

#ifdef __BT_RAMRUN__
    uint32_t ramrun_size;
    ramrun_size = (uint32_t)__bt_ramrun_code_end_flash -  (uint32_t)__bt_ramrun_code_start_flash;

#ifdef __BT_COMP_BIN__
    /* compress bin */
    int ret = dsp_code_decom ((unsigned char *)BT_RAMRUN_BASE, &ramrun_size, (unsigned char *)__bt_ramrun_code_start_flash, ramrun_size);
    TRACE(0, "bt decom %d", ret);
    if (ret != 0)
        return;
#else
    if(ramrun_size > 512 * 1024)
    {
        BT_DRV_TRACE(1,"bt controller ram run code size  over 512k,over size %d bytes",ramrun_size - 512 * 1024);
        ASSERT_ERR(0);
    }
    memcpy((uint32_t *)BT_RAMRUN_BASE, __bt_ramrun_code_start_flash, MIN(512 * 1024,ramrun_size));
#endif
    BT_DRV_TRACE(1,"bt ramrun %s", BT_CONTROLLER_COMMIT_ID);
    BT_DRV_TRACE(1,"bt ramrun %s", BT_CONTROLLER_COMMIT_DATE);
    BT_DRV_TRACE(1,"BT RAMRUN START, CONTROLLER ROM SIZE %d bytes",ramrun_size);
    //BTDIGITAL_REG_SET_FIELD(0x4000008c, 0x1, 24, 1);//sram2+
    BTDIGITAL_REG_SET_FIELD(0x4000008c, 0x1, 25, 1);//sram3
#endif

    btdrv_poweron(BT_POWERON);
#ifndef __BT_RAMRUN__
        //rom patch init
        btdrv_ins_patch_init();
        btdrv_patch_en(1);
#endif
    btdrv_hciopen();

#if defined(__BT_RAMRUN__) && !defined(__BT_COMP_BIN__)
    if(memcmp((uint32_t *)0xa0000000, __bt_ramrun_code_start_flash, ramrun_size))
    {
        for(uint32_t i = 0; i < (ramrun_size / 4); i++)
        {
            if(*(uint32_t *)(0xa0000000 + 4 * i) != __bt_ramrun_code_start_flash[i])
            {
                BT_DRV_TRACE(3,"rom code copy fail %d %x,%x\n", i, *(uint32_t *)(0xa0000000 + 4 * i), __bt_ramrun_code_start_flash[i]);
            }
        }
        ASSERT_ERR(0);
    }
#endif

#if __COEX_MODE_FDD__
    config_coex_mode(1);
#ifdef __COEX_MODE_HYBRID__
    bwifi_reg_user_evt_handler(WIFI_USER_EVT_COEX_MODE_CHANGE, coex_mode_evt_handler);
#endif
#else
    /* COEX code: register it in tdd mode, 5G force FDD mode */
    bwifi_reg_user_evt_handler(WIFI_USER_EVT_COEX_MODE_CHANGE, coex_mode_evt_handler);
#endif
    btdrv_rf_init();
    btdrv_config_init();

#ifdef TX_PULLING_CAL
    hal_btdump_clk_enable();
    bt_iq_calibration_setup();
    hal_btdump_clk_disable();
#endif
    if(g_metal_id>=HAL_CHIP_METAL_ID_8)
    {
        btdrv_tx_iq_manual_cal_2006();
    }


#ifdef BLE_EXT_ADV_TX_PWR_INDEPEND
    bt_drv_reg_op_le_ext_adv_txpwr_independent(true);
#endif

#ifdef BT_EXT_LNA_PA
    int LNA_flag = 0,PA_flag = 0;
#ifdef BT_EXT_LNA
    LNA_flag = 1;
#endif
#ifdef BT_EXT_PA
    PA_flag = 1;
#endif
    btdrv_enable_rf_sw(LNA_flag,PA_flag);
#endif

    btdrv_fa_config();

#if defined(BT_LOG_POWEROFF) && !defined(__BT_RAMRUN__)
    bt_drv_reg_op_power_off_rx_gain_config();
#endif

    btdrv_ecc_config();
    /*reg controller crash dump*/
    hal_trace_crash_dump_register(HAL_TRACE_CRASH_DUMP_MODULE_BT, btdrv_btc_fault_dump);

#ifdef BT_UART_LOG
    uint16_t cmd_filter_buf[1] = {HCI_HOST_NB_CMP_PKTS_CMD_OPCODE};
    uint8_t evt_filter_buf[1] = {HCI_NB_CMP_PKTS_EVT_CODE};
    bt_drv_reg_op_config_controller_log(TRC_DEFAULT_TYPE, cmd_filter_buf, sizeof(cmd_filter_buf)/sizeof(cmd_filter_buf[0]), evt_filter_buf, sizeof(evt_filter_buf)/sizeof(evt_filter_buf[0]));
#endif

#ifndef NO_SLEEP
    pmu_sleep_en(1);
#endif

    btdrv_config_end();

#ifndef __BT_RAMRUN__
    bt_drv_iq_tab_set_normalmode();
#endif

#ifdef __NEW_SWAGC_MODE__
    struct bt_cb_tag* bt_drv_func_cb = bt_drv_get_func_cb_ptr();
    bt_drv_func_cb->bt_switch_agc = bt_drv_select_agc_mode;

    //initialize agc mode
    if(bt_drv_func_cb->bt_switch_agc != NULL)
    {
        bt_drv_func_cb->bt_switch_agc(BT_IDLE_MODE);
    }
    if(hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_7)
    {
        btdrv_regop_rx_gain_ths_3m_init();
    }

#endif

    btdrv_hcioff();

    hal_iomux_ispi_access_enable(HAL_IOMUX_ISPI_MCU_RF);

    hal_sysfreq_req(HAL_SYSFREQ_USER_BT, HAL_CMU_FREQ_32K);

    bt_drv_enable_transq_local_isr_mask();
#ifdef PCM_PRIVATE_DATA_FLAG
    bt_drv_reg_op_sco_pri_data_init();
#endif
}

const uint8_t hci_cmd_enable_dut[] =
{
    0x01,0x03, 0x18, 0x00
};
const uint8_t hci_cmd_enable_allscan[] =
{
    0x01, 0x1a, 0x0c, 0x01, 0x03
};
const uint8_t hci_cmd_disable_scan[] =
{
    0x01, 0x1a, 0x0c, 0x01, 0x00
};
const uint8_t hci_cmd_enable_pagescan[] =
{
    0x01, 0x1a, 0x0c, 0x01, 0x02
};
const uint8_t hci_cmd_autoaccept_connect[] =
{
    0x01,0x05, 0x0c, 0x03, 0x02, 0x00, 0x02
};
const uint8_t hci_cmd_hci_reset[] =
{
    0x01,0x03,0x0c,0x00
};


const uint8_t hci_cmd_inquiry_scan_activity[] =
{
    0x01,0x1e,0x0c,0x04,0x40,0x00,0x12,0x00
};

const uint8_t hci_cmd_page_scan_activity[] =
{
    0x01,0x1c,0x0c,0x04,0x40,0x00,0x12,0x00
};


const uint8_t hci_cmd_nonsig_tx_dh1_pn9_t0[] =
{
    0x01, 0x87, 0xfc, 0x1c, 0x00, 0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x06, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01, 0x00, 0x04, 0x04, 0x1b, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff
};
const uint8_t hci_cmd_nonsig_tx_2dh1_pn9_t0[] =
{
    0x01, 0x87, 0xfc, 0x1c, 0x00, 0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x06, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01, 0x01, 0x04, 0x04, 0x36, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff
};
const uint8_t hci_cmd_nonsig_tx_3dh1_pn9_t0[] =
{
    0x01, 0x87, 0xfc, 0x1c, 0x00, 0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x06, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01, 0x01, 0x08, 0x04, 0x53, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff
};
const uint8_t hci_cmd_nonsig_tx_2dh3_pn9_t0[] =
{
    0x01, 0x87, 0xfc, 0x1c, 0x00, 0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x06, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01, 0x01, 0x0a, 0x04, 0x6f, 0x01,
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff
};
const uint8_t hci_cmd_nonsig_tx_3dh3_pn9_t0[] =
{
    0x01, 0x87, 0xfc, 0x1c, 0x00, 0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x06, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01, 0x01, 0x0b, 0x04, 0x28, 0x02,
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff
};

const uint8_t hci_cmd_nonsig_rx_dh1_pn9_t0[] =
{
    0x01, 0x87, 0xfc, 0x1c, 0x01, 0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x06, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01, 0x00, 0x04, 0x00, 0x1b, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff
};
const uint8_t hci_cmd_nonsig_rx_2dh1_pn9_t0[] =
{
    0x01, 0x87, 0xfc, 0x1c, 0x01, 0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x06, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01, 0x01, 0x04, 0x00, 0x36, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff
};
const uint8_t hci_cmd_nonsig_rx_3dh1_pn9_t0[] =
{
    0x01, 0x87, 0xfc, 0x1c, 0x01, 0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x06, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01, 0x01, 0x08, 0x00, 0x53, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff
};
const uint8_t hci_cmd_nonsig_rx_2dh3_pn9_t0[] =
{
    0x01, 0x87, 0xfc, 0x1c, 0x01, 0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x06, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01, 0x01, 0x0a, 0x00, 0x6f, 0x01,
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff
};
const uint8_t hci_cmd_nonsig_rx_3dh3_pn9_t0[] =
{
    0x01, 0x87, 0xfc, 0x1c, 0x01, 0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x06, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01, 0x01, 0x0b, 0x00, 0x28, 0x02,
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff
};

//vco test
const uint8_t hci_cmd_start_bt_vco_test[] =
{
    0x01, 0xaa, 0xfc, 0x02, 0x00,0x02
};
const uint8_t hci_cmd_stop_bt_vco_test[] =
{
    0x01, 0xaa, 0xfc, 0x02, 0x00,0x04
};

const uint8_t hci_cmd_enable_ibrt_test[] =
{
    0x01, 0xb4, 0xfc, 0x01, 0x01
};

const uint8_t hci_cmd_set_ibrt_mode[] =
{
    0x01, 0xa2, 0xfc, 0x02, 0x01,0x00
};

void btdrv_testmode_start(void)
{

#ifndef __BT_RAMRUN__
    bt_drv_reg_op_power_adjust_onoff(DIGITAL_POWER_ADJUST_EN);
    bt_drv_reg_op_init_swagc_3m_thd();
#ifdef RX_IQ_CAL
    bt_drv_iq_tab_set_testmode();
    BTDIGITAL_REG_SET_FIELD(0xD035025c, 0x3, 2, 0);
#endif
#endif// !__BT_RAMRUN__

#ifdef FORCE_SIGNALINGMODE
#elif defined FORCE_NOSIGNALINGMODE
    BTRF_REG_SET_FIELD(0xB7,0x1,11,0);     //Bit11 disable BLE hw agc en

    //BLE 2M ADC low noise by xuyuanyuan
    BTRF_REG_SET_FIELD(0xC0, 0x7, 1, 0x7);  // bit 3:1=111
    BTRF_REG_SET_FIELD(0x2BD, 0x7, 8, 0x7); // bit10:8=111
    BTRF_REG_SET_FIELD(0x38A, 0xff, 0, 0xA0);// bit7:0=10100000
#endif

    BTDIGITAL_REG_SET_FIELD(0xC0005EC0, 0xff, 0, 0xA);

#ifdef __NEW_SWAGC_MODE__
    struct bt_cb_tag* bt_drv_func_cb = bt_drv_get_func_cb_ptr();

    //initialize agc mode
    if(bt_drv_func_cb->bt_switch_agc != NULL)
    {
        bt_drv_func_cb->bt_switch_agc(BT_A2DP_WORK_MODE);
    }
#endif

    bt_drv_reg_op_ble_sync_agc_mode_set(true);

    bt_drv_reg_op_set_nosig_sch_flag(0);
}

void btdrv_write_localinfo(const char *name, uint8_t len, const uint8_t *addr)
{
    uint8_t hci_cmd_write_addr[5+6] =
    {
        0x01, 0x72, 0xfc, 0x07, 0x00
    };

    uint8_t hci_cmd_write_name[248+4] =
    {
        0x01, 0x13, 0x0c, 0xF8
    };
    memset(&hci_cmd_write_name[4], 0, sizeof(hci_cmd_write_name)-4);
    memcpy(&hci_cmd_write_name[4], name, len);
    btdrv_SendData(hci_cmd_write_name, sizeof(hci_cmd_write_name));
    btdrv_delay(50);
    memcpy(&hci_cmd_write_addr[5], addr, 6);
    btdrv_SendData(hci_cmd_write_addr, sizeof(hci_cmd_write_addr));
    btdrv_delay(20);
}

void btdrv_enable_dut(void)
{
    btdrv_SendData(hci_cmd_enable_dut, sizeof(hci_cmd_enable_dut));
    btdrv_delay(100);
#if 0
    btdrv_SendData(hci_cmd_inquiry_scan_activity, sizeof(hci_cmd_inquiry_scan_activity));
    btdrv_delay(100);
    btdrv_SendData(hci_cmd_page_scan_activity, sizeof(hci_cmd_page_scan_activity));
    btdrv_delay(100);
#endif
    btdrv_SendData(hci_cmd_enable_allscan, sizeof(hci_cmd_enable_allscan));
    btdrv_delay(100);
    btdrv_SendData(hci_cmd_autoaccept_connect, sizeof(hci_cmd_autoaccept_connect));
    btdrv_delay(100);
    bt_drv_reg_op_set_accessible_mode(3);
    btdrv_dut_mode_enable = true;
}


void btdrv_enable_dut_again(void)
{
    btdrv_SendData(hci_cmd_enable_allscan, sizeof(hci_cmd_enable_allscan));
    btdrv_delay(20);
    bt_drv_reg_op_set_accessible_mode(3);
    btdrv_dut_mode_enable = true;
}

void btdrv_enable_ibrt_test(void)
{
    btdrv_SendData(hci_cmd_enable_ibrt_test, sizeof(hci_cmd_enable_ibrt_test));
    btdrv_delay(20);
    btdrv_SendData(hci_cmd_set_ibrt_mode, sizeof(hci_cmd_set_ibrt_mode));
    btdrv_delay(20);
}

void btdrv_connect_ibrt_device(uint8_t *addr)
{
    uint8_t hci_cmd_connect_device[17] =
    {
        0x01, 0x05, 0x04, 0x0D, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01, 0x18, 0xCC,
        0x01, 0x00, 0x00, 0x00, 0x00
    };
    memcpy(&hci_cmd_connect_device[4],addr,6);
    btdrv_SendData(hci_cmd_connect_device, sizeof(hci_cmd_connect_device));
    btdrv_delay(50);

}

void btdrv_disable_scan(void)
{
    btdrv_SendData(hci_cmd_disable_scan, sizeof(hci_cmd_disable_scan));
    btdrv_delay(20);
}

static uint32_t dut_connect_status = DUT_CONNECT_STATUS_DISCONNECTED;

uint32_t btdrv_dut_get_connect_status(void)
{
    return dut_connect_status;
}

void btdrv_enable_dut_again(void);
void btdrv_dut_accessible_mode_manager(const unsigned char *data)
{
    if(btdrv_dut_mode_enable)
    {
        if(data[0]==0x04&&data[1]==0x03&&data[2]==0x0b&&data[3]==0x00)
        {
#ifdef __IBRT_IBRT_TESTMODE__
            if(memcmp(&data[6],bt_addr,6))
            {
                bt_drv_reg_op_set_accessible_mode(0);
                btdrv_disable_scan();
            }
#else
            bt_drv_reg_op_set_accessible_mode(0);
            btdrv_disable_scan();
#endif
            dut_connect_status = DUT_CONNECT_STATUS_CONNECTED;
        }
        else if(data[0]==0x04&&data[1]==0x05&&data[2]==0x04&&data[3]==0x00)
        {
            btdrv_enable_dut_again();
            dut_connect_status = DUT_CONNECT_STATUS_DISCONNECTED;
        }
    }
}

void btdrv_hci_reset(void)
{
    btdrv_SendData(hci_cmd_hci_reset, sizeof(hci_cmd_hci_reset));
    btdrv_delay(350);
}

void btdrv_enable_nonsig_tx(uint8_t index)
{
    BT_DRV_TRACE(1,"%s\n", __func__);

    if (index == 0)
        btdrv_SendData(hci_cmd_nonsig_tx_2dh1_pn9_t0, sizeof(hci_cmd_nonsig_tx_2dh1_pn9_t0));
    else if (index == 1)
        btdrv_SendData(hci_cmd_nonsig_tx_3dh1_pn9_t0, sizeof(hci_cmd_nonsig_tx_3dh1_pn9_t0));
    else if (index == 2)
        btdrv_SendData(hci_cmd_nonsig_tx_2dh3_pn9_t0, sizeof(hci_cmd_nonsig_tx_2dh1_pn9_t0));
    else if (index == 3)
        btdrv_SendData(hci_cmd_nonsig_tx_3dh3_pn9_t0, sizeof(hci_cmd_nonsig_tx_3dh1_pn9_t0));
    else
        btdrv_SendData(hci_cmd_nonsig_tx_dh1_pn9_t0, sizeof(hci_cmd_nonsig_tx_dh1_pn9_t0));

    btdrv_delay(20);

}

void btdrv_enable_nonsig_rx(uint8_t index)
{
    BT_DRV_TRACE(1,"%s\n", __func__);

    if (index == 0)
        btdrv_SendData(hci_cmd_nonsig_rx_2dh1_pn9_t0, sizeof(hci_cmd_nonsig_rx_2dh1_pn9_t0));
    else if (index == 1)
        btdrv_SendData(hci_cmd_nonsig_rx_3dh1_pn9_t0, sizeof(hci_cmd_nonsig_rx_3dh1_pn9_t0));
    else if (index == 2)
        btdrv_SendData(hci_cmd_nonsig_rx_2dh3_pn9_t0, sizeof(hci_cmd_nonsig_rx_2dh1_pn9_t0));
    else if (index == 3)
        btdrv_SendData(hci_cmd_nonsig_rx_3dh3_pn9_t0, sizeof(hci_cmd_nonsig_rx_3dh1_pn9_t0));
    else
        btdrv_SendData(hci_cmd_nonsig_rx_dh1_pn9_t0, sizeof(hci_cmd_nonsig_rx_dh1_pn9_t0));

    btdrv_delay(20);
}

static bool btdrv_vco_test_running = false;
static uint16_t vco_test_reg_val_3b0 = 0;
#ifdef VCO_TEST_TOOL
static unsigned short vco_test_hack_flag = 0;
static unsigned short vco_test_channel = 0xff;

unsigned short btdrv_get_vco_test_process_flag(void)
{
    return vco_test_hack_flag;
}

bool btdrv_vco_test_bridge_intsys_callback(const unsigned char *data)
{
    bool status = false;
    if(data[0]==0x01 &&data[1]==0xaa&&data[2]==0xfc &&data[3]==0x02)
    {
        status = true;
        vco_test_hack_flag = data[5];
        vco_test_channel = data[4];
    }

    return status;
}

void btdrv_vco_test_process(uint8_t op)
{
    if(op == 0x02)//vco test start
    {
        if(vco_test_channel != 0xff)
            btdrv_vco_test_start(vco_test_channel);
    }
    else if(op ==0x04)//vco test stop
    {
        btdrv_vco_test_stop();
    }
    vco_test_channel =0xff;
    vco_test_hack_flag = 0;
}
#endif

void btdrv_vco_test_start(uint8_t chnl)
{
    if (!btdrv_vco_test_running)
    {
        btdrv_vco_test_running = true;
        btdrv_read_rf_reg(0x3B0, &vco_test_reg_val_3b0);

        btdrv_reg_op_vco_test_start(chnl);

        BTRF_REG_SET_FIELD(0x3B0, 0x1, 9, 0);   //Transmit signal without modulation
    }
}

void btdrv_vco_test_stop(void)
{
    if (btdrv_vco_test_running)
    {
        btdrv_vco_test_running = false;
        btdrv_reg_op_vco_test_stop();

        if(0 != vco_test_reg_val_3b0)
        {
            btdrv_write_rf_reg(0x3B0, vco_test_reg_val_3b0);
        }
    }
}

void btdrv_stop_bt(void)
{
    btdrv_poweron(BT_POWEROFF);
    btdrv_pm_deregister_notif_handler();
}

void btdrv_write_memory(uint8_t wr_type,uint32_t address,const uint8_t *value,uint8_t length)
{
    uint8_t buff[256];
    if(length ==0 || length >128)
        return;
    buff[0] = 0x01;
    buff[1] = 0x02;
    buff[2] = 0xfc;
    buff[3] = length + 6;
    buff[4] = address & 0xff;
    buff[5] = (address &0xff00)>>8;
    buff[6] = (address &0xff0000)>>16;
    buff[7] = address>>24;
    buff[8] = wr_type;
    buff[9] = length;
    memcpy(&buff[10],value,length);
    btdrv_SendData(buff,length+10);
    btdrv_delay(2);


}

void btdrv_send_cmd(uint16_t opcode,uint8_t cmdlen,const uint8_t *param)
{
    uint8_t buff[256];
    buff[0] = 0x01;
    buff[1] = opcode & 0xff;
    buff[2] = (opcode &0xff00)>>8;
    buff[3] = cmdlen;
    if(cmdlen>0)
        memcpy(&buff[4],param,cmdlen);
    btdrv_SendData(buff,cmdlen+4);
}

//[26:0] 0x07ffffff
//[27:0] 0x0fffffff

uint32_t btdrv_syn_get_curr_ticks(void)
{
    uint32_t value;

    value = btdrv_reg_op_syn_get_curr_ticks();
    return value;
}

uint32_t btdrv_syn_get_cis_curr_time(void)
{
    uint32_t value;

    value = btdrv_reg_op_syn_get_cis_curr_time();
    return value;
}

// Clear trigger signal with software
void  btdrv_syn_clr_trigger(uint8_t trig_route)
{
    btdrv_reg_op_sync_clr_trigger(trig_route);
}


#ifdef __SW_TRIG__

uint16_t Tbit_M_ori_flag = 200;
#define RF_DELAY  18
uint16_t sw_trig_conhdl = 0;
uint8_t sw_trig_tws_role = 0;
uint16_t sw_trig_tws_conhdl = 0;
int32_t tg_clkoffset = 0;
uint16_t tg_finecnt = 0;

extern "C" void btdrv_sync_sw_trig_store_conhdl(uint16_t conhdl)
{
    sw_trig_conhdl = conhdl;
}

extern "C" uint16_t btdrv_sync_sw_trig_get_conhdl(void)
{
    return sw_trig_conhdl;
}

extern "C" void btdrv_sync_sw_trig_store_tws_conhdl(uint16_t conhdl)
{
    sw_trig_tws_conhdl = conhdl;
}

extern "C" uint16_t btdrv_sync_sw_trig_get_tws_conhdl(void)
{
    return sw_trig_tws_conhdl;
}

void btdrv_sync_sw_trig_store_tws_role(uint8_t role)
{
    sw_trig_tws_role = role;
}

uint8_t btdrv_sync_sw_trig_get_tws_role(void)
{
    return sw_trig_tws_role;
}

void btdrv_sync_sw_trig_store_tg_clkoffset(int32_t clkoffset)
{
    tg_clkoffset = clkoffset;
}

int32_t btdrv_sync_sw_trig_get_tg_clkoffset(void)
{
    return tg_clkoffset;
}

void btdrv_sync_sw_trig_store_tg_finecnt(uint16_t finecnt)
{
    tg_finecnt = finecnt;
}

uint16_t btdrv_sync_sw_trig_get_tg_finecnt(void)
{
    return tg_finecnt;
}

#define ROUNDDOWN(x)   ((int)(x))
#define ROUNDUP(x)     ((int)(x) + ((x-(int)(x)) > 0 ? 1 : 0))
#define ROUND(x)       ((int)((x) + ((x) > 0 ? 0.5 : -0.5)))

uint16_t btdrv_Tbit_M_h_get(uint32_t Tclk_M, int16_t Tbit_M_h_ori)
{
    uint16_t Tbit_M_h;
    Tbit_M_h = Tbit_M_h_ori;
    BT_DRV_TRACE(1,"[%s]:%d\n",__func__,Tbit_M_h);
    return Tbit_M_h;
}


uint16_t btdrv_Tbit_M_get(uint32_t Tclk_M, uint16_t Tbit_M_h)
{
    uint16_t Tbit_M;
    if(Tbit_M_h % 2)///if Tbit_M_h is odd
    {
        Tbit_M = (uint16_t)ROUNDUP(Tbit_M_h/2) + 1;
    }
    else
    {
        Tbit_M = (uint16_t)ROUNDUP(Tbit_M_h/2);
    }
    BT_DRV_TRACE(1,"[%s]:%d\n",__func__,Tbit_M);
    return Tbit_M;
}


uint32_t btdrv_rxclkn_get(uint16_t conhdl)
{
    uint32_t rxclkn;
    uint16_t rxclkn0;
    uint16_t rxclkn1;
    uint8_t conidx;
    conidx = btdrv_conhdl_to_linkid(conhdl);
    rxclkn0 = btdrv_reg_op_rxclkn_get(0,conidx);
    rxclkn1 = btdrv_reg_op_rxclkn_get(1,conidx);
    rxclkn = (rxclkn0 & 0x0000ffff) | ((rxclkn1 << 16) & 0x0fff0000);
    BT_DRV_TRACE(1,"[%s] conhdl=0x%x,rxclkn=%d\n",__func__,conhdl,rxclkn);
    return rxclkn;
}

uint16_t btdrv_rxbit_get(uint16_t conhdl)
{
    uint16_t rxbit;
    uint8_t conidx;
    conidx = btdrv_conhdl_to_linkid(conhdl);
    rxbit = btdrv_reg_op_rxbit_get(conidx);
    BT_DRV_TRACE(1,"[%s] conhdl=0x%x,rxbit=0x%x,%d\n",__func__,conhdl,rxbit,rxbit & 0x3ff);
    return (rxbit & 0x3ff);
}

uint16_t btdrv_fcnt_1us_get(uint32_t rxclkn,uint16_t rxbit)
{
    uint16_t fcnt_1us;
    uint16_t tmp;
    if(rxbit % 2)
    {
        //if rxbit is odd
        tmp = ROUND(rxbit/2) + 1;
    }
    else
    {
        tmp = ROUND(rxbit/2);
    }
    if(rxclkn % 2)
    {
        fcnt_1us = tmp;
    }
    else
    {
        fcnt_1us = tmp + 312;
    }
    BT_DRV_TRACE(1,"[%s] fcnt_1us=%d\n",__func__,fcnt_1us);
    return fcnt_1us;
}



extern "C" int32_t bt_syn_get_clkoffset(uint16_t conhdl)
{
    int32_t offset;

    if(conhdl>=0x80)
        offset = bt_drv_reg_op_get_clkoffset(conhdl-0x80);
    else
        offset = 0;

    return offset;
}

int32_t btdrv_slotoff_get(uint16_t conhdl,int32_t clkoff)
{
    int32_t slotoff;

    if(clkoff < 0)
    {
        slotoff = -(int32_t)ROUNDDOWN((-clkoff)/2);
    }
    else
    {
        if(clkoff % 2)
        {
            slotoff = (int32_t)ROUNDUP(clkoff/2) + 1;
        }
        else
        {
            slotoff = (int32_t)ROUNDUP(clkoff/2);
        }
    }
    BT_DRV_TRACE(1,"[%s]:%d\n",__func__,slotoff);
    return slotoff;
}

int16_t btdrv_bitoff_get(uint16_t rxbit_1us,int32_t clkoff)
{
    int16_t bitoff;

    bitoff = rxbit_1us - 68 - RF_DELAY;
    BT_DRV_TRACE(1,"[%s] bitoff=%d\n",__func__,bitoff);
    if((bitoff < 0) && (clkoff % 2))
    {
        bitoff += 624;
    }
    BT_DRV_TRACE(1,"[%s]:%d\n",__func__,bitoff);
    return bitoff;
}

uint16_t btdrv_Tbit_S_get(uint16_t Tbit_M, int bitoff)
{
    uint16_t Tbit_S;
    int temp;

    temp = Tbit_M - bitoff;
    if(temp < 0)
    {
        Tbit_S = temp + 625;
    }
    else if(temp > 624)
    {
        Tbit_S = temp - 625;
    }
    else
    {
        Tbit_S = temp;
    }
    BT_DRV_TRACE(1,"[%s]:%d\n",__func__,Tbit_S);
    return Tbit_S;
}

uint8_t btdrv_clk_adj_M_get(uint16_t Tbit_M,int16_t bitoff)
{
    uint8_t clk_adj_M;

    if(bitoff >= Tbit_M)
    {
        clk_adj_M = 1;
    }
    else
    {
        clk_adj_M = 0;
    }
    BT_DRV_TRACE(1,"[%s]:%d\n",__func__,clk_adj_M);
    return clk_adj_M;
}

uint8_t btdrv_clk_adj_S_get(uint8_t clk_adj_M,uint16_t Tbit_M,int16_t bitoff)
{
    uint8_t clk_adj_S;

    if(clk_adj_M > 0)
    {
        int16_t temp = (bitoff - Tbit_M)*2;
        if(temp > 624)
        {
            clk_adj_S = 1;
        }
        else
        {
            clk_adj_S = 0;
        }
    }
    else
    {
        clk_adj_S = 0;
    }
    BT_DRV_TRACE(1,"[%s]:%d\n",__func__,clk_adj_S);
    return clk_adj_S;
}

uint32_t btdrv_Tclk_S_get(uint32_t Tclk_M, uint8_t clk_adj_M,
                                uint8_t clk_adj_S, int32_t slotoff)
{
    uint32_t Tclk_S;
    Tclk_S = Tclk_M - slotoff*2 + clk_adj_M + clk_adj_S;
    Tclk_S &= 0x0fffffff;
    BT_DRV_TRACE(1,"[%s]:%d\n",__func__,Tclk_S);
    return Tclk_S;
}


uint16_t btdrv_Tclk_S_h_get(uint8_t clk_adj_M,uint8_t clk_adj_S,
                                    uint16_t Tbit_M,int16_t bitoff)
{
    uint16_t Tclk_S_h;
    if(clk_adj_M == 0)
    {
        Tclk_S_h = (Tbit_M - bitoff)*2;
    }
    else
    {
        if(clk_adj_S == 1)
        {
            Tclk_S_h = (1248-(bitoff-Tbit_M)*2);
        }
        else
        {
            Tclk_S_h = (624-(bitoff-Tbit_M)*2);
        }
    }
    BT_DRV_TRACE(1,"[%s]:%d\n",__func__,Tclk_S_h);
    return Tclk_S_h;
}

void btdrv_sw_trig_tg_finecnt_set(uint16_t tg_bitcnt, uint8_t trig_route)
{
    btdrv_reg_op_sw_trig_tg_finecnt_set(tg_bitcnt,trig_route);
}

uint16_t btdrv_sw_trig_tg_finecnt_get(uint8_t trig_route)
{
    uint16_t finecnt = 0;
    finecnt = btdrv_reg_op_sw_trig_tg_finecnt_get(trig_route);
    return finecnt;
}

void btdrv_sw_trig_tg_clkncnt_set(uint32_t num, uint8_t trig_route)
{
    btdrv_reg_op_sw_trig_tg_clkncnt_set(num,trig_route);
}

void btdrv_sw_trig_slave_calculate_and_set(uint16_t conhdl, uint32_t Tclk_M, uint16_t Tbit_M_h_ori, uint8_t trig_route)
{
    uint16_t Tbit_M_h;
    uint16_t Tbit_M;
    uint16_t rxbit_1us;
    int16_t bitoff;
    int32_t clkoff;
    int32_t slotoff;
    uint8_t clk_adj_M;
    uint8_t clk_adj_S;
    uint32_t Tclk_S;
    uint32_t Tclk_S_h;

    Tbit_M_h = Tbit_M_ori_flag;
    Tbit_M = btdrv_Tbit_M_get(Tclk_M,Tbit_M_h);
    rxbit_1us = bt_drv_reg_op_rxbit_1us_get(conhdl);
    clkoff = bt_syn_get_clkoffset(conhdl);
    slotoff = btdrv_slotoff_get(conhdl,clkoff);
    bitoff = btdrv_bitoff_get(rxbit_1us,clkoff);
    clk_adj_M = btdrv_clk_adj_M_get(Tbit_M,bitoff);
    clk_adj_S = btdrv_clk_adj_S_get(clk_adj_M,Tbit_M,bitoff);
    Tclk_S = btdrv_Tclk_S_get(Tclk_M,clk_adj_M,clk_adj_S,slotoff);
    Tclk_S_h = btdrv_Tclk_S_h_get(clk_adj_M,clk_adj_S,Tbit_M,bitoff);

    btdrv_sw_trig_tg_clkncnt_set(Tclk_S,trig_route);
    btdrv_sw_trig_tg_finecnt_set(Tclk_S_h,trig_route);
    btdrv_sw_trig_en_set(trig_route);
}

void btdrv_sw_trig_master_set(uint32_t Tclk_M, uint16_t Tbit_M_h_ori, uint8_t trig_route)
{
    uint16_t Tbit_M_h;

    btdrv_sw_trig_tg_clkncnt_set(Tclk_M,trig_route);
    Tbit_M_h = btdrv_Tbit_M_h_get(Tclk_M,Tbit_M_h_ori);
    btdrv_sw_trig_tg_finecnt_set(Tbit_M_h,trig_route);
    btdrv_sw_trig_en_set(trig_route);
}

#endif

void bt_syn_cancel_tg_ticks(uint8_t trig_route)
{
#ifdef __SW_TRIG__
    btdrv_sw_trig_disable_set(trig_route);
#endif
}

static void btdrv_syn_set_tg_ticks(uint32_t num, uint8_t mode, uint16_t conhdl, uint8_t trig_route)
{
    if (mode == BT_TRIG_MASTER_ROLE)
    {
#ifdef __SW_TRIG__
        BT_DRV_TRACE(1,"[%s] __SW_TRIG__ conhdl=0x%x num=%d trig_route=%d\n",__func__,conhdl,num,trig_route);
        btdrv_sw_trig_master_set(num, Tbit_M_ori_flag, trig_route);
#else
        btdrv_reg_op_syn_set_tg_ticks_master_role();
#endif
    }
    else
    {
#ifdef __SW_TRIG__
        //uint16_t conhdl = btdrv_sync_sw_trig_get_conhdl();
        BT_DRV_TRACE(1,"[%s] __SW_TRIG__ conhdl=0x%x num=%d trig_route=%d\n",__func__,conhdl,num,trig_route);
        btdrv_sw_trig_slave_calculate_and_set(conhdl, num, Tbit_M_ori_flag,trig_route);
#else
        btdrv_reg_op_syn_set_tg_ticks_slave_role(num);
#endif
    }
}

void btdrv_syn_trigger_codec_en(uint32_t v)
{
    btdrv_reg_op_syn_trigger_codec_en(v);
}

// Can be used by master or slave
// Ref: Master bt clk
uint32_t bt_syn_get_curr_ticks(uint16_t conhdl)
{
    int32_t curr,offset;

    curr = btdrv_syn_get_curr_ticks();

    if (btdrv_is_link_index_valid(btdrv_conhdl_to_linkid(conhdl)))
        offset = bt_drv_reg_op_get_clkoffset(btdrv_conhdl_to_linkid(conhdl));
    else
        offset = 0;
//    BT_DRV_TRACE(4,"[%s] curr(%d) + offset(%d) = %d", __func__, curr , offset,curr + offset);
    return (curr + offset) & 0x0fffffff;
}

void bt_syn_trig_checker(uint16_t conhdl)
{
}

// Can be used by master or slave
// Ref: Master bt clk
void bt_syn_set_tg_ticks(uint32_t val,uint16_t conhdl, uint8_t mode, uint8_t trig_route, bool no_link_trig)
{
    int32_t offset;
    uint8_t link_id = btdrv_conhdl_to_linkid(conhdl);

    if(no_link_trig == false)
    {
        if(HCI_LINK_INDEX_INVALID == link_id)
        {
            BT_DRV_TRACE(3,"%s,ERR INVALID CONHDL 0x%x!! ca=%p",__func__,conhdl, __builtin_return_address(0));
            return;
        }
    }
    if (btdrv_is_link_index_valid(link_id))
    {
        offset = bt_drv_reg_op_get_clkoffset(link_id);
    }
    else
    {
        offset = 0;
    }

#if !defined(__SW_TRIG__)
    btdrv_reg_op_syn_set_tg_ticks_linkid(link_id);
#endif

    if ((mode == BT_TRIG_MASTER_ROLE) && (offset !=0))
    {
        BT_DRV_TRACE(0,"ERROR OFFSET !!");
    }

#if !defined(__SW_TRIG__)
    val = val>>1;
    val = val<<1;
    val += 1;
#endif

    bt_syn_cancel_tg_ticks(trig_route);
    BT_DRV_TRACE(4,"bt_syn_set_tg_ticks val:%d num:%d mode:%d conhdl:%02x", val, val - offset, mode, conhdl);
    btdrv_syn_set_tg_ticks(val, mode, conhdl, trig_route);
    bt_syn_trig_checker(conhdl);
}

void bt_syn_ble_set_tg_ticks(uint32_t val,uint8_t trig_route)
{
#ifdef __SW_TRIG__
    uint32_t clkncnt = 0;
    uint16_t finecnt = 0;

    btdrv_syn_trigger_codec_en(0);
    btdrv_syn_clr_trigger(0);
    btdrv_reg_op_bts_to_bt_time(val, &clkncnt, &finecnt);

    btdrv_sw_trig_tg_clkncnt_set(clkncnt,trig_route);
    btdrv_sw_trig_tg_finecnt_set(finecnt,trig_route);
    btdrv_syn_trigger_codec_en(1);
    btdrv_sw_trig_en_set(trig_route);
#endif
}

uint32_t bt_syn_ble_bt_time_to_bts(uint32_t hs, uint16_t hus)
{
    return btdrv_reg_op_bt_time_to_bts(hs, hus);
}

void btdrv_enable_playback_triggler(uint8_t triggle_mode)
{
    btdrv_reg_op_enable_playback_triggler(triggle_mode);
}


void btdrv_play_trig_mode_set(uint8_t mode)
{
    btdrv_reg_op_play_trig_mode_set(mode);
}


void btdrv_disable_playback_triggler(void)
{
    btdrv_reg_op_disable_playback_triggler();
}

void btdrv_set_bt_pcm_triggler_en(uint8_t  en)
{

}

void btdrv_set_bt_pcm_triggler_delay(uint8_t  delay)
{
}


void btdrv_set_bt_pcm_en(uint8_t  en)
{

}

//pealse use btdrv_is_link_index_valid() check link index whether valid
uint8_t btdrv_conhdl_to_linkid(uint16_t connect_hdl)
{
    //invalid hci handle,such as link disconnected
    if(connect_hdl < HCI_HANDLE_MIN || connect_hdl > HCI_HANDLE_MAX)
    {
        TRACE(2, "ERROR Connect Handle=0x%x ca=%p", connect_hdl, __builtin_return_address(0));
        return HCI_LINK_INDEX_INVALID;
    }
    else
    {
        return (connect_hdl - HCI_HANDLE_MIN);
    }
}

void btdrv_pcm_enable(void)
{
    btdrv_reg_op_pcm_enable();
}

void btdrv_pcm_disable(void)
{
    btdrv_reg_op_pcm_disable();
}

// Trace tport
static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_tport[] =
{
    {HAL_IOMUX_PIN_P0_0, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE},
};

int btdrv_host_gpio_tport_open(void)
{
    uint32_t i;

    for (i=0; i<sizeof(pinmux_tport)/sizeof(struct HAL_IOMUX_PIN_FUNCTION_MAP); i++)
    {
        hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&pinmux_tport[i], 1);
        hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)pinmux_tport[i].pin, HAL_GPIO_DIR_OUT, 0);
    }
    return 0;
}

int btdrv_gpio_port_set(int port)
{
    hal_gpio_pin_set((enum HAL_GPIO_PIN_T)pinmux_tport[port].pin);
    return 0;
}

int btdrv_gpio_tport_clr(int port)
{
    hal_gpio_pin_clr((enum HAL_GPIO_PIN_T)pinmux_tport[port].pin);
    return 0;
}
void btdrv_set_powerctrl_rssi_low(uint16_t rssi)
{
}

extern void bt_drv_reg_op_set_music_link(uint8_t link_id);

void btdrv_enable_dual_slave_configurable_slot_mode(bool isEnable,
        uint16_t activeDevHandle, uint8_t activeDevRole,
        uint16_t idleDevHandle, uint8_t idleDevRole)
{
    if(isEnable)
    {
        bt_drv_reg_op_set_music_link(activeDevHandle-0x80);
    }
    else
    {
        bt_drv_reg_op_set_music_link(0xff);
    }
}

#if defined(TX_RX_PCM_MASK)
uint8_t  btdrv_is_pcm_mask_enable(void)
{
    return 1;
}
#endif

#ifdef PCM_FAST_MODE
void btdrv_open_pcm_fast_mode_enable(void)
{
    btdrv_reg_op_open_pcm_fast_mode_enable();
}
void btdrv_open_pcm_fast_mode_disable(void)
{
    btdrv_reg_op_open_pcm_fast_mode_disable();
}
#endif

#if defined(CVSD_BYPASS)
void btdrv_cvsd_bypass_enable(void)
{
    btdrv_reg_op_cvsd_bypass_enable();
}
#endif

void btdrv_enable_rf_sw(int rx_on, int tx_on)
{
    hal_iomux_set_bt_rf_sw(rx_on, tx_on);
    //maybe affect the use of Tport
    BTDIGITAL_REG(0xD0340020) |= 1<<5;
}

void bt_drv_reg_op_ecc_softbit_process(uint16_t* p_conn_handle1,uint16_t* p_conn_handle2, uint16_t length, uint8_t *data)
{

}

//tx level 0~64
//default tx power 10db - 20*log(d0350308(9:0)/2^d0350300(3:0))
//d0350308(19:10) (9:0) should the same
//d0350300(7:4\3:0) should the same
void btdrv_set_bdr_ble_txpower(uint8_t div, uint8_t power_level)
{
    uint32_t reg32 = 0;

    if(power_level >= (uint8_t)pow(2,div)){
        power_level = (uint8_t)pow(2,div);
    }else if(power_level){
        power_level += 1;
    }
    div += div<<4;
    reg32 = BTDIGITAL_REG(0xd0350300);
    if((reg32 & 0xff) != div){
        reg32 &= ~0xff;
        reg32 |= div;
        BTDIGITAL_REG(0xd0350300) = reg32;
    }

    reg32 = BTDIGITAL_REG(0xd0350308);
    reg32&=  ~0xfffff;
    reg32 |= (power_level << 10) | power_level;
    BTDIGITAL_REG(0xd0350308) = reg32;
    factory_section_set_bt_tx_power(0, div, reg32);
}

//tx level 0~64
//default tx power 10db - 20*log(d0350344(9:0)/2^d0350340(3:0))
void btdrv_set_edr_txpower(uint8_t div, uint8_t power_level)
{
    uint32_t reg32 = 0;

    if(power_level >= (uint8_t)pow(2,div)){
        power_level = (uint8_t)pow(2,div);
    }else if(power_level){
        power_level += 1;
    }
    reg32 = BTDIGITAL_REG(0xd0350340);
    if((reg32 & 0xf) != div){
        reg32 &= ~0xf;
        reg32 |= div;
        BTDIGITAL_REG(0xd0350340) = reg32;
    }

    reg32 = BTDIGITAL_REG(0xd0350344);
    reg32 &=  ~0x3ff;
    reg32 |= power_level;
    BTDIGITAL_REG(0xd0350344) = reg32;
    factory_section_set_bt_tx_power(2, div, reg32);
 }

void btdrv_switch_to_i2c(void)
{
    hal_iomux_set_analog_i2c();
}

void write_reg32(uint32_t reg32, uint32_t value)
{
    TRACE(2,"0x%08x 0x%04x",reg32, BTDIGITAL_REG(reg32));
    BTDIGITAL_REG(reg32) = value;
    TRACE(2,"0x%08x 0x%04x",reg32, BTDIGITAL_REG(reg32));
}

void read_reg32(uint32_t reg32)
{
    TRACE(2,"0x%08x 0x%04x",reg32, BTDIGITAL_REG(reg32));
}

void write_reg16(uint8_t reg16, uint16_t value)
{
    btdrv_write_rf_reg(reg16, value);
    TRACE(2,"0x%02x 0x%04x",reg16, value);
}

void read_reg16(uint8_t reg16)
{
    uint16_t reg16_val;
    btdrv_read_rf_reg(reg16, &reg16_val);
    TRACE(2,"0x%02x 0x%04x",reg16, reg16_val);
}

void wifi_write_reg16(uint8_t reg16, uint16_t value)
{
    btdrv_write_wifi_reg(reg16, value);
    TRACE(2,"0x%02x 0x%04x",reg16, value);
}

void wifi_read_reg16(uint8_t reg16)
{
    uint16_t reg16_val;
    btdrv_read_wifi_reg(reg16, &reg16_val);
    TRACE(2,"0x%02x 0x%04x",reg16, reg16_val);
}

void nosignal_set_max_power_level(void)
{
    uint32_t reg32;
    uint16_t reg16;
    btdrv_read_rf_reg(0x1a, &reg16);
    reg16 = (reg16 & 0xFC3F) | 0x0140;
    btdrv_write_rf_reg(0x1a, reg16);

    reg32 = BTDIGITAL_REG(0xd02204C4);
    BTDIGITAL_REG(0xd02204C4) = reg32 | 0x80000000;
    btdrv_delay(10);
}

void nosignal_clear_max_power_level(void)
{
    uint32_t reg32;
    uint16_t reg16;
    btdrv_read_rf_reg(0x1a, &reg16);
    reg16 = reg16 & 0xFC3F;
    btdrv_write_rf_reg(0x1a, reg16);

    reg32 = BTDIGITAL_REG(0xd02204C4);
    BTDIGITAL_REG(0xd02204C4) = reg32 & 0x7FFFFFFF;
    btdrv_delay(10);
}
