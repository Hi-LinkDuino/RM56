/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef DISPLAY_TEST_H
#define DISPLAY_TEST_H
#include <cstring>
#include <stdio.h>
#include "display_gfx.h"
#include "display_gralloc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define __FILENAME__ (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1) : __FILE__)

#ifndef DISPLAY_TEST_CHK_RETURN_NOT_VALUE
#define DISPLAY_TEST_CHK_RETURN_NOT_VALUE(val, ret, ...) \
    do {                                                 \
        if (val) {                                       \
            __VA_ARGS__;                                 \
            return;                                      \
        }                                                \
    } while (0)
#endif

#ifndef DISPLAY_TEST_CHK_RETURN
#define DISPLAY_TEST_CHK_RETURN(val, ret, ...) \
    do {                                       \
        if (val) {                             \
            __VA_ARGS__;                       \
            return (ret);                      \
        }                                      \
    } while (0)
#endif

#define DISPLAY_TEST_LOGD(format, ...)                                                          \
    do {                                                                                        \
        printf("[%s@%s:%d] " format "\n", __FUNCTION__, __FILENAME__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#define DISPLAY_TEST_LOGE(format, ...)                            \
    do {                                                          \
        printf("\033[0;32;31m"                                    \
            "[%s@%s:%d] " format "\033[m"                         \
            "\n",                                                 \
            __FUNCTION__, __FILENAME__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#ifdef __cplusplus
}
#endif
#endif // DISPLAY_TEST_H
