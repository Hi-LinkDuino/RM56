/*
 * Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "los_process_pri.h"
#include "los_sched_pri.h"
#include "los_task_pri.h"
#include "los_hw_pri.h"
#include "los_sem_pri.h"
#include "los_mp.h"
#include "los_exc.h"
#include "asm/page.h"
#ifdef LOSCFG_FS_VFS
#include "fs/fd_table.h"
#include "fs/fs_operation.h"
#endif
#include "time.h"
#include "user_copy.h"
#include "los_signal.h"
#ifdef LOSCFG_SECURITY_VID
#include "vid_api.h"
#endif
#ifdef LOSCFG_SECURITY_CAPABILITY
#include "capability_api.h"
#endif
#ifdef LOSCFG_KERNEL_DYNLOAD
#include "los_load_elf.h"
#endif
#include "los_swtmr_pri.h"
#include "los_vm_map.h"
#include "los_vm_phys.h"
#include "los_vm_syscall.h"


LITE_OS_SEC_BSS LosProcessCB *g_processCBArray = NULL;
LITE_OS_SEC_DATA_INIT STATIC LOS_DL_LIST g_freeProcess;
LITE_OS_SEC_DATA_INIT STATIC LOS_DL_LIST g_processRecycleList;
LITE_OS_SEC_BSS UINT32 g_userInitProcess = OS_INVALID_VALUE;
LITE_OS_SEC_BSS UINT32 g_kernelInitProcess = OS_INVALID_VALUE;
LITE_OS_SEC_BSS UINT32 g_kernelIdleProcess = OS_INVALID_VALUE;
LITE_OS_SEC_BSS UINT32 g_processMaxNum;
LITE_OS_SEC_BSS ProcessGroup *g_processGroup = NULL;

STATIC INLINE VOID OsInsertPCBToFreeList(LosProcessCB *processCB)
{
    UINT32 pid = processCB->processID;
    (VOID)memset_s(processCB, sizeof(LosProcessCB), 0, sizeof(LosProcessCB));
    processCB->processID = pid;
    processCB->processStatus = OS_PROCESS_FLAG_UNUSED;
    processCB->timerID = (timer_t)(UINTPTR)MAX_INVALID_TIMER_VID;
    LOS_ListTailInsert(&g_freeProcess, &processCB->pendList);
}

VOID OsDeleteTaskFromProcess(LosTaskCB *taskCB)
{
    LosProcessCB *processCB = OS_PCB_FROM_PID(taskCB->processID);

    LOS_ListDelete(&taskCB->threadList);
    processCB->threadNumber--;
    OsTaskInsertToRecycleList(taskCB);
}

UINT32 OsProcessAddNewTask(UINT32 pid, LosTaskCB *taskCB)
{
    UINT32 intSave;
    UINT16 numCount;
    LosProcessCB *processCB = OS_PCB_FROM_PID(pid);

    SCHEDULER_LOCK(intSave);
    taskCB->processID = pid;
    LOS_ListTailInsert(&(processCB->threadSiblingList), &(taskCB->threadList));

    if (OsProcessIsUserMode(processCB)) {
        taskCB->taskStatus |= OS_TASK_FLAG_USER_MODE;
        if (processCB->threadNumber > 0) {
            taskCB->basePrio = OS_TCB_FROM_TID(processCB->threadGroupID)->basePrio;
        } else {
            taskCB->basePrio = OS_USER_PROCESS_PRIORITY_HIGHEST;
        }
    } else {
        taskCB->basePrio = OsCurrTaskGet()->basePrio;
    }

#ifdef LOSCFG_KERNEL_VM
    taskCB->archMmu = (UINTPTR)&processCB->vmSpace->archMmu;
#endif
    if (!processCB->threadNumber) {
        processCB->threadGroupID = taskCB->taskID;
    }
    processCB->threadNumber++;

    numCount = processCB->threadCount;
    processCB->threadCount++;
    SCHEDULER_UNLOCK(intSave);
    return numCount;
}

STATIC ProcessGroup *OsCreateProcessGroup(UINT32 pid)
{
    LosProcessCB *processCB = NULL;
    ProcessGroup *group = LOS_MemAlloc(m_aucSysMem1, sizeof(ProcessGroup));
    if (group == NULL) {
        return NULL;
    }

    group->groupID = pid;
    LOS_ListInit(&group->processList);
    LOS_ListInit(&group->exitProcessList);

    processCB = OS_PCB_FROM_PID(pid);
    LOS_ListTailInsert(&group->processList, &processCB->subordinateGroupList);
    processCB->group = group;
    processCB->processStatus |= OS_PROCESS_FLAG_GROUP_LEADER;
    if (g_processGroup != NULL) {
        LOS_ListTailInsert(&g_processGroup->groupList, &group->groupList);
    }

    return group;
}

STATIC VOID OsExitProcessGroup(LosProcessCB *processCB, ProcessGroup **group)
{
    LosProcessCB *groupProcessCB = OS_PCB_FROM_PID(processCB->group->groupID);

    LOS_ListDelete(&processCB->subordinateGroupList);
    if (LOS_ListEmpty(&processCB->group->processList) && LOS_ListEmpty(&processCB->group->exitProcessList)) {
        LOS_ListDelete(&processCB->group->groupList);
        groupProcessCB->processStatus &= ~OS_PROCESS_FLAG_GROUP_LEADER;
        *group = processCB->group;
        if (OsProcessIsUnused(groupProcessCB) && !(groupProcessCB->processStatus & OS_PROCESS_FLAG_EXIT)) {
            LOS_ListDelete(&groupProcessCB->pendList);
            OsInsertPCBToFreeList(groupProcessCB);
        }
    }

    processCB->group = NULL;
}

STATIC ProcessGroup *OsFindProcessGroup(UINT32 gid)
{
    ProcessGroup *group = NULL;
    if (g_processGroup->groupID == gid) {
        return g_processGroup;
    }

    LOS_DL_LIST_FOR_EACH_ENTRY(group, &g_processGroup->groupList, ProcessGroup, groupList) {
        if (group->groupID == gid) {
            return group;
        }
    }

    PRINT_INFO("%s is find group : %u failed!\n", __FUNCTION__, gid);
    return NULL;
}

STATIC INT32 OsSendSignalToSpecifyProcessGroup(ProcessGroup *group, siginfo_t *info, INT32 permission)
{
    INT32 ret, success, err;
    LosProcessCB *childCB = NULL;

    success = 0;
    ret = -LOS_ESRCH;
    LOS_DL_LIST_FOR_EACH_ENTRY(childCB, &(group->processList), LosProcessCB, subordinateGroupList) {
        if (childCB->processID == 0) {
            continue;
        }

        err = OsDispatch(childCB->processID, info, permission);
        success |= !err;
        ret = err;
    }
    /* At least one success. */
    return success ? LOS_OK : ret;
}

LITE_OS_SEC_TEXT INT32 OsSendSignalToAllProcess(siginfo_t *info, INT32 permission)
{
    INT32 ret, success, err;
    ProcessGroup *group = NULL;

    success = 0;
    err = OsSendSignalToSpecifyProcessGroup(g_processGroup, info, permission);
    success |= !err;
    ret = err;
    /* all processes group */
    LOS_DL_LIST_FOR_EACH_ENTRY(group, &g_processGroup->groupList, ProcessGroup, groupList) {
        /* all processes in the process group. */
        err = OsSendSignalToSpecifyProcessGroup(group, info, permission);
        success |= !err;
        ret = err;
    }
    return success ? LOS_OK : ret;
}

LITE_OS_SEC_TEXT INT32 OsSendSignalToProcessGroup(INT32 pid, siginfo_t *info, INT32 permission)
{
    ProcessGroup *group = NULL;
    /* Send SIG to all processes in process group PGRP.
       If PGRP is zero, send SIG to all processes in
       the current process's process group. */
    group = OsFindProcessGroup(pid ? -pid : LOS_GetCurrProcessGroupID());
    if (group == NULL) {
        return -LOS_ESRCH;
    }
    /* all processes in the process group. */
    return OsSendSignalToSpecifyProcessGroup(group, info, permission);
}

STATIC LosProcessCB *OsFindGroupExitProcess(ProcessGroup *group, INT32 pid)
{
    LosProcessCB *childCB = NULL;

    LOS_DL_LIST_FOR_EACH_ENTRY(childCB, &(group->exitProcessList), LosProcessCB, subordinateGroupList) {
        if ((childCB->processID == pid) || (pid == OS_INVALID_VALUE)) {
            return childCB;
        }
    }

    PRINT_INFO("%s find exit process : %d failed in group : %u\n", __FUNCTION__, pid, group->groupID);
    return NULL;
}

STATIC UINT32 OsFindChildProcess(const LosProcessCB *processCB, INT32 childPid)
{
    LosProcessCB *childCB = NULL;

    if (childPid < 0) {
        goto ERR;
    }

    LOS_DL_LIST_FOR_EACH_ENTRY(childCB, &(processCB->childrenList), LosProcessCB, siblingList) {
        if (childCB->processID == childPid) {
            return LOS_OK;
        }
    }

ERR:
    PRINT_INFO("%s is find the child : %d failed in parent : %u\n", __FUNCTION__, childPid, processCB->processID);
    return LOS_NOK;
}

STATIC LosProcessCB *OsFindExitChildProcess(const LosProcessCB *processCB, INT32 childPid)
{
    LosProcessCB *exitChild = NULL;

    LOS_DL_LIST_FOR_EACH_ENTRY(exitChild, &(processCB->exitChildList), LosProcessCB, siblingList) {
        if ((childPid == OS_INVALID_VALUE) || (exitChild->processID == childPid)) {
            return exitChild;
        }
    }

    PRINT_INFO("%s is find the exit child : %d failed in parent : %u\n", __FUNCTION__, childPid, processCB->processID);
    return NULL;
}

