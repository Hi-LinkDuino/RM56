/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "osal_case_cmd_test.h"

int OsalGetTestResult(uint32_t cmd)
{
    return OSAL_TEST_CASE_CHECK(cmd);
}

