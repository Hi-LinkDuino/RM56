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

#include "hci_evt_le_cmd_complete.h"

#include <securec.h>

#include "platform/include/list.h"
#include "platform/include/mutex.h"

#include "hci/hci.h"

#include "hci_evt.h"

typedef void (*HciLeCmdCompleteFunc)(const void *param, uint8_t length);

static void HciEventOnLeSetEventMaskComplete(const void *param, uint8_t length)
{
    HciLeSetEventMaskReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetEventMaskComplete != NULL) {
        callbacks->leSetEventMaskComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeReadBufferSizeComplete(const void *param, uint8_t length)
{
    HciLeReadBufferSizeReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leReadBufferSizeComplete != NULL) {
        callbacks->leReadBufferSizeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeReadLocalSupportedFeaturesComplete(const void *param, uint8_t length)
{
    HciLeReadLocalSupportedFeaturesReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leReadLocalSupportedFeaturesComplete != NULL) {
        callbacks->leReadLocalSupportedFeaturesComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeSetRandomAddressComplete(const void *param, uint8_t length)
{
    HciLeSetRandomAddressReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetRandomAddressComplete != NULL) {
        callbacks->leSetRandomAddressComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeSetAdvertisingParametersComplete(const void *param, uint8_t length)
{
    HciLeSetAdvertisingParametersReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetAdvertisingParametersComplete != NULL) {
        callbacks->leSetAdvertisingParametersComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeReadAdvertisingChannelTxPowerComplete(const void *param, uint8_t length)
{
    HciLeReadAdvertisingChannelTxPowerReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leReadAdvertisingChannelTxPowerComplete != NULL) {
        callbacks->leReadAdvertisingChannelTxPowerComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeSetAdvertisingDataComplete(const void *param, uint8_t length)
{
    HciLeSetAdvertisingDataReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetAdvertisingDataComplete != NULL) {
        callbacks->leSetAdvertisingDataComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeSetScanResponseDataComplete(const void *param, uint8_t length)
{
    HciLeSetScanResponseDataReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetScanResponseDataComplete != NULL) {
        callbacks->leSetScanResponseDataComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeSetAdvertisingEnableComplete(const void *param, uint8_t length)
{
    HciLeSetAdvertisingEnableReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetAdvertisingEnableComplete != NULL) {
        callbacks->leSetAdvertisingEnableComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeSetScanParametersComplete(const void *param, uint8_t length)
{
    HciLeSetScanParametersReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetScanParametersComplete != NULL) {
        callbacks->leSetScanParametersComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeSetScanEnableComplete(const void *param, uint8_t length)
{
    HciLeSetScanEnableReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetScanEnableComplete != NULL) {
        callbacks->leSetScanEnableComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeCreateConnectionCancelComplete(const void *param, uint8_t length)
{
    HciLeCreateConnectionCancelReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leCreateConnectionCancelComplete != NULL) {
        callbacks->leCreateConnectionCancelComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeReadWhiteListSizeComplete(const void *param, uint8_t length)
{
    HciLeReadWhiteListSizeReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leReadWhiteListSizeComplete != NULL) {
        callbacks->leReadWhiteListSizeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeClearWhiteListComplete(const void *param, uint8_t length)
{
    HciLeClearWhiteListReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leClearWhiteListComplete != NULL) {
        callbacks->leClearWhiteListComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeAddDeviceToWhiteListComplete(const void *param, uint8_t length)
{
    HciLeAddDeviceToWhiteListReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leAddDeviceToWhiteListComplete != NULL) {
        callbacks->leAddDeviceToWhiteListComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeRemoveDeviceFromWhiteListComplete(const void *param, uint8_t length)
{
    HciLeRemoveDeviceFromWhiteListReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leRemoveDeviceFromWhiteListComplete != NULL) {
        callbacks->leRemoveDeviceFromWhiteListComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeSetHostChannelClassificationComplete(const void *param, uint8_t length)
{
    HciLeSetHostChannelClassificationReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetHostChannelClassificationComplete != NULL) {
        callbacks->leSetHostChannelClassificationComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeReadChannelMapComplete(const void *param, uint8_t length)
{
    HciLeReadChannelMapReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leReadChannelMapComplete != NULL) {
        callbacks->leReadChannelMapComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeEncryptComplete(const void *param, uint8_t length)
{
    HciLeEncryptReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leEncryptComplete != NULL) {
        callbacks->leEncryptComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeRandComplete(const void *param, uint8_t length)
{
    HciLeRandReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leRandComplete != NULL) {
        callbacks->leRandComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeLongTermKeyRequestReplyComplete(const void *param, uint8_t length)
{
    HciLeLongTermKeyRequestReplyReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leLongTermKeyRequestReplyComplete != NULL) {
        callbacks->leLongTermKeyRequestReplyComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeLongTermKeyRequestNegativeReplyComplete(const void *param, uint8_t length)
{
    HciLeLongTermKeyRequestNegativeReplyReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leLongTermKeyRequestNegativeReplyComplete != NULL) {
        callbacks->leLongTermKeyRequestNegativeReplyComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeRemoteConnectionParameterRequestReplyComplete(const void *param, uint8_t length)
{
    HciLeRemoteConnectionParameterRequestReplyReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leRemoteConnectionParameterRequestReplyComplete != NULL) {
        callbacks->leRemoteConnectionParameterRequestReplyComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeRemoteConnectionParameterRequestNegativeReplyComplete(const void *param, uint8_t length)
{
    HciLeRemoteConnectionParameterRequestNegativeReplyReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leRemoteConnectionParameterRequestNegativeReplyComplete != NULL) {
        callbacks->leRemoteConnectionParameterRequestNegativeReplyComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeAddDeviceToResolvingListComplete(const void *param, uint8_t length)
{
    HciLeAddDeviceToResolvingListReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leAddDeviceToResolvingListComplete != NULL) {
        callbacks->leAddDeviceToResolvingListComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeRemoveDeviceFromResolvingListComplete(const void *param, uint8_t length)
{
    HciLeRemoveDeviceFromResolvingListReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leRemoveDeviceFromResolvingListComplete != NULL) {
        callbacks->leRemoveDeviceFromResolvingListComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeClearResolvingListComplete(const void *param, uint8_t length)
{
    HciLeClearResolvingListReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leClearResolvingListComplete != NULL) {
        callbacks->leClearResolvingListComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeReadResolvingListSizeComplete(const void *param, uint8_t length)
{
    HciLeReadResolvingListSizeReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leReadResolvingListSizeComplete != NULL) {
        callbacks->leReadResolvingListSizeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeReadPeerResolvableAddressComplete(const void *param, uint8_t length)
{
    HciLeReadPeerResolvableAddressReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leReadPeerResolvableAddressComplete != NULL) {
        callbacks->leReadPeerResolvableAddressComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeReadLocalResolvableAddressComplete(const void *param, uint8_t length)
{
    HciLeReadLocalResolvableAddressReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leReadLocalResolvableAddressComplete != NULL) {
        callbacks->leReadLocalResolvableAddressComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeSetAddressResolutionEnableComplete(const void *param, uint8_t length)
{
    HciLeSetAddressResolutionEnableReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetAddressResolutionEnableComplete != NULL) {
        callbacks->leSetAddressResolutionEnableComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnSetResolvablePrivateAddressTimeoutComplete(const void *param, uint8_t length)
{
    HciLeSetResolvablePrivateAddressTimeoutReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetResolvablePrivateAddressTimeoutComplete != NULL) {
        callbacks->leSetResolvablePrivateAddressTimeoutComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeSetAdvertisingSetRandomAddressComplete(const void *param, uint8_t length)
{
    HciLeSetAdvertisingSetRandomAddressReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetAdvertisingSetRandomAddressComplete != NULL) {
        callbacks->leSetAdvertisingSetRandomAddressComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeSetExtendedAdvertisingParametersComplete(const void *param, uint8_t length)
{
    HciLeSetExtendedAdvertisingParametersReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetExtendedAdvertisingParametersComplete != NULL) {
        callbacks->leSetExtendedAdvertisingParametersComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeSetExtendedAdvertisingDataComplete(const void *param, uint8_t length)
{
    HciLeSetExtendedAdvertisingDataReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetExtendedAdvertisingDataComplete != NULL) {
        callbacks->leSetExtendedAdvertisingDataComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeSetExtendedScanResponseDataComplete(const void *param, uint8_t length)
{
    HciLeSetExtendedScanResponseDataReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetExtendedScanResponseDataComplete != NULL) {
        callbacks->leSetExtendedScanResponseDataComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeSetExtendedAdvertisingEnableComplete(const void *param, uint8_t length)
{
    HciLeSetExtendedAdvertisingEnableReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetExtendedAdvertisingEnableComplete != NULL) {
        callbacks->leSetExtendedAdvertisingEnableComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeReadMaximumAdvertisingDataLengthComplete(const void *param, uint8_t length)
{
    HciLeReadMaximumAdvertisingDataLengthReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leReadMaximumAdvertisingDataLengthComplete != NULL) {
        callbacks->leReadMaximumAdvertisingDataLengthComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeReadNumberofSupportedAdvertisingSetsComplete(const void *param, uint8_t length)
{
    HciLeReadNumberofSupportedAdvertisingSetsReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leReadNumberofSupportedAdvertisingSetsComplete != NULL) {
        callbacks->leReadNumberofSupportedAdvertisingSetsComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeRemoveAdvertisingSetComplete(const void *param, uint8_t length)
{
    HciLeRemoveAdvertisingSetReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leRemoveAdvertisingSetComplete != NULL) {
        callbacks->leRemoveAdvertisingSetComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeClearAdvertisingSetsComplete(const void *param, uint8_t length)
{
    HciLeClearAdvertisingSetsReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leClearAdvertisingSetsComplete != NULL) {
        callbacks->leClearAdvertisingSetsComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeSetExtendedScanParametersComplete(const void *param, uint8_t length)
{
    HciLeClearAdvertisingSetsReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetExtendedScanParametersComplete != NULL) {
        callbacks->leSetExtendedScanParametersComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeSetExtendedScanEnableComplete(const void *param, uint8_t length)
{
    HciLeSetExtendedScanEnableReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetExtendedScanEnableComplete != NULL) {
        callbacks->leSetExtendedScanEnableComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeReadSupportedStatesComplete(const void *param, uint8_t length)
{
    HciLeReadSupportedStatesReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leReadSupportedStatesComplete != NULL) {
        callbacks->leReadSupportedStatesComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeReceiverTestComplete(const void *param, uint8_t length)
{
    HciLeReceiverTestReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leReceiverTestComplete != NULL) {
        callbacks->leReceiverTestComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeTransmitterTestComplete(const void *param, uint8_t length)
{
    HciLeTransmitterTestReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leTransmitterTestComplete != NULL) {
        callbacks->leTransmitterTestComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeTestEndComplete(const void *param, uint8_t length)
{
    HciLeTestEndReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leTestEndComplete != NULL) {
        callbacks->leTestEndComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeSetDataLengthComplete(const void *param, uint8_t length)
{
    HciLeSetDataLengthReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetDataLengthComplete != NULL) {
        callbacks->leSetDataLengthComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeReadSuggestedDefaultDataLengthComplete(const void *param, uint8_t length)
{
    HciLeReadSuggestedDefaultDataLengthReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leReadSuggestedDefaultDataLengthComplete != NULL) {
        callbacks->leReadSuggestedDefaultDataLengthComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeWriteSuggestedDefaultDataLengthComplete(const void *param, uint8_t length)
{
    HciLeWriteSuggestedDefaultDataLengthReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leWriteSuggestedDefaultDataLengthComplete != NULL) {
        callbacks->leWriteSuggestedDefaultDataLengthComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeReadMaximumDataLengthComplete(const void *param, uint8_t length)
{
    HciLeReadMaximumDataLengthReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leReadMaximumDataLengthComplete != NULL) {
        callbacks->leReadMaximumDataLengthComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeReadPhyComplete(const void *param, uint8_t length)
{
    HciLeReadPhyReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leReadPhyComplete != NULL) {
        callbacks->leReadPhyComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeSetDefaultPhyComplete(const void *param, uint8_t length)
{
    HciLeSetDefaultPhyReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetDefaultPhyComplete != NULL) {
        callbacks->leSetDefaultPhyComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeEnhancedReceiverTestComplete(const void *param, uint8_t length)
{
    HciLeEnhancedReceiverTestReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leEnhancedReceiverTestComplete != NULL) {
        callbacks->leEnhancedReceiverTestComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeEnhancedTransmitterTestComplete(const void *param, uint8_t length)
{
    HciLeEnhancedTransmitterTestReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leEnhancedTransmitterTestComplete != NULL) {
        callbacks->leEnhancedTransmitterTestComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeSetPeriodicAdvertisingParametersComplete(const void *param, uint8_t length)
{
    HciLeSetPeriodicAdvertisingParametersReturnParameters returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetPeriodicAdvertisingParametersComplete != NULL) {
        callbacks->leSetPeriodicAdvertisingParametersComplete(param);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeSetPeriodicAdvertisingDataComplete(const void *param, uint8_t length)
{
    HciLeSetPeriodicAdvertisingDataReturnParameters returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetPeriodicAdvertisingDataComplete != NULL) {
        callbacks->leSetPeriodicAdvertisingDataComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeSetPeriodicAdvertisingEnableComplete(const void *param, uint8_t length)
{
    HciLeSetPeriodicAdvertisingEnableReturnParameters returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetPeriodicAdvertisingEnableComplete != NULL) {
        callbacks->leSetPeriodicAdvertisingEnableComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLePeriodicAdvertisingCreateSyncCancelComplete(const void *param, uint8_t length)
{
    HciLePeriodicAdvertisingCreateSyncCancelReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->lePeriodicAdvertisingCreateSyncCancelComplete != NULL) {
        callbacks->lePeriodicAdvertisingCreateSyncCancelComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLePeriodicAdvertisingTerminateSyncComplete(const void *param, uint8_t length)
{
    HciLePeriodicAdvertisingTerminateSyncReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->lePeriodicAdvertisingTerminateSyncComplete != NULL) {
        callbacks->lePeriodicAdvertisingTerminateSyncComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeAddDeviceToPeriodicAdvertiserListComplete(const void *param, uint8_t length)
{
    HciLeAddDeviceToPeriodicAdvertiserListReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leAddDeviceToPeriodicAdvertiserListComplete != NULL) {
        callbacks->leAddDeviceToPeriodicAdvertiserListComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeRemoveDeviceFromPeriodicAdvertiserListComplete(const void *param, uint8_t length)
{
    HciLeRemoveDeviceFromPeriodicAdvertiserListReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leRemoveDeviceFromPeriodicAdvertiserListComplete != NULL) {
        callbacks->leRemoveDeviceFromPeriodicAdvertiserListComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeClearPeriodicAdvertiserListComplete(const void *param, uint8_t length)
{
    HciLeClearPeriodicAdvertiserListReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leClearPeriodicAdvertiserListComplete != NULL) {
        callbacks->leClearPeriodicAdvertiserListComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeReadPeriodicAdvertiserListSizeComplete(const void *param, uint8_t length)
{
    HciLeReadPeriodicAdvertiserListSizeReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leReadPeriodicAdvertiserListSizeComplete != NULL) {
        callbacks->leReadPeriodicAdvertiserListSizeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeReadTransmitPowerComplete(const void *param, uint8_t length)
{
    HciLeReadTransmitPowerReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leReadTransmitPowerComplete != NULL) {
        callbacks->leReadTransmitPowerComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeReadRFPathCompensationComplete(const void *param, uint8_t length)
{
    HciLeReadRfPathCompensationReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leReadRfPathCompensationComplete != NULL) {
        callbacks->leReadRfPathCompensationComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeWriteRFPathCompensationComplete(const void *param, uint8_t length)
{
    HciLeWriteRfPathCompensationReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leWriteRfPathCompensationComplete != NULL) {
        callbacks->leWriteRfPathCompensationComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeSetPrivacyModeComplete(const void *param, uint8_t length)
{
    HciLeSetPrivacyModeReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leSetPrivacyModeComplete != NULL) {
        callbacks->leSetPrivacyModeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static HciLeCmdCompleteFunc g_leControllerCommandCompleteMap[] = {
    NULL,                                                               // 0x0000
    HciEventOnLeSetEventMaskComplete,                                   // 0x0001
    HciEventOnLeReadBufferSizeComplete,                                 // 0x0002
    HciEventOnLeReadLocalSupportedFeaturesComplete,                     // 0x0003
    NULL,                                                               // 0x0004
    HciEventOnLeSetRandomAddressComplete,                               // 0x0005
    HciEventOnLeSetAdvertisingParametersComplete,                       // 0x0006
    HciEventOnLeReadAdvertisingChannelTxPowerComplete,                  // 0x0007
    HciEventOnLeSetAdvertisingDataComplete,                             // 0x0008
    HciEventOnLeSetScanResponseDataComplete,                            // 0x0009
    HciEventOnLeSetAdvertisingEnableComplete,                           // 0x000A
    HciEventOnLeSetScanParametersComplete,                              // 0x000B
    HciEventOnLeSetScanEnableComplete,                                  // 0x000C
    NULL,                                                               // 0x000D
    HciEventOnLeCreateConnectionCancelComplete,                         // 0x000E
    HciEventOnLeReadWhiteListSizeComplete,                              // 0x000F
    HciEventOnLeClearWhiteListComplete,                                 // 0x0010
    HciEventOnLeAddDeviceToWhiteListComplete,                           // 0x0011
    HciEventOnLeRemoveDeviceFromWhiteListComplete,                      // 0x0012
    NULL,                                                               // 0x0013
    HciEventOnLeSetHostChannelClassificationComplete,                   // 0x0014
    HciEventOnLeReadChannelMapComplete,                                 // 0x0015
    NULL,                                                               // 0x0016
    HciEventOnLeEncryptComplete,                                        // 0x0017
    HciEventOnLeRandComplete,                                           // 0x0018
    NULL,                                                               // 0x0019
    HciEventOnLeLongTermKeyRequestReplyComplete,                        // 0x001A
    HciEventOnLeLongTermKeyRequestNegativeReplyComplete,                // 0x001B
    HciEventOnLeReadSupportedStatesComplete,                            // 0x001C
    HciEventOnLeReceiverTestComplete,                                   // 0x001D
    HciEventOnLeTransmitterTestComplete,                                // 0x001E
    HciEventOnLeTestEndComplete,                                        // 0x001F
    HciEventOnLeRemoteConnectionParameterRequestReplyComplete,          // 0x0020
    HciEventOnLeRemoteConnectionParameterRequestNegativeReplyComplete,  // 0x0021
    HciEventOnLeSetDataLengthComplete,                                  // 0x0022
    HciEventOnLeReadSuggestedDefaultDataLengthComplete,                 // 0x0023
    HciEventOnLeWriteSuggestedDefaultDataLengthComplete,                // 0x0024
    NULL,                                                               // 0x0025
    NULL,                                                               // 0x0026
    HciEventOnLeAddDeviceToResolvingListComplete,                       // 0x0027
    HciEventOnLeRemoveDeviceFromResolvingListComplete,                  // 0x0028
    HciEventOnLeClearResolvingListComplete,                             // 0x0029
    HciEventOnLeReadResolvingListSizeComplete,                          // 0x002A
    HciEventOnLeReadPeerResolvableAddressComplete,                      // 0x002B
    HciEventOnLeReadLocalResolvableAddressComplete,                     // 0x002C
    HciEventOnLeSetAddressResolutionEnableComplete,                     // 0x002D
    HciEventOnSetResolvablePrivateAddressTimeoutComplete,               // 0x002E
    HciEventOnLeReadMaximumDataLengthComplete,                          // 0x002F
    HciEventOnLeReadPhyComplete,                                        // 0x0030
    HciEventOnLeSetDefaultPhyComplete,                                  // 0x0031
    NULL,                                                               // 0x0032
    HciEventOnLeEnhancedReceiverTestComplete,                           // 0x0033
    HciEventOnLeEnhancedTransmitterTestComplete,                        // 0x0034
    HciEventOnLeSetAdvertisingSetRandomAddressComplete,                 // 0x0035
    HciEventOnLeSetExtendedAdvertisingParametersComplete,               // 0x0036
    HciEventOnLeSetExtendedAdvertisingDataComplete,                     // 0x0037
    HciEventOnLeSetExtendedScanResponseDataComplete,                    // 0x0038
    HciEventOnLeSetExtendedAdvertisingEnableComplete,                   // 0x0039
    HciEventOnLeReadMaximumAdvertisingDataLengthComplete,               // 0x003A
    HciEventOnLeReadNumberofSupportedAdvertisingSetsComplete,           // 0x003B
    HciEventOnLeRemoveAdvertisingSetComplete,                           // 0x003C
    HciEventOnLeClearAdvertisingSetsComplete,                           // 0x003D
    HciEventOnLeSetPeriodicAdvertisingParametersComplete,               // 0x003E
    HciEventOnLeSetPeriodicAdvertisingDataComplete,                     // 0x003F
    HciEventOnLeSetPeriodicAdvertisingEnableComplete,                   // 0x0040
    HciEventOnLeSetExtendedScanParametersComplete,                      // 0x0041
    HciEventOnLeSetExtendedScanEnableComplete,                          // 0x0042
    NULL,                                                               // 0x0043
    NULL,                                                               // 0x0044
    HciEventOnLePeriodicAdvertisingCreateSyncCancelComplete,            // 0x0045
    HciEventOnLePeriodicAdvertisingTerminateSyncComplete,               // 0x0046
    HciEventOnLeAddDeviceToPeriodicAdvertiserListComplete,              // 0x0047
    HciEventOnLeRemoveDeviceFromPeriodicAdvertiserListComplete,         // 0x0048
    HciEventOnLeClearPeriodicAdvertiserListComplete,                    // 0x0049
    HciEventOnLeReadPeriodicAdvertiserListSizeComplete,                 // 0x004A
    HciEventOnLeReadTransmitPowerComplete,                              // 0x004B
    HciEventOnLeReadRFPathCompensationComplete,                         // 0x004C
    HciEventOnLeWriteRFPathCompensationComplete,                        // 0x004D
    HciEventOnLeSetPrivacyModeComplete,                                 // 0x004E
};

#define LECONTROLLER_OCF_MAX 0x004E

void HciEventOnLeCommandComplete(uint16_t opCode, const void *param, uint8_t length)
{
    uint16_t ocf = GET_OCF(opCode);
    if (ocf > LECONTROLLER_OCF_MAX) {
        return;
    }

    HciLeCmdCompleteFunc func = g_leControllerCommandCompleteMap[ocf];
    if (func != NULL) {
        func(param, length);
    }
}