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
// 7.5.4 Read RSSI Command
int HCI_ReadRssi(const HciReadRssiParam *param)
{
    HciCmd *cmd = HciAllocCmd(HCI_READ_RSSI, (void *)param, sizeof(HciReadRssiParam));
    return HciSendCmd(cmd);
}

