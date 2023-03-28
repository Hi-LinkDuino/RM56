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

#include "los_exc.h"
#include "los_memory_pri.h"
#include "los_printf_pri.h"
#include "los_task_pri.h"
#include "los_percpu_pri.h"
#include "los_hw_pri.h"
#ifdef LOSCFG_SAVE_EXCINFO
#include "los_excinfo_pri.h"
#endif
#include "los_sys_stack_pri.h"
#ifdef LOSCFG_COREDUMP
#include "los_coredump.h"
#endif
#ifdef LOSCFG_GDB
#include "gdb_int.h"
#endif
#include "los_mp.h"
#include "los_vm_map.h"
#include "los_vm_dump.h"
#include "los_arch_mmu.h"
#include "los_vm_phys.h"
#include "los_vm_fault.h"
#include "los_vm_common.h"
#ifdef LOSCFG_KERNEL_DYNLOAD
#include "los_load_elf.h"
#endif
#include "arm.h"
#include "los_bitmap.h"
#include "los_process_pri.h"
#include "los_exc_pri.h"
#include "los_sched_pri.h"
#ifdef LOSCFG_FS_VFS
#include "console.h"
#endif
#ifdef LOSCFG_BLACKBOX
#include "los_blackbox.h"
#endif


#define INVALID_CPUID  0xFFFF
#define OS_EXC_VMM_NO_REGION  0x0U
#define OS_EXC_VMM_ALL_REGION 0x1U

STATIC UINTPTR g_minAddr;
STATIC UINTPTR g_maxAddr;
STATIC UINT32 g_currHandleExcCpuID = INVALID_CPUID;
VOID OsExcHook(UINT32 excType, ExcContext *excBufAddr, UINT32 far, UINT32 fsr);
UINT32 g_curNestCount[LOSCFG_KERNEL_CORE_NUM] = { 0 };
BOOL g_excFromUserMode[LOSCFG_KERNEL_CORE_NUM];
STATIC EXC_PROC_FUNC g_excHook = (EXC_PROC_FUNC)OsExcHook;
#ifdef LOSCFG_KERNEL_SMP
STATIC SPIN_LOCK_INIT(g_excSerializerSpin);
STATIC UINT32 g_currHandleExcPID = OS_INVALID_VALUE;
STATIC UINT32 g_nextExcWaitCpu = INVALID_CPUID;
#endif

#define OS_MAX_BACKTRACE    15U
#define DUMPSIZE            128U
#define DUMPREGS            12U
#define INSTR_SET_MASK      0x01000020U
#define THUMB_INSTR_LEN     2U
#define ARM_INSTR_LEN       4U
#define POINTER_SIZE        4U
#define WNR_BIT             11U
#define FSR_FLAG_OFFSET_BIT 10U
#define FSR_BITS_BEGIN_BIT  3U


#define GET_FS(fsr) (((fsr) & 0xFU) | (((fsr) & (1U << 10)) >> 6))
#define GET_WNR(dfsr) ((dfsr) & (1U << 11))

#define IS_VALID_ADDR(ptr) (((ptr) >= g_minAddr) &&       \
                            ((ptr) <= g_maxAddr) && \
                            (IS_ALIGNED((ptr), sizeof(CHAR *))))

STATIC const StackInfo g_excStack[] = {
    { &__svc_stack,   OS_EXC_SVC_STACK_SIZE,   "svc_stack" },
    { &__exc_stack,   OS_EXC_STACK_SIZE,       "exc_stack" }
};

UINT32 OsGetSystemStatus(VOID)
{
    UINT32 flag;
    UINT32 cpuID = g_currHandleExcCpuID;

    if (cpuID == INVALID_CPUID) {
        flag = OS_SYSTEM_NORMAL;
    } else if (cpuID == ArchCurrCpuid()) {
        flag = OS_SYSTEM_EXC_CURR_CPU;
    } else {
        flag = OS_SYSTEM_EXC_OTHER_CPU;
    }

    return flag;
}

STATIC INT32 OsDecodeFS(UINT32 bitsFS)
{
    switch (bitsFS) {
        case 0x05:  /* 0b00101 */
        case 0x07:  /* 0b00111 */
            PrintExcInfo("Translation fault, %s\n", (bitsFS & 0x2) ? "page" : "section");
            break;
        case 0x09:  /* 0b01001 */
        case 0x0b:  /* 0b01011 */
            PrintExcInfo("Domain fault, %s\n", (bitsFS & 0x2) ? "page" : "section");
            break;
        case 0x0d:  /* 0b01101 */
        case 0x0f:  /* 0b01111 */
            PrintExcInfo("Permission fault, %s\n", (bitsFS & 0x2) ? "page" : "section");
            break;
        default:
            PrintExcInfo("Unknown fault! FS:0x%x. "
                         "Check IFSR and DFSR in ARM Architecture Reference Manual.\n",
                         bitsFS);
            break;
    }

    return LOS_OK;
}

STATIC INT32 OsDecodeInstructionFSR(UINT32 regIFSR)
{
    INT32 ret;
    UINT32 bitsFS = GET_FS(regIFSR); /* FS bits[4]+[3:0] */

    ret = OsDecodeFS(bitsFS);
    return ret;
}

STATIC INT32 OsDecodeDataFSR(UINT32 regDFSR)
{
    INT32 ret = 0;
    UINT32 bitWnR = GET_WNR(regDFSR); /* WnR bit[11] */
    UINT32 bitsFS = GET_FS(regDFSR);  /* FS bits[4]+[3:0] */

    if (bitWnR) {
        PrintExcInfo("Abort caused by a write instruction. ");
    } else {
        PrintExcInfo("Abort caused by a read instruction. ");
    }

    if (bitsFS == 0x01) { /* 0b00001 */
        PrintExcInfo("Alignment fault.\n");
        return ret;
    }
    ret = OsDecodeFS(bitsFS);
    return ret;
}

