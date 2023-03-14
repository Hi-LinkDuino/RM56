
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

#include "hci_cmd_failure_info_params.h"

#include "btstack.h"
#include "platform/include/list.h"

#include "hci/evt/hci_evt.h"
#include "hci/hci.h"
#include "hci/hci_def.h"

#include "hci_cmd_failure.h"

static void HciCmdOnReadLocalVersionInformationFailed(uint8_t status, const void *param)
{
    HciReadLocalVersionInformationReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readLocalVersionInformationComplete != NULL) {
        callbacks->readLocalVersionInformationComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadLocalSupportedCommandsFailed(uint8_t status, const void *param)
{
    HciReadLocalSupportedCommandsReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readLocalSupportedCommandsComplete != NULL) {
        callbacks->readLocalSupportedCommandsComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadLocalSupportedFeaturesFailed(uint8_t status, const void *param)
{
    HciReadLocalSupportedFeaturesReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readLocalSupportedFeaturesComplete != NULL) {
        callbacks->readLocalSupportedFeaturesComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadLocalExtendedFeaturesFailed(uint8_t status, const void *param)
{
    HciReadLocalExtendedFeaturesReturnParam returnParam = {
        .status = status,
        .pageNumber = ((HciReadLocalExtendedFeaturesParam *)param)->pageNumber,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readLocalExtendedFeaturesComplete != NULL) {
        callbacks->readLocalExtendedFeaturesComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadBufferSizeFailed(uint8_t status, const void *param)
{
    HciReadBufferSizeReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readBufferSizeComplete != NULL) {
        callbacks->readBufferSizeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadBdAddrFailed(uint8_t status, const void *param)
{
    HciReadBdAddrReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readBdAddrComplete != NULL) {
        callbacks->readBdAddrComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadDataBlockSizeFailed(uint8_t status, const void *param)
{
    HciReadDataBlockSizeReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readDataBlockSizeComplete != NULL) {
        callbacks->readDataBlockSizeComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnReadLocalSupportedCodecsFailed(uint8_t status, const void *param)
{
    HciReadLocalSupportedCodecsReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readLocalSupportedCodecsComplete != NULL) {
        callbacks->readLocalSupportedCodecsComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static HciCmdOnFailedFunc g_funcMap[] = {
    NULL,                                       // 0x0000
    HciCmdOnReadLocalVersionInformationFailed,  // 0x0001
    HciCmdOnReadLocalSupportedCommandsFailed,   // 0x0002
    HciCmdOnReadLocalSupportedFeaturesFailed,   // 0x0003
    HciCmdOnReadLocalExtendedFeaturesFailed,    // 0x0004
    HciCmdOnReadBufferSizeFailed,               // 0x0005
    NULL,                                       // 0x0006
    NULL,                                       // 0x0007
    NULL,                                       // 0x0008
    HciCmdOnReadBdAddrFailed,                   // 0x0009
    HciCmdOnReadDataBlockSizeFailed,            // 0x000A
    HciCmdOnReadLocalSupportedCodecsFailed,     // 0x000B
};

#define INNFORMATIONALPARAMETERS_OCF_MAX 0x000B

void HciOnInformationalParametersCmdFailed(uint16_t opCode, uint8_t status, const void *param)
{
    uint16_t ocf = GET_OCF(opCode);
    if (ocf > INNFORMATIONALPARAMETERS_OCF_MAX) {
        return;
    }

    HciCmdOnFailedFunc func = g_funcMap[ocf];
    if (func != NULL) {
        func(status, param);
    }
}