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

#ifndef HCI_DEF_LINK_POLICY_CMD_H
#define HCI_DEF_LINK_POLICY_CMD_H

#include <stdint.h>

#include "hci_def_cmd_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.2 LINK POLICY COMMANDS
#define HCI_COMMAND_OGF_LINK_POLICY 0x02

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.2.1 Hold Mode Command
#define HCI_HOLD_MODE MAKE_OPCODE(0x0001, HCI_COMMAND_OGF_LINK_POLICY)

typedef struct {
    uint16_t connectionHandle;
    uint16_t holdModeMaxInterval;
    uint16_t holdModeMinInterval;
} HciHoldModeParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.2.2 Sniff Mode Command
#define HCI_SNIFF_MODE MAKE_OPCODE(0x0003, HCI_COMMAND_OGF_LINK_POLICY)

typedef struct {
    uint16_t connectionHandle;
    uint16_t sniffMaxInterval;
    uint16_t sniffMinInterval;
    uint16_t sniffAttempt;
    uint16_t sniffTimeout;
} HciSniffModeParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.2.3 Exit Sniff Mode Command
#define HCI_EXIT_SNIFF_MODE MAKE_OPCODE(0x0004, HCI_COMMAND_OGF_LINK_POLICY)

typedef struct {
    uint16_t connectionHandle;
} HciExitSniffModeParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.2.6 QoS Setup Command
#define HCI_QOS_SETUP MAKE_OPCODE(0x0007, HCI_COMMAND_OGF_LINK_POLICY)

typedef struct {
    uint16_t connectionHandle;
    uint8_t flags;
    uint8_t serviceType;
    uint32_t tokenRate;
    uint32_t peakBandwidth;
    uint32_t latency;
    uint32_t delayVariation;
} HciQosSetupParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.2.7 Role Discovery Command
#define HCI_ROLE_DISCOVERY MAKE_OPCODE(0x0009, HCI_COMMAND_OGF_LINK_POLICY)

typedef struct {
    uint16_t connectionHandle;
} HciRoleDiscoveryParam;

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
    uint8_t currentRole;
} HciRoleDiscoveryReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.2.8 Switch Role Command
#define HCI_SWITCH_ROLE MAKE_OPCODE(0x000B, HCI_COMMAND_OGF_LINK_POLICY)

typedef struct {
    HciBdAddr bdAddr;
    uint8_t role;
} HciSwitchRoleParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.2.9 Read Link Policy Settings Command
#define HCI_READ_LINK_POLICY_SETTINGS MAKE_OPCODE(0x000C, HCI_COMMAND_OGF_LINK_POLICY)

typedef struct {
    uint16_t connectionHandle;
} HciReadLinkPolicySettingsParam;

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
    uint16_t linkPolicySettings;
} HciReadLinkPolicySettingsReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.2.10 Write Link Policy Settings Command
#define HCI_WRITE_LINK_POLICY_SETTINGS MAKE_OPCODE(0x000D, HCI_COMMAND_OGF_LINK_POLICY)

typedef struct {
    uint16_t connectionHandle;
    uint16_t linkPolicySettings;
} HciWriteLinkPolicySettingsParam;

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
} HciWriteLinkPolicySettingsReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.2.11 Read Default Link Policy Settings Command
#define HCI_READ_DEFAULT_LINK_POLICY_SETTINGS MAKE_OPCODE(0x000E, HCI_COMMAND_OGF_LINK_POLICY)

typedef struct {
    uint8_t status;
    uint16_t defaultLinkPolicySettings;
} HciReadDefaultLinkPolicySettingsReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.2.12 Write Default Link Policy Settings Command
#define HCI_WRITE_DEFAULT_LINK_POLICY_SETTINGS MAKE_OPCODE(0x00F, HCI_COMMAND_OGF_LINK_POLICY)

#define HCI_LINK_POLICY_DISABLE_ALL 0x0000
#define HCI_LINK_POLICY_ENABLE_ROLE_SWITCH 0x0001
#define HCI_LINK_POLICY_ENABLE_HOLD_MODE 0x0002
#define HCI_LINK_POLICY_ENABLE_SNIFF_MODE 0x0004

typedef struct {
    uint16_t defaultLinkPolicySettings;
} HciWriteDefaultLinkPolicySettingsParam;

typedef HciStatusParam HciWriteDefaultLinkPolicySettingsReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.2.13 Flow Specification Command
#define HCI_FLOW_SPECIFICATION MAKE_OPCODE(0x00010, HCI_COMMAND_OGF_LINK_POLICY)

typedef struct {
    uint16_t connectionHandle;
    uint8_t flags;
    uint8_t flowdirection;
    uint8_t serviceType;
    uint32_t tokenRate;
    uint32_t tokenBucketSize;
    uint32_t peakBandwidth;
    uint32_t accessLatency;
} HciFlowSpecificationParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.2.14 Sniff Subrating Command
#define HCI_SNIFF_SUBRATING MAKE_OPCODE(0x0011, HCI_COMMAND_OGF_LINK_POLICY)

typedef struct {
    uint16_t connectionHandle;
    uint16_t maximumLatency;
    uint16_t minimumRemoteTimeout;
    uint16_t minimumLocalTimeout;
} HciSniffSubratingParam;

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
} HciSniffSubratingReturnParam;

#pragma pack(0)

#ifdef __cplusplus
}
#endif

#endif