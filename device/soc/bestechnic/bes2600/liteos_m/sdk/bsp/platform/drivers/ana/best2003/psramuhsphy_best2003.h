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
#ifndef __PSRAMUHSPHY_BEST2003_H__
#define __PSRAMUHSPHY_BEST2003_H__

#include "plat_types.h"

#define SPIPHY_PSRAMUHS0_CS                 1

#define SPIPHY_PSRAMUHS0_REG(reg)           (((reg) & 0xFFF) | (SPIPHY_PSRAMUHS0_CS << 12))

#ifdef __cplusplus
extern "C" {
#endif

void psramuhs_read_reg(uint32_t reg, uint32_t *val);
void hal_psramuhsip_mc_busy_wait(void);
void hal_psramuhsip_wb_busy_wait(void);
void psramuhsphy_printf_window();

#ifdef __cplusplus
}
#endif

#endif

