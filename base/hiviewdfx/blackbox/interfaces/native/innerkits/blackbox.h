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

#ifndef BLACKBOX_H
#define BLACKBOX_H

#include "hiview_log.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* End of #if __cplusplus */
#endif /* End of #ifdef __cplusplus */

#define ERROR_INFO_HEADER            "#### error info ####\r\n"
#define ERROR_INFO_HEADER_FORMAT     "event: %s\r\nmodule: %s\r\nerrorDesc: %s\r\n"
#define ERROR_INFO_MAX_LEN 768
#define Min(a, b)                    (((a) > (b)) ? (b) : (a))
#define BBOX_PRINT_ERR(format, ...)  HILOG_DEBUG(HILOG_MODULE_HIVIEW, "bbox: func: %s line: %d, Err: " \
    format, __func__, __LINE__, ##__VA_ARGS__)
#define BBOX_PRINT_INFO(format, ...) HILOG_DEBUG(HILOG_MODULE_HIVIEW, "bbox: Info: " format, ##__VA_ARGS__)

#define PATH_MAX_LEN                 256
#define EVENT_MAX_LEN                32
#define MODULE_MAX_LEN               32
#define ERROR_DESC_MAX_LEN           512
#define MODULE_SYSTEM                "SYSTEM"
#define EVENT_SYSREBOOT              "SYSREBOOT"
#define EVENT_LONGPRESS              "LONGPRESS"
#define EVENT_COMBINATIONKEY         "COMBINATIONKEY"
#define EVENT_SUBSYSREBOOT           "SUBSYSREBOOT"
#define EVENT_POWEROFF               "POWEROFF"
#define EVENT_PANIC                  "PANIC"
#define EVENT_SYS_WATCHDOG           "SYSWATCHDOG"
#define EVENT_HUNGTASK               "HUNGTASK"
#define EVENT_BOOTFAIL               "BOOTFAIL"

struct ErrorInfo {
    char event[EVENT_MAX_LEN];
    char module[MODULE_MAX_LEN];
    char errorDesc[ERROR_DESC_MAX_LEN];
};

struct ModuleOps {
    char module[MODULE_MAX_LEN];
    void (*Dump)(const char *logDir, struct ErrorInfo *info);
    void (*Reset)(struct ErrorInfo *info);
    int (*GetLastLogInfo)(struct ErrorInfo *info);
    int (*SaveLastLog)(const char *logDir, struct ErrorInfo *info);
};

int BBoxRegisterModuleOps(struct ModuleOps *ops);
int BBoxNotifyError(const char event[EVENT_MAX_LEN],
    const char module[MODULE_MAX_LEN],
    const char errorDesc[ERROR_DESC_MAX_LEN],
    int needSysReset);
int BBoxDriverInit(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* End of #if __cplusplus */
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef BLACKBOX_H */