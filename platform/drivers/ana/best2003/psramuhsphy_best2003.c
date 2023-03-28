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
#include "string.h"
#include "psramuhsphy.h"
#include "hal_cmu.h"
#include "hal_phyif.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "hal_psramuhs.h"
#include "hal_cache.h"
#include "pmu.h"
#include CHIP_SPECIFIC_HDR(reg_psramuhsphy)

//#define NO_CALIB

enum PSRAM_ID_T {
    PSRAM_ID_0,
    PSRAM_ID_QTY,
};

#ifdef PSRAMUHS_DEBUG
    #define PSRAMUHS_TRACE TRACE_IMM
#else
    #define PSRAMUHS_TRACE(...)
#endif

static void psramuhs_read(enum PSRAM_ID_T id, uint16_t reg, uint16_t *val)
{
    ASSERT(id < PSRAM_ID_QTY, "%s: Bad psram id: %d", __func__, id);
    psramuhsphy0_read(reg, val);
}

static void psramuhs_write(enum PSRAM_ID_T id, uint16_t reg, uint16_t val)
{
    ASSERT(id < PSRAM_ID_QTY, "%s: Bad psram id: %d", __func__, id);
    psramuhsphy0_write(reg, val);
}
void psramuhsphy_set_range(uint32_t range)
{
    uint16_t val;
    //psramuhs_read(PSRAM_ID_0, PSRAMPHY_REG_03, &val);
    //val = SET_BITFIELD(val, ANA0_REG_RANGE_CK, range);
    //psramuhs_write(PSRAM_ID_0, PSRAMPHY_REG_03, val);

    psramuhs_read(PSRAM_ID_0, PSRAMPHY_REG_13, &val);
    val = SET_BITFIELD(val, ANA0_REG_PSRAM_DLL_RANGE, range);
    psramuhs_write(PSRAM_ID_0, PSRAMPHY_REG_13, val);
}

