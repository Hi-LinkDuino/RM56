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

#include "stdint.h"
#include "stdbool.h"

#define HKS_TEST_ASSERT(test) \
    if (!(test)) { \
        printf("[ASSERT][%s](%d):fail\n", __func__, __LINE__); \
    }

#ifndef _CUT_LOG_
#define HKS_TEST_LOG_E(fmt...) \
do { \
    printf("[ERROR]\t[%s](%d): ", __func__, __LINE__); \
    printf(fmt); \
    printf("\r\n"); \
} while (0)

#define HKS_TEST_LOG_I(fmt...) \
do { \
    printf("[INFO]\t[%s](%d): ", __func__, __LINE__); \
    printf(fmt); \
    printf("\r\n"); \
} while (0)

#define HKS_TEST_LOG_W(fmt...) \
do { \
    printf("[WARN]\t[%s](%d): ", __func__, __LINE__); \
    printf(fmt); \
    printf("\r\n"); \
} while (0)

#define HKS_TEST_LOG_D(fmt...) \
do { \
    printf("[DEBUG]\t[%s](%d): ", __func__, __LINE__); \
    printf(fmt); \
    printf("\r\n"); \
} while (0)
#else
#define HKS_TEST_LOG_E(...)
#define HKS_TEST_LOG_I(...)
#define HKS_TEST_LOG_W(...)
#define HKS_TEST_LOG_D(...)
#endif

#endif /* HKS_TEST_LOG_H */
