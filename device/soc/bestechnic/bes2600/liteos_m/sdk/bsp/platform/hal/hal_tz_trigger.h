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
#ifndef __HAL_TZ_TRIGGER_H__
#define __HAL_TZ_TRIGGER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdbool.h"

typedef unsigned int (*HAL_TZ_TRIGGER_HANDLER)(void *param);
int hal_tz_trigger_open(HAL_TZ_TRIGGER_HANDLER rxhandler);

int hal_tz_trigger_close();

int hal_tz_trigger_set();

#ifdef __cplusplus
}
#endif

#endif


