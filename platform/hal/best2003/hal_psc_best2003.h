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
#ifndef __HAL_PSC_BEST2003_H__
#define __HAL_PSC_BEST2003_H__

#ifdef __cplusplus
extern "C" {
#endif

void  hal_psc_dslp_force_on_bt_enable(void);

void  hal_psc_dslp_force_on_bt_disable(void);

void hal_psc_wlan_enable(void);

void hal_psc_wlan_disable(void);

void hal_psc_a7_enable(void);

void hal_psc_a7_disable(void);

#ifdef __cplusplus
}
#endif

#endif

