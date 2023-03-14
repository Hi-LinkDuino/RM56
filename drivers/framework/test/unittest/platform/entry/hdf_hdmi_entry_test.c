/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_hdmi_entry_test.h"
#include "hdf_log.h"
#include "hdmi_if.h"
#include "hdmi_test.h"

#define HDF_LOG_TAG hdf_hdmi_entry_test

int32_t HdfHdmiUnitTestEntry(HdfTestMsg *msg)
{
    struct HdmiTester *tester = NULL;

    if (msg == NULL) {
        HDF_LOGE("HdfHdmiUnitTestEntry: msg is NULL");
        return HDF_FAILURE;
    }
    tester = GetHdmiTester();
    if (tester == NULL || tester->TestEntry == NULL) {
        HDF_LOGE("HdfHdmiUnitTestEntry: tester/TestEntry is NULL");
        msg->result = HDF_FAILURE;
        return HDF_FAILURE;
    }
    msg->result = tester->TestEntry(tester, msg->subCmd);
    return msg->result;
}
