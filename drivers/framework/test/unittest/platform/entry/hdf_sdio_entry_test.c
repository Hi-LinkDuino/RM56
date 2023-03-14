/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_sdio_entry_test.h"
#include "hdf_log.h"
#include "sdio_if.h"
#include "sdio_test.h"

#define HDF_LOG_TAG hdf_sdio_entry_test

int32_t HdfSdioUnitTestEntry(HdfTestMsg *msg)
{
    struct SdioTester *tester = NULL;

    if (msg == NULL) {
        HDF_LOGE("HdfSdioUnitTestEntry: msg is NULL");
        return HDF_FAILURE;
    }
    tester = GetSdioTest();
    if (tester == NULL || tester->TestEntry == NULL) {
        HDF_LOGE("HdfSdioUnitTestEntry: tester/TestEntry is NULL");
        msg->result = HDF_FAILURE;
        return HDF_FAILURE;
    }
    msg->result = tester->TestEntry(tester, msg->subCmd);
    return msg->result;
}
