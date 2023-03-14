/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef HKS_TEST_LOG_H
#define HKS_TEST_LOG_H

#include <stdint.h>
#include <stdbool.h>

#include "securec.h"

enum HksLogLevel {
    HKS_LOG_LEVEL_I,
    HKS_LOG_LEVEL_E,
    HKS_LOG_LEVEL_W,
    HKS_LOG_LEVEL_D,
};

#ifdef __cplusplus
extern "C" {
#endif
void HksTestLog(uint32_t logLevel, const char *funcName, int32_t lineNo, const char *format, ...);
void HksAssertLog(bool test);

#ifdef __cplusplus
}
#endif

#ifndef _CUT_LOG_
#define HKS_TEST_LOG_E(...) \
do { \
    HksTestLog(HKS_LOG_LEVEL_E, __FUNCTION__, __LINE__, __VA_ARGS__); \
} while (0)

#define HKS_TEST_LOG_I(...) \
do { \
    HksTestLog(HKS_LOG_LEVEL_I, __FUNCTION__, __LINE__, __VA_ARGS__); \
} while (0)

#define HKS_TEST_LOG_W(...) \
do { \
    HksTestLog(HKS_LOG_LEVEL_W, __FUNCTION__, __LINE__, __VA_ARGS__); \
} while (0)

#define HKS_TEST_LOG_D(...) \
do { \
    HksTestLog(HKS_LOG_LEVEL_D, __FUNCTION__, __LINE__, __VA_ARGS__); \
} while (0)
#else
#define HKS_TEST_LOG_E(...)
#define HKS_TEST_LOG_I(...)
#define HKS_TEST_LOG_W(...)
#define HKS_TEST_LOG_D(...)
#endif

#define HKS_TEST_ASSERT HksAssertLog

#endif /* HKS_TEST_LOG_H */
