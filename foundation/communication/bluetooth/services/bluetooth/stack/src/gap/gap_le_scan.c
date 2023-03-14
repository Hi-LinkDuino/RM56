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

#include "gap_le.h"
#include "gap_internal.h"

#include <securec.h>

#include "allocator.h"
#include "log.h"
#include "thread.h"

#include "btm/btm_le_sec.h"
#include "btm/btm_thread.h"
#include "hci/hci.h"
#include "hci/hci_error.h"
#include "smp/smp.h"

typedef enum {
    ADV_REPORT,
    EXTENDED_ADV_REPORT,
    DIRECTED_ADV_REPORT,
} AdvReportType;

typedef struct {
    AdvReportType reportType;
    void *report;
    bool processing;
    BtmIdentityResolvingKey *IRKList;
    uint16_t listCount;
    uint16_t resolveIndex;
    BtAddr addr;
    bool doCallback;
} AdvReportRPAResolveInfo;

typedef struct {
    GapScanCallback callback;
    void *context;
} LeScanCallback;

typedef struct {
    GapExScanCallback callback;
    void *context;
} LeExScanCallback;

static LeScanCallback g_leScanCallback;
static LeExScanCallback g_leExScanCallback;

void GapFreeReportRPAResolveInfo(void *data)
{
    AdvReportRPAResolveInfo *info = data;
    switch (info->reportType) {
        case ADV_REPORT: {
            HciLeAdvertisingReport *report = info->report;
            MEM_MALLOC.free(report->data);
            break;
        }
        case EXTENDED_ADV_REPORT: {
            HciLeExtendedAdvertisingReport *report = info->report;
            MEM_MALLOC.free(report->data);
            break;
        }
        case DIRECTED_ADV_REPORT:
        default:
            break;
    }
    MEM_MALLOC.free(info->report);
    MEM_MALLOC.free(info->IRKList);
    MEM_MALLOC.free(info);
}

int GAP_RegisterScanCallback(const GapScanCallback *callback, void *context)
{
    LOG_INFO("%{public}s:%{public}s", __FUNCTION__, callback ? "register" : "NULL");
    if (callback == NULL) {
        (void)memset_s(
            &g_leScanCallback.callback, sizeof(g_leScanCallback.callback), 0x00, sizeof(g_leScanCallback.callback));
    } else {
        g_leScanCallback.callback = *callback;
    }
    g_leScanCallback.context = context;
    return GAP_SUCCESS;
}

int GAP_DeregisterScanCallback(void)
{
    (void)memset_s(
        &g_leScanCallback.callback, sizeof(g_leScanCallback.callback), 0x00, sizeof(g_leScanCallback.callback));
    g_leScanCallback.context = NULL;
    return GAP_SUCCESS;
}

