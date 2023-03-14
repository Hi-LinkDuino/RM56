/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#ifndef HOS_LITE_HIVIEW_CONFIG_H
#define HOS_LITE_HIVIEW_CONFIG_H

#include <stdint.h>

#include "ohos_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/* Set the following parameters for different products based on their own resources. */
#ifndef HIVIEW_FILE_DIR
#define HIVIEW_FILE_DIR                    ""
#endif
/* file path config */
#define HIVIEW_FILE_OUT_PATH_LOG           HIVIEW_FILE_DIR"debug.log"
#define HIVIEW_FILE_OUT_PATH_UE_EVENT      HIVIEW_FILE_DIR"ue.event"
#define HIVIEW_FILE_OUT_PATH_FAULT_EVENT   HIVIEW_FILE_DIR"fault.event"
#define HIVIEW_FILE_OUT_PATH_STAT_EVENT    HIVIEW_FILE_DIR"stat.event"

#define HIVIEW_FILE_PATH_DUMP              HIVIEW_FILE_DIR"dump.dat"

#define HIVIEW_FILE_PATH_LOG               HIVIEW_FILE_OUT_PATH_LOG".tmp"
#define HIVIEW_FILE_PATH_UE_EVENT          HIVIEW_FILE_OUT_PATH_UE_EVENT".tmp"
#define HIVIEW_FILE_PATH_FAULT_EVENT       HIVIEW_FILE_OUT_PATH_FAULT_EVENT".tmp"
#define HIVIEW_FILE_PATH_STAT_EVENT        HIVIEW_FILE_OUT_PATH_STAT_EVENT".tmp"

/* cache size config */
#ifndef LOG_STATIC_CACHE_SIZE
#define LOG_STATIC_CACHE_SIZE              1024            /* Must be greater than HIVIEW_HILOG_FILE_BUF_SIZE. */
#endif
#ifndef EVENT_CACHE_SIZE
#define EVENT_CACHE_SIZE                   256             /* Must be greater than HIVIEW_HIEVENT_FILE_BUF_SIZE. */
#endif
#ifndef JS_LOG_CACHE_SIZE
#define JS_LOG_CACHE_SIZE                  512
#endif
#ifndef HIVIEW_HILOG_FILE_BUF_SIZE
#define HIVIEW_HILOG_FILE_BUF_SIZE         512
#endif
#ifndef HIVIEW_HIEVENT_FILE_BUF_SIZE
#define HIVIEW_HIEVENT_FILE_BUF_SIZE       128
#endif

/* RAM dump config */
#define HIVIEW_DUMP_PRE_SIZE               (384 * 1024)
#define HIVIEW_DUMP_HEADER_OFFSET          0x400
#define HIVIEW_DUMP_RAM_ADDR               0x10000400      /* 10000000 ~ 100003FF reserved for NVIC, no need save */
#define HIVIEW_DUMP_RAM_SIZE               (HIVIEW_DUMP_PRE_SIZE - HIVIEW_DUMP_HEADER_OFFSET)

/* feature switch */
#define HIVIEW_FEATURE_ON                  1
#define HIVIEW_FEATURE_OFF                 0

#define OUT_PATH_WAIT_TIMEOUT              (5 * 1000)      /* 5 seconds */

#pragma pack(1)
typedef struct {
    uint8 outputOption : 4;       /* Control log output mode. */
    uint8 hiviewInited : 1;       /* Indicates whether the hiview service is inited. */
    uint8 level : 3;              /* Control log output level. HILOG_LV_XXX */
    uint8 logSwitch : 1;          /* Indicates whether to enable the log component. */
    uint8 eventSwitch : 1;        /* Indicates whether to enable the event component. */
    uint8 dumpSwitch : 1;         /* Indicates whether to enable the dump component. */
    uint64 logOutputModule;       /* Control log output module. */
    uint16 writeFailureCount;
} HiviewConfig;
#pragma pack()

typedef enum {
    OUTPUT_OPTION_DEBUG = 0,    /* Output to the UART without buffer. Commercial versions are forbidden. */
    OUTPUT_OPTION_FLOW,         /* Output to UART via SAMR */
    OUTPUT_OPTION_TEXT_FILE,
    OUTPUT_OPTION_BIN_FILE,
    OUTPUT_OPTION_PRINT = 8,
    OUTPUT_OPTION_MAX
} HiviewOutputOption;

extern HiviewConfig g_hiviewConfig;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef HOS_LITE_HIVIEW_CONFIG_H */
