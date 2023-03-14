/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#include "btm_snoop_filter.h"

#include <log.h>
#include <securec.h>

#include "gap/gap_def.h"
#include "hci/hci.h"
#include "hci/hci_error.h"
#include "platform/include/allocator.h"
#include "platform/include/list.h"
#include "platform/include/mutex.h"

#include "btm.h"

#define L2CAP_DYNAMIC_PSM_NUM_BASE 0x1000

#define HCI_H4_HEADER_LEN 1

static bool g_filter = false;
static Mutex *g_filterMutex = NULL;
static List *g_filterInfoList = NULL;

static void FreeListNodeData(void *data)
{
    MEM_MALLOC.free(data);
}

List *BtmGetFilterInfoList(void)
{
    return g_filterInfoList;
}

void BtmInitSnoopFilter(void)
{
    g_filter = false;
    g_filterMutex = MutexCreate();
}

void BtmCloseSnoopFilter(void)
{
    g_filter = false;
    if (g_filterMutex != NULL) {
        MutexDelete(g_filterMutex);
        g_filterMutex = NULL;
    }
}

void BtmEnableSnoopFilter(void)
{
    MutexLock(g_filterMutex);
    if (g_filter) {
        MutexUnlock(g_filterMutex);
        return;
    }

    g_filter = true;

    if (g_filterInfoList == NULL) {
        g_filterInfoList = ListCreate(FreeListNodeData);
    }

    BtmEnableSnoopFilterAcl();
    MutexUnlock(g_filterMutex);
}

void BtmDisableSnoopFilter(void)
{
    MutexLock(g_filterMutex);
    if (g_filter) {
        MutexUnlock(g_filterMutex);
        return;
    }

    g_filter = false;

    BtmDisableSnoopFilterAcl();

    if (g_filterInfoList != NULL) {
        ListDelete(g_filterInfoList);
        g_filterInfoList = NULL;
    }
    MutexUnlock(g_filterMutex);
}

void FilterData(uint8_t *data, uint8_t length)
{
    (void)memset_s(data, length, 0, length);
}

void BtmFilterAddress(uint8_t *addr, uint8_t length)
{
    FilterData(addr, (length > BT_ADDRESS_SIZE) ? BT_ADDRESS_SIZE : length);
}

static bool IsL2capDynamicPsm(uint16_t psm)
{
    if (psm > L2CAP_DYNAMIC_PSM_NUM_BASE) {
        return true;
    }
    return false;
}

bool BtmFindFilterInfoByInfoUsePsm(void *nodeData, void *info)
{
    BtmSnoopFilterInfo *srcInfo = nodeData;
    BtmSnoopFilterInfo *cmpInfo = info;

    if (!IsL2capDynamicPsm(cmpInfo->l2capPsm)) {
        if (srcInfo->l2capPsm == cmpInfo->l2capPsm) {
            return true;
        }
    } else {
        cmpInfo->rfcommScn = srcInfo->rfcommScn;
        cmpInfo->module = srcInfo->module;
        if (!memcmp(srcInfo, cmpInfo, sizeof(BtmSnoopFilterInfo))) {
            return true;
        }
    }

    return false;
}

bool BtmFindFilterInfoByInfoUseScn(void *nodeData, void *info)
{
    BtmSnoopFilterInfo *srcInfo = nodeData;
    BtmSnoopFilterInfo *cmpInfo = info;

    cmpInfo->l2capPsm = srcInfo->l2capPsm;
    cmpInfo->module = srcInfo->module;
    if (!memcmp(srcInfo, cmpInfo, sizeof(BtmSnoopFilterInfo))) {
        return true;
    } else {
        return false;
    }
}

void BtmChangeIncludeLength(uint16_t *includedLength, uint16_t len)
{
    *includedLength = len + HCI_H4_HEADER_LEN;
}

uint8_t *BtmCreateFilterBuffer(const uint16_t *includedLength, const uint8_t *data)
{
    uint16_t bufferLen = *includedLength - HCI_H4_HEADER_LEN;
    uint8_t *buffer = MEM_MALLOC.alloc(bufferLen);
    if (buffer != NULL) {
        if (data != NULL) {
            (void)memcpy_s(buffer, bufferLen, data, bufferLen);
        }
    }

    return buffer;
}

