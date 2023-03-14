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

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "ohos_init.h"
#include "hiview_def.h"
#include "hiview_util.h"
#include "hiview_config.h"
#include "hiview_service.h"
#include "hiview_log.h"
#include "hiview_log_limit.h"
#include "hiview_output_log.h"

#define LOG_IS_OUTPUT(mod) (g_hiviewConfig.logOutputModule & (((uint64_t)1) << (mod)))

static HiLogModuleInfo g_logModuleInfo[HILOG_MODULE_MAX];
const char * const FUN_ARG_S = "0123456I";

static boolean CheckParameters(uint8 module, uint8 level);

/* The first step does not involve memory allocation. */
static void HiLogInit(void)
{
    HIVIEW_UartPrint("hilog will init.\n");
    InitCoreLogOutput();

    /* The module that is not registered cannot print the log. */
    if (HiLogRegisterModule(HILOG_MODULE_HIVIEW, "HIVIEW") == FALSE ||
        HiLogRegisterModule(HILOG_MODULE_SAMGR, "SAMGR") == FALSE ||
        HiLogRegisterModule(HILOG_MODULE_UPDATE, "UPDATE") == FALSE ||
        HiLogRegisterModule(HILOG_MODULE_ACE, "ACE") == FALSE ||
        HiLogRegisterModule(HILOG_MODULE_AAFWK, "AAFWK") == FALSE ||
        HiLogRegisterModule(HILOG_MODULE_APP, "APP") == FALSE ||
        HiLogRegisterModule(HILOG_MODULE_GRAPHIC, "GRAPHIC") == FALSE ||
        HiLogRegisterModule(HILOG_MODULE_MEDIA, "MEDIA") == FALSE ||
        HiLogRegisterModule(HILOG_MODULE_DMS, "DMS") == FALSE ||
        HiLogRegisterModule(HILOG_MODULE_SEN, "SEN") == FALSE ||
        HiLogRegisterModule(HILOG_MODULE_SCY, "SCY") == FALSE ||
        HiLogRegisterModule(HILOG_MODULE_SOFTBUS, "SOFTBUS") == FALSE ||
        HiLogRegisterModule(HILOG_MODULE_POWERMGR, "POWERMGR") == FALSE ||
        HiLogRegisterModule(HILOG_MODULE_UIKIT, "UIKIT") == FALSE) {
        return;
    }

    HiviewRegisterInitFunc(HIVIEW_CMP_TYPE_LOG, InitLogOutput);
    HiviewRegisterInitFunc(HIVIEW_CMP_TYPE_LOG_LIMIT, InitLogLimit);
    HILOG_DEBUG(HILOG_MODULE_HIVIEW, "hilog init success.");
}
CORE_INIT_PRI(HiLogInit, 0);

static boolean CheckParameters(uint8 module, uint8 level)
{
    if ((level < g_hiviewConfig.level) ||  (level < HILOG_COMPILE_LEVEL) ||
        (module >= HILOG_MODULE_MAX) || (g_logModuleInfo[module].name == NULL)) {
        return FALSE;
    }

    return TRUE;
}

boolean HiLogRegisterModule(uint16 id, const char *name)
{
    if ((id >= HILOG_MODULE_MAX) || name == NULL || g_logModuleInfo[id].name != NULL) {
        return FALSE;
    }

    uint32 len = (uint32)strnlen(name, LOG_MODULE_NAME_LEN + 1);
    if (len >= LOG_MODULE_NAME_LEN - 1) {
        return FALSE;
    }
    uint32 i = 0;
    while (i < len && name[i] != 0) {
        if (!((name[i] >= 'a' && name[i] <= 'z') || (name[i] >= 'A' && name[i] <= 'Z'))) {
            return FALSE;
        }
        i++;
    }

    g_logModuleInfo[id].name = name;
    g_logModuleInfo[id].id = id;
    return TRUE;
}

const char *HiLogGetModuleName(uint8 id)
{
    if (id >= HILOG_MODULE_MAX) {
        return "";
    }
    const char *name = g_logModuleInfo[id].name;
    return (name == NULL) ? "" : name;
}