#ifdef LOSCFG_KERNEL_VM
UINT32 OsArmSharedPageFault(UINT32 excType, ExcContext *frame, UINT32 far, UINT32 fsr)
{
    BOOL instructionFault = FALSE;
    UINT32 pfFlags = 0;
    UINT32 fsrFlag;
    BOOL write = FALSE;
    UINT32 ret;

    PRINT_INFO("page fault entry!!!\n");
    if (OsGetSystemStatus() == OS_SYSTEM_EXC_CURR_CPU) {
        return LOS_ERRNO_VM_NOT_FOUND;
    }
#if defined(LOSCFG_KERNEL_SMP) && defined(LOSCFG_DEBUG_VERSION)
    BOOL irqEnable = !(LOS_SpinHeld(&g_taskSpin) && OsSchedIsLock());
    if (irqEnable) {
        ArchIrqEnable();
    } else {
        PrintExcInfo("[ERR][%s] may be held scheduler lock when entering [%s] on cpu [%u]\n",
                     OsCurrTaskGet()->taskName, __FUNCTION__, ArchCurrCpuid());
    }
#else
    ArchIrqEnable();
#endif
    if (excType == OS_EXCEPT_PREFETCH_ABORT) {
        instructionFault = TRUE;
    } else {
        write = !!BIT_GET(fsr, WNR_BIT);
    }

    fsrFlag = ((BIT_GET(fsr, FSR_FLAG_OFFSET_BIT) ? 0b10000 : 0) | BITS_GET(fsr, FSR_BITS_BEGIN_BIT, 0));
    switch (fsrFlag) {
        case 0b00101:
        /* translation fault */
        case 0b00111:
        /* translation fault */
        case 0b01101:
        /* permission fault */
        case 0b01111: {
        /* permission fault */
            BOOL user = (frame->regCPSR & CPSR_MODE_MASK) == CPSR_MODE_USR;
            pfFlags |= write ? VM_MAP_PF_FLAG_WRITE : 0;
            pfFlags |= user ? VM_MAP_PF_FLAG_USER : 0;
            pfFlags |= instructionFault ? VM_MAP_PF_FLAG_INSTRUCTION : 0;
            pfFlags |= VM_MAP_PF_FLAG_NOT_PRESENT;
            OsSigIntLock();
            ret = OsVmPageFaultHandler(far, pfFlags, frame);
            OsSigIntUnlock();
            break;
        }
        default:
            OsArmWriteTlbimvaais(ROUNDDOWN(far, PAGE_SIZE));
            ret = LOS_OK;
            break;
    }
#if defined(LOSCFG_KERNEL_SMP) && defined(LOSCFG_DEBUG_VERSION)
    if (irqEnable) {
        ArchIrqDisable();
    }
#else
    ArchIrqDisable();
#endif
    return ret;
}
#endif

STATIC VOID OsExcType(UINT32 excType, ExcContext *excBufAddr, UINT32 far, UINT32 fsr)
{
    /* undefined exception handling or software interrupt */
    if ((excType == OS_EXCEPT_UNDEF_INSTR) || (excType == OS_EXCEPT_SWI)) {
        if ((excBufAddr->regCPSR & INSTR_SET_MASK) == 0) { /* work status: ARM */
            excBufAddr->PC = excBufAddr->PC - ARM_INSTR_LEN;
        } else if ((excBufAddr->regCPSR & INSTR_SET_MASK) == 0x20) { /* work status: Thumb */
            excBufAddr->PC = excBufAddr->PC - THUMB_INSTR_LEN;
        }
    }

    if (excType == OS_EXCEPT_PREFETCH_ABORT) {
        PrintExcInfo("prefetch_abort fault fsr:0x%x, far:0x%0+8x\n", fsr, far);
        (VOID)OsDecodeInstructionFSR(fsr);
    } else if (excType == OS_EXCEPT_DATA_ABORT) {
        PrintExcInfo("data_abort fsr:0x%x, far:0x%0+8x\n", fsr, far);
        (VOID)OsDecodeDataFSR(fsr);
    }
}

STATIC const CHAR *g_excTypeString[] = {
    "reset",
    "undefined instruction",
    "software interrupt",
    "prefetch abort",
    "data abort",
    "fiq",
    "address abort",
    "irq"
};

#ifdef LOSCFG_KERNEL_VM
STATIC VADDR_T OsGetTextRegionBase(LosVmMapRegion *region, LosProcessCB *runProcess)
{
    struct Vnode *curVnode = NULL;
    struct Vnode *lastVnode = NULL;
    LosVmMapRegion *curRegion = NULL;
    LosVmMapRegion *lastRegion = NULL;

    if ((region == NULL) || (runProcess == NULL)) {
        return 0;
    }

    if (!LOS_IsRegionFileValid(region)) {
        return region->range.base;
    }

    lastRegion = region;
    do {
        curRegion = lastRegion;
        lastRegion = LOS_RegionFind(runProcess->vmSpace, curRegion->range.base - 1);
        if ((lastRegion == NULL) || !LOS_IsRegionFileValid(lastRegion)) {
            goto DONE;
        }
        curVnode = curRegion->unTypeData.rf.vnode;
        lastVnode = lastRegion->unTypeData.rf.vnode;
    } while (curVnode == lastVnode);

DONE:
#ifdef LOSCFG_KERNEL_DYNLOAD
    if (curRegion->range.base == EXEC_MMAP_BASE) {
        return 0;
    }
#endif
    return curRegion->range.base;
}
#endif