VOID OsWaitWakeTask(LosTaskCB *taskCB, UINT32 wakePID)
{
    taskCB->waitID = wakePID;
    OsSchedTaskWake(taskCB);
#ifdef LOSCFG_KERNEL_SMP
    LOS_MpSchedule(OS_MP_CPU_ALL);
#endif
}

STATIC BOOL OsWaitWakeSpecifiedProcess(LOS_DL_LIST *head, const LosProcessCB *processCB, LOS_DL_LIST **anyList)
{
    LOS_DL_LIST *list = head;
    LosTaskCB *taskCB = NULL;
    UINT32 pid = 0;
    BOOL find = FALSE;

    while (list->pstNext != head) {
        taskCB = OS_TCB_FROM_PENDLIST(LOS_DL_LIST_FIRST(list));
        if ((taskCB->waitFlag == OS_PROCESS_WAIT_PRO) && (taskCB->waitID == processCB->processID)) {
            if (pid == 0) {
                pid = processCB->processID;
                find = TRUE;
            } else {
                pid = OS_INVALID_VALUE;
            }

            OsWaitWakeTask(taskCB, pid);
            continue;
        }

        if (taskCB->waitFlag != OS_PROCESS_WAIT_PRO) {
            *anyList = list;
            break;
        }
        list = list->pstNext;
    }

    return find;
}

STATIC VOID OsWaitCheckAndWakeParentProcess(LosProcessCB *parentCB, const LosProcessCB *processCB)
{
    LOS_DL_LIST *head = &parentCB->waitList;
    LOS_DL_LIST *list = NULL;
    LosTaskCB *taskCB = NULL;
    BOOL findSpecified = FALSE;

    if (LOS_ListEmpty(&parentCB->waitList)) {
        return;
    }

    findSpecified = OsWaitWakeSpecifiedProcess(head, processCB, &list);
    if (findSpecified == TRUE) {
        /* No thread is waiting for any child process to finish */
        if (LOS_ListEmpty(&parentCB->waitList)) {
            return;
        } else if (!LOS_ListEmpty(&parentCB->childrenList)) {
            /* Other child processes exist, and other threads that are waiting
             * for the child to finish continue to wait
             */
            return;
        }
    }

    /* Waiting threads are waiting for a specified child process to finish */
    if (list == NULL) {
        return;
    }

    /* No child processes exist and all waiting threads are awakened */
    if (findSpecified == TRUE) {
        while (list->pstNext != head) {
            taskCB = OS_TCB_FROM_PENDLIST(LOS_DL_LIST_FIRST(list));
            OsWaitWakeTask(taskCB, OS_INVALID_VALUE);
        }
        return;
    }

    while (list->pstNext != head) {
        taskCB = OS_TCB_FROM_PENDLIST(LOS_DL_LIST_FIRST(list));
        if (taskCB->waitFlag == OS_PROCESS_WAIT_GID) {
            if (taskCB->waitID != processCB->group->groupID) {
                list = list->pstNext;
                continue;
            }
        }

        if (findSpecified == FALSE) {
            OsWaitWakeTask(taskCB, processCB->processID);
            findSpecified = TRUE;
        } else {
            OsWaitWakeTask(taskCB, OS_INVALID_VALUE);
        }

        if (!LOS_ListEmpty(&parentCB->childrenList)) {
            break;
        }
    }

    return;
}

LITE_OS_SEC_TEXT VOID OsProcessResourcesToFree(LosProcessCB *processCB)
{
#ifdef LOSCFG_KERNEL_VM
    if (OsProcessIsUserMode(processCB)) {
        (VOID)OsVmSpaceRegionFree(processCB->vmSpace);
    }
#endif

#ifdef LOSCFG_FS_VFS
    if (OsProcessIsUserMode(processCB)) {
        delete_files(processCB->files);
    }
    processCB->files = NULL;
#endif

#ifdef LOSCFG_SECURITY_CAPABILITY
    if (processCB->user != NULL) {
        (VOID)LOS_MemFree(m_aucSysMem1, processCB->user);
        processCB->user = NULL;
    }
#endif

    OsSwtmrRecycle(processCB->processID);
    processCB->timerID = (timer_t)(UINTPTR)MAX_INVALID_TIMER_VID;

#ifdef LOSCFG_SECURITY_VID
    if (processCB->timerIdMap.bitMap != NULL) {
        VidMapDestroy(processCB);
        processCB->timerIdMap.bitMap = NULL;
    }
#endif

#ifdef LOSCFG_KERNEL_LITEIPC
    (VOID)LiteIpcPoolDestroy(processCB->processID);
#endif

#ifdef LOSCFG_KERNEL_CPUP
    UINT32 intSave;
    OsCpupBase *processCpup = processCB->processCpup;
    SCHEDULER_LOCK(intSave);
    processCB->processCpup = NULL;
    SCHEDULER_UNLOCK(intSave);
    (VOID)LOS_MemFree(m_aucSysMem1, processCpup);
#endif

    if (processCB->resourceLimit != NULL) {
        (VOID)LOS_MemFree((VOID *)m_aucSysMem0, processCB->resourceLimit);
        processCB->resourceLimit = NULL;
    }
}

LITE_OS_SEC_TEXT STATIC VOID OsRecycleZombiesProcess(LosProcessCB *childCB, ProcessGroup **group)
{
    OsExitProcessGroup(childCB, group);
    LOS_ListDelete(&childCB->siblingList);
    if (childCB->processStatus & OS_PROCESS_STATUS_ZOMBIES) {
        OsDeleteTaskFromProcess(OS_TCB_FROM_TID(childCB->threadGroupID));
        childCB->processStatus &= ~OS_PROCESS_STATUS_ZOMBIES;
        childCB->processStatus |= OS_PROCESS_FLAG_UNUSED;
    }

    LOS_ListDelete(&childCB->pendList);
    if (childCB->processStatus & OS_PROCESS_FLAG_EXIT) {
        LOS_ListHeadInsert(&g_processRecycleList, &childCB->pendList);
    } else if (childCB->processStatus & OS_PROCESS_FLAG_GROUP_LEADER) {
        LOS_ListTailInsert(&g_processRecycleList, &childCB->pendList);
    } else {
        OsInsertPCBToFreeList(childCB);
    }
}

STATIC VOID OsDealAliveChildProcess(LosProcessCB *processCB)
{
    UINT32 parentID;
    LosProcessCB *childCB = NULL;
    LosProcessCB *parentCB = NULL;
    LOS_DL_LIST *nextList = NULL;
    LOS_DL_LIST *childHead = NULL;

    if (!LOS_ListEmpty(&processCB->childrenList)) {
        childHead = processCB->childrenList.pstNext;
        LOS_ListDelete(&(processCB->childrenList));
        if (OsProcessIsUserMode(processCB)) {
            parentID = g_userInitProcess;
        } else {
            parentID = g_kernelInitProcess;
        }

        for (nextList = childHead; ;) {
            childCB = OS_PCB_FROM_SIBLIST(nextList);
            childCB->parentProcessID = parentID;
            nextList = nextList->pstNext;
            if (nextList == childHead) {
                break;
            }
        }

        parentCB = OS_PCB_FROM_PID(parentID);
        LOS_ListTailInsertList(&parentCB->childrenList, childHead);
    }

    return;
}

STATIC VOID OsChildProcessResourcesFree(const LosProcessCB *processCB)
{
    LosProcessCB *childCB = NULL;
    ProcessGroup *group = NULL;

    while (!LOS_ListEmpty(&((LosProcessCB *)processCB)->exitChildList)) {
        childCB = LOS_DL_LIST_ENTRY(processCB->exitChildList.pstNext, LosProcessCB, siblingList);
        OsRecycleZombiesProcess(childCB, &group);
        (VOID)LOS_MemFree(m_aucSysMem1, group);
    }
}

VOID OsProcessNaturalExit(LosProcessCB *processCB, UINT32 status)
{
    LosProcessCB *parentCB = NULL;

    OsChildProcessResourcesFree(processCB);

    /* is a child process */
    if (processCB->parentProcessID != OS_INVALID_VALUE) {
        parentCB = OS_PCB_FROM_PID(processCB->parentProcessID);
        LOS_ListDelete(&processCB->siblingList);
        if (!OsProcessExitCodeSignalIsSet(processCB)) {
            OsProcessExitCodeSet(processCB, status);
        }
        LOS_ListTailInsert(&parentCB->exitChildList, &processCB->siblingList);
        LOS_ListDelete(&processCB->subordinateGroupList);
        LOS_ListTailInsert(&processCB->group->exitProcessList, &processCB->subordinateGroupList);

        OsWaitCheckAndWakeParentProcess(parentCB, processCB);

        OsDealAliveChildProcess(processCB);

        processCB->processStatus |= OS_PROCESS_STATUS_ZOMBIES;

#ifdef LOSCFG_KERNEL_VM
        (VOID)OsKill(processCB->parentProcessID, SIGCHLD, OS_KERNEL_KILL_PERMISSION);
#endif
        LOS_ListHeadInsert(&g_processRecycleList, &processCB->pendList);
        return;
    }

    LOS_Panic("pid : %u is the root process exit!\n", processCB->processID);
    return;
}