int GAP_LeScanSetParam(GapLeScanParam param, uint8_t scanFilterPolity)
{
    LOG_INFO("%{public}s:", __FUNCTION__);
    int ret;

    if (GapIsLeEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    if (GapLeRolesCheck(GAP_LE_ROLE_OBSERVER | GAP_LE_ROLE_CENTRAL) == false) {
        ret = GAP_ERR_INVAL_STATE;
    } else {
        HciLeSetScanParametersParam hciCmdParam = {
            .leScanType = param.scanType,
            .leScanInterval = param.param.scanInterval,
            .leScanWindow = param.param.scanWindow,
            .ownAddressType = BTM_GetOwnAddressType(),
            .scanningFilterPolicy = scanFilterPolity,
        };
        ret = HCI_LeSetScanParameters(&hciCmdParam);
    }

    return ret;
}

void GapLeScanSetParamComplete(const HciLeSetExtendedScanParametersReturnParam *param)
{
    if (g_leScanCallback.callback.scanSetParamResult) {
        g_leScanCallback.callback.scanSetParamResult(param->status, g_leScanCallback.context);
    }
}

static void GapLeSetExAdvReportParam(GapExAdvReportParam *advParam, const HciLeExtendedAdvertisingReport *report)
{
    if (advParam != NULL && report != NULL) {
        advParam->advertisingSid = report->advertisingSID;
        advParam->data = report->data;
        advParam->dataLen = report->dataLength;
        advParam->periodicAdvInterval = report->periodicAdvertisingInterval;
        advParam->primaryPhy = report->primaryPHY;
        advParam->rssi = report->rssi;
        advParam->secondaryPhy = report->secondaryPHY;
        advParam->txPower = report->txPower;
    }
}

static void GapCallbackRPAAdvertisingReport(const AdvReportRPAResolveInfo *info, const BtAddr *currentAddr)
{
    if (info == NULL) {
        LOG_WARN("%{public}s: miss info", __FUNCTION__);
        return;
    }

    HciLeAdvertisingReport *report = info->report;
    if (report == NULL || (report->lengthData != 0 && report->data == NULL)) {
        LOG_WARN("%{public}s: miss report or data", __FUNCTION__);
        return;
    }
    LOG_INFO("%{public}s:" BT_ADDR_FMT " -> " BT_ADDR_FMT,
        __FUNCTION__,
        BT_ADDR_FMT_OUTPUT(report->address.raw),
        BT_ADDR_FMT_OUTPUT(info->addr.addr));
    if (g_leScanCallback.callback.advertisingReport) {
        GapAdvReportParam reportParam = {
            .dataLen = report->lengthData,
            .data = report->data,
            .rssi = report->rssi,
        };
        g_leScanCallback.callback.advertisingReport(
            report->eventType, &info->addr, reportParam, currentAddr, g_leScanCallback.context);
    }
}

static void GapCallbackRPAExtendedAdvertisingReport(const AdvReportRPAResolveInfo *info, const BtAddr *currentAddr)
{
    if (info == NULL) {
        LOG_WARN("%{public}s: miss info.", __FUNCTION__);
        return;
    }

    HciLeExtendedAdvertisingReport *report = info->report;
    GapExAdvReportParam advParam;
    BtAddr directAddr;

    if (report == NULL || ((report->dataLength != 0) && (report->data == NULL))) {
        LOG_WARN("%{public}s: miss report or data", __FUNCTION__);
        return;
    }

    GapLeSetExAdvReportParam(&advParam, report);
    advParam.directAddr = &directAddr;
    GapChangeHCIAddr(&directAddr, &report->directAddress, report->directAddressType);

    LOG_INFO("%{public}s:" BT_ADDR_FMT " -> " BT_ADDR_FMT,
        __FUNCTION__,
        BT_ADDR_FMT_OUTPUT(report->address.raw),
        BT_ADDR_FMT_OUTPUT(info->addr.addr));
    if (g_leExScanCallback.callback.exAdvertisingReport) {
        g_leExScanCallback.callback.exAdvertisingReport(
            report->eventType, &info->addr, advParam, currentAddr, g_leExScanCallback.context);
    }
}

static void GapCallbackRPADirectedAdvertisingReport(const AdvReportRPAResolveInfo *info, const BtAddr *currentAddr)
{
    if (info == NULL || info->report == NULL) {
        LOG_WARN("%{public}s: miss info or report", __FUNCTION__);
        return;
    }

    HciLeDirectedAdvertisingReport *report = info->report;
    BtAddr directAddr;
    GapChangeHCIAddr(&directAddr, &report->directAddress, report->directAddressType);
    LOG_INFO("%{public}s:" BT_ADDR_FMT " -> " BT_ADDR_FMT,
        __FUNCTION__,
        BT_ADDR_FMT_OUTPUT(report->address.raw),
        BT_ADDR_FMT_OUTPUT(info->addr.addr));
    if (g_leExScanCallback.callback.directedAdvertisingReport) {
        GapDirectedAdvReportParam reportParam = {
            .directAddr = &directAddr,
            .rssi = report->rssi,
        };
        g_leExScanCallback.callback.directedAdvertisingReport(
            report->eventType, &info->addr, reportParam, currentAddr, g_leExScanCallback.context);
    }
}

void GapDoCallbackRPAAdvertisingReport(void *data, const BtAddr *currentAddr)
{
    AdvReportRPAResolveInfo *info = data;
    switch (info->reportType) {
        case ADV_REPORT: {
            GapCallbackRPAAdvertisingReport(info, currentAddr);
            break;
        }
        case EXTENDED_ADV_REPORT: {
            GapCallbackRPAExtendedAdvertisingReport(info, currentAddr);
            break;
        }
        case DIRECTED_ADV_REPORT: {
            GapCallbackRPADirectedAdvertisingReport(info, currentAddr);
            break;
        }
        default:
            break;
    }
    ListRemoveNode(GapGetLeRandomAddressBlock()->reportRPAResolveList, data);
}

void GapRPAResolveProcess(void)
{
    int ret;

    ListNode *node = ListGetFirstNode(GapGetLeRandomAddressBlock()->reportRPAResolveList);
    while (node != 0) {
        AdvReportRPAResolveInfo *info = ListGetNodeData(node);

        if (info->processing) {
            break;
        }

        if (!info->doCallback) {
            LOG_DEBUG("%{public}s: " BT_ADDR_FMT " start resolve RPA", __FUNCTION__, BT_ADDR_FMT_OUTPUT(info->addr.addr));

            uint8_t addr[BT_ADDRESS_SIZE];
            (void)memcpy_s(addr, BT_ADDRESS_SIZE, info->addr.addr, BT_ADDRESS_SIZE);
            const uint8_t *addrPtr = addr;
            const uint8_t *keyPtr = info->IRKList[info->resolveIndex].irk.key;
            ret = SMP_AsyncResolveRPA(addrPtr, keyPtr);
            if (ret != BT_NO_ERROR) {
                info->doCallback = true;
                GapDoCallbackRPAAdvertisingReport(info, NULL);
            } else {
                info->processing = true;
            }
            break;
        }
        node = ListGetNextNode(node);
    }
}

void GapResolveRPAResult(uint8_t status, bool result, const uint8_t *addr, const uint8_t *irk)
{
    LOG_INFO("%{public}s: status:%02x, result:%02x", __FUNCTION__, status, result);
    ListNode *node = ListGetFirstNode(GapGetLeRandomAddressBlock()->reportRPAResolveList);
    while (node != 0) {
        AdvReportRPAResolveInfo *info = ListGetNodeData(node);
        BtAddr currentAddr = info->addr;
        node = ListGetNextNode(node);
        if (!info->processing) {
            continue;
        }

        info->processing = false;
        if (status == SMP_PAIR_STATUS_SUCCESS && result == true) {
            if (memcmp(info->IRKList[info->resolveIndex].irk.key, irk, GAP_IRK_SIZE)) {
                LOG_ERROR("%{public}s: IRK mismatch", __FUNCTION__);
            } else {
                BTM_UpdateCurrentRemoteAddress(&info->IRKList[info->resolveIndex].addr, &info->addr);
                (void)memcpy_s(&info->addr, sizeof(BtAddr), &info->IRKList[info->resolveIndex].addr, sizeof(BtAddr));
            }
            info->doCallback = true;
        } else {
            info->resolveIndex++;
            if (info->resolveIndex == info->listCount) {
                info->doCallback = true;
            }
        }
        if (info->doCallback) {
            GapDoCallbackRPAAdvertisingReport(info, result ? &currentAddr : NULL);
        }
        break;
    }

    GapRPAResolveProcess();
}

static AdvReportRPAResolveInfo *GapLeAllocAdvReportRPAResolveInfo(
    BtAddr addr, AdvReportType type, const void *advReport)
{
    AdvReportRPAResolveInfo *info = MEM_MALLOC.alloc(sizeof(AdvReportRPAResolveInfo));
    void *report = NULL;
    uint8_t *advData = NULL;

    if (type == ADV_REPORT) {
        advData = MEM_MALLOC.alloc(((const HciLeAdvertisingReport *)advReport)->lengthData);
        if (advData != NULL) {
            (void)memcpy_s(advData,
                ((const HciLeAdvertisingReport *)advReport)->lengthData,
                ((const HciLeAdvertisingReport *)advReport)->data,
                ((const HciLeAdvertisingReport *)advReport)->lengthData);
        }
        report = MEM_MALLOC.alloc(sizeof(HciLeAdvertisingReport));
        if (report != NULL) {
            (void)memcpy_s(report, sizeof(HciLeAdvertisingReport), advReport, sizeof(HciLeAdvertisingReport));
            ((HciLeAdvertisingReport *)report)->data = advData;
        }
    } else if (type == EXTENDED_ADV_REPORT) {
        advData = MEM_MALLOC.alloc(((const HciLeExtendedAdvertisingReport *)advReport)->dataLength);
        if (advData != NULL) {
            (void)memcpy_s(advData,
                ((const HciLeExtendedAdvertisingReport *)advReport)->dataLength,
                ((const HciLeExtendedAdvertisingReport *)advReport)->data,
                ((const HciLeExtendedAdvertisingReport *)advReport)->dataLength);
        }
        report = MEM_MALLOC.alloc(sizeof(HciLeExtendedAdvertisingReport));
        if (report != NULL) {
            (void)memcpy_s(
                report, sizeof(HciLeExtendedAdvertisingReport), advReport, sizeof(HciLeExtendedAdvertisingReport));
            ((HciLeExtendedAdvertisingReport *)report)->data = advData;
        }
    } else if (type == DIRECTED_ADV_REPORT) {
        report = MEM_MALLOC.alloc(sizeof(HciLeDirectedAdvertisingReport));
        if (report != NULL) {
            (void)memcpy_s(
                report, sizeof(HciLeDirectedAdvertisingReport), advReport, sizeof(HciLeDirectedAdvertisingReport));
        }
    }

    if (info != NULL) {
        info->report = report;
        info->reportType = type;
        info->resolveIndex = 0;
        info->processing = false;
        info->doCallback = false;
        (void)memcpy_s(&info->addr, sizeof(BtAddr), &addr, sizeof(BtAddr));
    }

    return info;
}

static bool GapTryChangeAddressForIdentityAddress(BtAddr *addr)
{
    BtAddr pairedAddr = {0};
    int ret = BTM_GetPairdAddressFromRemoteIdentityAddress(addr, &pairedAddr);
    if (ret == BT_NO_ERROR) {
        LOG_INFO("%{public}s:" BT_ADDR_FMT " -> " BT_ADDR_FMT,
            __FUNCTION__,
            BT_ADDR_FMT_OUTPUT(addr->addr),
            BT_ADDR_FMT_OUTPUT(pairedAddr.addr));
        *addr = pairedAddr;
        return true;
    }
    return false;
}

static void GapOnLeAdvertisingReportEventProcessOnce(const HciLeAdvertisingReport *report)
{
    BtAddr addr;
    GapChangeHCIAddr(&addr, &report->address, report->addressType);
    BtAddr currentAddr = addr;
    uint8_t advType = report->eventType;
    uint8_t rssi = report->rssi;
    uint8_t dataLen = report->lengthData;
    uint8_t *data = report->data;

    if (GapAddrIsResolvablePrivateAddress(&addr)) {
        BtmIdentityResolvingKey *deviceIRKList = NULL;
        uint16_t listCount = 0;
        int ret = BTM_GetAllRemoteIdentityResolvingKey(&deviceIRKList, &listCount);
        if (ret == BT_NO_ERROR && listCount != 0) {
            AdvReportRPAResolveInfo *info = GapLeAllocAdvReportRPAResolveInfo(addr, ADV_REPORT, report);
            if (info != NULL) {
                info->IRKList = deviceIRKList;
                info->listCount = listCount;
                ListAddLast(GapGetLeRandomAddressBlock()->reportRPAResolveList, info);
                GapRPAResolveProcess();
                return;
            }
        }
        if (deviceIRKList != NULL) {
            MEM_MALLOC.free(deviceIRKList);
        }
    } else if (GapAddrIsIdentityAddress(&addr)) {
        GapTryChangeAddressForIdentityAddress(&addr);
    } else if (GapAddrIsStaticAddress(&addr) || GapAddrIsPublicAddress(&addr)) {
        if (GapTryChangeAddressForIdentityAddress(&addr)) {
            BTM_UpdateCurrentRemoteAddress(&addr, &currentAddr);
        }
    }

    LOG_INFO("%{public}s:" BT_ADDR_FMT " type=%hhu", __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr.addr), addr.type);
    if (g_leScanCallback.callback.advertisingReport) {
        GapAdvReportParam reportParam = {
            .dataLen = dataLen,
            .data = data,
            .rssi = rssi,
        };
        g_leScanCallback.callback.advertisingReport(advType, &addr, reportParam, NULL, g_leScanCallback.context);
    }
}

