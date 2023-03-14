/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_rtc_entry_test.h"
#include "hdf_log.h"
#include "rtc_test.h"

#define HDF_LOG_TAG hdf_rtc_entry_test

int32_t HdfRtcTestEntry(HdfTestMsg *msg)
{
    if (msg == NULL) {
        HDF_LOGE("%s: msg is NULL", __func__);
        return HDF_FAILURE;
    }

    msg->result = RtcTestExecute(msg->subCmd);

    return HDF_SUCCESS;
}
