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

#include "gap.h"

#include "log.h"
#include "securec.h"

#include "btm.h"
#include "btstack.h"

#include "allocator.h"
#include "module.h"

#include "btm/btm_controller.h"
#include "btm/btm_inq_db.h"
#include "btm/btm_interop.h"
#include "btm/btm_le_sec.h"
#include "btm/btm_thread.h"
#include "hci/hci.h"
#include "hci/hci_error.h"
#include "smp/smp.h"
#include "smp/smp_def.h"

#include "gap_internal.h"
#include "gap_le.h"
#include "gap_task_internal.h"

static GapMng g_gapMng;

typedef struct {
    int traceLevel;
} GapInitializeParam;

static int GapWriteScanEnable(void);
static int GapWriteInquiryScanActivity(void);
static int GapWriteInquiryScanType(void);
static int GapWriteCurrentIACLAP(void);
static int GapWritePageScanActivity(void);
static int GapWritePageScanType(void);
static int GapWriteClassOfDevice(uint32_t cod);

static void GapFreeRegSecInfo(void *data);
static void GapFreeReqSecInfo(void *data);
static void GapFreeDeviceInfo(void *data);

static void GapFreeListNode(void *data)
{
    MEM_MALLOC.free(data);
}

static void GapInitializeTask(void *ctx)
{
    LOG_DEBUG("%{public}s:", __FUNCTION__);

    (void)memset_s(&g_gapMng, sizeof(GapMng), 0x00, sizeof(GapMng));
    g_gapMng.traceLevel = ((GapInitializeParam *)ctx)->traceLevel;
#ifdef GAP_BREDR_SUPPORT
    g_gapMng.bredr.profileSecBlock.registerlist = ListCreate(GapFreeRegSecInfo);
    g_gapMng.bredr.profileSecBlock.requestlist = ListCreate(GapFreeReqSecInfo);
    g_gapMng.bredr.connectionInfoBlock.devicelist = ListCreate(GapFreeDeviceInfo);
#endif
#ifdef GAP_LE_SUPPORT
    g_gapMng.le.connectionInfoBlock.deviceList = ListCreate(GapFreeLeDeviceInfo);
    g_gapMng.le.signatureBlock.RequestList = ListCreate(GapFreeLeSignatureRequest);
    g_gapMng.le.randomAddressBlock.reportRPAResolveList = ListCreate(GapFreeReportRPAResolveInfo);
    g_gapMng.le.exAdvBlock.exAdvInfoList = ListCreate(GapFreeListNode);
#endif
}

static void GapInitialize(int traceLevel)
{
    LOG_INFO("%{public}s:", __FUNCTION__);

    BTM_CreateProcessingQueue(PROCESSING_QUEUE_ID_GAP, BTM_PROCESSING_QUEUE_SIZE_DEFAULT);

    GapInitializeParam *ctx = MEM_MALLOC.alloc(sizeof(GapInitializeParam));
    if (ctx == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return;
    }

    int ret = GapRunTaskBlockProcess(GapInitializeTask, ctx);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s: Run task error.", __FUNCTION__);
    }

    MEM_MALLOC.free(ctx);
}

static void GapEnableBredr(void)
{
    if (BTM_IsControllerSupportBrEdr()) {
        g_gapMng.bredr.isEnable = true;

        g_gapMng.bredr.keyMissingRetry = true;

        HciWriteInquiryModeParam writeInquiryModeParam = {
            .inquiryMode = HCI_INQUIRY_MODE_STANDARD,
        };

        if (BTM_IsControllerSupportEirInquiryResponse()) {
            writeInquiryModeParam.inquiryMode = HCI_INQUIRY_MODE_EIR;
        } else if (BTM_IsControllerSupportRssiInquiryResponse()) {
            writeInquiryModeParam.inquiryMode = HCI_INQUIRY_MODE_RSSI;
        }

        HCI_WriteInquiryMode(&writeInquiryModeParam);
    }
}

static void GapEnableLe(void)
{
    if (BTM_IsControllerSupportLe()) {
        g_gapMng.le.isEnable = true;
        g_gapMng.le.local.minEncKeySize = GAP_ENC_KEY_MAX_SIZE;
        int ret = BTM_GetLocalAddr(&g_gapMng.le.local.addr);
        if (ret == BT_NO_ERROR) {
            g_gapMng.le.local.addr.type = LOCAL_ADDR_TYPE_PUBLIC;
        } else {
            g_gapMng.le.local.addr.type = LOCAL_ADDR_TYPE_RANDOM;
        }

        if (BTM_IsControllerSupportLeExtendedAdvertising()) {
            HCI_LeReadMaximumAdvertisingDataLength();
            HCI_LeReadNumberofSupportedAdvertisingSets();
        }

        GapRegisterL2capCallbacks();
        GapRegisterSmCallbacks();
    }
}

static void GapEnableTask(void *ctx)
{
    LOG_DEBUG("%{public}s:", __FUNCTION__);

#ifdef GAP_BREDR_SUPPORT
    GapEnableBredr();
#endif
#ifdef GAP_LE_SUPPORT
    GapEnableLe();
#endif
    GapRegisterHciEventCallbacks();
    GapRegisterBtmAclCallbacks();
}

static void GapEnable(void)
{
    LOG_INFO("%{public}s:", __FUNCTION__);

    int ret = GapRunTaskBlockProcess(GapEnableTask, NULL);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s: Run task error.", __FUNCTION__);
    }
}

static void GapDisableTask(void *ctx)
{
    LOG_DEBUG("%{public}s:", __FUNCTION__);
    GapDeregisterHciEventCallbacks();
    GapDeregisterBtmAclCallbacks();
    if (BTM_IsControllerSupportBrEdr()) {
        ListClear(g_gapMng.bredr.profileSecBlock.registerlist);
        ListNode *node = ListGetFirstNode(g_gapMng.bredr.profileSecBlock.requestlist);
        while (node != NULL) {
            RequestSecInfo *reqInfo = ListGetNodeData(node);
            node = ListGetNextNode(node);
            if (!reqInfo->doCallback) {
                ListRemoveNode(g_gapMng.bredr.profileSecBlock.requestlist, reqInfo);
            }
        }
        ListClear(g_gapMng.bredr.connectionInfoBlock.devicelist);
        g_gapMng.bredr.scanModeBlock.status = GAP_SCANMODE_STATUS_IDLE;
        g_gapMng.bredr.inquiryBlock.status = GAP_INQUIRY_STATUS_IDLE;
        g_gapMng.bredr.remoteNameBlock.status = GAP_REMOTE_NAME_STATUS_IDLE;
        g_gapMng.bredr.encryptionBlock.status = GAP_SET_ENCRYPTION_STATUS_IDLE;
        g_gapMng.bredr.isEnable = false;
    }

    if (BTM_IsControllerSupportLe()) {
        GapDeregisterL2capCallbacks();
        GapDeregisterSmCallbacks();
        ListClear(g_gapMng.le.connectionInfoBlock.deviceList);
        ListClear(g_gapMng.le.signatureBlock.RequestList);
        ListClear(g_gapMng.le.randomAddressBlock.reportRPAResolveList);
        ListClear(g_gapMng.le.exAdvBlock.exAdvInfoList);
        g_gapMng.le.bondBlock.isPairing = false;
        g_gapMng.le.randomAddressBlock.generationInfo.processing = false;
        g_gapMng.le.isEnable = false;
    }
}

