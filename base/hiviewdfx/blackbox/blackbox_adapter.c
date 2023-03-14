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

/*-----------includes---------*/
#include "blackbox.h"
#include "blackbox_adapter.h"
#include "blackbox_detector.h"
#include "ohos_init.h"

/*----------local macroes-----------*/
/*----------global variables-----------*/
/*----------function definitions-----------*/
WEAK void SystemModuleDump(const char *logDir, struct ErrorInfo *info)
{
    (void)logDir;
    (void)info;
    BBOX_PRINT_ERR("Please implement the interface according to the platform!\n");
}

WEAK void SystemModuleReset(struct ErrorInfo *info)
{
    (void)info;
    BBOX_PRINT_ERR("Please implement the interface according to the platform!\n");
}

WEAK int SystemModuleGetLastLogInfo(struct ErrorInfo *info)
{
    (void)info;
    BBOX_PRINT_ERR("Please implement the interface according to the platform!\n");
    return -1;
}

WEAK int SystemModuleSaveLastLog(const char *logDir, struct ErrorInfo *info)
{
    (void)logDir;
    (void)info;
    BBOX_PRINT_ERR("Please implement the interface according to the platform!\n");
    return -1;
}

WEAK int FullWriteFile(const char *filePath, const char *buf,
    unsigned int bufSize, int isAppend)
{
    (void)filePath;
    (void)buf;
    (void)bufSize;
    (void)isAppend;
    BBOX_PRINT_ERR("Please implement the interface according to the platform!\n");
    return -1;
}

WEAK char *GetFaultLogPath(void)
{
    BBOX_PRINT_ERR("Please implement the interface according to the platform!\n");
    return "";
}

WEAK void RebootSystem(void)
{
    BBOX_PRINT_ERR("Please implement the interface according to the platform!\n");
}

#ifdef BLACKBOX_TEST
static void BBoxTest(void)
{
    struct ModuleOps ops = {
        .module = "MODULE_TEST",
        .Dump = NULL,
        .Reset = NULL,
        .GetLastLogInfo = NULL,
        .SaveLastLog = NULL,
    };

    if (BBoxRegisterModuleOps(&ops) != 0) {
        BBOX_PRINT_ERR("BBoxRegisterModuleOps failed!\n");
        return;
    }
    BBoxNotifyError("EVENT_TEST1", "MODULE_TEST", "Test BBoxNotifyError1", 0);
}
#endif

static void BBoxAdapterInit(void)
{
    struct ModuleOps ops = {
        .module = MODULE_SYSTEM,
        .Dump = SystemModuleDump,
        .Reset = SystemModuleReset,
        .GetLastLogInfo = SystemModuleGetLastLogInfo,
        .SaveLastLog = SystemModuleSaveLastLog,
    };

    if (BBoxRegisterModuleOps(&ops) != 0) {
        BBOX_PRINT_ERR("BBoxRegisterModuleOps failed!\n");
        return;
    }
#ifdef BLACKBOX_TEST
    BBoxTest();
#endif
}
CORE_INIT_PRI(BBoxAdapterInit, 2);