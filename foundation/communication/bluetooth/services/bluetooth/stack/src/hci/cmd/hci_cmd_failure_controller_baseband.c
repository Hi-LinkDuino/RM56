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

#include "hci_cmd_failure_controller_baseband.h"

#include "btstack.h"
#include "platform/include/list.h"

#include "hci/evt/hci_evt.h"
#include "hci/hci.h"
#include "hci/hci_def.h"

#include "hci_cmd_failure.h"

static void HciCmdOnSetEventMaskFailed(uint8_t status, const void *param)
{
    HciSetEventMaskReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->setEventMaskComplete != NULL) {
        callbacks->setEventMaskComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnResetFailed(uint8_t status, const void *param)
{
    HciResetReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->resetComplete != NULL) {
        callbacks->resetComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnSetEventFilterFailed(uint8_t status, const void *param)
{
    HciSetEventFilterReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->setEventFilterComplete != NULL) {
        callbacks->setEventFilterComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnFlushFailed(uint8_t status, const void *param)
{
    HciFlushReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->flushComplete != NULL) {
        callbacks->flushComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadPinTypeFailed(uint8_t status, const void *param)
{
    HciReadPinTypeReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readPinTypeComplete != NULL) {
        callbacks->readPinTypeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnWritePinTypeFailed(uint8_t status, const void *param)
{
    HciWritePinTypeReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writePinTypeComplete != NULL) {
        callbacks->writePinTypeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnCreateNewUnitKeyFailed(uint8_t status, const void *param)
{
    HciCreateNewUnitKeyReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->createNewUnitKeyComplete != NULL) {
        callbacks->createNewUnitKeyComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadStoredLinkKeyFailed(uint8_t status, const void *param)
{
    HciReadStoredLinkKeyReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readStoredLinkKeyComplete != NULL) {
        callbacks->readStoredLinkKeyComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnDeleteStoredLinkKeyFailed(uint8_t status, const void *param)
{
    HciDeleteStoredLinkKeyReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->deleteStoredLinkKeyComplete != NULL) {
        callbacks->deleteStoredLinkKeyComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnWriteLocalNameFailed(uint8_t status, const void *param)
{
    HciWriteLocalNameReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeLocalNameComplete != NULL) {
        callbacks->writeLocalNameComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadLocalNameFailed(uint8_t status, const void *param)
{
    HciReadLocalNameReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readLocalNameComplete != NULL) {
        callbacks->readLocalNameComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadConnectionAcceptTimeoutFailed(uint8_t status, const void *param)
{
    HciReadConnectionAcceptTimeoutReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readConnectionAcceptTimeoutComplete != NULL) {
        callbacks->readConnectionAcceptTimeoutComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnWriteConnectionAcceptTimeoutFailed(uint8_t status, const void *param)
{
    HciWriteConnectionAcceptTimeoutReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeConnectionAcceptTimeoutComplete != NULL) {
        callbacks->writeConnectionAcceptTimeoutComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadPageTimeoutFailed(uint8_t status, const void *param)
{
    HciReadPageTimeoutReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readPageTimeoutComplete != NULL) {
        callbacks->readPageTimeoutComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnWritePageTimeoutFailed(uint8_t status, const void *param)
{
    HciWritePageTimeoutReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writePageTimeoutComplete != NULL) {
        callbacks->writePageTimeoutComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadScanEnableFailed(uint8_t status, const void *param)
{
    HciReadScanEnableReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readScanEnableComplete != NULL) {
        callbacks->readScanEnableComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnWriteScanEnableFailed(uint8_t status, const void *param)
{
    HciWriteScanEnableReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeScanEnableComplete != NULL) {
        callbacks->writeScanEnableComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadPageScanActivityFailed(uint8_t status, const void *param)
{
    HciReadPageScanActivityReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readPageScanActivityComplete != NULL) {
        callbacks->readPageScanActivityComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnWritePageScanActivityFailed(uint8_t status, const void *param)
{
    HciWritePageScanActivityReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writePageScanActivityComplete != NULL) {
        callbacks->writePageScanActivityComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadInquiryScanActivityFailed(uint8_t status, const void *param)
{
    HciReadInquiryScanActivityReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readInquiryScanActivityComplete != NULL) {
        callbacks->readInquiryScanActivityComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnWriteInquiryScanActivityFailed(uint8_t status, const void *param)
{
    HciWriteInquiryScanActivityReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeInquiryScanActivityComplete != NULL) {
        callbacks->writeInquiryScanActivityComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadAuthenticationEnableFailed(uint8_t status, const void *param)
{
    HciReadAuthenticationEnableReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readAuthenticationEnableComplete != NULL) {
        callbacks->readAuthenticationEnableComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnWriteAuthenticationEnableFailed(uint8_t status, const void *param)
{
    HciWriteAuthenticationEnableReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeAuthenticationEnableComplete != NULL) {
        callbacks->writeAuthenticationEnableComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadClassofDeviceFailed(uint8_t status, const void *param)
{
    HciReadClassofDeviceReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readClassofDeviceComplete != NULL) {
        callbacks->readClassofDeviceComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnWriteClassofDeviceFailed(uint8_t status, const void *param)
{
    HciWriteClassofDeviceReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeClassofDeviceComplete != NULL) {
        callbacks->writeClassofDeviceComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadVoiceSettingFailed(uint8_t status, const void *param)
{
    HciReadVoiceSettingReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readVoiceSettingComplete != NULL) {
        callbacks->readVoiceSettingComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnWriteVoiceSettingFailed(uint8_t status, const void *param)
{
    HciWriteVoiceSettingParamReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeVoiceSettingComplete != NULL) {
        callbacks->writeVoiceSettingComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadAutomaticFlushTimeoutFailed(uint8_t status, const void *param)
{
    HciReadAutomaticFlushTimeoutReturnParam returnParam = {
        .status = status,
        .connectionHandle = ((HciReadAutomaticFlushTimeoutReturnParam *)param)->connectionHandle,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readAutomaticFlushTimeoutComplete != NULL) {
        callbacks->readAutomaticFlushTimeoutComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnWriteAutomaticFlushTimeoutFailed(uint8_t status, const void *param)
{
    HciWriteAutomaticFlushTimeoutReturnParam returnParam = {
        .status = status,
        .connectionHandle = ((HciWriteAutomaticFlushTimeoutReturnParam *)param)->connectionHandle,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeAutomaticFlushTimeoutComplete != NULL) {
        callbacks->writeAutomaticFlushTimeoutComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadNumBroadcastRetransmissionsFailed(uint8_t status, const void *param)
{
    HciReadNumBroadcastRetransmissionsReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readNumBroadcastRetransmissionsComplete != NULL) {
        callbacks->readNumBroadcastRetransmissionsComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnWriteNumBroadcastRetransmissionsFailed(uint8_t status, const void *param)
{
    HciWriteNumBroadcastRetransmissionsReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeNumBroadcastRetransmissionsComplete != NULL) {
        callbacks->writeNumBroadcastRetransmissionsComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadHoldModeActivityFailed(uint8_t status, const void *param)
{
    HciReadHoldModeActivityReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readHoldModeActivityComplete != NULL) {
        callbacks->readHoldModeActivityComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnWriteHoldModeActivityFailed(uint8_t status, const void *param)
{
    HciWriteHoldModeActivityReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeHoldModeActivityComplete != NULL) {
        callbacks->writeHoldModeActivityComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadTransmitPowerLevelFailed(uint8_t status, const void *param)
{
    HciReadTransmitPowerLevelReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readTransmitPowerLevelComplete != NULL) {
        callbacks->readTransmitPowerLevelComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadSynchronousFlowControlEnableFailed(uint8_t status, const void *param)
{
    HciReadSynchronousFlowControlEnableReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readSynchronousFlowControlEnableComplete != NULL) {
        callbacks->readSynchronousFlowControlEnableComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnWriteSynchronousFlowControlEnableFailed(uint8_t status, const void *param)
{
    HciWriteSynchronousFlowControlEnableReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeSynchronousFlowControlEnableComplete != NULL) {
        callbacks->writeSynchronousFlowControlEnableComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnSetControllerToHostFlowControlFailed(uint8_t status, const void *param)
{
    HciSetControllerToHostFlowControlReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->setControllerToHostFlowControlComplete != NULL) {
        callbacks->setControllerToHostFlowControlComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnHostBufferSizeFailed(uint8_t status, const void *param)
{
    HciHostBufferSizeReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->hostBufferSizeComplete != NULL) {
        callbacks->hostBufferSizeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadLinkSupervisionTimeoutFailed(uint8_t status, const void *param)
{
    HciReadLinkSupervisionTimeoutReturnParam returnParam = {
        .status = status,
        .handle = ((HciReadLinkSupervisionTimeoutReturnParam *)param)->handle,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readLinkSupervisionTimeoutComplete != NULL) {
        callbacks->readLinkSupervisionTimeoutComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnWriteLinkSupervisionTimeoutFailed(uint8_t status, const void *param)
{
    HciWriteLinkSupervisionTimeoutReturnParam returnParam = {
        .status = status,
        .handle = ((HciWriteLinkSupervisionTimeoutReturnParam *)param)->handle,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeLinkSupervisionTimeoutComplete != NULL) {
        callbacks->writeLinkSupervisionTimeoutComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadNumberOfSupportedIACFailed(uint8_t status, const void *param)
{
    HciReadNumberOfSupportedIacReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readNumberOfSupportedIacComplete != NULL) {
        callbacks->readNumberOfSupportedIacComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadCurrentIacLapFailed(uint8_t status, const void *param)
{
    HciReadCurrentIacLapReturnParam returParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readCurrentIacLapComplete != NULL) {
        callbacks->readCurrentIacLapComplete(&returParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnWriteCurrentIacLapFailed(uint8_t status, const void *param)
{
    HciWriteCurrentIacLapReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeCurrentIacLapComplete != NULL) {
        callbacks->writeCurrentIacLapComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnSetAfhHostChannelClassificationFailed(uint8_t status, const void *param)
{
    HciSetAfhHostChannelClassificationReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->setAfhHostChannelClassificationComplete != NULL) {
        callbacks->setAfhHostChannelClassificationComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadInquiryScanTypeFailed(uint8_t status, const void *param)
{
    HciReadInquiryScanTypeReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readInquiryScanTypeComplete != NULL) {
        callbacks->readInquiryScanTypeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnWriteInquiryScanTypeFailed(uint8_t status, const void *param)
{
    HciWriteInquiryScanTypeReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeInquiryScanTypeComplete != NULL) {
        callbacks->writeInquiryScanTypeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadInquiryModeFailed(uint8_t status, const void *param)
{
    HciReadInquiryModeReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readInquiryModeComplete != NULL) {
        callbacks->readInquiryModeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnWriteInquiryModeFailed(uint8_t status, const void *param)
{
    HciWriteInquiryModeReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeInquiryModeComplete != NULL) {
        callbacks->writeInquiryModeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadPageScanTypeFailed(uint8_t status, const void *param)
{
    HciReadPageScanTypeReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readPageScanTypeComplete != NULL) {
        callbacks->readPageScanTypeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnWritePageScanTypeFailed(uint8_t status, const void *pararm)
{
    HciWritePageScanTypeReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writePageScanTypeComplete != NULL) {
        callbacks->writePageScanTypeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadAfhChannelAssessmentModeFailed(uint8_t status, const void *pararm)
{
    HciReadAfhChannelAssessmentModeReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readAfhChannelAssessmentModeComplete != NULL) {
        callbacks->readAfhChannelAssessmentModeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnWriteAfhChannelAssessmentModeFailed(uint8_t status, const void *pararm)
{
    HciWriteAfhChannelAssessmentModeReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeAfhChannelAssessmentModeComplete != NULL) {
        callbacks->writeAfhChannelAssessmentModeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadExtendedInquiryResponseFailed(uint8_t status, const void *pararm)
{
    HciReadExtendedInquiryResponseReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readExtendedInquiryResponseComplete != NULL) {
        callbacks->readExtendedInquiryResponseComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnWriteExtendedInquiryResponseFailed(uint8_t status, const void *param)
{
    HciWriteExtendedInquiryResponseReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeExtendedInquiryResponseComplete != NULL) {
        callbacks->writeExtendedInquiryResponseComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadSimplePairingModeFailed(uint8_t status, const void *param)
{
    HciReadSimplePairingModeReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readSimplePairingModeComplete != NULL) {
        callbacks->readSimplePairingModeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnWriteSimplePairingModeFailed(uint8_t status, const void *param)
{
    HciWriteSimplePairingModeReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeSimplePairingModeComplete != NULL) {
        callbacks->writeSimplePairingModeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadLocalOobDataFailed(uint8_t status, const void *param)
{
    HciReadLocalOOBDataReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readLocalOOBDataComplete != NULL) {
        callbacks->readLocalOOBDataComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadInquiryResponseTransmitPowerLevelFailed(uint8_t status, const void *param)
{
    HciReadInquiryResponseTransmitPowerLevelReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readInquiryResponseTransmitPowerLevelComplete != NULL) {
        callbacks->readInquiryResponseTransmitPowerLevelComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnWriteInquiryTransmitPowerLevelFailed(uint8_t status, const void *param)
{
    HciWriteInquiryTransmitPowerLevelReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeInquiryTransmitPowerLevelComplete != NULL) {
        callbacks->writeInquiryTransmitPowerLevelComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnSendKeypressNotificationFailed(uint8_t status, const void *param)
{
    HciSendKeypressNotificationReturnParam returnParam = {
        .status = status,
        .bdAddr = ((HciSendKeypressNotificationParam *)param)->bdAddr,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->sendKeypressNotificationComplete != NULL) {
        callbacks->sendKeypressNotificationComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadDefaultErroneousDataReportingFailed(uint8_t status, const void *param)
{
    HciReadDefaultErroneousDataReportingReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readDefaultErroneousDataReportingComplete != NULL) {
        callbacks->readDefaultErroneousDataReportingComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnWriteDefaultErroneousDataReportingFailed(uint8_t status, const void *param)
{
    HciWriteDefaultErroneousDataReportingReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeDefaultErroneousDataReportingComplete != NULL) {
        callbacks->writeDefaultErroneousDataReportingComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadLogicalLinkAcceptTimeoutFailed(uint8_t status, const void *param)
{
    HciReadLogicalLinkAcceptTimeoutReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readLogicalLinkAcceptTimeoutComplete != NULL) {
        callbacks->readLogicalLinkAcceptTimeoutComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnWriteLogicalLinkAcceptTimeoutFailed(uint8_t status, const void *param)
{
    HciWriteLogicalLinkAcceptTimeoutReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeLogicalLinkAcceptTimeoutComplete != NULL) {
        callbacks->writeLogicalLinkAcceptTimeoutComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnSetEventMaskPage2Failed(uint8_t status, const void *param)
{
    HciSetEventMaskPage2ReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->setEventMaskPage2Complete != NULL) {
        callbacks->setEventMaskPage2Complete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadLocationDataFailed(uint8_t status, const void *param)
{
    HciReadLocationDataReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readLocationDataComplete != NULL) {
        callbacks->readLocationDataComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnWriteLocationDataFailed(uint8_t status, const void *param)
{
    HciWriteLocationDataReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeLocationDataComplete != NULL) {
        callbacks->writeLocationDataComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadFlowControlModeFailed(uint8_t status, const void *param)
{
    HciReadFlowControlModeReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readFlowControlModeComplete != NULL) {
        callbacks->readFlowControlModeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnWriteFlowControlModeFailed(uint8_t status, const void *param)
{
    HciWriteFlowControlModeReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeFlowControlModeComplete != NULL) {
        callbacks->writeFlowControlModeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadEnhancedTransmitPowerLevelFailed(uint8_t status, const void *param)
{
    HciReadEnhancedTransmitPowerLevelReturnParam returnParam = {
        .status = status,
        .connectionHandle = ((HciReadEnhancedTransmitPowerLevelReturnParam *)param)->connectionHandle,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readEnhancedTransmitPowerLevelComplete != NULL) {
        callbacks->readEnhancedTransmitPowerLevelComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadBestEffortFlushTimeoutFailed(uint8_t status, const void *param)
{
    HciReadBestEffortFlushTimeoutReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readBestEffortFlushTimeoutComplete != NULL) {
        callbacks->readBestEffortFlushTimeoutComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnWriteBestEffortFlushTimeoutFailed(uint8_t status, const void *param)
{
    HciWriteBestEffortFlushTimeoutReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeBestEffortFlushTimeoutComplete != NULL) {
        callbacks->writeBestEffortFlushTimeoutComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadLeHostSupportFailed(uint8_t status, const void *param)
{
    HciReadLeHostSupportReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readLeHostSupportComplete != NULL) {
        callbacks->readLeHostSupportComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnWriteLeHostSupportFailed(uint8_t status, const void *param)
{
    HciWriteLeHostSupportReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeLeHostSupportComplete != NULL) {
        callbacks->writeLeHostSupportComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnSetMwsChannelParametersFailed(uint8_t status, const void *param)
{
    HciSetMwsChannelParametersReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->setMwsChannelParametersComplete != NULL) {
        callbacks->setMwsChannelParametersComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnSetMwsSignalingFailed(uint8_t status, const void *param)
{
    HciSetMwsSignalingReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->setMwsSignalingComplete != NULL) {
        callbacks->setMwsSignalingComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnSetMwsTransportLayerFailed(uint8_t status, const void *param)
{
    HciSetMwsTransportLayerReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->setMwsTransportLayerComplete != NULL) {
        callbacks->setMwsTransportLayerComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnSetReservedLtAddrFailed(uint8_t status, const void *param)
{
    HciSetReservedLtAddrReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->setReservedLtAddrComplete != NULL) {
        callbacks->setReservedLtAddrComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnDeleteReservedLtAddrFailed(uint8_t status, const void *param)
{
    HciDeleteReservedLtAddrReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->deleteReservedLtAddrComplete != NULL) {
        callbacks->deleteReservedLtAddrComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadSynchronizationTrainParametersFailed(uint8_t status, const void *param)
{
    HciReadSynchronizationTrainParametersReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readSynchronizationTrainParametersComplete != NULL) {
        callbacks->readSynchronizationTrainParametersComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnWriteSynchronizationTrainParametersFailed(uint8_t status, const void *param)
{
    HciWriteSynchronizationTrainParametersReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeSynchronizationTrainParametersComplete != NULL) {
        callbacks->writeSynchronizationTrainParametersComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadSecureConnectionsHostSupportFailed(uint8_t status, const void *param)
{
    HciReadSecureConnectionsHostSupportReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readSecureConnectionsHostSupportComplete != NULL) {
        callbacks->readSecureConnectionsHostSupportComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnWriteSecureConnectionsHostSupportFailed(uint8_t status, const void *param)
{
    HciWriteSecureConnectionsHostSupportReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeSecureConnectionsHostSupportComplete != NULL) {
        callbacks->writeSecureConnectionsHostSupportComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadAuthenticatedPayloadTimeoutFailed(uint8_t status, const void *param)
{
    HciReadAuthenticatedPayloadTimeoutReturnParam returnParam = {
        .status = status,
        .connectionHandle = ((HciReadAuthenticatedPayloadTimeoutReturnParam *)param)->connectionHandle,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readAuthenticatedPayloadTimeoutComplete != NULL) {
        callbacks->readAuthenticatedPayloadTimeoutComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnWriteAuthenticatedPayloadTimeoutFailed(uint8_t status, const void *param)
{
    HciWriteAuthenticatedPayloadTimeoutReturnParam returnParam = {
        .status = status,
        .connectionHandle = ((HciWriteAuthenticatedPayloadTimeoutParam *)param)->connectionHandle,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeAuthenticatedPayloadTimeoutComplete != NULL) {
        callbacks->writeAuthenticatedPayloadTimeoutComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadLocalOobExtendedDataFailed(uint8_t status, const void *param)
{
    HciReadLocalOobExtendedDataReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readLocalOOBExtendedDataComplete != NULL) {
        callbacks->readLocalOOBExtendedDataComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadExtendedPageTimeoutFailed(uint8_t status, const void *param)
{
    HciReadExtendedPageTimeoutReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readExtendedPageTimeoutComplete != NULL) {
        callbacks->readExtendedPageTimeoutComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnWriteExtendedPageTimeoutFailed(uint8_t status, const void *param)
{
    HciWriteExtendedPageTimeoutReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeExtendedPageTimeoutComplete != NULL) {
        callbacks->writeExtendedPageTimeoutComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadExtendedInquiryLengthFailed(uint8_t status, const void *param)
{
    HciReadExtendedInquiryLengthReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readExtendedInquiryLengthComplete != NULL) {
        callbacks->readExtendedInquiryLengthComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnWriteExtendedInquiryLengthFailed(uint8_t status, const void *param)
{
    HciWriteExtendedInquiryLengthReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeExtendedInquiryLengthComplete != NULL) {
        callbacks->writeExtendedInquiryLengthComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static HciCmdOnFailedFunc g_funcMap[] = {
    NULL,                                                 // 0x0000
    HciCmdOnSetEventMaskFailed,                           // 0x0001
    NULL,                                                 // 0x0002
    HciCmdOnResetFailed,                                  // 0x0003
    NULL,                                                 // 0x0004
    HciCmdOnSetEventFilterFailed,                         // 0x0005
    NULL,                                                 // 0x0006
    NULL,                                                 // 0x0007
    HciCmdOnFlushFailed,                                  // 0x0008
    HciCmdOnReadPinTypeFailed,                            // 0x0009
    HciCmdOnWritePinTypeFailed,                           // 0x000A
    HciCmdOnCreateNewUnitKeyFailed,                       // 0x000B
    NULL,                                                 // 0x000C
    HciCmdOnReadStoredLinkKeyFailed,                      // 0x000D
    NULL,                                                 // 0x000E
    NULL,                                                 // 0x000F
    NULL,                                                 // 0x0010
    NULL,                                                 // 0x0011
    HciCmdOnDeleteStoredLinkKeyFailed,                    // 0x0012
    HciCmdOnWriteLocalNameFailed,                         // 0x0013
    HciCmdOnReadLocalNameFailed,                          // 0x0014
    HciCmdOnReadConnectionAcceptTimeoutFailed,            // 0x0015
    HciCmdOnWriteConnectionAcceptTimeoutFailed,           // 0x0016
    HciCmdOnReadPageTimeoutFailed,                        // 0x0017
    HciCmdOnWritePageTimeoutFailed,                       // 0x0018
    HciCmdOnReadScanEnableFailed,                         // 0x0019
    HciCmdOnWriteScanEnableFailed,                        // 0x001A
    HciCmdOnReadPageScanActivityFailed,                   // 0x001B
    HciCmdOnWritePageScanActivityFailed,                  // 0x001C
    HciCmdOnReadInquiryScanActivityFailed,                // 0x001D
    HciCmdOnWriteInquiryScanActivityFailed,               // 0x001E
    HciCmdOnReadAuthenticationEnableFailed,               // 0x001F
    HciCmdOnWriteAuthenticationEnableFailed,              // 0x0020
    NULL,                                                 // 0x0021
    NULL,                                                 // 0x0022
    HciCmdOnReadClassofDeviceFailed,                      // 0x0023
    HciCmdOnWriteClassofDeviceFailed,                     // 0x0024
    HciCmdOnReadVoiceSettingFailed,                       // 0x0025
    HciCmdOnWriteVoiceSettingFailed,                      // 0x0026
    HciCmdOnReadAutomaticFlushTimeoutFailed,              // 0x0027
    HciCmdOnWriteAutomaticFlushTimeoutFailed,             // 0x0028
    HciCmdOnReadNumBroadcastRetransmissionsFailed,        // 0x0029
    HciCmdOnWriteNumBroadcastRetransmissionsFailed,       // 0x002A
    HciCmdOnReadHoldModeActivityFailed,                   // 0x002B
    HciCmdOnWriteHoldModeActivityFailed,                  // 0x002C
    HciCmdOnReadTransmitPowerLevelFailed,                 // 0x002D
    HciCmdOnReadSynchronousFlowControlEnableFailed,       // 0x002E
    HciCmdOnWriteSynchronousFlowControlEnableFailed,      // 0x002F
    NULL,                                                 // 0x0030
    HciCmdOnSetControllerToHostFlowControlFailed,         // 0x0031
    NULL,                                                 // 0x0032
    HciCmdOnHostBufferSizeFailed,                         // 0x0033
    NULL,                                                 // 0x0034
    NULL,                                                 // 0x0035
    HciCmdOnReadLinkSupervisionTimeoutFailed,             // 0x0036
    HciCmdOnWriteLinkSupervisionTimeoutFailed,            // 0x0037
    HciCmdOnReadNumberOfSupportedIACFailed,               // 0x0038
    HciCmdOnReadCurrentIacLapFailed,                      // 0x0039
    HciCmdOnWriteCurrentIacLapFailed,                     // 0x003A
    NULL,                                                 // 0x003B
    NULL,                                                 // 0x003C
    NULL,                                                 // 0x003D
    NULL,                                                 // 0x003E
    HciCmdOnSetAfhHostChannelClassificationFailed,        // 0x003F
    NULL,                                                 // 0x0040
    NULL,                                                 // 0x0041
    HciCmdOnReadInquiryScanTypeFailed,                    // 0x0042
    HciCmdOnWriteInquiryScanTypeFailed,                   // 0x0043
    HciCmdOnReadInquiryModeFailed,                        // 0x0044
    HciCmdOnWriteInquiryModeFailed,                       // 0x0045
    HciCmdOnReadPageScanTypeFailed,                       // 0x0046
    HciCmdOnWritePageScanTypeFailed,                      // 0x0047
    HciCmdOnReadAfhChannelAssessmentModeFailed,           // 0x0048
    HciCmdOnWriteAfhChannelAssessmentModeFailed,          // 0x0049
    NULL,                                                 // 0x004A
    NULL,                                                 // 0x004B
    NULL,                                                 // 0x004C
    NULL,                                                 // 0x004D
    NULL,                                                 // 0x004E
    NULL,                                                 // 0x004F
    NULL,                                                 // 0x0050
    HciCmdOnReadExtendedInquiryResponseFailed,            // 0x0051
    HciCmdOnWriteExtendedInquiryResponseFailed,           // 0x0052
    NULL,                                                 // 0x0053
    NULL,                                                 // 0x0054
    HciCmdOnReadSimplePairingModeFailed,                  // 0x0055
    HciCmdOnWriteSimplePairingModeFailed,                 // 0x0056
    HciCmdOnReadLocalOobDataFailed,                       // 0x0057
    HciCmdOnReadInquiryResponseTransmitPowerLevelFailed,  // 0x0058
    HciCmdOnWriteInquiryTransmitPowerLevelFailed,         // 0x0059
    HciCmdOnReadDefaultErroneousDataReportingFailed,      // 0x005A
    HciCmdOnWriteDefaultErroneousDataReportingFailed,     // 0x005B
    NULL,                                                 // 0x005C
    NULL,                                                 // 0x005D
    NULL,                                                 // 0x005E
    NULL,                                                 // 0x005F
    HciCmdOnSendKeypressNotificationFailed,               // 0x0060
    HciCmdOnReadLogicalLinkAcceptTimeoutFailed,           // 0x0061
    HciCmdOnWriteLogicalLinkAcceptTimeoutFailed,          // 0x0062
    HciCmdOnSetEventMaskPage2Failed,                      // 0x0063
    HciCmdOnReadLocationDataFailed,                       // 0x0064
    HciCmdOnWriteLocationDataFailed,                      // 0x0065
    HciCmdOnReadFlowControlModeFailed,                    // 0x0066
    HciCmdOnWriteFlowControlModeFailed,                   // 0x0067
    HciCmdOnReadEnhancedTransmitPowerLevelFailed,         // 0x0068
    HciCmdOnReadBestEffortFlushTimeoutFailed,             // 0x0069
    HciCmdOnWriteBestEffortFlushTimeoutFailed,            // 0x006A
    NULL,                                                 // 0x006B
    HciCmdOnReadLeHostSupportFailed,                      // 0x006C
    HciCmdOnWriteLeHostSupportFailed,                     // 0x006D
    HciCmdOnSetMwsChannelParametersFailed,                // 0x006E
    NULL,                                                 // 0x006F
    HciCmdOnSetMwsSignalingFailed,                        // 0x0070
    HciCmdOnSetMwsTransportLayerFailed,                   // 0x0071
    NULL,                                                 // 0x0072
    NULL,                                                 // 0x0073
    HciCmdOnSetReservedLtAddrFailed,                      // 0x0074
    HciCmdOnDeleteReservedLtAddrFailed,                   // 0x0075
    NULL,                                                 // 0x0076
    HciCmdOnReadSynchronizationTrainParametersFailed,     // 0x0077
    HciCmdOnWriteSynchronizationTrainParametersFailed,    // 0x0078
    HciCmdOnReadSecureConnectionsHostSupportFailed,       // 0x0079
    HciCmdOnWriteSecureConnectionsHostSupportFailed,      // 0x007A
    HciCmdOnReadAuthenticatedPayloadTimeoutFailed,        // 0x007B
    HciCmdOnWriteAuthenticatedPayloadTimeoutFailed,       // 0x007C
    HciCmdOnReadLocalOobExtendedDataFailed,               // 0x007D
    HciCmdOnReadExtendedPageTimeoutFailed,                // 0x007E
    HciCmdOnWriteExtendedPageTimeoutFailed,               // 0x007F
    HciCmdOnReadExtendedInquiryLengthFailed,              // 0x0080
    HciCmdOnWriteExtendedInquiryLengthFailed,             // 0x0081
};

#define CONTROLLERBASEBAND_OCF_MAX 0x0081

void HciOnControllerBasebandCmdFailed(uint16_t opCode, uint8_t status, const void *param)
{
    uint16_t ocf = GET_OCF(opCode);
    if (ocf > CONTROLLERBASEBAND_OCF_MAX) {
        return;
    }

    HciCmdOnFailedFunc func = g_funcMap[ocf];
    if (func != NULL) {
        func(status, param);
    }
}