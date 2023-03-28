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
#ifndef __HAL_SEC_BEST2003_H__
#define __HAL_SEC_BEST2003_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"

void hal_sec_set_cksum_nonsec(bool nonsec);
void hal_sec_set_crc_nonsec(bool nonsec);
void hal_sec_set_wf2mcu_nonsec(bool nonsec);
void hal_mpc_spy_nonsec_bypass(uint32_t addr, bool bypass);
void hal_page_spy_set(uint32_t spy_base, uint32_t start_addr, uint32_t len, bool write, bool read);

#ifdef __cplusplus
}
#endif

#endif

