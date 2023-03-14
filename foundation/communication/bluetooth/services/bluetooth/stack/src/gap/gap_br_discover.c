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
#include "gap_internal.h"

#include <securec.h>

#include "log.h"

#include "btm.h"
#include "btm/btm_inq_db.h"

typedef struct {
    GapDiscoveryCallback callback;
    void *context;
} DiscoveryCallback;

static DiscoveryCallback g_discoveryCallback;

int GAP_RegisterDiscoveryCallback(const GapDiscoveryCallback *callback, void *context)
{
    LOG_INFO("%{public}s:%{public}s", __FUNCTION__, callback ? "register" : "NULL");
    if (callback == NULL) {
        (void)memset_s(&g_discoveryCallback.callback,
            sizeof(g_discoveryCallback.callback),
            0x00,
            sizeof(g_discoveryCallback.callback));
    } else {
        g_discoveryCallback.callback = *callback;
    }
    g_discoveryCallback.context = context;
    return GAP_SUCCESS;
}

int GAP_DeregisterDiscoveryCallback(void)
{
    (void)memset_s(&g_discoveryCallback.callback,
        sizeof(g_discoveryCallback.callback),
        0x00,
        sizeof(g_discoveryCallback.callback));
    g_discoveryCallback.context = NULL;
    return GAP_SUCCESS;
}

int GAP_Inquiry(uint8_t mode, uint8_t inquiryLength)
{
    int ret;
    InquiryBlock *inquiryBlock = NULL;
    const uint8_t resNum = 0;

    LOG_INFO("%{public}s:%hhu[%02x]", __FUNCTION__, mode, inquiryLength);

    if (mode > GAP_INQUIRY_MODE_LIMITED) {
        return GAP_ERR_INVAL_PARAM;
    }

    if (GapIsBredrEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    inquiryBlock = GapGetInquiryBlock();
    if (inquiryBlock->status != GAP_INQUIRY_STATUS_IDLE) {
        ret = GAP_ERR_INVAL_STATE;
    } else {
        uint32_t iacLAP = LAP_GENERAL_INQUIRY_ACCESS;
        inquiryBlock->status = GAP_INQUIRY_STATUS_START;

        if (mode == GAP_INQUIRY_MODE_GENERAL) {
            iacLAP = LAP_GENERAL_INQUIRY_ACCESS;
        } else if (mode == GAP_INQUIRY_MODE_LIMITED) {
            iacLAP = LAP_LIMITED_INQUIRY_ACCESS;
        }

        HciInquiryeParam param = {
            .lap = iacLAP,
            .inquiryLen = inquiryLength,
            .numResponses = resNum,
        };
        ret = HCI_Inquiry(&param);
    }

    return ret;
}

NO_SANITIZE("cfi") void GapOnInquiryComplete(const HciInquiryCompleteEventParam *eventParam)
{
    InquiryBlock *inquiryBlock = NULL;

    LOG_DEBUG("%{public}s:", __FUNCTION__);

    inquiryBlock = GapGetInquiryBlock();
    inquiryBlock->status = GAP_INQUIRY_STATUS_IDLE;

    if (g_discoveryCallback.callback.inquiryComplete) {
        g_discoveryCallback.callback.inquiryComplete(eventParam->status, g_discoveryCallback.context);
    }
}

void GapOnInquiryResult(const HciInquiryResultEventParam *eventParam)
{
    BtmInquiryInfo inquiryInfo;

    for (uint8_t i = 0; i < eventParam->numResponses; i++) {
        BtAddr addr = BT_ADDR_NULL;
        GapChangeHCIAddr(&addr, &eventParam->responses[i].bdAddr, BT_PUBLIC_DEVICE_ADDRESS);
        uint32_t cod = COD_ARRAY_TO_UINT(eventParam->responses[i].classOfDevice);

        inquiryInfo.clockOffset = eventParam->responses[i].clockOffset;
        inquiryInfo.pageScanRepetitionMode = eventParam->responses[i].pageScanRepetitionMode;
        BtmAssignOrUpdateInquiryInfo(&addr, &inquiryInfo);

        LOG_DEBUG("%{public}s: " BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr.addr));
        if (g_discoveryCallback.callback.inquiryResult) {
            g_discoveryCallback.callback.inquiryResult(&addr, cod, g_discoveryCallback.context);
        }
    }
}

