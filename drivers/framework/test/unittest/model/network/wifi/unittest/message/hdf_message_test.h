/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef WIFI_MESSAGE_TEST_H
#define WIFI_MESSAGE_TEST_H
#include "hdf_base.h"

#define MSG_BREAK_IF(ERRCODE_VAR, CONDITION)                          \
    if (CONDITION) {                                                  \
        HDF_LOGE("%s:Line=%d:Check " #CONDITION, __func__, __LINE__); \
        ERRCODE_VAR = HDF_FAILURE;                                             \
        break;                                                        \
    }

#define MSG_RETURN_IF(CONDITION)                                          \
    do {                                                                  \
        if (CONDITION) {                                                  \
            HDF_LOGE("%s:Line=%d:Check " #CONDITION, __func__, __LINE__); \
            return HDF_FAILURE;                                                    \
        }                                                                 \
    } while (false)

#define MSG_BREAK_IF_NOT_SUCCESS(ERROR_CODE)                                 \
    if ((ERROR_CODE) != HDF_SUCCESS) {                                       \
        HDF_LOGE("%s:Line=%d,errCode=%d", __func__, __LINE__, (ERROR_CODE)); \
        errCode = HDF_FAILURE;                                                        \
        break;                                                               \
    }

#define MSG_RETURN_IF_FUNCTION_FAILED(ERRCODE_VAR, FUNCTION_CALL)                                \
    do {                                                                                         \
        ERRCODE_VAR = FUNCTION_CALL;                                                             \
        if ((ERRCODE_VAR) != 0) {                                                                \
            HDF_LOGE("%s:Line=%d:Ret=%d,Call " #FUNCTION_CALL, __func__, __LINE__, ERRCODE_VAR); \
            return ERRCODE_VAR;                                                                  \
        }                                                                                        \
    } while (false)

#define MSG_BREAK_IF_FUNCTION_FAILED(ERRCODE_VAR, FUNCTION_CALL)                             \
    ERRCODE_VAR = FUNCTION_CALL;                                                             \
    if ((ERRCODE_VAR) != 0) {                                                                \
        HDF_LOGE("%s:Line=%d:Ret=%d,Call " #FUNCTION_CALL, __func__, __LINE__, ERRCODE_VAR); \
        break;                                                                               \
    }


int32_t MessageQueueTest001(void);
int32_t MessageQueueTest002(void);
int32_t MessageQueueTest003(void);
int32_t MessageSingleNodeTest001(void);
int32_t MessageSingleNodeTest002(void);
int32_t MessageSingleNodeTest003(void);
int32_t MessageSingleNodeTest004(void);
int32_t MessageSingleNodeTest005(void);

#endif