static BtmSnoopFilterInfo *AllocFilterInfo(
    uint8_t module, uint8_t rfcommScn, uint16_t l2capPsm, bool isLocal, const BtAddr *remoteAddr)
{
    BtmSnoopFilterInfo *info = MEM_MALLOC.alloc(sizeof(BtmSnoopFilterInfo));
    if (info != NULL) {
        info->module = module;
        info->rfcommScn = rfcommScn;
        info->l2capPsm = l2capPsm;
        info->isLocal = isLocal;
        if (remoteAddr != NULL) {
            info->remoteAddr = *remoteAddr;
        } else {
            BtAddr addr = {0};
            info->remoteAddr = addr;
        }
    }

    return info;
}

void BtmHciFilter(uint8_t type, const uint8_t **data, uint16_t originalLength, uint16_t *includedLength)
{
    MutexLock(g_filterMutex);
    if (!g_filter) {
        MutexUnlock(g_filterMutex);
        return;
    }

    switch (type) {
        case TRANSMISSON_TYPE_H2C_CMD:
            BtmFilterHciCmd(data, originalLength, includedLength);
            break;
        case TRANSMISSON_TYPE_C2H_EVENT:
            HciEvtFilter(data, originalLength, includedLength);
            break;
        case TRANSMISSON_TYPE_H2C_DATA:
        case TRANSMISSON_TYPE_C2H_DATA:
            BtmFilterAclData(type, data, originalLength, includedLength);
            break;
        default:
            break;
    }
    MutexUnlock(g_filterMutex);
}

void BTM_AddLocalL2capPsmForLogging(uint8_t module, uint16_t psm)
{
    MutexLock(g_filterMutex);
    if (!g_filter) {
        MutexUnlock(g_filterMutex);
        return;
    }
    LOG_INFO("%{public}s: module:%{public}d, local psm:0x%04x", __FUNCTION__, module, psm);
    BtmSnoopFilterInfo *info = AllocFilterInfo(module, 0, psm, true, NULL);
    if (info != NULL) {
        ListAddLast(g_filterInfoList, info);
    }
    MutexUnlock(g_filterMutex);
}

void BTM_AddRemoteL2capPsmForLogging(uint8_t module, uint16_t psm, const BtAddr *remoteAddr)
{
    MutexLock(g_filterMutex);
    if (!g_filter) {
        MutexUnlock(g_filterMutex);
        return;
    }
    LOG_INFO("%{public}s: module:%{public}d, " BT_ADDR_FMT " local psm:0x%04x",
        __FUNCTION__,
        module,
        BT_ADDR_FMT_OUTPUT(remoteAddr->addr),
        psm);
    BtmSnoopFilterInfo *info = AllocFilterInfo(module, 0, psm, false, remoteAddr);
    if (info != NULL) {
        ListAddLast(g_filterInfoList, info);
    }
    MutexUnlock(g_filterMutex);
}

void BTM_RemoveLocalL2capPsmForLogging(uint8_t module, uint16_t psm)
{
    MutexLock(g_filterMutex);
    if (!g_filter) {
        MutexUnlock(g_filterMutex);
        return;
    }
    BtmSnoopFilterInfo cmpInfo = {
        .isLocal = true,
        .l2capPsm = psm,
        .remoteAddr = {},
    };
    LOG_INFO("%{public}s: module:%{public}d, local psm:0x%04x", __FUNCTION__, module, psm);
    BtmSnoopFilterInfo *info = ListForEachData(g_filterInfoList, BtmFindFilterInfoByInfoUsePsm, &cmpInfo);
    if (info != NULL && info->module == module) {
        ListRemoveNode(g_filterInfoList, info);
    }
    MutexUnlock(g_filterMutex);
}