void psramuhsphy_open(uint32_t clk)
{
    int ret;
    uint16_t val;
    uint16_t range;
    ret = hal_phyif_open(SPIPHY_PSRAMUHS0_CS);
    if (ret) {
        return;
    }

    if (clk <= 300000000 / 2) {
        range = 3;
    } else if (clk <= 500000000 / 2) {
        range = 2;
    } else if (clk <= 1066000000 / 2) {
        range = 1;
    } else {
        range = 0;
    }
    psramuhsphy_set_range(range);

    psramuhs_read(PSRAM_ID_0, PSRAMPHY_REG_10, &val);
#ifdef PSRAMULS_MODE
    val = SET_BITFIELD(val, ANA0_REG_PSRAM_OCD_CLKP, 0xc);
    val = SET_BITFIELD(val, ANA0_REG_PSRAM_OCD_CLKN, 0xc);
#else
    val = SET_BITFIELD(val, ANA0_REG_PSRAM_OCD_CLKP, 0x4);
    val = SET_BITFIELD(val, ANA0_REG_PSRAM_OCD_CLKN, 0x4);
#endif
    psramuhs_write(PSRAM_ID_0, PSRAMPHY_REG_10, val);

    psramuhs_read(PSRAM_ID_0, PSRAMPHY_REG_11, &val);
#ifdef PSRAMULS_MODE
    val = SET_BITFIELD(val, ANA0_REG_PSRAM_OCD_DQSP, 0xc);
    val = SET_BITFIELD(val, ANA0_REG_PSRAM_OCD_DQSN, 0xc);
#else
    val = SET_BITFIELD(val, ANA0_REG_PSRAM_OCD_DQSP, 0x4);
    val = SET_BITFIELD(val, ANA0_REG_PSRAM_OCD_DQSN, 0x4);
#endif
    psramuhs_write(PSRAM_ID_0, PSRAMPHY_REG_11, val);

    psramuhs_read(PSRAM_ID_0, PSRAMPHY_REG_12, &val);
#ifdef PSRAMULS_MODE
    val = SET_BITFIELD(val, ANA0_REG_PSRAM_TXDRVP, 0xc);
    val = SET_BITFIELD(val, ANA0_REG_PSRAM_TXDRVN, 0xc);
#else
    val = SET_BITFIELD(val, ANA0_REG_PSRAM_TXDRVP, 0x2);
    val = SET_BITFIELD(val, ANA0_REG_PSRAM_TXDRVN, 0x2);
#endif
    psramuhs_write(PSRAM_ID_0, PSRAMPHY_REG_12, val);

    psramuhs_read(PSRAM_ID_0, PSRAMPHY_REG_02, &val);
#ifdef PSRAMULS_MODE
    val = SET_BITFIELD(val, ANA0_REG_VREF_VTUNE, 0x4);
    val = SET_BITFIELD(val, ANA0_REG_LDO_AVDDQ_RES, 0xA);
#else
    val = SET_BITFIELD(val, ANA0_REG_VREF_VTUNE, 0x0);
#endif
    val = SET_BITFIELD(val, ANA0_REG_VH_RE, 0x1);
    val = SET_BITFIELD(val, ANA0_REG_VL_RES, 0x1);
    val = SET_BITFIELD(val, ANA0_REG_LDO_DVDD_RES, 0xF);
    psramuhs_write(PSRAM_ID_0, PSRAMPHY_REG_02, val);

    psramuhs_read(PSRAM_ID_0, PSRAMPHY_REG_03, &val);
    val = SET_BITFIELD(val, ANA0_REG_RANGE_CK, 0x0);
    val = SET_BITFIELD(val, ANA0_REG_DVDD_SWRC, 0x3);
    psramuhs_write(PSRAM_ID_0, PSRAMPHY_REG_03, val);

    psramuhs_read(PSRAM_ID_0, PSRAMPHY_REG_13, &val);
#ifdef BEST2003_VCORE_0_95V
    val = SET_BITFIELD(val, ANA0_REG_PSRAM_TX_RD_CK_DLY, 0x7);
#else
    val = SET_BITFIELD(val, ANA0_REG_PSRAM_TX_RD_CK_DLY, 0x1);
#endif
    val |= ANA0_REG_PSRAM_TX_RD_POINTER_PSEL;
    psramuhs_write(PSRAM_ID_0, PSRAMPHY_REG_13, val);

    psramuhs_read(PSRAM_ID_0, PSRAMPHY_REG_05, &val);
#if (PSRAMUHS_SPEED <= 533)
    val &= ~ANA0_REG_PSRAM_TX_RD_CK_SEL;
#endif
    val = SET_BITFIELD(val, ANA0_REG_PSRAM_PULLUP_DQS, 0x1);
    val = SET_BITFIELD(val, ANA0_REG_PSRAM_PULLDN_DQS, 0x1);
#ifdef PSRAMULS_MODE
    val = SET_BITFIELD(val, ANA0_REG_PSRAM_ODT, 0x0);
    val = SET_BITFIELD(val, ANA0_REG_PSRAM_RX_BIAS, 0x2);
    val &= ~ANA0_REG_PSRAM_PULLUP;
    val &= ~ANA0_REG_PSRAM_PULLDN;
    val &= ~ANA0_REG_PSRAM_TX_WR_CK_SEL;
    val = SET_BITFIELD(val, ANA0_REG_PSRAM_TXDRV_CKE, 0x4);
#else
    val = SET_BITFIELD(val, ANA0_REG_PSRAM_ODT, 0x1);
    val = SET_BITFIELD(val, ANA0_REG_PSRAM_RX_BIAS, 0x3);
#endif
    psramuhs_write(PSRAM_ID_0, PSRAMPHY_REG_05, val);

    psramuhs_write(PSRAM_ID_0, PSRAMPHY_REG_14, 0xFF00);

    val = ANA0_REG_LDO_DVDD_PU | ANA0_REG_AVDDQ_LDO_PU |
#ifndef PSRAMULS_MODE
        ANA0_REG_VL_LDO_PU | ANA0_REG_VH_LDO_PU |
#endif
        ANA0_REG_LDO_AVDDQ_PRECHARGE | ANA0_REG_LDO_DVDD_PRECHARGE | ANA0_REG_VH_PRECHARGE;
    psramuhs_write(PSRAM_ID_0, PSRAMPHY_REG_01, val);
    hal_sys_timer_delay_us(15);

    val &= ~(ANA0_REG_LDO_AVDDQ_PRECHARGE | ANA0_REG_LDO_DVDD_PRECHARGE | ANA0_REG_VH_PRECHARGE);
    val |= ANA0_REG_VL_PU;
    psramuhs_write(PSRAM_ID_0, PSRAMPHY_REG_01, val);
    hal_sys_timer_delay_us(1);

    val |= ANA0_REG_DVDD_PU;
    psramuhs_write(PSRAM_ID_0, PSRAMPHY_REG_01, val);
    hal_sys_timer_delay_us(1);

    val |= ANA0_REG_AVDDQ_PU;
    psramuhs_write(PSRAM_ID_0, PSRAMPHY_REG_01, val);
    hal_sys_timer_delay_us(1);

    val |= ANA0_REG_VH_PU;
    psramuhs_write(PSRAM_ID_0, PSRAMPHY_REG_01, val);
    hal_sys_timer_delay_us(1);

    val |= ANA0_REG_DLL_RESETB | ANA0_REG_PSRAM_PUOK | ANA0_REG_DLL_CLK_RDY |
        ANA0_REG_PSRAM_DIE_PU1P3 | ANA0_REG_PSRAM_DIE_PU1P8;
    psramuhs_write(PSRAM_ID_0, PSRAMPHY_REG_01, val);
    hal_sys_timer_delay_ns(160);

    //psramuhs_read(PSRAM_ID_0, PSRAMPHY_REG_05, &val);
    //val &= ~OENB;
    //psramuhs_write(PSRAM_ID_0, PSRAMPHY_REG_05, val);
}