STATIC UINT32 OsProcessInit(VOID)
{
    UINT32 index;
    UINT32 size;

    g_processMaxNum = LOSCFG_BASE_CORE_PROCESS_LIMIT;
    size = g_processMaxNum * sizeof(LosProcessCB);

    g_processCBArray = (LosProcessCB *)LOS_MemAlloc(m_aucSysMem1, size);
    if (g_processCBArray == NULL) {
        return LOS_NOK;
    }
    (VOID)memset_s(g_processCBArray, size, 0, size);

    LOS_ListInit(&g_freeProcess);
    LOS_ListInit(&g_processRecycleList);

    for (index = 0; index < g_processMaxNum; index++) {
        g_processCBArray[index].processID = index;
        g_processCBArray[index].processStatus = OS_PROCESS_FLAG_UNUSED;
        LOS_ListTailInsert(&g_freeProcess, &g_processCBArray[index].pendList);
    }

    g_kernelIdleProcess = 0; /* 0: The idle process ID of the kernel-mode process is fixed at 0 */
    LOS_ListDelete(&OS_PCB_FROM_PID(g_kernelIdleProcess)->pendList);

    g_userInitProcess = 1; /* 1: The root process ID of the user-mode process is fixed at 1 */
    LOS_ListDelete(&OS_PCB_FROM_PID(g_userInitProcess)->pendList);

    g_kernelInitProcess = 2; /* 2: The root process ID of the kernel-mode process is fixed at 2 */
    LOS_ListDelete(&OS_PCB_FROM_PID(g_kernelInitProcess)->pendList);

    return LOS_OK;
}

LITE_OS_SEC_TEXT VOID OsProcessCBRecycleToFree(VOID)
{
    UINT32 intSave;
    LosProcessCB *processCB = NULL;

    SCHEDULER_LOCK(intSave);
    while (!LOS_ListEmpty(&g_processRecycleList)) {
        processCB = OS_PCB_FROM_PENDLIST(LOS_DL_LIST_FIRST(&g_processRecycleList));
        if (!(processCB->processStatus & OS_PROCESS_FLAG_EXIT)) {
            break;
        }
        SCHEDULER_UNLOCK(intSave);

        OsTaskCBRecycleToFree();

        SCHEDULER_LOCK(intSave);
        processCB->processStatus &= ~OS_PROCESS_FLAG_EXIT;
#ifdef LOSCFG_KERNEL_VM
        LosVmSpace *space = NULL;
        if (OsProcessIsUserMode(processCB)) {
            space = processCB->vmSpace;
        }
        processCB->vmSpace = NULL;
#endif
        /* OS_PROCESS_FLAG_GROUP_LEADER: The lead process group cannot be recycled without destroying the PCB.
         * !OS_PROCESS_FLAG_UNUSED: Parent process does not reclaim child process resources.
         */
        LOS_ListDelete(&processCB->pendList);
        if ((processCB->processStatus & OS_PROCESS_FLAG_GROUP_LEADER) ||
            (processCB->processStatus & OS_PROCESS_STATUS_ZOMBIES)) {
            LOS_ListTailInsert(&g_processRecycleList, &processCB->pendList);
        } else {
            /* Clear the bottom 4 bits of process status */
            OsInsertPCBToFreeList(processCB);
        }
#ifdef LOSCFG_KERNEL_VM
        SCHEDULER_UNLOCK(intSave);
        (VOID)LOS_VmSpaceFree(space);
        SCHEDULER_LOCK(intSave);
#endif
    }

    SCHEDULER_UNLOCK(intSave);
}

STATIC VOID OsDeInitPCB(LosProcessCB *processCB)
{
    UINT32 intSave;
    ProcessGroup *group = NULL;

    if (processCB == NULL) {
        return;
    }

    OsProcessResourcesToFree(processCB);

    SCHEDULER_LOCK(intSave);
    if (processCB->parentProcessID != OS_INVALID_VALUE) {
        LOS_ListDelete(&processCB->siblingList);
        processCB->parentProcessID = OS_INVALID_VALUE;
    }

    if (processCB->group != NULL) {
        OsExitProcessGroup(processCB, &group);
    }

    processCB->processStatus &= ~OS_PROCESS_STATUS_INIT;
    processCB->processStatus |= OS_PROCESS_FLAG_EXIT;
    LOS_ListHeadInsert(&g_processRecycleList, &processCB->pendList);
    SCHEDULER_UNLOCK(intSave);

    (VOID)LOS_MemFree(m_aucSysMem1, group);
    OsWriteResourceEvent(OS_RESOURCE_EVENT_FREE);
    return;
}

UINT32 OsSetProcessName(LosProcessCB *processCB, const CHAR *name)
{
    errno_t errRet;

    if (processCB == NULL) {
        return LOS_EINVAL;
    }

    if (name != NULL) {
        errRet = strncpy_s(processCB->processName, OS_PCB_NAME_LEN, name, OS_PCB_NAME_LEN - 1);
        if (errRet == EOK) {
            return LOS_OK;
        }
    }

    switch (processCB->processMode) {
        case OS_KERNEL_MODE:
            errRet = snprintf_s(processCB->processName, OS_PCB_NAME_LEN, OS_PCB_NAME_LEN - 1,
                                "KerProcess%u", processCB->processID);
            break;
        default:
            errRet = snprintf_s(processCB->processName, OS_PCB_NAME_LEN, OS_PCB_NAME_LEN - 1,
                                "UserProcess%u", processCB->processID);
            break;
    }

    if (errRet < 0) {
        return LOS_NOK;
    }
    return LOS_OK;
}

STATIC UINT32 OsInitPCB(LosProcessCB *processCB, UINT32 mode, const CHAR *name)
{
    processCB->processMode = mode;
    processCB->processStatus = OS_PROCESS_STATUS_INIT;
    processCB->parentProcessID = OS_INVALID_VALUE;
    processCB->threadGroupID = OS_INVALID_VALUE;
    processCB->umask = OS_PROCESS_DEFAULT_UMASK;
    processCB->timerID = (timer_t)(UINTPTR)MAX_INVALID_TIMER_VID;

    LOS_ListInit(&processCB->threadSiblingList);
    LOS_ListInit(&processCB->childrenList);
    LOS_ListInit(&processCB->exitChildList);
    LOS_ListInit(&(processCB->waitList));

#ifdef LOSCFG_KERNEL_VM
    if (OsProcessIsUserMode(processCB)) {
        processCB->vmSpace = OsCreateUserVmSpace();
        if (processCB->vmSpace == NULL) {
            processCB->processStatus = OS_PROCESS_FLAG_UNUSED;
            return LOS_ENOMEM;
        }
    } else {
        processCB->vmSpace = LOS_GetKVmSpace();
    }
#endif

#ifdef LOSCFG_KERNEL_CPUP
    processCB->processCpup = (OsCpupBase *)LOS_MemAlloc(m_aucSysMem1, sizeof(OsCpupBase));
    if (processCB->processCpup == NULL) {
        return LOS_ENOMEM;
    }
    (VOID)memset_s(processCB->processCpup, sizeof(OsCpupBase), 0, sizeof(OsCpupBase));
#endif

#ifdef LOSCFG_SECURITY_VID
    status_t status = VidMapListInit(processCB);
    if (status != LOS_OK) {
        return LOS_ENOMEM;
    }
#endif

#ifdef LOSCFG_SECURITY_CAPABILITY
    OsInitCapability(processCB);
#endif

    if (OsSetProcessName(processCB, name) != LOS_OK) {
        return LOS_ENOMEM;
    }

    return LOS_OK;
}

#ifdef LOSCFG_SECURITY_CAPABILITY
STATIC User *OsCreateUser(UINT32 userID, UINT32 gid, UINT32 size)
{
    User *user = LOS_MemAlloc(m_aucSysMem1, sizeof(User) + (size - 1) * sizeof(UINT32));
    if (user == NULL) {
        return NULL;
    }

    user->userID = userID;
    user->effUserID = userID;
    user->gid = gid;
    user->effGid = gid;
    user->groupNumber = size;
    user->groups[0] = gid;
    return user;
}

LITE_OS_SEC_TEXT BOOL LOS_CheckInGroups(UINT32 gid)
{
    UINT32 intSave;
    UINT32 count;
    User *user = NULL;

    SCHEDULER_LOCK(intSave);
    user = OsCurrUserGet();
    for (count = 0; count < user->groupNumber; count++) {
        if (user->groups[count] == gid) {
            SCHEDULER_UNLOCK(intSave);
            return TRUE;
        }
    }

    SCHEDULER_UNLOCK(intSave);
    return FALSE;
}
#endif

LITE_OS_SEC_TEXT INT32 LOS_GetUserID(VOID)
{
#ifdef LOSCFG_SECURITY_CAPABILITY
    UINT32 intSave;
    INT32 uid;

    SCHEDULER_LOCK(intSave);
    uid = (INT32)OsCurrUserGet()->userID;
    SCHEDULER_UNLOCK(intSave);
    return uid;
#else
    return 0;
#endif
}

LITE_OS_SEC_TEXT INT32 LOS_GetGroupID(VOID)
{
#ifdef LOSCFG_SECURITY_CAPABILITY
    UINT32 intSave;
    INT32 gid;

    SCHEDULER_LOCK(intSave);
    gid = (INT32)OsCurrUserGet()->gid;
    SCHEDULER_UNLOCK(intSave);

    return gid;
#else
    return 0;
#endif
}

STATIC UINT32 OsProcessCreateInit(LosProcessCB *processCB, UINT32 flags, const CHAR *name)
{
    ProcessGroup *group = NULL;
    UINT32 ret = OsInitPCB(processCB, flags, name);
    if (ret != LOS_OK) {
        goto EXIT;
    }

#ifdef LOSCFG_FS_VFS
    processCB->files = alloc_files();
    if (processCB->files == NULL) {
        ret = LOS_ENOMEM;
        goto EXIT;
    }
#endif

    group = OsCreateProcessGroup(processCB->processID);
    if (group == NULL) {
        ret = LOS_ENOMEM;
        goto EXIT;
    }

#ifdef LOSCFG_SECURITY_CAPABILITY
    processCB->user = OsCreateUser(0, 0, 1);
    if (processCB->user == NULL) {
        ret = LOS_ENOMEM;
        goto EXIT;
    }
#endif

    return LOS_OK;

EXIT:
    OsDeInitPCB(processCB);
    return ret;
}

