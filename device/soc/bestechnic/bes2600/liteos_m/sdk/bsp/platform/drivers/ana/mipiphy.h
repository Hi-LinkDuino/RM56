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
#ifndef __MIPIPHY_H__
#define __MIPIPHY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_addr_map.h"
#include CHIP_SPECIFIC_HDR(mipiphy)
#include "hal_phyif.h"

#ifdef SPIPHY_MIPI_REG
#define SPI_MIPI_REG(reg)           SPIPHY_MIPI_REG(reg)
#else
#define SPI_MIPI_REG(reg)           (reg)
#endif

#define mipiphy_read(reg, val)      hal_phyif_reg_read(SPI_MIPI_REG(reg), val)
#define mipiphy_write(reg, val)     hal_phyif_reg_write(SPI_MIPI_REG(reg), val)

/* DSIPHY DRIVER */
void dsiphy_open(uint32_t clk);

void dsiphy_close(void);

void dsiphy_sleep(void);

void dsiphy_wakeup(void);

/* CSIPHY DRIVER */
enum CSI_PHY_DATA_CH_T {
    CSI_PHY_DATA_CH0 = (1 << 0),
    CSI_PHY_DATA_CH1 = (1 << 1),

    CSI_PHY_DATA_CH_QTY
};

enum CSI_PHY_CLK_RANGE_T {
    CSI_PHY_CLK_RANGE_1GHZ = 0,
    CSI_PHY_CLK_RANGE_500MHZ,
    CSI_PHY_CLK_RANGE_300MHZ,
    CSI_PHY_CLK_RANGE_200MHZ,
};

struct CSI_PHY_TIMING_PARAM_T {
    uint16_t clock_delay;
    uint16_t data_delay[2];
};

struct CSI_PHY_CONFIG_T {
    enum CSI_PHY_CLK_RANGE_T range;
    struct CSI_PHY_TIMING_PARAM_T param;
};

void csiphy_set_data_chan_delay(enum CSI_PHY_DATA_CH_T ch, uint16_t delay);

void csiphy_set_clock_delay(uint16_t delay);

void csiphy_set_clock_range(enum CSI_PHY_CLK_RANGE_T range);

void csiphy_apply_timing_param(struct CSI_PHY_TIMING_PARAM_T *param);

void csiphy_dump_regs(void);

void csiphy_open(struct CSI_PHY_CONFIG_T *cfg);

void csiphy_close(void);

void csiphy_sleep(void);

void csiphy_wakeup(void);

#ifdef __cplusplus
}
#endif

#endif

