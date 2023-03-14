/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "audio_dsp_base_test.h"
#include "audio_dsp_base.h"
#include "audio_driver_log.h"

#define HDF_LOG_TAG audio_dsp_base_test
#define KCONTROL_TEST_SERVICE_NAME "dsp_service_0"

int32_t DspGetServiceNameTest(void)
{
    struct DeviceResourceAttr attr;
    struct DeviceResourceNode node;
    struct HdfDeviceObject device;
    if (DspGetServiceName(NULL, NULL) == HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("TestDspGetServiceName fail");
        return HDF_FAILURE;
    }

    attr.value = KCONTROL_TEST_SERVICE_NAME;
    attr.name  = "serviceName";
    node.attrData = &attr;
    device.property = &node;
    if (DspGetServiceName(&device, NULL) == HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("TestDspGetServiceName fail");
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t DspGetDaiNameTest(void)
{
    struct DeviceResourceNode node;
    struct HdfDeviceObject device;
    struct DeviceResourceAttr attr;

    if (DspGetDaiName(NULL, NULL) == HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("TestDspGetDaiName fail");
        return HDF_FAILURE;
    }

    attr.value = KCONTROL_TEST_SERVICE_NAME;
    attr.name  = "dspDaiName";
    node.attrData = &attr;
    device.property = &node;
    if (DspGetDaiName(&device, NULL) == HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("TestDspGetDaiName fail");
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