void psramuhsphy_init_calib(void)
{
    POSSIBLY_UNUSED uint16_t val, dly_in, rx_dqs, tx_clk, tx_ceb, tx_dm, tx_dq, tx_cmd, tx_dqs, rx_adq;
    uint16_t dll_state;

    do {
        psramuhs_read(PSRAM_ID_0, PSRAMPHY_REG_03, &dll_state);
    } while ((dll_state & ANA0_DLL_LOCK) == 0);

    dly_in = GET_BITFIELD(dll_state, ANA0_DLL_DLY_IN);
    //ASSERT(dly_in < (DLL_DLY_IN_MASK >> DLL_DLY_IN_SHIFT), "[%d] Bad DLL_DLY_IN=0x%X reg=0x%04X", i, dly_in, dll_state[i]);

#ifdef PSRAMULS_MODE
#ifdef NO_CALIB
    tx_clk = 4;
    tx_ceb = 4;
    tx_dm = 0;
    tx_dq = 0;
    tx_cmd = 0;
    tx_dqs = 4;
    rx_adq = 0;
    rx_dqs = 4;
#else
    tx_clk = 10;
    tx_ceb = 0;
    tx_dm = 30;
    tx_dq = 30;
    tx_cmd = 0;
    tx_dqs = 0;
    rx_adq = 0;
    rx_dqs = 0;
#endif
#else
    tx_clk = dly_in / 2;
    tx_ceb = dly_in / 2;
    tx_dm = 0;
    tx_dq = 0;
    tx_cmd = dly_in / 2;
    tx_dqs = dly_in / 2;
    rx_adq = 0;
    rx_dqs = dly_in / 2;
#endif

    PSRAMUHS_TRACE(1, "init cali :T/4:0x%x", dly_in / 2);
    val = ANA0_REG_PSRAM_TX_DM_DLY(tx_dm) | ANA0_REG_PSRAM_TX_DQ_DLY_4_0(tx_dq) | ANA0_REG_PSRAM_TX_DQ_DLY_9_5(tx_dq);
    psramuhs_write(PSRAM_ID_0, PSRAMPHY_REG_06, val);
    val = ANA0_REG_PSRAM_TX_DQ_DLY_14_10(tx_dq) | ANA0_REG_PSRAM_TX_DQ_DLY_19_15(tx_dq) | ANA0_REG_PSRAM_TX_DQ_DLY_24_20(tx_dq);
    psramuhs_write(PSRAM_ID_0, PSRAMPHY_REG_07, val);
    val = ANA0_REG_PSRAM_TX_DQ_DLY_29_25(tx_dq) | ANA0_REG_PSRAM_TX_DQ_DLY_34_30(tx_dq) | ANA0_REG_PSRAM_TX_DQ_DLY_39_35(tx_dq);
    psramuhs_write(PSRAM_ID_0, PSRAMPHY_REG_08, val);
    val = ANA0_REG_PSRAM_TX_CMD_DLY_4_0(tx_cmd) | ANA0_REG_PSRAM_TX_CMD_DLY_9_5(tx_cmd) | ANA0_REG_PSRAM_TX_CMD_DLY_14_10(tx_cmd);
    psramuhs_write(PSRAM_ID_0, PSRAMPHY_REG_09, val);
    val = ANA0_REG_PSRAM_TX_CMD_DLY_19_15(tx_cmd) | ANA0_REG_PSRAM_TX_CMD_DLY_24_20(tx_cmd) | ANA0_REG_PSRAM_TX_CMD_DLY_29_25(tx_cmd);
    psramuhs_write(PSRAM_ID_0, PSRAMPHY_REG_0A, val);
    val = ANA0_REG_PSRAM_TX_CMD_DLY_34_30(tx_cmd) | ANA0_REG_PSRAM_TX_CMD_DLY_39_35(tx_cmd);
    psramuhs_write(PSRAM_ID_0, PSRAMPHY_REG_0B, val);
    val = ANA0_REG_PSRAM_TX_DQS_DLY(tx_dqs) | ANA0_REG_PSRAM_RX_ADQ_DLY_4_0(rx_adq) | ANA0_REG_PSRAM_RX_ADQ_DLY_9_5(rx_adq);
    psramuhs_write(PSRAM_ID_0, PSRAMPHY_REG_0C, val);
    val = ANA0_REG_PSRAM_RX_ADQ_DLY_14_10(rx_adq) | ANA0_REG_PSRAM_RX_ADQ_DLY_19_15(rx_adq) | ANA0_REG_PSRAM_RX_ADQ_DLY_24_20(rx_adq);
    psramuhs_write(PSRAM_ID_0, PSRAMPHY_REG_0D, val);
    val = ANA0_REG_PSRAM_RX_ADQ_DLY_29_25(rx_adq) | ANA0_REG_PSRAM_RX_ADQ_DLY_34_30(rx_adq) | ANA0_REG_PSRAM_RX_ADQ_DLY_39_35(rx_adq);
    psramuhs_write(PSRAM_ID_0, PSRAMPHY_REG_0E, val);
    val = ANA0_REG_PSRAM_RX_DQS_DLY(rx_dqs) | ANA0_REG_PSRAM_TX_CLK_DLY(tx_clk) | ANA0_REG_PSRAM_TX_CEB_DLY(tx_ceb);
    psramuhs_write(PSRAM_ID_0, PSRAMPHY_REG_0F, val);
}

static bool psramuhsphy_check_read_valid(enum PSRAM_ID_T id)
{
    uint32_t mr32, mr40;
    psramuhs_read_reg(32, &mr32);//decimalism 32
    psramuhs_read_reg(40, &mr40);//decimalism 40

    PSRAMUHS_TRACE(3, "%s, mr32:0x%x, mr40:0x%x", __FUNCTION__, mr32, mr40);

    if (mr32 == 0x00ff00ff && mr40 == 0xffff0000)
        return true;
    return false;
}