static void GapDisable(void *ctx)
{
    LOG_INFO("%{public}s:", __FUNCTION__);

    int ret = GapRunTaskBlockProcess(GapDisableTask, NULL);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s: Run task error.", __FUNCTION__);
    }
}

static void GapFinalizeTask(void *ctx)
{
    LOG_INFO("%{public}s:", __FUNCTION__);

#ifdef GAP_BREDR_SUPPORT
    ListDelete(g_gapMng.bredr.profileSecBlock.registerlist);
    ListDelete(g_gapMng.bredr.profileSecBlock.requestlist);
    ListDelete(g_gapMng.bredr.connectionInfoBlock.devicelist);
#endif
#ifdef GAP_LE_SUPPORT
    ListDelete(g_gapMng.le.connectionInfoBlock.deviceList);
    ListDelete(g_gapMng.le.signatureBlock.RequestList);
    ListDelete(g_gapMng.le.randomAddressBlock.reportRPAResolveList);
    ListDelete(g_gapMng.le.exAdvBlock.exAdvInfoList);
#endif

    (void)memset_s(&g_gapMng, sizeof(GapMng), 0x00, sizeof(GapMng));

    BTM_DeleteProcessingQueue(PROCESSING_QUEUE_ID_GAP);
}

static void GapFinalize(void)
{
    LOG_INFO("%{public}s:", __FUNCTION__);

    int ret = GapRunTaskBlockProcess(GapFinalizeTask, NULL);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s: Run task error.", __FUNCTION__);
    }
}

void GapChangeHCIAddr(BtAddr *addr, const HciBdAddr *hciAddr, uint8_t addrType)
{
    (void)memcpy_s(addr->addr, BT_ADDRESS_SIZE, hciAddr->raw, BT_ADDRESS_SIZE);
    addr->type = addrType;
}

bool GapIsEmptyAddr(const uint8_t *addr)
{
    for (int i = 0; i < BT_ADDRESS_SIZE; i++) {
        if (addr[i] != 0) {
            return false;
        }
    }
    return true;
}

bool GapAddrCompare(const BtAddr *addr1, const BtAddr *addr2)
{
    return (!memcmp(addr1->addr, addr2->addr, BT_ADDRESS_SIZE) &&
            !((addr1->type == BT_PUBLIC_DEVICE_ADDRESS || addr1->type == BT_PUBLIC_IDENTITY_ADDRESS) ^
                (addr2->type == BT_PUBLIC_DEVICE_ADDRESS || addr2->type == BT_PUBLIC_IDENTITY_ADDRESS)));
}

bool GapCompareChannelID(GAP_SecMultiplexingProtocol protocolID, GapSecChannel channelID1, GapSecChannel channelID2)
{
    return (protocolID == SEC_PROTOCOL_L2CAP) ? (channelID1.l2capPsm == channelID2.l2capPsm)
                                              : (channelID1.rfcommChannel == channelID2.rfcommChannel);
}

#ifdef GAP_BREDR_SUPPORT

bool GapIsBredrEnable(void)
{
    return g_gapMng.bredr.isEnable;
}

ScanModeBlock *GapGetScanModeBlock(void)
{
    return &g_gapMng.bredr.scanModeBlock;
}

InquiryBlock *GapGetInquiryBlock(void)
{
    return &g_gapMng.bredr.inquiryBlock;
}

RemoteNameBlock *GapGetRemoteNameBlock(void)
{
    return &g_gapMng.bredr.remoteNameBlock;
}

GAP_SecurityMode *GapGetSecurityMode(void)
{
    return &g_gapMng.bredr.secMode;
}

ProfileSecurityBlock *GapGetProfileSecurityBlock(void)
{
    return &g_gapMng.bredr.profileSecBlock;
}

ConnectionInfoBlock *GapGetConnectionInfoBlock(void)
{
    return &g_gapMng.bredr.connectionInfoBlock;
}

EncryptionBlock *GapGetEncryptionBlock(void)
{
    return &g_gapMng.bredr.encryptionBlock;
}

bool GapIsBondMode(void)
{
    bool isBondMode;
    isBondMode = (g_gapMng.bredr.bondableMode == GAP_BONDABLE_MODE);
    return isBondMode;
}
#endif

#ifdef GAP_LE_SUPPORT

bool GapIsLeEnable(void)
{
    return g_gapMng.le.isEnable;
}

bool GapLeRolesCheck(uint8_t role)
{
    return !!(g_gapMng.le.local.role & role);
}

void GapFreeLeDeviceInfo(void *data)
{
    LeDeviceInfo *deviceInfo = data;

    AlarmCancel(deviceInfo->alarm);
    AlarmDelete(deviceInfo->alarm);
    if (deviceInfo->securityReq != NULL) {
        MEM_MALLOC.free(deviceInfo->securityReq);
        deviceInfo->securityReq = NULL;
    }
    if (deviceInfo->paramUpdateReq != NULL) {
        MEM_MALLOC.free(deviceInfo->paramUpdateReq);
        deviceInfo->paramUpdateReq = NULL;
    }
    MEM_MALLOC.free(deviceInfo);
}

void GapFreeLeSignatureRequest(void *data)
{
    MEM_MALLOC.free(data);
}

LeBondBlock *GapGetLeBondBlock(void)
{
    return &g_gapMng.le.bondBlock;
}

LeConnectionInfoBlock *GapGetLeConnectionInfoBlock(void)
{
    return &g_gapMng.le.connectionInfoBlock;
}

LeSignatureBlock *GapGetLeSignatureBlock(void)
{
    return &g_gapMng.le.signatureBlock;
}

LeRandomAddressBlock *GapGetLeRandomAddressBlock(void)
{
    return &g_gapMng.le.randomAddressBlock;
}

LeExAdvBlock *GapGetLeExAdvBlock(void)
{
    return &g_gapMng.le.exAdvBlock;
}

bool GapIsLeBondableMode(void)
{
    return g_gapMng.le.local.bondableMode == GAP_BONDABLE_MODE;
}

LeLocalInfo *GapGetLeLocalInfo(void)
{
    return &g_gapMng.le.local;
}

#endif

#ifdef GAP_BREDR_SUPPORT

int GAP_SetScanMode(const GapDiscoverModeInfo *discoverInfo, const GapConnectableModeInfo *connectableInfo,
    GapSetScanModeResultCallback callback, void *context)
{
    int ret;
    ScanModeBlock *scanModeBlock = NULL;

    LOG_INFO("%{public}s:discoverable:[%hhu][%hhu][%04x][%04x], connectable:[%hhu][%hhu][%04x][%04x]",
        __FUNCTION__,
        discoverInfo->mode,
        discoverInfo->type,
        discoverInfo->scanInterval,
        discoverInfo->scanWindow,
        connectableInfo->mode,
        connectableInfo->type,
        connectableInfo->scanInterval,
        connectableInfo->scanWindow);

    if (GapIsBredrEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    if (discoverInfo->mode > GAP_DISCOVERABLE_MODE_GENERAL || discoverInfo->type > GAP_INQUIRY_SCAN_TYPE_INTERLACED ||
        discoverInfo->scanInterval < GAP_SCAN_INTERVAL_RANGE_MIN ||
        discoverInfo->scanInterval > GAP_SCAN_INTERVAL_RANGE_MAX ||
        discoverInfo->scanWindow < GAP_SCAN_WINDOW_RANGE_MIN || discoverInfo->scanWindow > discoverInfo->scanInterval ||
        connectableInfo->mode > GAP_CONNECTABLE_MODE || connectableInfo->type > GAP_PAGE_SCAN_TYPE_INTERLACED ||
        connectableInfo->scanInterval < GAP_SCAN_INTERVAL_RANGE_MIN ||
        connectableInfo->scanInterval > GAP_SCAN_INTERVAL_RANGE_MAX ||
        connectableInfo->scanWindow < GAP_SCAN_WINDOW_RANGE_MIN ||
        connectableInfo->scanWindow > connectableInfo->scanInterval) {
        return GAP_ERR_INVAL_PARAM;
    }

    scanModeBlock = GapGetScanModeBlock();
    if (scanModeBlock->status != GAP_SCANMODE_STATUS_IDLE) {
        ret = GAP_ERR_INVAL_STATE;
    } else {
        (void)memcpy_s(
            &scanModeBlock->discoverMode, sizeof(GapDiscoverModeInfo), discoverInfo, sizeof(GapDiscoverModeInfo));
        (void)memcpy_s(&scanModeBlock->connectableMode,
            sizeof(GapConnectableModeInfo),
            connectableInfo,
            sizeof(GapConnectableModeInfo));
        scanModeBlock->callback = callback;
        scanModeBlock->context = context;
        scanModeBlock->status = GAP_SCANMODE_STATUS_CLOSING;

        ret = GapWriteScanEnable();
        if (ret != BT_NO_ERROR) {
            LOG_WARN("");
            scanModeBlock->status = GAP_SCANMODE_STATUS_IDLE;
        }
    }

    return ret;
}

static int GapWriteScanEnable(void)
{
    LOG_DEBUG("%{public}s:", __FUNCTION__);
    HciWriteScanEnableParam hciCmdParam;
    ScanModeBlock *scanModeBlock = GapGetScanModeBlock();
    uint8_t discoverMode = scanModeBlock->discoverMode.mode;
    uint8_t connectableMode = scanModeBlock->connectableMode.mode;
    int ret;

    if (scanModeBlock->status == GAP_SCANMODE_STATUS_CLOSING) {
        hciCmdParam.scanEnable = 0;
        ret = HCI_WriteScanEnable(&hciCmdParam);
    } else if (scanModeBlock->status == GAP_SCANMODE_STATUS_SETTING) {
        if (discoverMode != GAP_DISCOVERABLE_MODE_NON && connectableMode != GAP_CONNECTABLE_MODE_NON) {
            hciCmdParam.scanEnable = INQUIRY_SCAN_ENABLED_PAGE_SCAN_ENABLED;
        } else if (connectableMode != GAP_DISCOVERABLE_MODE_NON) {
            hciCmdParam.scanEnable = INQUIRY_SCAN_DISABLED_PAGE_SCAN_ENABLED;
        } else if (discoverMode != GAP_CONNECTABLE_MODE_NON) {
            hciCmdParam.scanEnable = INQUIRY_SCAN_ENABLED_PAGE_SCAN_DISABLED;
        } else {
            hciCmdParam.scanEnable = NO_SCANS_ENABLED;
        }
        ret = HCI_WriteScanEnable(&hciCmdParam);
    } else {
        ret = GAP_ERR_INVAL_STATE;
    }
    return ret;
}

NO_SANITIZE("cfi") void GapWriteScanEnableComplete(const HciWriteScanEnableReturnParam *param)
{
    LOG_DEBUG("%{public}s:", __FUNCTION__);
    int ret = GAP_SUCCESS;
    GapSetScanModeResultCallback callback = NULL;
    ScanModeBlock *scanModeBlock = NULL;
    uint8_t retStatus = param->status;

    if (GapIsBredrEnable() == false) {
        LOG_ERROR("Not Enable.");
        return;
    }

    scanModeBlock = GapGetScanModeBlock();
    if (scanModeBlock->status == GAP_SCANMODE_STATUS_CLOSING) {
        if (retStatus == HCI_SUCCESS) {
            if (scanModeBlock->discoverMode.mode) {
                scanModeBlock->status = GAP_SCANMODE_STATUS_SETTING;
                ret = GapWriteInquiryScanActivity();
            } else if (scanModeBlock->connectableMode.mode) {
                scanModeBlock->status = GAP_SCANMODE_STATUS_SETTING;
                ret = GapWritePageScanActivity();
            } else {
                scanModeBlock->status = GAP_SCANMODE_STATUS_IDLE;
                callback = scanModeBlock->callback;
            }
            if (ret != BT_NO_ERROR) {
                LOG_ERROR("HCI Command Error ret = %{public}d.", ret);
                retStatus = GAP_STATUS_FAILED;
                callback = scanModeBlock->callback;
                scanModeBlock->status = GAP_SCANMODE_STATUS_IDLE;
            }
        } else {
            scanModeBlock->status = GAP_SCANMODE_STATUS_IDLE;
            callback = scanModeBlock->callback;
        }
    } else if (scanModeBlock->status == GAP_SCANMODE_STATUS_SETTING) {
        scanModeBlock->status = GAP_SCANMODE_STATUS_IDLE;
        callback = scanModeBlock->callback;
    } else {
        LOG_WARN("");
    }

    if (callback != NULL) {
        callback(retStatus, scanModeBlock->context);
    }
}

static int GapWriteInquiryScanActivity(void)
{
    LOG_DEBUG("%{public}s:", __FUNCTION__);
    int ret;
    ScanModeBlock *scanModeBlock = GapGetScanModeBlock();
    HciWriteInquiryScanActivityParam hciCmdParam;

    if (scanModeBlock->status == GAP_SCANMODE_STATUS_SETTING) {
        hciCmdParam.inquiryScanInterval = scanModeBlock->discoverMode.scanInterval;
        hciCmdParam.inquiryScanWindow = scanModeBlock->discoverMode.scanWindow;
        ret = HCI_WriteInquiryScanActivity(&hciCmdParam);
    } else {
        ret = GAP_ERR_INVAL_STATE;
    }
    return ret;
}

void GapWriteInquiryScanActivityComplete(const HciWriteInquiryScanActivityReturnParam *param)
{
    LOG_DEBUG("%{public}s:", __FUNCTION__);
    GapSetScanModeResultCallback callback = NULL;
    uint8_t retStatus = param->status;

    if (GapIsBredrEnable() == false) {
        LOG_ERROR("%sNot Enable.", "");
        return;
    }

    ScanModeBlock *scanModeBlock = GapGetScanModeBlock();
    if (scanModeBlock->status == GAP_SCANMODE_STATUS_SETTING) {
        if (retStatus == HCI_SUCCESS) {
            int ret = GapWriteInquiryScanType();
            if (ret != BT_NO_ERROR) {
                scanModeBlock->status = GAP_SCANMODE_STATUS_IDLE;
                retStatus = GAP_STATUS_FAILED;
                callback = scanModeBlock->callback;
                LOG_ERROR("HCI Command Error ret = %{public}d.", ret);
            }
        } else {
            scanModeBlock->status = GAP_SCANMODE_STATUS_IDLE;
            callback = scanModeBlock->callback;
        }
    }

    if (callback != NULL) {
        callback(retStatus, scanModeBlock->context);
    }
}

static int GapWriteInquiryScanType(void)
{
    LOG_DEBUG("%{public}s:", __FUNCTION__);
    int ret;
    ScanModeBlock *scanModeBlock = GapGetScanModeBlock();
    HciWriteInquiryScanTypeParam hciCmdParam;

    if (scanModeBlock->status == GAP_SCANMODE_STATUS_SETTING) {
        hciCmdParam.scanType = scanModeBlock->discoverMode.type;
        ret = HCI_WriteInquiryScanType(&hciCmdParam);
    } else {
        ret = GAP_ERR_INVAL_STATE;
    }
    return ret;
}

void GapWriteInquiryScanTypeComplete(const HciWriteInquiryScanTypeReturnParam *param)
{
    LOG_DEBUG("%{public}s:", __FUNCTION__);
    ScanModeBlock *scanModeBlock = NULL;
    GapSetScanModeResultCallback callback = NULL;
    uint8_t retStatus = param->status;

    if (GapIsBredrEnable() == false) {
        LOG_ERROR("%sNot Enable.", "");
        return;
    }

    scanModeBlock = GapGetScanModeBlock();
    if (scanModeBlock->status == GAP_SCANMODE_STATUS_SETTING) {
        if (retStatus == HCI_SUCCESS) {
            int ret = GapWriteCurrentIACLAP();
            if (ret != BT_NO_ERROR) {
                LOG_ERROR("HCI Command Error ret = %{public}d.", ret);
                callback = scanModeBlock->callback;
                retStatus = GAP_STATUS_FAILED;
                scanModeBlock->status = GAP_SCANMODE_STATUS_IDLE;
            }
        } else {
            scanModeBlock->status = GAP_SCANMODE_STATUS_IDLE;
            callback = scanModeBlock->callback;
        }
    }

    if (callback != NULL) {
        callback(retStatus, scanModeBlock->context);
    }
}

static int GapWriteCurrentIACLAP(void)
{
    LOG_DEBUG("%{public}s:", __FUNCTION__);
    ScanModeBlock *scanModeBlock = GapGetScanModeBlock();
    uint8_t discoverableMode = scanModeBlock->discoverMode.mode;
    uint8_t numCurrentIAC = 0;
    uint32_t iacLAP[CURRENT_IAC_MAX_NUM];

    if (discoverableMode & GAP_DISCOVERABLE_MODE_GENERAL) {
        iacLAP[numCurrentIAC] = LAP_GENERAL_INQUIRY_ACCESS;
        numCurrentIAC++;
    } else if (discoverableMode & GAP_DISCOVERABLE_MODE_LIMITED) {
        iacLAP[numCurrentIAC] = LAP_GENERAL_INQUIRY_ACCESS;
        numCurrentIAC++;
        iacLAP[numCurrentIAC] = LAP_LIMITED_INQUIRY_ACCESS;
        numCurrentIAC++;
    }

    HciWriteCurrentIacLapParam param = {
        .numCurrentIAC = numCurrentIAC,
        .iacLAP = iacLAP,
    };

    return HCI_WriteCurrentIacLap(&param);
}

void GapWriteCurrentIACLAPComplete(const HciWriteCurrentIacLapReturnParam *param)
{
    LOG_DEBUG("%{public}s:", __FUNCTION__);
    ScanModeBlock *scanModeBlock = NULL;
    GapSetScanModeResultCallback callback = NULL;
    uint8_t retStatus = param->status;

    if (GapIsBredrEnable() == false) {
        LOG_ERROR("%sNot Enable.", "");
        return;
    }

    scanModeBlock = GapGetScanModeBlock();
    if (scanModeBlock->status == GAP_SCANMODE_STATUS_SETTING) {
        if (retStatus == HCI_SUCCESS) {
            if (scanModeBlock->discoverMode.mode & GAP_INQUIRY_MODE_LIMITED) {
                g_gapMng.bredr.classOfDevice |= COD_LIMITED_DISCOVERABLE_BIT;
            } else {
                g_gapMng.bredr.classOfDevice &= ~COD_LIMITED_DISCOVERABLE_BIT;
            }

            int ret = GapWriteClassOfDevice(g_gapMng.bredr.classOfDevice);
            if (ret != BT_NO_ERROR) {
                LOG_ERROR("HCI Command Error ret = %{public}d.", ret);
                retStatus = GAP_STATUS_FAILED;
                scanModeBlock->status = GAP_SCANMODE_STATUS_IDLE;
                callback = scanModeBlock->callback;
            }
        } else {
            callback = scanModeBlock->callback;
            scanModeBlock->status = GAP_SCANMODE_STATUS_IDLE;
        }
    }

    if (callback != NULL) {
        callback(retStatus, scanModeBlock->context);
    }
}

static int GapWritePageScanActivity(void)
{
    LOG_DEBUG("%{public}s:", __FUNCTION__);
    int ret;
    ScanModeBlock *scanModeBlock = GapGetScanModeBlock();
    HciWritePageScanActivityParam hciCmdParam;

    hciCmdParam.pageScanInterval = scanModeBlock->connectableMode.scanInterval;
    hciCmdParam.pageScanWindow = scanModeBlock->connectableMode.scanWindow;
    ret = HCI_WritePageScanActivity(&hciCmdParam);
    return ret;
}

void GapWritePageScanActivityComplete(const HciWritePageScanActivityReturnParam *param)
{
    LOG_DEBUG("%{public}s:", __FUNCTION__);
    GapSetScanModeResultCallback callback = NULL;
    uint8_t retStatus = param->status;

    if (GapIsBredrEnable() == false) {
        LOG_ERROR("%sNot Enable.", "");
        return;
    }

    ScanModeBlock *scanModeBlock = GapGetScanModeBlock();
    if (scanModeBlock->status == GAP_SCANMODE_STATUS_SETTING) {
        if (retStatus == HCI_SUCCESS) {
            int ret = GapWritePageScanType();
            if (ret != BT_NO_ERROR) {
                LOG_ERROR("HCI Command Error ret = %{public}d.", ret);
                scanModeBlock->status = GAP_SCANMODE_STATUS_IDLE;
                callback = scanModeBlock->callback;
                retStatus = GAP_STATUS_FAILED;
            }
        } else {
            callback = scanModeBlock->callback;
            scanModeBlock->status = GAP_SCANMODE_STATUS_IDLE;
        }
    }

    if (callback != NULL) {
        callback(retStatus, scanModeBlock->context);
    }
}

static int GapWritePageScanType(void)
{
    LOG_DEBUG("%{public}s:", __FUNCTION__);
    int ret;
    ScanModeBlock *scanModeBlock = GapGetScanModeBlock();
    HciWritePageScanTypeParam hciCmdParam;

    hciCmdParam.pageScanType = scanModeBlock->connectableMode.type;
    ret = HCI_WritePageScanType(&hciCmdParam);
    return ret;
}

void GapWritePageScanTypeComplete(const HciWritePageScanTypeReturnParam *param)
{
    LOG_DEBUG("%{public}s:", __FUNCTION__);
    GapSetScanModeResultCallback callback = NULL;
    uint8_t retStatus = param->status;

    if (GapIsBredrEnable() == false) {
        LOG_ERROR("%sNot Enable.", "");
        return;
    }

    ScanModeBlock *scanModeBlock = GapGetScanModeBlock();
    if (scanModeBlock->status == GAP_SCANMODE_STATUS_SETTING) {
        if (retStatus == HCI_SUCCESS) {
            int ret = GapWriteScanEnable();
            if (ret != BT_NO_ERROR) {
                LOG_ERROR("HCI Command Error ret = %{public}d.", ret);
                callback = scanModeBlock->callback;
                scanModeBlock->status = GAP_SCANMODE_STATUS_IDLE;
                retStatus = GAP_STATUS_FAILED;
            }
        } else {
            callback = scanModeBlock->callback;
            scanModeBlock->status = GAP_SCANMODE_STATUS_IDLE;
        }
    }

    if (callback != NULL) {
        callback(retStatus, scanModeBlock->context);
    }
}

void GapReadLocalOobExtendedDataComplete(const HciReadLocalOobExtendedDataReturnParam *param)
{
    if (param->status == HCI_SUCCESS) {
        (void)memcpy_s(g_gapMng.bredr.oobData256.C, GAP_OOB_DATA_CONFIRM_SIZE, param->c256, GAP_OOB_DATA_CONFIRM_SIZE);
        (void)memcpy_s(g_gapMng.bredr.oobData256.R, GAP_OOB_DATA_RANDOM_SIZE, param->r256, GAP_OOB_DATA_RANDOM_SIZE);
        (void)memcpy_s(g_gapMng.bredr.oobData192.C, GAP_OOB_DATA_CONFIRM_SIZE, param->c192, GAP_OOB_DATA_CONFIRM_SIZE);
        (void)memcpy_s(g_gapMng.bredr.oobData192.R, GAP_OOB_DATA_RANDOM_SIZE, param->r192, GAP_OOB_DATA_RANDOM_SIZE);
    } else {
        int ret = HCI_ReadLocalOOBData();
        if (ret != BT_NO_ERROR) {
            LOG_WARN("%{public}s: Local OOB data read failed", __FUNCTION__);
        }
    }
}

void GapReadLocalOobDataComplete(const HciReadLocalOOBDataReturnParam *param)
{
    if (param->status == HCI_SUCCESS) {
        (void)memset_s(g_gapMng.bredr.oobData256.C, GAP_OOB_DATA_CONFIRM_SIZE, 0x00, GAP_OOB_DATA_CONFIRM_SIZE);
        (void)memset_s(g_gapMng.bredr.oobData256.R, GAP_OOB_DATA_RANDOM_SIZE, 0x00, GAP_OOB_DATA_RANDOM_SIZE);
        (void)memcpy_s(g_gapMng.bredr.oobData192.C, GAP_OOB_DATA_CONFIRM_SIZE, param->C, GAP_OOB_DATA_CONFIRM_SIZE);
        (void)memcpy_s(g_gapMng.bredr.oobData192.R, GAP_OOB_DATA_RANDOM_SIZE, param->R, GAP_OOB_DATA_RANDOM_SIZE);
    } else {
        (void)memset_s(g_gapMng.bredr.oobData256.C, GAP_OOB_DATA_CONFIRM_SIZE, 0x00, GAP_OOB_DATA_CONFIRM_SIZE);
        (void)memset_s(g_gapMng.bredr.oobData256.R, GAP_OOB_DATA_RANDOM_SIZE, 0x00, GAP_OOB_DATA_RANDOM_SIZE);
        (void)memset_s(g_gapMng.bredr.oobData192.C, GAP_OOB_DATA_CONFIRM_SIZE, 0x00, GAP_OOB_DATA_CONFIRM_SIZE);
        (void)memset_s(g_gapMng.bredr.oobData192.R, GAP_OOB_DATA_RANDOM_SIZE, 0x00, GAP_OOB_DATA_RANDOM_SIZE);
    }
}

int GapReadNewLocalOOBData(void)
{
    int ret;

    BtmLocalVersionInformation version;
    BTM_GetLocalVersionInformation(&version);
    if (version.hciVersion >= BLUETOOTH_CORE_SPECIFICATION_4_1) {
        ret = HCI_ReadLocalOOBExtendedData();
        if (ret != BT_NO_ERROR) {
            LOG_WARN("%{public}s: Local OOB data read failed", __FUNCTION__);
        }
    } else {
        ret = HCI_ReadLocalOOBData();
        if (ret != BT_NO_ERROR) {
            LOG_WARN("%{public}s: Local OOB data read failed", __FUNCTION__);
        }
    }

    return ret;
}

bool GapIsKeyMissingRetry(void)
{
    return g_gapMng.bredr.keyMissingRetry;
}

int GAP_SetKeyMissingRetry(bool retry)
{
    g_gapMng.bredr.keyMissingRetry = retry;
    return GAP_SUCCESS;
}

int GAP_SetBondableMode(uint8_t bondableMode)
{
    LOG_INFO("%{public}s:bondable[%hhu]", __FUNCTION__, bondableMode);

    if (GapIsBredrEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    g_gapMng.bredr.bondableMode = bondableMode;
    if (bondableMode == GAP_BONDABLE_MODE) {
        GapReadNewLocalOOBData();
    }

    return GAP_SUCCESS;
}

static void GapFreeRegSecInfo(void *data)
{
    MEM_MALLOC.free(data);
}

static void GapFreeReqSecInfo(void *data)
{
    GapAuthenticationClearInfo(data);
    MEM_MALLOC.free(data);
}

static void GapFreeDeviceInfo(void *data)
{
    DeviceInfo *devInfo = data;
    if (devInfo != NULL) {
        AlarmCancel(devInfo->alarm);
        AlarmDelete(devInfo->alarm);
        AlarmCancel(devInfo->aclAlarm);
        AlarmDelete(devInfo->aclAlarm);
        if (devInfo->waitEncryptAlarm != NULL) {
            AlarmCancel(devInfo->waitEncryptAlarm);
            AlarmDelete(devInfo->waitEncryptAlarm);
        }
        MEM_MALLOC.free(devInfo);
    }
}

int GAP_RegisterServiceSecurity(const BtAddr *addr, const GapServiceSecurityInfo *serviceInfo, uint16_t securityMode)
{
    int ret = GAP_SUCCESS;
    ListNode *node = NULL;
    ProfileSecurityInfo *regInfo = NULL;

    if (GapIsBredrEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    ProfileSecurityBlock *profileSecurityBlock = GapGetProfileSecurityBlock();
    node = ListGetFirstNode(profileSecurityBlock->registerlist);
    while (node != 0) {
        regInfo = (ProfileSecurityInfo *)ListGetNodeData(node);
        if ((addr == NULL || GapIsEmptyAddr(addr->addr) || GapAddrCompare(addr, &regInfo->addr)) &&
            (regInfo->info.direction == serviceInfo->direction) &&
            (regInfo->info.serviceId == serviceInfo->serviceId) &&
            (regInfo->info.protocolId == serviceInfo->protocolId) &&
            GapCompareChannelID(serviceInfo->protocolId, regInfo->info.channelId, serviceInfo->channelId)) {
            ret = GAP_ERR_REPEATED;
            LOG_WARN("Repeated Register Security %{public}d", serviceInfo->serviceId);
            break;
        }
        node = ListGetNextNode(node);
    }

    if (node == NULL) {
        regInfo = MEM_MALLOC.alloc(sizeof(ProfileSecurityInfo));
        if (regInfo == NULL) {
            ret = GAP_ERR_OUT_OF_RES;
        } else {
            (void)memset_s(regInfo, sizeof(ProfileSecurityInfo), 0x00, sizeof(ProfileSecurityInfo));
            if (addr != NULL) {
                (void)memcpy_s(&regInfo->addr, BT_ADDRESS_SIZE, addr, BT_ADDRESS_SIZE);
            }
            regInfo->info = *serviceInfo;
            regInfo->securityMode = securityMode;

            ListAddFirst(profileSecurityBlock->registerlist, regInfo);
        }
    }

    if (addr != NULL) {
        LOG_INFO("%{public}s:" BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr));
    }

    return ret;
}
int GAP_DeregisterServiceSecurity(const BtAddr *addr, const GapServiceSecurityInfo *serviceInfo)
{
    int ret = GAP_SUCCESS;
    ProfileSecurityBlock *profileSecurityBlock = NULL;
    ListNode *node = NULL;
    ProfileSecurityInfo *regInfo = NULL;

    if (GapIsBredrEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    profileSecurityBlock = GapGetProfileSecurityBlock();
    node = ListGetFirstNode(profileSecurityBlock->registerlist);
    while (node != 0) {
        regInfo = (ProfileSecurityInfo *)ListGetNodeData(node);
        if ((addr == NULL || GapIsEmptyAddr(addr->addr) || GapAddrCompare(addr, &regInfo->addr)) &&
            regInfo->info.direction == serviceInfo->direction && regInfo->info.serviceId == serviceInfo->serviceId &&
            regInfo->info.protocolId == serviceInfo->protocolId &&
            GapCompareChannelID(serviceInfo->protocolId, regInfo->info.channelId, serviceInfo->channelId)) {
            ListRemoveNode(profileSecurityBlock->registerlist, regInfo);
            break;
        }
        node = ListGetNextNode(node);
    }

    if (node == NULL) {
        LOG_WARN("%{public}s:not found", __FUNCTION__);
        ret = GAP_ERR_INVAL_PARAM;
    }

    if (addr != NULL) {
        LOG_INFO("%{public}s:" BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr));
    }

    return ret;
}

bool GapFindConnectionDeviceByAddr(void *nodeData, void *param)
{
    DeviceInfo *deviceInfo = nodeData;
    BtAddr *addr = param;

    if (!memcmp(&deviceInfo->addr, addr, sizeof(BtAddr))) {
        return true;
    }

    return false;
}

bool GapFindConnectionDeviceByHandle(void *nodeData, void *param)
{
    DeviceInfo *deviceInfo = nodeData;
    uint16_t handle = *(uint16_t *)param;

    if (handle == deviceInfo->handle) {
        return true;
    }

    return false;
}

bool GapFindCmpListData(void *nodeData, void *param)
{
    return (nodeData == param);
}

void GapStopUseAclConnection(void *dev)
{
    DeviceInfo *device = ListForEachData(GapGetConnectionInfoBlock()->devicelist, GapFindCmpListData, dev);
    if (device != NULL) {
        AlarmCancel(device->aclAlarm);
        BTM_AclRelease(device->handle);
    }
}

void GapUseAclConnectionTimeoutTask(void *ctx)
{
    DeviceInfo *dev = ((GapGeneralPointerInfo *)ctx)->pointer;

    GapStopUseAclConnection(dev);
}

void GapUseAclConnectionTimeout(void *dev)
{
    LOG_INFO("%{public}s: ", __FUNCTION__);
    GapGeneralPointerInfo *ctx = MEM_MALLOC.alloc(sizeof(GapGeneralPointerInfo));
    if (ctx == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return;
    }

    ctx->pointer = dev;

    int ret = GapRunTaskUnBlockProcess(GapUseAclConnectionTimeoutTask, ctx, NULL);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s: Task error:%{public}d.", __FUNCTION__, ret);
    }
}

void GapStartUseAclConnection(DeviceInfo *device, uint64_t timeMs)
{
    if (device != NULL) {
        AlarmCancel(device->aclAlarm);
        BTM_AclAddRef(device->handle);
        AlarmSet(device->aclAlarm, timeMs, GapUseAclConnectionTimeout, device);
    }
}

int GAP_GetLocalExtendedOOBData(GapOOBData *oobData192, GapOOBData *oobData256)
{
    int ret = GAP_SUCCESS;

    (void)memcpy_s(oobData192, sizeof(GapOOBData), &g_gapMng.bredr.oobData192, sizeof(GapOOBData));
    (void)memcpy_s(oobData256, sizeof(GapOOBData), &g_gapMng.bredr.oobData256, sizeof(GapOOBData));

    return ret;
}

int GAP_GetLocalAddr(BtAddr *addr)
{
    LOG_INFO("%{public}s:" BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr));

    return BTM_GetLocalAddr(addr);
}

