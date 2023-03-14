/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "audio_accessory_base_test.h"
#include "audio_accessory_base.h"
#include "audio_driver_log.h"

#define HDF_LOG_TAG audio_accessory_base_test

int32_t AccessoryI2cReadWriteTest(void)
{
    if (AccessoryI2cReadWrite(NULL, 0) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t AccessoryRegBitsReadTest(void)
{
    if (AccessoryRegBitsRead(NULL, NULL) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t AccessoryRegBitsUpdateTest(void)
{
    struct AudioMixerControl regValue;
    regValue.reg = 1; // 1 is test reg
    if (AccessoryRegBitsUpdate(regValue) == HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AccessoryDaiParamsUpdate fail", __func__, __LINE__);
    }

    return HDF_SUCCESS;
}


int32_t AccessoryDeviceFrequencyParseTest(void)
{
    if (AccessoryDeviceFrequencyParse(0, NULL) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t AccessoryDaiParamsUpdateTest(void)
{
    struct DaiParamsVal value;

    value.frequencyVal = 1; // 1 is dma channel
    if (AccessoryDaiParamsUpdate(value) == HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AccessoryDaiParamsUpdate fail", __func__, __LINE__);
    }

    return HDF_SUCCESS;
}

int32_t AccessoryDeviceCfgGetTest(void)
{
    if (AccessoryDeviceCfgGet(NULL, NULL) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t AccessoryDeviceCtrlRegInitTest(void)
{
    if (AccessoryDeviceCtrlRegInit() == HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AccessoryDeviceCtrlRegInitTest fail", __func__, __LINE__);
    }

    return HDF_SUCCESS;
}

int32_t AccessoryDeviceRegReadTest(void)
{
    uint32_t val;
    struct AccessoryDevice codec;
    if (AccessoryDeviceRegRead(NULL, 0, NULL) == HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AccessoryDeviceRegRead fail", __func__, __LINE__);
    }

    if (AccessoryDeviceRegRead(&codec, 0, &val) == HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AccessoryDeviceRegRead fail", __func__, __LINE__);
    }
    return HDF_SUCCESS;
}

int32_t AccessoryDeviceRegWriteTest(void)
{
    struct AccessoryDevice codec;
    if (AccessoryDeviceRegWrite(NULL, 0, 0) == HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AccessoryDeviceRegWrite fail", __func__, __LINE__);
    }

    if (AccessoryDeviceRegWrite(&codec, 0, 0) == HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AccessoryDeviceRegWrite fail", __func__, __LINE__);
    }
    return HDF_SUCCESS;
}

int32_t AccessoryGetConfigInfoTest(void)
{
    struct HdfDeviceObject device;
    struct AccessoryData codecData;
    (void)memset_s(&device, sizeof(struct HdfDeviceObject), 0, sizeof(struct HdfDeviceObject));
    (void)memset_s(&codecData, sizeof(struct AccessoryData), 0, sizeof(struct AccessoryData));

    if (AccessoryGetConfigInfo(NULL, NULL) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (AccessoryGetConfigInfo(&device, &codecData) == HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AccessoryGetConfigInfo fail", __func__, __LINE__);
    }

    return HDF_SUCCESS;
}

