/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */
#ifndef HDF_KEY_H
#define HDF_KEY_H

#include <securec.h>
#include "osal_time.h"
#include "input_config.h"

typedef struct {
    uint32_t definedEvent;
} KeyEventData;

typedef struct KeyDriverInfo {
    struct HdfDeviceObject *hdfKeyDev;
    KeyEventData eventData;
    uint8_t devType;
    KeyChipCfg *keyCfg;
    InputDevice *inputdev;
    uint16_t preStatus;
    uint64_t timeStamp;
} KeyDriver;

enum KeyStatusInfo {
    INPUT_KEY_UP,
    INPUT_KEY_DOWN,
    INPUT_KEY_INVALID
};
#endif