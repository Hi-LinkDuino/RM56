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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "securec.h"

#include "hidumper.h"
#include "hidumper_adapter.h"
#include "ohos_init.h"
#include "ohos_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

WEAK int DumpSysInfo(void)
{
    printf("Please implement the interface according to the platform!\n");
    return 0;
}

WEAK int DumpCpuUsage(void)
{
    printf("Please implement the interface according to the platform!\n");
    return 0;
}

WEAK int DumpMemUsage(void)
{
    printf("Please implement the interface according to the platform!\n");

    return 0;
}

WEAK int DumpTaskInfo(void)
{
    printf("Please implement the interface according to the platform!\n");
    return 0;
}

WEAK int DumpFaultLog(void)
{
    printf("Please implement the interface according to the platform!\n");
    return 0;
}

WEAK int DumpMemRegion(unsigned long long addr, unsigned long long size)
{
    (void)addr;
    (void)size;
    printf("Please implement the interface according to the platform!\n");
    return 0;
}

WEAK int DumpAllMem(void)
{
    printf("Please implement the interface according to the platform!\n");
    return 0;
}

WEAK int PlatformHiDumperIinit(void)
{
    printf("Please implement the interface according to the platform!\n");
    return 0;
}

static void HiDumperAdapterInit(void)
{
    struct HiDumperAdapter adapter = {
        .DumpSysInfo = DumpSysInfo,
        .DumpCpuUsage = DumpCpuUsage,
        .DumpMemUsage = DumpMemUsage,
        .DumpTaskInfo = DumpTaskInfo,
        .DumpFaultLog = DumpFaultLog,
        .DumpMemRegion = DumpMemRegion,
        .DumpAllMem = DumpAllMem,
    };

    if (HiDumperRegisterAdapter(&adapter) != 0) {
        printf("HiDumperRegisterAdapter failed!\n");
        return;
    }
    (void)PlatformHiDumperIinit();
}
CORE_INIT_PRI(HiDumperAdapterInit, 3);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif