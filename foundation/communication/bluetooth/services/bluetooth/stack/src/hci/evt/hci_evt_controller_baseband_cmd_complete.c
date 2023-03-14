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

#include "hci_evt_controller_baseband_cmd_complete.h"

#include <securec.h>

#include "platform/include/allocator.h"
#include "platform/include/list.h"
#include "platform/include/mutex.h"

#include "hci/cmd/hci_cmd.h"
#include "hci/hci.h"
#include "hci/hci_def.h"

#include "hci_evt.h"
#include "hci_evt_cmd_complete.h"

static void HciEventOnSetEventMaskCompete(const void *param, uint8_t length)
{
    HciSetEventMaskReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->setEventMaskComplete != NULL) {
        callbacks->setEventMaskComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnResetComplete(const void *param, uint8_t length)
{
    HciResetReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->resetComplete != NULL) {
        callbacks->resetComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnWriteLocalNameComplete(const void *param, uint8_t length)
{
    HciWriteLocalNameReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeLocalNameComplete != NULL) {
        callbacks->writeLocalNameComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadLocalNameComplete(const void *param, uint8_t length)
{
    HciReadLocalNameReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readLocalNameComplete != NULL) {
        callbacks->readLocalNameComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadScanEnableComplete(const void *param, uint8_t length)
{
    HciReadScanEnableReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readScanEnableComplete != NULL) {
        callbacks->readScanEnableComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnWriteScanEnableComplete(const void *param, uint8_t length)
{
    HciWriteScanEnableReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeScanEnableComplete != NULL) {
        callbacks->writeScanEnableComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadPageScanActivityComplete(const void *param, uint8_t length)
{
    HciReadPageScanActivityReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readPageScanActivityComplete != NULL) {
        callbacks->readPageScanActivityComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnWritePageScanActivityComplete(const void *param, uint8_t length)
{
    HciWritePageScanActivityReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writePageScanActivityComplete != NULL) {
        callbacks->writePageScanActivityComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadInquiryScanActivityComplete(const void *param, uint8_t length)
{
    HciReadInquiryScanActivityReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readInquiryScanActivityComplete != NULL) {
        callbacks->readInquiryScanActivityComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnWriteInquiryScanActivityComplete(const void *param, uint8_t length)
{
    HciWriteInquiryScanActivityReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeInquiryScanActivityComplete != NULL) {
        callbacks->writeInquiryScanActivityComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadClassofDeviceComplete(const void *param, uint8_t length)
{
    HciReadClassofDeviceReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readClassofDeviceComplete != NULL) {
        callbacks->readClassofDeviceComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnWriteClassofDeviceComplete(const void *param, uint8_t length)
{
    HciWriteClassofDeviceReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeClassofDeviceComplete != NULL) {
        callbacks->writeClassofDeviceComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnWriteVoiceSettingComplete(const void *param, uint8_t length)
{
    HciWriteVoiceSettingParamReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeVoiceSettingComplete != NULL) {
        callbacks->writeVoiceSettingComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnHostBufferSizeComplete(const void *param, uint8_t length)
{
    HciHostBufferSizeReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->hostBufferSizeComplete != NULL) {
        callbacks->hostBufferSizeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadCurrentIacLapComplete(const void *param, uint8_t length)
{
    HciReadCurrentIacLapReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readCurrentIacLapComplete != NULL) {
        callbacks->readCurrentIacLapComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnWriteCurrentIacLapComplete(const void *param, uint8_t length)
{
    HciWriteCurrentIacLapReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeCurrentIacLapComplete != NULL) {
        callbacks->writeCurrentIacLapComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadInquiryScanTypeComplete(const void *param, uint8_t length)
{
    HciReadInquiryScanTypeReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readInquiryScanTypeComplete != NULL) {
        callbacks->readInquiryScanTypeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnWriteInquiryScanTypeComplete(const void *param, uint8_t length)
{
    HciWriteInquiryScanTypeReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeInquiryScanTypeComplete != NULL) {
        callbacks->writeInquiryScanTypeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadInquiryModeComplete(const void *param, uint8_t length)
{
    HciReadInquiryModeReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readInquiryModeComplete != NULL) {
        callbacks->readInquiryModeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnWriteInquiryModeComplete(const void *param, uint8_t length)
{
    HciWriteInquiryModeReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeInquiryModeComplete != NULL) {
        callbacks->writeInquiryModeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadPageScanTypeCommandComplete(const void *param, uint8_t length)
{
    HciReadPageScanTypeReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readPageScanTypeComplete != NULL) {
        callbacks->readPageScanTypeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnWritePageScanTypeCommandComplete(const void *param, uint8_t length)
{
    HciWritePageScanTypeReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writePageScanTypeComplete != NULL) {
        callbacks->writePageScanTypeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnWriteExtendedInquiryResponseComplete(const void *param, uint8_t length)
{
    HciWriteExtendedInquiryResponseReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeExtendedInquiryResponseComplete != NULL) {
        callbacks->writeExtendedInquiryResponseComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnWriteSimplePairingModeComplete(const void *param, uint8_t length)
{
    HciWriteSimplePairingModeReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeSimplePairingModeComplete != NULL) {
        callbacks->writeSimplePairingModeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadLocalOOBDataComplete(const void *param, uint8_t length)
{
    HciReadLocalOOBDataReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readLocalOOBDataComplete != NULL) {
        callbacks->readLocalOOBDataComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnSendKeypressNotificationComplete(const void *param, uint8_t length)
{
    HciSendKeypressNotificationReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->sendKeypressNotificationComplete != NULL) {
        callbacks->sendKeypressNotificationComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnWriteLeHostSupportComplete(const void *param, uint8_t length)
{
    HciWriteLeHostSupportReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeLeHostSupportComplete != NULL) {
        callbacks->writeLeHostSupportComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnWriteSecureConnectionsHostSupportComplete(const void *param, uint8_t length)
{
    HciWriteSecureConnectionsHostSupportReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeSecureConnectionsHostSupportComplete != NULL) {
        callbacks->writeSecureConnectionsHostSupportComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnWriteAuthenticatedPayloadTimeoutComplete(const void *param, uint8_t length)
{
    HciWriteAuthenticatedPayloadTimeoutReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeAuthenticatedPayloadTimeoutComplete != NULL) {
        callbacks->writeAuthenticatedPayloadTimeoutComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadLocalOOBExtendedDataComplete(const void *param, uint8_t length)
{
    HciReadLocalOobExtendedDataReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readLocalOOBExtendedDataComplete != NULL) {
        callbacks->readLocalOOBExtendedDataComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnSetEventFilterComplete(const void *param, uint8_t length)
{
    HciSetEventFilterReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->setEventFilterComplete != NULL) {
        callbacks->setEventFilterComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnFlushComplete(const void *param, uint8_t length)
{
    HciFlushReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->flushComplete != NULL) {
        callbacks->flushComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadPinTypeComplete(const void *param, uint8_t length)
{
    HciReadPinTypeReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readPinTypeComplete != NULL) {
        callbacks->readPinTypeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnWritePinTypeComplete(const void *param, uint8_t length)
{
    HciWritePinTypeReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writePinTypeComplete != NULL) {
        callbacks->writePinTypeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnCreateNewUnitKeyComplete(const void *param, uint8_t length)
{
    HciCreateNewUnitKeyReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->createNewUnitKeyComplete != NULL) {
        callbacks->createNewUnitKeyComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadStoredLinkKeyComplete(const void *param, uint8_t length)
{
    HciReadStoredLinkKeyReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readStoredLinkKeyComplete != NULL) {
        callbacks->readStoredLinkKeyComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnWriteStoredLinkKeyComplete(const void *param, uint8_t length)
{
    HciWriteStoredLinkKeyReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeStoredLinkKeyComplete != NULL) {
        callbacks->writeStoredLinkKeyComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnDeleteStoredLinkKeyComplete(const void *param, uint8_t length)
{
    HciDeleteStoredLinkKeyReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->deleteStoredLinkKeyComplete != NULL) {
        callbacks->deleteStoredLinkKeyComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadConnectionAcceptTimeoutComplete(const void *param, uint8_t length)
{
    HciReadConnectionAcceptTimeoutReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readConnectionAcceptTimeoutComplete != NULL) {
        callbacks->readConnectionAcceptTimeoutComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnWriteConnectionAcceptTimeoutComplete(const void *param, uint8_t length)
{
    HciWriteConnectionAcceptTimeoutReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeConnectionAcceptTimeoutComplete != NULL) {
        callbacks->writeConnectionAcceptTimeoutComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadPageTimeoutComplete(const void *param, uint8_t length)
{
    HciReadPageTimeoutReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readPageTimeoutComplete != NULL) {
        callbacks->readPageTimeoutComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnWritePageTimeoutComplete(const void *param, uint8_t length)
{
    HciWritePageTimeoutReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writePageTimeoutComplete != NULL) {
        callbacks->writePageTimeoutComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnWriteAuthenticationEnableComplete(const void *param, uint8_t length)
{
    HciWriteAuthenticationEnableReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeAuthenticationEnableComplete != NULL) {
        callbacks->writeAuthenticationEnableComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadVoiceSettingComplete(const void *param, uint8_t length)
{
    HciReadVoiceSettingReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readVoiceSettingComplete != NULL) {
        callbacks->readVoiceSettingComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadAutomaticFlushTimeoutComplete(const void *param, uint8_t length)
{
    HciReadAutomaticFlushTimeoutReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readAutomaticFlushTimeoutComplete != NULL) {
        callbacks->readAutomaticFlushTimeoutComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnWriteAutomaticFlushTimeoutComplete(const void *param, uint8_t length)
{
    HciWriteAutomaticFlushTimeoutReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeAutomaticFlushTimeoutComplete != NULL) {
        callbacks->writeAutomaticFlushTimeoutComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadNumBroadcastRetransmissionsComplete(const void *param, uint8_t length)
{
    HciReadNumBroadcastRetransmissionsReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readNumBroadcastRetransmissionsComplete != NULL) {
        callbacks->readNumBroadcastRetransmissionsComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnWriteNumBroadcastRetransmissionsComplete(const void *param, uint8_t length)
{
    HciWriteNumBroadcastRetransmissionsReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeNumBroadcastRetransmissionsComplete != NULL) {
        callbacks->writeNumBroadcastRetransmissionsComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadHoldModeActivityComplete(const void *param, uint8_t length)
{
    HciReadHoldModeActivityReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readHoldModeActivityComplete != NULL) {
        callbacks->readHoldModeActivityComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnWriteHoldModeActivityComplete(const void *param, uint8_t length)
{
    HciWriteHoldModeActivityReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeHoldModeActivityComplete != NULL) {
        callbacks->writeHoldModeActivityComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadTransmitPowerLevelComplete(const void *param, uint8_t length)
{
    HciReadTransmitPowerLevelReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readTransmitPowerLevelComplete != NULL) {
        callbacks->readTransmitPowerLevelComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadSynchronousFlowControlEnableComplete(const void *param, uint8_t length)
{
    HciReadSynchronousFlowControlEnableReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readSynchronousFlowControlEnableComplete != NULL) {
        callbacks->readSynchronousFlowControlEnableComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnWriteSynchronousFlowControlEnableComplete(const void *param, uint8_t length)
{
    HciWriteSynchronousFlowControlEnableReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeSynchronousFlowControlEnableComplete != NULL) {
        callbacks->writeSynchronousFlowControlEnableComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnSetControllerToHostFlowControlComplete(const void *param, uint8_t length)
{
    HciSetControllerToHostFlowControlReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->setControllerToHostFlowControlComplete != NULL) {
        callbacks->setControllerToHostFlowControlComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadLinkSupervisionTimeoutComplete(const void *param, uint8_t length)
{
    HciReadLinkSupervisionTimeoutReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readLinkSupervisionTimeoutComplete != NULL) {
        callbacks->readLinkSupervisionTimeoutComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnWriteLinkSupervisionTimeoutComplete(const void *param, uint8_t length)
{
    HciWriteLinkSupervisionTimeoutReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeLinkSupervisionTimeoutComplete != NULL) {
        callbacks->writeLinkSupervisionTimeoutComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnSetAFHHostChannelClassificationComplete(const void *param, uint8_t length)
{
    HciSetAfhHostChannelClassificationReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->setAfhHostChannelClassificationComplete != NULL) {
        callbacks->setAfhHostChannelClassificationComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadNumberOfSupportedIACComplete(const void *param, uint8_t length)
{
    HciReadNumberOfSupportedIacReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readNumberOfSupportedIacComplete != NULL) {
        callbacks->readNumberOfSupportedIacComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadAFHChannelAssessmentModeComplete(const void *param, uint8_t length)
{
    HciReadAfhChannelAssessmentModeReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readAfhChannelAssessmentModeComplete != NULL) {
        callbacks->readAfhChannelAssessmentModeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnWriteAFHChannelAssessmentModeComplete(const void *param, uint8_t length)
{
    HciWriteAfhChannelAssessmentModeReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeAfhChannelAssessmentModeComplete != NULL) {
        callbacks->writeAfhChannelAssessmentModeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadExtendedInquiryResponseComplete(const void *param, uint8_t length)
{
    HciReadExtendedInquiryResponseReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readExtendedInquiryResponseComplete != NULL) {
        callbacks->readExtendedInquiryResponseComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadSimplePairingModeComplete(const void *param, uint8_t length)
{
    HciReadSimplePairingModeReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readSimplePairingModeComplete != NULL) {
        callbacks->readSimplePairingModeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnWriteInquiryTransmitPowerLevelComplete(const void *param, uint8_t length)
{
    HciWriteInquiryTransmitPowerLevelReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeInquiryTransmitPowerLevelComplete != NULL) {
        callbacks->writeInquiryTransmitPowerLevelComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadDefaultErroneousDataReportingComplete(const void *param, uint8_t length)
{
    HciReadDefaultErroneousDataReportingReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readDefaultErroneousDataReportingComplete != NULL) {
        callbacks->readDefaultErroneousDataReportingComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnWriteDefaultErroneousDataReportingLevelComplete(const void *param, uint8_t length)
{
    HciWriteDefaultErroneousDataReportingReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeDefaultErroneousDataReportingComplete != NULL) {
        callbacks->writeDefaultErroneousDataReportingComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnWriteLogicalLinkAcceptTimeoutComplete(const void *param, uint8_t length)
{
    HciWriteLogicalLinkAcceptTimeoutReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeLogicalLinkAcceptTimeoutComplete != NULL) {
        callbacks->writeLogicalLinkAcceptTimeoutComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnSetEventMaskPage2Complete(const void *param, uint8_t length)
{
    HciSetEventMaskPage2ReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->setEventMaskPage2Complete != NULL) {
        callbacks->setEventMaskPage2Complete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadLocationDataComplete(const void *param, uint8_t length)
{
    HciReadLocationDataReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readLocationDataComplete != NULL) {
        callbacks->readLocationDataComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnWriteLocationDataComplete(const void *param, uint8_t length)
{
    HciWriteLocationDataReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeLocationDataComplete != NULL) {
        callbacks->writeLocationDataComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadFlowControlModeComplete(const void *param, uint8_t length)
{
    HciReadFlowControlModeReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readFlowControlModeComplete != NULL) {
        callbacks->readFlowControlModeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnWriteFlowControlModeComplete(const void *param, uint8_t length)
{
    HciWriteFlowControlModeReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeFlowControlModeComplete != NULL) {
        callbacks->writeFlowControlModeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadEnhancedTransmitPowerLevelComplete(const void *param, uint8_t length)
{
    HciReadEnhancedTransmitPowerLevelReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readEnhancedTransmitPowerLevelComplete != NULL) {
        callbacks->readEnhancedTransmitPowerLevelComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadBestEffortFlushTimeoutComplete(const void *param, uint8_t length)
{
    HciReadBestEffortFlushTimeoutReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readBestEffortFlushTimeoutComplete != NULL) {
        callbacks->readBestEffortFlushTimeoutComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnWriteBestEffortFlushTimeoutComplete(const void *param, uint8_t length)
{
    HciWriteBestEffortFlushTimeoutReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeBestEffortFlushTimeoutComplete != NULL) {
        callbacks->writeBestEffortFlushTimeoutComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadLEHostSupportComplete(const void *param, uint8_t length)
{
    HciReadLeHostSupportReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readLeHostSupportComplete != NULL) {
        callbacks->readLeHostSupportComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnSetMWSChannelParametersComplete(const void *param, uint8_t length)
{
    HciSetMwsChannelParametersReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->setMwsChannelParametersComplete != NULL) {
        callbacks->setMwsChannelParametersComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnSetExternalFrameConfigurationComplete(const void *param, uint8_t length)
{
    HciSetExternalFrameConfigurationReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->setExternalFrameConfigurationComplete != NULL) {
        callbacks->setExternalFrameConfigurationComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnSetMWSSignalingComplete(const void *param, uint8_t length)
{
    HciSetMwsSignalingReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->setMwsSignalingComplete != NULL) {
        callbacks->setMwsSignalingComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnSetMWSTransportLayerComplete(const void *param, uint8_t length)
{
    HciSetMwsTransportLayerReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->setMwsTransportLayerComplete != NULL) {
        callbacks->setMwsTransportLayerComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnSetMWSScanFrequencyTableComplete(const void *param, uint8_t length)
{
    HciSetMwsScanFrequencyTableReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->setMwsScanFrequencyTableComplete != NULL) {
        callbacks->setMwsScanFrequencyTableComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnSetMWSPATTERNConfigurationComplete(const void *param, uint8_t length)
{
    HciSetMwsPatternConfigurationReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->setMwsPatternConfigurationComplete != NULL) {
        callbacks->setMwsPatternConfigurationComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnSetReservedLTADDRComplete(const void *param, uint8_t length)
{
    HciSetReservedLtAddrReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->setReservedLtAddrComplete != NULL) {
        callbacks->setReservedLtAddrComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnDeleteReservedLTADDRComplete(const void *param, uint8_t length)
{
    HciDeleteReservedLtAddrReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->deleteReservedLtAddrComplete != NULL) {
        callbacks->deleteReservedLtAddrComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnSetConnectionlessSlaveBroadcastDataComplete(const void *param, uint8_t length)
{
    HciSetConnectionlessSlaveBroadcastDataReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->setConnectionlessSlaveBroadcastDataComplete != NULL) {
        callbacks->setConnectionlessSlaveBroadcastDataComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadSynchronizationTrainParametersComplete(const void *param, uint8_t length)
{
    HciReadSynchronizationTrainParametersReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readSynchronizationTrainParametersComplete != NULL) {
        callbacks->readSynchronizationTrainParametersComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnWriteSynchronizationTrainParametersComplete(const void *param, uint8_t length)
{
    HciWriteSynchronizationTrainParametersReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeSynchronizationTrainParametersComplete != NULL) {
        callbacks->writeSynchronizationTrainParametersComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadSecureConnectionsHostSupportComplete(const void *param, uint8_t length)
{
    HciReadSecureConnectionsHostSupportReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readSecureConnectionsHostSupportComplete != NULL) {
        callbacks->readSecureConnectionsHostSupportComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadAuthenticatedPayloadTimeoutComplete(const void *param, uint8_t length)
{
    HciReadAuthenticatedPayloadTimeoutReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readAuthenticatedPayloadTimeoutComplete != NULL) {
        callbacks->readAuthenticatedPayloadTimeoutComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadExtendedPageTimeoutComplete(const void *param, uint8_t length)
{
    HciReadExtendedPageTimeoutReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readExtendedPageTimeoutComplete != NULL) {
        callbacks->readExtendedPageTimeoutComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnWriteExtendedPageTimeoutComplete(const void *param, uint8_t length)
{
    HciWriteExtendedPageTimeoutReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeExtendedPageTimeoutComplete != NULL) {
        callbacks->writeExtendedPageTimeoutComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadExtendedInquiryLengthComplete(const void *param, uint8_t length)
{
    HciReadExtendedInquiryLengthReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readExtendedInquiryLengthComplete != NULL) {
        callbacks->readExtendedInquiryLengthComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnWriteExtendedInquiryLengthComplete(const void *param, uint8_t length)
{
    HciWriteExtendedInquiryLengthReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeExtendedInquiryLengthComplete != NULL) {
        callbacks->writeExtendedInquiryLengthComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static HciEventCommandCompleteFunc g_controllerBasebandCommandCompleteMap[] = {
    NULL,                                                       // 0x0000
    HciEventOnSetEventMaskCompete,                              // 0x0001
    NULL,                                                       // 0x0002
    HciEventOnResetComplete,                                    // 0x0003
    NULL,                                                       // 0x0004
    HciEventOnSetEventFilterComplete,                           // 0x0005
    NULL,                                                       // 0x0006
    NULL,                                                       // 0x0007
    HciEventOnFlushComplete,                                    // 0x0008
    HciEventOnReadPinTypeComplete,                              // 0x0009
    HciEventOnWritePinTypeComplete,                             // 0x000A
    HciEventOnCreateNewUnitKeyComplete,                         // 0x000B
    NULL,                                                       // 0x000C
    HciEventOnReadStoredLinkKeyComplete,                        // 0x000D
    NULL,                                                       // 0x000E
    NULL,                                                       // 0x000F
    NULL,                                                       // 0x0010
    HciEventOnWriteStoredLinkKeyComplete,                       // 0x0011
    HciEventOnDeleteStoredLinkKeyComplete,                      // 0x0012
    HciEventOnWriteLocalNameComplete,                           // 0x0013
    HciEventOnReadLocalNameComplete,                            // 0x0014
    HciEventOnReadConnectionAcceptTimeoutComplete,              // 0x0015
    HciEventOnWriteConnectionAcceptTimeoutComplete,             // 0x0016
    HciEventOnReadPageTimeoutComplete,                          // 0x0017
    HciEventOnWritePageTimeoutComplete,                         // 0x0018
    HciEventOnReadScanEnableComplete,                           // 0x0019
    HciEventOnWriteScanEnableComplete,                          // 0x001A
    HciEventOnReadPageScanActivityComplete,                     // 0x001B
    HciEventOnWritePageScanActivityComplete,                    // 0x001C
    HciEventOnReadInquiryScanActivityComplete,                  // 0x001D
    HciEventOnWriteInquiryScanActivityComplete,                 // 0x001E
    NULL,                                                       // 0x001F
    HciEventOnWriteAuthenticationEnableComplete,                // 0x0020
    NULL,                                                       // 0x0021
    NULL,                                                       // 0x0022
    HciEventOnReadClassofDeviceComplete,                        // 0x0023
    HciEventOnWriteClassofDeviceComplete,                       // 0x0024
    HciEventOnReadVoiceSettingComplete,                         // 0x0025
    HciEventOnWriteVoiceSettingComplete,                        // 0x0026
    HciEventOnReadAutomaticFlushTimeoutComplete,                // 0x0027
    HciEventOnWriteAutomaticFlushTimeoutComplete,               // 0x0028
    HciEventOnReadNumBroadcastRetransmissionsComplete,          // 0x0029
    HciEventOnWriteNumBroadcastRetransmissionsComplete,         // 0x002A
    HciEventOnReadHoldModeActivityComplete,                     // 0x002B
    HciEventOnWriteHoldModeActivityComplete,                    // 0x002C
    HciEventOnReadTransmitPowerLevelComplete,                   // 0x002D
    HciEventOnReadSynchronousFlowControlEnableComplete,         // 0x002E
    HciEventOnWriteSynchronousFlowControlEnableComplete,        // 0x002F
    NULL,                                                       // 0x0030
    HciEventOnSetControllerToHostFlowControlComplete,           // 0x0031
    NULL,                                                       // 0x0032
    HciEventOnHostBufferSizeComplete,                           // 0x0033
    NULL,                                                       // 0x0034
    NULL,                                                       // 0x0035
    HciEventOnReadLinkSupervisionTimeoutComplete,               // 0x0036
    HciEventOnWriteLinkSupervisionTimeoutComplete,              // 0x0037
    HciEventOnReadNumberOfSupportedIACComplete,                 // 0x0038
    HciEventOnReadCurrentIacLapComplete,                        // 0x0039
    HciEventOnWriteCurrentIacLapComplete,                       // 0x003A
    NULL,                                                       // 0x003B
    NULL,                                                       // 0x003C
    NULL,                                                       // 0x003D
    NULL,                                                       // 0x003E
    HciEventOnSetAFHHostChannelClassificationComplete,          // 0x003F
    NULL,                                                       // 0x0040
    NULL,                                                       // 0x0041
    HciEventOnReadInquiryScanTypeComplete,                      // 0x0042
    HciEventOnWriteInquiryScanTypeComplete,                     // 0x0043
    HciEventOnReadInquiryModeComplete,                          // 0x0044
    HciEventOnWriteInquiryModeComplete,                         // 0x0045
    HciEventOnReadPageScanTypeCommandComplete,                  // 0x0046
    HciEventOnWritePageScanTypeCommandComplete,                 // 0x0047
    HciEventOnReadAFHChannelAssessmentModeComplete,             // 0x0048
    HciEventOnWriteAFHChannelAssessmentModeComplete,            // 0x0049
    NULL,                                                       // 0x004A
    NULL,                                                       // 0x004B
    NULL,                                                       // 0x004C
    NULL,                                                       // 0x004D
    NULL,                                                       // 0x004E
    NULL,                                                       // 0x004F
    NULL,                                                       // 0x0050
    HciEventOnReadExtendedInquiryResponseComplete,              // 0x0051
    HciEventOnWriteExtendedInquiryResponseComplete,             // 0x0052
    NULL,                                                       // 0x0053
    NULL,                                                       // 0x0054
    HciEventOnReadSimplePairingModeComplete,                    // 0x0055
    HciEventOnWriteSimplePairingModeComplete,                   // 0x0056
    HciEventOnReadLocalOOBDataComplete,                         // 0x0057
    NULL,                                                       // 0x0058
    HciEventOnWriteInquiryTransmitPowerLevelComplete,           // 0x0059
    HciEventOnReadDefaultErroneousDataReportingComplete,        // 0x005A
    HciEventOnWriteDefaultErroneousDataReportingLevelComplete,  // 0x005B
    NULL,                                                       // 0x005C
    NULL,                                                       // 0x005D
    NULL,                                                       // 0x005E
    NULL,                                                       // 0x005F
    HciEventOnSendKeypressNotificationComplete,                 // 0x0060
    NULL,                                                       // 0x0061
    HciEventOnWriteLogicalLinkAcceptTimeoutComplete,            // 0x0062
    HciEventOnSetEventMaskPage2Complete,                        // 0x0063
    HciEventOnReadLocationDataComplete,                         // 0x0064
    HciEventOnWriteLocationDataComplete,                        // 0x0065
    HciEventOnReadFlowControlModeComplete,                      // 0x0066
    HciEventOnWriteFlowControlModeComplete,                     // 0x0067
    HciEventOnReadEnhancedTransmitPowerLevelComplete,           // 0x0068
    HciEventOnReadBestEffortFlushTimeoutComplete,               // 0x0069
    HciEventOnWriteBestEffortFlushTimeoutComplete,              // 0x006A
    NULL,                                                       // 0x006B
    HciEventOnReadLEHostSupportComplete,                        // 0x006C
    HciEventOnWriteLeHostSupportComplete,                       // 0x006D
    HciEventOnSetMWSChannelParametersComplete,                  // 0x006E
    HciEventOnSetExternalFrameConfigurationComplete,            // 0x006F
    HciEventOnSetMWSSignalingComplete,                          // 0x0070
    HciEventOnSetMWSTransportLayerComplete,                     // 0x0071
    HciEventOnSetMWSScanFrequencyTableComplete,                 // 0x0072
    HciEventOnSetMWSPATTERNConfigurationComplete,               // 0x0073
    HciEventOnSetReservedLTADDRComplete,                        // 0x0074
    HciEventOnDeleteReservedLTADDRComplete,                     // 0x0075
    HciEventOnSetConnectionlessSlaveBroadcastDataComplete,      // 0x0076
    HciEventOnReadSynchronizationTrainParametersComplete,       // 0x0077
    HciEventOnWriteSynchronizationTrainParametersComplete,      // 0x0078
    HciEventOnReadSecureConnectionsHostSupportComplete,         // 0x0079
    HciEventOnWriteSecureConnectionsHostSupportComplete,        // 0x007A
    HciEventOnReadAuthenticatedPayloadTimeoutComplete,          // 0x007B
    HciEventOnWriteAuthenticatedPayloadTimeoutComplete,         // 0x007C
    HciEventOnReadLocalOOBExtendedDataComplete,                 // 0x007D
    HciEventOnReadExtendedPageTimeoutComplete,                  // 0x007E
    HciEventOnWriteExtendedPageTimeoutComplete,                 // 0x007F
    HciEventOnReadExtendedInquiryLengthComplete,                // 0x0080
    HciEventOnWriteExtendedInquiryLengthComplete,               // 0x0081
};

#define CONTROLLERBASEBAND_OCF_MAX 0x0081

void HciEventOnControllerBasebandCommandComplete(uint16_t opCode, const void *param, uint8_t length)
{
    uint16_t ocf = GET_OCF(opCode);
    if (ocf > CONTROLLERBASEBAND_OCF_MAX) {
        return;
    }

    HciEventCommandCompleteFunc func = g_controllerBasebandCommandCompleteMap[ocf];
    if (func != NULL) {
        func(param, length);
    }
}