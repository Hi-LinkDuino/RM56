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

#include "gap_internal.h"
#include "gap_task_internal.h"

#include <securec.h>

#include "allocator.h"
#include "log.h"
#include "thread.h"

#include "btm/btm_thread.h"

static int GapProcessHciEventInTask(TaskFunc run, const void *ctx, uint32_t ctxLen, TaskFunc free)
{
    void *hciParam = NULL;
    if (ctx != NULL && ctxLen != 0) {
        hciParam = MEM_MALLOC.alloc(ctxLen);
        if (hciParam == NULL) {
            return BT_NO_MEMORY;
        }
        (void)memcpy_s(hciParam, ctxLen, ctx, ctxLen);
    }

    return GapRunTaskUnBlockProcess(run, hciParam, free);
}

#ifdef GAP_BREDR_SUPPORT

static void GapRecvInquiryCancelComplete(const HciInquiryCancelReturnParam *param)
{
    HILOGI("status: 0x%{public}02x", param->status);
    int ret = GapProcessHciEventInTask((TaskFunc)GapInquiryCancelComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvLinkKeyRequestReplyComplete(const HciLinkKeyRequestReplyReturnParam *param)
{
    HILOGI("status: 0x%{public}02x", param->status);
    int ret = GapProcessHciEventInTask((TaskFunc)GapLinkKeyRequestReplyComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvLinkKeyRequestNegativeReplyComplete(const HciLinkKeyRequestNegativeReplyReturnParam *param)
{
    HILOGI("status: 0x%{public}02x", param->status);
    int ret = GapProcessHciEventInTask((TaskFunc)GapLinkKeyRequestNegativeReplyComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvPINCodeRequestReplyComplete(const HciPinCodeRequestReplyReturnParam *param)
{
    HILOGI("status: 0x%{public}02x", param->status);
    int ret = GapProcessHciEventInTask((TaskFunc)GapPINCodeRequestReplyComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvPINCodeRequestNegativeReplyComplete(const HciPinCodeRequestNegativeReplyReturnParam *param)
{
    HILOGI("status: 0x%{public}02x", param->status);
    int ret = GapProcessHciEventInTask((TaskFunc)GapPINCodeRequestNegativeReplyComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvGetRemoteNameCancelComplete(const HciRemoteNameRequestCancelReturnParam *param)
{
    HILOGI("status: 0x%{public}02x", param->status);
    int ret = GapProcessHciEventInTask((TaskFunc)GapGetRemoteNameCancelComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvIOCapabilityRequestReplyComplete(const HciIOCapabilityRequestReplyReturnParam *param)
{
    HILOGI("status: 0x%{public}02x", param->status);
    int ret = GapProcessHciEventInTask((TaskFunc)GapIOCapabilityRequestReplyComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvUserConfirmationRequestReplyComplete(const HciUserConfirmationRequestReplyReturnParam *param)
{
    HILOGI("status: 0x%{public}02x", param->status);
    int ret = GapProcessHciEventInTask((TaskFunc)GapUserConfirmationRequestReplyComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvUserConfirmationRequestNegativeReplyComplete(
    const HciUserConfirmationRequestNegativeReplyReturnParam *param)
{
    HILOGI("status: 0x%{public}02x", param->status);
    int ret = GapProcessHciEventInTask(
        (TaskFunc)GapUserConfirmationRequestNegativeReplyComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvUserPasskeyRequestReplyComplete(const HciUserPasskeyRequestReplyReturnParam *param)
{
    HILOGI("status: 0x%{public}02x", param->status);
    int ret = GapProcessHciEventInTask((TaskFunc)GapUserPasskeyRequestReplyComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvUserPasskeyRequestNegativeReplyComplete(const HciUserPasskeyRequestNegativeReplyReturnParam *param)
{
    HILOGI("status: 0x%{public}02x", param->status);
    int ret =
        GapProcessHciEventInTask((TaskFunc)GapUserPasskeyRequestNegativeReplyComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvRemoteOOBDataRequestReplyComplete(const HciRemoteOobDataRequestReplyReturnParam *param)
{
    HILOGI("status: 0x%{public}02x", param->status);
    int ret = GapProcessHciEventInTask((TaskFunc)GapRemoteOOBDataRequestReplyComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvRemoteOOBDataRequestNegativeReplyComplete(
    const HciRemoteOobDataRequestNegativeReplyReturnParam *param)
{
    HILOGI("status: 0x%{public}02x", param->status);
    int ret =
        GapProcessHciEventInTask((TaskFunc)GapRemoteOOBDataRequestNegativeReplyComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvIOCapabilityRequestNegativeReplyComplete(const HciIoCapabilityRequestNegativeReplyReturnParam *param)
{
    HILOGI("status: 0x%{public}02x", param->status);
    int ret =
        GapProcessHciEventInTask((TaskFunc)GapIOCapabilityRequestNegativeReplyComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvRemoteOOBExtendedDataRequestReplyComplete(
    const HciRemoteOobExtendedDataRequestReplyReturnParam *param)
{
    HILOGI("status: 0x%{public}02x", param->status);
    int ret =
        GapProcessHciEventInTask((TaskFunc)GapRemoteOOBExtendedDataRequestReplyComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvWriteScanEnableComplete(const HciWriteScanEnableReturnParam *param)
{
    HILOGI("status: 0x%{public}02x", param->status);
    int ret = GapProcessHciEventInTask((TaskFunc)GapWriteScanEnableComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvWritePageScanActivityComplete(const HciWritePageScanActivityReturnParam *param)
{
    HILOGI("status: 0x%{public}02x", param->status);
    int ret = GapProcessHciEventInTask((TaskFunc)GapWritePageScanActivityComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvWriteInquiryScanActivityComplete(const HciWriteInquiryScanActivityReturnParam *param)
{
    HILOGI("status: 0x%{public}02x", param->status);
    int ret = GapProcessHciEventInTask((TaskFunc)GapWriteInquiryScanActivityComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvWriteClassOfDeviceComplete(const HciWriteClassofDeviceReturnParam *param)
{
    HILOGI("status: 0x%{public}02x", param->status);
    int ret = GapProcessHciEventInTask((TaskFunc)GapWriteClassOfDeviceComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvWriteCurrentIACLAPComplete(const HciWriteCurrentIacLapReturnParam *param)
{
    HILOGI("status: 0x%{public}02x", param->status);
    int ret = GapProcessHciEventInTask((TaskFunc)GapWriteCurrentIACLAPComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvWriteInquiryScanTypeComplete(const HciWriteInquiryScanTypeReturnParam *param)
{
    HILOGI("status: 0x%{public}02x", param->status);
    int ret = GapProcessHciEventInTask((TaskFunc)GapWriteInquiryScanTypeComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvWritePageScanTypeComplete(const HciWritePageScanTypeReturnParam *param)
{
    HILOGI("status: 0x%{public}02x", param->status);
    int ret = GapProcessHciEventInTask((TaskFunc)GapWritePageScanTypeComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvSetExtendedInquiryResponseComplete(const HciWriteExtendedInquiryResponseReturnParam *param)
{
    HILOGI("status: 0x%{public}02x", param->status);
    int ret = GapProcessHciEventInTask((TaskFunc)GapSetExtendedInquiryResponseComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvWriteAuthenticatedPayloadTimeoutComplete(const HciWriteAuthenticatedPayloadTimeoutReturnParam *param)
{
    HILOGI("status: 0x%{public}02x", param->status);
    int ret =
        GapProcessHciEventInTask((TaskFunc)GapWriteAuthenticatedPayloadTimeoutComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvReadLocalOobDataComplete(const HciReadLocalOOBDataReturnParam *param)
{
    HILOGI("status: 0x%{public}02x", param->status);
    int ret = GapProcessHciEventInTask((TaskFunc)GapReadLocalOobDataComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvReadLocalOobExtendedDataComplete(const HciReadLocalOobExtendedDataReturnParam *param)
{
    HILOGI("status: 0x%{public}02x", param->status);
    int ret = GapProcessHciEventInTask((TaskFunc)GapReadLocalOobExtendedDataComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvInquiryComplete(const HciInquiryCompleteEventParam *eventParam)
{
    HILOGI("status: 0x%{public}02x", eventParam->status);
    int ret = GapProcessHciEventInTask((TaskFunc)GapOnInquiryComplete, eventParam, sizeof(*eventParam), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapFreeInquiryResult(void *ctx)
{
    HciInquiryResultEventParam *hciParam = ctx;
    MEM_MALLOC.free(hciParam->responses);
}

static void GapRecvInquiryResult(const HciInquiryResultEventParam *eventParam)
{
    HILOGI("num: %{public}hhu", eventParam->numResponses);
    HciInquiryResultEventParam hciParam = *eventParam;
    hciParam.responses = MEM_MALLOC.alloc(hciParam.numResponses * sizeof(HciInquiryResult));
    if (hciParam.responses == NULL) {
        HILOGE("Alloc error.");
        return;
    }

    (void)memcpy_s(hciParam.responses,
        hciParam.numResponses * sizeof(HciInquiryResult),
        eventParam->responses,
        hciParam.numResponses * sizeof(HciInquiryResult));

    int ret = GapProcessHciEventInTask((TaskFunc)GapOnInquiryResult, &hciParam, sizeof(hciParam), GapFreeInquiryResult);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvAuthenticationComplete(const HciAuthenticationCompleteEventParam *eventParam)
{
    HILOGI("handle: 0x%{public}04x, status: 0x%{public}02x", eventParam->connectionHandle, eventParam->status);
    int ret = GapProcessHciEventInTask((TaskFunc)GapOnAuthenticationComplete, eventParam, sizeof(*eventParam), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvGetRemoteNameComplete(const HciRemoteNameRequestCompleteEventParam *eventParam)
{
    HILOGI("addr:" BT_ADDR_FMT "status: 0x%{public}02x",
        BT_ADDR_FMT_OUTPUT(eventParam->bdAddr.raw), eventParam->status);
    int ret = GapProcessHciEventInTask((TaskFunc)GapOnGetRemoteNameComplete, eventParam, sizeof(*eventParam), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvEncryptionChangeEvent(const HciEncryptionChangeEventParam *eventParam)
{
    HILOGI("handle: 0x%{public}04x, status: 0x%{public}02x, enable: %{public}hhu",
        eventParam->connectionHandle,
        eventParam->status,
        eventParam->encryptionEnabled);
    int ret = GapProcessHciEventInTask((TaskFunc)GapOnEncryptionChangeEvent, eventParam, sizeof(*eventParam), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvPINCodeRequestEvent(const HciPinCodeRequestEventParam *eventParam)
{
    HILOGI("addr:" BT_ADDR_FMT, BT_ADDR_FMT_OUTPUT(eventParam->bdAddr.raw));
    int ret = GapProcessHciEventInTask((TaskFunc)GapOnPINCodeRequestEvent, eventParam, sizeof(*eventParam), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvLinkKeyRequestEvent(const HciLinkKeyRequestEventParam *eventParam)
{
    HILOGI("addr:" BT_ADDR_FMT, BT_ADDR_FMT_OUTPUT(eventParam->bdAddr.raw));
    int ret = GapProcessHciEventInTask((TaskFunc)GapOnLinkKeyRequestEvent, eventParam, sizeof(*eventParam), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvLinkKeyNotificationEvent(const HciLinkKeyNotificationEventParam *eventParam)
{
    HILOGI("addr:" BT_ADDR_FMT "type: %{public}d", BT_ADDR_FMT_OUTPUT(eventParam->bdAddr.raw), eventParam->keyType);
    int ret = GapProcessHciEventInTask((TaskFunc)GapOnLinkKeyNotificationEvent, eventParam, sizeof(*eventParam), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapFreeInquiryResultRssi(void *ctx)
{
    HciInquiryResultWithRssiEventParam *hciParam = ctx;

    MEM_MALLOC.free(hciParam->responses);
}

static void GapRecvInquiryResultRssi(const HciInquiryResultWithRssiEventParam *eventParam)
{
    HILOGI("num: %{public}hhu", eventParam->numResponses);
    HciInquiryResultWithRssiEventParam hciParam = *eventParam;
    hciParam.responses = MEM_MALLOC.alloc(hciParam.numResponses * sizeof(HciInquiryResultWithRssi));
    if (hciParam.responses == NULL) {
        HILOGE("Alloc error.");
        return;
    }

    (void)memcpy_s(hciParam.responses,
        hciParam.numResponses * sizeof(HciInquiryResultWithRssi),
        eventParam->responses,
        hciParam.numResponses * sizeof(HciInquiryResultWithRssi));

    int ret = GapProcessHciEventInTask(
        (TaskFunc)GapOnInquiryResultRssi, &hciParam, sizeof(hciParam), GapFreeInquiryResultRssi);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvEntendedInquiryResult(const HciExtendedInquiryResultEventParam *eventParam)
{
    HILOGI("num: %{public}hhu", eventParam->numResponses);
    int ret = GapProcessHciEventInTask((TaskFunc)GapOnEntendedInquiryResult, eventParam, sizeof(*eventParam), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvEncryptionKeyRefreshComplete(const HciEncryptionKeyRefreshCompleteEventParam *eventParam)
{
    HILOGI("handle: 0x%{public}04x, status: 0x%{public}02x", eventParam->connectionHandle, eventParam->status);
    int ret =
        GapProcessHciEventInTask((TaskFunc)GapOnEncryptionKeyRefreshComplete, eventParam, sizeof(*eventParam), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvIOCapabilityRequestEvent(const HciIoCapabilityRequestEventParam *eventParam)
{
    HILOGI("addr:" BT_ADDR_FMT, BT_ADDR_FMT_OUTPUT(eventParam->bdAddr.raw));
    int ret = GapProcessHciEventInTask((TaskFunc)GapOnIOCapabilityRequestEvent, eventParam, sizeof(*eventParam), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvIOCapabilityResponseEvent(const HciIoCapabilityResponseEventParam *eventParam)
{
    HILOGI("addr:" BT_ADDR_FMT "IO: %{public}hhu, Authreq: %{public}hhu",
        BT_ADDR_FMT_OUTPUT(eventParam->bdAddr.raw),
        eventParam->IOCapability,
        eventParam->authenticationRequirements);
    int ret = GapProcessHciEventInTask((TaskFunc)GapOnIOCapabilityResponseEvent, eventParam, sizeof(*eventParam), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvUserConfirmationRequestEvent(const HciUserConfirmationRequestEventParam *eventParam)
{
    HILOGI("addr:" BT_ADDR_FMT, BT_ADDR_FMT_OUTPUT(eventParam->bdAddr.raw));
    int ret =
        GapProcessHciEventInTask((TaskFunc)GapOnUserConfirmationRequestEvent, eventParam, sizeof(*eventParam), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvUserPasskeyRequestEvent(const HciUserPasskeyRequestEventParam *eventParam)
{
    HILOGI("addr:" BT_ADDR_FMT, BT_ADDR_FMT_OUTPUT(eventParam->bdAddr.raw));
    int ret = GapProcessHciEventInTask((TaskFunc)GapOnUserPasskeyRequestEvent, eventParam, sizeof(*eventParam), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvRemoteOOBDataRequestEvent(const HciRemoteOobDataRequestEventParam *eventParam)
{
    HILOGI("addr:" BT_ADDR_FMT, BT_ADDR_FMT_OUTPUT(eventParam->bdAddr.raw));
    int ret = GapProcessHciEventInTask((TaskFunc)GapOnRemoteOOBDataRequestEvent, eventParam, sizeof(*eventParam), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvSimplePairingComplete(const HciSimplePairingCompleteEventParam *eventParam)
{
    HILOGI("addr:" BT_ADDR_FMT "status: 0x%{public}02x",
        BT_ADDR_FMT_OUTPUT(eventParam->bdAddr.raw),
        eventParam->status);
    int ret = GapProcessHciEventInTask((TaskFunc)GapOnSimplePairingComplete, eventParam, sizeof(*eventParam), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvUserPasskeyNotificationEvent(const HciUserPasskeyNotificationEventParam *eventParam)
{
    HILOGI("addr:" BT_ADDR_FMT, BT_ADDR_FMT_OUTPUT(eventParam->bdAddr.raw));
    int ret =
        GapProcessHciEventInTask((TaskFunc)GapOnUserPasskeyNotificationEvent, eventParam, sizeof(*eventParam), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvAuthenticatedPayloadTimeoutExpiredEvent(
    const HciAuthenticatedPayloadTimeoutExpiredEventParam *eventParam)
{
    HILOGI("handle: 0x%{public}04x", eventParam->connectionHandle);
    int ret = GapProcessHciEventInTask(
        (TaskFunc)GapOnAuthenticatedPayloadTimeoutExpiredEvent, eventParam, sizeof(*eventParam), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}
#endif

#ifdef GAP_LE_SUPPORT

static void GapRecvLeAdvSetParamComplete(const HciLeSetAdvertisingParametersReturnParam *param)
{
    HILOGI("status: 0x%{public}02x", param->status);
    int ret = GapProcessHciEventInTask((TaskFunc)GapLeAdvSetParamComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvLeAdvReadTxPowerComplete(const HciLeReadAdvertisingChannelTxPowerReturnParam *param)
{
    HILOGI("status: 0x%{public}02x", param->status);
    int ret = GapProcessHciEventInTask((TaskFunc)GapLeAdvReadTxPowerComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvLeAdvSetDataComplete(const HciLeSetAdvertisingDataReturnParam *param)
{
    HILOGI("status: 0x%{public}02x", param->status);
    int ret = GapProcessHciEventInTask((TaskFunc)GapLeAdvSetDataComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvLeAdvSetScanRspDataComplete(const HciLeSetScanResponseDataReturnParam *param)
{
    HILOGI("status: 0x%{public}02x", param->status);
    int ret = GapProcessHciEventInTask((TaskFunc)GapLeAdvSetScanRspDataComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvLeAdvSetEnableComplete(const HciLeSetAdvertisingEnableReturnParam *param)
{
    HILOGI("status: 0x%{public}02x", param->status);
    int ret = GapProcessHciEventInTask((TaskFunc)GapLeAdvSetEnableComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvLeScanSetParamComplete(const HciLeSetExtendedScanParametersReturnParam *param)
{
    HILOGI("status: 0x%{public}02x", param->status);
    int ret = GapProcessHciEventInTask((TaskFunc)GapLeScanSetParamComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvLeScanSetEnableComplete(const HciLeSetScanEnableReturnParam *param)
{
    HILOGI("status: 0x%{public}02x", param->status);
    int ret = GapProcessHciEventInTask((TaskFunc)GapLeScanSetEnableComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvLeSetHostChannelClassificationComplete(const HciLeSetHostChannelClassificationReturnParam *param)
{
    HILOGI("status: 0x%{public}02x", param->status);
    int ret =
        GapProcessHciEventInTask((TaskFunc)GapLeSetHostChannelClassificationComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvLeReadChannelMapComplete(const HciLeReadChannelMapReturnParam *param)
{
    HILOGI("status: 0x%{public}02x, handle: 0x%{public}04x", param->status, param->connectionHandle);
    int ret = GapProcessHciEventInTask((TaskFunc)GapLeReadChannelMapComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvLeRemoteConnectionParameterRequestReplyComplete(
    const HciLeRemoteConnectionParameterRequestReplyReturnParam *param)
{
    HILOGI("status: 0x%{public}02x, handle:0x%{public}04x", param->status, param->connectionHandle);
    int ret = GapProcessHciEventInTask(
        (TaskFunc)GapLeRemoteConnectionParameterRequestReplyComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvLeRemoteConnectionParameterRequestNegativeReplyComplete(
    const HciLeRemoteConnectionParameterRequestNegativeReplyReturnParam *param)
{
    HILOGI("status: 0x%{public}02x, handle:0x%{public}04x", param->status, param->connectionHandle);
    int ret = GapProcessHciEventInTask(
        (TaskFunc)GapLeRemoteConnectionParameterRequestNegativeReplyComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvLeSetAdvertisingSetRandomAddressComplete(const HciLeSetAdvertisingSetRandomAddressReturnParam *param)
{
    HILOGI("status: 0x%{public}02x", param->status);
    int ret =
        GapProcessHciEventInTask((TaskFunc)GapLeSetAdvertisingSetRandomAddressComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvLeSetExtendedAdvertisingParametersComplete(
    const HciLeSetExtendedAdvertisingParametersReturnParam *param)
{
    HILOGI("status: 0x%{public}02x", param->status);
    int ret =
        GapProcessHciEventInTask((TaskFunc)GapLeSetExtendedAdvertisingParametersComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvLeSetExtendedAdvertisingDataComplete(const HciLeSetExtendedAdvertisingDataReturnParam *param)
{
    HILOGI("status: 0x%{public}02x", param->status);
    int ret = GapProcessHciEventInTask((TaskFunc)GapLeSetExtendedAdvertisingDataComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvLeSetExtendedScanResponseDataComplete(const HciLeSetExtendedScanResponseDataReturnParam *param)
{
    HILOGI("status: 0x%{public}02x", param->status);
    int ret = GapProcessHciEventInTask((TaskFunc)GapLeSetExtendedScanResponseDataComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvLeSetExtendedAdvertisingEnableComplete(const HciLeSetExtendedAdvertisingEnableReturnParam *param)
{
    HILOGI("status: 0x%{public}02x", param->status);
    int ret =
        GapProcessHciEventInTask((TaskFunc)GapLeSetExtendedAdvertisingEnableComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvLeReadMaximumAdvertisingDataLengthComplete(
    const HciLeReadMaximumAdvertisingDataLengthReturnParam *param)
{
    HILOGI("status: 0x%{public}02x, len: %{public}hu", param->status, param->maximumAdvertisingDataLength);
    int ret =
        GapProcessHciEventInTask((TaskFunc)GapLeReadMaximumAdvertisingDataLengthComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvLeReadNumberofSupportedAdvertisingSetsComplete(
    const HciLeReadNumberofSupportedAdvertisingSetsReturnParam *param)
{
    HILOGI("status: 0x%{public}02x, num: %{public}hhu", param->status, param->numSupportedAdvertisingSets);
    int ret = GapProcessHciEventInTask(
        (TaskFunc)GapLeReadNumberofSupportedAdvertisingSetsComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvLeRemoveAdvertisingSetComplete(const HciLeRemoveAdvertisingSetReturnParam *param)
{
    HILOGI("status: 0x%{public}02x", param->status);
    int ret = GapProcessHciEventInTask((TaskFunc)GapLeRemoveAdvertisingSetComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvLeClearAdvertisingSetsComplete(const HciLeClearAdvertisingSetsReturnParam *param)
{
    HILOGI("status: 0x%{public}02x", param->status);
    int ret = GapProcessHciEventInTask((TaskFunc)GapLeClearAdvertisingSetsComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvLeSetExtendedScanParametersComplete(const HciLeSetExtendedScanParametersReturnParam *param)
{
    HILOGI("status: 0x%{public}02x", param->status);
    int ret = GapProcessHciEventInTask((TaskFunc)GapLeSetExtendedScanParametersComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvLeSetExtendedScanEnableComplete(const HciLeSetExtendedScanEnableReturnParam *param)
{
    HILOGI("status: 0x%{public}02x", param->status);
    int ret = GapProcessHciEventInTask((TaskFunc)GapLeSetExtendedScanEnableComplete, param, sizeof(*param), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapFreeLeAdvertisingReportEvent(void *ctx)
{
    HILOGI("enter");
    HciLeAdvertisingReportEventParam *hciParam = ctx;

    for (int i = 0; i < hciParam->numReports; i++) {
        MEM_MALLOC.free(hciParam->reports[i].data);
    }
    MEM_MALLOC.free(hciParam->reports);
}

static void GapRecvLeAdvertisingReportEvent(const HciLeAdvertisingReportEventParam *eventParam)
{
    HILOGI("num: %{public}hhu", eventParam->numReports);
    HciLeAdvertisingReportEventParam hciParam = *eventParam;
    int index;
    hciParam.reports = MEM_MALLOC.alloc(hciParam.numReports * sizeof(HciLeAdvertisingReport));
    if (hciParam.reports == NULL) {
        HILOGE("Alloc report error.");
        return;
    }

    (void)memcpy_s(hciParam.reports,
        hciParam.numReports * sizeof(HciLeAdvertisingReport),
        eventParam->reports,
        hciParam.numReports * sizeof(HciLeAdvertisingReport));
    for (index = 0; index < hciParam.numReports; index++) {
        hciParam.reports[index].data = MEM_MALLOC.alloc(hciParam.reports[index].lengthData);
        if (hciParam.reports[index].data == NULL) {
            HILOGE("Alloc report data error.");
            break;
        }
        (void)memcpy_s(hciParam.reports[index].data,
            hciParam.reports[index].lengthData,
            eventParam->reports[index].data,
            eventParam->reports[index].lengthData);
    }

    if (index < hciParam.numReports) {
        while (index-- > 0) {
            MEM_MALLOC.free(hciParam.reports[index].data);
        }
        MEM_MALLOC.free(hciParam.reports);
        return;
    }

    int ret = GapProcessHciEventInTask(
        (TaskFunc)GapOnLeAdvertisingReportEvent, &hciParam, sizeof(hciParam), GapFreeLeAdvertisingReportEvent);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvLeConnectionUpdateCompleteEvent(const HciLeConnectionUpdateCompleteEventParam *eventParam)
{
    HILOGI("status: 0x%{public}02x, handle: 0x%{public}04x", eventParam->status, eventParam->connectionHandle);
    int ret =
        GapProcessHciEventInTask((TaskFunc)GapOnLeConnectionUpdateCompleteEvent, eventParam, sizeof(*eventParam), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvLeRemoteConnectionParameterRequestEvent(
    const HciLeRemoteConnectionParameterRequestEventParam *eventParam)
{
    HILOGI("handle: 0x%{public}04x", eventParam->connectionHandle);
    int ret = GapProcessHciEventInTask(
        (TaskFunc)GapOnLeRemoteConnectionParameterRequestEvent, eventParam, sizeof(*eventParam), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvLeDirectedAdvertisingReport(const HciLeDirectedAdvertisingReportEventParam *eventParam)
{
    HILOGI("enter");
    HciLeDirectedAdvertisingReportEventParam hciParam = *eventParam;
    hciParam.reports = MEM_MALLOC.alloc(hciParam.numReports * sizeof(HciLeDirectedAdvertisingReport));
    if (hciParam.reports == NULL) {
        HILOGE("Alloc report error.");
        return;
    }

    (void)memcpy_s(hciParam.reports,
        hciParam.numReports * sizeof(HciLeDirectedAdvertisingReport),
        eventParam->reports,
        hciParam.numReports * sizeof(HciLeDirectedAdvertisingReport));
    int ret = GapProcessHciEventInTask((TaskFunc)GapOnLeDirectedAdvertisingReport, &hciParam, sizeof(hciParam), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapFreeLeExtendedAdvertisingReportEvent(void *ctx)
{
    HciLeExtendedAdvertisingReportEventParam *hciParam = ctx;

    for (int i = 0; i < hciParam->numReports; i++) {
        MEM_MALLOC.free(hciParam->reports[i].data);
    }
    MEM_MALLOC.free(hciParam->reports);
}

static void GapRecvLeExtendedAdvertisingReportEvent(const HciLeExtendedAdvertisingReportEventParam *eventParam)
{
    HILOGI("num: %{public}hhu", eventParam->numReports);
    HciLeExtendedAdvertisingReportEventParam hciParam = *eventParam;
    int i;
    hciParam.reports = MEM_MALLOC.alloc(hciParam.numReports * sizeof(HciLeExtendedAdvertisingReport));
    if (hciParam.reports == NULL) {
        HILOGE("Alloc report error.");
        return;
    }

    (void)memcpy_s(hciParam.reports,
        hciParam.numReports * sizeof(HciLeExtendedAdvertisingReport),
        eventParam->reports,
        hciParam.numReports * sizeof(HciLeExtendedAdvertisingReport));
    for (i = 0; i < hciParam.numReports; i++) {
        hciParam.reports[i].data = MEM_MALLOC.alloc(hciParam.reports[i].dataLength);
        if (hciParam.reports[i].data == NULL) {
            HILOGE("Alloc report data error.");
            break;
        }
        (void)memcpy_s(hciParam.reports[i].data,
            hciParam.reports[i].dataLength,
            eventParam->reports[i].data,
            eventParam->reports[i].dataLength);
    }

    if (i < hciParam.numReports) {
        while (i-- > 0) {
            MEM_MALLOC.free(hciParam.reports[i].data);
        }
        MEM_MALLOC.free(hciParam.reports);
        return;
    }

    int ret = GapProcessHciEventInTask((TaskFunc)GapOnLeExtendedAdvertisingReportEvent,
        &hciParam,
        sizeof(hciParam),
        GapFreeLeExtendedAdvertisingReportEvent);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvLeScanTimeoutEvent(void)
{
    HILOGI("enter");
    int ret = GapProcessHciEventInTask((TaskFunc)GapOnLeScanTimeoutEvent, NULL, 0, NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvLeAdvertisingSetTerminated(const HciLeAdvertisingSetTerminatedEventParam *eventParam)
{
    HILOGI("advHandle: %{public}hhu, status: 0x%{public}02x", eventParam->advertisingHandle, eventParam->status);
    int ret =
        GapProcessHciEventInTask((TaskFunc)GapOnLeAdvertisingSetTerminated, eventParam, sizeof(*eventParam), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

static void GapRecvLeScanRequestReceivedEvent(const HciLeScanRequestReceivedEventParam *eventParam)
{
    HILOGI("addr:" BT_ADDR_FMT "advHandle: %{public}hhu",
        BT_ADDR_FMT_OUTPUT(eventParam->scannerAddress.raw),
        eventParam->advertisingHandle);
    int ret =
        GapProcessHciEventInTask((TaskFunc)GapOnLeScanRequestReceivedEvent, eventParam, sizeof(*eventParam), NULL);
    if (ret != BT_NO_ERROR) {
        HILOGE("Task error: %{public}d.", ret);
    }
}

#endif

static HciEventCallbacks g_hciEventCallbacks = {
#ifdef GAP_BREDR_SUPPORT
    .inquiryCancelComplete = GapRecvInquiryCancelComplete,
    .linkKeyRequestReplyComplete = GapRecvLinkKeyRequestReplyComplete,
    .linkKeyRequestNegativeReplyComplete = GapRecvLinkKeyRequestNegativeReplyComplete,
    .pinCodeRequestReplyComplete = GapRecvPINCodeRequestReplyComplete,
    .pinCodeRequestNegativeReplyComplete = GapRecvPINCodeRequestNegativeReplyComplete,
    .remoteNameRequestCancelComplete = GapRecvGetRemoteNameCancelComplete,
    .ioCapabilityRequestReplyComplete = GapRecvIOCapabilityRequestReplyComplete,
    .userConfirmationRequestReplyComplete = GapRecvUserConfirmationRequestReplyComplete,
    .userConfirmationRequestNegativeReplyComplete = GapRecvUserConfirmationRequestNegativeReplyComplete,
    .userPasskeyRequestReplyComplete = GapRecvUserPasskeyRequestReplyComplete,
    .userPasskeyRequestNegativeReplyComplete = GapRecvUserPasskeyRequestNegativeReplyComplete,
    .remoteOOBDataRequestReplyComplete = GapRecvRemoteOOBDataRequestReplyComplete,
    .remoteOOBDataRequestNegativeReplyComplete = GapRecvRemoteOOBDataRequestNegativeReplyComplete,
    .iOCapabilityRequestNegativeReplyComplete = GapRecvIOCapabilityRequestNegativeReplyComplete,
    .remoteOOBExtendedDataRequestReplyComplete = GapRecvRemoteOOBExtendedDataRequestReplyComplete,

    .writeScanEnableComplete = GapRecvWriteScanEnableComplete,
    .writePageScanActivityComplete = GapRecvWritePageScanActivityComplete,
    .writeInquiryScanActivityComplete = GapRecvWriteInquiryScanActivityComplete,
    .writeClassofDeviceComplete = GapRecvWriteClassOfDeviceComplete,
    .writeCurrentIacLapComplete = GapRecvWriteCurrentIACLAPComplete,
    .writeInquiryScanTypeComplete = GapRecvWriteInquiryScanTypeComplete,
    .writePageScanTypeComplete = GapRecvWritePageScanTypeComplete,
    .writeExtendedInquiryResponseComplete = GapRecvSetExtendedInquiryResponseComplete,
    .writeAuthenticatedPayloadTimeoutComplete = GapRecvWriteAuthenticatedPayloadTimeoutComplete,
    .readLocalOOBDataComplete = GapRecvReadLocalOobDataComplete,
    .readLocalOOBExtendedDataComplete = GapRecvReadLocalOobExtendedDataComplete,

    .inquiryComplete = GapRecvInquiryComplete,
    .inquiryResult = GapRecvInquiryResult,
    .authenticationComplete = GapRecvAuthenticationComplete,
    .remoteNameRequestComplete = GapRecvGetRemoteNameComplete,
    .encryptionChange = GapRecvEncryptionChangeEvent,
    .pinCodeRequest = GapRecvPINCodeRequestEvent,
    .linkKeyRequest = GapRecvLinkKeyRequestEvent,
    .linkKeyNotification = GapRecvLinkKeyNotificationEvent,
    .inquiryResultWithRSSI = GapRecvInquiryResultRssi,
    .extendedInquiryResult = GapRecvEntendedInquiryResult,
    .encryptionKeyRefreshComplete = GapRecvEncryptionKeyRefreshComplete,
    .ioCapabilityRequest = GapRecvIOCapabilityRequestEvent,
    .ioCapabilityResponse = GapRecvIOCapabilityResponseEvent,
    .userConfirmationRequest = GapRecvUserConfirmationRequestEvent,
    .userPasskeyRequest = GapRecvUserPasskeyRequestEvent,
    .remoteOOBDataRequest = GapRecvRemoteOOBDataRequestEvent,
    .simplePairingComplete = GapRecvSimplePairingComplete,
    .userPasskeyNotification = GapRecvUserPasskeyNotificationEvent,
    .authenticatedPayloadTimeoutExpired = GapRecvAuthenticatedPayloadTimeoutExpiredEvent,
#endif

#ifdef GAP_LE_SUPPORT
    .leSetAdvertisingParametersComplete = GapRecvLeAdvSetParamComplete,
    .leReadAdvertisingChannelTxPowerComplete = GapRecvLeAdvReadTxPowerComplete,
    .leSetAdvertisingDataComplete = GapRecvLeAdvSetDataComplete,
    .leSetScanResponseDataComplete = GapRecvLeAdvSetScanRspDataComplete,
    .leSetAdvertisingEnableComplete = GapRecvLeAdvSetEnableComplete,
    .leSetScanParametersComplete = GapRecvLeScanSetParamComplete,
    .leSetScanEnableComplete = GapRecvLeScanSetEnableComplete,
    .leSetHostChannelClassificationComplete = GapRecvLeSetHostChannelClassificationComplete,
    .leReadChannelMapComplete = GapRecvLeReadChannelMapComplete,
    .leRemoteConnectionParameterRequestReplyComplete = GapRecvLeRemoteConnectionParameterRequestReplyComplete,
    .leRemoteConnectionParameterRequestNegativeReplyComplete =
        GapRecvLeRemoteConnectionParameterRequestNegativeReplyComplete,
    .leSetAdvertisingSetRandomAddressComplete = GapRecvLeSetAdvertisingSetRandomAddressComplete,
    .leSetExtendedAdvertisingParametersComplete = GapRecvLeSetExtendedAdvertisingParametersComplete,
    .leSetExtendedAdvertisingDataComplete = GapRecvLeSetExtendedAdvertisingDataComplete,
    .leSetExtendedScanResponseDataComplete = GapRecvLeSetExtendedScanResponseDataComplete,
    .leSetExtendedAdvertisingEnableComplete = GapRecvLeSetExtendedAdvertisingEnableComplete,
    .leReadMaximumAdvertisingDataLengthComplete = GapRecvLeReadMaximumAdvertisingDataLengthComplete,
    .leReadNumberofSupportedAdvertisingSetsComplete = GapRecvLeReadNumberofSupportedAdvertisingSetsComplete,
    .leRemoveAdvertisingSetComplete = GapRecvLeRemoveAdvertisingSetComplete,
    .leClearAdvertisingSetsComplete = GapRecvLeClearAdvertisingSetsComplete,
    .leSetExtendedScanParametersComplete = GapRecvLeSetExtendedScanParametersComplete,
    .leSetExtendedScanEnableComplete = GapRecvLeSetExtendedScanEnableComplete,

    .leAdvertisingReport = GapRecvLeAdvertisingReportEvent,
    .leConnectionUpdateComplete = GapRecvLeConnectionUpdateCompleteEvent,
    .leRemoteConnectionParameterRequest = GapRecvLeRemoteConnectionParameterRequestEvent,
    .leDirectedAdvertisingReport = GapRecvLeDirectedAdvertisingReport,
    .leExtendedAdvertisingReport = GapRecvLeExtendedAdvertisingReportEvent,
    .leScanTimeoutComplete = GapRecvLeScanTimeoutEvent,
    .leAdvertisingSetTerminated = GapRecvLeAdvertisingSetTerminated,
    .leScanRequestReceived = GapRecvLeScanRequestReceivedEvent,
#endif
};

void GapRegisterHciEventCallbacks(void)
{
    HILOGI("enter");
    HCI_RegisterEventCallbacks(&g_hciEventCallbacks);
}

void GapDeregisterHciEventCallbacks(void)
{
    HILOGI("enter");
    HCI_DeregisterEventCallbacks(&g_hciEventCallbacks);
}
