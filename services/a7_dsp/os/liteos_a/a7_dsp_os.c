/***************************************************************************
 *
 * Copyright 2015-2019 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#include <string.h>
#include "cmsis_os.h"
#include "plat_addr_map.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "los_memory.h"
#include "los_cpup_pri.h"
#include "los_process_pri.h"

extern uint32_t plat_ipc_lock(void);
extern void plat_ipc_unlock(uint32_t state);

int transq_lock_init(void)
{
    return 0;
}

uint32_t transq_lock(void)
{
    return plat_ipc_lock();
}

void transq_unlock(uint32_t state)
{
    plat_ipc_unlock(state);
}

void a7_show_mem_info(void)
{
    uint8_t fragment = 0;
    uint8_t usage = 0;
    LOS_MEM_POOL_STATUS poolStatus = {0};
    if (OS_SYS_MEM_ADDR == NULL) {
        TRACE(0,"%s OS_SYS_MEM_ADDR is NULL!", __FUNCTION__);
        return;
    }
    UINT32 ret = LOS_MemInfoGet(OS_SYS_MEM_ADDR, &poolStatus);
    if (ret != LOS_OK) {
        TRACE(0,"%s LOS_MemInfoGet failed,ret:%d", __FUNCTION__, ret);
        return;
    }
    TRACE(0,"pool size  used size   free size   "
            "max free size  used node num   free node num   UsageWaterLine");
    TRACE(0,"       0x%-8x   0x%-8x   0x%-8x    0x%-8x   0x%-13x   0x%-13x   0x%-13x",
            LOS_MemPoolSizeGet(OS_SYS_MEM_ADDR), poolStatus.totalUsedSize,
            poolStatus.totalFreeSize, poolStatus.maxFreeNodeSize, poolStatus.usedNodeNum,
            poolStatus.freeNodeNum, poolStatus.usageWaterLine);

    if (poolStatus.totalFreeSize != 0) {
        fragment = 100 - poolStatus.maxFreeNodeSize * 100 / poolStatus.totalFreeSize; /* 100: percent denominator. */
    }

    if (LOS_MemPoolSizeGet(OS_SYS_MEM_ADDR) != 0) {
        usage = LOS_MemTotalUsedGet(OS_SYS_MEM_ADDR) * 100 / LOS_MemPoolSizeGet(OS_SYS_MEM_ADDR); /* 100: percent denominator. */
    }
    TRACE(0, "os heap usage = %d\%, fragment = %d, maxFreeSize = %d, totalFreeSize = %d\n", usage, fragment,
        poolStatus.maxFreeNodeSize, poolStatus.totalFreeSize);
}

static int g_cpu_task_running_flag = 0;
static void a7_show_cpu_task(void *argument);
osThreadDef(a7_show_cpu_task, osPriorityNormal, 1, (6*1024), "a7_show_cpu_task");
static void a7_show_cpu_task(void *argument)
{
    g_cpu_task_running_flag = 1;
    OsShellCmdTskInfoGet(OS_ALL_TASK_MASK, NULL, OS_PROCESS_INFO_ALL);
    g_cpu_task_running_flag = 0;
    osThreadExit();
}

void a7_debug_mm_print(void)
{
    a7_show_mem_info();
    if (g_cpu_task_running_flag ==  0) {
        osThreadCreate(osThread(a7_show_cpu_task), NULL);
    }
}