int GAP_SetLocalName(const char *name, int length)
{
    HciWriteLocalNameParam param = {0};

    LOG_INFO("%{public}s: name:%{public}s", __FUNCTION__, name);

    if (GapIsBredrEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    (void)memcpy_s(param.localName, sizeof(param.localName), name, length);
    return HCI_WriteLocalName(&param);
}

int GAP_SetClassOfDevice(uint32_t cod)
{
    LOG_INFO("%{public}s: cmd:%06x", __FUNCTION__, cod);

    if (GapIsBredrEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    g_gapMng.bredr.classOfDevice = cod;

    return GapWriteClassOfDevice(cod);
}

static int GapWriteClassOfDevice(uint32_t cod)
{
    HciWriteClassofDeviceParam param = {
        .classofDevice = COD_UINT_TO_ARRAY(cod),
    };

    return HCI_WriteClassofDevice(&param);
}

void GapWriteClassOfDeviceComplete(const HciWriteClassofDeviceReturnParam *param)
{
    LOG_DEBUG("%{public}s:", __FUNCTION__);
    ScanModeBlock *scanModeBlock = NULL;
    GapSetScanModeResultCallback callback = NULL;
    uint8_t retStatus = param->status;

    if (GapIsBredrEnable() == false) {
        LOG_ERROR("%sNot Enable.", "");
        return;
    }

    scanModeBlock = GapGetScanModeBlock();
    if (scanModeBlock->status == GAP_SCANMODE_STATUS_SETTING) {
        if (retStatus == HCI_SUCCESS) {
            int ret;
            if (scanModeBlock->connectableMode.mode) {
                ret = GapWritePageScanActivity();
            } else {
                ret = GapWriteScanEnable();
            }
            if (ret != BT_NO_ERROR) {
                LOG_ERROR("HCI Command Error ret = %{public}d.", ret);
                retStatus = GAP_STATUS_FAILED;
                scanModeBlock->status = GAP_SCANMODE_STATUS_IDLE;
                callback = scanModeBlock->callback;
            }
        } else {
            scanModeBlock->status = GAP_SCANMODE_STATUS_IDLE;
            callback = scanModeBlock->callback;
        }
    }

    if (callback != NULL) {
        callback(retStatus, scanModeBlock->context);
    }
}

int GAP_SetExtendedInquiryResponse(const uint8_t eir[GAP_EIR_SIZE_MAX])
{
    int ret;
    HciWriteExtendedInquiryResponseParam hciCmdParam;

    LOG_INFO("%{public}s:", __FUNCTION__);

    if (GapIsBredrEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    (void)memcpy_s(hciCmdParam.extendInquiryRes, sizeof(hciCmdParam.extendInquiryRes), eir, GAP_EIR_SIZE_MAX);
    hciCmdParam.fecRequired = GAP_EIR_FEC_REQUIRED;

    ret = HCI_WriteExtendedInquiryResponse(&hciCmdParam);

    return ret;
}

void GapSetExtendedInquiryResponseComplete(const HciWriteExtendedInquiryResponseReturnParam *param)
{
    if (param->status) {
        LOG_WARN("%{public}s:", __FUNCTION__);
    }
}

static DeviceInfo *GapAllocDeviceInfo(const BtAddr *addr, const uint16_t handle)
{
    DeviceInfo *deviceInfo = MEM_MALLOC.alloc(sizeof(DeviceInfo));
    if (deviceInfo == NULL) {
        LOG_ERROR("%{public}s:malloc error.", __FUNCTION__);
    } else {
        (void)memset_s(deviceInfo, sizeof(DeviceInfo), 0x00, sizeof(DeviceInfo));
        deviceInfo->addr = *addr;
        deviceInfo->handle = handle;
        deviceInfo->status = GAP_DEV_SEC_STATUS_IDLE;
        deviceInfo->authenticationStatus = GAP_AUTH_STATUS_IDLE;
        deviceInfo->encryptionStatus = GAP_ENC_STATUS_IDLE;
        deviceInfo->inDedicatedBonding = false;
        deviceInfo->linkkeyType = GAP_LINK_KEY_TYPE_UNKNOWN;
        deviceInfo->remoteAuthReq = AUTHENTICATION_UNKNOWN_MITM;
        deviceInfo->alarm = AlarmCreate("gapSec", false);
        deviceInfo->aclAlarm = AlarmCreate("gapAcl", false);
    }

    return deviceInfo;
}

void GapAclConnectionComplete(const BtmAclConnectCompleteParam *param, void *context)
{
    ConnectionInfoBlock *connectionInfoBlock = NULL;
    ProfileSecurityBlock *profileSecurityBlock = NULL;
    DeviceInfo *deviceInfo = NULL;
    RequestSecInfo *reqInfo = NULL;
    connectionInfoBlock = GapGetConnectionInfoBlock();
    profileSecurityBlock = GapGetProfileSecurityBlock();
    if (param->status == HCI_SUCCESS) {
        deviceInfo = GapAllocDeviceInfo(param->addr, param->connectionHandle);
        if (deviceInfo == NULL) {
            LOG_ERROR("%{public}s:malloc error.", __FUNCTION__);
        } else {
            ListAddFirst(connectionInfoBlock->devicelist, deviceInfo);
            GapUpdateSecurityRequest(deviceInfo, GAP_SEC_EVENT_CONNECT_COMP, param->status);
            if (*GapGetSecurityMode() != SEC_MODE_2) {
                GapIsRemoteDeviceSupportHostSecureSimplePairingAsync(&deviceInfo->addr);
            }
            GapRequestSecurityProcess();
        }
    } else {
        ListNode *node = ListGetFirstNode(profileSecurityBlock->requestlist);
        while (node != 0) {
            reqInfo = ListGetNodeData(node);
            node = ListGetNextNode(node);
            if (!GapAddrCompare(&reqInfo->addr, param->addr)) {
                continue;
            }
            if (BtmInteropIsMatchedAddr(INTEROP_AUTO_RETRY_PAIRING, param->addr)) {
                GapAuthenticationRetry(NULL, reqInfo, param->status);
                continue;
            }

            reqInfo->status = GAP_SEC_REQ_STATUS_FAILED;
            reqInfo->hciStatus = param->status;
            if (reqInfo->info.serviceId == GAP) {
                GapDoAuthenticationCallback(reqInfo);
            }
            if (!reqInfo->doCallback) {
                reqInfo->doCallback = true;
                GapDoSecurityCallback(reqInfo);
            }
        }
    }
}

void GapAclDisconnectionComplete(uint8_t status, uint16_t connectionHandle, uint8_t reason, void *context)
{
    ConnectionInfoBlock *connectionInfoBlock = NULL;
    if (status == HCI_SUCCESS) {
        DeviceInfo *info = NULL;
        connectionInfoBlock = GapGetConnectionInfoBlock();
        info = ListForEachData(
            connectionInfoBlock->devicelist, GapFindConnectionDeviceByHandle, (void *)&connectionHandle);
        if (info != NULL) {
            GapUpdateSecurityRequest(info, GAP_SEC_EVENT_ACL_DISCONNECT, reason);
            GapRequestSecurityProcess();

            ListRemoveNode(connectionInfoBlock->devicelist, info);
        }
    }
}

#endif

#ifdef GAP_LE_SUPPORT

bool GapAddrIsResolvablePrivateAddress(const BtAddr *addr)
{
    return (addr->type == BT_RANDOM_DEVICE_ADDRESS) && (addr->addr[5] & 0x40) && !(addr->addr[5] & 0x80);
}

bool GapAddrIsStaticAddress(const BtAddr *addr)
{
    return (addr->type == BT_RANDOM_DEVICE_ADDRESS) && (addr->addr[5] & 0x40) && (addr->addr[5] & 0x80);
}

bool GapAddrIsPublicAddress(const BtAddr *addr)
{
    return addr->type == BT_PUBLIC_DEVICE_ADDRESS;
}

bool GapAddrIsIdentityAddress(const BtAddr *addr)
{
    return addr->type == BT_PUBLIC_IDENTITY_ADDRESS || addr->type == BT_RANDOM_IDENTITY_ADDRESS;
}

bool GapFindLeConnectionDeviceByAddr(void *nodeData, void *param)
{
    LeDeviceInfo *deviceInfo = nodeData;
    BtAddr *addr = param;

    return GapAddrCompare(&deviceInfo->addr, addr);
}

bool GapFindLeConnectionDeviceByHandle(void *nodeData, void *param)
{
    LeDeviceInfo *deviceInfo = nodeData;
    uint16_t *handle = (uint16_t *)param;

    return deviceInfo->handle == *handle;
}

int GAP_LeSetRole(uint8_t role)
{
    LOG_INFO("%{public}s: role[%02x]", __FUNCTION__, role);

    if (GapIsLeEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    g_gapMng.le.local.role = role;
    if (GapLeRolesCheck(GAP_LE_ROLE_PREIPHERAL | GAP_LE_ROLE_CENTRAL) == true) {
        BtmKey key;
        BTM_GetLocalIdentityResolvingKey(&key);
        SMP_SetIRK(key.key);
        SMP_SetIdentAddr(&g_gapMng.le.local.addr);
    }

    return GAP_SUCCESS;
}

int GAP_LeSetStaticIdentityAddr(uint8_t addr[BT_ADDRESS_SIZE])
{
    int ret;
    LeLocalInfo *localInfo = NULL;

    LOG_INFO("%{public}s:" BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr));

    if (GapIsLeEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    localInfo = GapGetLeLocalInfo();
    if (localInfo->addr.type == BT_PUBLIC_DEVICE_ADDRESS) {
        LOG_WARN("%{public}s: have public address " BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(localInfo->addr.addr));
        localInfo->addr.type = BT_RANDOM_DEVICE_ADDRESS;
    }
    (void)memcpy_s(localInfo->addr.addr, BT_ADDRESS_SIZE, addr, BT_ADDRESS_SIZE);
    ret = SMP_SetIdentAddr(&localInfo->addr);

    return ret;
}

NO_SANITIZE("cfi") void GapGenerateRPAResult(uint8_t status, const uint8_t *addr)
{
    GenResPriAddrResult callback = NULL;
    void *context = NULL;

    if (GapGetLeRandomAddressBlock()->generationInfo.processing) {
        callback = GapGetLeRandomAddressBlock()->generationInfo.callback;
        context = GapGetLeRandomAddressBlock()->generationInfo.context;

        GapGetLeRandomAddressBlock()->generationInfo.processing = false;
        GapGetLeRandomAddressBlock()->generationInfo.callback = NULL;
        GapGetLeRandomAddressBlock()->generationInfo.context = NULL;
    }

    if (callback != NULL) {
        callback(status, addr, context);
    }
}

int GAP_LeGenResPriAddrAsync(GenResPriAddrResult callback, void *context)
{
    LOG_INFO("%{public}s:", __FUNCTION__);
    int ret;

    BtmKey localIRK = {0};

    ret = BTM_GetLocalIdentityResolvingKey(&localIRK);
    if (ret != BT_NO_ERROR) {
        return ret;
    }

    ret = SMP_GenerateRPA(localIRK.key);
    if (ret != BT_NO_ERROR) {
        return ret;
    }

    if (GapGetLeRandomAddressBlock()->generationInfo.processing == false) {
        GapGetLeRandomAddressBlock()->generationInfo.callback = callback;
        GapGetLeRandomAddressBlock()->generationInfo.context = context;
        GapGetLeRandomAddressBlock()->generationInfo.processing = true;
    } else {
        ret = GAP_ERR_INVAL_STATE;
    }

    return ret;
}

#endif

Module g_gap = {
    .name = MODULE_NAME_GAP,
    .init = GapInitialize,
    .startup = GapEnable,
    .shutdown = GapDisable,
    .cleanup = GapFinalize,
    .dependencies = {MODULE_NAME_HCI, MODULE_NAME_L2CAP, MODULE_NAME_SMP},
};

MODULE_DECL(g_gap)