void BTM_RemoveRemoteL2capPsmForLogging(uint8_t module, uint16_t psm, const BtAddr *remoteAddr)
{
    MutexLock(g_filterMutex);
    if (!g_filter) {
        MutexUnlock(g_filterMutex);
        return;
    }
    BtmSnoopFilterInfo cmpInfo = {
        .isLocal = false,
        .l2capPsm = psm,
        .remoteAddr = *remoteAddr,
    };
    LOG_INFO("%{public}s: module:%{public}d, " BT_ADDR_FMT " local psm:0x%04x",
        __FUNCTION__,
        module,
        BT_ADDR_FMT_OUTPUT(remoteAddr->addr),
        psm);
    BtmSnoopFilterInfo *info = ListForEachData(g_filterInfoList, BtmFindFilterInfoByInfoUsePsm, &cmpInfo);
    if (info != NULL && info->module == module) {
        ListRemoveNode(g_filterInfoList, info);
    }
    MutexUnlock(g_filterMutex);
}

void BTM_AddLocalRfcommScnForLogging(uint8_t module, uint8_t scn)
{
    MutexLock(g_filterMutex);
    if (!g_filter) {
        MutexUnlock(g_filterMutex);
        return;
    }
    LOG_INFO("%{public}s: module:%{public}d, local scn:%02d", __FUNCTION__, module, scn);
    BtmSnoopFilterInfo *info = AllocFilterInfo(module, scn, 0, true, NULL);
    if (info != NULL) {
        ListAddLast(g_filterInfoList, info);
    }
    MutexUnlock(g_filterMutex);
}

void BTM_AddRemoteRfcommScnForLogging(uint8_t module, uint8_t scn, const BtAddr *remoteAddr)
{
    MutexLock(g_filterMutex);
    if (!g_filter) {
        MutexUnlock(g_filterMutex);
        return;
    }
    LOG_INFO("%{public}s: module:%{public}d, " BT_ADDR_FMT " local scn:%02d",
        __FUNCTION__,
        module,
        BT_ADDR_FMT_OUTPUT(remoteAddr->addr),
        scn);
    BtmSnoopFilterInfo *info = AllocFilterInfo(module, scn, 0, false, remoteAddr);
    if (info != NULL) {
        ListAddLast(g_filterInfoList, info);
    }
    MutexUnlock(g_filterMutex);
}

void BTM_RemoveLocalRfcommScnChannelForLogging(uint8_t module, uint8_t scn)
{
    MutexLock(g_filterMutex);
    if (!g_filter) {
        MutexUnlock(g_filterMutex);
        return;
    }
    BtmSnoopFilterInfo cmpInfo = {
        .isLocal = true,
        .rfcommScn = scn,
        .module = module,
        .remoteAddr = {},
    };
    LOG_INFO("%{public}s: module:%{public}d, local scn:%02d", __FUNCTION__, module, scn);
    BtmSnoopFilterInfo *info = ListForEachData(g_filterInfoList, BtmFindFilterInfoByInfoUseScn, &cmpInfo);
    if (info != NULL && info->module == module) {
        ListRemoveNode(g_filterInfoList, info);
    }
    MutexUnlock(g_filterMutex);
}

void BTM_RemoveRemoteRfcommScnChannelForLogging(uint8_t module, uint8_t scn, const BtAddr *remoteAddr)
{
    MutexLock(g_filterMutex);
    if (!g_filter) {
        MutexUnlock(g_filterMutex);
        return;
    }
    BtmSnoopFilterInfo cmpInfo = {
        .isLocal = true,
        .rfcommScn = scn,
        .module = module,
        .remoteAddr = {},
    };
    LOG_INFO("%{public}s: module:%{public}d, " BT_ADDR_FMT " local scn:%02d",
        __FUNCTION__,
        module,
        BT_ADDR_FMT_OUTPUT(remoteAddr->addr),
        scn);
    BtmSnoopFilterInfo *info = ListForEachData(g_filterInfoList, BtmFindFilterInfoByInfoUseScn, &cmpInfo);
    if (info != NULL && info->module == module) {
        ListRemoveNode(g_filterInfoList, info);
    }
    MutexUnlock(g_filterMutex);
}