static uint32_t g_rx_adq, g_rx_dqs_l, g_rx_dqs_r, g_tx_dqs_l, g_tx_dqs_r;
void psramuhsphy_printf_window()
{
    uint32_t rx_dqs, rx_adq;
    rx_adq = g_rx_adq;
    rx_dqs = (g_rx_dqs_l+g_rx_dqs_r)/2;
    if (rx_dqs > rx_adq) {
        rx_dqs = rx_dqs - rx_adq;
        rx_adq = 0;
    } else {
        rx_adq = rx_adq - rx_dqs;
        rx_dqs = 0;
    }
    TRACE(0, "psramuhs rx_adq:0x%x, rx_dqs(0x%x-0x%x)", g_rx_adq, g_rx_dqs_l, g_rx_dqs_r);
    TRACE(0, "psramuhs rx_dqs:0x%x, rx_adq:0x%x", rx_dqs, rx_adq);
    TRACE(0, "psramuhs tx_dqs:0x%x(0x%x-0x%x)", (g_tx_dqs_l + g_tx_dqs_r) / 2, g_tx_dqs_l, g_tx_dqs_r);
}
static void psramuhsphy_read_calib_bank(enum PSRAM_ID_T id, uint16_t range)
{
    uint16_t val_01, val, dly_in, rx_dqs, tx_clk, tx_ceb, tx_dm, tx_dq, tx_cmd, tx_dqs, rx_adq;
    uint16_t dll_state;
    static uint16_t last_range = 0;

    PSRAMUHS_TRACE(0, "\n####\n");
    ASSERT(range <= (ANA0_REG_PSRAM_DLL_RANGE_MASK >> ANA0_REG_PSRAM_DLL_RANGE_SHIFT), "ERROR, bad ana phy range:%d", range);

    if (last_range != range) {
        last_range = range;
        psramuhs_read(id, PSRAMPHY_REG_01, &val_01);
        val_01 &= ~(ANA0_REG_DLL_RESETB | ANA0_REG_DLL_CLK_RDY);
        psramuhs_write(id, PSRAMPHY_REG_01, val_01);

        psramuhsphy_set_range(range);

        val_01 |= (ANA0_REG_DLL_RESETB | ANA0_REG_DLL_CLK_RDY);
        psramuhs_write(id, PSRAMPHY_REG_01, val_01);
    }

    do {
        psramuhs_read(id, PSRAMPHY_REG_03, &dll_state);
    } while ((dll_state & ANA0_DLL_LOCK) == 0);

    dly_in = GET_BITFIELD(dll_state, ANA0_DLL_DLY_IN);
    if (dly_in == (ANA0_DLL_DLY_IN_MASK >> ANA0_DLL_DLY_IN_SHIFT)) {
        PSRAMUHS_TRACE(3, "%s, range:0x%x,  dly_in = 0x%x", __FUNCTION__, range, dly_in);
        return psramuhsphy_read_calib_bank(id, range + 1);
    }

#ifdef PSRAMULS_MODE
    tx_clk = 10;
    tx_ceb = 0;
    tx_dm = 30;
    tx_dq = 30;
    tx_cmd = 0;
    tx_dqs = 0;
    rx_adq = 0;
    rx_dqs = 0;
#else
    tx_clk = dly_in / 2;
    tx_ceb = dly_in / 2;
    tx_dm = 0;
    tx_dq = 0;
    tx_cmd = dly_in / 2;
    tx_dqs = dly_in / 2;
    rx_adq = 0;
#endif

    PSRAMUHS_TRACE(3, "%s, range:0x%x,  T/4 = 0x%x", __FUNCTION__, range, dly_in / 2);

#ifdef PSRAMUHS_WINDOW_TEST
    TRACE_IMM(TR_ATTR_NO_LF | TR_ATTR_NO_TS | TR_ATTR_NO_ID, ", T/4=0x%02x", dly_in / 2);
    static int8_t last_left_bound = 0;
    static int8_t last_right_bound = 0;
#endif

    int8_t left_bound;
    int8_t right_bound;
    bool valid_array[(ANA0_REG_PSRAM_RX_DQS_DLY_MASK >> ANA0_REG_PSRAM_RX_DQS_DLY_SHIFT) + 1];
re_cali:

#ifdef PSRAMULS_MODE
    left_bound = -1;
    right_bound = -1;
    memset(valid_array, 0, sizeof(valid_array));
    for (tx_clk = 0; tx_clk <= ANA0_REG_PSRAM_TX_CLK_DLY_MASK >> ANA0_REG_PSRAM_TX_CLK_DLY_SHIFT; tx_clk++) {
        uint32_t toggling;
        val = ANA0_REG_PSRAM_TX_CLK_DLY(tx_clk) | ANA0_REG_PSRAM_TX_CEB_DLY(tx_ceb);
        psramuhs_write(id, PSRAMPHY_REG_0F, val);

        toggling = hal_psramuhs_ca_calib_result();
        PSRAMUHS_TRACE(3, "%s, tx_clk:0x%x, toggling:%d", __FUNCTION__, tx_clk, toggling);

        valid_array[tx_clk] = toggling;
        if (tx_clk == 0) {
            if (toggling) {
                left_bound = tx_clk;//find left bound
                PSRAMUHS_TRACE(1, "left_bound:0x%x", left_bound);
            }
        } else if (tx_clk == ANA0_REG_PSRAM_TX_CLK_DLY_MASK >> ANA0_REG_PSRAM_TX_CLK_DLY_SHIFT) {
            if (toggling) {
                right_bound = tx_clk;//find right bound
                PSRAMUHS_TRACE(1, "right_bound:0x%x", right_bound);
            } else {
                if (valid_array[tx_clk - 1]) {
                    right_bound = tx_clk - 1; //find right bound
                    PSRAMUHS_TRACE(1, "right_bound:0x%x", right_bound);
                }
            }
        } else if (toggling && !valid_array[tx_clk - 1]) {
            left_bound = tx_clk;//find left bound
            PSRAMUHS_TRACE(1, "left_bound:0x%x", left_bound);
        } else if (!toggling && valid_array[tx_clk - 1]) {
            right_bound = tx_clk - 1; //find right bound
            PSRAMUHS_TRACE(1, "right_bound:0x%x", right_bound);
            break;
        }
    }
    if (left_bound != -1 && right_bound != -1) {
        tx_clk = (left_bound + right_bound) / 2;
        PSRAMUHS_TRACE(4, "ca training done, tx_clk:0x%x(0x%x-0x%x), range:0x%x", tx_clk, left_bound, right_bound, range);
        val = ANA0_REG_PSRAM_TX_CLK_DLY(tx_clk) | ANA0_REG_PSRAM_TX_CEB_DLY(tx_ceb);
        psramuhs_write(id, PSRAMPHY_REG_0F, val);
    } else {
        ///TODO:
        ASSERT(false, "ERROR, ca training fail");
    }
#endif

    val = ANA0_REG_PSRAM_TX_DM_DLY(tx_dm) | ANA0_REG_PSRAM_TX_DQ_DLY_4_0(tx_dq) | ANA0_REG_PSRAM_TX_DQ_DLY_9_5(tx_dq);
    psramuhs_write(id, PSRAMPHY_REG_06, val);
    val = ANA0_REG_PSRAM_TX_DQ_DLY_14_10(tx_dq) | ANA0_REG_PSRAM_TX_DQ_DLY_19_15(tx_dq) | ANA0_REG_PSRAM_TX_DQ_DLY_24_20(tx_dq);
    psramuhs_write(id, PSRAMPHY_REG_07, val);
    val = ANA0_REG_PSRAM_TX_DQ_DLY_29_25(tx_dq) | ANA0_REG_PSRAM_TX_DQ_DLY_34_30(tx_dq) | ANA0_REG_PSRAM_TX_DQ_DLY_39_35(tx_dq);
    psramuhs_write(id, PSRAMPHY_REG_08, val);
    val = ANA0_REG_PSRAM_TX_CMD_DLY_4_0(tx_cmd) | ANA0_REG_PSRAM_TX_CMD_DLY_9_5(tx_cmd) | ANA0_REG_PSRAM_TX_CMD_DLY_14_10(tx_cmd);
    psramuhs_write(id, PSRAMPHY_REG_09, val);
    val = ANA0_REG_PSRAM_TX_CMD_DLY_19_15(tx_cmd) | ANA0_REG_PSRAM_TX_CMD_DLY_24_20(tx_cmd) | ANA0_REG_PSRAM_TX_CMD_DLY_29_25(tx_cmd);
    psramuhs_write(id, PSRAMPHY_REG_0A, val);
    val = ANA0_REG_PSRAM_TX_CMD_DLY_34_30(tx_cmd) | ANA0_REG_PSRAM_TX_CMD_DLY_39_35(tx_cmd);
    psramuhs_write(id, PSRAMPHY_REG_0B, val);
    val = ANA0_REG_PSRAM_TX_DQS_DLY(tx_dqs) | ANA0_REG_PSRAM_RX_ADQ_DLY_4_0(rx_adq) | ANA0_REG_PSRAM_RX_ADQ_DLY_9_5(rx_adq);
    psramuhs_write(id, PSRAMPHY_REG_0C, val);
    val = ANA0_REG_PSRAM_RX_ADQ_DLY_14_10(rx_adq) | ANA0_REG_PSRAM_RX_ADQ_DLY_19_15(rx_adq) | ANA0_REG_PSRAM_RX_ADQ_DLY_24_20(rx_adq);
    psramuhs_write(id, PSRAMPHY_REG_0D, val);
    val = ANA0_REG_PSRAM_RX_ADQ_DLY_29_25(rx_adq) | ANA0_REG_PSRAM_RX_ADQ_DLY_34_30(rx_adq) | ANA0_REG_PSRAM_RX_ADQ_DLY_39_35(rx_adq);
    psramuhs_write(id, PSRAMPHY_REG_0E, val);
    val = ANA0_REG_PSRAM_TX_CLK_DLY(tx_clk) | ANA0_REG_PSRAM_TX_CEB_DLY(tx_ceb);
    psramuhs_write(id, PSRAMPHY_REG_0F, val);

    left_bound = -1;
    right_bound = -1;
    memset(valid_array, 0, sizeof(valid_array));
#ifdef PSRAMULS_MODE
    for (rx_dqs = 0; rx_dqs <= (ANA0_REG_PSRAM_RX_DQS_DLY_MASK >> ANA0_REG_PSRAM_RX_DQS_DLY_SHIFT); ++rx_dqs)
#else
    for (rx_dqs = 0; rx_dqs <= dly_in && rx_dqs <= (ANA0_REG_PSRAM_RX_DQS_DLY_MASK >> ANA0_REG_PSRAM_RX_DQS_DLY_SHIFT); ++rx_dqs)
#endif
    {
        bool valid;
        val = SET_BITFIELD(val, ANA0_REG_PSRAM_RX_DQS_DLY, rx_dqs);
        psramuhs_write(id, PSRAMPHY_REG_0F, val);
        valid = psramuhsphy_check_read_valid(id);
        PSRAMUHS_TRACE(3, "%s, rx_dqs:0x%x, valid:%d", __FUNCTION__, rx_dqs, valid);
        valid_array[rx_dqs] = valid;
        if (rx_dqs == 0) {
            if (valid) {
                if (rx_adq <= dly_in) {
                    rx_adq ++;
                    PSRAMUHS_TRACE(1, "left boud is valid, increase rx_adq(0x%x)", rx_adq);
                    goto re_cali;
                } else {
                    left_bound = rx_dqs;
                    PSRAMUHS_TRACE(1, "left_bound:0x%x", left_bound);
                }
                //return psramuhsphy_read_calib_bank(id, range+1);
            }
#ifndef PSRAMULS_MODE
        } else if (rx_dqs == dly_in) {
            if (valid) {
                right_bound = rx_dqs;//find right bound
                PSRAMUHS_TRACE(1, "right_bound:0x%x", right_bound);
            } else {
                if (valid_array[rx_dqs - 1]) {
                    right_bound = rx_dqs - 1; //find right bound
                    PSRAMUHS_TRACE(1, "right_bound:0x%x", right_bound);
                }
            }
#endif
        } else if (rx_dqs == ANA0_REG_PSRAM_RX_DQS_DLY_MASK >> ANA0_REG_PSRAM_RX_DQS_DLY_SHIFT) {
            if (valid) {
                if (range < (ANA0_REG_PSRAM_DLL_RANGE_MASK >> ANA0_REG_PSRAM_DLL_RANGE_SHIFT)) {
                    return psramuhsphy_read_calib_bank(id, range + 1);
                } else {
                    right_bound = rx_dqs;//find right bound
                    PSRAMUHS_TRACE(1, "right_bound:0x%x", right_bound);
                }
            } else {
                if (valid_array[rx_dqs - 1]) {
                    right_bound = rx_dqs - 1; //find right bound
                    PSRAMUHS_TRACE(1, "right_bound:0x%x", right_bound);
                }
            }
        } else if (valid && !valid_array[rx_dqs - 1]) {
            left_bound = rx_dqs;//find left bound
            PSRAMUHS_TRACE(1, "left_bound:0x%x", left_bound);
        } else if (!valid && valid_array[rx_dqs - 1]) {
            right_bound = rx_dqs - 1; //find right bound
            PSRAMUHS_TRACE(1, "right_bound:0x%x", right_bound);
            break;
        }
    }
    if (left_bound != -1 && right_bound != -1 && (right_bound - left_bound > 1)) {
        g_rx_adq = rx_adq;
        g_rx_dqs_l = left_bound;
        g_rx_dqs_r = right_bound;
        rx_dqs = (left_bound + right_bound) / 2;
        PSRAMUHS_TRACE(4, "cali done, rx_dqs:0x%x(0x%x-0x%x), range:0x%x", rx_dqs, left_bound, right_bound, range);
#ifdef PSRAMUHS_WINDOW_TEST
        if ((last_left_bound != left_bound) || (last_right_bound != right_bound)) {
            last_left_bound = left_bound;
            last_right_bound = right_bound;
            TRACE_IMM(TR_ATTR_NO_LF | TR_ATTR_NO_TS | TR_ATTR_NO_ID, ", rx_dqs:0x%02x(0x%02x-0x%02x)", rx_dqs, left_bound, right_bound);
        } else {
            TRACE_IMM(TR_ATTR_NO_LF | TR_ATTR_NO_TS | TR_ATTR_NO_ID, ", **********************");
        }
#endif
        if (rx_dqs > rx_adq) {
            rx_dqs = rx_dqs - rx_adq;
            rx_adq = 0;
        } else {
            rx_adq = rx_adq - rx_dqs;
            rx_dqs = 0;
        }
        PSRAMUHS_TRACE(2, "cali done, rx_dqs:0x%x, rx_adq:0x%x", rx_dqs, rx_adq);
        val = SET_BITFIELD(val, ANA0_REG_PSRAM_RX_DQS_DLY, rx_dqs);
        psramuhs_write(id, PSRAMPHY_REG_0F, val);
        val = ANA0_REG_PSRAM_TX_DQS_DLY(tx_dqs) | ANA0_REG_PSRAM_RX_ADQ_DLY_4_0(rx_adq) | ANA0_REG_PSRAM_RX_ADQ_DLY_9_5(rx_adq);
        psramuhs_write(id, PSRAMPHY_REG_0C, val);
        val = ANA0_REG_PSRAM_RX_ADQ_DLY_14_10(rx_adq) | ANA0_REG_PSRAM_RX_ADQ_DLY_19_15(rx_adq) | ANA0_REG_PSRAM_RX_ADQ_DLY_24_20(rx_adq);
        psramuhs_write(id, PSRAMPHY_REG_0D, val);
        val = ANA0_REG_PSRAM_RX_ADQ_DLY_29_25(rx_adq) | ANA0_REG_PSRAM_RX_ADQ_DLY_34_30(rx_adq) | ANA0_REG_PSRAM_RX_ADQ_DLY_39_35(rx_adq);
        psramuhs_write(id, PSRAMPHY_REG_0E, val);
    } else {
        ///TODO:
        ASSERT(false, "ERROR, read need do more cali");
    }
}

