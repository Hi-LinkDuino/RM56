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

#include <stdint.h>

#include "hci/hci.h"

#include "btm_snoop_filter.h"

#pragma pack(1)
typedef struct {
    uint8_t eventCode;
    uint8_t parameterTotalLength;
} HciEventHeader;
#pragma pack()

typedef struct {
    BtmFilterFunc evtFunc;
    uint16_t EvtDataOffset;
} HciEventFilterFunc;

static void HciCommonFilterAddress(uint8_t *param)
{
    BtmFilterAddress(param, BT_ADDRESS_SIZE);
}

static void HciLeAdvertisingReportEventFilter(uint8_t *param)
{
    uint8_t offset = 0;
    uint8_t *numReports = param + offset;
    offset += sizeof(uint8_t);

    for (int ii = 0; ii < *numReports; ii++) {
        HciLeAdvertisingReport *report = (HciLeAdvertisingReport *)(param + offset);
        offset += offsetof(HciLeAdvertisingReport, data);
        offset += report->lengthData;
        offset += sizeof(report->rssi);
        BtmFilterAddress((uint8_t *)&report->address, BT_ADDRESS_SIZE);
    }
}

static void HciLeEnhancedConnectionCompleteEventFilter(uint8_t *param)
{
    BtmFilterAddress((uint8_t *)&((HciLeEnhancedConnectionCompleteEventParam *)param)->peerAddress, BT_ADDRESS_SIZE);
    BtmFilterAddress((uint8_t *)&((HciLeEnhancedConnectionCompleteEventParam *)param)->peerResolvablePrivateAddress,
        BT_ADDRESS_SIZE);
    BtmFilterAddress((uint8_t *)&((HciLeEnhancedConnectionCompleteEventParam *)param)->localResolvablePrivateAddress,
        BT_ADDRESS_SIZE);
}

static void HciLeDirectedAdvertisingReportEventFilter(uint8_t *param)
{
    uint8_t offset = 0;
    uint8_t *numReports = param + offset;
    offset += sizeof(uint8_t);

    for (int ii = 0; ii < *numReports; ii++) {
        HciLeDirectedAdvertisingReport *report = (HciLeDirectedAdvertisingReport *)(param + offset);
        offset += sizeof(HciLeDirectedAdvertisingReport);
        BtmFilterAddress((uint8_t *)&report->address, BT_ADDRESS_SIZE);
        BtmFilterAddress((uint8_t *)&report->directAddress, BT_ADDRESS_SIZE);
    }
}

static void HciLeExtendedAdvertisingReportEventFilter(uint8_t *param)
{
    uint8_t offset = 0;
    uint8_t *numReports = param + offset;
    offset += sizeof(uint8_t);

    for (int ii = 0; ii < *numReports; ii++) {
        HciLeExtendedAdvertisingReport *report = (HciLeExtendedAdvertisingReport *)(param + offset);
        offset += offsetof(HciLeExtendedAdvertisingReport, data);
        offset += report->dataLength;
        offset += report->rssi;
        BtmFilterAddress((uint8_t *)&report->address, BT_ADDRESS_SIZE);
        BtmFilterAddress((uint8_t *)&report->directAddress, BT_ADDRESS_SIZE);
    }
}