STATIC VOID OsExcSysInfo(UINT32 excType, const ExcContext *excBufAddr)
{
    LosTaskCB *runTask = OsCurrTaskGet();
    LosProcessCB *runProcess = OsCurrProcessGet();

    PrintExcInfo("excType: %s\n"
                 "processName       = %s\n"
                 "processID         = %u\n"
#ifdef LOSCFG_KERNEL_VM
                 "process aspace    = 0x%08x -> 0x%08x\n"
#endif
                 "taskName          = %s\n"
                 "taskID            = %u\n",
                 g_excTypeString[excType],
                 runProcess->processName,
                 runProcess->processID,
#ifdef LOSCFG_KERNEL_VM
                 runProcess->vmSpace->base,
                 runProcess->vmSpace->base + runProcess->vmSpace->size,
#endif
                 runTask->taskName,
                 runTask->taskID);

#ifdef LOSCFG_KERNEL_VM
    if (OsProcessIsUserMode(runProcess)) {
        PrintExcInfo("task user stack   = 0x%08x -> 0x%08x\n",
                     runTask->userMapBase, runTask->userMapBase + runTask->userMapSize);
    } else
#endif
    {
        PrintExcInfo("task kernel stack = 0x%08x -> 0x%08x\n",
                     runTask->topOfStack, runTask->topOfStack + runTask->stackSize);
    }

    PrintExcInfo("pc    = 0x%x ", excBufAddr->PC);
#ifdef LOSCFG_KERNEL_VM
    LosVmMapRegion *region = NULL;
    if (g_excFromUserMode[ArchCurrCpuid()] == TRUE) {
        if (LOS_IsUserAddress((vaddr_t)excBufAddr->PC)) {
            region = LOS_RegionFind(runProcess->vmSpace, (VADDR_T)excBufAddr->PC);
            if (region != NULL) {
                PrintExcInfo("in %s ---> 0x%x", OsGetRegionNameOrFilePath(region),
                             (VADDR_T)excBufAddr->PC - OsGetTextRegionBase(region, runProcess));
            }
        }

        PrintExcInfo("\nulr   = 0x%x ", excBufAddr->ULR);
        region = LOS_RegionFind(runProcess->vmSpace, (VADDR_T)excBufAddr->ULR);
        if (region != NULL) {
            PrintExcInfo("in %s ---> 0x%x", OsGetRegionNameOrFilePath(region),
                         (VADDR_T)excBufAddr->ULR - OsGetTextRegionBase(region, runProcess));
        }
        PrintExcInfo("\nusp   = 0x%x", excBufAddr->USP);
    } else
#endif
    {
        PrintExcInfo("\nklr   = 0x%x\n"
                     "ksp   = 0x%x\n",
                     excBufAddr->LR,
                     excBufAddr->SP);
    }

    PrintExcInfo("\nfp    = 0x%x\n", excBufAddr->R11);
}

STATIC VOID OsExcRegsInfo(const ExcContext *excBufAddr)
{
    /*
     * Split register information into two parts:
     * Ensure printing does not rely on memory modules.
     */
    PrintExcInfo("R0    = 0x%x\n"
                 "R1    = 0x%x\n"
                 "R2    = 0x%x\n"
                 "R3    = 0x%x\n"
                 "R4    = 0x%x\n"
                 "R5    = 0x%x\n"
                 "R6    = 0x%x\n",
                 excBufAddr->R0, excBufAddr->R1, excBufAddr->R2, excBufAddr->R3,
                 excBufAddr->R4, excBufAddr->R5, excBufAddr->R6);
    PrintExcInfo("R7    = 0x%x\n"
                 "R8    = 0x%x\n"
                 "R9    = 0x%x\n"
                 "R10   = 0x%x\n"
                 "R11   = 0x%x\n"
                 "R12   = 0x%x\n"
                 "CPSR  = 0x%x\n",
                 excBufAddr->R7, excBufAddr->R8, excBufAddr->R9, excBufAddr->R10,
                 excBufAddr->R11, excBufAddr->R12, excBufAddr->regCPSR);
}

LITE_OS_SEC_TEXT_INIT UINT32 LOS_ExcRegHook(EXC_PROC_FUNC excHook)
{
    UINT32 intSave;

    intSave = LOS_IntLock();
    g_excHook = excHook;
    LOS_IntRestore(intSave);

    return LOS_OK;
}

EXC_PROC_FUNC OsExcRegHookGet(VOID)
{
    return g_excHook;
}

#ifdef LOSCFG_KERNEL_VM
STATIC VOID OsDumpExcVaddrRegion(LosVmSpace *space, LosVmMapRegion *region)
{
    INT32 i, numPages, pageCount;
    paddr_t addr, oldAddr, startVaddr, startPaddr;
    vaddr_t pageBase;
    BOOL mmuFlag = FALSE;

    numPages = region->range.size >> PAGE_SHIFT;
    mmuFlag = TRUE;
    for (pageCount = 0, startPaddr = 0, startVaddr = 0, i = 0; i < numPages; i++) {
        pageBase = region->range.base + i * PAGE_SIZE;
        addr = 0;
        if (LOS_ArchMmuQuery(&space->archMmu, pageBase, &addr, NULL) != LOS_OK) {
            if (startPaddr == 0) {
                continue;
            }
        } else if (startPaddr == 0) {
            startVaddr = pageBase;
            startPaddr = addr;
            oldAddr = addr;
            pageCount++;
            if (numPages > 1) {
                continue;
            }
        } else if (addr == (oldAddr + PAGE_SIZE)) {
            pageCount++;
            oldAddr = addr;
            if (i < (numPages - 1)) {
                continue;
            }
        }
        if (mmuFlag == TRUE) {
            PrintExcInfo("       uvaddr       kvaddr       mapped size\n");
            mmuFlag = FALSE;
        }
        PrintExcInfo("       0x%08x   0x%08x   0x%08x\n",
                     startVaddr, LOS_PaddrToKVaddr(startPaddr), (UINT32)pageCount << PAGE_SHIFT);
        pageCount = 0;
        startPaddr = 0;
    }
}

STATIC VOID OsDumpProcessUsedMemRegion(LosProcessCB *runProcess, LosVmSpace *runspace, UINT16 vmmFlags)
{
    LosVmMapRegion *region = NULL;
    LosRbNode *pstRbNodeTemp = NULL;
    LosRbNode *pstRbNodeNext = NULL;
    UINT32 count = 0;

    /* search the region list */
    RB_SCAN_SAFE(&runspace->regionRbTree, pstRbNodeTemp, pstRbNodeNext)
        region = (LosVmMapRegion *)pstRbNodeTemp;
        PrintExcInfo("%3u -> regionBase: 0x%08x regionSize: 0x%08x\n", count, region->range.base, region->range.size);
        if (vmmFlags == OS_EXC_VMM_ALL_REGION) {
            OsDumpExcVaddrRegion(runspace, region);
        }
        count++;
        (VOID)OsRegionOverlapCheckUnlock(runspace, region);
    RB_SCAN_SAFE_END(&space->regionRbTree, pstRbNodeTemp, pstRbNodeNext)
}

