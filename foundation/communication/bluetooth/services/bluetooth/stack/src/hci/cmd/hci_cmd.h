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

#ifndef HCI_CMD_H
#define HCI_CMD_H

#include <stdint.h>

#include "packet.h"
#include "platform/include/alarm.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint16_t opCode;
    void *param;
    Packet *packet;
    Alarm *alarm;
} HciCmd;

void HciInitCmd();
void HciCloseCmd();

void HciSetNumberOfHciCmd(uint8_t numberOfHciCmd);

void HciCmdOnCommandComplete(uint16_t opCode);
void HciCmdOnCommandStatus(uint16_t opCode, uint8_t status);

HciCmd *HciAllocCmd(uint16_t opCode, const void *param, size_t paramLength);
int HciSendCmd(HciCmd *cmd);

#ifdef __cplusplus
}
#endif

#endif