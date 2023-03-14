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

#include "hci_cmd_failure.h"

#include "hci/hci.h"
#include "hci/hci_def.h"

#include "hci_cmd_failure_controller_baseband.h"
#include "hci_cmd_failure_info_params.h"
#include "hci_cmd_failure_le_controller.h"
#include "hci_cmd_failure_link_ctrl.h"
#include "hci_cmd_failure_link_policy.h"
#include "hci_cmd_failure_status_params.h"

void HciOnCmdFailed(uint16_t opCode, uint8_t status, const void *param)
{
    switch (GET_OGF(opCode)) {
        case HCI_COMMAND_OGF_LINK_CONTROL:
            // BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
            // 7.1 LINK CONTROL COMMANDS
            HciOnLinkControlCmdFailed(opCode, status, param);
            break;
        case HCI_COMMAND_OGF_LINK_POLICY:
            // BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
            // 7.2 LINK POLICY COMMANDS
            HciOnLinkPolicyCmdFailed(opCode, status, param);
            break;
        case HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND:
            // BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
            // 7.3 CONTROLLER & BASEBAND COMMANDS
            HciOnControllerBasebandCmdFailed(opCode, status, param);
            break;
        case HCI_COMMAND_OGF_INFORMATIONAL_PARAMETERS:
            // BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
            // 7.4 INFORMATIONAL PARAMETERS
            HciOnInformationalParametersCmdFailed(opCode, status, param);
            break;
        case HCI_COMMAND_OGF_STATUS_PARAMETERS:
            // BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
            // 7.5 STATUS PARAMETERS
            HciOnStatusParametersCmdFailed(opCode, status, param);
            break;
        case HCI_COMMAND_OGF_LE_CONTROLLER:
            // BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
            // 7.8 LE CONTROLLER COMMANDS
            HciOnLeControllerCmdFailed(opCode, status, param);
            break;
        default:
            break;
    }
}