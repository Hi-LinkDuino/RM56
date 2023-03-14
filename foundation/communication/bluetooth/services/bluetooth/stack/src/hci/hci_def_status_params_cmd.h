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

#ifndef HCI_DEF_STATUS_PARAMS_CMD_H
#define HCI_DEF_STATUS_PARAMS_CMD_H

#include <stdint.h>

#include "hci_def_cmd_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.5 STATUS PARAMETERS
#define HCI_COMMAND_OGF_STATUS_PARAMETERS 0x05

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.5.1 Read Failed Contact Counter Command
#define HCI_READ_FAILED_CONTACT_COUNTER MAKE_OPCODE(0x0001, HCI_COMMAND_OGF_STATUS_PARAMETERS)

typedef struct {
    uint16_t handle;
} HciReadFailedContactCounterParam;

typedef struct {
    uint8_t status;
    uint16_t handle;
    uint16_t failedContactCounter;
} HciReadFailedContactCounterReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.5.2 Reset Failed Contact Counter Command
#define HCI_RESET_FAILED_CONTACT_COUNTER MAKE_OPCODE(0x0002, HCI_COMMAND_OGF_STATUS_PARAMETERS)

typedef struct {
    uint16_t handle;
} HciResetFailedContactCounterParam;

typedef struct {
    uint8_t status;
    uint16_t handle;
} HciResetFailedContactCounterReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.5.3 Read Link Quality Command
#define HCI_READ_LINK_QUALITY MAKE_OPCODE(0x0003, HCI_COMMAND_OGF_STATUS_PARAMETERS)

typedef struct {
    uint16_t handle;
} HciReadLinkQualityParam;

typedef struct {
    uint8_t status;
    uint16_t handle;
    uint8_t linkQuality;
} HciReadLinkQualityReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.5.4 Read RSSI Command
#define HCI_READ_RSSI MAKE_OPCODE(0x0005, HCI_COMMAND_OGF_STATUS_PARAMETERS)

typedef struct {
    uint16_t handle;
} HciReadRssiParam;

typedef struct {
    uint8_t status;
    uint16_t handle;
    int8_t rssi;
} HciReadRssiReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.5.5 Read AFH Channel Map Command
#define HCI_READ_AFH_CHANNEL_MAP MAKE_OPCODE(0x0006, HCI_COMMAND_OGF_STATUS_PARAMETERS)

typedef struct {
    uint16_t connectionHandle;
} HciReadAfhChannelMapParam;

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
    uint8_t afhMode;
    uint8_t afhChannelMap[10];
} HciReadAfhChannelMapReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.5.6 Read Clock Command
#define HCI_READ_CLOCK MAKE_OPCODE(0x0007, HCI_COMMAND_OGF_STATUS_PARAMETERS)

typedef struct {
    uint16_t connectionHandle;
    uint8_t whichClock;
} HciReadClockParam;

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
    uint32_t clock;
    uint16_t accuracy;
} HciReadClockReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.5.7 Read Encryption Key Size Command
#define HCI_READ_ENCRYPTION_KEY_SIZE MAKE_OPCODE(0x0008, HCI_COMMAND_OGF_STATUS_PARAMETERS)

typedef struct {
    uint16_t connectionHandle;
} HciReadEncryptionKeySizeParam;

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
    uint8_t keySize;
} HciReadEncryptionKeySizeReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.5.8 Read Local AMP Info Command
#define HCI_READ_LOCAL_AMP_INFO MAKE_OPCODE(0x0009, HCI_COMMAND_OGF_STATUS_PARAMETERS)

typedef struct {
    uint8_t status;
    uint8_t ampStatus;
    uint32_t totalBandwidth;
    uint32_t maxGuaranteedBandwidth;
    uint32_t minLatency;
    uint16_t maxPduSize;
    uint8_t controllerType;
    uint16_t palCapabilities;
    uint16_t maxAmpAssocLength;
    uint32_t maxFlushTimeout;
    uint32_t bestEffortFlushTimeout;
} HciReadLocalAMPInfoReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.5.9 Read Local AMP ASSOC Command
#define HCI_READ_LOCAL_AMP_ASSOC MAKE_OPCODE(0x000A, HCI_COMMAND_OGF_STATUS_PARAMETERS)

typedef struct {
    uint8_t physicalLinkHandle;
    uint16_t lengthSoFar;
    uint16_t ampAssocLength;
} HciReadLocalAmpAssocParam;

typedef struct {
    uint8_t status;
    uint8_t physicalLinkHandle;
    uint16_t ampAssocLength;
} HciReadLocalAmpAssocReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.5.10 Write Remote AMP ASSOC Command
#define HCI_WRITE_REMOTE_AMP_ASSOC MAKE_OPCODE(0x000B, HCI_COMMAND_OGF_STATUS_PARAMETERS)

typedef struct {
    uint8_t physicalLinkHandle;
    uint16_t lengthSoFar;
    uint16_t ampAssocLength;
    uint8_t ampAssocfragment[248];
} HciWriteRemoteAmpAssocParam;

typedef struct {
    uint8_t status;
    uint8_t physicalLinkHandle;
    uint16_t ampAssocLength;
} HciWriteRemoteAmpAssocReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.5.11 Get MWS Transport Layer Configuration Command
#define HCI_GET_MWS_TRANSPORT_LAYER_CONFIGURATION MAKE_OPCODE(0x000C, HCI_COMMAND_OGF_STATUS_PARAMETERS)

typedef struct {
    uint8_t status;
    uint8_t numTransports;
    uint8_t transportLayer;
    uint8_t numBaudRates;
    uint8_t *toMwsBaudRate;
    uint8_t *fromMwsBaudRate;
} HciGetMwsTransportLayerConfigurationReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.5.12 Set Triggered Clock Capture Command
#define HCI_SET_TRIGGERED_CLOCK_CAPTURE MAKE_OPCODE(0x000D, HCI_COMMAND_OGF_STATUS_PARAMETERS)

typedef struct {
    uint16_t connectionHandle;
    uint8_t enable;
    uint8_t whichClock;
    uint8_t lpoAllowed;
    uint8_t numClockCapturesToFilter;
} HciSetTriggeredClockCaptureParam;

typedef HciStatusParam HciSetTriggeredClockCaptureReturnParam;

#pragma pack(0)

#ifdef __cplusplus
}
#endif

#endif