void GapOnLeAdvertisingReportEvent(const HciLeAdvertisingReportEventParam *eventParam)
{
    for (uint8_t i = 0; i < eventParam->numReports; i++) {
        GapOnLeAdvertisingReportEventProcessOnce(&eventParam->reports[i]);
    }
}

int GAP_LeScanSetEnable(uint8_t scanEnable, uint8_t filterDuplicates)
{
    LOG_INFO("%{public}s:", __FUNCTION__);
    int ret;

    if (GapIsLeEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    if (GapLeRolesCheck(GAP_LE_ROLE_OBSERVER | GAP_LE_ROLE_CENTRAL) == false) {
        ret = GAP_ERR_INVAL_STATE;
    } else {
        HciLeSetScanEnableParam hciCmdParam = {
            .leScanEnable = scanEnable,
            .filterDuplicates = filterDuplicates,
        };
        ret = HCI_LeSetScanEnable(&hciCmdParam);
    }

    return ret;
}

void GapLeScanSetEnableComplete(const HciLeSetScanEnableReturnParam *param)
{
    if (g_leScanCallback.callback.scanSetEnableResult) {
        g_leScanCallback.callback.scanSetEnableResult(param->status, g_leScanCallback.context);
    }
}

int GAP_RegisterExScanCallback(const GapExScanCallback *callback, void *context)
{
    LOG_INFO("%{public}s:%{public}s", __FUNCTION__, callback ? "register" : "NULL");
    if (callback == NULL) {
        (void)memset_s(&g_leExScanCallback.callback,
            sizeof(g_leExScanCallback.callback),
            0x00,
            sizeof(g_leExScanCallback.callback));
    } else {
        g_leExScanCallback.callback = *callback;
    }
    g_leExScanCallback.context = context;
    return GAP_SUCCESS;
}

int GAP_DeregisterExScanCallback(void)
{
    (void)memset_s(
        &g_leExScanCallback.callback, sizeof(g_leExScanCallback.callback), 0x00, sizeof(g_leExScanCallback.callback));
    g_leExScanCallback.context = NULL;
    return GAP_SUCCESS;
}

NO_SANITIZE("cfi")
static void GapOnLeExtendedAdvertisingReportEventProcessOnce(const HciLeExtendedAdvertisingReport *report)
{
    BtAddr addr;
    GapChangeHCIAddr(&addr, &report->address, report->addressType);
    BtAddr currentAddr = addr;
    uint8_t advType = report->eventType;
    GapExAdvReportParam advParam;
    BtAddr directAddr;
    GapLeSetExAdvReportParam(&advParam, report);
    GapChangeHCIAddr(&directAddr, &report->directAddress, report->directAddressType);
    advParam.directAddr = &directAddr;

    if (GapAddrIsResolvablePrivateAddress(&addr)) {
        BtmIdentityResolvingKey *deviceIRKList = NULL;
        uint16_t listCount = 0;
        int ret = BTM_GetAllRemoteIdentityResolvingKey(&deviceIRKList, &listCount);
        if (ret == BT_NO_ERROR && listCount != 0) {
            AdvReportRPAResolveInfo *info = GapLeAllocAdvReportRPAResolveInfo(addr, EXTENDED_ADV_REPORT, report);
            if (info != NULL) {
                info->IRKList = deviceIRKList;
                info->listCount = listCount;
                ListAddLast(GapGetLeRandomAddressBlock()->reportRPAResolveList, info);
                GapRPAResolveProcess();
                return;
            }
        }
        if (deviceIRKList != NULL) {
            MEM_MALLOC.free(deviceIRKList);
        }
    } else if (GapAddrIsIdentityAddress(&addr)) {
        GapTryChangeAddressForIdentityAddress(&addr);
    } else if (GapAddrIsStaticAddress(&addr) || GapAddrIsPublicAddress(&addr)) {
        if (GapTryChangeAddressForIdentityAddress(&addr)) {
            BTM_UpdateCurrentRemoteAddress(&addr, &currentAddr);
        }
    }

    LOG_INFO("%{public}s:" BT_ADDR_FMT " type=%hhu", __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr.addr), addr.type);
    if (g_leExScanCallback.callback.exAdvertisingReport) {
        g_leExScanCallback.callback.exAdvertisingReport(advType, &addr, advParam, NULL, g_leExScanCallback.context);
    }
}