NO_SANITIZE("cfi") void GapOnInquiryResultRssi(const HciInquiryResultWithRssiEventParam *eventParam)
{
    BtmInquiryInfo inquiryInfo;

    for (uint8_t i = 0; i < eventParam->numResponses; i++) {
        BtAddr addr = BT_ADDR_NULL;
        GapChangeHCIAddr(&addr, &eventParam->responses[i].bdAddr, BT_PUBLIC_DEVICE_ADDRESS);

        uint32_t cod = COD_ARRAY_TO_UINT(eventParam->responses[i].classOfDevice);
        int8_t rssi = (int8_t)eventParam->responses[i].rssi;

        inquiryInfo.clockOffset = eventParam->responses[i].clockOffset;
        inquiryInfo.pageScanRepetitionMode = eventParam->responses[i].pageScanRepetitionMode;
        BtmAssignOrUpdateInquiryInfo(&addr, &inquiryInfo);

        LOG_DEBUG("%{public}s: " BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr.addr));
        if (g_discoveryCallback.callback.inquiryResultRssi) {
            g_discoveryCallback.callback.inquiryResultRssi(&addr, cod, rssi, g_discoveryCallback.context);
        }
    }
}

NO_SANITIZE("cfi") void GapOnEntendedInquiryResult(const HciExtendedInquiryResultEventParam *eventParam)
{
    BtmInquiryInfo inquiryInfo;

    BtAddr addr = BT_ADDR_NULL;
    GapChangeHCIAddr(&addr, &eventParam->bdAddr, BT_PUBLIC_DEVICE_ADDRESS);

    uint32_t cod = COD_ARRAY_TO_UINT(eventParam->classofDevice);
    int8_t rssi = (int8_t)eventParam->rssi;

    inquiryInfo.clockOffset = eventParam->clockOffset;
    inquiryInfo.pageScanRepetitionMode = eventParam->pageScanRepetitionMode;
    BtmAssignOrUpdateInquiryInfo(&addr, &inquiryInfo);

    LOG_DEBUG("%{public}s: " BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr.addr));
    if (g_discoveryCallback.callback.extendedInquiryResult) {
        g_discoveryCallback.callback.extendedInquiryResult(
            &addr, cod, rssi, eventParam->extendedInquiryResponse, g_discoveryCallback.context);
    }
}