#ifdef NO_CALIB
POSSIBLY_UNUSED
#endif
static void psramuhsphy_read_calib(uint32_t clk)
{
    uint16_t range;

    if (clk <= 300000000 / 2) {
        range = 3;
    } else if (clk <= 500000000 / 2) {
        range = 2;
    } else if (clk <= 1066000000 / 2) {
        range = 1;
    } else {
        range = 0;
    }
    PSRAMUHS_TRACE(3, "%s, clk:%d, range:%d", __FUNCTION__, clk, range);
#ifdef PSRAMUHS_WINDOW_TEST
    TRACE_IMM(TR_ATTR_NO_LF, "clk:%d, range:%d", clk, range);
#endif
    psramuhsphy_read_calib_bank(PSRAM_ID_0, range);
}

static bool psramuhsphy_check_write_valid(enum PSRAM_ID_T id)
{
    uint32_t i;
    uint32_t base;
    uint32_t val, val0, val1;
    uint32_t val2, val3, val4, val5;
    volatile uint32_t *psram_base = (volatile uint32_t *)PSRAMUHS_BASE;
    volatile uint32_t *psram_base1 = psram_base + 0x40000;//offset 1MB
    for (base=0; base<0x10; ++base) {
        val = base & 0xF;
        val = val | (val<<4);
        val = val | (val << 8) | (val << 16) | (val << 24);
        val0 = (val & 0x00FF00FF) | ((~val) & 0xFF00FF00);
        val1 = (val & 0xFF0000FF) | ((~val) & 0x00FFFF00);

        for (i=0; i<0x32; ++i) {
            *(psram_base+2*i) = val0;
            *(psram_base+2*i+1) = 0;
            *(psram_base1+2*i) = val1;
            *(psram_base1+2*i+1) = 0;
        }

        hal_psramuhsip_wb_busy_wait();
        hal_psramuhsip_mc_busy_wait();
        hal_cache_invalidate_all(HAL_CACHE_ID_D_CACHE);
        hal_cache_invalidate_all(HAL_CACHE_ID_I_CACHE);

        for (i=0; i<0x32; ++i) {
            val2 = *(psram_base+2*i);
            val3 = *(psram_base+2*i+1);
            val4 = *(psram_base1+2*i);
            val5 = *(psram_base1+2*i+1);
            if (val2 != val0){
                //PSRAMUHS_TRACE(3, "%s, i:%d, 0x%x, 0x%x", __FUNCTION__, i, val2, val0);
                return false;
            }
            if (val3 != 0){
                //PSRAMUHS_TRACE(3, "%s, i:%d, 0x%x, 0x%x", __FUNCTION__, i, val3, 0);
                return false;
            }
            if (val4 != val1){
                //PSRAMUHS_TRACE(3, "%s, i:%d, 0x%x, 0x%x", __FUNCTION__, i, val4, val1);
                return false;
            }
            if (val5 != 0){
                //PSRAMUHS_TRACE(3, "%s, i:%d, 0x%x, 0x%x", __FUNCTION__, i, val5, 0);
                return false;
            }
        }
    }
    return true;
}