STATIC VOID OsDumpProcessUsedMemNode(UINT16 vmmFlags)
{
    LosProcessCB *runProcess = NULL;
    LosVmSpace *runspace = NULL;

    runProcess = OsCurrProcessGet();
    if (runProcess == NULL) {
        return;
    }

    if (!OsProcessIsUserMode(runProcess)) {
        return;
    }

    PrintExcInfo("\n   ******Current process %u vmm regions: ******\n", runProcess->processID);

    runspace = runProcess->vmSpace;
    if (!runspace) {
        return;
    }

    OsDumpProcessUsedMemRegion(runProcess, runspace, vmmFlags);
    return;
}
#endif

VOID OsDumpContextMem(const ExcContext *excBufAddr)
{
    UINT32 count = 0;
    const UINT32 *excReg = NULL;
    if (g_excFromUserMode[ArchCurrCpuid()] == TRUE) {
        return;
    }

    for (excReg = &(excBufAddr->R0); count <= DUMPREGS; excReg++, count++) {
        if (IS_VALID_ADDR(*excReg)) {
            PrintExcInfo("\ndump mem around R%u:%p", count, (*excReg));
            OsDumpMemByte(DUMPSIZE, ((*excReg) - (DUMPSIZE >> 1)));
        }
    }

    if (IS_VALID_ADDR(excBufAddr->SP)) {
        PrintExcInfo("\ndump mem around SP:%p", excBufAddr->SP);
        OsDumpMemByte(DUMPSIZE, (excBufAddr->SP - (DUMPSIZE >> 1)));
    }
}

STATIC VOID OsExcRestore(VOID)
{
    UINT32 currCpuID = ArchCurrCpuid();

    g_excFromUserMode[currCpuID] = FALSE;
    g_intCount[currCpuID] = 0;
    g_curNestCount[currCpuID] = 0;
#ifdef LOSCFG_KERNEL_SMP
    OsCpuStatusSet(CPU_RUNNING);
#endif
    OsSchedLockSet(0);
}

STATIC VOID OsUserExcHandle(ExcContext *excBufAddr)
{
    UINT32 intSave;
    UINT32 currCpu = ArchCurrCpuid();
    LosTaskCB *runTask = OsCurrTaskGet();
    LosProcessCB *runProcess = OsCurrProcessGet();

    if (g_excFromUserMode[ArchCurrCpuid()] == FALSE) {
        return;
    }

#ifdef LOSCFG_KERNEL_SMP
    LOS_SpinLock(&g_excSerializerSpin);
    if (g_nextExcWaitCpu != INVALID_CPUID) {
        g_currHandleExcCpuID = g_nextExcWaitCpu;
        g_nextExcWaitCpu = INVALID_CPUID;
    } else {
        g_currHandleExcCpuID = INVALID_CPUID;
    }
    g_currHandleExcPID = OS_INVALID_VALUE;
    LOS_SpinUnlock(&g_excSerializerSpin);
#else
    g_currHandleExcCpuID = INVALID_CPUID;
#endif

#ifdef LOSCFG_KERNEL_SMP
#ifdef LOSCFG_FS_VFS
    OsWakeConsoleSendTask();
#endif
#endif

#ifdef LOSCFG_BLACKBOX
    BBoxNotifyError("USER_CRASH", MODULE_SYSTEM, "Crash in user", 0);
#endif
    SCHEDULER_LOCK(intSave);
#ifdef LOSCFG_SAVE_EXCINFO
    OsProcessExitCodeCoreDumpSet(runProcess);
#endif
    OsProcessExitCodeSignalSet(runProcess, SIGUSR2);

    /* An exception was raised by a task during the exit process of
     * the current process.
     */
    if (runProcess->processStatus & OS_PROCESS_FLAG_EXIT) {
        SCHEDULER_UNLOCK(intSave);
        /* Exception handling All operations should be kept prior to that operation */
        OsExcRestore();
        OsRunningTaskToExit(runTask, OS_PRO_EXIT_OK);
    } else {
        SCHEDULER_UNLOCK(intSave);

        /* Exception handling All operations should be kept prior to that operation */
        OsExcRestore();
        /* kill user exc process */
        LOS_Exit(OS_PRO_EXIT_OK);
    }

    /* User mode exception handling failed , which normally does not exist */
    g_curNestCount[currCpu]++;
    g_intCount[currCpu]++;
    PrintExcInfo("User mode exception ends unscheduled!\n");
}

/* this function is used to validate fp or validate the checking range start and end. */
STATIC INLINE BOOL IsValidFP(UINTPTR regFP, UINTPTR start, UINTPTR end, vaddr_t *vaddr)
{
    VADDR_T kvaddr = regFP;

    if (!((regFP > start) && (regFP < end) && IS_ALIGNED(regFP, sizeof(CHAR *)))) {
        return FALSE;
    }

#ifdef LOSCFG_KERNEL_VM
    PADDR_T paddr;
    if (g_excFromUserMode[ArchCurrCpuid()] == TRUE) {
        LosProcessCB *runProcess = OsCurrProcessGet();
        LosVmSpace *runspace = runProcess->vmSpace;
        if (runspace == NULL) {
            return FALSE;
        }

        if (LOS_ArchMmuQuery(&runspace->archMmu, regFP, &paddr, NULL) != LOS_OK) {
            return FALSE;
        }

        kvaddr = (PADDR_T)(UINTPTR)LOS_PaddrToKVaddr(paddr);
    }
#endif
    if (vaddr != NULL) {
        *vaddr = kvaddr;
    }

    return TRUE;
}

