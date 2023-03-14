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

#include "hci_evt_link_policy_cmd_complete.h"

#include <securec.h>

#include "platform/include/allocator.h"
#include "platform/include/list.h"
#include "platform/include/mutex.h"

#include "hci/cmd/hci_cmd.h"
#include "hci/hci.h"
#include "hci/hci_def.h"

#include "hci_evt.h"
#include "hci_evt_cmd_complete.h"

static void HciEventOnWriteLinkPolicySettingsComplete(const void *param, uint8_t length)
{
    HciWriteLinkPolicySettingsReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeLinkPolicySettingsComplete != NULL) {
        callbacks->writeLinkPolicySettingsComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnSniffSubratingComplete(const void *param, uint8_t length)
{
    HciSniffSubratingReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->sniffSubratingComplete != NULL) {
        callbacks->sniffSubratingComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnRoleDiscoveryComplete(const void *param, uint8_t length)
{
    HciRoleDiscoveryReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->roleDiscoveryComplete != NULL) {
        callbacks->roleDiscoveryComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadLinkPolicySettingsComplete(const void *param, uint8_t length)
{
    HciReadLinkPolicySettingsReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readLinkPolicySettingsComplete != NULL) {
        callbacks->readLinkPolicySettingsComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnWriteDefaultLinkPolicySettingsComplete(const void *param, uint8_t length)
{
    HciWriteDefaultLinkPolicySettingsReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->writeDefaultLinkPolicySettingsComplete != NULL) {
        callbacks->writeDefaultLinkPolicySettingsComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnReadDefaultLinkPolicySettingsComplete(const void *param, uint8_t length)
{
    HciReadDefaultLinkPolicySettingsReturnParam returnParam = {0};
    (void)memcpy_s(
        &returnParam, sizeof(returnParam), param, (length > sizeof(returnParam)) ? sizeof(returnParam) : length);

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->readDefaultLinkPolicySettingsComplete != NULL) {
        callbacks->readDefaultLinkPolicySettingsComplete(&returnParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static HciEventCommandCompleteFunc g_linkPolicyCommandCompleteMap[] = {
    NULL,                                              // 0x0000
    NULL,                                              // 0x0001
    NULL,                                              // 0x0002
    NULL,                                              // 0x0003
    NULL,                                              // 0x0004
    NULL,                                              // 0x0005
    NULL,                                              // 0x0006
    NULL,                                              // 0x0007
    NULL,                                              // 0x0008
    HciEventOnRoleDiscoveryComplete,                   // 0x0009
    NULL,                                              // 0x000A
    NULL,                                              // 0x000B
    HciEventOnReadLinkPolicySettingsComplete,          // 0x000C
    HciEventOnWriteLinkPolicySettingsComplete,         // 0x000D
    HciEventOnReadDefaultLinkPolicySettingsComplete,   // 0x000E
    HciEventOnWriteDefaultLinkPolicySettingsComplete,  // 0x000F
    NULL,                                              // 0x0010
    HciEventOnSniffSubratingComplete,                  // 0x0011
};

#define LINKPOLICY_OCF_MAX 0x0011

void HciEventOnLinkPolicyCommandComplete(uint16_t opCode, const void *param, uint8_t length)
{
    uint16_t ocf = GET_OCF(opCode);
    if (ocf > LINKPOLICY_OCF_MAX) {
        return;
    }

    HciEventCommandCompleteFunc func = g_linkPolicyCommandCompleteMap[ocf];
    if (func != NULL) {
        func(param, length);
    }
}