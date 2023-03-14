/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include "hci_evt_cmd_complete.h"

#include <string.h>

#include "platform/include/allocator.h"
#include "platform/include/list.h"
#include "platform/include/mutex.h"

#include "hci/cmd/hci_cmd.h"
#include "hci/hci.h"
#include "hci/hci_def.h"

#include "hci_evt_controller_baseband_cmd_complete.h"
#include "hci_evt_info_params_cmd_complete.h"
#include "hci_evt_le_cmd_complete.h"
#include "hci_evt_link_ctrl_cmd_complete.h"
#include "hci_evt_link_policy_cmd_complete.h"
#include "hci_evt_status_params_cmd_complete.h"
#include "hci_vendor_if.h"

void HciEventOnCommandCompleteEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }

    if (BufferGetSize(payloadBuffer) < sizeof(HciCommandCompleteEventParam)) {
        return;
    }

    HciCommandCompleteEventParam *param = (HciCommandCompleteEventParam *)BufferPtr(payloadBuffer);
    if (param == NULL) {
        return;
    }
    HciSetNumberOfHciCmd(param->numHciCommandPackets);
    HciCmdOnCommandComplete(param->commandOpcode);

    uint8_t returnParametesLength = BufferGetSize(payloadBuffer) - sizeof(HciCommandCompleteEventParam);
    if (returnParametesLength == 0) {
        return;
    }
    const void *returnParametes = (uint8_t *)param + sizeof(HciCommandCompleteEventParam);

    switch (GET_OGF(param->commandOpcode)) {
        case HCI_COMMAND_OGF_LINK_CONTROL:
            HciEventOnLinkControlCommandComplete(param->commandOpcode, returnParametes, returnParametesLength);
            break;
        case HCI_COMMAND_OGF_LINK_POLICY:
            HciEventOnLinkPolicyCommandComplete(param->commandOpcode, returnParametes, returnParametesLength);
            break;
        case HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND:
            HciEventOnControllerBasebandCommandComplete(param->commandOpcode, returnParametes, returnParametesLength);
            break;
        case HCI_COMMAND_OGF_INFORMATIONAL_PARAMETERS:
            HciEventOnInformationalParametersCommandComplete(
                param->commandOpcode, returnParametes, returnParametesLength);
            break;
        case HCI_COMMAND_OGF_STATUS_PARAMETERS:
            HciEventOnStatusParametersCommandComplete(param->commandOpcode, returnParametes, returnParametesLength);
            break;
        case HCI_COMMAND_OGF_LE_CONTROLLER:
            HciEventOnLeCommandComplete(param->commandOpcode, returnParametes, returnParametesLength);
            break;
        case HCI_COMMAND_OGF_VENDOR_SPECIFIC:
            HciEventOnVendorCommandComplete(param->commandOpcode, returnParametes, returnParametesLength);
            break;
        default:
            break;
    }
}