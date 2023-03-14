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
#ifndef __OTA_BASIC_H__
#define __OTA_BASIC_H__

// ota data path is exclusive, anytime there is only one ota datapath available
typedef enum
{
    OTA_BASIC_NO_DATAPATH_ENALBED  = 0,
    OTA_BASIC_SPP_DATAPATH_ENABLED = 1,
    OTA_BASIC_BLE_DATAPATH_ENABLED = 2,
} OTA_BASIC_ENABLED_DATAPATH_E;

typedef enum
{
    OTA_BASIC_TWS_INVALID_ROLE   = 0,
    OTA_BASIC_TWS_FREEMAN   = 1,
    OTA_BASIC_TWS_MASTER    = 2,
    OTA_BASIC_TWS_SLAVE     = 3,
    OTA_BASIC_TWS_UNKNOWN   = 4,
} OTA_BASIC_TWS_ROLE_E;

#ifndef NEW_IMAGE_FLASH_OFFSET
#define NEW_IMAGE_FLASH_OFFSET    0x180000
#endif

#ifdef __cplusplus
extern "C" {
#endif

void ota_basic_env_init(void);
bool ota_basic_enable_datapath_status(OTA_BASIC_ENABLED_DATAPATH_E datapath, uint8_t* macAddr);
bool ota_basic_disable_datapath_status(OTA_BASIC_ENABLED_DATAPATH_E datapath, uint8_t* macAddr);
uint8_t* ota_basic_get_mac_addr(void);
OTA_BASIC_TWS_ROLE_E ota_basic_get_tws_role(void);
OTA_BASIC_ENABLED_DATAPATH_E ota_basic_get_datapath(void);
bool ota_basic_is_remap_enabled(void);
uint32_t ota_basic_get_remap_offset(void);
uint32_t ota_basic_get_max_image_size(void);

#ifdef __cplusplus
}
#endif

#endif

