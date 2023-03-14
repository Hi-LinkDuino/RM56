/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "osal_test_entry.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "osal_case_cmd_test.h"
#include "osal_get_case_test.h"

int32_t HdfOsalEntry(HdfTestMsg *msg)
{
    int32_t result = 0;
    if (msg->subCmd < OSAL_TEST_MAX) {
        if (msg->subCmd == OSAL_TEST_START) {
            result = OsaTestBegin();
        } else if (msg->subCmd == OSAL_TEST_END) {
            result = OsaTestEnd();
        } else if (msg->subCmd == OSAL_TEST_ALL) {
            result = OsaTestALLResult();
        } else {
            result = OsalGetTestResult(msg->subCmd);
        }
    } else {
        return HDF_FAILURE;
    }

    msg->result = (result != 0) ? HDF_FAILURE : HDF_SUCCESS;

    return HDF_SUCCESS;
}

