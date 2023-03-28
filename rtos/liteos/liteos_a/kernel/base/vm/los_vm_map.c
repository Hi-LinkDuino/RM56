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

#include "los_vm_map.h"
#include "los_vm_page.h"
#include "los_vm_phys.h"
#include "los_vm_dump.h"
#include "los_vm_lock.h"
#include "los_vm_zone.h"
#include "los_vm_common.h"
#include "los_vm_filemap.h"
#include "los_vm_shm_pri.h"
#include "los_arch_mmu.h"
#include "los_process_pri.h"
#ifdef LOSCFG_FS_VFS
#include "fs/file.h"
#include "vnode.h"
#endif
#include "los_task.h"
#include "los_memory_pri.h"
#include "los_vm_boot.h"


#ifdef LOSCFG_KERNEL_VM

#define VM_MAP_WASTE_MEM_LEVEL          (PAGE_SIZE >> 2)
LosMux g_vmSpaceListMux;
LOS_DL_LIST_HEAD(g_vmSpaceList);
LosVmSpace g_kVmSpace;
LosVmSpace g_vMallocSpace;

LosVmSpace *LOS_CurrSpaceGet(VOID)
{
    return OsCurrProcessGet()->vmSpace;
}

LosVmSpace *LOS_SpaceGet(VADDR_T vaddr)
{
    if (LOS_IsKernelAddress(vaddr)) {
        return LOS_GetKVmSpace();
    } else if (LOS_IsUserAddress(vaddr)) {
        return LOS_CurrSpaceGet();
    } else if (LOS_IsVmallocAddress(vaddr)) {
        return LOS_GetVmallocSpace();
    } else {
        return NULL;
    }
}

LosVmSpace *LOS_GetKVmSpace(VOID)
{
    return &g_kVmSpace;
}

LOS_DL_LIST *LOS_GetVmSpaceList(VOID)
{
    return &g_vmSpaceList;
}

LosVmSpace *LOS_GetVmallocSpace(VOID)
{
    return &g_vMallocSpace;
}

ULONG_T OsRegionRbFreeFn(LosRbNode *pstNode)
{
    LOS_MemFree(m_aucSysMem0, pstNode);
    return LOS_OK;
}

VOID *OsRegionRbGetKeyFn(LosRbNode *pstNode)
{
    LosVmMapRegion *region = (LosVmMapRegion *)LOS_DL_LIST_ENTRY(pstNode, LosVmMapRegion, rbNode);
    return (VOID *)&region->range;
}

ULONG_T OsRegionRbCmpKeyFn(const VOID *pNodeKeyA, const VOID *pNodeKeyB)
{
    LosVmMapRange rangeA = *(LosVmMapRange *)pNodeKeyA;
    LosVmMapRange rangeB = *(LosVmMapRange *)pNodeKeyB;
    UINT32 startA = rangeA.base;
    UINT32 endA = rangeA.base + rangeA.size - 1;
    UINT32 startB = rangeB.base;
    UINT32 endB = rangeB.base + rangeB.size - 1;

    if (startA > endB) {
        return RB_BIGGER;
    } else if (startA >= startB) {
        if (endA <= endB) {
            return RB_EQUAL;
        } else {
            return RB_BIGGER;
        }
    } else if (startA <= startB) {
        if (endA >= endB) {
            return RB_EQUAL;
        } else {
            return RB_SMALLER;
        }
    } else if (endA < startB) {
        return RB_SMALLER;
    }
    return RB_EQUAL;
}

STATIC BOOL OsVmSpaceInitCommon(LosVmSpace *vmSpace, VADDR_T *virtTtb)
{
    LOS_RbInitTree(&vmSpace->regionRbTree, OsRegionRbCmpKeyFn, OsRegionRbFreeFn, OsRegionRbGetKeyFn);

    status_t retval = LOS_MuxInit(&vmSpace->regionMux, NULL);
    if (retval != LOS_OK) {
        VM_ERR("Create mutex for vm space failed, status: %d", retval);
        return FALSE;
    }

    (VOID)LOS_MuxAcquire(&g_vmSpaceListMux);
    LOS_ListAdd(&g_vmSpaceList, &vmSpace->node);
    (VOID)LOS_MuxRelease(&g_vmSpaceListMux);

    return OsArchMmuInit(&vmSpace->archMmu, virtTtb);
}

VOID OsVmMapInit(VOID)
{
    status_t retval = LOS_MuxInit(&g_vmSpaceListMux, NULL);
    if (retval != LOS_OK) {
        VM_ERR("Create mutex for g_vmSpaceList failed, status: %d", retval);
    }
}

BOOL OsKernVmSpaceInit(LosVmSpace *vmSpace, VADDR_T *virtTtb)
{
    vmSpace->base = KERNEL_ASPACE_BASE;
    vmSpace->size = KERNEL_ASPACE_SIZE;
    vmSpace->mapBase = KERNEL_VMM_BASE;
    vmSpace->mapSize = KERNEL_VMM_SIZE;
#ifdef LOSCFG_DRIVERS_TZDRIVER
    vmSpace->codeStart = 0;
    vmSpace->codeEnd = 0;
#endif
    return OsVmSpaceInitCommon(vmSpace, virtTtb);
}

BOOL OsVMallocSpaceInit(LosVmSpace *vmSpace, VADDR_T *virtTtb)
{
    vmSpace->base = VMALLOC_START;
    vmSpace->size = VMALLOC_SIZE;
    vmSpace->mapBase = VMALLOC_START;
    vmSpace->mapSize = VMALLOC_SIZE;
#ifdef LOSCFG_DRIVERS_TZDRIVER
    vmSpace->codeStart = 0;
    vmSpace->codeEnd = 0;
#endif
    return OsVmSpaceInitCommon(vmSpace, virtTtb);
}

