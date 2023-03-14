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

#ifndef TEST_TIMER_H
#define TEST_TIMER_H

#include "print_log.h"

#include <hctest.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

double CalcTimeSpecDiff(struct timespec before, struct timespec after);

unsigned long long TimeConsumingOperation(void);

#ifdef __cplusplus
}
#endif

#define RUN_AND_PRINT_ELAPSED_TIME(out_result, operation)               \
    do {                                                                \
        struct timespec __before, __after;                              \
        int __beforeRes = clock_gettime(CLOCK_MONOTONIC, &__before);    \
        out_result = operation;                                         \
        int __afterRes = clock_gettime(CLOCK_MONOTONIC, &__after);      \
        TEST_ASSERT_TRUE(__beforeRes == 0 && __afterRes == 0);          \
        double __mseconds = CalcTimeSpecDiff(__before, __after);        \
        LOGI("operation %s took %f ms", #operation, __mseconds);        \
    } while (0)

#define RUN_AND_PRINT_ELAPSED_TIME_WITHOUT_RESULT(operation)            \
    do {                                                                \
        struct timespec __before, __after;                              \
        int __beforeRes = clock_gettime(CLOCK_MONOTONIC, &__before);    \
        (void)(operation);                                              \
        int __afterRes = clock_gettime(CLOCK_MONOTONIC, &__after);      \
        TEST_ASSERT_TRUE(__beforeRes == 0 && __afterRes == 0);          \
        double __mseconds = CalcTimeSpecDiff(__before, __after);        \
        LOGI("operation %s took %f ms", #operation, __mseconds);        \
    } while (0)

#endif // TEST_TIMER_H
