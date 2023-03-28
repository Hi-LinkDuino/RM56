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
#include "hal_cmu.h"
#include "hal_phyif.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "pmu.h"
#include "mipiphy.h"
#include CHIP_SPECIFIC_HDR(reg_mipiphy)

#if !defined(NUTTX_BUILD)
#include "graphic_conf.h"
#endif

//#define MIPIPHY_DEBUG
#ifdef MIPIPHY_DEBUG
#define MIPIPHY_TRACE TRACE_IMM
#else
#define MIPIPHY_TRACE(...)
#endif

void dsiphy_open(uint32_t clk)
{
    uint16_t val;

    mipiphy_read(MIPIPHY_REG_0A, &val);
    val |= REG_CHCK_EN_DSI;
    val = SET_BITFIELD(val, REG_CHCK_DLY_TUNE_DSI, 0x4);
    val = SET_BITFIELD(val, REG_CHCK_HSTX_DRV_DSI, 0xF);
    val = SET_BITFIELD(val, REG_CHCK_LPTX_DRV_DSI, 0x4);
#if defined(CONFIG_SWAP_DSI_CLK_PN) || defined(WORDROUND_FOR_DSI_CLK_PN_SWAP)
    val |= REG_CHCK_SWAP_PN_DSI;
#endif
    mipiphy_write(MIPIPHY_REG_0A, val);

#if defined(CONFIG_SWAP_DSI_LANE0_PN)
    mipiphy_read(MIPIPHY_REG_08, &val);
    val |= REG_CH0_SWAP_PN_DSI;
    mipiphy_write(MIPIPHY_REG_08, val);
#endif

#if defined(CONFIG_SWAP_DSI_LANE1_PN)
    mipiphy_read(MIPIPHY_REG_09, &val);
    val |= REG_CH1_SWAP_PN_DSI;
    mipiphy_write(MIPIPHY_REG_09, val);
#endif
    val = REG_CLK_SWRC_DSI(0x1);
    val |= REG_PU_CH_DSI | REG_PU_LDO_DSI | REG_PRECHARGE_LDO_DSI | REG_CLKGEN_PU_DSI;
    mipiphy_write(MIPIPHY_REG_06, val);
    hal_sys_timer_delay(MS_TO_TICKS(1));

    val |= REG_EN_LPRX0_DSI;
    val &= ~REG_PRECHARGE_LDO_DSI;
    mipiphy_write(MIPIPHY_REG_06, val);

    val |= REG_DSI_PHY_RSTN;
    mipiphy_write(MIPIPHY_REG_06, val);
}

void dsiphy_sleep(void)
{
    uint16_t val;
    mipiphy_read(MIPIPHY_REG_06, &val);
    val &= ~(REG_CLKGEN_PU_DSI | REG_PU_CH_DSI | REG_PU_LDO_DSI);
    mipiphy_write(MIPIPHY_REG_06, val);
}

void dsiphy_wakeup(void)
{
    uint16_t val;
    mipiphy_read(MIPIPHY_REG_06, &val);
    val |= REG_PRECHARGE_LDO_DSI | REG_PU_LDO_DSI;
    mipiphy_write(MIPIPHY_REG_06, val);
    hal_sys_timer_delay_us(10);
    val &= ~REG_PRECHARGE_LDO_DSI;
    mipiphy_write(MIPIPHY_REG_06, val);
    val |= REG_PU_CH_DSI | REG_CLKGEN_PU_DSI;
    mipiphy_write(MIPIPHY_REG_06, val);
}

void csiphy_open(uint32_t clk)
{
    uint16_t val;

    mipiphy_read(MIPIPHY_REG_01, &val);
    val |= REG_PU_LDO_CSI | REG_PRECHARGE_CSI;
    //val = SET_BITFIELD(val, REG_RANGE_CSI, 0x3);
    mipiphy_write(MIPIPHY_REG_01, val);
    hal_sys_timer_delay(MS_TO_TICKS(1));

    val &= ~REG_PRECHARGE_CSI;
    mipiphy_write(MIPIPHY_REG_01, val);

    val |= REG_PU_CH_CSI | REG_LPRX_CTL_CSI;
    mipiphy_write(MIPIPHY_REG_01, val);

    mipiphy_read(MIPIPHY_REG_02, &val);
    val |= REG_CHCK_EN_CH_CSI;
    val = SET_BITFIELD(val, REG_CHCK_HSRX_DLY_CSI, 0x6);
    //val = SET_BITFIELD(val, REG_CHCK_HSRX_RTERM_CSI, 0x7);
    mipiphy_write(MIPIPHY_REG_02, val);

    mipiphy_read(MIPIPHY_REG_04, &val);
    val |= REG_CH0_EN_CH_CSI;
    //val = SET_BITFIELD(val, REG_CH0_HSRX_RTERM_CSI, 0x7);
    mipiphy_write(MIPIPHY_REG_04, val);

    mipiphy_read(MIPIPHY_REG_05, &val);
    val |= REG_CH1_EN_CH_CSI;
    //val = SET_BITFIELD(val, REG_CH1_HSRX_RTERM_CSI, 0x7);
    mipiphy_write(MIPIPHY_REG_05, val);
}

void csiphy_sleep(void)
{
}

void csiphy_wakeup(void)
{
}
