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
// 7.4.1 Read Local Version Information Command
int HCI_ReadLocalVersionInformation()
{
    HciCmd *cmd = HciAllocCmd(HCI_READ_LOCAL_VERSION_INFORMATION, NULL, 0);
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.4.2 Read Local Supported Commands Command
int HCI_ReadLocalSupportedCommands()
{
    HciCmd *cmd = HciAllocCmd(HCI_READ_LOCAL_SUPPORTED_COMMANDS, NULL, 0);
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.4.3 Read Local Supported Features Command
int HCI_ReadLocalSupportedFeatures()
{
    HciCmd *cmd = HciAllocCmd(HCI_READ_LOCAL_SUPPORTED_FEATURES, NULL, 0);
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.4.4 Read Local Extended Features Command
int HCI_ReadLocalExtendedFeatures(const HciReadLocalExtendedFeaturesParam *param)
{
    HciCmd *cmd =
        HciAllocCmd(HCI_READ_LOCAL_EXTENDED_FEATURES, (void *)param, sizeof(HciReadLocalExtendedFeaturesParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.4.5 Read Buffer Size Command
int HCI_ReadBufferSize()
{
    HciCmd *cmd = HciAllocCmd(HCI_READ_BUFFER_SIZE, NULL, 0);
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.4.6 Read BD_ADDR Command
int HCI_ReadBdAddr()
{
    HciCmd *cmd = HciAllocCmd(HCI_READ_BD_ADDR, NULL, 0);
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.4.8 Read Local Supported Codecs Command
int HCI_ReadLocalSupportedCodecs()
{
    HciCmd *cmd = HciAllocCmd(HCI_READ_LOCAL_SUPPORTED_CODECS, NULL, 0);
    return HciSendCmd(cmd);
}