int GAP_InquiryCancel(void)
{
    int ret;
    InquiryBlock *inquiryBlock = NULL;

    LOG_INFO("%{public}s:", __FUNCTION__);

    if (GapIsBredrEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    inquiryBlock = GapGetInquiryBlock();
    if (inquiryBlock->status == GAP_INQUIRY_STATUS_START) {
        inquiryBlock->status = GAP_INQUIRY_STATUS_CANCEL;
        ret = HCI_InquiryCancel();
    } else {
        ret = GAP_ERR_INVAL_STATE;
    }

    return ret;
}

NO_SANITIZE("cfi") void GapInquiryCancelComplete(const HciInquiryCancelReturnParam *param)
{
    LOG_DEBUG("%{public}s:", __FUNCTION__);
    InquiryBlock *inquiryBlock = NULL;
    bool callback = false;

    inquiryBlock = GapGetInquiryBlock();
    if (inquiryBlock->status == GAP_INQUIRY_STATUS_CANCEL) {
        callback = true;
        if (param->status != HCI_SUCCESS) {
            LOG_ERROR("Inquiry Cancel Fail. status = %hhu", param->status);
        } else {
            inquiryBlock->status = GAP_INQUIRY_STATUS_IDLE;
        }
    } else {
        LOG_ERROR("Error Status");
    }

    if (callback && g_discoveryCallback.callback.inquiryComplete) {
        g_discoveryCallback.callback.inquiryComplete(param->status, g_discoveryCallback.context);
    }
}

int GAP_GetRemoteName(const BtAddr *addr)
{
    int ret;
    RemoteNameBlock *remoteNameBlock = NULL;
    BtmInquiryInfo inquiryInfo;

    LOG_INFO("%{public}s:" BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr));

    if (GapIsBredrEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    remoteNameBlock = GapGetRemoteNameBlock();
    if (remoteNameBlock->status != GAP_REMOTE_NAME_STATUS_IDLE) {
        ret = GAP_ERR_INVAL_STATE;
    } else {
        HciRemoteNameRequestParam hciCmdParam = {0};

        remoteNameBlock->status = GAP_REMOTE_NAME_STATUS_START;
        (void)memcpy_s(hciCmdParam.addr.raw, BT_ADDRESS_SIZE, addr->addr, BT_ADDRESS_SIZE);

        ret = BtmQueryInquiryInfoByAddr(addr, &inquiryInfo);
        if (ret == BT_NO_ERROR) {
            hciCmdParam.clockOffset = inquiryInfo.clockOffset;
            hciCmdParam.pageScanRepetMode = inquiryInfo.pageScanRepetitionMode;
        }

        ret = HCI_RemoteNameRequest(&hciCmdParam);
    }

    return ret;
}

NO_SANITIZE("cfi") void GapOnGetRemoteNameComplete(const HciRemoteNameRequestCompleteEventParam *eventParam)
{
    LOG_DEBUG("%{public}s:", __FUNCTION__);
    RemoteNameBlock *remoteNameBlock = NULL;

    remoteNameBlock = GapGetRemoteNameBlock();
    if (remoteNameBlock->status == GAP_REMOTE_NAME_STATUS_IDLE) {
        LOG_ERROR("Error Status");
    }
    remoteNameBlock->status = GAP_REMOTE_NAME_STATUS_IDLE;
    BtAddr addr = BT_ADDR_NULL;
    GapChangeHCIAddr(&addr, &eventParam->bdAddr, BT_PUBLIC_DEVICE_ADDRESS);

    if (g_discoveryCallback.callback.remoteName) {
        g_discoveryCallback.callback.remoteName(
            eventParam->status, &addr, eventParam->remoteName, g_discoveryCallback.context);
    }
}

int GAP_GetRemoteNameCancel(const BtAddr *addr)
{
    int ret;
    RemoteNameBlock *remoteNameBlock = NULL;

    LOG_INFO("%{public}s:" BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr));

    if (GapIsBredrEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    remoteNameBlock = GapGetRemoteNameBlock();
    if (remoteNameBlock->status == GAP_REMOTE_NAME_STATUS_IDLE) {
        ret = GAP_ERR_INVAL_STATE;
    } else {
        remoteNameBlock->status = GAP_REMOTE_NAME_STATUS_CANCEL;
        HciRemoteNameRequestCancelParam hciCmdParam;
        (void)memcpy_s(hciCmdParam.addr.raw, BT_ADDRESS_SIZE, addr->addr, BT_ADDRESS_SIZE);

        ret = HCI_RemoteNameRequestCancel(&hciCmdParam);
    }

    return ret;
}

void GapGetRemoteNameCancelComplete(const HciRemoteNameRequestCancelReturnParam *param)
{
    LOG_DEBUG("%{public}s:", __FUNCTION__);
    RemoteNameBlock *remoteNameBlock = NULL;

    remoteNameBlock = GapGetRemoteNameBlock();
    if (remoteNameBlock->status == GAP_REMOTE_NAME_STATUS_CANCEL) {
        if (param->status != HCI_SUCCESS) {
            LOG_ERROR("Get Remote Name Cancel Fail. status = %hhu", param->status);
        }
    } else {
        LOG_ERROR("Error Status");
    }
}
