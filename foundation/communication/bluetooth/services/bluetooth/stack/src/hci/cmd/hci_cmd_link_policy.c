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

#include "hci/hci.h"

#include "btstack.h"

#include "hci_cmd.h"

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.2.2 Sniff Mode Command
int HCI_SniffMode(const HciSniffModeParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(HCI_SNIFF_MODE, (void *)param, sizeof(HciSniffModeParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.2.3 Exit Sniff Mode Command
int HCI_ExitSniffMode(const HciExitSniffModeParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(HCI_EXIT_SNIFF_MODE, (void *)param, sizeof(HciExitSniffModeParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.2.8 Switch Role Command
int HCI_SwitchRole(const HciSwitchRoleParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(HCI_SWITCH_ROLE, (void *)param, sizeof(HciSwitchRoleParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.2.10 Write Link Policy Settings Command
int HCI_WriteLinkPolicySettings(const HciWriteLinkPolicySettingsParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(HCI_WRITE_LINK_POLICY_SETTINGS, (void *)param, sizeof(HciWriteLinkPolicySettingsParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.2.12 Write Default Link Policy Settings Command
int HCI_WriteDefaultLinkPolicySettings(const HciWriteDefaultLinkPolicySettingsParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(
        HCI_WRITE_DEFAULT_LINK_POLICY_SETTINGS, (void *)param, sizeof(HciWriteDefaultLinkPolicySettingsParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.2.14 Sniff Subrating Command
int HCI_SniffSubrating(const HciSniffSubratingParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(HCI_SNIFF_SUBRATING, (void *)param, sizeof(HciSniffSubratingParam));
    return HciSendCmd(cmd);
}