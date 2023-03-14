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

#include "hci_evt_link_ctrl_cmd_complete.h"

#include <securec.h>

#include "platform/include/allocator.h"
#include "platform/include/list.h"
#include "platform/include/mutex.h"

#include "hci/cmd/hci_cmd.h"
#include "hci/hci.h"
#include "hci/hci_def.h"

#include "hci_evt.h"
#include "hci_evt_cmd_complete.h"

static void HciEventOnInquiryCancelComplete(const void *param, uint8_t length)
{
    HciInquiryCancelReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->inquiryCancelComplete != NULL) {
        callbacks->inquiryCancelComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnCreateConnectionCancelComplete(const void *param, uint8_t length)
{
    HciCreateConnectionCancelReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->createConnectionCancelComplete != NULL) {
        callbacks->createConnectionCancelComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLinkKeyRequestReplyComplete(const void *param, uint8_t length)
{
    HciLinkKeyRequestReplyReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->linkKeyRequestReplyComplete != NULL) {
        callbacks->linkKeyRequestReplyComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLinkKeyRequestNegativeReplyComplete(const void *param, uint8_t length)
{
    HciLinkKeyRequestNegativeReplyReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->linkKeyRequestNegativeReplyComplete != NULL) {
        callbacks->linkKeyRequestNegativeReplyComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnPINCodeRequestReplyCompete(const void *param, uint8_t length)
{
    HciPinCodeRequestReplyReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->pinCodeRequestReplyComplete != NULL) {
        callbacks->pinCodeRequestReplyComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnPINCodeRequestNegativeReplyComplete(const void *param, uint8_t length)
{
    HciPinCodeRequestNegativeReplyReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->pinCodeRequestNegativeReplyComplete != NULL) {
        callbacks->pinCodeRequestNegativeReplyComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnRemoteNameRequestCancelComplete(const void *param, uint8_t length)
{
    HciRemoteNameRequestCancelReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->remoteNameRequestCancelComplete != NULL) {
        callbacks->remoteNameRequestCancelComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnIOCapabilityRequestReplyComplete(const void *param, uint8_t length)
{
    HciIOCapabilityRequestReplyReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->ioCapabilityRequestReplyComplete != NULL) {
        callbacks->ioCapabilityRequestReplyComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnUserConfirmationRequestReplyComplete(const void *param, uint8_t length)
{
    HciUserConfirmationRequestReplyReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->userConfirmationRequestReplyComplete != NULL) {
        callbacks->userConfirmationRequestReplyComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnUserConfirmationRequestNegativeReplyComplete(const void *param, uint8_t length)
{
    HciUserConfirmationRequestNegativeReplyReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->userConfirmationRequestNegativeReplyComplete != NULL) {
        callbacks->userConfirmationRequestNegativeReplyComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnUserPasskeyRequestReplyComplete(const void *param, uint8_t length)
{
    HciUserPasskeyRequestReplyReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->userPasskeyRequestReplyComplete != NULL) {
        callbacks->userPasskeyRequestReplyComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnUserPasskeyRequestNegativeReplyComplete(const void *param, uint8_t length)
{
    HciUserPasskeyRequestNegativeReplyReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->userPasskeyRequestNegativeReplyComplete != NULL) {
        callbacks->userPasskeyRequestNegativeReplyComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnRemoteOOBDataRequestReplyComplete(const void *param, uint8_t length)
{
    HciRemoteOobDataRequestReplyReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->remoteOOBDataRequestReplyComplete != NULL) {
        callbacks->remoteOOBDataRequestReplyComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnRemoteOOBDataRequestNegativeReplyComplete(const void *param, uint8_t length)
{
    HciRemoteOobDataRequestNegativeReplyReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->remoteOOBDataRequestNegativeReplyComplete != NULL) {
        callbacks->remoteOOBDataRequestNegativeReplyComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnIOCapabilityRequestNegativeReplyComplete(const void *param, uint8_t length)
{
    HciIoCapabilityRequestNegativeReplyReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->iOCapabilityRequestNegativeReplyComplete != NULL) {
        callbacks->iOCapabilityRequestNegativeReplyComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnRemoteOOBExtendedDataRequestReplyComplete(const void *param, uint8_t length)
{
    HciRemoteOobExtendedDataRequestReplyReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->remoteOOBExtendedDataRequestReplyComplete != NULL) {
        callbacks->remoteOOBExtendedDataRequestReplyComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnExitPeriodicInquiryModeComplete(const void *param, uint8_t length)
{
    HciExitPeriodicInquiryModeReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->exitPeriodicInquiryModeComplete != NULL) {
        callbacks->exitPeriodicInquiryModeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnPeriodicInquiryModeComplete(const void *param, uint8_t length)
{
    HciPeriodicInquiryModeReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->periodicInquiryModeComplete != NULL) {
        callbacks->periodicInquiryModeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadLMPHandleComplete(const void *param, uint8_t length)
{
    HciReadLmpHandleReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readLmpHandleComplete != NULL) {
        callbacks->readLmpHandleComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLogicalLinkCancelComplete(const void *param, uint8_t length)
{
    HciLogicalLinkCancelReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->logicalLinkCancelComplete != NULL) {
        callbacks->logicalLinkCancelComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnTruncatedPageCancelComplete(const void *param, uint8_t length)
{
    HciTruncatedPageCancelReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->truncatedPageCancelComplete != NULL) {
        callbacks->truncatedPageCancelComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnSetConnectionlessSlaveBroadcastComplete(const void *param, uint8_t length)
{
    HciSetConnectionlessSlaveBroadcastReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->setConnectionlessSlaveBroadcastComplete != NULL) {
        callbacks->setConnectionlessSlaveBroadcastComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnSetConnectionlessSlaveBroadcastReceiveComplete(const void *param, uint8_t length)
{
    HciSetConnectionlessSlaveBroadcastReceiveReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->setConnectionlessSlaveBroadcastReceiveComplete != NULL) {
        callbacks->setConnectionlessSlaveBroadcastReceiveComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static HciEventCommandCompleteFunc g_linkControlCommandCompleteMap[] = {
    NULL,                                                      // 0x0000
    NULL,                                                      // 0x0001
    HciEventOnInquiryCancelComplete,                           // 0x0002
    HciEventOnPeriodicInquiryModeComplete,                     // 0x0003
    HciEventOnExitPeriodicInquiryModeComplete,                 // 0x0004
    NULL,                                                      // 0x0005
    NULL,                                                      // 0x0006
    NULL,                                                      // 0x0007
    HciEventOnCreateConnectionCancelComplete,                  // 0x0008
    NULL,                                                      // 0x0009
    NULL,                                                      // 0x000A
    HciEventOnLinkKeyRequestReplyComplete,                     // 0x000B
    HciEventOnLinkKeyRequestNegativeReplyComplete,             // 0x000C
    HciEventOnPINCodeRequestReplyCompete,                      // 0x000D
    HciEventOnPINCodeRequestNegativeReplyComplete,             // 0x000E
    NULL,                                                      // 0x000F
    NULL,                                                      // 0x0010
    NULL,                                                      // 0x0011
    NULL,                                                      // 0x0012
    NULL,                                                      // 0x0013
    NULL,                                                      // 0x0014
    NULL,                                                      // 0x0015
    NULL,                                                      // 0x0016
    NULL,                                                      // 0x0017
    NULL,                                                      // 0x0018
    NULL,                                                      // 0x0019
    HciEventOnRemoteNameRequestCancelComplete,                 // 0x001A
    NULL,                                                      // 0x001B
    NULL,                                                      // 0x001C
    NULL,                                                      // 0x001D
    NULL,                                                      // 0x001E
    NULL,                                                      // 0x001F
    HciEventOnReadLMPHandleComplete,                           // 0x0020
    NULL,                                                      // 0x0021
    NULL,                                                      // 0x0022
    NULL,                                                      // 0x0023
    NULL,                                                      // 0x0024
    NULL,                                                      // 0x0025
    NULL,                                                      // 0x0026
    NULL,                                                      // 0x0027
    NULL,                                                      // 0x0028
    NULL,                                                      // 0x0029
    NULL,                                                      // 0x002A
    HciEventOnIOCapabilityRequestReplyComplete,                // 0x002B
    HciEventOnUserConfirmationRequestReplyComplete,            // 0x002C
    HciEventOnUserConfirmationRequestNegativeReplyComplete,    // 0x002D
    HciEventOnUserPasskeyRequestReplyComplete,                 // 0x002E
    HciEventOnUserPasskeyRequestNegativeReplyComplete,         // 0x002F
    HciEventOnRemoteOOBDataRequestReplyComplete,               // 0x0030
    NULL,                                                      // 0x0031
    NULL,                                                      // 0x0032
    HciEventOnRemoteOOBDataRequestNegativeReplyComplete,       // 0x0033
    HciEventOnIOCapabilityRequestNegativeReplyComplete,        // 0x0034
    NULL,                                                      // 0x0035
    NULL,                                                      // 0x0036
    NULL,                                                      // 0x0037
    NULL,                                                      // 0x0038
    NULL,                                                      // 0x0039
    NULL,                                                      // 0x003A
    HciEventOnLogicalLinkCancelComplete,                       // 0x003B
    NULL,                                                      // 0x003C
    NULL,                                                      // 0x003D
    NULL,                                                      // 0x003E
    NULL,                                                      // 0x003F
    HciEventOnTruncatedPageCancelComplete,                     // 0x0040
    HciEventOnSetConnectionlessSlaveBroadcastComplete,         // 0x0041
    HciEventOnSetConnectionlessSlaveBroadcastReceiveComplete,  // 0x0042
    NULL,                                                      // 0x0043
    NULL,                                                      // 0x0044
    HciEventOnRemoteOOBExtendedDataRequestReplyComplete,       // 0x0045
};

#define LINKCONTROL_OCF_MAX 0x0045

void HciEventOnLinkControlCommandComplete(uint16_t opCode, const void *param, uint8_t length)
{
    uint16_t ogf = GET_OCF(opCode);
    if (ogf > LINKCONTROL_OCF_MAX) {
        return;
    }

    HciEventCommandCompleteFunc func = g_linkControlCommandCompleteMap[ogf];
    if (func != NULL) {
        func(param, length);
    }
}