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

#include "hci_evt_info_params_cmd_complete.h"

#include <securec.h>

#include "platform/include/allocator.h"
#include "platform/include/list.h"
#include "platform/include/mutex.h"

#include "hci/cmd/hci_cmd.h"
#include "hci/hci.h"
#include "hci/hci_def.h"

#include "hci_evt.h"
#include "hci_evt_cmd_complete.h"

static void HciEventOnReadLocalVersionComplete(const void *param, uint8_t length)
{
    HciReadLocalVersionInformationReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readLocalVersionInformationComplete != NULL) {
        callbacks->readLocalVersionInformationComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadLocalSupportedCommandsComplete(const void *param, uint8_t length)
{
    HciReadLocalSupportedCommandsReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readLocalSupportedCommandsComplete != NULL) {
        callbacks->readLocalSupportedCommandsComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadLocalSupportedFeaturesComplete(const void *param, uint8_t length)
{
    HciReadLocalSupportedFeaturesReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readLocalSupportedFeaturesComplete != NULL) {
        callbacks->readLocalSupportedFeaturesComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadLocalExtendedFeaturesComplete(const void *param, uint8_t length)
{
    HciReadLocalExtendedFeaturesReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readLocalExtendedFeaturesComplete != NULL) {
        callbacks->readLocalExtendedFeaturesComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadBufferSizeComplete(const void *param, uint8_t length)
{
    HciReadBufferSizeReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readBufferSizeComplete != NULL) {
        callbacks->readBufferSizeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadBdAddr(const void *param, uint8_t length)
{
    HciReadBdAddrReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readBdAddrComplete != NULL) {
        callbacks->readBdAddrComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCopySupportedCodecs(uint8_t *supportedCodecs, uint8_t count, const uint8_t *buf)
{
    int offset = 0;
    for (int i = 0; i < count; i++) {
        supportedCodecs[i] = buf[offset];
        offset += sizeof(uint8_t);
    }
}

static void HciCopySupportedVendorSpecificCodecs(
    HciVendorSpecificCodec *vendorSpecificCodecs, uint8_t count, const uint8_t *buf)
{
    int offset = 0;
    for (int i = 0; i < count; i++) {
        if (memcpy_s(&vendorSpecificCodecs[i].companyID, sizeof(uint16_t), buf + offset, sizeof(uint16_t)) != EOK) {
            return;
        }
        offset += sizeof(uint16_t);
        if (memcpy_s(
            &vendorSpecificCodecs[i].vendorDefinedCodecID, sizeof(uint16_t), buf + offset, sizeof(uint16_t)) != EOK) {
            return;
        }
        offset += sizeof(uint16_t);
    }
}

static void HciEventOnReadLocalSupportedCodecs(const void *param, uint8_t length)
{
    uint8_t *buf = (uint8_t *)param;
    HciReadLocalSupportedCodecsReturnParam returnParam = {0};
    int offset = 0;
    returnParam.status = buf[offset];
    offset += sizeof(uint8_t);

    if (offset + sizeof(uint8_t) <= length) {
        returnParam.numberOfSupportedCodecs = buf[offset];
        offset += sizeof(uint8_t);
    }

    if (returnParam.numberOfSupportedCodecs) {
        if (offset + returnParam.numberOfSupportedCodecs * sizeof(uint8_t) <= length) {
            returnParam.supportedCodecs = MEM_MALLOC.alloc(returnParam.numberOfSupportedCodecs * sizeof(uint8_t));
            if (returnParam.supportedCodecs != NULL) {
                HciCopySupportedCodecs(returnParam.supportedCodecs, returnParam.numberOfSupportedCodecs, buf + offset);
            }
            offset += returnParam.numberOfSupportedCodecs * sizeof(uint8_t);
        } else {
            returnParam.numberOfSupportedCodecs = 0;
            offset = length;
        }
    }

    if (offset + sizeof(uint8_t) <= length) {
        returnParam.numberOfSupportedVendorSpecificCodecs = buf[offset];
        offset += sizeof(uint8_t);
    }

    if (returnParam.numberOfSupportedVendorSpecificCodecs &&
        (offset + returnParam.numberOfSupportedVendorSpecificCodecs * sizeof(HciVendorSpecificCodec) <= length)) {
        returnParam.vendorSpecificCodecs =
            MEM_MALLOC.alloc(returnParam.numberOfSupportedVendorSpecificCodecs * sizeof(HciVendorSpecificCodec));
        if (returnParam.vendorSpecificCodecs != NULL) {
            HciCopySupportedVendorSpecificCodecs(
                returnParam.vendorSpecificCodecs, returnParam.numberOfSupportedVendorSpecificCodecs, buf + offset);
        }
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readLocalSupportedCodecsComplete != NULL) {
        callbacks->readLocalSupportedCodecsComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;

    if (returnParam.supportedCodecs != NULL) {
        MEM_MALLOC.free(returnParam.supportedCodecs);
    }
    if (returnParam.vendorSpecificCodecs != NULL) {
        MEM_MALLOC.free(returnParam.vendorSpecificCodecs);
    }
}

static HciEventCommandCompleteFunc g_informationalParametersCommandCompleteMap[] = {
    NULL,                                          // 0x0000
    HciEventOnReadLocalVersionComplete,            // 0x0001
    HciEventOnReadLocalSupportedCommandsComplete,  // 0x0002
    HciEventOnReadLocalSupportedFeaturesComplete,  // 0x0003
    HciEventOnReadLocalExtendedFeaturesComplete,   // 0x0004
    HciEventOnReadBufferSizeComplete,              // 0x0005
    NULL,                                          // 0x0006
    NULL,                                          // 0x0007
    NULL,                                          // 0x0008
    HciEventOnReadBdAddr,                          // 0x0009
    NULL,                                          // 0x000A
    HciEventOnReadLocalSupportedCodecs,            // 0x000B
};

#define INFORMATIONALPARAMETERS_OCF_MAX 0x000B

void HciEventOnInformationalParametersCommandComplete(uint16_t opCode, const void *param, uint8_t length)
{
    uint16_t ocf = GET_OCF(opCode);
    if (ocf > INFORMATIONALPARAMETERS_OCF_MAX) {
        return;
    }

    HciEventCommandCompleteFunc func = g_informationalParametersCommandCompleteMap[ocf];
    if (func != NULL) {
        func(param, length);
    }
}