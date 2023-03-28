/**
 * @file ota_config.h
 * @author BES AI team
 * @version 0.1
 * @date 2020-06-20
 * 
 * @copyright Copyright (c) 2015-2020 BES Technic.
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
 */

/**
 * NOTE: This file is used to initialize the BES OTA related configurations
 * 
 */

#ifndef __OTA_CONFIG_H__
#define __OTA_CONFIG_H__

#ifdef __cplusplus
extern "C"{
#endif

/*****************************header include********************************/

/******************************macro defination*****************************/
#define CONTENT_INFO_MAX 40

/******************************type defination******************************/
typedef struct
{
    uint32_t id;
    uint32_t offset;
    uint32_t length;
} CONTENT_INFO_T;

typedef struct
{
    uint32_t mainInfo;
    uint32_t version;
    uint32_t contentNum;
    CONTENT_INFO_T info[CONTENT_INFO_MAX];
    uint32_t crc32;
} CUSTOM_IMAGE_HEADER_T;

/****************************function declearation**************************/
void bes_ota_init_handler(void);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef __OTA_CONFIG_H__ */