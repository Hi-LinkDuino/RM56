/***************************************************************************
 *
 * Copyright 2015-2023 BES.
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
#ifndef __AP_VPLAYER_APP_H__
#define __AP_VPLAYER_APP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "vplayer_app_common.h"

void ap_vplayer_app_init(enum VPLAYER_APP_MODE_T mode);

void ap_vplayer_app_open(enum VPLAYER_APP_MODE_T mode);

void ap_vplayer_app_close(enum VPLAYER_APP_MODE_T mode);

void ap_vplayer_app_on(bool on);

void ap_vplayer_app_trigger(bool on);

#ifdef __cplusplus
}
#endif

#endif /* __AP_VPLAYER_APP_H__ */