LITE_OS_SEC_TEXT_INIT UINT32 OsSystemProcessCreate(VOID)
{
    UINT32 ret = OsProcessInit();
    if (ret != LOS_OK) {
        return ret;
    }

    LosProcessCB *kerInitProcess = OS_PCB_FROM_PID(g_kernelInitProcess);
    ret = OsProcessCreateInit(kerInitProcess, OS_KERNEL_MODE, "KProcess");
    if (ret != LOS_OK) {
        return ret;
    }

    kerInitProcess->processStatus &= ~OS_PROCESS_STATUS_INIT;
    g_processGroup = kerInitProcess->group;
    LOS_ListInit(&g_processGroup->groupList);

    LosProcessCB *idleProcess = OS_PCB_FROM_PID(g_kernelIdleProcess);
    ret = OsInitPCB(idleProcess, OS_KERNEL_MODE, "KIdle");
    if (ret != LOS_OK) {
        return ret;
    }
    idleProcess->parentProcessID = kerInitProcess->processID;
    LOS_ListTailInsert(&kerInitProcess->childrenList, &idleProcess->siblingList);
    idleProcess->group = kerInitProcess->group;
    LOS_ListTailInsert(&kerInitProcess->group->processList, &idleProcess->subordinateGroupList);
#ifdef LOSCFG_SECURITY_CAPABILITY
    idleProcess->user = kerInitProcess->user;
#endif
#ifdef LOSCFG_FS_VFS
    idleProcess->files = kerInitProcess->files;
#endif
    idleProcess->processStatus &= ~OS_PROCESS_STATUS_INIT;

    ret = OsIdleTaskCreate();
    if (ret != LOS_OK) {
        return ret;
    }
    idleProcess->threadGroupID = OsGetIdleTaskId();

    return LOS_OK;
}

STATIC INLINE INT32 OsProcessSchedlerParamCheck(INT32 which, INT32 pid, UINT16 prio, UINT16 policy)
{
    if (OS_PID_CHECK_INVALID(pid)) {
        return LOS_EINVAL;
    }

    if (which != LOS_PRIO_PROCESS) {
        return LOS_EINVAL;
    }

    if (prio > OS_PROCESS_PRIORITY_LOWEST) {
        return LOS_EINVAL;
    }

    if (policy != LOS_SCHED_RR) {
        return LOS_EINVAL;
    }

    return LOS_OK;
}

#ifdef LOSCFG_SECURITY_CAPABILITY
STATIC BOOL OsProcessCapPermitCheck(const LosProcessCB *processCB, UINT16 prio)
{
    LosProcessCB *runProcess = OsCurrProcessGet();

    /* always trust kernel process */
    if (!OsProcessIsUserMode(runProcess)) {
        return TRUE;
    }

    /* user mode process can reduce the priority of itself */
    if ((runProcess->processID == processCB->processID) && (prio > OsCurrTaskGet()->basePrio)) {
        return TRUE;
    }

    /* user mode process with privilege of CAP_SCHED_SETPRIORITY can change the priority */
    if (IsCapPermit(CAP_SCHED_SETPRIORITY)) {
        return TRUE;
    }
    return FALSE;
}
#endif

LITE_OS_SEC_TEXT INT32 OsSetProcessScheduler(INT32 which, INT32 pid, UINT16 prio, UINT16 policy)
{
    LosProcessCB *processCB = NULL;
    BOOL needSched = FALSE;
    UINT32 intSave;
    INT32 ret;

    ret = OsProcessSchedlerParamCheck(which, pid, prio, policy);
    if (ret != LOS_OK) {
        return -ret;
    }

    SCHEDULER_LOCK(intSave);
    processCB = OS_PCB_FROM_PID(pid);
    if (OsProcessIsInactive(processCB)) {
        ret = LOS_ESRCH;
        goto EXIT;
    }

#ifdef LOSCFG_SECURITY_CAPABILITY
    if (!OsProcessCapPermitCheck(processCB, prio)) {
        ret = LOS_EPERM;
        goto EXIT;
    }
#endif

    needSched = OsSchedModifyProcessSchedParam(pid, policy, prio);
    SCHEDULER_UNLOCK(intSave);

    LOS_MpSchedule(OS_MP_CPU_ALL);
    if (needSched && OS_SCHEDULER_ACTIVE) {
        LOS_Schedule();
    }
    return LOS_OK;

EXIT:
    SCHEDULER_UNLOCK(intSave);
    return -ret;
}

LITE_OS_SEC_TEXT INT32 LOS_SetProcessScheduler(INT32 pid, UINT16 policy, UINT16 prio)
{
    return OsSetProcessScheduler(LOS_PRIO_PROCESS, pid, prio, policy);
}

LITE_OS_SEC_TEXT INT32 LOS_GetProcessScheduler(INT32 pid)
{
    UINT32 intSave;

    if (OS_PID_CHECK_INVALID(pid)) {
        return -LOS_EINVAL;
    }

    SCHEDULER_LOCK(intSave);
    LosProcessCB *processCB = OS_PCB_FROM_PID(pid);
    if (OsProcessIsUnused(processCB)) {
        SCHEDULER_UNLOCK(intSave);
        return -LOS_ESRCH;
    }

    SCHEDULER_UNLOCK(intSave);

    return LOS_SCHED_RR;
}

LITE_OS_SEC_TEXT INT32 LOS_SetProcessPriority(INT32 pid, UINT16 prio)
{
    return OsSetProcessScheduler(LOS_PRIO_PROCESS, pid, prio, LOS_GetProcessScheduler(pid));
}

LITE_OS_SEC_TEXT INT32 OsGetProcessPriority(INT32 which, INT32 pid)
{
    INT32 prio;
    UINT32 intSave;
    (VOID)which;

    if (OS_PID_CHECK_INVALID(pid)) {
        return -LOS_EINVAL;
    }

    if (which != LOS_PRIO_PROCESS) {
        return -LOS_EINVAL;
    }

    LosProcessCB *processCB = OS_PCB_FROM_PID(pid);
    SCHEDULER_LOCK(intSave);
    if (OsProcessIsUnused(processCB)) {
        prio = -LOS_ESRCH;
        goto OUT;
    }

    prio = (INT32)OS_TCB_FROM_TID(processCB->threadGroupID)->basePrio;

OUT:
    SCHEDULER_UNLOCK(intSave);
    return prio;
}

LITE_OS_SEC_TEXT INT32 LOS_GetProcessPriority(INT32 pid)
{
    return OsGetProcessPriority(LOS_PRIO_PROCESS, pid);
}

STATIC VOID OsWaitInsertWaitListInOrder(LosTaskCB *runTask, LosProcessCB *processCB)
{
    LOS_DL_LIST *head = &processCB->waitList;
    LOS_DL_LIST *list = head;
    LosTaskCB *taskCB = NULL;

    if (runTask->waitFlag == OS_PROCESS_WAIT_GID) {
        while (list->pstNext != head) {
            taskCB = OS_TCB_FROM_PENDLIST(LOS_DL_LIST_FIRST(list));
            if (taskCB->waitFlag == OS_PROCESS_WAIT_PRO) {
                list = list->pstNext;
                continue;
            }
            break;
        }
    } else if (runTask->waitFlag == OS_PROCESS_WAIT_ANY) {
        while (list->pstNext != head) {
            taskCB = OS_TCB_FROM_PENDLIST(LOS_DL_LIST_FIRST(list));
            if (taskCB->waitFlag != OS_PROCESS_WAIT_ANY) {
                list = list->pstNext;
                continue;
            }
            break;
        }
    }
    /* if runTask->waitFlag == OS_PROCESS_WAIT_PRO,
     * this node is inserted directly into the header of the waitList
     */

    (VOID)OsSchedTaskWait(list->pstNext, LOS_WAIT_FOREVER, TRUE);
    return;
}

STATIC UINT32 OsWaitSetFlag(const LosProcessCB *processCB, INT32 pid, LosProcessCB **child)
{
    LosProcessCB *childCB = NULL;
    ProcessGroup *group = NULL;
    LosTaskCB *runTask = OsCurrTaskGet();
    UINT32 ret;

    if (pid > 0) {
        /* Wait for the child process whose process number is pid. */
        childCB = OsFindExitChildProcess(processCB, pid);
        if (childCB != NULL) {
            goto WAIT_BACK;
        }

        ret = OsFindChildProcess(processCB, pid);
        if (ret != LOS_OK) {
            return LOS_ECHILD;
        }
        runTask->waitFlag = OS_PROCESS_WAIT_PRO;
        runTask->waitID = pid;
    } else if (pid == 0) {
        /* Wait for any child process in the same process group */
        childCB = OsFindGroupExitProcess(processCB->group, OS_INVALID_VALUE);
        if (childCB != NULL) {
            goto WAIT_BACK;
        }
        runTask->waitID = processCB->group->groupID;
        runTask->waitFlag = OS_PROCESS_WAIT_GID;
    } else if (pid == -1) {
        /* Wait for any child process */
        childCB = OsFindExitChildProcess(processCB, OS_INVALID_VALUE);
        if (childCB != NULL) {
            goto WAIT_BACK;
        }
        runTask->waitID = pid;
        runTask->waitFlag = OS_PROCESS_WAIT_ANY;
    } else { /* pid < -1 */
        /* Wait for any child process whose group number is the pid absolute value. */
        group = OsFindProcessGroup(-pid);
        if (group == NULL) {
            return LOS_ECHILD;
        }

        childCB = OsFindGroupExitProcess(group, OS_INVALID_VALUE);
        if (childCB != NULL) {
            goto WAIT_BACK;
        }

        runTask->waitID = -pid;
        runTask->waitFlag = OS_PROCESS_WAIT_GID;
    }

WAIT_BACK:
    *child = childCB;
    return LOS_OK;
}