VOID OsKSpaceInit(VOID)
{
    OsVmMapInit();
    OsKernVmSpaceInit(&g_kVmSpace, OsGFirstTableGet());
    OsVMallocSpaceInit(&g_vMallocSpace, OsGFirstTableGet());
}

BOOL OsUserVmSpaceInit(LosVmSpace *vmSpace, VADDR_T *virtTtb)
{
    vmSpace->base = USER_ASPACE_BASE;
    vmSpace->size = USER_ASPACE_SIZE;
    vmSpace->mapBase = USER_MAP_BASE;
    vmSpace->mapSize = USER_MAP_SIZE;
    vmSpace->heapBase = USER_HEAP_BASE;
    vmSpace->heapNow = USER_HEAP_BASE;
    vmSpace->heap = NULL;
#ifdef LOSCFG_DRIVERS_TZDRIVER
    vmSpace->codeStart = 0;
    vmSpace->codeEnd = 0;
#endif
    return OsVmSpaceInitCommon(vmSpace, virtTtb);
}

LosVmSpace *OsCreateUserVmSpace(VOID)
{
    BOOL retVal = FALSE;

    LosVmSpace *space = LOS_MemAlloc(m_aucSysMem0, sizeof(LosVmSpace));
    if (space == NULL) {
        return NULL;
    }

    VADDR_T *ttb = LOS_PhysPagesAllocContiguous(1);
    if (ttb == NULL) {
        (VOID)LOS_MemFree(m_aucSysMem0, space);
        return NULL;
    }

    (VOID)memset_s(ttb, PAGE_SIZE, 0, PAGE_SIZE);
    retVal = OsUserVmSpaceInit(space, ttb);
    LosVmPage *vmPage = OsVmVaddrToPage(ttb);
    if ((retVal == FALSE) || (vmPage == NULL)) {
        (VOID)LOS_MemFree(m_aucSysMem0, space);
        LOS_PhysPagesFreeContiguous(ttb, 1);
        return NULL;
    }
    LOS_ListAdd(&space->archMmu.ptList, &(vmPage->node));

    return space;
}

STATIC BOOL OsVmSpaceParamCheck(LosVmSpace *vmSpace)
{
    if (vmSpace == NULL) {
        return FALSE;
    }
    return TRUE;
}

STATUS_T LOS_VmSpaceClone(LosVmSpace *oldVmSpace, LosVmSpace *newVmSpace)
{
    LosVmMapRegion *oldRegion = NULL;
    LosVmMapRegion *newRegion = NULL;
    LosRbNode *pstRbNode = NULL;
    LosRbNode *pstRbNodeNext = NULL;
    STATUS_T ret = LOS_OK;
    UINT32 numPages;
    PADDR_T paddr;
    VADDR_T vaddr;
    UINT32 intSave;
    LosVmPage *page = NULL;
    UINT32 flags;
    UINT32 i;

    if ((OsVmSpaceParamCheck(oldVmSpace) == FALSE) || (OsVmSpaceParamCheck(newVmSpace) == FALSE)) {
        return LOS_ERRNO_VM_INVALID_ARGS;
    }

    if ((OsIsVmRegionEmpty(oldVmSpace) == TRUE) || (oldVmSpace == &g_kVmSpace)) {
        return LOS_ERRNO_VM_INVALID_ARGS;
    }

    /* search the region list */
    newVmSpace->mapBase = oldVmSpace->mapBase;
    newVmSpace->heapBase = oldVmSpace->heapBase;
    newVmSpace->heapNow = oldVmSpace->heapNow;
    (VOID)LOS_MuxAcquire(&oldVmSpace->regionMux);
    RB_SCAN_SAFE(&oldVmSpace->regionRbTree, pstRbNode, pstRbNodeNext)
        oldRegion = (LosVmMapRegion *)pstRbNode;
        newRegion = OsVmRegionDup(newVmSpace, oldRegion, oldRegion->range.base, oldRegion->range.size);
        if (newRegion == NULL) {
            VM_ERR("dup new region failed");
            ret = LOS_ERRNO_VM_NO_MEMORY;
            break;
        }

#ifdef LOSCFG_KERNEL_SHM
        if (oldRegion->regionFlags & VM_MAP_REGION_FLAG_SHM) {
            OsShmFork(newVmSpace, oldRegion, newRegion);
            continue;
        }
#endif

        if (oldRegion == oldVmSpace->heap) {
            newVmSpace->heap = newRegion;
        }

        numPages = newRegion->range.size >> PAGE_SHIFT;
        for (i = 0; i < numPages; i++) {
            vaddr = newRegion->range.base + (i << PAGE_SHIFT);
            if (LOS_ArchMmuQuery(&oldVmSpace->archMmu, vaddr, &paddr, &flags) != LOS_OK) {
                continue;
            }

            page = LOS_VmPageGet(paddr);
            if (page != NULL) {
                LOS_AtomicInc(&page->refCounts);
            }
            if (flags & VM_MAP_REGION_FLAG_PERM_WRITE) {
                LOS_ArchMmuUnmap(&oldVmSpace->archMmu, vaddr, 1);
                LOS_ArchMmuMap(&oldVmSpace->archMmu, vaddr, paddr, 1, flags & ~VM_MAP_REGION_FLAG_PERM_WRITE);
            }
            LOS_ArchMmuMap(&newVmSpace->archMmu, vaddr, paddr, 1, flags & ~VM_MAP_REGION_FLAG_PERM_WRITE);

#ifdef LOSCFG_FS_VFS
            if (LOS_IsRegionFileValid(oldRegion)) {
                LosFilePage *fpage = NULL;
                LOS_SpinLockSave(&oldRegion->unTypeData.rf.vnode->mapping.list_lock, &intSave);
                fpage = OsFindGetEntry(&oldRegion->unTypeData.rf.vnode->mapping, newRegion->pgOff + i);
                if ((fpage != NULL) && (fpage->vmPage == page)) { /* cow page no need map */
                    OsAddMapInfo(fpage, &newVmSpace->archMmu, vaddr);
                }
                LOS_SpinUnlockRestore(&oldRegion->unTypeData.rf.vnode->mapping.list_lock, intSave);
            }
#endif
        }
    RB_SCAN_SAFE_END(&oldVmSpace->regionRbTree, pstRbNode, pstRbNodeNext)
    (VOID)LOS_MuxRelease(&oldVmSpace->regionMux);
    return ret;
}

