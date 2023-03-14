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

#include "hci_evt_status_params_cmd_complete.h"

#include <securec.h>

#include "platform/include/allocator.h"
#include "platform/include/list.h"
#include "platform/include/mutex.h"

#include "hci/cmd/hci_cmd.h"
#include "hci/hci.h"
#include "hci/hci_def.h"

#include "hci_evt.h"
#include "hci_evt_cmd_complete.h"

static void HciEventOnReadRssiComplete(const void *param, uint8_t length)
{
    HciReadRssiReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readRssiComplete != NULL) {
        callbacks->readRssiComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadFailedContactCounterComplete(const void *param, uint8_t length)
{
    HciReadFailedContactCounterReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readFailedContactCounterComplete != NULL) {
        callbacks->readFailedContactCounterComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnResetFailedContactCounterComplete(const void *param, uint8_t length)
{
    HciResetFailedContactCounterReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->resetFailedContactCounterComplete != NULL) {
        callbacks->resetFailedContactCounterComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadLinkQualityComplete(const void *param, uint8_t length)
{
    HciReadLinkQualityReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readLinkQualityComplete != NULL) {
        callbacks->readLinkQualityComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadAFHChannelMapComplete(const void *param, uint8_t length)
{
    HciReadAfhChannelMapReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readAfhChannelMapComplete != NULL) {
        callbacks->readAfhChannelMapComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadClockComplete(const void *param, uint8_t length)
{
    HciReadClockReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readClockComplete != NULL) {
        callbacks->readClockComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadEncryptionKeySizeComplete(const void *param, uint8_t length)
{
    HciReadEncryptionKeySizeReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readEncryptionKeySizeComplete != NULL) {
        callbacks->readEncryptionKeySizeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadLocalAMPInfoComplete(const void *param, uint8_t length)
{
    HciReadLocalAMPInfoReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readLocalAmpInfoComplete != NULL) {
        callbacks->readLocalAmpInfoComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadLocalAMPASSOCComplete(const void *param, uint8_t length)
{
    HciReadLocalAmpAssocReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readLocalAmpAssocComplete != NULL) {
        callbacks->readLocalAmpAssocComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnWriteRemoteAMPASSOCComplete(const void *param, uint8_t length)
{
    HciWriteRemoteAmpAssocReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeRemoteAmpAssocComplete != NULL) {
        callbacks->writeRemoteAmpAssocComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnGetMWSTransportLayerConfigurationComplete(const void *param, uint8_t length)
{
    HciGetMwsTransportLayerConfigurationReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->getMwsTransportLayerConfigurationComplete != NULL) {
        callbacks->getMwsTransportLayerConfigurationComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnSetTriggeredClockCaptureComplete(const void *param, uint8_t length)
{
    HciSetTriggeredClockCaptureReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->setTriggeredClockCaptureComplete != NULL) {
        callbacks->setTriggeredClockCaptureComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static HciEventCommandCompleteFunc g_statusParametersCommandCompleteMap[] = {
    NULL,                                                 // 0x0000
    HciEventOnReadFailedContactCounterComplete,           // 0x0001
    HciEventOnResetFailedContactCounterComplete,          // 0x0002
    HciEventOnReadLinkQualityComplete,                    // 0x0003
    NULL,                                                 // 0x0004
    HciEventOnReadRssiComplete,                           // 0x0005
    HciEventOnReadAFHChannelMapComplete,                  // 0x0006
    HciEventOnReadClockComplete,                          // 0x0007
    HciEventOnReadEncryptionKeySizeComplete,              // 0x0008
    HciEventOnReadLocalAMPInfoComplete,                   // 0x0009
    HciEventOnReadLocalAMPASSOCComplete,                  // 0x000A
    HciEventOnWriteRemoteAMPASSOCComplete,                // 0x000B
    HciEventOnGetMWSTransportLayerConfigurationComplete,  // 0x000C
    HciEventOnSetTriggeredClockCaptureComplete,           // 0x000D
};

#define STATUSPARAMETERS_OCF_MAX 0x000D

void HciEventOnStatusParametersCommandComplete(uint16_t opCode, const void *param, uint8_t length)
{
    uint16_t ocf = GET_OCF(opCode);
    if (ocf > STATUSPARAMETERS_OCF_MAX) {
        return;
    }

    HciEventCommandCompleteFunc func = g_statusParametersCommandCompleteMap[ocf];
    if (func != NULL) {
        func(param, length);
    }
}