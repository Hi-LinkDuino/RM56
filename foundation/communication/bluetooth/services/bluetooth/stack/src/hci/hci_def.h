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

#ifndef HCI_DEF_H
#define HCI_DEF_H

#include "packet.h"

#include "hci_def_cmd.h"
#include "hci_def_common.h"
#include "hci_def_evt.h"
#include "hci_def_feature.h"
#include "hci_def_le_cmd.h"
#include "hci_def_le_evt.h"
#include "hci_def_le_feature.h"
#include "hci_def_supported_cmds.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    H2C_CMD,
    H2C_ACLDATA,
    H2C_SCODATA,
    C2H_ACLDATA,
    C2H_SCODATA,
    C2H_EVENT,
} HciPacketType;

typedef struct {
    HciPacketType type;
    Packet *packet;
} HciPacket;

#ifdef __cplusplus
}
#endif

#endif