LosVmMapRegion *OsFindRegion(LosRbTree *regionRbTree, VADDR_T vaddr, size_t len)
{
    LosVmMapRegion *regionRst = NULL;
    LosRbNode *pstRbNode = NULL;
    LosVmMapRange rangeKey;
    rangeKey.base = vaddr;
    rangeKey.size = len;

    if (LOS_RbGetNode(regionRbTree, (VOID *)&rangeKey, &pstRbNode)) {
        regionRst = (LosVmMapRegion *)LOS_DL_LIST_ENTRY(pstRbNode, LosVmMapRegion, rbNode);
    }
    return regionRst;
}

LosVmMapRegion *LOS_RegionFind(LosVmSpace *vmSpace, VADDR_T addr)
{
    LosVmMapRegion *region = NULL;

    (VOID)LOS_MuxAcquire(&vmSpace->regionMux);
    region = OsFindRegion(&vmSpace->regionRbTree, addr, 1);
    (VOID)LOS_MuxRelease(&vmSpace->regionMux);

    return region;
}

LosVmMapRegion *LOS_RegionRangeFind(LosVmSpace *vmSpace, VADDR_T addr, size_t len)
{
    LosVmMapRegion *region = NULL;

    (VOID)LOS_MuxAcquire(&vmSpace->regionMux);
    region = OsFindRegion(&vmSpace->regionRbTree, addr, len);
    (VOID)LOS_MuxRelease(&vmSpace->regionMux);

    return region;
}

VADDR_T OsAllocRange(LosVmSpace *vmSpace, size_t len)
{
    LosVmMapRegion *curRegion = NULL;
    LosRbNode *pstRbNode = NULL;
    LosRbNode *pstRbNodeTmp = NULL;
    LosRbTree *regionRbTree = &vmSpace->regionRbTree;
    VADDR_T curEnd = vmSpace->mapBase;
    VADDR_T nextStart;

    curRegion = LOS_RegionFind(vmSpace, vmSpace->mapBase);
    if (curRegion != NULL) {
        pstRbNode = &curRegion->rbNode;
        curEnd = curRegion->range.base + curRegion->range.size;
        RB_MID_SCAN(regionRbTree, pstRbNode)
            curRegion = (LosVmMapRegion *)pstRbNode;
            nextStart = curRegion->range.base;
            if (nextStart < curEnd) {
                continue;
            }
            if ((nextStart - curEnd) >= len) {
                return curEnd;
            } else {
                curEnd = curRegion->range.base + curRegion->range.size;
            }
        RB_MID_SCAN_END(regionRbTree, pstRbNode)
    } else {
        /* rbtree scan is sorted, from small to big */
        RB_SCAN_SAFE(regionRbTree, pstRbNode, pstRbNodeTmp)
            curRegion = (LosVmMapRegion *)pstRbNode;
            nextStart = curRegion->range.base;
            if (nextStart < curEnd) {
                continue;
            }
            if ((nextStart - curEnd) >= len) {
                return curEnd;
            } else {
                curEnd = curRegion->range.base + curRegion->range.size;
            }
        RB_SCAN_SAFE_END(regionRbTree, pstRbNode, pstRbNodeTmp)
    }

    nextStart = vmSpace->mapBase + vmSpace->mapSize;
    if ((nextStart >= curEnd) && ((nextStart - curEnd) >= len)) {
        return curEnd;
    }

    return 0;
}

VADDR_T OsAllocSpecificRange(LosVmSpace *vmSpace, VADDR_T vaddr, size_t len, UINT32 regionFlags)
{
    STATUS_T status;

    if (LOS_IsRangeInSpace(vmSpace, vaddr, len) == FALSE) {
        return 0;
    }

    if ((LOS_RegionFind(vmSpace, vaddr) != NULL) ||
        (LOS_RegionFind(vmSpace, vaddr + len - 1) != NULL) ||
        (LOS_RegionRangeFind(vmSpace, vaddr, len - 1) != NULL)) {
        if ((regionFlags & VM_MAP_REGION_FLAG_FIXED_NOREPLACE) != 0) {
            return 0;
        } else if ((regionFlags & VM_MAP_REGION_FLAG_FIXED) != 0) {
            status = LOS_UnMMap(vaddr, len);
            if (status != LOS_OK) {
                VM_ERR("unmap specific range va: %#x, len: %#x failed, status: %d", vaddr, len, status);
                return 0;
            }
        } else {
            return OsAllocRange(vmSpace, len);
        }
    }

    return vaddr;
}

