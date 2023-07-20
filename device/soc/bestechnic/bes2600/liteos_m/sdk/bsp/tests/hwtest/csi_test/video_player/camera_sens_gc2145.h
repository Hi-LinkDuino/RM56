/***************************************************************************
 *
 * Copyright 2015-2022 BES.
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
#ifndef __CSI_SENS_GC2145_H__
#define __CSI_SENS_GC2145_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_addr_map.h"
#include "video_player.h"

struct VP_DEVICE_OPS_T *gc2145_sensor_ops_get(void);

void gc2145_sensor_page_select(int page);

#ifdef __cplusplus
}
#endif

#endif