STATIC INLINE BOOL FindSuitableStack(UINTPTR regFP, UINTPTR *start, UINTPTR *end, vaddr_t *vaddr)
{
    UINT32 index, stackStart, stackEnd;
    BOOL found = FALSE;
    LosTaskCB *taskCB = NULL;
    const StackInfo *stack = NULL;
    vaddr_t kvaddr;

#ifdef LOSCFG_KERNEL_VM
    if (g_excFromUserMode[ArchCurrCpuid()] == TRUE) {
        taskCB = OsCurrTaskGet();
        stackStart = taskCB->userMapBase;
        stackEnd = taskCB->userMapBase + taskCB->userMapSize;
        if (IsValidFP(regFP, stackStart, stackEnd, &kvaddr) == TRUE) {
            found = TRUE;
            goto FOUND;
        }
        return found;
    }
#endif

    /* Search in the task stacks */
    for (index = 0; index < g_taskMaxNum; index++) {
        taskCB = &g_taskCBArray[index];
        if (OsTaskIsUnused(taskCB)) {
            continue;
        }

        stackStart = taskCB->topOfStack;
        stackEnd = taskCB->topOfStack + taskCB->stackSize;
        if (IsValidFP(regFP, stackStart, stackEnd, &kvaddr) == TRUE) {
            found = TRUE;
            goto FOUND;
        }
    }

    /* Search in the exc stacks */
    for (index = 0; index < sizeof(g_excStack) / sizeof(StackInfo); index++) {
        stack = &g_excStack[index];
        stackStart = (UINTPTR)stack->stackTop;
        stackEnd = stackStart + LOSCFG_KERNEL_CORE_NUM * stack->stackSize;
        if (IsValidFP(regFP, stackStart, stackEnd, &kvaddr) == TRUE) {
            found = TRUE;
            goto FOUND;
        }
    }

FOUND:
    if (found == TRUE) {
        *start = stackStart;
        *end = stackEnd;
        *vaddr = kvaddr;
    }

    return found;
}

BOOL OsGetUsrIpInfo(UINTPTR ip, IpInfo *info)
{
    if (info == NULL) {
        return FALSE;
    }
#ifdef LOSCFG_KERNEL_VM
    BOOL ret = FALSE;
    const CHAR *name = NULL;
    LosVmMapRegion *region = NULL;
    LosProcessCB *runProcess = OsCurrProcessGet();

    if (LOS_IsUserAddress((VADDR_T)ip) == FALSE) {
        info->ip = ip;
        name = "kernel";
        ret = FALSE;
        goto END;
    }

    region = LOS_RegionFind(runProcess->vmSpace, (VADDR_T)ip);
    if (region == NULL) {
        info->ip = ip;
        name = "invalid";
        ret = FALSE;
        goto END;
    }

    info->ip = ip - OsGetTextRegionBase(region, runProcess);
    name = OsGetRegionNameOrFilePath(region);
    ret = TRUE;
    if (strcmp(name, "/lib/libc.so") != 0) {
        PRINT_ERR("ip = 0x%x, %s\n", info->ip, name);
    }
END:
    info->len = strlen(name);
    if (strncpy_s(info->f_path, REGION_PATH_MAX, name, REGION_PATH_MAX - 1) != EOK) {
        info->f_path[0] = '\0';
        info->len = 0;
        PRINT_ERR("copy f_path failed, %s\n", name);
    }
    return ret;
#else
    info->ip = ip;
    return FALSE;
#endif
}

UINT32 BackTraceGet(UINTPTR regFP, IpInfo *callChain, UINT32 maxDepth)
{
    UINTPTR tmpFP, backLR;
    UINTPTR stackStart, stackEnd;
    UINTPTR backFP = regFP;
    UINT32 count = 0;
    BOOL ret;
    VADDR_T kvaddr;

    if (FindSuitableStack(regFP, &stackStart, &stackEnd, &kvaddr) == FALSE) {
        if (callChain == NULL) {
            PrintExcInfo("traceback error fp = 0x%x\n", regFP);
        }
        return 0;
    }

    /*
     * Check whether it is the leaf function.
     * Generally, the frame pointer points to the address of link register, while in the leaf function,
     * there's no function call, and compiler will not store the link register, but the frame pointer
     * will still be stored and updated. In that case we needs to find the right position of frame pointer.
     */
    tmpFP = *(UINTPTR *)(UINTPTR)kvaddr;
    if (IsValidFP(tmpFP, stackStart, stackEnd, NULL) == TRUE) {
        backFP = tmpFP;
        if (callChain == NULL) {
            PrintExcInfo("traceback fp fixed, trace using   fp = 0x%x\n", backFP);
        }
    }

    while (IsValidFP(backFP, stackStart, stackEnd, &kvaddr) == TRUE) {
        tmpFP = backFP;
#ifdef LOSCFG_COMPILER_CLANG_LLVM
        backFP = *(UINTPTR *)(UINTPTR)kvaddr;
        if (IsValidFP(tmpFP + POINTER_SIZE, stackStart, stackEnd, &kvaddr) == FALSE) {
            if (callChain == NULL) {
                PrintExcInfo("traceback backLR check failed, backLP: 0x%x\n", tmpFP + POINTER_SIZE);
            }
            return 0;
        }
        backLR = *(UINTPTR *)(UINTPTR)kvaddr;
#else
        backLR = *(UINTPTR *)(UINTPTR)kvaddr;
        if (IsValidFP(tmpFP - POINTER_SIZE, stackStart, stackEnd, &kvaddr) == FALSE) {
            if (callChain == NULL) {
                PrintExcInfo("traceback backFP check failed, backFP: 0x%x\n", tmpFP - POINTER_SIZE);
            }
            return 0;
        }
        backFP = *(UINTPTR *)(UINTPTR)kvaddr;
#endif
        IpInfo info = {0};
        ret = OsGetUsrIpInfo((VADDR_T)backLR, &info);
        if (callChain == NULL) {
            PrintExcInfo("traceback %u -- lr = 0x%x    fp = 0x%x ", count, backLR, backFP);
            if (ret) {
#ifdef LOSCFG_KERNEL_VM
                PrintExcInfo("lr in %s --> 0x%x\n", info.f_path, info.ip);
#else
                PrintExcInfo("\n");
#endif
            } else {
                PrintExcInfo("\n");
            }
        } else {
            (VOID)memcpy_s(&callChain[count], sizeof(IpInfo), &info, sizeof(IpInfo));
        }
        count++;
        if ((count == maxDepth) || (backFP == tmpFP)) {
            break;
        }
    }
    return count;
}

VOID BackTraceSub(UINTPTR regFP)
{
    (VOID)BackTraceGet(regFP, NULL, OS_MAX_BACKTRACE);
}

VOID BackTrace(UINT32 regFP)
{
    PrintExcInfo("*******backtrace begin*******\n");

    BackTraceSub(regFP);
}

VOID OsExcInit(VOID)
{
    OsExcStackInfoReg(g_excStack, sizeof(g_excStack) / sizeof(g_excStack[0]));
}

VOID OsExcHook(UINT32 excType, ExcContext *excBufAddr, UINT32 far, UINT32 fsr)
{
    OsExcType(excType, excBufAddr, far, fsr);
    OsExcSysInfo(excType, excBufAddr);
    OsExcRegsInfo(excBufAddr);

    BackTrace(excBufAddr->R11);

    (VOID)OsShellCmdTskInfoGet(OS_ALL_TASK_MASK, NULL, OS_PROCESS_INFO_ALL);

#ifndef LOSCFG_DEBUG_VERSION
    if (g_excFromUserMode[ArchCurrCpuid()] != TRUE) {
#endif
#ifdef LOSCFG_KERNEL_VM
        OsDumpProcessUsedMemNode(OS_EXC_VMM_NO_REGION);
#endif
        OsExcStackInfo();
#ifndef LOSCFG_DEBUG_VERSION
    }
#endif

    OsDumpContextMem(excBufAddr);

    (VOID)OsShellCmdMemCheck(0, NULL);

#ifdef LOSCFG_COREDUMP
    LOS_CoreDumpV2(excType, excBufAddr);
#endif

    OsUserExcHandle(excBufAddr);
}

VOID OsCallStackInfo(VOID)
{
    UINT32 count = 0;
    LosTaskCB *runTask = OsCurrTaskGet();
    UINTPTR stackBottom = runTask->topOfStack + runTask->stackSize;
    UINT32 *stackPointer = (UINT32 *)stackBottom;

    PrintExcInfo("runTask->stackPointer = 0x%x\n"
                 "runTask->topOfStack = 0x%x\n"
                 "text_start:0x%x,text_end:0x%x\n",
                 stackPointer, runTask->topOfStack, &__text_start, &__text_end);

    while ((stackPointer > (UINT32 *)runTask->topOfStack) && (count < OS_MAX_BACKTRACE)) {
        if ((*stackPointer > (UINTPTR)(&__text_start)) &&
            (*stackPointer < (UINTPTR)(&__text_end)) &&
            IS_ALIGNED((*stackPointer), POINTER_SIZE)) {
            if ((*(stackPointer - 1) > (UINT32)runTask->topOfStack) &&
                (*(stackPointer - 1) < stackBottom) &&
                IS_ALIGNED((*(stackPointer - 1)), POINTER_SIZE)) {
                count++;
                PrintExcInfo("traceback %u -- lr = 0x%x\n", count, *stackPointer);
            }
        }
        stackPointer--;
    }
    PRINTK("\n");
}

VOID OsTaskBackTrace(UINT32 taskID)
{
    LosTaskCB *taskCB = NULL;

    if (OS_TID_CHECK_INVALID(taskID)) {
        PRINT_ERR("\r\nTask ID is invalid!\n");
        return;
    }
    taskCB = OS_TCB_FROM_TID(taskID);
    if (OsTaskIsUnused(taskCB) || (taskCB->taskEntry == NULL)) {
        PRINT_ERR("\r\nThe task is not created!\n");
        return;
    }
    PRINTK("TaskName = %s\n", taskCB->taskName);
    PRINTK("TaskID = 0x%x\n", taskCB->taskID);
    BackTrace(((TaskContext *)(taskCB->stackPointer))->R11); /* R11 : FP */
}

VOID OsBackTrace(VOID)
{
    UINT32 regFP = Get_Fp();
    LosTaskCB *runTask = OsCurrTaskGet();
    PrintExcInfo("OsBackTrace fp = 0x%x\n", regFP);
    PrintExcInfo("runTask->taskName = %s\n", runTask->taskName);
    PrintExcInfo("runTask->taskID = %u\n", runTask->taskID);
    BackTrace(regFP);
}

#ifdef LOSCFG_GDB
VOID OsUndefIncExcHandleEntry(ExcContext *excBufAddr)
{
    excBufAddr->PC -= 4;  /* lr in undef is pc + 4 */

    if (gdb_undef_hook(excBufAddr, OS_EXCEPT_UNDEF_INSTR)) {
        return;
    }

    if (g_excHook != NULL) {
        /* far, fsr are unused in exc type of OS_EXCEPT_UNDEF_INSTR */
        g_excHook(OS_EXCEPT_UNDEF_INSTR, excBufAddr, 0, 0);
    }
    while (1) {}
}

#if __LINUX_ARM_ARCH__ >= 7
VOID OsPrefetchAbortExcHandleEntry(ExcContext *excBufAddr)
{
    UINT32 far;
    UINT32 fsr;

    excBufAddr->PC -= 4;  /* lr in prefetch abort is pc + 4 */

    if (gdbhw_hook(excBufAddr, OS_EXCEPT_PREFETCH_ABORT)) {
        return;
    }

    if (g_excHook != NULL) {
        far = OsArmReadIfar();
        fsr = OsArmReadIfsr();
        g_excHook(OS_EXCEPT_PREFETCH_ABORT, excBufAddr, far, fsr);
    }
    while (1) {}
}

VOID OsDataAbortExcHandleEntry(ExcContext *excBufAddr)
{
    UINT32 far;
    UINT32 fsr;

    excBufAddr->PC -= 8;  /* lr in data abort is pc + 8 */

    if (gdbhw_hook(excBufAddr, OS_EXCEPT_DATA_ABORT)) {
        return;
    }

    if (g_excHook != NULL) {
        far = OsArmReadDfar();
        fsr = OsArmReadDfsr();
        g_excHook(OS_EXCEPT_DATA_ABORT, excBufAddr, far, fsr);
    }
    while (1) {}
}
#endif /* __LINUX_ARM_ARCH__ */
#endif /* LOSCFG_GDB */

#ifdef LOSCFG_KERNEL_SMP
#define EXC_WAIT_INTER 50U
#define EXC_WAIT_TIME  2000U

STATIC VOID WaitAllCpuStop(UINT32 cpuID)
{
    UINT32 i;
    UINT32 time = 0;

    while (time < EXC_WAIT_TIME) {
        for (i = 0; i < LOSCFG_KERNEL_CORE_NUM; i++) {
            if ((i != cpuID) && !OsCpuStatusIsHalt(i)) {
                LOS_Mdelay(EXC_WAIT_INTER);
                time += EXC_WAIT_INTER;
                break;
            }
        }
        /* Other CPUs are all haletd or in the exc. */
        if (i == LOSCFG_KERNEL_CORE_NUM) {
            break;
        }
    }
    return;
}

STATIC VOID OsWaitOtherCoresHandleExcEnd(UINT32 currCpuID)
{
    while (1) {
        LOS_SpinLock(&g_excSerializerSpin);
        if ((g_currHandleExcCpuID == INVALID_CPUID) || (g_currHandleExcCpuID == currCpuID)) {
            g_currHandleExcCpuID = currCpuID;
            g_currHandleExcPID = OsCurrProcessGet()->processID;
            LOS_SpinUnlock(&g_excSerializerSpin);
            break;
        }

        if (g_nextExcWaitCpu == INVALID_CPUID) {
            g_nextExcWaitCpu = currCpuID;
        }
        LOS_SpinUnlock(&g_excSerializerSpin);
        LOS_Mdelay(EXC_WAIT_INTER);
    }
}

STATIC VOID OsCheckAllCpuStatus(VOID)
{
    UINT32 currCpuID = ArchCurrCpuid();
    UINT32 ret, target;

    OsCpuStatusSet(CPU_EXC);
    LOCKDEP_CLEAR_LOCKS();

    LOS_SpinLock(&g_excSerializerSpin);
    /* Only the current CPU anomaly */
    if (g_currHandleExcCpuID == INVALID_CPUID) {
        g_currHandleExcCpuID = currCpuID;
        g_currHandleExcPID = OsCurrProcessGet()->processID;
        LOS_SpinUnlock(&g_excSerializerSpin);
#ifndef LOSCFG_SAVE_EXCINFO
        if (g_excFromUserMode[currCpuID] == FALSE) {
            target = (UINT32)(OS_MP_CPU_ALL & ~CPUID_TO_AFFI_MASK(currCpuID));
            HalIrqSendIpi(target, LOS_MP_IPI_HALT);
        }
#endif
    } else if (g_excFromUserMode[currCpuID] == TRUE) {
        /* Both cores raise exceptions, and the current core is a user-mode exception.
         * Both cores are abnormal and come from the same process
         */
        if (OsCurrProcessGet()->processID == g_currHandleExcPID) {
            LOS_SpinUnlock(&g_excSerializerSpin);
            OsExcRestore();
            ret = LOS_TaskDelete(OsCurrTaskGet()->taskID);
            LOS_Panic("%s supend task :%u failed: 0x%x\n", __FUNCTION__, OsCurrTaskGet()->taskID, ret);
        }
        LOS_SpinUnlock(&g_excSerializerSpin);

        OsWaitOtherCoresHandleExcEnd(currCpuID);
    } else {
        if ((g_currHandleExcCpuID < LOSCFG_KERNEL_CORE_NUM) && (g_excFromUserMode[g_currHandleExcCpuID] == TRUE)) {
            g_currHandleExcCpuID = currCpuID;
            LOS_SpinUnlock(&g_excSerializerSpin);
            target = (UINT32)(OS_MP_CPU_ALL & ~CPUID_TO_AFFI_MASK(currCpuID));
            HalIrqSendIpi(target, LOS_MP_IPI_HALT);
        } else {
            LOS_SpinUnlock(&g_excSerializerSpin);
            while (1) {}
        }
    }
#ifndef LOSCFG_SAVE_EXCINFO
    /* use halt ipi to stop other active cores */
    if (g_excFromUserMode[ArchCurrCpuid()] == FALSE) {
        WaitAllCpuStop(currCpuID);
    }
#endif
}
#endif

STATIC VOID OsCheckCpuStatus(VOID)
{
#ifdef LOSCFG_KERNEL_SMP
    OsCheckAllCpuStatus();
#else
    g_currHandleExcCpuID = ArchCurrCpuid();
#endif
}

LITE_OS_SEC_TEXT VOID STATIC OsExcPriorDisposal(ExcContext *excBufAddr)
{
    if ((excBufAddr->regCPSR & CPSR_MASK_MODE) == CPSR_USER_MODE) {
        g_minAddr = USER_ASPACE_BASE;
        g_maxAddr = USER_ASPACE_BASE + USER_ASPACE_SIZE;
        g_excFromUserMode[ArchCurrCpuid()] = TRUE;
    } else {
        g_minAddr = KERNEL_ASPACE_BASE;
        g_maxAddr = KERNEL_ASPACE_BASE + KERNEL_ASPACE_SIZE;
        g_excFromUserMode[ArchCurrCpuid()] = FALSE;
    }

    OsCheckCpuStatus();

#ifdef LOSCFG_KERNEL_SMP
#ifdef LOSCFG_FS_VFS
    /* Wait for the end of the Console task to avoid multicore printing code */
    OsWaitConsoleSendTaskPend(OsCurrTaskGet()->taskID);
#endif
#endif
}

