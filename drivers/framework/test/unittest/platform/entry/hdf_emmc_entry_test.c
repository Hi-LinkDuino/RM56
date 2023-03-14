/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_emmc_entry_test.h"
#include "emmc_test.h"
#include "hdf_log.h"

#define HDF_LOG_TAG hdf_emmc_entry_test

int32_t HdfEmmcUnitTestEntry(HdfTestMsg *msg)
{
    struct EmmcTester *tester = NULL;

    if (msg == NULL) {
        HDF_LOGE("HdfEmmcUnitTestEntry: msg is NULL");
        return HDF_FAILURE;
    }
    tester = GetEmmcTester();
    if (tester == NULL || tester->TestEntry == NULL) {
        HDF_LOGE("HdfEmmcUnitTestEntry: tester/TestEntry is NULL");
        msg->result = HDF_FAILURE;
        return HDF_FAILURE;
    }
    msg->result = tester->TestEntry(tester, msg->subCmd);
    return msg->result;
}
