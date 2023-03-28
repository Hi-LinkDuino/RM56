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
#ifndef __MIPIPHY_BEST2003_H__
#define __MIPIPHY_BEST2003_H__

#include "plat_types.h"

#define SPIPHY_MIPI_CS                  2

#define SPIPHY_MIPI_REG(reg)            (((reg) & 0xFFF) | (SPIPHY_MIPI_CS << 12))

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif

