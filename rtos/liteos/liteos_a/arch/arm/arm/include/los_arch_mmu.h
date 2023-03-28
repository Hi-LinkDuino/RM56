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

/**
 * @defgroup los_arch_mmu architecture mmu
 * @ingroup kernel
 */

#ifndef __LOS_ARCH_MMU_H__
#define __LOS_ARCH_MMU_H__

#include "los_typedef.h"
#include "los_vm_phys.h"
#ifndef LOSCFG_PAGE_TABLE_FINE_LOCK
#include "los_spinlock.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef struct ArchMmu {
#ifndef LOSCFG_PAGE_TABLE_FINE_LOCK
    SPIN_LOCK_S         lock;           /**< arch mmu page table entry modification spin lock */
#endif
    VADDR_T             *virtTtb;       /**< translation table base virtual addr */
    PADDR_T             physTtb;        /**< translation table base phys addr */
    UINT32              asid;           /**< TLB asid */
    LOS_DL_LIST         ptList;         /**< page table vm page list */
} LosArchMmu;

BOOL OsArchMmuInit(LosArchMmu *archMmu, VADDR_T *virtTtb);
STATUS_T LOS_ArchMmuQuery(const LosArchMmu *archMmu, VADDR_T vaddr, PADDR_T *paddr, UINT32 *flags);
STATUS_T LOS_ArchMmuUnmap(LosArchMmu *archMmu, VADDR_T vaddr, size_t count);
STATUS_T LOS_ArchMmuMap(LosArchMmu *archMmu, VADDR_T vaddr, PADDR_T paddr, size_t count, UINT32 flags);
STATUS_T LOS_ArchMmuChangeProt(LosArchMmu *archMmu, VADDR_T vaddr, size_t count, UINT32 flags);
STATUS_T LOS_ArchMmuMove(LosArchMmu *archMmu, VADDR_T oldVaddr, VADDR_T newVaddr, size_t count, UINT32 flags);
VOID LOS_ArchMmuContextSwitch(LosArchMmu *archMmu);
STATUS_T LOS_ArchMmuDestroy(LosArchMmu *archMmu);
VOID OsArchMmuInitPerCPU(VOID);
VADDR_T *OsGFirstTableGet(VOID);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* __LOS_ARCH_MMU_H__ */