void GapOnLeExtendedAdvertisingReportEvent(const HciLeExtendedAdvertisingReportEventParam *eventParam)
{
    for (uint8_t i = 0; i < eventParam->numReports; i++) {
        GapOnLeExtendedAdvertisingReportEventProcessOnce(&eventParam->reports[i]);
    }
}

static void GapOnLeDirectedAdvertisingReportProcessOnce(const HciLeDirectedAdvertisingReport *report)
{
    BtAddr addr;
    GapChangeHCIAddr(&addr, &report->address, report->addressType);
    BtAddr currentAddr = addr;
    uint8_t advType = report->eventType;
    BtAddr directAddr;
    GapChangeHCIAddr(&directAddr, &report->directAddress, report->directAddressType);
    int8_t rssi = report->rssi;

    if (GapAddrIsResolvablePrivateAddress(&addr)) {
        BtmIdentityResolvingKey *deviceIRKList = NULL;
        uint16_t listCount = 0;
        int ret = BTM_GetAllRemoteIdentityResolvingKey(&deviceIRKList, &listCount);
        if (ret == BT_NO_ERROR && listCount != 0) {
            AdvReportRPAResolveInfo *info = GapLeAllocAdvReportRPAResolveInfo(addr, DIRECTED_ADV_REPORT, report);
            if (info != NULL) {
                info->IRKList = deviceIRKList;
                info->listCount = listCount;
                ListAddLast(GapGetLeRandomAddressBlock()->reportRPAResolveList, info);
                GapRPAResolveProcess();
                return;
            }
        }
        if (deviceIRKList != NULL) {
            MEM_MALLOC.free(deviceIRKList);
        }
    } else if (GapAddrIsIdentityAddress(&addr)) {
        GapTryChangeAddressForIdentityAddress(&addr);
    } else if (GapAddrIsStaticAddress(&addr) || GapAddrIsPublicAddress(&addr)) {
        if (GapTryChangeAddressForIdentityAddress(&addr)) {
            BTM_UpdateCurrentRemoteAddress(&addr, &currentAddr);
        }
    }

    LOG_INFO("%{public}s:" BT_ADDR_FMT " type=%hhu", __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr.addr), addr.type);
    if (g_leExScanCallback.callback.directedAdvertisingReport) {
        GapDirectedAdvReportParam reportParam = {
            .directAddr = &directAddr,
            .rssi = rssi,
        };
        g_leExScanCallback.callback.directedAdvertisingReport(
            advType, &addr, reportParam, NULL, g_leExScanCallback.context);
    }
}

