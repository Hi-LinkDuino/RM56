/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_regulator_entry_test.h"
#include "hdf_log.h"
#include "regulator_test.h"

#define HDF_LOG_TAG hdf_regulator_entry_test

int32_t HdfRegulatorUnitTestEntry(HdfTestMsg *msg)
{
    struct RegulatorTest *test = NULL;

    if (msg == NULL) {
        return HDF_FAILURE;
    }
    test = GetRegulatorTest();
    if (test == NULL || test->TestEntry == NULL) {
        HDF_LOGE("%s: tester is NULL!\n", __func__);
        msg->result = HDF_FAILURE;
        return HDF_FAILURE;
    }
    msg->result = test->TestEntry(test, msg->subCmd);
    return msg->result;
}