BOOL LOS_IsRegionFileValid(LosVmMapRegion *region)
{
    if ((region != NULL) && (LOS_IsRegionTypeFile(region)) &&
        (region->unTypeData.rf.vnode != NULL)) {
        return TRUE;
    }
    return FALSE;
}

BOOL OsInsertRegion(LosRbTree *regionRbTree, LosVmMapRegion *region)
{
    if (LOS_RbAddNode(regionRbTree, (LosRbNode *)region) == FALSE) {
        VM_ERR("insert region failed, base: %#x, size: %#x", region->range.base, region->range.size);
        OsDumpAspace(region->space);
        return FALSE;
    }
    return TRUE;
}

LosVmMapRegion *OsCreateRegion(VADDR_T vaddr, size_t len, UINT32 regionFlags, unsigned long offset)
{
    LosVmMapRegion *region = LOS_MemAlloc(m_aucSysMem0, sizeof(LosVmMapRegion));
    if (region == NULL) {
        VM_ERR("memory allocate for LosVmMapRegion failed");
        return region;
    }

    memset_s(region, sizeof(LosVmMapRegion), 0, sizeof(LosVmMapRegion));
    region->range.base = vaddr;
    region->range.size = len;
    region->pgOff = offset;
    region->regionFlags = regionFlags;
    region->regionType = VM_MAP_REGION_TYPE_NONE;
    region->forkFlags = 0;
    region->shmid = -1;
    return region;
}

PADDR_T LOS_PaddrQuery(VOID *vaddr)
{
    PADDR_T paddr = 0;
    STATUS_T status;
    LosVmSpace *space = NULL;
    LosArchMmu *archMmu = NULL;

    if (LOS_IsKernelAddress((VADDR_T)(UINTPTR)vaddr)) {
        archMmu = &g_kVmSpace.archMmu;
    } else if (LOS_IsUserAddress((VADDR_T)(UINTPTR)vaddr)) {
        space = OsCurrProcessGet()->vmSpace;
        archMmu = &space->archMmu;
    } else if (LOS_IsVmallocAddress((VADDR_T)(UINTPTR)vaddr)) {
        archMmu = &g_vMallocSpace.archMmu;
    } else {
        VM_ERR("vaddr is beyond range");
        return 0;
    }

    status = LOS_ArchMmuQuery(archMmu, (VADDR_T)(UINTPTR)vaddr, &paddr, 0);
    if (status == LOS_OK) {
        return paddr;
    } else {
        return 0;
    }
}

LosVmMapRegion *LOS_RegionAlloc(LosVmSpace *vmSpace, VADDR_T vaddr, size_t len, UINT32 regionFlags, VM_OFFSET_T pgoff)
{
    VADDR_T rstVaddr;
    LosVmMapRegion *newRegion = NULL;
    BOOL isInsertSucceed = FALSE;
    /**
     * If addr is NULL, then the kernel chooses the address at which to create the mapping;
     * this is the most portable method of creating a new mapping.  If addr is not NULL,
     * then the kernel takes it as where to place the mapping;
     */
    (VOID)LOS_MuxAcquire(&vmSpace->regionMux);
    if (vaddr == 0) {
        rstVaddr = OsAllocRange(vmSpace, len);
    } else {
        /* if it is already mmapped here, we unmmap it */
        rstVaddr = OsAllocSpecificRange(vmSpace, vaddr, len, regionFlags);
        if (rstVaddr == 0) {
            VM_ERR("alloc specific range va: %#x, len: %#x failed", vaddr, len);
            goto OUT;
        }
    }
    if (rstVaddr == 0) {
        goto OUT;
    }

    newRegion = OsCreateRegion(rstVaddr, len, regionFlags, pgoff);
    if (newRegion == NULL) {
        goto OUT;
    }
    newRegion->space = vmSpace;
    isInsertSucceed = OsInsertRegion(&vmSpace->regionRbTree, newRegion);
    if (isInsertSucceed == FALSE) {
        (VOID)LOS_MemFree(m_aucSysMem0, newRegion);
        newRegion = NULL;
    }

OUT:
    (VOID)LOS_MuxRelease(&vmSpace->regionMux);
    return newRegion;
}

STATIC VOID OsAnonPagesRemove(LosArchMmu *archMmu, VADDR_T vaddr, UINT32 count)
{
    status_t status;
    paddr_t paddr;
    LosVmPage *page = NULL;

    if ((archMmu == NULL) || (vaddr == 0) || (count == 0)) {
        VM_ERR("OsAnonPagesRemove invalid args, archMmu %p, vaddr %p, count %d", archMmu, vaddr, count);
        return;
    }

    while (count > 0) {
        count--;
        status = LOS_ArchMmuQuery(archMmu, vaddr, &paddr, NULL);
        if (status != LOS_OK) {
            vaddr += PAGE_SIZE;
            continue;
        }

        LOS_ArchMmuUnmap(archMmu, vaddr, 1);

        page = LOS_VmPageGet(paddr);
        if (page != NULL) {
            if (!OsIsPageShared(page)) {
                LOS_PhysPageFree(page);
            }
        }
        vaddr += PAGE_SIZE;
    }
}

STATIC VOID OsDevPagesRemove(LosArchMmu *archMmu, VADDR_T vaddr, UINT32 count)
{
    status_t status;

    if ((archMmu == NULL) || (vaddr == 0) || (count == 0)) {
        VM_ERR("OsDevPagesRemove invalid args, archMmu %p, vaddr %p, count %d", archMmu, vaddr, count);
        return;
    }

    status = LOS_ArchMmuQuery(archMmu, vaddr, NULL, NULL);
    if (status != LOS_OK) {
        return;
    }

    /* in order to unmap section */
    LOS_ArchMmuUnmap(archMmu, vaddr, count);
}