void GapOnLeDirectedAdvertisingReport(const HciLeDirectedAdvertisingReportEventParam *eventParam)
{
    for (uint8_t i = 0; i < eventParam->numReports; i++) {
        GapOnLeDirectedAdvertisingReportProcessOnce(&eventParam->reports[i]);
    }
}

void GapOnLeScanTimeoutEvent(const void *eventParam)
{
    if (g_leExScanCallback.callback.scanTimeoutEvent) {
        g_leExScanCallback.callback.scanTimeoutEvent(g_leExScanCallback.context);
    }
}

static int GapLeSetExtendedScanParameters(
    uint8_t ownAddrType, uint8_t scanFilterPolity, uint8_t scanPhys, const GapLeScanParam param[])
{
    int ret;
    HciLeSetExtendedScanParametersParam hciCmdParam;
    hciCmdParam.ownAddressType = ownAddrType;
    hciCmdParam.ScanningFilterPolicy = scanFilterPolity;
    hciCmdParam.ScanningPHYs = scanPhys;

    hciCmdParam.sets = MEM_MALLOC.alloc(EXTENDED_SCAN_PHY_MAX_NUM * sizeof(HciLeExtendedScanParametersSet));

    if (hciCmdParam.sets) {
        for (uint8_t i = 0, jj = 0; i < EXTENDED_SCAN_PHY_MAX_NUM; i++) {
            if ((scanPhys >> i) & 0x01) {
                hciCmdParam.sets[jj].scanInterval = param[jj].param.scanInterval;
                hciCmdParam.sets[jj].scanWindow = param[jj].param.scanWindow;
                hciCmdParam.sets[jj].scanType = param[jj].scanType;
                jj++;
            }
        }

        ret = HCI_LeSetExtendedScanParameters(&hciCmdParam);
        MEM_MALLOC.free(hciCmdParam.sets);
    } else {
        ret = GAP_ERR_OUT_OF_RES;
    }

    return ret;
}

