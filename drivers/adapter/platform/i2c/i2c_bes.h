/*
 * Copyright (c) 2021-2022 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
 *
 * This file is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef _I2C_BES_H__
#define _I2C_BES_H__

#include "hal_i2c.h"
#include "hal_iomux.h"
#ifdef CHIP_BEST2003
#include "hal_iomux_best2003.h"
#endif
#include "device_resource_if.h"
#include "osal_mutex.h"
#ifdef __cplusplus
extern "C" {
#endif

struct I2cResource {
    uint32_t port;
    uint32_t mode;
    uint32_t sclPin;
    uint32_t sdaPin;
    uint32_t useSync;
    uint32_t useDma;
    uint32_t asMaster;
    uint32_t speed;
    uint32_t addrAsSlave;
    uint32_t addressWidth;
};

struct I2cDevice {
    uint16_t devAddr;      /**< slave device addr */
    uint32_t addressWidth; /**< Addressing mode: 7 bit or 10 bit */
    struct OsalMutex mutex;
    uint32_t port;
    struct HAL_I2C_CONFIG_T i2cCfg;
    struct I2cResource resource;
};

#ifdef __cplusplus
}
#endif

#endif
