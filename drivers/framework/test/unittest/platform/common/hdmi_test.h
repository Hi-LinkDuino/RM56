/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDMI_TEST_H
#define HDMI_TEST_H

#include "hdf_device_desc.h"
#include "platform_if.h"

enum HdmiTestCmd {
    HDMI_START_AND_STOP_01 = 0,
    HDMI_SET_AUDIO_ATTR_01 = 1,
    HDMI_SET_VIDEO_ATTR_01 = 2,
    HDMI_SET_HDR_ATTR_01 = 3,
    HDMI_SET_AVMUTE_01 = 4,
    HDMI_EDID_RAW_DATA_GET_01 = 5,
    HDMI_DEEP_COLOR_SET_AND_GET_01 = 6,
    HDMI_HPD_REGISTER_AND_UNREGISTER_01 = 7,
};

struct HdmiTester {
    struct IDeviceIoService service;
    struct HdfDeviceObject *device;
    int32_t (*TestEntry)(struct HdmiTester *tester, int32_t cmd);
    uint32_t busNum;
    DevHandle handle;
};

static inline struct HdmiTester *GetHdmiTester(void)
{
    return (struct HdmiTester *)DevSvcManagerClntGetService("HDMI_TEST");
}

#endif /* HDMI_TEST_H */
