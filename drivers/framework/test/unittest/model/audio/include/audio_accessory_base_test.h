/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef AUDIO_ACCESSORY_BASE_TEST_H
#define AUDIO_ACCESSORY_BASE_TEST_H

#include "hdf_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

int32_t AccessoryI2cReadWriteTest(void);
int32_t AccessoryRegBitsReadTest(void);
int32_t AccessoryRegBitsUpdateTest(void);

int32_t AccessoryDeviceFrequencyParseTest(void);
int32_t AccessoryDaiParamsUpdateTest(void);
int32_t AccessoryDeviceCfgGetTest(void);
int32_t AccessoryDeviceCtrlRegInitTest(void);
int32_t AccessoryDeviceRegReadTest(void);
int32_t AccessoryDeviceRegWriteTest(void);
int32_t AccessoryGetConfigInfoTest(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* AUDIO_ACCESSORY_BASE_TEST_H */