STATIC UINT32 OsWaitRecycleChildProcess(const LosProcessCB *childCB, UINT32 intSave, INT32 *status, siginfo_t *info)
{
    ProcessGroup *group = NULL;
    UINT32 pid = childCB->processID;
    UINT16 mode = childCB->processMode;
    INT32 exitCode = childCB->exitCode;
    UINT32 uid = 0;

#ifdef LOSCFG_SECURITY_CAPABILITY
    if (childCB->user != NULL) {
        uid = childCB->user->userID;
    }
#endif

    OsRecycleZombiesProcess((LosProcessCB *)childCB, &group);
    SCHEDULER_UNLOCK(intSave);

    if (status != NULL) {
        if (mode == OS_USER_MODE) {
            (VOID)LOS_ArchCopyToUser((VOID *)status, (const VOID *)(&(exitCode)), sizeof(INT32));
        } else {
            *status = exitCode;
        }
    }
    /* get signal info */
    if (info != NULL) {
        siginfo_t tempinfo = { 0 };

        tempinfo.si_signo = SIGCHLD;
        tempinfo.si_errno = 0;
        tempinfo.si_pid = pid;
        tempinfo.si_uid = uid;
        /*
         * Process exit code
         * 31	 15 		  8 		  7 	   0
         * |	 | exit code  | core dump | signal |
         */
        if ((exitCode & 0x7f) == 0) {
            tempinfo.si_code = CLD_EXITED;
            tempinfo.si_status = (exitCode >> 8U);
        } else {
            tempinfo.si_code = (exitCode & 0x80) ? CLD_DUMPED : CLD_KILLED;
            tempinfo.si_status = (exitCode & 0x7f);
        }

        if (mode == OS_USER_MODE) {
            (VOID)LOS_ArchCopyToUser((VOID *)(info), (const VOID *)(&(tempinfo)), sizeof(siginfo_t));
        } else {
            (VOID)memcpy_s((VOID *)(info), sizeof(siginfo_t), (const VOID *)(&(tempinfo)), sizeof(siginfo_t));
        }
    }
    (VOID)LOS_MemFree(m_aucSysMem1, group);
    return pid;
}

STATIC UINT32 OsWaitChildProcessCheck(LosProcessCB *processCB, INT32 pid, LosProcessCB **childCB)
{
    if (LOS_ListEmpty(&(processCB->childrenList)) && LOS_ListEmpty(&(processCB->exitChildList))) {
        return LOS_ECHILD;
    }

    return OsWaitSetFlag(processCB, pid, childCB);
}

STATIC UINT32 OsWaitOptionsCheck(UINT32 options)
{
    UINT32 flag = LOS_WAIT_WNOHANG | LOS_WAIT_WUNTRACED | LOS_WAIT_WCONTINUED;

    flag = ~flag & options;
    if (flag != 0) {
        return LOS_EINVAL;
    }

    if ((options & (LOS_WAIT_WCONTINUED | LOS_WAIT_WUNTRACED)) != 0) {
        return LOS_EOPNOTSUPP;
    }

    if (OS_INT_ACTIVE) {
        return LOS_EINTR;
    }

    return LOS_OK;
}

STATIC INT32 OsWait(INT32 pid, USER INT32 *status, USER siginfo_t *info, UINT32 options, VOID *rusage)
{
    (VOID)rusage;
    UINT32 ret;
    UINT32 intSave;
    LosProcessCB *childCB = NULL;
    LosProcessCB *processCB = NULL;
    LosTaskCB *runTask = NULL;

    SCHEDULER_LOCK(intSave);
    processCB = OsCurrProcessGet();
    runTask = OsCurrTaskGet();

    ret = OsWaitChildProcessCheck(processCB, pid, &childCB);
    if (ret != LOS_OK) {
        pid = -ret;
        goto ERROR;
    }

    if (childCB != NULL) {
        return (INT32)OsWaitRecycleChildProcess(childCB, intSave, status, info);
    }

    if ((options & LOS_WAIT_WNOHANG) != 0) {
        runTask->waitFlag = 0;
        pid = 0;
        goto ERROR;
    }

    OsWaitInsertWaitListInOrder(runTask, processCB);

    runTask->waitFlag = 0;
    if (runTask->waitID == OS_INVALID_VALUE) {
        pid = -LOS_ECHILD;
        goto ERROR;
    }

    childCB = OS_PCB_FROM_PID(runTask->waitID);
    if (!(childCB->processStatus & OS_PROCESS_STATUS_ZOMBIES)) {
        pid = -LOS_ESRCH;
        goto ERROR;
    }

    return (INT32)OsWaitRecycleChildProcess(childCB, intSave, status, info);

ERROR:
    SCHEDULER_UNLOCK(intSave);
    return pid;
}

LITE_OS_SEC_TEXT INT32 LOS_Wait(INT32 pid, USER INT32 *status, UINT32 options, VOID *rusage)
{
    (VOID)rusage;
    UINT32 ret;

    ret = OsWaitOptionsCheck(options);
    if (ret != LOS_OK) {
        return -ret;
    }

    return OsWait(pid, status, NULL, options, NULL);
}

STATIC UINT32 OsWaitidOptionsCheck(UINT32 options)
{
    UINT32 flag = LOS_WAIT_WNOHANG | LOS_WAIT_WSTOPPED | LOS_WAIT_WCONTINUED | LOS_WAIT_WEXITED | LOS_WAIT_WNOWAIT;

    flag = ~flag & options;
    if ((flag != 0) || (options == 0)) {
        return LOS_EINVAL;
    }

    /*
     * only support LOS_WAIT_WNOHANG | LOS_WAIT_WEXITED
     * notsupport LOS_WAIT_WSTOPPED | LOS_WAIT_WCONTINUED | LOS_WAIT_WNOWAIT
     */
    if ((options & (LOS_WAIT_WSTOPPED | LOS_WAIT_WCONTINUED | LOS_WAIT_WNOWAIT)) != 0) {
        return LOS_EOPNOTSUPP;
    }

    if (OS_INT_ACTIVE) {
        return LOS_EINTR;
    }

    return LOS_OK;
}

LITE_OS_SEC_TEXT INT32 LOS_Waitid(INT32 pid, USER siginfo_t *info, UINT32 options, VOID *rusage)
{
    (VOID)rusage;
    UINT32 ret;

    /* check options value */
    ret = OsWaitidOptionsCheck(options);
    if (ret != LOS_OK) {
        return -ret;
    }

    return OsWait(pid, NULL, info, options, NULL);
}

STATIC UINT32 OsSetProcessGroupCheck(const LosProcessCB *processCB, UINT32 gid)
{
    LosProcessCB *runProcessCB = OsCurrProcessGet();
    LosProcessCB *groupProcessCB = OS_PCB_FROM_PID(gid);

    if (OsProcessIsInactive(processCB)) {
        return LOS_ESRCH;
    }

    if (!OsProcessIsUserMode(processCB) || !OsProcessIsUserMode(groupProcessCB)) {
        return LOS_EPERM;
    }

    if (runProcessCB->processID == processCB->parentProcessID) {
        if (processCB->processStatus & OS_PROCESS_FLAG_ALREADY_EXEC) {
            return LOS_EACCES;
        }
    } else if (processCB->processID != runProcessCB->processID) {
        return LOS_ESRCH;
    }

    /* Add the process to another existing process group */
    if (processCB->processID != gid) {
        if (!(groupProcessCB->processStatus & OS_PROCESS_FLAG_GROUP_LEADER)) {
            return LOS_EPERM;
        }

        if ((groupProcessCB->parentProcessID != processCB->parentProcessID) && (gid != processCB->parentProcessID)) {
            return LOS_EPERM;
        }
    }

    return LOS_OK;
}

STATIC UINT32 OsSetProcessGroupIDUnsafe(UINT32 pid, UINT32 gid, ProcessGroup **group)
{
    ProcessGroup *oldGroup = NULL;
    ProcessGroup *newGroup = NULL;
    LosProcessCB *processCB = OS_PCB_FROM_PID(pid);
    UINT32 ret = OsSetProcessGroupCheck(processCB, gid);
    if (ret != LOS_OK) {
        return ret;
    }

    if (processCB->group->groupID == gid) {
        return LOS_OK;
    }

    oldGroup = processCB->group;
    OsExitProcessGroup(processCB, group);

    newGroup = OsFindProcessGroup(gid);
    if (newGroup != NULL) {
        LOS_ListTailInsert(&newGroup->processList, &processCB->subordinateGroupList);
        processCB->group = newGroup;
        return LOS_OK;
    }

    newGroup = OsCreateProcessGroup(gid);
    if (newGroup == NULL) {
        LOS_ListTailInsert(&oldGroup->processList, &processCB->subordinateGroupList);
        processCB->group = oldGroup;
        if (*group != NULL) {
            LOS_ListTailInsert(&g_processGroup->groupList, &oldGroup->groupList);
            processCB = OS_PCB_FROM_PID(oldGroup->groupID);
            processCB->processStatus |= OS_PROCESS_FLAG_GROUP_LEADER;
            *group = NULL;
        }
        return LOS_EPERM;
    }
    return LOS_OK;
}

LITE_OS_SEC_TEXT INT32 OsSetProcessGroupID(UINT32 pid, UINT32 gid)
{
    ProcessGroup *group = NULL;
    UINT32 ret;
    UINT32 intSave;

    if ((OS_PID_CHECK_INVALID(pid)) || (OS_PID_CHECK_INVALID(gid))) {
        return -LOS_EINVAL;
    }

    SCHEDULER_LOCK(intSave);
    ret = OsSetProcessGroupIDUnsafe(pid, gid, &group);
    SCHEDULER_UNLOCK(intSave);
    (VOID)LOS_MemFree(m_aucSysMem1, group);
    return -ret;
}

