/***************************************************************************
 *
 * Copyright 2015-2021 BES.
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

#ifndef __WIFI_POWER_CFG_BEST2003_H__
#define __WIFI_POWER_CFG_BEST2003_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "wifi_rf_test_def.h"

#if FORMULA_POWERLEVEL_OFFSET

extern int wifi_set_tpc_table_to_rf(void);

#endif //FORMULA_POWERLEVEL_OFFSET

#ifdef __cplusplus
}
#endif

#endif /* __WIFI_POWER_CFG_BEST2003_H__ */
