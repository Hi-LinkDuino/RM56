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

#ifndef HCI_DEF_INFO_PARAMS_CMD_H
#define HCI_DEF_INFO_PARAMS_CMD_H

#include <stdint.h>

#include "hci_def_cmd_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.4 INFORMATIONAL PARAMETERS
#define HCI_COMMAND_OGF_INFORMATIONAL_PARAMETERS 0x04

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.4.1 Read Local Version Information Command
#define HCI_READ_LOCAL_VERSION_INFORMATION MAKE_OPCODE(0x0001, HCI_COMMAND_OGF_INFORMATIONAL_PARAMETERS)

typedef struct {
    uint8_t status;
    uint8_t hciVersion;
    uint16_t hciRevision;
    uint8_t lmpVersion;
    uint16_t manufacturerName;
    uint16_t lmpSubversion;
} HciReadLocalVersionInformationReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.4.2 Read Local Supported Commands Command
#define HCI_READ_LOCAL_SUPPORTED_COMMANDS MAKE_OPCODE(0x0002, HCI_COMMAND_OGF_INFORMATIONAL_PARAMETERS)

typedef struct {
    uint8_t status;
    uint8_t supportedCommands[64];
} HciReadLocalSupportedCommandsReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.4.3 Read Local Supported Features Command
#define HCI_READ_LOCAL_SUPPORTED_FEATURES MAKE_OPCODE(0x0003, HCI_COMMAND_OGF_INFORMATIONAL_PARAMETERS)

typedef struct {
    uint8_t status;
    HciLmpFeatures lmpFeatures;
} HciReadLocalSupportedFeaturesReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.4.4 Read Local Extended Features Command
#define HCI_READ_LOCAL_EXTENDED_FEATURES MAKE_OPCODE(0x0004, HCI_COMMAND_OGF_INFORMATIONAL_PARAMETERS)

typedef struct {
    uint8_t pageNumber;
} HciReadLocalExtendedFeaturesParam;

typedef struct {
    uint8_t status;
    uint8_t pageNumber;
    uint8_t maximunPageNumber;
    uint8_t extendedLMPFeatures[LMP_FEATURES_SIZE];
} HciReadLocalExtendedFeaturesReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.4.5 Read Buffer Size Command
#define HCI_READ_BUFFER_SIZE MAKE_OPCODE(0x0005, HCI_COMMAND_OGF_INFORMATIONAL_PARAMETERS)

typedef struct {
    uint8_t status;
    uint16_t hcAclDataPacketLength;
    uint8_t hcSynchronousDataPacketLength;
    uint16_t hcTotalNumAclDataPackets;
    uint16_t hcTotalNumSynchronousDataPackets;
} HciReadBufferSizeReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.4.6 Read BD_ADDR Command
#define HCI_READ_BD_ADDR MAKE_OPCODE(0x0009, HCI_COMMAND_OGF_INFORMATIONAL_PARAMETERS)

typedef struct {
    uint8_t status;
    HciBdAddr bdAddr;
} HciReadBdAddrReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.4.7 Read Data Block Size Command
#define HCI_READ_DATA_BLOCK_SIZE MAKE_OPCODE(0x000A, HCI_COMMAND_OGF_INFORMATIONAL_PARAMETERS)

typedef struct {
    uint8_t status;
    uint16_t maxAclDataPacketLength;
    uint16_t dataBlockLength;
    uint16_t totalNumDataBlocks;
} HciReadDataBlockSizeReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.4.8 Read Local Supported Codecs Command
#define HCI_READ_LOCAL_SUPPORTED_CODECS MAKE_OPCODE(0x000B, HCI_COMMAND_OGF_INFORMATIONAL_PARAMETERS)

typedef struct {
    uint16_t companyID;
    uint16_t vendorDefinedCodecID;
} HciVendorSpecificCodec;

typedef struct {
    uint8_t status;
    uint8_t numberOfSupportedCodecs;
    uint8_t *supportedCodecs;
    uint8_t numberOfSupportedVendorSpecificCodecs;
    HciVendorSpecificCodec *vendorSpecificCodecs;
} HciReadLocalSupportedCodecsReturnParam;

#pragma pack(0)

#ifdef __cplusplus
}
#endif

#endif