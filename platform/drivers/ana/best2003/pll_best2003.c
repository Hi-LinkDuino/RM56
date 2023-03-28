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
#include "hal_timer.h"
#include "hal_trace.h"
#include "pmu.h"
#include "hal_phyif.h"
#include "hal_location.h"
#include "mipiphy.h"

#include CHIP_SPECIFIC_HDR(reg_ddrpll)
#include CHIP_SPECIFIC_HDR(reg_dsppll)
#include CHIP_SPECIFIC_HDR(reg_bbpll)
#include CHIP_SPECIFIC_HDR(reg_dsipll)

#define PLL_MHZ 1000000

void ddr_pll_config()
{
#if 0
    uint64_t freq_mhz;
    uint8_t ddr_div;

    uint64_t PLL_cfg_val;
    uint16_t v[2];
    uint16_t val;
    uint32_t crystal_mhz = hal_cmu_get_crystal_freq() / PLL_MHZ;
#if (PSRAMUHS_SPEED == 350)
    freq_mhz = (uint64_t)2100;
    ddr_div = 3;
#elif (PSRAMUHS_SPEED == 400)
    freq_mhz = (uint64_t)2400;
    ddr_div = 3;
#elif (PSRAMUHS_SPEED == 600)
    freq_mhz = (uint64_t)2400;
    ddr_div = 2;
#elif (PSRAMUHS_SPEED == 800)
    freq_mhz = (uint64_t)1600;
    ddr_div = 0;
#elif (PSRAMUHS_SPEED == 900)
    freq_mhz = (uint64_t)1800;
    ddr_div = 0;
#elif (PSRAMUHS_SPEED == 1000)
    freq_mhz = (uint64_t)2000;
    ddr_div = 0;
#elif (PSRAMUHS_SPEED == 1066)
    freq_mhz = (uint64_t)2132;
    ddr_div = 0;
#else
#error "invalid PSRAMUHS_SPEED"
#endif

    if (crystal_mhz == 24) {
        rf_read(DDRPLL_REG_DA, &val);
        //val |= REG_DDRPLL_REFCLK_SEL;
        rf_write(DDRPLL_REG_DA, val);
    } else if (crystal_mhz == 40) {
        rf_read(DDRPLL_REG_DA, &val);
        //val |= REG_DDRPLL_REFCLK_SEL;
        rf_write(DDRPLL_REG_DA, val);
    } else {
        ASSERT(1, "ERROR unsupport crystal freq");
    }

    PLL_cfg_val = ((uint64_t)(1 << 25) * freq_mhz / 2 + crystal_mhz / 2) / crystal_mhz;
    v[0] = PLL_cfg_val & 0xFFFF;
    v[1] = (PLL_cfg_val >> 16) & 0xFFFF;

    rf_write(DDRPLL_REG_DD, v[0]);
    rf_write(DDRPLL_REG_DE, v[1]);

    rf_read(DDRPLL_REG_D6, &val);
    val = SET_BITFIELD(val, REG_DDRPLL_PDIV_PSRAM, ddr_div);
    val = SET_BITFIELD(val, REG_DDRPLL_PDIV_PSRAM2, ddr_div);
    val = SET_BITFIELD(val, REG_DDRPLL_PATH_EN, 0x3);
    rf_write(DDRPLL_REG_D6, val);
#endif
}