LITE_OS_SEC_TEXT INT32 OsSetCurrProcessGroupID(UINT32 gid)
{
    return OsSetProcessGroupID(OsCurrProcessGet()->processID, gid);
}

LITE_OS_SEC_TEXT INT32 LOS_GetProcessGroupID(UINT32 pid)
{
    INT32 gid;
    UINT32 intSave;
    LosProcessCB *processCB = NULL;

    if (OS_PID_CHECK_INVALID(pid)) {
        return -LOS_EINVAL;
    }

    SCHEDULER_LOCK(intSave);
    processCB = OS_PCB_FROM_PID(pid);
    if (OsProcessIsUnused(processCB)) {
        gid = -LOS_ESRCH;
        goto EXIT;
    }

    gid = (INT32)processCB->group->groupID;

EXIT:
    SCHEDULER_UNLOCK(intSave);
    return gid;
}

LITE_OS_SEC_TEXT INT32 LOS_GetCurrProcessGroupID(VOID)
{
    return LOS_GetProcessGroupID(OsCurrProcessGet()->processID);
}

#ifdef LOSCFG_KERNEL_VM
STATIC LosProcessCB *OsGetFreePCB(VOID)
{
    LosProcessCB *processCB = NULL;
    UINT32 intSave;

    SCHEDULER_LOCK(intSave);
    if (LOS_ListEmpty(&g_freeProcess)) {
        SCHEDULER_UNLOCK(intSave);
        PRINT_ERR("No idle PCB in the system!\n");
        return NULL;
    }

    processCB = OS_PCB_FROM_PENDLIST(LOS_DL_LIST_FIRST(&g_freeProcess));
    LOS_ListDelete(&processCB->pendList);
    SCHEDULER_UNLOCK(intSave);

    return processCB;
}

STATIC VOID *OsUserInitStackAlloc(LosProcessCB *processCB, UINT32 *size)
{
    LosVmMapRegion *region = NULL;
    UINT32 stackSize = ALIGN(OS_USER_TASK_STACK_SIZE, PAGE_SIZE);

    region = LOS_RegionAlloc(processCB->vmSpace, 0, stackSize,
                             VM_MAP_REGION_FLAG_PERM_USER | VM_MAP_REGION_FLAG_PERM_READ |
                             VM_MAP_REGION_FLAG_PERM_WRITE, 0);
    if (region == NULL) {
        return NULL;
    }

    LOS_SetRegionTypeAnon(region);
    region->regionFlags |= VM_MAP_REGION_FLAG_STACK;

    *size = stackSize;

    return (VOID *)(UINTPTR)region->range.base;
}

#ifdef LOSCFG_KERNEL_DYNLOAD
LITE_OS_SEC_TEXT VOID OsExecProcessVmSpaceRestore(LosVmSpace *oldSpace)
{
    LosProcessCB *processCB = OsCurrProcessGet();
    LosTaskCB *runTask = OsCurrTaskGet();

    processCB->vmSpace = oldSpace;
    runTask->archMmu = (UINTPTR)&processCB->vmSpace->archMmu;
    LOS_ArchMmuContextSwitch((LosArchMmu *)runTask->archMmu);
}

LITE_OS_SEC_TEXT LosVmSpace *OsExecProcessVmSpaceReplace(LosVmSpace *newSpace, UINTPTR stackBase, INT32 randomDevFD)
{
    LosProcessCB *processCB = OsCurrProcessGet();
    LosTaskCB *runTask = OsCurrTaskGet();

    OsProcessThreadGroupDestroy();
    OsTaskCBRecycleToFree();

    LosVmSpace *oldSpace = processCB->vmSpace;
    processCB->vmSpace = newSpace;
    processCB->vmSpace->heapBase += OsGetRndOffset(randomDevFD);
    processCB->vmSpace->heapNow = processCB->vmSpace->heapBase;
    processCB->vmSpace->mapBase += OsGetRndOffset(randomDevFD);
    processCB->vmSpace->mapSize = stackBase - processCB->vmSpace->mapBase;
    runTask->archMmu = (UINTPTR)&processCB->vmSpace->archMmu;
    LOS_ArchMmuContextSwitch((LosArchMmu *)runTask->archMmu);
    return oldSpace;
}

LITE_OS_SEC_TEXT UINT32 OsExecRecycleAndInit(LosProcessCB *processCB, const CHAR *name,
                                             LosVmSpace *oldSpace, UINTPTR oldFiles)
{
    UINT32 ret;
    const CHAR *processName = NULL;

    if ((processCB == NULL) || (name == NULL)) {
        return LOS_NOK;
    }

    processName = strrchr(name, '/');
    processName = (processName == NULL) ? name : (processName + 1); /* 1: Do not include '/' */

    ret = (UINT32)OsSetTaskName(OsCurrTaskGet(), processName, TRUE);
    if (ret != LOS_OK) {
        return ret;
    }

#ifdef LOSCFG_KERNEL_LITEIPC
    (VOID)LiteIpcPoolDestroy(processCB->processID);
#endif

    processCB->sigHandler = 0;
    OsCurrTaskGet()->sig.sigprocmask = 0;

    LOS_VmSpaceFree(oldSpace);
#ifdef LOSCFG_FS_VFS
    CloseOnExec((struct files_struct *)oldFiles);
    delete_files_snapshot((struct files_struct *)oldFiles);
#endif

    OsSwtmrRecycle(processCB->processID);
    processCB->timerID = (timer_t)(UINTPTR)MAX_INVALID_TIMER_VID;

#ifdef LOSCFG_SECURITY_VID
    VidMapDestroy(processCB);
    ret = VidMapListInit(processCB);
    if (ret != LOS_OK) {
        return LOS_NOK;
    }
#endif

    processCB->processStatus &= ~OS_PROCESS_FLAG_EXIT;
    processCB->processStatus |= OS_PROCESS_FLAG_ALREADY_EXEC;

    return LOS_OK;
}

LITE_OS_SEC_TEXT UINT32 OsExecStart(const TSK_ENTRY_FUNC entry, UINTPTR sp, UINTPTR mapBase, UINT32 mapSize)
{
    UINT32 intSave;

    if (entry == NULL) {
        return LOS_NOK;
    }

    if ((sp == 0) || (LOS_Align(sp, LOSCFG_STACK_POINT_ALIGN_SIZE) != sp)) {
        return LOS_NOK;
    }

    if ((mapBase == 0) || (mapSize == 0) || (sp <= mapBase) || (sp > (mapBase + mapSize))) {
        return LOS_NOK;
    }

    LosTaskCB *taskCB = OsCurrTaskGet();

    SCHEDULER_LOCK(intSave);
    taskCB->userMapBase = mapBase;
    taskCB->userMapSize = mapSize;
    taskCB->taskEntry = (TSK_ENTRY_FUNC)entry;

    TaskContext *taskContext = (TaskContext *)OsTaskStackInit(taskCB->taskID, taskCB->stackSize,
                                                              (VOID *)taskCB->topOfStack, FALSE);
    OsUserTaskStackInit(taskContext, (UINTPTR)taskCB->taskEntry, sp);
    SCHEDULER_UNLOCK(intSave);
    return LOS_OK;
}
#endif

STATIC UINT32 OsUserInitProcessStart(LosProcessCB *processCB, TSK_INIT_PARAM_S *param)
{
    UINT32 intSave;
    INT32 ret;

    UINT32 taskID = OsCreateUserTask(processCB->processID, param);
    if (taskID == OS_INVALID_VALUE) {
        return LOS_NOK;
    }

    ret = LOS_SetProcessPriority(processCB->processID, OS_PROCESS_USERINIT_PRIORITY);
    if (ret != LOS_OK) {
        PRINT_ERR("User init process set priority failed! ERROR:%d \n", ret);
        goto EXIT;
    }

    SCHEDULER_LOCK(intSave);
    processCB->processStatus &= ~OS_PROCESS_STATUS_INIT;
    SCHEDULER_UNLOCK(intSave);

    ret = LOS_SetTaskScheduler(taskID, LOS_SCHED_RR, OS_TASK_PRIORITY_LOWEST);
    if (ret != LOS_OK) {
        PRINT_ERR("User init process set scheduler failed! ERROR:%d \n", ret);
        goto EXIT;
    }

    return LOS_OK;

EXIT:
    (VOID)LOS_TaskDelete(taskID);
    return ret;
}

STATIC UINT32 OsLoadUserInit(LosProcessCB *processCB)
{
    /*              userInitTextStart               -----
     * | user text |
     *
     * | user data |                                initSize
     *              userInitBssStart  ---
     * | user bss  |                  initBssSize
     *              userInitEnd       ---           -----
     */
    errno_t errRet;
    INT32 ret;
    CHAR *userInitTextStart = (CHAR *)&__user_init_entry;
    CHAR *userInitBssStart = (CHAR *)&__user_init_bss;
    CHAR *userInitEnd = (CHAR *)&__user_init_end;
    UINT32 initBssSize = userInitEnd - userInitBssStart;
    UINT32 initSize = userInitEnd - userInitTextStart;
    VOID *userBss = NULL;
    VOID *userText = NULL;

    if ((LOS_Align((UINTPTR)userInitTextStart, PAGE_SIZE) != (UINTPTR)userInitTextStart) ||
        (LOS_Align((UINTPTR)userInitEnd, PAGE_SIZE) != (UINTPTR)userInitEnd)) {
        return LOS_EINVAL;
    }

    if ((initSize == 0) || (initSize <= initBssSize)) {
        return LOS_EINVAL;
    }

    userText = LOS_PhysPagesAllocContiguous(initSize >> PAGE_SHIFT);
    if (userText == NULL) {
        return LOS_NOK;
    }

    errRet = memcpy_s(userText, initSize, (VOID *)&__user_init_load_addr, initSize - initBssSize);
    if (errRet != EOK) {
        PRINT_ERR("Load user init text, data and bss failed! err : %d\n", errRet);
        goto ERROR;
    }
    ret = LOS_VaddrToPaddrMmap(processCB->vmSpace, (VADDR_T)(UINTPTR)userInitTextStart, LOS_PaddrQuery(userText),
                               initSize, VM_MAP_REGION_FLAG_PERM_READ | VM_MAP_REGION_FLAG_PERM_WRITE |
                               VM_MAP_REGION_FLAG_FIXED | VM_MAP_REGION_FLAG_PERM_EXECUTE |
                               VM_MAP_REGION_FLAG_PERM_USER);
    if (ret < 0) {
        PRINT_ERR("Mmap user init text, data and bss failed! err : %d\n", ret);
        goto ERROR;
    }

    /* The User init boot segment may not actually exist */
    if (initBssSize != 0) {
        userBss = (VOID *)((UINTPTR)userText + userInitBssStart - userInitTextStart);
        errRet = memset_s(userBss, initBssSize, 0, initBssSize);
        if (errRet != EOK) {
            PRINT_ERR("memset user init bss failed! err : %d\n", errRet);
            goto ERROR;
        }
    }

    return LOS_OK;

ERROR:
    (VOID)LOS_PhysPagesFreeContiguous(userText, initSize >> PAGE_SHIFT);
    return LOS_NOK;
}

