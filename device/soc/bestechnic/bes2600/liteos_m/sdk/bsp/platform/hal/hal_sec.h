/***************************************************************************
 *
 * Copyright 2015-2020 BES.
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
#ifndef __HAL_SEC_H__
#define __HAL_SEC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"
#include CHIP_SPECIFIC_HDR(hal_sec)

void hal_sec_set_gdma_nonsec(bool nonsec);
void hal_sec_set_adma_nonsec(bool nonsec);
void hal_sec_set_bcm_nonsec(bool nonsec);
void hal_sec_set_usb_nonsec(bool nonsec);
void hal_sec_set_i2c_slv_nonsec(bool nonsec);
void hal_sec_set_bt2mcu_nonsec(bool nonsec);
void hal_sec_set_flashctrl_nonsec(bool nonsec);
void hal_sec_init();
void hal_sec_idau_enable(bool enable);

#ifdef __cplusplus
}
#endif

#endif

