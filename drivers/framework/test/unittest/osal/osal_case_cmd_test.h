/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef OSAL_TEST_CASE_CMD_H
#define OSAL_TEST_CASE_CMD_H

#include "hdf_base.h"
#include "osal_test_case_def.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BITS_PER_INT 32

#define OSAL_TEST_CASE_CNT (OSAL_TEST_MAX / BITS_PER_INT + 1)

extern uint32_t g_osalTestCases[OSAL_TEST_CASE_CNT];
extern bool g_testEndFlag;

#define OSAL_TEST_CASE_SET(cmd) (g_osalTestCases[(cmd) / BITS_PER_INT] |= (1 << ((cmd) % BITS_PER_INT)))
#define OSAL_TEST_CASE_CHECK(cmd) (g_osalTestCases[(cmd) / BITS_PER_INT] & (1 << ((cmd) % BITS_PER_INT)))

#define UT_TEST_CHECK_RET(val, cmd) do { \
    if ((val) && (g_testEndFlag == false)) { \
        HDF_LOGE("[OSAL_UT_TEST] %s %d %d OSA_UT_TEST_FAIL ", __func__, __LINE__, cmd); \
        OSAL_TEST_CASE_SET((cmd)); \
    } \
} while (0)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OSAL_TEST_CASE_CMD_H */