static void psramuhsphy_write_calib_bank(enum PSRAM_ID_T id)
{
    uint16_t val, val_0f;
    POSSIBLY_UNUSED uint16_t dly_in, tx_dqs, tx_ceb, tx_clk;

    psramuhs_read(id, PSRAMPHY_REG_03, &val);
    dly_in = GET_BITFIELD(val, ANA0_DLL_DLY_IN);
    ASSERT(dly_in < (ANA0_DLL_DLY_IN_MASK >> ANA0_DLL_DLY_IN_SHIFT), "[%d] Bad DLL_DLY_IN=0x%X reg=0x%04X", id, dly_in, val);

    PSRAMUHS_TRACE(2, "%s, T/4 = 0x%x", __FUNCTION__, dly_in / 2);

    psramuhs_read(id, PSRAMPHY_REG_0C, &val);

    psramuhs_read(id, PSRAMPHY_REG_0F, &val_0f);
    tx_clk = GET_BITFIELD(val_0f, ANA0_REG_PSRAM_TX_CLK_DLY);
    tx_ceb = GET_BITFIELD(val_0f, ANA0_REG_PSRAM_TX_CEB_DLY);

    int8_t left_bound = -1;
    int8_t right_bound = -1;
#ifdef PSRAMUHS_WINDOW_TEST
    static int8_t last_left_bound = 0;
    static int8_t last_right_bound = 0;
#endif
    bool valid_array[(ANA0_REG_PSRAM_TX_DQS_DLY_MASK >> ANA0_REG_PSRAM_TX_DQS_DLY_SHIFT) + 1];
    left_bound = -1;
    right_bound = -1;
    memset(valid_array, 0, sizeof(valid_array));
#ifdef PSRAMULS_MODE
    for (tx_dqs = 0; tx_dqs <= (ANA0_REG_PSRAM_TX_DQS_DLY_MASK >> ANA0_REG_PSRAM_TX_DQS_DLY_SHIFT); ++tx_dqs)
#else
    for (tx_dqs = 0; tx_dqs <= dly_in && tx_dqs <= (ANA0_REG_PSRAM_TX_DQS_DLY_MASK >> ANA0_REG_PSRAM_TX_DQS_DLY_SHIFT); ++tx_dqs)
#endif
    {
        bool valid;

        val = SET_BITFIELD(val, ANA0_REG_PSRAM_TX_DQS_DLY, tx_dqs);
        psramuhs_write(id, PSRAMPHY_REG_0C, val);

        valid = psramuhsphy_check_write_valid(id);
        PSRAMUHS_TRACE(3, "%s, tx_dqs:0x%x, valid:%d", __FUNCTION__, tx_dqs, valid);
        valid_array[tx_dqs] = valid;
        if (tx_dqs == 0) {
            if (valid) {
                left_bound = tx_dqs;
                PSRAMUHS_TRACE(1, "left_bound:0x%x", left_bound);
            }
#ifdef PSRAMULS_MODE
        } else if (tx_dqs == ANA0_REG_PSRAM_TX_DQS_DLY_MASK >> ANA0_REG_PSRAM_TX_DQS_DLY_SHIFT) {
#else
        } else if (tx_dqs == dly_in || tx_dqs == ANA0_REG_PSRAM_TX_DQS_DLY_MASK >> ANA0_REG_PSRAM_TX_DQS_DLY_SHIFT) {
#endif
            if (valid) {
                right_bound = tx_dqs;//find right bound
                PSRAMUHS_TRACE(1, "right_bound:0x%x", right_bound);
            } else {
                if (valid_array[tx_dqs - 1]) {
                    right_bound = tx_dqs - 1; //find right bound
                    PSRAMUHS_TRACE(1, "right_bound:0x%x", right_bound);
                }
            }
        } else {
            if (valid && !valid_array[tx_dqs - 1]) {
                left_bound = tx_dqs;//find left bound
                PSRAMUHS_TRACE(1, "left_bound:0x%x", left_bound);
            } else if (!valid && valid_array[tx_dqs - 1]) {
                right_bound = tx_dqs - 1; //find right bound
                PSRAMUHS_TRACE(1, "right_bound:0x%x", right_bound);
                if (right_bound - left_bound > 2)
                    break;
            }
        }
    }
    if (left_bound != -1 && right_bound != -1 && (right_bound - left_bound > 1)) {
        g_tx_dqs_l = left_bound;
        g_tx_dqs_r = right_bound;
        tx_dqs = (left_bound + right_bound) / 2;
        val = SET_BITFIELD(val, ANA0_REG_PSRAM_TX_DQS_DLY, tx_dqs);
        psramuhs_write(id, PSRAMPHY_REG_0C, val);
        PSRAMUHS_TRACE(3, "cali done, tx_dqs:0x%x(0x%x-0x%x)", tx_dqs, left_bound, right_bound);
#ifdef PSRAMUHS_WINDOW_TEST
        if ((last_left_bound != left_bound) || (last_right_bound != right_bound)) {
            last_left_bound = left_bound;
            last_right_bound = right_bound;
            TRACE_IMM(TR_ATTR_NO_TS | TR_ATTR_NO_ID, ", tx_dqs:0x%02x(0x%02x-0x%02x)", tx_dqs, left_bound, right_bound);
        } else {
            TRACE_IMM(TR_ATTR_NO_TS | TR_ATTR_NO_ID, ", ----------------------");
        }
#endif
    } else {
        ///TODO:, range+=1, recalib read and write
        ASSERT(false, "ERROR, write need do more cali");
    }
}