LITE_OS_SEC_TEXT_INIT STATIC VOID OsPrintExcHead(UINT32 far)
{
#ifdef LOSCFG_BLACKBOX
#ifdef LOSCFG_SAVE_EXCINFO
    SetExcInfoIndex(0);
#endif
#endif
#ifdef LOSCFG_KERNEL_VM
    /* You are not allowed to add any other print information before this exception information */
    if (g_excFromUserMode[ArchCurrCpuid()] == TRUE) {
#ifdef LOSCFG_DEBUG_VERSION
        VADDR_T vaddr = ROUNDDOWN(far, PAGE_SIZE);
        LosVmSpace *space = LOS_SpaceGet(vaddr);
        if (space != NULL) {
            LOS_DumpMemRegion(vaddr);
        }
#endif
        PrintExcInfo("##################excFrom: User!####################\n");
    } else
#endif
    {
        PrintExcInfo("##################excFrom: kernel!###################\n");
    }
}

#ifdef LOSCFG_SAVE_EXCINFO
STATIC VOID OsSysStateSave(UINT32 *intCount, UINT32 *lockCount)
{
    *intCount = g_intCount[ArchCurrCpuid()];
    *lockCount = OsSchedLockCountGet();
    g_intCount[ArchCurrCpuid()] = 0;
    OsSchedLockSet(0);
}

STATIC VOID OsSysStateRestore(UINT32 intCount, UINT32 lockCount)
{
    g_intCount[ArchCurrCpuid()] = intCount;
    OsSchedLockSet(lockCount);
}
#endif

/*
 * Description : EXC handler entry
 * Input       : excType    --- exc type
 *               excBufAddr --- address of EXC buf
 */
LITE_OS_SEC_TEXT_INIT VOID OsExcHandleEntry(UINT32 excType, ExcContext *excBufAddr, UINT32 far, UINT32 fsr)
{
#ifdef LOSCFG_SAVE_EXCINFO
    UINT32 intCount;
    UINT32 lockCount;
#endif

    /* Task scheduling is not allowed during exception handling */
    OsSchedLock();

    g_curNestCount[ArchCurrCpuid()]++;

    OsExcPriorDisposal(excBufAddr);

    OsPrintExcHead(far);

#ifdef LOSCFG_KERNEL_SMP
    OsAllCpuStatusOutput();
#endif

#ifdef LOSCFG_SAVE_EXCINFO
    log_read_write_fn func = GetExcInfoRW();
#endif

    if (g_excHook != NULL) {
        if (g_curNestCount[ArchCurrCpuid()] == 1) {
#ifdef LOSCFG_SAVE_EXCINFO
            if (func != NULL) {
#ifndef LOSCFG_BLACKBOX
                SetExcInfoIndex(0);
#endif
                OsSysStateSave(&intCount, &lockCount);
                OsRecordExcInfoTime();
                OsSysStateRestore(intCount, lockCount);
            }
#endif
            g_excHook(excType, excBufAddr, far, fsr);
        } else {
            OsCallStackInfo();
        }

#ifdef LOSCFG_SAVE_EXCINFO
        if (func != NULL) {
            PrintExcInfo("Be sure flash space bigger than GetExcInfoIndex():0x%x\n", GetExcInfoIndex());
            OsSysStateSave(&intCount, &lockCount);
            func(GetRecordAddr(), GetRecordSpace(), 0, GetExcInfoBuf());
            OsSysStateRestore(intCount, lockCount);
        }
#endif
    }

#ifdef LOSCFG_SHELL_CMD_DEBUG
    SystemRebootFunc rebootHook = OsGetRebootHook();
    if ((OsSystemExcIsReset() == TRUE) && (rebootHook != NULL)) {
        LOS_Mdelay(3000); /* 3000: System dead, delay 3 seconds after system restart */
        rebootHook();
    }
#endif

#ifdef LOSCFG_BLACKBOX
    BBoxNotifyError(EVENT_PANIC, MODULE_SYSTEM, "Crash in kernel", 1);
#endif
    while (1) {}
}

__attribute__((noinline)) VOID LOS_Panic(const CHAR *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    OsVprintf(fmt, ap, EXC_OUTPUT);
    va_end(ap);
    __asm__ __volatile__("swi 0");
    while (1) {}
}

/* stack protector */
USED UINT32 __stack_chk_guard = 0xd00a0dff;

VOID __stack_chk_fail(VOID)
{
    /* __builtin_return_address is a builtin function, building in gcc */
    LOS_Panic("stack-protector: Kernel stack is corrupted in: %p\n",
              __builtin_return_address(0));
}

VOID LOS_RecordLR(UINTPTR *LR, UINT32 LRSize, UINT32 recordCount, UINT32 jumpCount)
{
    UINT32 count = 0;
    UINT32 index = 0;
    UINT32 stackStart, stackEnd;
    LosTaskCB *taskCB = NULL;
    UINTPTR framePtr, tmpFramePtr, linkReg;

    if (LR == NULL) {
        return;
    }
    /* if LR array is not enough,just record LRSize. */
    if (LRSize < recordCount) {
        recordCount = LRSize;
    }

    taskCB = OsCurrTaskGet();
    stackStart = taskCB->topOfStack;
    stackEnd = stackStart + taskCB->stackSize;

    framePtr = Get_Fp();
    while ((framePtr > stackStart) && (framePtr < stackEnd) && IS_ALIGNED(framePtr, sizeof(CHAR *))) {
        tmpFramePtr = framePtr;
#ifdef LOSCFG_COMPILER_CLANG_LLVM
        linkReg = *(UINTPTR *)(tmpFramePtr + sizeof(UINTPTR));
#else
        linkReg = *(UINTPTR *)framePtr;
#endif
        if (index >= jumpCount) {
            LR[count++] = linkReg;
            if (count == recordCount) {
                break;
            }
        }
        index++;
#ifdef LOSCFG_COMPILER_CLANG_LLVM
        framePtr = *(UINTPTR *)framePtr;
#else
        framePtr = *(UINTPTR *)(tmpFramePtr - sizeof(UINTPTR));
#endif
    }

    /* if linkReg is not enough,clean up the last of the effective LR as the end. */
    if (count < recordCount) {
        LR[count] = 0;
    }
}