void HiLogPrintf(uint8 module, uint8 level, const char *nums, const char *fmt, ...)
{
    static char newFmt[] = "The number of parameters is invalid.";
    HiLogContent logContent = { 0 };
    int32 argsNum = (nums - FUN_ARG_S);
    if (argsNum < 0 || argsNum > LOG_MULTI_PARA_MAX) {
        fmt = newFmt;
        argsNum = 0;
    }

    if (g_hiviewConfig.logSwitch == HIVIEW_FEATURE_OFF || !CheckParameters(module, level) ||
        !LOG_IS_OUTPUT(module)) {
        return;
    }

    HiLogCommon *pCommon = &(logContent.commonContent);
    pCommon->head = LOG_INFO_HEAD;
    pCommon->module = module;
    pCommon->level = level;
    pCommon->fmt = fmt;
    pCommon->valueNumber = (uint8)argsNum;
    pCommon->task = (uint8)HIVIEW_GetTaskId();
    uint64 cur = HIVIEW_GetCurrentTime();
    pCommon->time = (uint32)(cur / MS_PER_SECOND);
    pCommon->milli = (uint16)(cur % MS_PER_SECOND);

    uint8 i = 0;
    va_list args;
    va_start(args, fmt);
    while (i < argsNum) {
        logContent.values[i++] = va_arg(args, uint32);
    }
    va_end(args);

    OutputLog((uint8 *)&logContent, sizeof(HiLogCommon) + sizeof(uint32) * argsNum);
}

void HILOG_HashPrintf(uint8 module, uint8 level, const char *nums, uint32 hash, ...)
{
    HiLogContent logContent = {0};
    int32 argsNum = (nums - FUN_ARG_S);
    if (argsNum < 0 || argsNum > LOG_MULTI_PARA_MAX) {
        argsNum = 0;
    }

    if (g_hiviewConfig.logSwitch == HIVIEW_FEATURE_OFF || !CheckParameters(module, level) ||
        !LOG_IS_OUTPUT(module)) {
        return;
    }

    HiLogCommon *pCommon = &(logContent.commonContent);
    pCommon->head = LOG_INFO_HEAD;
    pCommon->module = module;
    pCommon->level = SET_HASH_FLAG(level);
    pCommon->fmt = (const char*)hash;
    pCommon->valueNumber = (uint8)argsNum;
    pCommon->task = (uint8)HIVIEW_GetTaskId();
    uint64 cur = HIVIEW_GetCurrentTime();
    pCommon->time = (uint32)(cur / MS_PER_SECOND);
    pCommon->milli = (uint16)(cur % MS_PER_SECOND);

    uint8 i = 0;
    va_list args;
    va_start(args, hash);
    while (i < argsNum) {
        logContent.values[i++] = va_arg(args, uint32);
    }
    va_end(args);

    OutputLog((uint8 *)&logContent, sizeof(HiLogCommon) + sizeof(uint32) * argsNum);
}

void HiLogFlush(boolean syncFlag)
{
    FlushLog(syncFlag);
}

uint32 HiLogGetConfigOption(void)
{
    return HiviewGetConfigOption();
}

void HiLogRegisterProc(HilogProc func)
{
    HiviewRegisterHilogProc(func);
}

void HiLogUnRegisterProc(HilogProc func)
{
    HiviewUnRegisterHilogProc(func);
}

void HiLogFileAddWatcher(FileProc func, const char *path)
{
    HiviewRegisterHiLogFileWatcher(func, path);
}

void HiLogFileRemoveWatcher(FileProc func)
{
    HiviewUnRegisterHiLogFileWatcher(func);
}

int HiLogFileProc(const char *dest, uint8 mode)
{
    return HiLogFileProcImp(dest, mode);
}

void HiLogOutputFileLock(void)
{
    HiLogOutputFileLockImp();
}

void HiLogOutputFileUnLock(void)
{
    HiLogOutputFileUnLockImp();
}

uint32 HiLogGetLogLevel(void)
{
    return g_hiviewConfig.level;
}

boolean HiLogSetLogLevel(uint8 level)
{
    if (level >= HILOG_LV_DEBUG && level < HILOG_LV_MAX) {
        g_hiviewConfig.level = level;
        printf("Set log level: %d\n", level);
        return TRUE;
    }
    return FALSE;
}
