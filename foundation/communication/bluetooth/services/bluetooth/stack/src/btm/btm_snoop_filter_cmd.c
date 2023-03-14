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

typedef struct {
    BtmFilterFunc cmdFunc;
    uint16_t cmdDataOffset;
    BtmFilterFunc completeFunc;
    uint16_t completeEvtDataOffset;
} HciCommandFilterFunc;

#pragma pack(1)
typedef struct {
    uint16_t opCode;
    uint8_t parameterTotalLength;
} BtmHciCmdHeader;
typedef struct {
    uint8_t eventCode;
    uint8_t parameterTotalLength;
} HciEventHeader;
#pragma pack()

static void HciCommonFilterAddress(uint8_t *param)
{
    BtmFilterAddress(param, BT_ADDRESS_SIZE);
}

static const HciCommandFilterFunc G_LINK_CONTROL_FILTER_FUNC_MAP[HCI_LINK_CONTROL_COMMAND_MAX_NUM + 1] = {
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {HciCommonFilterAddress, offsetof(HciCreateConnectionParam, bdAddr), NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {
        HciCommonFilterAddress,
        offsetof(HciCreateConnectionCancelParam, bdAddr),
        HciCommonFilterAddress,
        offsetof(HciCreateConnectionCancelReturnParam, bdAddr),
    },
    {HciCommonFilterAddress, offsetof(HciAcceptConnectionReqestParam, bdAddr), NULL, 0},
    {HciCommonFilterAddress, offsetof(HciRejectConnectionRequestParam, bdAddr), NULL, 0},
    {
        HciCommonFilterAddress,
        offsetof(HciLinkKeyRequestReplyParam, bdAddr),
        HciCommonFilterAddress,
        offsetof(HciLinkKeyRequestReplyReturnParam, bdAddr),
    },
    {
        HciCommonFilterAddress,
        offsetof(HciLinkKeyRequestNegativeReplyParam, bdAddr),
        HciCommonFilterAddress,
        offsetof(HciLinkKeyRequestNegativeReplyReturnParam, bdAddr),
    },
    {
        HciCommonFilterAddress,
        offsetof(HciPinCodeRequestReplyParam, bdAddr),
        HciCommonFilterAddress,
        offsetof(HciPinCodeRequestReplyReturnParam, bdAddr),
    },
    {
        HciCommonFilterAddress,
        offsetof(HciPinCodeRequestNegativeReplyParam, bdAddr),
        HciCommonFilterAddress,
        offsetof(HciPinCodeRequestNegativeReplyReturnParam, bdAddr),
    },
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {HciCommonFilterAddress, offsetof(HciRemoteNameRequestParam, addr), NULL, 0},
    {
        HciCommonFilterAddress,
        offsetof(HciRemoteNameRequestCancelParam, addr),
        HciCommonFilterAddress,
        offsetof(HciRemoteNameRequestCancelReturnParam, addr),
    },
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {HciCommonFilterAddress, offsetof(HciAcceptSynchronousConnectionRequestParam, addr), NULL, 0},
    {HciCommonFilterAddress, offsetof(HciRejectSynchronousConnectionRequestParam, bdAddr), NULL, 0},
    {
        HciCommonFilterAddress,
        offsetof(HciIOCapabilityRequestReplyParam, bdAddr),
        HciCommonFilterAddress,
        offsetof(HciIOCapabilityRequestReplyReturnParam, bdAddr),
    },
    {
        HciCommonFilterAddress,
        offsetof(HciUserConfirmationRequestReplyParam, bdAddr),
        HciCommonFilterAddress,
        offsetof(HciUserConfirmationRequestReplyReturnParam, bdAddr),
    },
    {
        HciCommonFilterAddress,
        offsetof(HciUserConfirmationRequestNegativeReplyParam, bdAddr),
        HciCommonFilterAddress,
        offsetof(HciUserConfirmationRequestNegativeReplyReturnParam, bdAddr),
    },
    {
        HciCommonFilterAddress,
        offsetof(HciUserPasskeyRequestReplyParam, bdAddr),
        HciCommonFilterAddress,
        offsetof(HciUserPasskeyRequestReplyReturnParam, bdAddr),
    },
    {
        HciCommonFilterAddress,
        offsetof(HciUserPasskeyRequestNegativeReplyParam, bdAddr),
        HciCommonFilterAddress,
        offsetof(HciUserPasskeyRequestNegativeReplyReturnParam, bdAddr),
    },
    {
        HciCommonFilterAddress,
        offsetof(HciRemoteOobDataRequestReplyParam, bdAddr),
        HciCommonFilterAddress,
        offsetof(HciRemoteOobDataRequestReplyReturnParam, bdAddr),
    },
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {
        HciCommonFilterAddress,
        offsetof(HciRemoteOobDataRequestNegativeReplyParam, bdAddr),
        HciCommonFilterAddress,
        offsetof(HciRemoteOobDataRequestNegativeReplyReturnParam, bdAddr),
    },
    {
        HciCommonFilterAddress,
        offsetof(HciIoCapabilityRequestNegativeReplyParam, bdAddr),
        HciCommonFilterAddress,
        offsetof(HciIoCapabilityRequestNegativeReplyReturnParam, bdAddr),
    },
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {HciCommonFilterAddress, offsetof(HciEnhancedAcceptSynchronousConnectionRequestParam, bdAddr), NULL, 0},
    {HciCommonFilterAddress, offsetof(HciTruncatedPageParam, bdAddr), NULL, 0},
    {
        HciCommonFilterAddress,
        offsetof(HciTruncatedPageCancelParam, bdAddr),
        HciCommonFilterAddress,
        offsetof(HciTruncatedPageCancelReturnParam, bdAddr),
    },
    {NULL, 0, NULL, 0},
    {
        HciCommonFilterAddress,
        offsetof(HciSetConnectionlessSlaveBroadcastReceiveParam, bdAddr),
        HciCommonFilterAddress,
        offsetof(HciSetConnectionlessSlaveBroadcastReceiveReturnParam, bdAddr),
    },
    {NULL, 0, NULL, 0},
    {HciCommonFilterAddress, offsetof(HciReceiveSynchronizationTrainParam, bdAddr), NULL, 0},
    {
        HciCommonFilterAddress,
        offsetof(HciRemoteOobExtendedDataRequestReplyParam, bdAddr),
        HciCommonFilterAddress,
        offsetof(HciRemoteOobExtendedDataRequestReplyReturnParam, bdAddr),
    },
};

static const HciCommandFilterFunc G_LINK_POLICY_FILTER_FUNC_MAP[HCI_LINK_POLICY_COMMAND_MAX_NUM + 1] = {
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {HciCommonFilterAddress, offsetof(HciSwitchRoleParam, bdAddr), NULL, 0},
};

static void HciWriteStoredLinkKeyCmdFilter(uint8_t *param)
{
    uint16_t offset = 0;
    uint8_t *numKeysToWrite = param + offset;
    offset += sizeof(uint8_t);

    for (int ii = 0; ii < *numKeysToWrite; ii++) {
        BtmFilterAddress(param + offset, BT_ADDRESS_SIZE);
        offset += sizeof(HciBdAddr) + LINK_KEY_SIZE;
    }
}

static const HciCommandFilterFunc
    G_CONTROLLER_AND_BASEBAND_FILTER_FUNC_MAP[HCI_CONTTOLLER_AND_BASEBAND_COMMAND_MAX_NUM + 1] = {
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {HciCommonFilterAddress, offsetof(HciReadStoredLinkKeyParam, bdAddr), NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {HciWriteStoredLinkKeyCmdFilter, 0, NULL, 0},
        {HciCommonFilterAddress, offsetof(HciDeleteStoredLinkKeyParam, bdAddr), NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {
            HciCommonFilterAddress,
            offsetof(HciSendKeypressNotificationParam, bdAddr),
            HciCommonFilterAddress,
            offsetof(HciSendKeypressNotificationReturnParam, bdAddr),
        },
};

static const HciCommandFilterFunc
    G_INFORMATION_PARAMETER_FILTER_FUNC_MAP[HCI_INFORMATIONAL_PARAMETERS_COMMAND_MAX_NUM + 1] = {
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, NULL, 0},
        {NULL, 0, HciCommonFilterAddress, offsetof(HciReadBdAddrReturnParam, bdAddr)},
};

static const HciCommandFilterFunc G_LE_CONTROLLER_FILTER_FUNC_MAP[HCI_LE_CONTROLLER_COMMAND_MAX_NUM + 1] = {
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {HciCommonFilterAddress, offsetof(HciLeSetAdvertisingParametersParam, peerAddress), NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {HciCommonFilterAddress, offsetof(HciLeCreateConnectionParam, peerAddress), NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {HciCommonFilterAddress, offsetof(HciLeAddDeviceToWhiteListParam, address), NULL, 0},
    {HciCommonFilterAddress, offsetof(HciLeRemoveDeviceFromWhiteListParam, address), NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {HciCommonFilterAddress, offsetof(HciLeAddDeviceToResolvingListParam, peerIdentityAddress), NULL, 0},
    {HciCommonFilterAddress, offsetof(HciLeRemoveDeviceFromResolvingListParam, peerIdentityAddress), NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {
        HciCommonFilterAddress,
        offsetof(HciLeReadPeerResolvableAddressParam, peerIdentityAddress),
        HciCommonFilterAddress,
        offsetof(HciLeReadPeerResolvableAddressReturnParam, peerResolvableAddress),
    },
    {
        HciCommonFilterAddress,
        offsetof(HciLeReadLocalResolvableAddressParam, peerIdentityAddress),
        HciCommonFilterAddress,
        offsetof(HciLeReadLocalResolvableAddressReturnParam, localResolvableAddress),
    },
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {HciCommonFilterAddress, offsetof(HciLeExtendedCreateConnectionParam, peerAddress), NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {NULL, 0, NULL, 0},
    {HciCommonFilterAddress, offsetof(HciLeSetPrivacyModeParam, peerIdentityAddress), NULL, 0},
};

void BtmFilterHciCmd(const uint8_t **data, uint16_t originalLength, uint16_t *includedLength)
{
    if (originalLength < sizeof(BtmHciCmdHeader)) {
        return;
    }

    uint16_t offset = 0;
    BtmHciCmdHeader *header = (BtmHciCmdHeader *)(*data + offset);
    offset += sizeof(BtmHciCmdHeader);
    const HciCommandFilterFunc *filterFuncInfo = NULL;

    switch (GET_OGF(header->opCode)) {
        case HCI_COMMAND_OGF_LINK_CONTROL:
            filterFuncInfo = &G_LINK_CONTROL_FILTER_FUNC_MAP[GET_OCF(header->opCode)];
            break;
        case HCI_COMMAND_OGF_LINK_POLICY:
            filterFuncInfo = &G_LINK_POLICY_FILTER_FUNC_MAP[GET_OCF(header->opCode)];
            break;
        case HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND:
            filterFuncInfo = &G_CONTROLLER_AND_BASEBAND_FILTER_FUNC_MAP[GET_OCF(header->opCode)];
            break;
        case HCI_COMMAND_OGF_INFORMATIONAL_PARAMETERS:
            filterFuncInfo = &G_INFORMATION_PARAMETER_FILTER_FUNC_MAP[GET_OCF(header->opCode)];
            break;
        case HCI_COMMAND_OGF_LE_CONTROLLER:
            filterFuncInfo = &G_LE_CONTROLLER_FILTER_FUNC_MAP[GET_OCF(header->opCode)];
            break;
        case HCI_COMMAND_OGF_STATUS_PARAMETERS:
        default:
            break;
    }

    if (filterFuncInfo != NULL && filterFuncInfo->cmdFunc != NULL) {
        uint8_t *copyData = BtmCreateFilterBuffer(includedLength, *data);
        filterFuncInfo->cmdFunc(copyData + offset + filterFuncInfo->cmdDataOffset);
        *data = copyData;
    }
}

void BtmFilterHciCmdCompleteEvt(const uint8_t **data, uint16_t originalLength, uint16_t *includedLength)
{
    if (originalLength < sizeof(HciEventHeader)) {
        return;
    }

    uint8_t offset = 0;
    HciEventHeader *header = (HciEventHeader *)(*data + offset);
    offset += sizeof(HciEventHeader);

    if (header->eventCode == HCI_COMMAND_COMPLETE_EVENT) {
        HciCommandCompleteEventParam *evtParam = (HciCommandCompleteEventParam *)(*data + offset);
        offset += sizeof(HciCommandCompleteEventParam);
        const HciCommandFilterFunc *filterFuncInfo = NULL;
        switch (GET_OGF(evtParam->commandOpcode)) {
            case HCI_COMMAND_OGF_LINK_CONTROL:
                filterFuncInfo = &G_LINK_CONTROL_FILTER_FUNC_MAP[GET_OCF(evtParam->commandOpcode)];
                break;
            case HCI_COMMAND_OGF_LINK_POLICY:
                filterFuncInfo = &G_LINK_POLICY_FILTER_FUNC_MAP[GET_OCF(evtParam->commandOpcode)];
                break;
            case HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND:
                filterFuncInfo = &G_CONTROLLER_AND_BASEBAND_FILTER_FUNC_MAP[GET_OCF(evtParam->commandOpcode)];
                break;
            case HCI_COMMAND_OGF_INFORMATIONAL_PARAMETERS:
                filterFuncInfo = &G_INFORMATION_PARAMETER_FILTER_FUNC_MAP[GET_OCF(evtParam->commandOpcode)];
                break;
            case HCI_COMMAND_OGF_LE_CONTROLLER:
                filterFuncInfo = &G_LE_CONTROLLER_FILTER_FUNC_MAP[GET_OCF(evtParam->commandOpcode)];
                break;
            case HCI_COMMAND_OGF_STATUS_PARAMETERS:
            default:
                break;
        }
        if (filterFuncInfo != NULL && filterFuncInfo->completeFunc != NULL) {
            uint8_t *copyData = BtmCreateFilterBuffer(includedLength, *data);
            filterFuncInfo->completeFunc(copyData + offset + filterFuncInfo->completeEvtDataOffset);
        }
    }
}
