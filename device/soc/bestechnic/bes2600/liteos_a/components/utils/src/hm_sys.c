/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
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
#include "hal_trace.h"
#include <stdio.h>
#include <stdarg.h>
#include "los_init.h"
#include "los_config.h"
#include "stdio.h"
#include "string.h"
#include "los_task_pri.h"
#include "ui_main.h"


STATIC VOID UiMainTaskFunc(VOID)
{
    printf("UiMapinTaskFunc start ...\n");
    void fs_test(void);
    fs_test();
    UiMain();
}

STATIC UINT32 OsUiMainTaskCreate(VOID)
{
    UINT32 taskID;
    TSK_INIT_PARAM_S sysTask;

    (VOID)memset_s(&sysTask, sizeof(TSK_INIT_PARAM_S), 0, sizeof(TSK_INIT_PARAM_S));
    sysTask.pfnTaskEntry = (TSK_ENTRY_FUNC)UiMainTaskFunc;
    sysTask.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    sysTask.pcName = "UiMainTaskFunc";
    sysTask.usTaskPrio = LOSCFG_BASE_CORE_TSK_DEFAULT_PRIO;
    sysTask.uwResved = LOS_TASK_STATUS_DETACHED;
#ifdef LOSCFG_KERNEL_SMP
    sysTask.usCpuAffiMask = CPUID_TO_AFFI_MASK(ArchCurrCpuid());
#endif
    return LOS_TaskCreate(&taskID, &sysTask);
}

STATIC VOID HdfInit(VOID)
{
    extern int DeviceManagerStart(void);
    printf("DeviceManagerStart start ...\n");
    if (DeviceManagerStart()) {
        printf("No drivers need load by hdf manager!");
    }
    // PRINTK("DeviceManagerStart end ...\n");
    printf("DeviceManagerStart end ...\r\n");
    printf("UIKIT START ...\n");
    OsUiMainTaskCreate();
    printf("UIKIT END ...\n");
}

STATIC UINT32 OsHdfTaskCreate(VOID)
{
    UINT32 taskID;
    TSK_INIT_PARAM_S sysTask;

    (VOID)memset_s(&sysTask, sizeof(TSK_INIT_PARAM_S), 0, sizeof(TSK_INIT_PARAM_S));
    sysTask.pfnTaskEntry = (TSK_ENTRY_FUNC)HdfInit;
    sysTask.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    sysTask.pcName = "HdfInit";
    sysTask.usTaskPrio = LOSCFG_BASE_CORE_TSK_DEFAULT_PRIO;
    sysTask.uwResved = LOS_TASK_STATUS_DETACHED;
#ifdef LOSCFG_KERNEL_SMP
    sysTask.usCpuAffiMask = CPUID_TO_AFFI_MASK(ArchCurrCpuid());
#endif
    return LOS_TaskCreate(&taskID, &sysTask);
}

void OsHdfInit(void)
{
    UINT32 ret;
    ret = OsHdfTaskCreate();
    if (ret != LOS_OK) {
        return ret;
    }

    return 0;
}

void os_pre_init_hook(void)
{
    OsHdfInit();
}