LITE_OS_SEC_TEXT_INIT UINT32 OsUserInitProcess(VOID)
{
    UINT32 ret;
    UINT32 size;
    TSK_INIT_PARAM_S param = { 0 };
    VOID *stack = NULL;

    LosProcessCB *processCB = OS_PCB_FROM_PID(g_userInitProcess);
    ret = OsProcessCreateInit(processCB, OS_USER_MODE, "Init");
    if (ret != LOS_OK) {
        return ret;
    }

    ret = OsLoadUserInit(processCB);
    if (ret != LOS_OK) {
        goto ERROR;
    }

    stack = OsUserInitStackAlloc(processCB, &size);
    if (stack == NULL) {
        PRINT_ERR("Alloc user init process user stack failed!\n");
        goto ERROR;
    }

    param.pfnTaskEntry = (TSK_ENTRY_FUNC)(CHAR *)&__user_init_entry;
    param.userParam.userSP = (UINTPTR)stack + size;
    param.userParam.userMapBase = (UINTPTR)stack;
    param.userParam.userMapSize = size;
    param.uwResved = OS_TASK_FLAG_PTHREAD_JOIN;
    ret = OsUserInitProcessStart(processCB, &param);
    if (ret != LOS_OK) {
        (VOID)OsUnMMap(processCB->vmSpace, param.userParam.userMapBase, param.userParam.userMapSize);
        goto ERROR;
    }

    return LOS_OK;

ERROR:
    OsDeInitPCB(processCB);
    return ret;
}

STATIC UINT32 OsCopyUser(LosProcessCB *childCB, LosProcessCB *parentCB)
{
#ifdef LOSCFG_SECURITY_CAPABILITY
    UINT32 size = sizeof(User) + sizeof(UINT32) * (parentCB->user->groupNumber - 1);
    childCB->user = LOS_MemAlloc(m_aucSysMem1, size);
    if (childCB->user == NULL) {
        return LOS_ENOMEM;
    }

    (VOID)memcpy_s(childCB->user, size, parentCB->user, size);
#endif
    return LOS_OK;
}

STATIC VOID OsInitCopyTaskParam(LosProcessCB *childProcessCB, const CHAR *name, UINTPTR entry, UINT32 size,
                                TSK_INIT_PARAM_S *childPara)
{
    LosTaskCB *mainThread = NULL;
    UINT32 intSave;

    SCHEDULER_LOCK(intSave);
    mainThread = OsCurrTaskGet();

    if (OsProcessIsUserMode(childProcessCB)) {
        childPara->pfnTaskEntry = mainThread->taskEntry;
        childPara->uwStackSize = mainThread->stackSize;
        childPara->userParam.userArea = mainThread->userArea;
        childPara->userParam.userMapBase = mainThread->userMapBase;
        childPara->userParam.userMapSize = mainThread->userMapSize;
    } else {
        childPara->pfnTaskEntry = (TSK_ENTRY_FUNC)entry;
        childPara->uwStackSize = size;
    }
    childPara->pcName = (CHAR *)name;
    childPara->policy = mainThread->policy;
    childPara->usTaskPrio = mainThread->priority;
    childPara->processID = childProcessCB->processID;
    if (mainThread->taskStatus & OS_TASK_FLAG_PTHREAD_JOIN) {
        childPara->uwResved = LOS_TASK_ATTR_JOINABLE;
    }

    SCHEDULER_UNLOCK(intSave);
}

STATIC UINT32 OsCopyTask(UINT32 flags, LosProcessCB *childProcessCB, const CHAR *name, UINTPTR entry, UINT32 size)
{
    LosTaskCB *runTask = OsCurrTaskGet();
    TSK_INIT_PARAM_S childPara = { 0 };
    UINT32 ret;
    UINT32 intSave;
    UINT32 taskID;

    OsInitCopyTaskParam(childProcessCB, name, entry, size, &childPara);

    ret = LOS_TaskCreateOnly(&taskID, &childPara);
    if (ret != LOS_OK) {
        if (ret == LOS_ERRNO_TSK_TCB_UNAVAILABLE) {
            return LOS_EAGAIN;
        }
        return LOS_ENOMEM;
    }

    LosTaskCB *childTaskCB = OS_TCB_FROM_TID(taskID);
    childTaskCB->taskStatus = runTask->taskStatus;
    childTaskCB->basePrio = runTask->basePrio;
    if (childTaskCB->taskStatus & OS_TASK_STATUS_RUNNING) {
        childTaskCB->taskStatus &= ~OS_TASK_STATUS_RUNNING;
    } else {
        if (OS_SCHEDULER_ACTIVE) {
            LOS_Panic("Clone thread status not running error status: 0x%x\n", childTaskCB->taskStatus);
        }
        childTaskCB->taskStatus &= ~OS_TASK_STATUS_UNUSED;
    }

    if (OsProcessIsUserMode(childProcessCB)) {
        SCHEDULER_LOCK(intSave);
        OsUserCloneParentStack(childTaskCB->stackPointer, runTask->topOfStack, runTask->stackSize);
        SCHEDULER_UNLOCK(intSave);
    }
    return LOS_OK;
}

STATIC UINT32 OsCopyParent(UINT32 flags, LosProcessCB *childProcessCB, LosProcessCB *runProcessCB)
{
    UINT32 ret;
    UINT32 intSave;
    LosProcessCB *parentProcessCB = NULL;

    SCHEDULER_LOCK(intSave);

    if (flags & CLONE_PARENT) {
        parentProcessCB = OS_PCB_FROM_PID(runProcessCB->parentProcessID);
    } else {
        parentProcessCB = runProcessCB;
    }
    childProcessCB->parentProcessID = parentProcessCB->processID;
    LOS_ListTailInsert(&parentProcessCB->childrenList, &childProcessCB->siblingList);
    childProcessCB->group = parentProcessCB->group;
    LOS_ListTailInsert(&parentProcessCB->group->processList, &childProcessCB->subordinateGroupList);
    ret = OsCopyUser(childProcessCB, parentProcessCB);

    SCHEDULER_UNLOCK(intSave);
    return ret;
}

STATIC UINT32 OsCopyMM(UINT32 flags, LosProcessCB *childProcessCB, LosProcessCB *runProcessCB)
{
    status_t status;
    UINT32 intSave;

    if (!OsProcessIsUserMode(childProcessCB)) {
        return LOS_OK;
    }

    if (flags & CLONE_VM) {
        SCHEDULER_LOCK(intSave);
        childProcessCB->vmSpace->archMmu.virtTtb = runProcessCB->vmSpace->archMmu.virtTtb;
        childProcessCB->vmSpace->archMmu.physTtb = runProcessCB->vmSpace->archMmu.physTtb;
        SCHEDULER_UNLOCK(intSave);
        return LOS_OK;
    }

    status = LOS_VmSpaceClone(runProcessCB->vmSpace, childProcessCB->vmSpace);
    if (status != LOS_OK) {
        return LOS_ENOMEM;
    }
    return LOS_OK;
}

STATIC UINT32 OsCopyFile(UINT32 flags, LosProcessCB *childProcessCB, LosProcessCB *runProcessCB)
{
#ifdef LOSCFG_FS_VFS
    if (flags & CLONE_FILES) {
        childProcessCB->files = runProcessCB->files;
    } else {
        childProcessCB->files = dup_fd(runProcessCB->files);
    }
    if (childProcessCB->files == NULL) {
        return LOS_ENOMEM;
    }
#endif

    childProcessCB->consoleID = runProcessCB->consoleID;
    childProcessCB->umask = runProcessCB->umask;
    return LOS_OK;
}

STATIC UINT32 OsForkInitPCB(UINT32 flags, LosProcessCB *child, const CHAR *name, UINTPTR sp, UINT32 size)
{
    UINT32 ret;
    LosProcessCB *run = OsCurrProcessGet();

    ret = OsInitPCB(child, run->processMode, name);
    if (ret != LOS_OK) {
        return ret;
    }

    ret = OsCopyParent(flags, child, run);
    if (ret != LOS_OK) {
        return ret;
    }

    return OsCopyTask(flags, child, name, sp, size);
}

STATIC UINT32 OsChildSetProcessGroupAndSched(LosProcessCB *child, LosProcessCB *run)
{
    UINT32 intSave;
    UINT32 ret;
    ProcessGroup *group = NULL;

    SCHEDULER_LOCK(intSave);
    if (run->group->groupID == OS_USER_PRIVILEGE_PROCESS_GROUP) {
        ret = OsSetProcessGroupIDUnsafe(child->processID, child->processID, &group);
        if (ret != LOS_OK) {
            SCHEDULER_UNLOCK(intSave);
            return LOS_ENOMEM;
        }
    }

    child->processStatus &= ~OS_PROCESS_STATUS_INIT;
    OsSchedTaskEnQueue(OS_TCB_FROM_TID(child->threadGroupID));
    SCHEDULER_UNLOCK(intSave);

    (VOID)LOS_MemFree(m_aucSysMem1, group);
    return LOS_OK;
}

