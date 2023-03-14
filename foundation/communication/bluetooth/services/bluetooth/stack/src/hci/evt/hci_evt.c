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

#include "hci_evt.h"

#include <securec.h>

#include "btstack.h"
#include "platform/include/allocator.h"
#include "platform/include/list.h"
#include "platform/include/mutex.h"

#include "hci/acl/hci_acl.h"
#include "hci/cmd/hci_cmd.h"
#include "hci/hci.h"
#include "hci/hci_error.h"

#include "hci_evt_cmd_complete.h"
#include "hci_evt_le.h"

#define COD_SIZE 3

#pragma pack(1)
typedef struct {
    uint8_t eventCode;
    uint8_t parameterTotalLength;
} HciEventHeader;
#pragma pack()

typedef void (*HciEventFunc)(Packet *packet);

static List *g_eventCallbackList = NULL;
static Mutex *g_lockCallbackList = NULL;

void HciInitEvent()
{
    g_eventCallbackList = ListCreate(NULL);
    g_lockCallbackList = MutexCreate();
}

void HciCloseEvent()
{
    if (g_lockCallbackList != NULL) {
        MutexDelete(g_lockCallbackList);
        g_lockCallbackList = NULL;
    }

    if (g_eventCallbackList != NULL) {
        ListDelete(g_eventCallbackList);
        g_eventCallbackList = NULL;
    }
}

static void HciEventOnInquiryCompleteEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }
    HciInquiryCompleteEventParam *param = (HciInquiryCompleteEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->inquiryComplete != NULL) {
        callbacks->inquiryComplete(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnInquiryResultEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }
    uint8_t *param = BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    int offset = 0;
    HciInquiryResultEventParam eventParam = {
        .numResponses = param[0],
    };
    offset += sizeof(uint8_t);

    if ((sizeof(HciInquiryResult) * eventParam.numResponses + 1) != BufferGetSize(payloadBuffer)) {
        return;
    }

    HciInquiryResult *responses = NULL;
    if (eventParam.numResponses) {
        responses = MEM_MALLOC.alloc(sizeof(HciInquiryResult) * eventParam.numResponses);
        if (responses != NULL) {
            for (uint8_t i = 0; i < eventParam.numResponses; i++) {
                (void)memcpy_s(responses[i].bdAddr.raw, BT_ADDRESS_SIZE, param + offset, sizeof(HciBdAddr));
                offset += sizeof(HciBdAddr);

                responses[i].pageScanRepetitionMode = param[offset];
                offset += sizeof(uint8_t);

                responses[i].reserved1 = param[offset];
                offset += sizeof(uint8_t);

                responses[i].reserved2 = param[offset];
                offset += sizeof(uint8_t);

                (void)memcpy_s(responses[i].classOfDevice, COD_SIZE, param + offset, COD_SIZE);
                offset += COD_SIZE;

                (void)memcpy_s(&responses[i].clockOffset, sizeof(uint16_t), param + offset, sizeof(uint16_t));
                offset += sizeof(uint16_t);
            }
        }

        eventParam.responses = responses;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->inquiryResult != NULL) {
        callbacks->inquiryResult(&eventParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;

    if (responses != NULL) {
        MEM_MALLOC.free(responses);
    }
}

static void HciEventOnCommandStatusEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    HciCommandStatusEventParam *param = (HciCommandStatusEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    size_t length = BufferGetSize(payloadBuffer);
    if (length != sizeof(HciCommandStatusEventParam)) {
        return;
    }

    HciSetNumberOfHciCmd(param->numHciCommandPackets);
    HciCmdOnCommandStatus(param->commandOpcode, param->status);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->commandStatus != NULL) {
        callbacks->commandStatus(param->status, param->commandOpcode);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnConnectionCompleteEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }
    HciConnectionCompleteEventParam *param = (HciConnectionCompleteEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }
    size_t length = BufferGetSize(payloadBuffer);
    if (length != sizeof(HciConnectionCompleteEventParam)) {
        return;
    }

    if (param->status == HCI_SUCCESS) {
        HciAclOnConnectionComplete(param->connectionHandle, TRANSPORT_BREDR);
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->connectionComplete != NULL) {
        callbacks->connectionComplete(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnConnectionRequestEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }
    HciConnectionRequestEventParam *param = (HciConnectionRequestEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }
    size_t length = BufferGetSize(payloadBuffer);
    if (length != sizeof(HciConnectionRequestEventParam)) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->connectionRequest != NULL) {
        callbacks->connectionRequest(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnRemoteNameRequestCompleteEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }
    HciRemoteNameRequestCompleteEventParam *param = (HciRemoteNameRequestCompleteEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }
    size_t length = BufferGetSize(payloadBuffer);
    if (length != sizeof(HciRemoteNameRequestCompleteEventParam)) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->remoteNameRequestComplete != NULL) {
        callbacks->remoteNameRequestComplete(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnEncryptionChangeEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }
    HciEncryptionChangeEventParam *param = (HciEncryptionChangeEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }
    size_t length = BufferGetSize(payloadBuffer);
    if (length != sizeof(HciEncryptionChangeEventParam)) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->encryptionChange != NULL) {
        callbacks->encryptionChange(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadRemoteSupportedFeaturesCompleteEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }
    HciReadRemoteSupportedFeaturesCompleteEventParam *param =
        (HciReadRemoteSupportedFeaturesCompleteEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }
    size_t length = BufferGetSize(payloadBuffer);
    if (length != sizeof(HciReadRemoteSupportedFeaturesCompleteEventParam)) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readRemoteSupportedFeaturesComplete != NULL) {
        callbacks->readRemoteSupportedFeaturesComplete(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadRemoteVersionInformationCompleteEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }
    HciReadRemoteVersionInformationCompleteEventParam *param =
        (HciReadRemoteVersionInformationCompleteEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }
    size_t length = BufferGetSize(payloadBuffer);
    if (length != sizeof(HciReadRemoteVersionInformationCompleteEventParam)) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readRemoteVersionInformationComplete != NULL) {
        callbacks->readRemoteVersionInformationComplete(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnRemoteHostSupportedFeaturesNotificationEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }
    HciRemoteHostSupportedFeaturesNotificationEventParam *param =
        (HciRemoteHostSupportedFeaturesNotificationEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }
    size_t length = BufferGetSize(payloadBuffer);
    if (length != sizeof(HciRemoteHostSupportedFeaturesNotificationEventParam)) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->remoteHostSupportedFeaturesNotification != NULL) {
        callbacks->remoteHostSupportedFeaturesNotification(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnNumberOfCompletedPacketsEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }
    size_t length = BufferGetSize(payloadBuffer);
    if (length < 1) {
        return;
    }
    uint8_t *buf = BufferPtr(payloadBuffer);
    if (buf == NULL) {
        return;
    }
    int offset = 0;
    uint8_t numberOfHandles = buf[0];
    offset += sizeof(uint8_t);

    if (numberOfHandles == 0) {
        return;
    }
    if ((sizeof(uint8_t) + numberOfHandles * (sizeof(uint16_t) + sizeof(uint16_t))) != length) {
        return;
    }

    HciNumberOfCompletedPackets *list = MEM_MALLOC.alloc(sizeof(HciNumberOfCompletedPackets) * numberOfHandles);
    if (list != NULL) {
        for (int i = 0; i < numberOfHandles; i++) {
            (void)memcpy_s(&list[i].connectionHandle, sizeof(uint16_t), buf + offset, sizeof(uint16_t));
            offset += sizeof(uint16_t);

            (void)memcpy_s(&list[i].numOfCompletedPackets, sizeof(uint16_t), buf + offset, sizeof(uint16_t));
            offset += sizeof(uint16_t);
        }

        HciAclOnNumberOfCompletedPacket(numberOfHandles, list);
        MEM_MALLOC.free(list);
    }
}

static void HciEventOnModeChangeEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }
    HciModeChangeEventParam *param = (HciModeChangeEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }
    size_t length = BufferGetSize(payloadBuffer);
    if (length != sizeof(HciModeChangeEventParam)) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->modeChange != NULL) {
        callbacks->modeChange(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnPINCodeRequestEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    HciPinCodeRequestEventParam *param = (HciPinCodeRequestEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }
    size_t length = BufferGetSize(payloadBuffer);
    if (length != sizeof(HciPinCodeRequestEventParam)) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->pinCodeRequest != NULL) {
        callbacks->pinCodeRequest(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLinkKeyRequestEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }
    HciLinkKeyRequestEventParam *param = (HciLinkKeyRequestEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }
    size_t length = BufferGetSize(payloadBuffer);
    if (length != sizeof(HciLinkKeyRequestEventParam)) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->linkKeyRequest != NULL) {
        callbacks->linkKeyRequest(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLinkKeyNotificationEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }
    HciLinkKeyNotificationEventParam *param = (HciLinkKeyNotificationEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }
    size_t length = BufferGetSize(payloadBuffer);
    if (length != sizeof(HciLinkKeyNotificationEventParam)) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->linkKeyNotification != NULL) {
        callbacks->linkKeyNotification(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnSynchronousConnectionCompleteEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }
    HciSynchronousConnectionCompleteEventParam *param =
        (HciSynchronousConnectionCompleteEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }
    size_t length = BufferGetSize(payloadBuffer);
    if (length != sizeof(HciSynchronousConnectionCompleteEventParam)) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->synchronousConnectionComplete != NULL) {
        callbacks->synchronousConnectionComplete(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnSynchronousConnectionChangedEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }
    HciSynchronousConnectionChangedEventParam *param =
        (HciSynchronousConnectionChangedEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }
    size_t length = BufferGetSize(payloadBuffer);
    if (length != sizeof(HciSynchronousConnectionChangedEventParam)) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->synchronousConnectionChanged != NULL) {
        callbacks->synchronousConnectionChanged(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnDisconnectCompeteEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }
    HciDisconnectCompleteEventParam *param = (HciDisconnectCompleteEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }
    size_t length = BufferGetSize(payloadBuffer);
    if (length != sizeof(HciDisconnectCompleteEventParam)) {
        return;
    }

    if (param->status == HCI_SUCCESS) {
        HciAclOnDisconnectComplete(param->connectionHandle);
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->disconnectComplete != NULL) {
        callbacks->disconnectComplete(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnAuthencationCompleteEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }
    HciAuthenticationCompleteEventParam *param = (HciAuthenticationCompleteEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }
    size_t length = BufferGetSize(payloadBuffer);
    if (length != sizeof(HciAuthenticationCompleteEventParam)) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->authenticationComplete != NULL) {
        callbacks->authenticationComplete(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnInquiryResultWithRSSIEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }
    uint8_t *param = BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    int offset = 0;
    HciInquiryResultWithRssiEventParam eventParam = {
        .numResponses = param[0],
    };
    offset += sizeof(uint8_t);

    if ((sizeof(HciInquiryResultWithRssi) * eventParam.numResponses + 1) != BufferGetSize(payloadBuffer)) {
        return;
    }

    HciInquiryResultWithRssi *responses = NULL;
    if (eventParam.numResponses > 0) {
        responses = MEM_MALLOC.alloc(sizeof(HciInquiryResultWithRssi) * eventParam.numResponses);
        if (responses != NULL) {
            for (uint8_t i = 0; i < eventParam.numResponses; i++) {
                (void)memcpy_s(responses[i].bdAddr.raw, BT_ADDRESS_SIZE, param + offset, sizeof(HciBdAddr));
                offset += sizeof(HciBdAddr);

                responses[i].pageScanRepetitionMode = param[offset];
                offset += sizeof(uint8_t);

                responses[i].reserved = param[offset];
                offset += sizeof(uint8_t);

                (void)memcpy_s(responses[i].classOfDevice, COD_SIZE, param + offset, COD_SIZE);
                offset += COD_SIZE;

                (void)memcpy_s(&responses[i].clockOffset, sizeof(uint16_t), param + offset, sizeof(uint16_t));
                offset += sizeof(uint16_t);

                responses[i].rssi = param[offset];
                offset += sizeof(uint8_t);
            }
        }

        eventParam.responses = responses;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->inquiryResultWithRSSI != NULL) {
        callbacks->inquiryResultWithRSSI(&eventParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;

    if (responses != NULL) {
        MEM_MALLOC.free(responses);
    }
}

static void HciEventOnReadRemoteExtendedFeaturesCompleteEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }
    HciReadRemoteExtendedFeaturesCompleteEventParam *param =
        (HciReadRemoteExtendedFeaturesCompleteEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }
    size_t length = BufferGetSize(payloadBuffer);
    if (length != sizeof(HciReadRemoteExtendedFeaturesCompleteEventParam)) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readRemoteExtendedFeaturesComplete != NULL) {
        callbacks->readRemoteExtendedFeaturesComplete(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnExtendedInquiryResultEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciExtendedInquiryResultEventParam)) {
        return;
    }

    HciExtendedInquiryResultEventParam *param = (HciExtendedInquiryResultEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->extendedInquiryResult != NULL) {
        callbacks->extendedInquiryResult(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnEncryptionKeyRefreshCompleteEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciEncryptionKeyRefreshCompleteEventParam)) {
        return;
    }

    HciEncryptionKeyRefreshCompleteEventParam *param =
        (HciEncryptionKeyRefreshCompleteEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->encryptionKeyRefreshComplete != NULL) {
        callbacks->encryptionKeyRefreshComplete(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnIOCapabilityRequestEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciIoCapabilityRequestEventParam)) {
        return;
    }

    HciIoCapabilityRequestEventParam *param = (HciIoCapabilityRequestEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->ioCapabilityRequest != NULL) {
        callbacks->ioCapabilityRequest(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnIOCapabilityResponseEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciIoCapabilityResponseEventParam)) {
        return;
    }

    HciIoCapabilityResponseEventParam *param = (HciIoCapabilityResponseEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->ioCapabilityResponse != NULL) {
        callbacks->ioCapabilityResponse(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnUserConfirmationRequestEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciUserConfirmationRequestEventParam)) {
        return;
    }

    HciUserConfirmationRequestEventParam *param = (HciUserConfirmationRequestEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->userConfirmationRequest != NULL) {
        callbacks->userConfirmationRequest(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnUserPasskeyRequestEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciUserPasskeyRequestEventParam)) {
        return;
    }

    HciUserPasskeyRequestEventParam *param = (HciUserPasskeyRequestEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->userPasskeyRequest != NULL) {
        callbacks->userPasskeyRequest(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnRemoteOOBDataRequestEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciRemoteOobDataRequestEventParam)) {
        return;
    }

    HciRemoteOobDataRequestEventParam *param = (HciRemoteOobDataRequestEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->remoteOOBDataRequest != NULL) {
        callbacks->remoteOOBDataRequest(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnSimplePairingCompleteEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciSimplePairingCompleteEventParam)) {
        return;
    }

    HciSimplePairingCompleteEventParam *param = (HciSimplePairingCompleteEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->simplePairingComplete != NULL) {
        callbacks->simplePairingComplete(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnUserPasskeyNotificationEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciUserPasskeyNotificationEventParam)) {
        return;
    }

    HciUserPasskeyNotificationEventParam *param = (HciUserPasskeyNotificationEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->userPasskeyNotification != NULL) {
        callbacks->userPasskeyNotification(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnKeypressNotificationEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciKeypressNotificationEventParam)) {
        return;
    }

    HciKeypressNotificationEventParam *param = (HciKeypressNotificationEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->keypressNotification != NULL) {
        callbacks->keypressNotification(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnAuthenticatedPayloadTimeoutExpiredEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }
    HciAuthenticatedPayloadTimeoutExpiredEventParam *param =
        (HciAuthenticatedPayloadTimeoutExpiredEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }
    size_t length = BufferGetSize(payloadBuffer);
    if (length != sizeof(HciAuthenticatedPayloadTimeoutExpiredEventParam)) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->authenticatedPayloadTimeoutExpired != NULL) {
        callbacks->authenticatedPayloadTimeoutExpired(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnChangeConnectionLinkKeyCompleteEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciChangeConnectionLinkKeyCompleteEventParam)) {
        return;
    }

    HciChangeConnectionLinkKeyCompleteEventParam *param =
        (HciChangeConnectionLinkKeyCompleteEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->changeConnectionLinkKeyComplete != NULL) {
        callbacks->changeConnectionLinkKeyComplete(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnMasterLinkKeyCompleteEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciMasterLinkKeyCompleteEventParam)) {
        return;
    }

    HciMasterLinkKeyCompleteEventParam *param = (HciMasterLinkKeyCompleteEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->masterLinkKeyComplete != NULL) {
        callbacks->masterLinkKeyComplete(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnQoSSetupCompleteEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciQosSetupCompleteEventParam)) {
        return;
    }

    HciQosSetupCompleteEventParam *param = (HciQosSetupCompleteEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->qosSetupComplete != NULL) {
        callbacks->qosSetupComplete(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnHardwareErrorEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciHardwareErrorEventParam)) {
        return;
    }

    HciHardwareErrorEventParam *param = (HciHardwareErrorEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->hardwareError != NULL) {
        callbacks->hardwareError(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnFlushOccurredEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciFlushOccurredEventParam)) {
        return;
    }

    HciFlushOccurredEventParam *param = (HciFlushOccurredEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->flushOccurred != NULL) {
        callbacks->flushOccurred(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnRoleChangeEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciRoleChangeEventParam)) {
        return;
    }

    HciRoleChangeEventParam *param = (HciRoleChangeEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->roleChange != NULL) {
        callbacks->roleChange(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReturnLinkKeysEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciReturnLinkKeysEventParam)) {
        return;
    }

    HciReturnLinkKeysEventParam *param = (HciReturnLinkKeysEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->returnLinkKeys != NULL) {
        callbacks->returnLinkKeys(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnDataBufferOverflowEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciDataBufferOverflowEventParam)) {
        return;
    }

    HciDataBufferOverflowEventParam *param = (HciDataBufferOverflowEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->dataBufferOverflow != NULL) {
        callbacks->dataBufferOverflow(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnMaxSlotsChangeEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciMaxSlotsChangeEventParam)) {
        return;
    }

    HciMaxSlotsChangeEventParam *param = (HciMaxSlotsChangeEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->maxSlotsChange != NULL) {
        callbacks->maxSlotsChange(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadClockOffsetCompleteEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciReadClockOffsetCompleteEventParam)) {
        return;
    }

    HciReadClockOffsetCompleteEventParam *param = (HciReadClockOffsetCompleteEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readClockOffsetComplete != NULL) {
        callbacks->readClockOffsetComplete(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnConnectionPacketTypeChangedEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciConnectionPacketTypeChangedEventParam)) {
        return;
    }

    HciConnectionPacketTypeChangedEventParam *param =
        (HciConnectionPacketTypeChangedEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->connectionPacketTypeChanged != NULL) {
        callbacks->connectionPacketTypeChanged(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnQoSViolationEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciQosViolationEventParam)) {
        return;
    }

    HciQosViolationEventParam *param = (HciQosViolationEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->qoSViolation != NULL) {
        callbacks->qoSViolation(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnPageScanRepetitionModeChangeEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciPageScanRepetitionModeChangeEventParam)) {
        return;
    }

    HciPageScanRepetitionModeChangeEventParam *param =
        (HciPageScanRepetitionModeChangeEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->pageScanRepetitionModeChange != NULL) {
        callbacks->pageScanRepetitionModeChange(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnFlowSpecificationCompleteEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciFlowSpecificationCompleteEventParam)) {
        return;
    }

    HciFlowSpecificationCompleteEventParam *param = (HciFlowSpecificationCompleteEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->flowSpecificationComplete != NULL) {
        callbacks->flowSpecificationComplete(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnSniffSubratingEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciSniffSubratingEventParam)) {
        return;
    }

    HciSniffSubratingEventParam *param = (HciSniffSubratingEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->sniffSubrating != NULL) {
        callbacks->sniffSubrating(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLinkSupervisionTimeoutChangedEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciLinkSupervisionTimeoutChangedEventParam)) {
        return;
    }

    HciLinkSupervisionTimeoutChangedEventParam *param =
        (HciLinkSupervisionTimeoutChangedEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->linkSupervisionTimeoutChanged != NULL) {
        callbacks->linkSupervisionTimeoutChanged(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnEnhancedFlushCompleteEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciEnhancedFlushCompleteEventParam)) {
        return;
    }

    HciEnhancedFlushCompleteEventParam *param = (HciEnhancedFlushCompleteEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->enhancedFlushComplete != NULL) {
        callbacks->enhancedFlushComplete(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnPhysicalLinkCompleteEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciPhysicalLinkCompleteEventParam)) {
        return;
    }

    HciPhysicalLinkCompleteEventParam *param = (HciPhysicalLinkCompleteEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->physicalLinkComplete != NULL) {
        callbacks->physicalLinkComplete(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnChannelSelectedEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciChannelSelectedEventParam)) {
        return;
    }

    HciChannelSelectedEventParam *param = (HciChannelSelectedEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->channelSelected != NULL) {
        callbacks->channelSelected(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnDisconnectionPhysicalLinkCompleteEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciDisconnectionPhysicalLinkCompleteEventParam)) {
        return;
    }

    HciDisconnectionPhysicalLinkCompleteEventParam *param =
        (HciDisconnectionPhysicalLinkCompleteEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->disconnectionPhysicalLinkComplete != NULL) {
        callbacks->disconnectionPhysicalLinkComplete(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnPhysicalLinkLossEarlyWarningEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciPhysicalLinkLossEarlyWarningEventParam)) {
        return;
    }

    HciPhysicalLinkLossEarlyWarningEventParam *param =
        (HciPhysicalLinkLossEarlyWarningEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->physicalLinkLossEarlyWarning != NULL) {
        callbacks->physicalLinkLossEarlyWarning(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnPhysicalLinkRecoveryEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciPhysicalLinkRecoveryEventParam)) {
        return;
    }

    HciPhysicalLinkRecoveryEventParam *param = (HciPhysicalLinkRecoveryEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->physicalLinkRecovery != NULL) {
        callbacks->physicalLinkRecovery(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLogicalLinkCompleteEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciLogicalLinkCompleteEventParam)) {
        return;
    }

    HciLogicalLinkCompleteEventParam *param = (HciLogicalLinkCompleteEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->logicalLinkComplete != NULL) {
        callbacks->logicalLinkComplete(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnDisconnectionLogicalLinkCompleteEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciDisconnectionLogicalLinkCompleteEventParam)) {
        return;
    }

    HciDisconnectionLogicalLinkCompleteEventParam *param =
        (HciDisconnectionLogicalLinkCompleteEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->disconnectionLogicalLinkComplete != NULL) {
        callbacks->disconnectionLogicalLinkComplete(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnFlowSpecModifyCompleteEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciFlowSpecModifyCompleteEventParam)) {
        return;
    }

    HciFlowSpecModifyCompleteEventParam *param = (HciFlowSpecModifyCompleteEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->flowSpecModifyComplete != NULL) {
        callbacks->flowSpecModifyComplete(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnNumberOfCompletedDataBlocksEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciNumberOfCompletedDataBlocksEventParam)) {
        return;
    }

    HciNumberOfCompletedDataBlocksEventParam *param =
        (HciNumberOfCompletedDataBlocksEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->numberOfCompletedDataBlocks != NULL) {
        callbacks->numberOfCompletedDataBlocks(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnShortRangeModeChangeCompleteEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciShortRangeModeChangeCompleteEventParam)) {
        return;
    }

    HciShortRangeModeChangeCompleteEventParam *param =
        (HciShortRangeModeChangeCompleteEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->shortRangeModeChangeComplete != NULL) {
        callbacks->shortRangeModeChangeComplete(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnAMPStatusChangeEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciAmpStatusChangeEventParam)) {
        return;
    }

    HciAmpStatusChangeEventParam *param = (HciAmpStatusChangeEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->ampStatusChange != NULL) {
        callbacks->ampStatusChange(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnAMPStartTestEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciAmpStartTestEventParam)) {
        return;
    }

    HciAmpStartTestEventParam *param = (HciAmpStartTestEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->ampStartTest != NULL) {
        callbacks->ampStartTest(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnAMPTestEndEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciAmpTestEndEventParam)) {
        return;
    }

    HciAmpTestEndEventParam *param = (HciAmpTestEndEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->ampTestEnd != NULL) {
        callbacks->ampTestEnd(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnAMPReceiverReportEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciAmpReceiverReportEventParam)) {
        return;
    }

    HciAmpReceiverReportEventParam *param = (HciAmpReceiverReportEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->ampReceiverReport != NULL) {
        callbacks->ampReceiverReport(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnTriggeredClockCaptureEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciTriggeredClockCaptureEventParam)) {
        return;
    }

    HciTriggeredClockCaptureEventParam *param = (HciTriggeredClockCaptureEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->triggeredClockCapture != NULL) {
        callbacks->triggeredClockCapture(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnSynchronizationTrainCompleteEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciSynchronizationTrainCompleteEventParam)) {
        return;
    }

    HciSynchronizationTrainCompleteEventParam *param =
        (HciSynchronizationTrainCompleteEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->synchronizationTrainComplete != NULL) {
        callbacks->synchronizationTrainComplete(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnSynchronizationTrainReceivedEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciSynchronizationTrainReceivedEventParam)) {
        return;
    }

    HciSynchronizationTrainReceivedEventParam *param =
        (HciSynchronizationTrainReceivedEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->synchronizationTrainReceived != NULL) {
        callbacks->synchronizationTrainReceived(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnConnectionlessSlaveBroadcastReceiveEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciConnectionlessSlaveBroadcastReceiveEventParam)) {
        return;
    }

    HciConnectionlessSlaveBroadcastReceiveEventParam *param =
        (HciConnectionlessSlaveBroadcastReceiveEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->connectionlessSlaveBroadcastReceive != NULL) {
        callbacks->connectionlessSlaveBroadcastReceive(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnConnectionlessSlaveBroadcastTimeoutEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciConnectionlessSlaveBroadcastTimeoutEventParam)) {
        return;
    }

    HciConnectionlessSlaveBroadcastTimeoutEventParam *param =
        (HciConnectionlessSlaveBroadcastTimeoutEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->connectionlessSlaveBroadcastTimeout != NULL) {
        callbacks->connectionlessSlaveBroadcastTimeout(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnTruncatedPageCompleteEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciTruncatedPageCompleteEventParam)) {
        return;
    }

    HciTruncatedPageCompleteEventParam *param = (HciTruncatedPageCompleteEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->truncatedPageComplete != NULL) {
        callbacks->truncatedPageComplete(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnConnectionlessSlaveBroadcastChannelMapChangeEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciConnectionlessSlaveBroadcastChannelMapChangeEventParam)) {
        return;
    }

    HciConnectionlessSlaveBroadcastChannelMapChangeEventParam *param =
        (HciConnectionlessSlaveBroadcastChannelMapChangeEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->connectionlessSlaveBroadcastChannelMapChange != NULL) {
        callbacks->connectionlessSlaveBroadcastChannelMapChange(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnInquiryResponseNotificationEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciInquiryResponseNotificationEventParam)) {
        return;
    }

    HciInquiryResponseNotificationEventParam *param =
        (HciInquiryResponseNotificationEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->inquiryResponseNotification != NULL) {
        callbacks->inquiryResponseNotification(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnSAMStatusChangeEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) != sizeof(HciSamStatusChangeEventParam)) {
        return;
    }

    HciSamStatusChangeEventParam *param = (HciSamStatusChangeEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->samStatusChange != NULL) {
        callbacks->samStatusChange(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static HciEventFunc g_eventFuncMap[] = {
    NULL,                                                         // 0x00
    HciEventOnInquiryCompleteEvent,                               // 0x01
    HciEventOnInquiryResultEvent,                                 // 0x02
    HciEventOnConnectionCompleteEvent,                            // 0x03
    HciEventOnConnectionRequestEvent,                             // 0x04
    HciEventOnDisconnectCompeteEvent,                             // 0x05
    HciEventOnAuthencationCompleteEvent,                          // 0x06
    HciEventOnRemoteNameRequestCompleteEvent,                     // 0x07
    HciEventOnEncryptionChangeEvent,                              // 0x08
    HciEventOnChangeConnectionLinkKeyCompleteEvent,               // 0x09
    HciEventOnMasterLinkKeyCompleteEvent,                         // 0x0A
    HciEventOnReadRemoteSupportedFeaturesCompleteEvent,           // 0x0B
    HciEventOnReadRemoteVersionInformationCompleteEvent,          // 0x0C
    HciEventOnQoSSetupCompleteEvent,                              // 0x0D
    HciEventOnCommandCompleteEvent,                               // 0x0E
    HciEventOnCommandStatusEvent,                                 // 0x0F
    HciEventOnHardwareErrorEvent,                                 // 0x10
    HciEventOnFlushOccurredEvent,                                 // 0x11
    HciEventOnRoleChangeEvent,                                    // 0x12
    HciEventOnNumberOfCompletedPacketsEvent,                      // 0x13
    HciEventOnModeChangeEvent,                                    // 0x14
    HciEventOnReturnLinkKeysEvent,                                // 0x15
    HciEventOnPINCodeRequestEvent,                                // 0x16
    HciEventOnLinkKeyRequestEvent,                                // 0x17
    HciEventOnLinkKeyNotificationEvent,                           // 0x18
    HciEventOnReturnLinkKeysEvent,                                // 0x19
    HciEventOnDataBufferOverflowEvent,                            // 0x1A
    HciEventOnMaxSlotsChangeEvent,                                // 0x1B
    HciEventOnReadClockOffsetCompleteEvent,                       // 0x1C
    HciEventOnConnectionPacketTypeChangedEvent,                   // 0x1D
    HciEventOnQoSViolationEvent,                                  // 0x1E
    NULL,                                                         // 0x1F
    HciEventOnPageScanRepetitionModeChangeEvent,                  // 0x20
    HciEventOnFlowSpecificationCompleteEvent,                     // 0x21
    HciEventOnInquiryResultWithRSSIEvent,                         // 0x22
    HciEventOnReadRemoteExtendedFeaturesCompleteEvent,            // 0x23
    NULL,                                                         // 0x24
    NULL,                                                         // 0x25
    NULL,                                                         // 0x26
    NULL,                                                         // 0x27
    NULL,                                                         // 0x28
    NULL,                                                         // 0x29
    NULL,                                                         // 0x2A
    NULL,                                                         // 0x2B
    HciEventOnSynchronousConnectionCompleteEvent,                 // 0x2C
    HciEventOnSynchronousConnectionChangedEvent,                  // 0x2D
    HciEventOnSniffSubratingEvent,                                // 0x2E
    HciEventOnExtendedInquiryResultEvent,                         // 0x2F
    HciEventOnEncryptionKeyRefreshCompleteEvent,                  // 0x30
    HciEventOnIOCapabilityRequestEvent,                           // 0x31
    HciEventOnIOCapabilityResponseEvent,                          // 0x32
    HciEventOnUserConfirmationRequestEvent,                       // 0x33
    HciEventOnUserPasskeyRequestEvent,                            // 0x34
    HciEventOnRemoteOOBDataRequestEvent,                          // 0x35
    HciEventOnSimplePairingCompleteEvent,                         // 0x36
    NULL,                                                         // 0x37
    HciEventOnLinkSupervisionTimeoutChangedEvent,                 // 0x38
    HciEventOnEnhancedFlushCompleteEvent,                         // 0x39
    NULL,                                                         // 0x3A
    HciEventOnUserPasskeyNotificationEvent,                       // 0x3B
    HciEventOnKeypressNotificationEvent,                          // 0x3C
    HciEventOnRemoteHostSupportedFeaturesNotificationEvent,       // 0x3D
    HciEventOnLeMetaEvent,                                        // 0x3E
    NULL,                                                         // 0x3F
    HciEventOnPhysicalLinkCompleteEvent,                          // 0x40
    HciEventOnChannelSelectedEvent,                               // 0x41
    HciEventOnDisconnectionPhysicalLinkCompleteEvent,             // 0x42
    HciEventOnPhysicalLinkLossEarlyWarningEvent,                  // 0x43
    HciEventOnPhysicalLinkRecoveryEvent,                          // 0x44
    HciEventOnLogicalLinkCompleteEvent,                           // 0x45
    HciEventOnDisconnectionLogicalLinkCompleteEvent,              // 0x46
    HciEventOnFlowSpecModifyCompleteEvent,                        // 0x47
    HciEventOnNumberOfCompletedDataBlocksEvent,                   // 0x48
    HciEventOnAMPStartTestEvent,                                  // 0x49
    HciEventOnAMPTestEndEvent,                                    // 0x4A
    HciEventOnAMPReceiverReportEvent,                             // 0x4B
    HciEventOnShortRangeModeChangeCompleteEvent,                  // 0x4C
    HciEventOnAMPStatusChangeEvent,                               // 0x4D
    HciEventOnTriggeredClockCaptureEvent,                         // 0x4E
    HciEventOnSynchronizationTrainCompleteEvent,                  // 0x4F
    HciEventOnSynchronizationTrainReceivedEvent,                  // 0x50
    HciEventOnConnectionlessSlaveBroadcastReceiveEvent,           // 0x51
    HciEventOnConnectionlessSlaveBroadcastTimeoutEvent,           // 0x52
    HciEventOnTruncatedPageCompleteEvent,                         // 0x53
    HciEventOnConnectionlessSlaveBroadcastTimeoutEvent,           // 0x54
    HciEventOnConnectionlessSlaveBroadcastChannelMapChangeEvent,  // 0x55
    HciEventOnInquiryResponseNotificationEvent,                   // 0x56
    HciEventOnAuthenticatedPayloadTimeoutExpiredEvent,            // 0x57
    HciEventOnSAMStatusChangeEvent,                               // 0x58
};

#define EVENTCODE_MAX 0x58

void HciOnEvent(Packet *packet)
{
    HciEventHeader header;
    PacketExtractHead(packet, (uint8_t *)&header, sizeof(header));
    size_t payloadLength = PacketPayloadSize(packet);
    if (payloadLength == header.parameterTotalLength) {
        if (header.eventCode > EVENTCODE_MAX) {
            return;
        }

        HciEventFunc func = g_eventFuncMap[header.eventCode];
        if (func != NULL) {
            func(packet);
        }
    }
}

int HCI_RegisterEventCallbacks(const HciEventCallbacks *callbacks)
{
    MutexLock(g_lockCallbackList);

    ListAddLast(g_eventCallbackList, (void *)callbacks);

    MutexUnlock(g_lockCallbackList);
    return BT_NO_ERROR;
}

int HCI_DeregisterEventCallbacks(const HciEventCallbacks *callbacks)
{
    MutexLock(g_lockCallbackList);

    ListRemoveNode(g_eventCallbackList, (void *)callbacks);

    MutexUnlock(g_lockCallbackList);
    return BT_NO_ERROR;
}

List *HciGetEventCallbackList()
{
    return g_eventCallbackList;
}

Mutex *HciGetEventCallbackListLock()
{
    return g_lockCallbackList;
}