#ifdef LOSCFG_FS_VFS
STATIC VOID OsFilePagesRemove(LosVmSpace *space, LosVmMapRegion *region)
{
    VM_OFFSET_T offset;
    size_t size;

    if ((space == NULL) || (region == NULL) || (region->unTypeData.rf.vmFOps == NULL)) {
        return;
    }

    offset = region->pgOff;
    size = region->range.size;
    while (size >= PAGE_SIZE) {
        region->unTypeData.rf.vmFOps->remove(region, &space->archMmu, offset);
        offset++;
        size -= PAGE_SIZE;
    }
}
#endif

STATUS_T LOS_RegionFree(LosVmSpace *space, LosVmMapRegion *region)
{
    if ((space == NULL) || (region == NULL)) {
        VM_ERR("args error, aspace %p, region %p", space, region);
        return LOS_ERRNO_VM_INVALID_ARGS;
    }

    (VOID)LOS_MuxAcquire(&space->regionMux);

#ifdef LOSCFG_FS_VFS
    if (LOS_IsRegionFileValid(region)) {
        OsFilePagesRemove(space, region);
        VnodeHold();
        region->unTypeData.rf.vnode->useCount--;
        VnodeDrop();
    } else
#endif

#ifdef LOSCFG_KERNEL_SHM
    if (OsIsShmRegion(region)) {
        OsShmRegionFree(space, region);
    } else if (LOS_IsRegionTypeDev(region)) {
#else
    if (LOS_IsRegionTypeDev(region)) {
#endif
        OsDevPagesRemove(&space->archMmu, region->range.base, region->range.size >> PAGE_SHIFT);
    } else {
        OsAnonPagesRemove(&space->archMmu, region->range.base, region->range.size >> PAGE_SHIFT);
    }

    /* remove it from space */
    LOS_RbDelNode(&space->regionRbTree, &region->rbNode);
    /* free it */
    LOS_MemFree(m_aucSysMem0, region);
    (VOID)LOS_MuxRelease(&space->regionMux);
    return LOS_OK;
}

LosVmMapRegion *OsVmRegionDup(LosVmSpace *space, LosVmMapRegion *oldRegion, VADDR_T vaddr, size_t size)
{
    LosVmMapRegion *newRegion = NULL;
    UINT32 regionFlags;

    (VOID)LOS_MuxAcquire(&space->regionMux);
    regionFlags = oldRegion->regionFlags;
    if (vaddr == 0) {
        regionFlags &= ~(VM_MAP_REGION_FLAG_FIXED | VM_MAP_REGION_FLAG_FIXED_NOREPLACE);
    } else {
        regionFlags |= VM_MAP_REGION_FLAG_FIXED;
    }
    newRegion = LOS_RegionAlloc(space, vaddr, size, regionFlags, oldRegion->pgOff);
    if (newRegion == NULL) {
        VM_ERR("LOS_RegionAlloc failed");
        goto REGIONDUPOUT;
    }
    newRegion->regionType = oldRegion->regionType;

#ifdef LOSCFG_KERNEL_SHM
    if (OsIsShmRegion(oldRegion)) {
        newRegion->shmid = oldRegion->shmid;
    }
#endif

#ifdef LOSCFG_FS_VFS
    if (LOS_IsRegionTypeFile(oldRegion)) {
        newRegion->unTypeData.rf.vmFOps = oldRegion->unTypeData.rf.vmFOps;
        newRegion->unTypeData.rf.vnode = oldRegion->unTypeData.rf.vnode;
        newRegion->unTypeData.rf.f_oflags = oldRegion->unTypeData.rf.f_oflags;
        VnodeHold();
        newRegion->unTypeData.rf.vnode->useCount++;
        VnodeDrop();
    }
#endif

REGIONDUPOUT:
    (VOID)LOS_MuxRelease(&space->regionMux);
    return newRegion;
}

STATIC LosVmMapRegion *OsVmRegionSplit(LosVmMapRegion *oldRegion, VADDR_T newRegionStart)
{
    LosVmMapRegion *newRegion = NULL;
    LosVmSpace *space = oldRegion->space;
    size_t size = LOS_RegionSize(newRegionStart, LOS_RegionEndAddr(oldRegion));

    oldRegion->range.size = LOS_RegionSize(oldRegion->range.base, newRegionStart - 1);
    if (oldRegion->range.size == 0) {
        LOS_RbDelNode(&space->regionRbTree, &oldRegion->rbNode);
    }

    newRegion = OsVmRegionDup(oldRegion->space, oldRegion, newRegionStart, size);
    if (newRegion == NULL) {
        VM_ERR("OsVmRegionDup fail");
        return NULL;
    }
#ifdef LOSCFG_FS_VFS
    newRegion->pgOff = oldRegion->pgOff + ((newRegionStart - oldRegion->range.base) >> PAGE_SHIFT);
#endif
    return newRegion;
}

STATUS_T OsVmRegionAdjust(LosVmSpace *space, VADDR_T newRegionStart, size_t size)
{
    LosVmMapRegion *region = NULL;
    VADDR_T nextRegionBase = newRegionStart + size;
    LosVmMapRegion *newRegion = NULL;

    region = LOS_RegionFind(space, newRegionStart);
    if ((region != NULL) && (newRegionStart > region->range.base)) {
        newRegion = OsVmRegionSplit(region, newRegionStart);
        if (newRegion == NULL) {
            VM_ERR("region split fail");
            return LOS_ERRNO_VM_NO_MEMORY;
        }
    }

    region = LOS_RegionFind(space, nextRegionBase - 1);
    if ((region != NULL) && (nextRegionBase < LOS_RegionEndAddr(region))) {
        newRegion = OsVmRegionSplit(region, nextRegionBase);
        if (newRegion == NULL) {
            VM_ERR("region split fail");
            return LOS_ERRNO_VM_NO_MEMORY;
        }
    }

    return LOS_OK;
}

STATUS_T OsRegionsRemove(LosVmSpace *space, VADDR_T regionBase, size_t size)
{
    STATUS_T status;
    VADDR_T regionEnd = regionBase + size - 1;
    LosVmMapRegion *regionTemp = NULL;
    LosRbNode *pstRbNodeTemp = NULL;
    LosRbNode *pstRbNodeNext = NULL;

    (VOID)LOS_MuxAcquire(&space->regionMux);

    status = OsVmRegionAdjust(space, regionBase, size);
    if (status != LOS_OK) {
        goto ERR_REGION_SPLIT;
    }

    RB_SCAN_SAFE(&space->regionRbTree, pstRbNodeTemp, pstRbNodeNext)
        regionTemp = (LosVmMapRegion *)pstRbNodeTemp;
        if (regionTemp->range.base > regionEnd) {
            break;
        }
        if (regionBase <= regionTemp->range.base && regionEnd >= LOS_RegionEndAddr(regionTemp)) {
            status = LOS_RegionFree(space, regionTemp);
            if (status != LOS_OK) {
                VM_ERR("fail to free region, status=%d", status);
                goto ERR_REGION_SPLIT;
            }
        }

    RB_SCAN_SAFE_END(&space->regionRbTree, pstRbNodeTemp, pstRbNodeNext)

ERR_REGION_SPLIT:
    (VOID)LOS_MuxRelease(&space->regionMux);
    return status;
}

INT32 OsUserHeapFree(LosVmSpace *vmSpace, VADDR_T addr, size_t len)
{
    LosVmMapRegion *vmRegion = NULL;
    LosVmPage *vmPage = NULL;
    PADDR_T paddr = 0;
    VADDR_T vaddr;
    STATUS_T ret;

    if (vmSpace == LOS_GetKVmSpace() || vmSpace->heap == NULL) {
        return -1;
    }

    vmRegion = LOS_RegionFind(vmSpace, addr);
    if (vmRegion == NULL) {
        return -1;
    }

    if (vmRegion == vmSpace->heap) {
        vaddr = addr;
        while (len > 0) {
            if (LOS_ArchMmuQuery(&vmSpace->archMmu, vaddr, &paddr, 0) == LOS_OK) {
                ret = LOS_ArchMmuUnmap(&vmSpace->archMmu, vaddr, 1);
                if (ret <= 0) {
                    VM_ERR("unmap failed, ret = %d", ret);
                }
                vmPage = LOS_VmPageGet(paddr);
                LOS_PhysPageFree(vmPage);
            }
            vaddr += PAGE_SIZE;
            len -= PAGE_SIZE;
        }
        return 0;
    }

    return -1;
}

STATUS_T OsIsRegionCanExpand(LosVmSpace *space, LosVmMapRegion *region, size_t size)
{
    LosVmMapRegion *nextRegion = NULL;

    if ((space == NULL) || (region == NULL)) {
        return LOS_NOK;
    }

    nextRegion = (LosVmMapRegion *)LOS_RbSuccessorNode(&space->regionRbTree, &region->rbNode);
    /* if the gap is larger than size, then we can expand */
    if ((nextRegion != NULL) && ((nextRegion->range.base - region->range.base) >= size)) {
        return LOS_OK;
    }

    return LOS_NOK;
}

STATUS_T OsUnMMap(LosVmSpace *space, VADDR_T addr, size_t size)
{
    size = LOS_Align(size, PAGE_SIZE);
    addr = LOS_Align(addr, PAGE_SIZE);
    (VOID)LOS_MuxAcquire(&space->regionMux);
    STATUS_T status = OsRegionsRemove(space, addr, size);
    if (status != LOS_OK) {
        status = -EINVAL;
        VM_ERR("region_split failed");
        goto ERR_REGION_SPLIT;
    }

ERR_REGION_SPLIT:
    (VOID)LOS_MuxRelease(&space->regionMux);
    return status;
}

STATIC VOID OsVmSpaceAllRegionFree(LosVmSpace *space)
{
    LosRbNode *pstRbNode = NULL;
    LosRbNode *pstRbNodeNext = NULL;

    /* free all of the regions */
    RB_SCAN_SAFE(&space->regionRbTree, pstRbNode, pstRbNodeNext)
        LosVmMapRegion *region = (LosVmMapRegion *)pstRbNode;
        if (region->range.size == 0) {
            VM_ERR("space free, region: %#x flags: %#x, base:%#x, size: %#x",
                   region, region->regionFlags, region->range.base, region->range.size);
        }
        STATUS_T ret = LOS_RegionFree(space, region);
        if (ret != LOS_OK) {
            VM_ERR("free region error, space %p, region %p", space, region);
        }
    RB_SCAN_SAFE_END(&space->regionRbTree, pstRbNode, pstRbNodeNext)

    return;
}

STATUS_T OsVmSpaceRegionFree(LosVmSpace *space)
{
    if (space == NULL) {
        return LOS_ERRNO_VM_INVALID_ARGS;
    }

    if (space == &g_kVmSpace) {
        VM_ERR("try to free kernel aspace, not allowed");
        return LOS_OK;
    }

    (VOID)LOS_MuxAcquire(&space->regionMux);
    OsVmSpaceAllRegionFree(space);
    (VOID)LOS_MuxRelease(&space->regionMux);

    return LOS_OK;
}

STATUS_T LOS_VmSpaceFree(LosVmSpace *space)
{
    if (space == NULL) {
        return LOS_ERRNO_VM_INVALID_ARGS;
    }

    if (space == &g_kVmSpace) {
        VM_ERR("try to free kernel aspace, not allowed");
        return LOS_OK;
    }

    /* pop it out of the global aspace list */
    (VOID)LOS_MuxAcquire(&space->regionMux);

    LOS_ListDelete(&space->node);

    OsVmSpaceAllRegionFree(space);

    /* make sure the current thread does not map the aspace */
    LosProcessCB *currentProcess = OsCurrProcessGet();
    if (currentProcess->vmSpace == space) {
        LOS_TaskLock();
        currentProcess->vmSpace = NULL;
        LOS_ArchMmuContextSwitch(&space->archMmu);
        LOS_TaskUnlock();
    }

    /* destroy the arch portion of the space */
    LOS_ArchMmuDestroy(&space->archMmu);

    (VOID)LOS_MuxRelease(&space->regionMux);
    (VOID)LOS_MuxDestroy(&space->regionMux);

    /* free the aspace */
    LOS_MemFree(m_aucSysMem0, space);
    return LOS_OK;
}

BOOL LOS_IsRangeInSpace(const LosVmSpace *space, VADDR_T vaddr, size_t size)
{
    /* is the starting address within the address space */
    if (vaddr < space->base || vaddr > space->base + space->size - 1) {
        return FALSE;
    }
    if (size == 0) {
        return TRUE;
    }
    /* see if the size is enough to wrap the integer */
    if (vaddr + size - 1 < vaddr) {
        return FALSE;
    }
    /* see if the end address is within the address space's */
    if (vaddr + size - 1 > space->base + space->size - 1) {
        return FALSE;
    }
    return TRUE;
}

STATUS_T LOS_VmSpaceReserve(LosVmSpace *space, size_t size, VADDR_T vaddr)
{
    UINT32 regionFlags = 0;

    if ((space == NULL) || (size == 0) || (!IS_PAGE_ALIGNED(vaddr) || !IS_PAGE_ALIGNED(size))) {
        return LOS_ERRNO_VM_INVALID_ARGS;
    }

    if (!LOS_IsRangeInSpace(space, vaddr, size)) {
        return LOS_ERRNO_VM_OUT_OF_RANGE;
    }

    /* lookup how it's already mapped */
    (VOID)LOS_ArchMmuQuery(&space->archMmu, vaddr, NULL, &regionFlags);

    /* build a new region structure */
    LosVmMapRegion *region = LOS_RegionAlloc(space, vaddr, size, regionFlags | VM_MAP_REGION_FLAG_FIXED, 0);

    return region ? LOS_OK : LOS_ERRNO_VM_NO_MEMORY;
}

STATUS_T LOS_VaddrToPaddrMmap(LosVmSpace *space, VADDR_T vaddr, PADDR_T paddr, size_t len, UINT32 flags)
{
    STATUS_T ret;
    LosVmMapRegion *region = NULL;
    LosVmPage *vmPage = NULL;

    if ((vaddr != ROUNDUP(vaddr, PAGE_SIZE)) ||
        (paddr != ROUNDUP(paddr, PAGE_SIZE)) ||
        (len != ROUNDUP(len, PAGE_SIZE))) {
        VM_ERR("vaddr :0x%x  paddr:0x%x len: 0x%x not page size align", vaddr, paddr, len);
        return LOS_ERRNO_VM_NOT_VALID;
    }

    if (space == NULL) {
        space = OsCurrProcessGet()->vmSpace;
    }

    region = LOS_RegionFind(space, vaddr);
    if (region != NULL) {
        VM_ERR("vaddr : 0x%x already used!", vaddr);
        return LOS_ERRNO_VM_BUSY;
    }

    region = LOS_RegionAlloc(space, vaddr, len, flags, 0);
    if (region == NULL) {
        VM_ERR("failed");
        return LOS_ERRNO_VM_NO_MEMORY;
    }

    while (len > 0) {
        vmPage = LOS_VmPageGet(paddr);
        if (vmPage == NULL) {
            LOS_RegionFree(space, region);
            VM_ERR("Page is NULL");
            return LOS_ERRNO_VM_NOT_VALID;
        }
        LOS_AtomicInc(&vmPage->refCounts);

        ret = LOS_ArchMmuMap(&space->archMmu, vaddr, paddr, 1, region->regionFlags);
        if (ret <= 0) {
            VM_ERR("LOS_ArchMmuMap failed: %d", ret);
            LOS_RegionFree(space, region);
            return ret;
        }

        paddr += PAGE_SIZE;
        vaddr += PAGE_SIZE;
        len -= PAGE_SIZE;
    }
    return LOS_OK;
}

VOID *LOS_VMalloc(size_t size)
{
    LosVmSpace *space = &g_vMallocSpace;
    LosVmMapRegion *region = NULL;
    size_t sizeCount;
    size_t count;
    LosVmPage *vmPage = NULL;
    VADDR_T va;
    PADDR_T pa;
    STATUS_T ret;

    size = LOS_Align(size, PAGE_SIZE);
    if ((size == 0) || (size > space->size)) {
        return NULL;
    }
    sizeCount = size >> PAGE_SHIFT;

    LOS_DL_LIST_HEAD(pageList);
    (VOID)LOS_MuxAcquire(&space->regionMux);

    count = LOS_PhysPagesAlloc(sizeCount, &pageList);
    if (count < sizeCount) {
        VM_ERR("failed to allocate enough pages (ask %zu, got %zu)", sizeCount, count);
        goto ERROR;
    }

    /* allocate a region and put it in the aspace list */
    region = LOS_RegionAlloc(space, 0, size, VM_MAP_REGION_FLAG_PERM_READ | VM_MAP_REGION_FLAG_PERM_WRITE, 0);
    if (region == NULL) {
        VM_ERR("alloc region failed, size = %x", size);
        goto ERROR;
    }

    va = region->range.base;
    while ((vmPage = LOS_ListRemoveHeadType(&pageList, LosVmPage, node))) {
        pa = vmPage->physAddr;
        LOS_AtomicInc(&vmPage->refCounts);
        ret = LOS_ArchMmuMap(&space->archMmu, va, pa, 1, region->regionFlags);
        if (ret != 1) {
            VM_ERR("LOS_ArchMmuMap failed!, err;%d", ret);
        }
        va += PAGE_SIZE;
    }

    (VOID)LOS_MuxRelease(&space->regionMux);
    return (VOID *)(UINTPTR)region->range.base;

ERROR:
    (VOID)LOS_PhysPagesFree(&pageList);
    (VOID)LOS_MuxRelease(&space->regionMux);
    return NULL;
}

VOID LOS_VFree(const VOID *addr)
{
    LosVmSpace *space = &g_vMallocSpace;
    LosVmMapRegion *region = NULL;
    STATUS_T ret;

    if (addr == NULL) {
        VM_ERR("addr is NULL!");
        return;
    }

    (VOID)LOS_MuxAcquire(&space->regionMux);

    region = LOS_RegionFind(space, (VADDR_T)(UINTPTR)addr);
    if (region == NULL) {
        VM_ERR("find region failed");
        goto DONE;
    }

    ret = LOS_RegionFree(space, region);
    if (ret) {
        VM_ERR("free region failed, ret = %d", ret);
    }

DONE:
    (VOID)LOS_MuxRelease(&space->regionMux);
}

LosMux *OsGVmSpaceMuxGet(VOID)
{
    return &g_vmSpaceListMux;
}

STATIC INLINE BOOL OsMemLargeAlloc(UINT32 size)
{
    if (g_kHeapInited == FALSE) {
        return FALSE;
    }

    if (size < KMALLOC_LARGE_SIZE) {
        return FALSE;
    }

    return TRUE;
}
#else
PADDR_T LOS_PaddrQuery(VOID *vaddr)
{
    if (!LOS_IsKernelAddress((VADDR_T)vaddr)) {
        return 0;
    }

    return (PADDR_T)VMM_TO_DMA_ADDR((VADDR_T)vaddr);
}
#endif

VOID *LOS_KernelMalloc(UINT32 size)
{
    VOID *ptr = NULL;

#ifdef LOSCFG_KERNEL_VM
    if (OsMemLargeAlloc(size)) {
        ptr = LOS_PhysPagesAllocContiguous(ROUNDUP(size, PAGE_SIZE) >> PAGE_SHIFT);
    } else
#endif
    {
        ptr = LOS_MemAlloc(OS_SYS_MEM_ADDR, size);
    }

    return ptr;
}

VOID *LOS_KernelMallocAlign(UINT32 size, UINT32 boundary)
{
    VOID *ptr = NULL;

#ifdef LOSCFG_KERNEL_VM
    if (OsMemLargeAlloc(size) && IS_ALIGNED(PAGE_SIZE, boundary)) {
        ptr = LOS_PhysPagesAllocContiguous(ROUNDUP(size, PAGE_SIZE) >> PAGE_SHIFT);
    } else
#endif
    {
        ptr = LOS_MemAllocAlign(OS_SYS_MEM_ADDR, size, boundary);
    }

    return ptr;
}

VOID *LOS_KernelRealloc(VOID *ptr, UINT32 size)
{
    VOID *tmpPtr = NULL;

#ifdef LOSCFG_KERNEL_VM
    LosVmPage *page = NULL;
    errno_t ret;
    if (ptr == NULL) {
        tmpPtr = LOS_KernelMalloc(size);
    } else {
        if (OsMemIsHeapNode(ptr) == FALSE) {
            page = OsVmVaddrToPage(ptr);
            if (page == NULL) {
                VM_ERR("page of ptr(%#x) is null", ptr);
                return NULL;
            }
            tmpPtr = LOS_KernelMalloc(size);
            if (tmpPtr == NULL) {
                VM_ERR("alloc memory failed");
                return NULL;
            }
            ret = memcpy_s(tmpPtr, size, ptr, page->nPages << PAGE_SHIFT);
            if (ret != EOK) {
                LOS_KernelFree(tmpPtr);
                VM_ERR("KernelRealloc memcpy error");
                return NULL;
            }
            OsMemLargeNodeFree(ptr);
        } else {
            tmpPtr = LOS_MemRealloc(OS_SYS_MEM_ADDR, ptr, size);
        }
    }
#else
    tmpPtr = LOS_MemRealloc(OS_SYS_MEM_ADDR, ptr, size);
#endif

    return tmpPtr;
}

VOID LOS_KernelFree(VOID *ptr)
{
#ifdef LOSCFG_KERNEL_VM
    UINT32 ret;
    if (OsMemIsHeapNode(ptr) == FALSE) {
        ret = OsMemLargeNodeFree(ptr);
        if (ret != LOS_OK) {
            VM_ERR("KernelFree %p failed", ptr);
            return;
        }
    } else
#endif
    {
        (VOID)LOS_MemFree(OS_SYS_MEM_ADDR, ptr);
    }
}
