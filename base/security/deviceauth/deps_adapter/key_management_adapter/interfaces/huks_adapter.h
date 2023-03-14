/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef HUKS_ADAPTER_H
#define HUKS_ADAPTER_H

#include "alg_defs.h"
#include "hal_error.h"
#include "hc_types.h"

#define BITS_PER_BYTE 8

#define CAL_ARRAY_SIZE(arr) ((sizeof(arr)) / (sizeof((arr)[0])))

#define CHECK_LEN_ZERO_RETURN_ERROR_CODE(len, paramTag) \
    do { \
        if ((len) == 0) { \
            LOGE("%s is invalid length.", (paramTag)); \
            return HAL_ERR_INVALID_LEN; \
        } \
    } while (0)

#define CHECK_PTR_RETURN_HAL_ERROR_CODE(ptr, paramTag) \
    do { \
        if ((ptr) == NULL) { \
            LOGE("%s is null.", (paramTag)); \
            return HAL_ERR_NULL_PTR; \
        } \
    } while (0)

#define CHECK_LEN_LOWER_RETURN(len, min, paramTag) \
    do { \
        if ((len) < (min)) { \
            LOGE("%s is invalid length.", (paramTag)); \
            return HAL_ERR_INVALID_LEN; \
        } \
    } while (0)

#define CHECK_LEN_HIGHER_RETURN(len, max, paramTag) \
    do { \
        if ((len) > (max)) { \
            LOGE("%s is invalid length.", (paramTag)); \
            return HAL_ERR_INVALID_LEN; \
        } \
    } while (0)

#define CHECK_LEN_EQUAL_RETURN(len, value, paramTag) \
    do { \
        if ((len) != (value)) { \
            LOGE("%s is invalid length.", (paramTag)); \
            return HAL_ERR_INVALID_LEN; \
        } \
    } while (0)

struct KeyRoleInfo {
    uint8_t userType;
    uint8_t pairType;
    uint8_t reserved1;
    uint8_t reserved2;
};

union KeyRoleInfoUnion {
    struct KeyRoleInfo roleInfoStruct;
    uint32_t roleInfo;
};

#ifdef __cplusplus
extern "C" {
#endif

const AlgLoader *GetRealLoaderInstance(void);

#ifdef __cplusplus
}
#endif
#endif