void dsp_pll_config(uint64_t freq)
{
#if 0
    int32_t ret;
    uint8_t ddr_div;
    uint32_t freq_mhz;
    uint64_t PLL_cfg_val;
    uint16_t v[2];
    uint16_t val;
    uint32_t crystal_mhz = hal_cmu_get_crystal_freq() / PLL_MHZ;
    uint8_t valid_div[14] = {2, 3, 4, 6, 8, 10, 12, 14, 6, 9, 12, 15, 18, 21};
    uint8_t i;

    freq_mhz = freq / PLL_MHZ;

    if (crystal_mhz == 24) {
        pmu_read(DSPPLL_REG_CC, &val);
        val = SET_BITFIELD(val, REG_A7PLL_REFCLK_SEL, 0x3);
        pmu_write(DSPPLL_REG_CC, val);
    } else if (crystal_mhz == 40) {
        pmu_read(DSPPLL_REG_CC, &val);
        val = SET_BITFIELD(val, REG_A7PLL_REFCLK_SEL, 0x1);
        pmu_write(DSPPLL_REG_CC, val);
    } else {
        ASSERT(1, "ERROR unsupport crystal freq");
    }

    for (i=0; i<ARRAY_SIZE(valid_div); ++i) {
        if (freq_mhz*valid_div[i]>=(uint64_t)1800 && freq_mhz*valid_div[i]<=(uint64_t)2700) {
            freq_mhz = freq_mhz*valid_div[i];
            ddr_div = valid_div[i];
            break;
        }
    }
    ASSERT(i < ARRAY_SIZE(valid_div), "Invalid freq input:%dMHz", freq_mhz);

    PLL_cfg_val = ((uint64_t)(1 << 25) * freq_mhz / 2 + crystal_mhz / 2) / crystal_mhz;
    v[0] = PLL_cfg_val & 0xFFFF;
    v[1] = (PLL_cfg_val >> 16) & 0xFFFF;

    pmu_write(DSPPLL_REG_CF, v[0]);
    pmu_write(DSPPLL_REG_D0, v[1]);

    //div =  div[0] ? (2+div[4]) : (2+div[4])*(div[3:1])
    if (ddr_div == 2) {
        ddr_div = 0x1;
    } else if (ddr_div == 3) {
        ddr_div = 0x1;
        ddr_div |= (1<<4);
    } else if (((ddr_div%2)==0) && (ddr_div>=2 && ddr_div<=14)) {
        ddr_div /= 2;
        ddr_div = ddr_div<<1;
    } else if (((ddr_div%3)==0) && (ddr_div>=3 && ddr_div<=21)) {
        ddr_div /= 3;
        ddr_div = ddr_div<<1;
        ddr_div |= (1<<4);
    } else {
        ASSERT(0, "invalid ddr_div:%d", ddr_div);
    }
    pmu_read(DSPPLL_REG_CD, &val);
    val = SET_BITFIELD(val, REG_A7PLL_POSTDIV, ddr_div);
    pmu_write(DSPPLL_REG_CD, val);
#endif
}

void dsi_pll_config(uint64_t freq)
{
    uint16_t val;
    uint32_t freq_hz;
    uint64_t PLL_cfg_val;
    uint16_t v[2];
    uint32_t crystal_hz = hal_cmu_get_crystal_freq();

    freq_hz = freq;

    PLL_cfg_val = ((uint64_t)(1 << 25) * freq_hz / 2 + crystal_hz / 2) / crystal_hz;
    v[0] = PLL_cfg_val & 0xFFFF;
    v[1] = (PLL_cfg_val >> 16) & 0xFFFF;

    mipiphy_read(DSIPLL_REG_0E, &val);
    val &= ~REG_MIPI_BBPLL_PU;
    mipiphy_write(DSIPLL_REG_0E, val);

    mipiphy_read(DSIPLL_REG_1A, &val);
    val |= REG_MIPI_BBPLL_PU_DR;
    mipiphy_write(DSIPLL_REG_1A, val);

    mipiphy_write(DSIPLL_REG_0F, v[0]);
    mipiphy_write(DSIPLL_REG_10, v[1]);

    val &= ~REG_MIPI_BBPLL_PU_DR;
    mipiphy_write(DSIPLL_REG_1A, val);
}

void bb_pll_config(uint64_t freq)
{
    uint32_t freq_mhz;
    uint64_t PLL_cfg_val;
    uint16_t v[2];
    uint32_t crystal_mhz = hal_cmu_get_crystal_freq() / PLL_MHZ;

    freq_mhz = freq / PLL_MHZ;

    PLL_cfg_val = ((uint64_t)(1 << 25) * freq_mhz + crystal_mhz / 2) / crystal_mhz;
    v[0] = PLL_cfg_val & 0xFFFF;
    v[1] = (PLL_cfg_val >> 16) & 0xFFFF;

    wfrf_write(BBPLL_REG_BE, v[0]);
    wfrf_write(BBPLL_REG_BF, v[1]);
}