NO_SANITIZE("cfi") void GapLeSetExtendedScanParametersComplete(const HciLeSetExtendedScanParametersReturnParam *param)
{
    if (g_leExScanCallback.callback.scanExSetParamResult) {
        g_leExScanCallback.callback.scanExSetParamResult(param->status, g_leExScanCallback.context);
    }
}

int GAP_LeExScanSetParam(uint8_t scanFilterPolity, uint8_t scanPhys, const GapLeScanParam param[])
{
    LOG_INFO("%{public}s:", __FUNCTION__);
    int ret;

    if (GapIsLeEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    if (GapLeRolesCheck(GAP_LE_ROLE_BROADCASTER | GAP_LE_ROLE_CENTRAL) == false) {
        ret = GAP_ERR_INVAL_STATE;
    } else {
        ret = GapLeSetExtendedScanParameters(BTM_GetOwnAddressType(), scanFilterPolity, scanPhys, param);
    }

    return ret;
}

static int GapLeSetExtendedScanEnable(uint8_t scanEnable, uint8_t filterDuplicates, uint16_t duration, uint16_t period)
{
    HciLeSetExtendedScanEnableParam hciCmdParam = {
        .duration = duration,
        .enable = scanEnable,
        .filterDuplicates = filterDuplicates,
        .period = period,
    };

    return HCI_LeSetExtendedScanEnable(&hciCmdParam);
}

NO_SANITIZE("cfi") void GapLeSetExtendedScanEnableComplete(const HciLeSetExtendedScanEnableReturnParam *param)
{
    if (g_leExScanCallback.callback.scanExSetEnableResult) {
        g_leExScanCallback.callback.scanExSetEnableResult(param->status, g_leExScanCallback.context);
    }
}

int GAP_LeExScanSetEnable(uint8_t scanEnable, uint8_t filterDuplicates, uint16_t duration, uint16_t period)
{
    LOG_INFO("%{public}s:", __FUNCTION__);
    int ret;

    if (GapIsLeEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    if (GapLeRolesCheck(GAP_LE_ROLE_BROADCASTER | GAP_LE_ROLE_CENTRAL) == false) {
        ret = GAP_ERR_INVAL_STATE;
    } else {
        ret = GapLeSetExtendedScanEnable(scanEnable, filterDuplicates, duration, period);
    }

    return ret;
}