#ifdef NO_CALIB
POSSIBLY_UNUSED
#endif
static void psramuhsphy_write_calib()
{
    PSRAMUHS_TRACE(1, "%s", __FUNCTION__);
    psramuhsphy_write_calib_bank(PSRAM_ID_0);
}
void psramuhsphy_calib(uint32_t clk)
{
#ifndef NO_CALIB
    psramuhsphy_read_calib(clk);
    psramuhsphy_write_calib();
#endif
    return;
}
void psramuhsphy_close(void)
{
    hal_phyif_close(SPIPHY_PSRAMUHS0_CS);
}

void psramuhsphy_sleep(void)
{
    uint16_t val= 0;
#ifdef PSRAMULS_MODE
    val = 0;
#else
    val = ANA0_REG_VL_PU | ANA0_REG_AVDDQ_PU;
#endif
    psramuhs_write(PSRAM_ID_0, PSRAMPHY_REG_01, val);
}

void psramuhsphy_wakeup(void)
{
    uint16_t val;
    uint16_t dll_state;

    val = ANA0_REG_LDO_AVDDQ_PRECHARGE | ANA0_REG_LDO_DVDD_PRECHARGE | ANA0_REG_VH_PRECHARGE |
#ifndef PSRAMULS_MODE
        ANA0_REG_VL_LDO_PU | ANA0_REG_VH_LDO_PU | ANA0_REG_VL_PU | ANA0_REG_AVDDQ_PU |
#endif
        ANA0_REG_PSRAM_DIE_PU1P8 | ANA0_REG_PSRAM_DIE_PU1P3 |ANA0_REG_LDO_DVDD_PU | ANA0_REG_AVDDQ_LDO_PU;
    psramuhs_write(PSRAM_ID_0, PSRAMPHY_REG_01, val);
    hal_sys_timer_delay_us(15);

    val &= ~(ANA0_REG_LDO_AVDDQ_PRECHARGE | ANA0_REG_LDO_DVDD_PRECHARGE | ANA0_REG_VH_PRECHARGE);
#ifdef PSRAMULS_MODE
    val |= ANA0_REG_VL_PU;
#endif
    psramuhs_write(PSRAM_ID_0, PSRAMPHY_REG_01, val);
    hal_sys_timer_delay_us(1);

    val |= ANA0_REG_DVDD_PU;
    psramuhs_write(PSRAM_ID_0, PSRAMPHY_REG_01, val);
    hal_sys_timer_delay_us(1);

#ifdef PSRAMULS_MODE
    val |= ANA0_REG_AVDDQ_PU;
    psramuhs_write(PSRAM_ID_0, PSRAMPHY_REG_01, val);
    hal_sys_timer_delay_us(1);
#endif

    val |= ANA0_REG_VH_PU;
    psramuhs_write(PSRAM_ID_0, PSRAMPHY_REG_01, val);
    hal_sys_timer_delay_us(1);

    val |= ANA0_REG_DLL_RESETB | ANA0_REG_PSRAM_PUOK | ANA0_REG_DLL_CLK_RDY |
           ANA0_REG_PSRAM_DIE_PU1P3 | ANA0_REG_PSRAM_DIE_PU1P8;
    psramuhs_write(PSRAM_ID_0, PSRAMPHY_REG_01, val);

    hal_sys_timer_delay_ns(160);

    do {
        psramuhs_read(PSRAM_ID_0, PSRAMPHY_REG_03, &dll_state);
    } while ((dll_state & ANA0_DLL_LOCK) == 0);
    hal_sys_timer_delay(MS_TO_TICKS(10));
}

