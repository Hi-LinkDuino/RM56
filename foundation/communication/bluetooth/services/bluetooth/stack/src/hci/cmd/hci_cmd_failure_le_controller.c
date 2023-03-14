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

#include "hci_cmd_failure_le_controller.h"

#include "btstack.h"
#include "platform/include/list.h"

#include "hci/evt/hci_evt.h"
#include "hci/hci.h"
#include "hci/hci_def.h"

#include "hci_cmd_failure.h"

static void HciCmdOnLeSetEventMaskFailed(uint8_t status, const void *param)
{
    HciLeSetEventMaskReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetEventMaskComplete != NULL) {
        callbacks->leSetEventMaskComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeReadBufferSizeFailed(uint8_t status, const void *param)
{
    HciLeReadBufferSizeReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leReadBufferSizeComplete != NULL) {
        callbacks->leReadBufferSizeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeReadLocalSupportedFeaturesFailed(uint8_t status, const void *param)
{
    HciLeReadLocalSupportedFeaturesReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leReadLocalSupportedFeaturesComplete != NULL) {
        callbacks->leReadLocalSupportedFeaturesComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeSetRandomAddressFailed(uint8_t status, const void *param)
{
    HciLeSetRandomAddressReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetRandomAddressComplete != NULL) {
        callbacks->leSetRandomAddressComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeSetAdvertisingParametersFailed(uint8_t status, const void *param)
{
    HciLeSetAdvertisingParametersReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetAdvertisingParametersComplete != NULL) {
        callbacks->leSetAdvertisingParametersComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadAdvertisingChannelTxPowerFailed(uint8_t status, const void *param)
{
    HciLeReadAdvertisingChannelTxPowerReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leReadAdvertisingChannelTxPowerComplete != NULL) {
        callbacks->leReadAdvertisingChannelTxPowerComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeSetAdvertisingDataFailed(uint8_t status, const void *param)
{
    HciLeSetAdvertisingDataReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetAdvertisingDataComplete != NULL) {
        callbacks->leSetAdvertisingDataComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeSetScanResponseDataFailed(uint8_t status, const void *param)
{
    HciLeSetScanResponseDataReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetScanResponseDataComplete != NULL) {
        callbacks->leSetScanResponseDataComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeSetAdvertisingEnableFailed(uint8_t status, const void *param)
{
    HciLeSetAdvertisingEnableReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetAdvertisingEnableComplete != NULL) {
        callbacks->leSetAdvertisingEnableComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeSetScanParametersFailed(uint8_t status, const void *param)
{
    HciLeSetScanParametersReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetScanParametersComplete != NULL) {
        callbacks->leSetScanParametersComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeSetScanEnableFailed(uint8_t status, const void *param)
{
    HciLeSetScanEnableReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetScanEnableComplete != NULL) {
        callbacks->leSetScanEnableComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeCreateConnectionFailed(uint8_t status, const void *param)
{
    HciLeConnectionCompleteEventParam eventParam = {
        .status = status,
        .peerAddress = ((HciLeCreateConnectionParam *)param)->peerAddress,
        .peerAddressType = ((HciLeCreateConnectionParam *)param)->peerAddressType,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leConnectionComplete != NULL) {
        callbacks->leConnectionComplete(&eventParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeCreateConnectionCancelFailed(uint8_t status, const void *param)
{
    HciLeCreateConnectionCancelReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leCreateConnectionCancelComplete != NULL) {
        callbacks->leCreateConnectionCancelComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeReadWhiteListSizeFailed(uint8_t status, const void *param)
{
    HciLeReadWhiteListSizeReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leReadWhiteListSizeComplete != NULL) {
        callbacks->leReadWhiteListSizeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeClearWhiteListFailed(uint8_t status, const void *param)
{
    HciLeClearWhiteListReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leClearWhiteListComplete != NULL) {
        callbacks->leClearWhiteListComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeAddDeviceToWhiteListFailed(uint8_t status, const void *param)
{
    HciLeAddDeviceToWhiteListReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leAddDeviceToWhiteListComplete != NULL) {
        callbacks->leAddDeviceToWhiteListComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeRemoveDeviceFromWhiteListFailed(uint8_t status, const void *param)
{
    HciLeRemoveDeviceFromWhiteListReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leRemoveDeviceFromWhiteListComplete != NULL) {
        callbacks->leRemoveDeviceFromWhiteListComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeConnectionUpdateFailed(uint8_t status, const void *param)
{
    HciLeConnectionUpdateCompleteEventParam eventParam = {
        .status = status,
        .connectionHandle = ((HciLeConnectionUpdateParam *)param)->connectionHandle,
        .connLatency = ((HciLeConnectionUpdateParam *)param)->connLatency,
        .supervisionTimeout = ((HciLeConnectionUpdateParam *)param)->supervisionTimeout,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leConnectionUpdateComplete != NULL) {
        callbacks->leConnectionUpdateComplete(&eventParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeSetHostChannelClassificationFailed(uint8_t status, const void *param)
{
    HciLeSetHostChannelClassificationReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetHostChannelClassificationComplete != NULL) {
        callbacks->leSetHostChannelClassificationComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeReadChannelMapFailed(uint8_t status, const void *param)
{
    HciLeReadChannelMapReturnParam returnParam = {
        .status = status,
        .connectionHandle = ((HciLeReadChannelMapParam *)param)->connectionHandle,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leReadChannelMapComplete != NULL) {
        callbacks->leReadChannelMapComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeReadRemoteFeaturesFailed(uint8_t status, const void *param)
{
    HciLeReadRemoteFeaturesCompleteEventParam eventParam = {
        .status = status,
        .connectionHandle = ((HciLeReadRemoteFeaturesParam *)param)->connectionHandle,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leReadRemoteFeaturesComplete != NULL) {
        callbacks->leReadRemoteFeaturesComplete(&eventParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeEncryptFailed(uint8_t status, const void *param)
{
    HciLeEncryptReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leEncryptComplete != NULL) {
        callbacks->leEncryptComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeRandFailed(uint8_t status, const void *param)
{
    HciLeRandReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leRandComplete != NULL) {
        callbacks->leRandComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeStartEncryptionFailed(uint8_t status, const void *param)
{
    HciEncryptionKeyRefreshCompleteEventParam eventParam = {
        .status = status,
        .connectionHandle = ((HciLeStartEncryptionParam *)param)->connectionHandle,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->encryptionKeyRefreshComplete != NULL) {
        callbacks->encryptionKeyRefreshComplete(&eventParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeLongTermKeyRequestReplyFailed(uint8_t status, const void *param)
{
    HciLeLongTermKeyRequestReplyReturnParam returnParam = {
        .status = status,
        .connectionHandle = ((HciLeLongTermKeyRequestReplyParam *)param)->connectionHandle,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leLongTermKeyRequestReplyComplete != NULL) {
        callbacks->leLongTermKeyRequestReplyComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeLongTermKeyRequestNegativeReplyFailed(uint8_t status, const void *param)
{
    HciLeLongTermKeyRequestNegativeReplyReturnParam returnParam = {
        .status = status,
        .connectionHandle = ((HciLeLongTermKeyRequestNegativeReplyParam *)param)->connectionHandle,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leLongTermKeyRequestNegativeReplyComplete != NULL) {
        callbacks->leLongTermKeyRequestNegativeReplyComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeReadSupportedStatesFailed(uint8_t status, const void *param)
{
    HciLeReadSupportedStatesReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leReadSupportedStatesComplete != NULL) {
        callbacks->leReadSupportedStatesComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeReceiverTestFailed(uint8_t status, const void *param)
{
    HciLeReceiverTestReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leReceiverTestComplete != NULL) {
        callbacks->leReceiverTestComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeTransmitterTestFailed(uint8_t status, const void *param)
{
    HciLeTransmitterTestReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leTransmitterTestComplete != NULL) {
        callbacks->leTransmitterTestComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeTestEndFailed(uint8_t status, const void *param)
{
    HciLeTestEndReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leTestEndComplete != NULL) {
        callbacks->leTestEndComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeRemoteConnectionParameterRequestFailed(uint8_t status, const void *param)
{
    HciLeRemoteConnectionParameterRequestReplyReturnParam returnParam = {
        .status = status,
        .connectionHandle = ((HciLeRemoteConnectionParameterRequestReplyParam *)param)->connectionHandle,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leRemoteConnectionParameterRequestReplyComplete != NULL) {
        callbacks->leRemoteConnectionParameterRequestReplyComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeRemoteConnectionParameterRequestNegativeReplyFailed(uint8_t status, const void *param)
{
    HciLeRemoteConnectionParameterRequestNegativeReplyReturnParam returnParam = {
        .status = status,
        .connectionHandle = ((HciLeRemoteConnectionParameterRequestNegativeReplyParam *)param)->connectionHandle,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leRemoteConnectionParameterRequestNegativeReplyComplete != NULL) {
        callbacks->leRemoteConnectionParameterRequestNegativeReplyComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeSetDataLengthFailed(uint8_t status, const void *param)
{
    HciLeSetDataLengthReturnParam returnParam = {
        .status = status,
        .connectionHandle = ((HciLeSetDataLengthReturnParam *)param)->connectionHandle,
    };

    MutexUnlock(HciGetEventCallbackListLock());

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetDataLengthComplete != NULL) {
        callbacks->leSetDataLengthComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeReadSuggestedDefaultDataLengthFailed(uint8_t status, const void *param)
{
    HciLeReadSuggestedDefaultDataLengthReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leReadSuggestedDefaultDataLengthComplete != NULL) {
        callbacks->leReadSuggestedDefaultDataLengthComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeWriteSuggestedDefaultDataLengthFailed(uint8_t status, const void *param)
{
    HciLeWriteSuggestedDefaultDataLengthReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leWriteSuggestedDefaultDataLengthComplete != NULL) {
        callbacks->leWriteSuggestedDefaultDataLengthComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeReadLocalP256PublicKeyFailed(uint8_t status, const void *param)
{
    HciLeReadLocalP256PublicKeyCompleteEventParam eventParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leReadLocalP256PublicKeyComplete != NULL) {
        callbacks->leReadLocalP256PublicKeyComplete(&eventParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeGenerateDhKeyFailed(uint8_t status, const void *param)
{
    HciLeGenerateDHKeyCompleteEventParam eventParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leGenerateDHKeyComplete != NULL) {
        callbacks->leGenerateDHKeyComplete(&eventParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeAddDeviceToResolvingListFailed(uint8_t status, const void *param)
{
    HciLeAddDeviceToResolvingListReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leAddDeviceToResolvingListComplete != NULL) {
        callbacks->leAddDeviceToResolvingListComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeRemoveDeviceFromResolvingListFailed(uint8_t status, const void *param)
{
    HciLeRemoveDeviceFromResolvingListReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leRemoveDeviceFromResolvingListComplete != NULL) {
        callbacks->leRemoveDeviceFromResolvingListComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeClearResolvingListFailed(uint8_t status, const void *param)
{
    HciLeClearResolvingListReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leClearResolvingListComplete != NULL) {
        callbacks->leClearResolvingListComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeReadResolvingListSizeFailed(uint8_t status, const void *param)
{
    HciLeReadResolvingListSizeReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leReadResolvingListSizeComplete != NULL) {
        callbacks->leReadResolvingListSizeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeReadPeerResolvableAddressFailed(uint8_t status, const void *param)
{
    HciLeReadPeerResolvableAddressReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leReadPeerResolvableAddressComplete != NULL) {
        callbacks->leReadPeerResolvableAddressComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeReadLocalResolvableAddressFailed(uint8_t status, const void *param)
{
    HciLeReadLocalResolvableAddressReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leReadLocalResolvableAddressComplete != NULL) {
        callbacks->leReadLocalResolvableAddressComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeSetAddressResolutionEnableFailed(uint8_t status, const void *param)
{
    HciLeSetAddressResolutionEnableReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetAddressResolutionEnableComplete != NULL) {
        callbacks->leSetAddressResolutionEnableComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeSetResolvablePrivateAddressTimeoutFailed(uint8_t status, const void *param)
{
    HciLeSetResolvablePrivateAddressTimeoutReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetResolvablePrivateAddressTimeoutComplete != NULL) {
        callbacks->leSetResolvablePrivateAddressTimeoutComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeReadMaximumDataLengthFailed(uint8_t status, const void *param)
{
    HciLeReadMaximumDataLengthReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leReadMaximumDataLengthComplete != NULL) {
        callbacks->leReadMaximumDataLengthComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeReadPhyFailed(uint8_t status, const void *param)
{
    HciLeReadPhyReturnParam returnParam = {
        .status = status,
        .connectionHandle = ((HciLeReadPhyReturnParam *)param)->connectionHandle,
    };

    MutexUnlock(HciGetEventCallbackListLock());

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leReadPhyComplete != NULL) {
        callbacks->leReadPhyComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeSetDefaultPhyFailed(uint8_t status, const void *param)
{
    HciLeSetDefaultPhyReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetDefaultPhyComplete != NULL) {
        callbacks->leSetDefaultPhyComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeEnhancedReceiverTestFailed(uint8_t status, const void *param)
{
    HciLeEnhancedReceiverTestReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leEnhancedReceiverTestComplete != NULL) {
        callbacks->leEnhancedReceiverTestComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeEnhancedTransmitterTestFailed(uint8_t status, const void *param)
{
    HciLeEnhancedTransmitterTestReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leEnhancedTransmitterTestComplete != NULL) {
        callbacks->leEnhancedTransmitterTestComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeSetAdvertisingSetRandomAddressFailed(uint8_t status, const void *param)
{
    HciLeSetAdvertisingSetRandomAddressReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetAdvertisingSetRandomAddressComplete != NULL) {
        callbacks->leSetAdvertisingSetRandomAddressComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeSetExtendedAdvertisingParametersFailed(uint8_t status, const void *param)
{
    HciLeSetExtendedAdvertisingParametersReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetExtendedAdvertisingParametersComplete != NULL) {
        callbacks->leSetExtendedAdvertisingParametersComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeSetExtendedAdvertisingDataFailed(uint8_t status, const void *param)
{
    HciLeSetExtendedAdvertisingDataReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetExtendedAdvertisingDataComplete != NULL) {
        callbacks->leSetExtendedAdvertisingDataComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeSetExtendedScanResponseDataFailed(uint8_t status, const void *param)
{
    HciLeSetExtendedScanResponseDataReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetExtendedScanResponseDataComplete != NULL) {
        callbacks->leSetExtendedScanResponseDataComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeSetExtendedAdvertisingEnableFailed(uint8_t status, const void *param)
{
    HciLeSetExtendedAdvertisingEnableReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetExtendedAdvertisingEnableComplete != NULL) {
        callbacks->leSetExtendedAdvertisingEnableComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeReadMaximumAdvertisingDataLengthFailed(uint8_t status, const void *param)
{
    HciLeReadMaximumAdvertisingDataLengthReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leReadMaximumAdvertisingDataLengthComplete != NULL) {
        callbacks->leReadMaximumAdvertisingDataLengthComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeReadNumberofSupportedAdvertisingSetsFailed(uint8_t status, const void *param)
{
    HciLeReadNumberofSupportedAdvertisingSetsReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leReadNumberofSupportedAdvertisingSetsComplete != NULL) {
        callbacks->leReadNumberofSupportedAdvertisingSetsComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeRemoveAdvertisingSetFailed(uint8_t status, const void *param)
{
    HciLeRemoveAdvertisingSetReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leRemoveAdvertisingSetComplete != NULL) {
        callbacks->leRemoveAdvertisingSetComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeClearAdvertisingSetsFailed(uint8_t status, const void *param)
{
    HciLeClearAdvertisingSetsReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leClearAdvertisingSetsComplete != NULL) {
        callbacks->leClearAdvertisingSetsComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeSetPeriodicAdvertisingParametersFailed(uint8_t status, const void *param)
{
    HciLeSetPeriodicAdvertisingParametersReturnParameters returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetPeriodicAdvertisingParametersComplete != NULL) {
        callbacks->leSetPeriodicAdvertisingParametersComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeSetPeriodicAdvertisingDataFailed(uint8_t status, const void *param)
{
    HciLeSetPeriodicAdvertisingDataReturnParameters returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetPeriodicAdvertisingDataComplete != NULL) {
        callbacks->leSetPeriodicAdvertisingDataComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeSetPeriodicAdvertisingEnableFailed(uint8_t status, const void *param)
{
    HciLeSetPeriodicAdvertisingEnableReturnParameters returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetPeriodicAdvertisingEnableComplete != NULL) {
        callbacks->leSetPeriodicAdvertisingEnableComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeSetExtendedScanParametersFailed(uint8_t status, const void *param)
{
    HciLeSetExtendedScanParametersReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetExtendedScanParametersComplete != NULL) {
        callbacks->leSetExtendedScanParametersComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeSetExtendedScanEnableFailed(uint8_t status, const void *param)
{
    HciLeSetExtendedScanEnableReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetExtendedScanEnableComplete != NULL) {
        callbacks->leSetExtendedScanEnableComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeExtendedCreateConnectionFailed(uint8_t status, const void *param)
{
    const HciLeExtendedCreateConnectionParam *cmdParam = param;

    HciLeEnhancedConnectionCompleteEventParam eventParam = {
        .status = status,
        .peerAddressType = cmdParam->peerAddressType,
        .peerAddress = cmdParam->peerAddress,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leEnhancedConnectionComplete != NULL) {
        callbacks->leEnhancedConnectionComplete(&eventParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLePeriodicAdvertisingCreateSyncCancelFailed(uint8_t status, const void *param)
{
    HciLePeriodicAdvertisingCreateSyncCancelReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->lePeriodicAdvertisingCreateSyncCancelComplete != NULL) {
        callbacks->lePeriodicAdvertisingCreateSyncCancelComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLePeriodicAdvertisingTerminateSyncFailed(uint8_t status, const void *param)
{
    HciLePeriodicAdvertisingTerminateSyncReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->lePeriodicAdvertisingTerminateSyncComplete != NULL) {
        callbacks->lePeriodicAdvertisingTerminateSyncComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeAddDeviceToPeriodicAdvertiserListFailed(uint8_t status, const void *param)
{
    HciLeAddDeviceToPeriodicAdvertiserListReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leAddDeviceToPeriodicAdvertiserListComplete != NULL) {
        callbacks->leAddDeviceToPeriodicAdvertiserListComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeRemoveDeviceFromPeriodicAdvertiserListFailed(uint8_t status, const void *param)
{
    HciLeRemoveDeviceFromPeriodicAdvertiserListReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leRemoveDeviceFromPeriodicAdvertiserListComplete != NULL) {
        callbacks->leRemoveDeviceFromPeriodicAdvertiserListComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeClearPeriodicAdvertiserListFailed(uint8_t status, const void *param)
{
    HciLeClearPeriodicAdvertiserListReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leClearPeriodicAdvertiserListComplete != NULL) {
        callbacks->leClearPeriodicAdvertiserListComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeReadPeriodicAdvertiserListSizeFailed(uint8_t status, const void *param)
{
    HciLeReadPeriodicAdvertiserListSizeReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leReadPeriodicAdvertiserListSizeComplete != NULL) {
        callbacks->leReadPeriodicAdvertiserListSizeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeReadTransmitPowerFailed(uint8_t status, const void *param)
{
    HciLeReadTransmitPowerReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leReadTransmitPowerComplete != NULL) {
        callbacks->leReadTransmitPowerComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeReadRfPathCompensationFailed(uint8_t status, const void *param)
{
    HciLeReadRfPathCompensationReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leReadRfPathCompensationComplete != NULL) {
        callbacks->leReadRfPathCompensationComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeWriteRfPathCompensationParamFailed(uint8_t status, const void *param)
{
    HciLeWriteRfPathCompensationReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leWriteRfPathCompensationComplete != NULL) {
        callbacks->leWriteRfPathCompensationComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnLeSetPrivacyModeFailed(uint8_t status, const void *param)
{
    HciLeSetPrivacyModeReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetPrivacyModeComplete != NULL) {
        callbacks->leSetPrivacyModeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static HciCmdOnFailedFunc g_funcMap[] = {
    NULL,                                                           // 0x0000
    HciCmdOnLeSetEventMaskFailed,                                   // 0x0001
    HciCmdOnLeReadBufferSizeFailed,                                 // 0x0002
    HciCmdOnLeReadLocalSupportedFeaturesFailed,                     // 0x0003
    NULL,                                                           // 0x0004
    HciCmdOnLeSetRandomAddressFailed,                               // 0x0005
    HciCmdOnLeSetAdvertisingParametersFailed,                       // 0x0006
    HciCmdOnReadAdvertisingChannelTxPowerFailed,                    // 0x0007
    HciCmdOnLeSetAdvertisingDataFailed,                             // 0x0008
    HciCmdOnLeSetScanResponseDataFailed,                            // 0x0009
    HciCmdOnLeSetAdvertisingEnableFailed,                           // 0x000A
    HciCmdOnLeSetScanParametersFailed,                              // 0x000B
    HciCmdOnLeSetScanEnableFailed,                                  // 0x000C
    HciCmdOnLeCreateConnectionFailed,                               // 0x000D
    HciCmdOnLeCreateConnectionCancelFailed,                         // 0x000E
    HciCmdOnLeReadWhiteListSizeFailed,                              // 0x000F
    HciCmdOnLeClearWhiteListFailed,                                 // 0x0010
    HciCmdOnLeAddDeviceToWhiteListFailed,                           // 0x0011
    HciCmdOnLeRemoveDeviceFromWhiteListFailed,                      // 0x0012
    HciCmdOnLeConnectionUpdateFailed,                               // 0x0013
    HciCmdOnLeSetHostChannelClassificationFailed,                   // 0x0014
    HciCmdOnLeReadChannelMapFailed,                                 // 0x0015
    HciCmdOnLeReadRemoteFeaturesFailed,                             // 0x0016
    HciCmdOnLeEncryptFailed,                                        // 0x0017
    HciCmdOnLeRandFailed,                                           // 0x0018
    HciCmdOnLeStartEncryptionFailed,                                // 0x0019
    HciCmdOnLeLongTermKeyRequestReplyFailed,                        // 0x001A
    HciCmdOnLeLongTermKeyRequestNegativeReplyFailed,                // 0x001B
    HciCmdOnLeReadSupportedStatesFailed,                            // 0x001C
    HciCmdOnLeReceiverTestFailed,                                   // 0x001D
    HciCmdOnLeTransmitterTestFailed,                                // 0x001E
    HciCmdOnLeTestEndFailed,                                        // 0x001F
    HciCmdOnLeRemoteConnectionParameterRequestFailed,               // 0x0020
    HciCmdOnLeRemoteConnectionParameterRequestNegativeReplyFailed,  // 0x0021
    HciCmdOnLeSetDataLengthFailed,                                  // 0x0022
    HciCmdOnLeReadSuggestedDefaultDataLengthFailed,                 // 0x0023
    HciCmdOnLeWriteSuggestedDefaultDataLengthFailed,                // 0x0024
    HciCmdOnLeReadLocalP256PublicKeyFailed,                         // 0x0025
    HciCmdOnLeGenerateDhKeyFailed,                                  // 0x0026
    HciCmdOnLeAddDeviceToResolvingListFailed,                       // 0x0027
    HciCmdOnLeRemoveDeviceFromResolvingListFailed,                  // 0x0028
    HciCmdOnLeClearResolvingListFailed,                             // 0x0029
    HciCmdOnLeReadResolvingListSizeFailed,                          // 0x002A
    HciCmdOnLeReadPeerResolvableAddressFailed,                      // 0x002B
    HciCmdOnLeReadLocalResolvableAddressFailed,                     // 0x002C
    HciCmdOnLeSetAddressResolutionEnableFailed,                     // 0x002D
    HciCmdOnLeSetResolvablePrivateAddressTimeoutFailed,             // 0x002E
    HciCmdOnLeReadMaximumDataLengthFailed,                          // 0x002F
    HciCmdOnLeReadPhyFailed,                                        // 0x0030
    HciCmdOnLeSetDefaultPhyFailed,                                  // 0x0031
    NULL,                                                           // 0x0032
    HciCmdOnLeEnhancedReceiverTestFailed,                           // 0x0033
    HciCmdOnLeEnhancedTransmitterTestFailed,                        // 0x0034
    HciCmdOnLeSetAdvertisingSetRandomAddressFailed,                 // 0x0035
    HciCmdOnLeSetExtendedAdvertisingParametersFailed,               // 0x0036
    HciCmdOnLeSetExtendedAdvertisingDataFailed,                     // 0x0037
    HciCmdOnLeSetExtendedScanResponseDataFailed,                    // 0x0038
    HciCmdOnLeSetExtendedAdvertisingEnableFailed,                   // 0x0039
    HciCmdOnLeReadMaximumAdvertisingDataLengthFailed,               // 0x003A
    HciCmdOnLeReadNumberofSupportedAdvertisingSetsFailed,           // 0x003B
    HciCmdOnLeRemoveAdvertisingSetFailed,                           // 0x003C
    HciCmdOnLeClearAdvertisingSetsFailed,                           // 0x003D
    HciCmdOnLeSetPeriodicAdvertisingParametersFailed,               // 0x003E
    HciCmdOnLeSetPeriodicAdvertisingDataFailed,                     // 0x003F
    HciCmdOnLeSetPeriodicAdvertisingEnableFailed,                   // 0x0040
    HciCmdOnLeSetExtendedScanParametersFailed,                      // 0x0041
    HciCmdOnLeSetExtendedScanEnableFailed,                          // 0x0042
    HciCmdOnLeExtendedCreateConnectionFailed,                       // 0x0043
    NULL,                                                           // 0x0044
    HciCmdOnLePeriodicAdvertisingCreateSyncCancelFailed,            // 0x0045
    HciCmdOnLePeriodicAdvertisingTerminateSyncFailed,               // 0x0046
    HciCmdOnLeAddDeviceToPeriodicAdvertiserListFailed,              // 0x0047
    HciCmdOnLeRemoveDeviceFromPeriodicAdvertiserListFailed,         // 0x0048
    HciCmdOnLeClearPeriodicAdvertiserListFailed,                    // 0x0049
    HciCmdOnLeReadPeriodicAdvertiserListSizeFailed,                 // 0x004A
    HciCmdOnLeReadTransmitPowerFailed,                              // 0x004B
    HciCmdOnLeReadRfPathCompensationFailed,                         // 0x004C
    HciCmdOnLeWriteRfPathCompensationParamFailed,                   // 0x004D
    HciCmdOnLeSetPrivacyModeFailed,                                 // 0x004E
};

#define LECONTROLLER_OCF_MAX 0x004E

void HciOnLeControllerCmdFailed(uint16_t opCode, uint8_t status, const void *param)
{
    uint16_t ocf = GET_OCF(opCode);
    if (ocf > LECONTROLLER_OCF_MAX) {
        return;
    }

    HciCmdOnFailedFunc func = g_funcMap[ocf];
    if (func != NULL) {
        func(status, param);
    }
}