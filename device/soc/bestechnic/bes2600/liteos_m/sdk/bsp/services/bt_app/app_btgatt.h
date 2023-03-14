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
#ifndef __APP_BTGATT_H__
#define __APP_BTGATT_H__

#define ATT_SERVICE_UUID 0x0118
#ifdef __cplusplus
extern "C" {
#endif
void app_btgatt_init(void);
void app_btgatt_addsdp(uint16_t pServiceUUID, uint16_t startHandle, uint16_t endHandle);

#ifdef __cplusplus
}
#endif

#endif