static const HciEventFilterFunc G_LE_EVT_FILTER_FUNC_MAP[HCI_LE_EVENT_MAX_NUM + 1] = {
    {NULL, 0},
    {HciCommonFilterAddress, offsetof(HciLeConnectionCompleteEventParam, peerAddress)},
    {HciLeAdvertisingReportEventFilter, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {HciLeEnhancedConnectionCompleteEventFilter, 0},
    {HciLeDirectedAdvertisingReportEventFilter, 0},
    {NULL, 0},
    {HciLeExtendedAdvertisingReportEventFilter, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {HciCommonFilterAddress, offsetof(HciLeScanRequestReceivedEventParam, scannerAddress)},
    {NULL, 0},
};

static void HciInquiryResultEventFilter(uint8_t *param)
{
    uint8_t offset = 0;
    uint8_t *numResponses = param + offset;
    offset += sizeof(uint8_t);

    for (int ii = 0; ii < *numResponses; ii++) {
        HciInquiryResult *result = (HciInquiryResult *)(param + offset);
        offset += sizeof(HciInquiryResult);
        BtmFilterAddress((uint8_t *)&result->bdAddr, BT_ADDRESS_SIZE);
    }
}

static void HciReturnLinkKeysEventFilter(uint8_t *param)
{
    uint8_t offset = 0;
    uint8_t *numKeys = param + offset;
    offset += sizeof(uint8_t);

    for (int ii = 0; ii < *numKeys; ii++) {
        HciReturnLinkKeys *keys = (HciReturnLinkKeys *)(param + offset);
        offset += sizeof(HciReturnLinkKeys);
        BtmFilterAddress((uint8_t *)&keys->bdAddr, BT_ADDRESS_SIZE);
    }
}

static void HciInquiryResultWithRssiEventFilter(uint8_t *param)
{
    uint8_t offset = 0;
    uint8_t *numResponses = param + offset;
    offset += sizeof(uint8_t);

    for (int ii = 0; ii < *numResponses; ii++) {
        HciInquiryResultWithRssi *result = (HciInquiryResultWithRssi *)(param + offset);
        offset += sizeof(HciInquiryResultWithRssi);
        BtmFilterAddress((uint8_t *)&result->bdAddr, BT_ADDRESS_SIZE);
    }
}

static const HciEventFilterFunc G_EVT_FILTER_FUNC_MAP[HCI_EVENT_MAX_NUM + 1] = {
    {NULL, 0},
    {NULL, 0},
    {HciInquiryResultEventFilter, 0},
    {HciCommonFilterAddress, offsetof(HciConnectionCompleteEventParam, bdAddr)},
    {HciCommonFilterAddress, offsetof(HciConnectionRequestEventParam, bdAddr)},
    {NULL, 0},
    {NULL, 0},
    {HciCommonFilterAddress, offsetof(HciRemoteNameRequestCompleteEventParam, bdAddr)},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {HciCommonFilterAddress, offsetof(HciRoleChangeEventParam, bdAddr)},
    {NULL, 0},
    {NULL, 0},
    {HciReturnLinkKeysEventFilter, 0},
    {HciCommonFilterAddress, offsetof(HciPinCodeRequestEventParam, bdAddr)},
    {HciCommonFilterAddress, offsetof(HciLinkKeyRequestEventParam, bdAddr)},
    {HciCommonFilterAddress, offsetof(HciLinkKeyNotificationEventParam, bdAddr)},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {HciCommonFilterAddress, offsetof(HciPageScanRepetitionModeChangeEventParam, bdAddr)},
    {NULL, 0},
    {HciInquiryResultWithRssiEventFilter, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {HciCommonFilterAddress, offsetof(HciSynchronousConnectionCompleteEventParam, bdAddr)},
    {NULL, 0},
    {NULL, 0},
    {HciCommonFilterAddress, offsetof(HciExtendedInquiryResultEventParam, bdAddr)},
    {NULL, 0},
    {HciCommonFilterAddress, offsetof(HciIoCapabilityRequestEventParam, bdAddr)},
    {HciCommonFilterAddress, offsetof(HciIoCapabilityResponseEventParam, bdAddr)},
    {HciCommonFilterAddress, offsetof(HciUserConfirmationRequestEventParam, bdAddr)},
    {HciCommonFilterAddress, offsetof(HciUserPasskeyRequestEventParam, bdAddr)},
    {HciCommonFilterAddress, offsetof(HciRemoteOobDataRequestEventParam, bdAddr)},
    {HciCommonFilterAddress, offsetof(HciSimplePairingCompleteEventParam, bdAddr)},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {HciCommonFilterAddress, offsetof(HciUserPasskeyNotificationEventParam, bdAddr)},
    {HciCommonFilterAddress, offsetof(HciKeypressNotificationEventParam, bdAddr)},
    {HciCommonFilterAddress, offsetof(HciRemoteHostSupportedFeaturesNotificationEventParam, addr)},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {HciCommonFilterAddress, offsetof(HciSynchronizationTrainReceivedEventParam, bdAdddr)},
    {HciCommonFilterAddress, offsetof(HciConnectionlessSlaveBroadcastReceiveEventParam, bdAddr)},
    {HciCommonFilterAddress, offsetof(HciConnectionlessSlaveBroadcastTimeoutEventParam, bdAddr)},
    {HciCommonFilterAddress, offsetof(HciTruncatedPageCompleteEventParam, bdAddr)},
};

void HciEvtFilter(const uint8_t **data, uint16_t originalLength, uint16_t *includedLength)
{
    if (originalLength < sizeof(HciEventHeader)) {
        return;
    }

    BtmFilterCheckAndSaveAclConnInfo(data, originalLength);

    uint8_t offset = 0;
    HciEventHeader *header = (HciEventHeader *)(*data + offset);
    offset += sizeof(HciEventHeader);
    const HciEventFilterFunc *filterFuncInfo = NULL;

    if (header->eventCode == HCI_COMMAND_COMPLETE_EVENT) {
        BtmFilterHciCmdCompleteEvt(data, originalLength, includedLength);
    } else if (header->eventCode == HCI_LE_META_EVENT) {
        const uint8_t *subEvtCode = *data + offset;
        offset += sizeof(uint8_t);
        filterFuncInfo = &G_LE_EVT_FILTER_FUNC_MAP[*subEvtCode];
    } else {
        filterFuncInfo = &G_EVT_FILTER_FUNC_MAP[header->eventCode];
    }

    if (filterFuncInfo != NULL && filterFuncInfo->evtFunc != NULL) {
        uint8_t *copyData = BtmCreateFilterBuffer(includedLength, *data);
        filterFuncInfo->evtFunc(copyData + offset + filterFuncInfo->EvtDataOffset);
        *data = copyData;
    }
}