STATIC UINT32 OsCopyProcessResources(UINT32 flags, LosProcessCB *child, LosProcessCB *run)
{
    UINT32 ret;

    ret = OsCopyMM(flags, child, run);
    if (ret != LOS_OK) {
        return ret;
    }

    ret = OsCopyFile(flags, child, run);
    if (ret != LOS_OK) {
        return ret;
    }

#ifdef LOSCFG_KERNEL_LITEIPC
    if (run->ipcInfo != NULL) {
        child->ipcInfo = LiteIpcPoolReInit((const ProcIpcInfo *)(run->ipcInfo));
        if (child->ipcInfo == NULL) {
            return LOS_ENOMEM;
        }
    }
#endif

#ifdef LOSCFG_SECURITY_CAPABILITY
    OsCopyCapability(run, child);
#endif

    return LOS_OK;
}

STATIC INT32 OsCopyProcess(UINT32 flags, const CHAR *name, UINTPTR sp, UINT32 size)
{
    UINT32 ret, processID;
    LosProcessCB *run = OsCurrProcessGet();

    LosProcessCB *child = OsGetFreePCB();
    if (child == NULL) {
        return -LOS_EAGAIN;
    }
    processID = child->processID;

    ret = OsForkInitPCB(flags, child, name, sp, size);
    if (ret != LOS_OK) {
        goto ERROR_INIT;
    }

    ret = OsCopyProcessResources(flags, child, run);
    if (ret != LOS_OK) {
        goto ERROR_TASK;
    }

    ret = OsChildSetProcessGroupAndSched(child, run);
    if (ret != LOS_OK) {
        goto ERROR_TASK;
    }

    LOS_MpSchedule(OS_MP_CPU_ALL);
    if (OS_SCHEDULER_ACTIVE) {
        LOS_Schedule();
    }

    return processID;

ERROR_TASK:
    (VOID)LOS_TaskDelete(child->threadGroupID);
ERROR_INIT:
    OsDeInitPCB(child);
    return -ret;
}

LITE_OS_SEC_TEXT INT32 OsClone(UINT32 flags, UINTPTR sp, UINT32 size)
{
    UINT32 cloneFlag = CLONE_PARENT | CLONE_THREAD | CLONE_VFORK | CLONE_VM;

    if (flags & (~cloneFlag)) {
        PRINT_WARN("Clone dont support some flags!\n");
    }

    return OsCopyProcess(cloneFlag & flags, NULL, sp, size);
}

LITE_OS_SEC_TEXT INT32 LOS_Fork(UINT32 flags, const CHAR *name, const TSK_ENTRY_FUNC entry, UINT32 stackSize)
{
    UINT32 cloneFlag = CLONE_PARENT | CLONE_THREAD | CLONE_VFORK | CLONE_FILES;

    if (flags & (~cloneFlag)) {
        PRINT_WARN("Clone dont support some flags!\n");
    }

    flags |= CLONE_FILES;
    return OsCopyProcess(cloneFlag & flags, name, (UINTPTR)entry, stackSize);
}
#else
LITE_OS_SEC_TEXT_INIT UINT32 OsUserInitProcess(VOID)
{
    return 0;
}
#endif

LITE_OS_SEC_TEXT VOID LOS_Exit(INT32 status)
{
    UINT32 intSave;

    /* The exit of a kernel - state process must be kernel - state and all threads must actively exit */
    LosProcessCB *processCB = OsCurrProcessGet();
    SCHEDULER_LOCK(intSave);
    if (!OsProcessIsUserMode(processCB) && (processCB->threadNumber != 1)) {
        SCHEDULER_UNLOCK(intSave);
        PRINT_ERR("Kernel-state processes with multiple threads are not allowed to exit directly\n");
        return;
    }
    SCHEDULER_UNLOCK(intSave);

    OsProcessThreadGroupDestroy();
    OsRunningTaskToExit(OsCurrTaskGet(), OS_PRO_EXIT_OK);
}

LITE_OS_SEC_TEXT INT32 LOS_GetUsedPIDList(UINT32 *pidList, INT32 pidMaxNum)
{
    LosProcessCB *pcb = NULL;
    INT32 num = 0;
    UINT32 intSave;
    UINT32 pid = 1;

    if (pidList == NULL) {
        return 0;
    }
    SCHEDULER_LOCK(intSave);
    while (OsProcessIDUserCheckInvalid(pid) == false) {
        pcb = OS_PCB_FROM_PID(pid);
        pid++;
        if (OsProcessIsUnused(pcb)) {
            continue;
        }
        pidList[num] = pcb->processID;
        num++;
        if (num >= pidMaxNum) {
            break;
        }
    }
    SCHEDULER_UNLOCK(intSave);
    return num;
}

#ifdef LOSCFG_FS_VFS
LITE_OS_SEC_TEXT struct fd_table_s *LOS_GetFdTable(UINT32 pid)
{
    LosProcessCB *pcb = NULL;
    struct files_struct *files = NULL;

    if (OS_PID_CHECK_INVALID(pid)) {
        return NULL;
    }
    pcb = OS_PCB_FROM_PID(pid);
    files = pcb->files;
    if (files == NULL) {
        return NULL;
    }

    return files->fdt;
}
#endif

LITE_OS_SEC_TEXT UINT32 LOS_GetCurrProcessID(VOID)
{
    return OsCurrProcessGet()->processID;
}

#ifdef LOSCFG_KERNEL_VM
STATIC VOID ThreadGroupActiveTaskKilled(LosTaskCB *taskCB)
{
    INT32 ret;

    taskCB->taskStatus |= OS_TASK_FLAG_EXIT_KILL;
#ifdef LOSCFG_KERNEL_SMP
    /** The other core that the thread is running on and is currently running in a non-system call */
    if (!taskCB->sig.sigIntLock && (taskCB->taskStatus & OS_TASK_STATUS_RUNNING)) {
        taskCB->signal = SIGNAL_KILL;
        LOS_MpSchedule(taskCB->currCpu);
    } else
#endif
    {
        ret = OsTaskKillUnsafe(taskCB->taskID, SIGKILL);
        if (ret != LOS_OK) {
            PRINT_ERR("pid %u exit, Exit task group %u kill %u failed! ERROR: %d\n",
                      taskCB->processID, OsCurrTaskGet()->taskID, taskCB->taskID, ret);
        }
    }

    if (!(taskCB->taskStatus & OS_TASK_FLAG_PTHREAD_JOIN)) {
        taskCB->taskStatus |= OS_TASK_FLAG_PTHREAD_JOIN;
        LOS_ListInit(&taskCB->joinList);
    }

    ret = OsTaskJoinPendUnsafe(taskCB);
    if (ret != LOS_OK) {
        PRINT_ERR("pid %u exit, Exit task group %u to wait others task %u(0x%x) exit failed! ERROR: %d\n",
                  taskCB->processID, OsCurrTaskGet()->taskID, taskCB->taskID, taskCB->taskStatus, ret);
    }
}
#endif

LITE_OS_SEC_TEXT VOID OsProcessThreadGroupDestroy(VOID)
{
#ifdef LOSCFG_KERNEL_VM
    UINT32 intSave;

    LosProcessCB *processCB = OsCurrProcessGet();
    LosTaskCB *currTask = OsCurrTaskGet();
    SCHEDULER_LOCK(intSave);
    if ((processCB->processStatus & OS_PROCESS_FLAG_EXIT) || !OsProcessIsUserMode(processCB)) {
        SCHEDULER_UNLOCK(intSave);
        return;
    }

    processCB->processStatus |= OS_PROCESS_FLAG_EXIT;
    processCB->threadGroupID = currTask->taskID;

    LOS_DL_LIST *list = &processCB->threadSiblingList;
    LOS_DL_LIST *head = list;
    do {
        LosTaskCB *taskCB = LOS_DL_LIST_ENTRY(list->pstNext, LosTaskCB, threadList);
        if ((OsTaskIsInactive(taskCB) ||
            ((taskCB->taskStatus & OS_TASK_STATUS_READY) && !taskCB->sig.sigIntLock)) &&
            !(taskCB->taskStatus & OS_TASK_STATUS_RUNNING)) {
            OsInactiveTaskDelete(taskCB);
        } else if (taskCB != currTask) {
            ThreadGroupActiveTaskKilled(taskCB);
        } else {
            /* Skip the current task */
            list = list->pstNext;
        }
    } while (head != list->pstNext);

    SCHEDULER_UNLOCK(intSave);

    LOS_ASSERT(processCB->threadNumber == 1);
#endif
    return;
}

LITE_OS_SEC_TEXT UINT32 LOS_GetSystemProcessMaximum(VOID)
{
    return g_processMaxNum;
}

LITE_OS_SEC_TEXT UINT32 OsGetUserInitProcessID(VOID)
{
    return g_userInitProcess;
}

LITE_OS_SEC_TEXT UINT32 OsGetKernelInitProcessID(VOID)
{
    return g_kernelInitProcess;
}

LITE_OS_SEC_TEXT UINT32 OsGetIdleProcessID(VOID)
{
    return g_kernelIdleProcess;
}

LITE_OS_SEC_TEXT VOID OsSetSigHandler(UINTPTR addr)
{
    OsCurrProcessGet()->sigHandler = addr;
}

LITE_OS_SEC_TEXT UINTPTR OsGetSigHandler(VOID)
{
    return OsCurrProcessGet()->sigHandler;
}

