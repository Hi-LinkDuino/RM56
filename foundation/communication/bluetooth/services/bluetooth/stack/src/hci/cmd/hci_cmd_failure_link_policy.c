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

#include "hci_cmd_failure_link_policy.h"

#include "btstack.h"
#include "platform/include/list.h"

#include "hci/evt/hci_evt.h"
#include "hci/hci.h"
#include "hci/hci_def.h"

#include "hci_cmd_failure.h"

static void HciCmdOnSniffModeFailed(uint8_t status, const void *param)
{
    HciModeChangeEventParam eventParam = {
        .status = status,
        .connectionHandle = ((HciSniffModeParam *)param)->connectionHandle,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->modeChange != NULL) {
        callbacks->modeChange(&eventParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnExitSnifModeFailed(uint8_t status, const void *param)
{
    HciModeChangeEventParam eventParam = {
        .status = status,
        .connectionHandle = ((HciExitSniffModeParam *)param)->connectionHandle,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->modeChange != NULL) {
        callbacks->modeChange(&eventParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnSniffSubratingFailed(uint8_t status, const void *param)
{
    HciSniffSubratingReturnParam returnParam = {
        .status = status,
        .connectionHandle = ((HciSniffSubratingParam *)param)->connectionHandle,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->sniffSubratingComplete != NULL) {
        callbacks->sniffSubratingComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnWriteLinkPolicySettingsFailed(uint8_t status, const void *param)
{
    HciWriteLinkPolicySettingsReturnParam returnParam = {
        .status = status,
        .connectionHandle = ((HciWriteLinkPolicySettingsReturnParam *)param)->connectionHandle,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeLinkPolicySettingsComplete != NULL) {
        callbacks->writeLinkPolicySettingsComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciCmdOnWriteDefaultLinkPolicySettingsFailed(uint8_t status, const void *param)
{
    HciWriteDefaultLinkPolicySettingsReturnParam returnParam = {
        .status = status,
    };

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeDefaultLinkPolicySettingsComplete != NULL) {
        callbacks->writeDefaultLinkPolicySettingsComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static HciCmdOnFailedFunc g_funcMap[] = {
    NULL,                                          // 0x0000
    NULL,                                          // 0x0001
    NULL,                                          // 0x0002
    HciCmdOnSniffModeFailed,                       // 0x0003
    HciCmdOnExitSnifModeFailed,                    // 0x0004
    NULL,                                          // 0x0005
    NULL,                                          // 0x0006
    NULL,                                          // 0x0007
    NULL,                                          // 0x0008
    NULL,                                          // 0x0009
    NULL,                                          // 0x000A
    NULL,                                          // 0x000B
    NULL,                                          // 0x000C
    HciCmdOnWriteLinkPolicySettingsFailed,         // 0x000D
    NULL,                                          // 0x000E
    HciCmdOnWriteDefaultLinkPolicySettingsFailed,  // 0x000F
    NULL,                                          // 0x0010
    HciCmdOnSniffSubratingFailed,                  // 0x0011
};

#define LINKPOLICY_OCF_MAX 0x0011

void HciOnLinkPolicyCmdFailed(uint16_t opCode, uint8_t status, const void *param)
{
    uint16_t ocf = GET_OCF(opCode);
    if (ocf > LINKPOLICY_OCF_MAX) {
        return;
    }

    HciCmdOnFailedFunc func = g_funcMap[ocf];
    if (func != NULL) {
        func(status, param);
    }
}