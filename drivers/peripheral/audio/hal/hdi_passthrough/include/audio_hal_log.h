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

#ifndef AUDIO_HAL_LOG_H
#define AUDIO_HAL_LOG_H

#include <stdio.h>
#include "osal_time.h"
#include "hdf_log.h"

#define MAX_TIME_INFO_LEN 64

#ifndef AUDIO_HAL_HDF_LOG
#define LOG_FUN_INFO() do { \
    } while (0)

#define LOG_FUN_ERR(fmt, arg...) do { \
        char s[MAX_TIME_INFO_LEN] = {0}; \
        AudioGetSysTime(s, MAX_TIME_INFO_LEN); \
        printf("%s %s: [%s]: [%d]:[ERROR]:" fmt"\n", s, __FILE__, __func__, __LINE__, ##arg); \
    } while (0)

#define LOG_PARA_INFO(fmt, arg...) do { \
    } while (0)

#else
#define LOG_FUN_INFO() do { \
    } while (0)

#define LOG_FUN_ERR(fmt, arg...) do { \
        HDF_LOGE("[%{public}s][%{public}s][line:%{public}d]: " fmt, __FILE__, __func__, __LINE__, ##arg); \
    } while (0)

#define LOG_PARA_INFO(fmt, arg...) do { \
    } while (0)

#endif


#endif
