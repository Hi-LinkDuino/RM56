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

#ifndef HCI_DEF_LINK_CTRL_CMD_H
#define HCI_DEF_LINK_CTRL_CMD_H

#include <stdint.h>

#include "hci_def_cmd_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1 LINK CONTROL COMMANDS
#define HCI_COMMAND_OGF_LINK_CONTROL 0x01

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.1 Inquiry Command
#define HCI_INQUIRY MAKE_OPCODE(0x0001, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    uint32_t lap : 24;
    uint8_t inquiryLen;
    uint8_t numResponses;
} HciInquiryeParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.2 Inquiry Cancel Command
#define HCI_INQUIRY_CANCEL MAKE_OPCODE(0x0002, HCI_COMMAND_OGF_LINK_CONTROL)

typedef HciStatusParam HciInquiryCancelReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.3 Periodic Inquiry Mode Command
#define HCI_PERIODIC_INQUIRY_MODE MAKE_OPCODE(0x0003, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    uint16_t maxPeriodLength;
    uint16_t minPeriodLength;
    uint32_t lap : 24;
    uint8_t inquiryLength;
    uint8_t numResponses;
} HciPeriodicInquiryModeParam;

typedef HciStatusParam HciPeriodicInquiryModeReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.4 Exit Periodic Inquiry Mode Command
#define HCI_EXIT_PERIODIC_INQUIRY_MODE MAKE_OPCODE(0x0004, HCI_COMMAND_OGF_LINK_CONTROL)

typedef HciStatusParam HciExitPeriodicInquiryModeReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.5 Create Connection Command
#define HCI_CREATE_CONNECTION MAKE_OPCODE(0x0005, HCI_COMMAND_OGF_LINK_CONTROL)

#define HCI_PACKET_TYPE_NO_2_DH1 0x0002
#define HCI_PACKET_TYPE_NO_3_DH1 0x0004
#define HCI_PACKET_TYPE_DM1 0x0008
#define HCI_PACKET_TYPE_DH1 0x0010
#define HCI_PACKET_TYPE_NO_2_DH3 0x0100
#define HCI_PACKET_TYPE_NO_3_DH3 0x0200
#define HCI_PACKET_TYPE_DM3 0x0400
#define HCI_PACKET_TYPE_DH3 0x0800
#define HCI_PACKET_TYPE_NO_2_DH5 0x1000
#define HCI_PACKET_TYPE_NO_3_HD5 0x2000
#define HCI_PACKET_TYPE_DM5 0x4000
#define HCI_PACKET_TYPE_DH5 0x8000

#define HCI_PAGE_SCAN_REPETITION_MODE_R0 0x00
#define HCI_PAGE_SCAN_REPETITION_MODE_R1 0x01
#define HCI_PAGE_SCAN_REPETITION_MODE_R2 0x02

#define HCI_NOT_ALLOW_ROLE_SWITCH 0x00
#define HCI_ALLOW_ROLE_SWITCH 0x01

typedef struct {
    HciBdAddr bdAddr;
    uint16_t packetType;
    uint8_t pageScanRepetitionMode;
    uint8_t reserved;
    uint16_t clockOffset;
    uint8_t allowRoleSwitch;
} HciCreateConnectionParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.6 Disconnect Command
#define HCI_DISCONNECT MAKE_OPCODE(0x0006, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    uint16_t connectionHandle;
    uint8_t reason;
} HciDisconnectParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.7 Create Connection Cancel Command
#define HCI_CREATE_CONNECTION_CANCEL MAKE_OPCODE(0x0008, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    HciBdAddr bdAddr;
} HciCreateConnectionCancelParam;

typedef struct {
    uint8_t status;
    HciBdAddr bdAddr;
} HciCreateConnectionCancelReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.8 Accept Connection Request Command
#define HCI_ACCEPT_CONNECTION_REQUEST MAKE_OPCODE(0x0009, HCI_COMMAND_OGF_LINK_CONTROL)

#define HCI_ROLE_MASTER 0x00
#define HCI_ROLE_SLAVE 0x01

typedef struct {
    HciBdAddr bdAddr;
    uint8_t role;
} HciAcceptConnectionReqestParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.9 Reject Connection Request Command
#define HCI_REJECT_CONNECTION_REQUEST MAKE_OPCODE(0x000A, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    HciBdAddr bdAddr;
    uint8_t reason;
} HciRejectConnectionRequestParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.10 Link Key Request Reply Command
#define HCI_LINK_KEY_REQUEST_REPLY MAKE_OPCODE(0x000B, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    HciBdAddr bdAddr;
    uint8_t linkKey[16];
} HciLinkKeyRequestReplyParam;

typedef struct {
    uint8_t status;
    HciBdAddr bdAddr;
} HciLinkKeyRequestReplyReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.11 Link Key Request Negative Reply Command
#define HCI_LINK_KEY_REQUEST_NEGATIVE_REPLY MAKE_OPCODE(0x000C, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    HciBdAddr bdAddr;
} HciLinkKeyRequestNegativeReplyParam;

typedef struct {
    uint8_t status;
    HciBdAddr bdAddr;
} HciLinkKeyRequestNegativeReplyReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.12 PIN Code Request Reply Command
#define HCI_PIN_CODE_REQUEST_REPLY MAKE_OPCODE(0x000D, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    HciBdAddr bdAddr;
    uint8_t pinCodeLength;
    uint8_t pinCode[16];
} HciPinCodeRequestReplyParam;

typedef struct {
    uint8_t status;
    HciBdAddr bdAddr;
} HciPinCodeRequestReplyReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.13 PIN Code Request Negative Reply Command
#define HCI_PIN_CODE_REQUEST_NEGATIVE_REPLY MAKE_OPCODE(0x000E, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    HciBdAddr bdAddr;
} HciPinCodeRequestNegativeReplyParam;

typedef struct {
    uint8_t status;
    HciBdAddr bdAddr;
} HciPinCodeRequestNegativeReplyReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.14 Change Connection Packet Type Command
#define HCI_CHANGE_CONNECTION_PACKET_TYPE MAKE_OPCODE(0x000F, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    uint16_t connectionHandle;
    uint16_t packetType;
} HciChangeConnectionPacketTypeParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.15 Authentication Requested Command
#define HCI_AUTHENTICATION_REQUESTED MAKE_OPCODE(0x0011, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    uint16_t connectionHandle;
} HciAuthenticationRequestedParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.16 Set Connection Encryption Command
#define HCI_SET_CONNECTION_ENCRYPTION MAKE_OPCODE(0x0013, HCI_COMMAND_OGF_LINK_CONTROL)

#define LINK_LEVEL_ENCRYPTION_OFF 0x00
#define LINK_LEVEL_ENCRYPTION_ON 0x01

typedef struct {
    uint16_t connectionHandle;
    uint8_t encryptionEnable;
} HciSetConnectionEncryptionParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.17 Change Connection Link Key Command
#define HCI_CHANGE_CONNECTION_LINK_KEY MAKE_OPCODE(0x0015, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    uint16_t connectionHandle;
} HciChangeConnectionLinkKeyParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.18 Master Link Key Command
#define HCI_MASTER_LINK_KEY MAKE_OPCODE(0x0017, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    uint8_t keyFlag;
} HciMasterLinkKeyParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.19 Remote Name Request Command
#define HCI_REMOTE_NAME_REQUEST MAKE_OPCODE(0x0019, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    HciBdAddr addr;
    uint8_t pageScanRepetMode;
    uint8_t reserved;
    uint16_t clockOffset;
} HciRemoteNameRequestParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.20 Remote Name Request Cancel Command
#define HCI_REMOTE_NAME_REQUEST_CANCEL MAKE_OPCODE(0x001A, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    HciBdAddr addr;
} HciRemoteNameRequestCancelParam;

typedef struct {
    uint8_t status;
    HciBdAddr addr;
} HciRemoteNameRequestCancelReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.21 Read Remote Supported Features Command
#define HCI_READ_REMOTE_SUPPORTED_FEATURES MAKE_OPCODE(0x001B, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    uint16_t connectionHandle;
} HciReadRemoteSupportedFeaturesParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.22 Read Remote Extended Features Command
#define HCI_READ_REMOTE_EXTENDED_FEATURES MAKE_OPCODE(0x001C, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    uint16_t connectionHandle;
    uint8_t pageNumber;
} HciReadRemoteExtendedFeaturesParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.23 Read Remote Version Information Command
#define HCI_READ_REMOTE_VERSION_INFORMATION MAKE_OPCODE(0x001D, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    uint16_t connectionHandle;
} HciReadRemoteVersionInformationParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.24 Read Clock Offset Command
#define HCI_READ_CLOCK_OFFSET MAKE_OPCODE(0x001F, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    uint16_t connectionHandle;
} HciReadClockOffsetParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.25 Read LMP Handle Command
#define HCI_READ_LMP_HANDLE MAKE_OPCODE(0x0020, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    uint16_t connectionHandle;
} HciReadLmpHandleParam;

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
    uint8_t lmpHandle;
    uint32_t reserved;
} HciReadLmpHandleReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.26 Setup Synchronous Connection Command
#define HCI_SETUP_SYNCHRONOUS_CONNECTION MAKE_OPCODE(0x0028, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    uint16_t connectionHandle;
    uint32_t transmitBandwidth;
    uint32_t receiveBandwidth;
    uint16_t maxLatency;
    uint16_t voiceSetting;
    uint8_t retransmissionEffort;
    uint16_t packetType;
} HciSetupSynchronousConnectionParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.27 Accept Synchronous Connection Request Command
#define HCI_ACCEPT_SYNCHRONOUS_CONNECTION_REQUEST MAKE_OPCODE(0x0029, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    HciBdAddr addr;
    uint32_t transmitBandwidth;
    uint32_t receiveBandwidth;
    uint16_t maxLatency;
    uint16_t voiceSetting;
    uint8_t retransmissionEffort;
    uint16_t packetType;
} HciAcceptSynchronousConnectionRequestParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.28 Reject Synchronous Connection Request Command
#define HCI_REJECT_SYNCHRONOUS_CONNECTION_REQUEST MAKE_OPCODE(0x002A, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    HciBdAddr bdAddr;
    uint8_t reason;
} HciRejectSynchronousConnectionRequestParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.29 IO Capability Request Reply Command
#define HCI_IO_CAPABILITY_REQUEST_REPLY MAKE_OPCODE(0x002B, HCI_COMMAND_OGF_LINK_CONTROL)

#define IO_CAPABILITY_DISPLAY_ONLY 0x00
#define IO_CAPABILITY_DISPLAY_YES_NO 0x01
#define IO_CAPABILITY_KEYBOARD_ONLY 0x02
#define IO_CAPABILITY_NO_INPUT_NO_OUTPUT 0x03

#define OOB_DATA_NOT_PRESENT 0x00
#define OOB_DATA_P_192 0x01
#define OOB_DATA_P_256 0x02
#define OOB_DATA_P_192_AND_P_256 0x03

#define AUTHENTICATION_NO_MITM_NO_BONDING 0x00
#define AUTHENTICATION_MITM_NO_BONDING 0x01
#define AUTHENTICATION_NO_MITM_DEDICATED_BONDING 0x02
#define AUTHENTICATION_MITM_DEDICATED_BONDING 0x03
#define AUTHENTICATION_NO_MITM_GENERAL_BONDING 0x04
#define AUTHENTICATION_MITM_GENERAL_BONDING 0x05

typedef struct {
    HciBdAddr bdAddr;
    uint8_t ioCapability;
    uint8_t oobDataPresent;
    uint8_t authenticationRequirements;
} HciIOCapabilityRequestReplyParam;

typedef struct {
    uint8_t status;
    HciBdAddr bdAddr;
} HciIOCapabilityRequestReplyReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.30 User Confirmation Request Reply Command
#define HCI_USER_CONFIRMATION_REQUEST_REPLY MAKE_OPCODE(0x002C, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    HciBdAddr bdAddr;
} HciUserConfirmationRequestReplyParam;

typedef struct {
    uint8_t status;
    HciBdAddr bdAddr;
} HciUserConfirmationRequestReplyReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.31 User Confirmation Request Negative Reply Command
#define HCI_USER_CONFIRMATION_REQUEST_NEGATIVE_REPLY MAKE_OPCODE(0x002D, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    HciBdAddr bdAddr;
} HciUserConfirmationRequestNegativeReplyParam;

typedef struct {
    uint8_t status;
    HciBdAddr bdAddr;
} HciUserConfirmationRequestNegativeReplyReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.32 User Passkey Request Reply Command
#define HCI_USER_PASSKEY_REQUEST_REPLY MAKE_OPCODE(0x002E, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    HciBdAddr bdAddr;
    uint32_t numericValue;
} HciUserPasskeyRequestReplyParam;

typedef struct {
    uint8_t status;
    HciBdAddr bdAddr;
} HciUserPasskeyRequestReplyReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.33 User Passkey Request Negative Reply Command
#define HCI_USER_PASSKEY_REQUEST_NEGATIVE_REPLY MAKE_OPCODE(0x002F, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    HciBdAddr bdAddr;
} HciUserPasskeyRequestNegativeReplyParam;

typedef struct {
    uint8_t status;
    HciBdAddr bdAddr;
} HciUserPasskeyRequestNegativeReplyReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.34 Remote OOB Data Request Reply Command
#define HCI_REMOTE_OOB_DATA_REQUEST_REPLY MAKE_OPCODE(0x0030, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    HciBdAddr bdAddr;
    uint8_t C[16];
    uint8_t r[16];
} HciRemoteOobDataRequestReplyParam;

typedef struct {
    uint8_t status;
    HciBdAddr bdAddr;
} HciRemoteOobDataRequestReplyReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.35 Remote OOB Data Request Negative Reply Command
#define HCI_REMOTE_OOB_DATA_REQUEST_NEGATIVE_REPLY MAKE_OPCODE(0x0033, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    HciBdAddr bdAddr;
} HciRemoteOobDataRequestNegativeReplyParam;

typedef struct {
    uint8_t status;
    HciBdAddr bdAddr;
} HciRemoteOobDataRequestNegativeReplyReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.36 IO Capability Request Negative Reply Command
#define HCI_IO_CAPABILITY_REQUEST_NEGATIVE_REPLY MAKE_OPCODE(0x0034, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    HciBdAddr bdAddr;
    uint8_t reason;
} HciIoCapabilityRequestNegativeReplyParam;

typedef struct {
    uint8_t status;
    HciBdAddr bdAddr;
} HciIoCapabilityRequestNegativeReplyReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.37 Create Physical Link Command
#define HCI_CREATE_PHYSICAL_LINK MAKE_OPCODE(0x0035, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    uint8_t physicalLinkHandle;
    uint8_t dedicatedAMPKeyLength;
    uint8_t dedicatedAMPKeyType;
    uint8_t *dedicatedAMPKey;
} HciCreatePhysicalLinkParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.38 Accept Physical Link Command
#define HCI_ACCEPT_PHYSICAL_LINK MAKE_OPCODE(0x0036, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    uint8_t physicalLinkHandle;
    uint8_t dedicatedAmpKeyLength;
    uint8_t dedicatedAmpKeyType;
    uint8_t *dedicatedAmpKey;
} HciAcceptPhysicalLinkParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.39 Disconnect Physical Link Command
#define HCI_DISCONNECT_PHYSICAL_LINK MAKE_OPCODE(0x0037, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    uint8_t physicalLinkHandle;
    uint8_t reason;
} HciDisconnectPhysicalLinkParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.40 Create Logical Link Command
#define HCI_CREATE_LOGICAL_LINK MAKE_OPCODE(0x0038, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    uint8_t physicalLinkHandle;
    uint8_t txFlowSpec[16];
    uint8_t rxFlowSpec[16];
} HciCreateLogicalLinkParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.41 Accept Logical Link Command
#define HCI_ACCEPT_LOGICAL_LINK MAKE_OPCODE(0x0039, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    uint8_t physicalLinkHandle;
    uint8_t txFlowSpec[16];
    uint8_t rxFlowSpec[16];
} HciAcceptLogicalLinkParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.42 Disconnect Logical Link Command
#define HCI_DISCONNECT_LOGICAL_LINK MAKE_OPCODE(0x003A, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    uint16_t logicalLinkHandle;
} HciDisconnectLogicalLinkParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.43 Logical Link Cancel Command
#define HCI_LOGICAL_LINK_CANCEL MAKE_OPCODE(0x003B, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    uint8_t physicalLinkHandle;
    uint8_t txFlowSpecID;
} HciLogicalLinkCancelParam;

typedef struct {
    uint8_t status;
    uint8_t physicalLinkHandle;
    uint8_t txFlowSpecID;
} HciLogicalLinkCancelReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.44 Flow Spec Modify Command
#define HCI_FLOW_SPEC_MODIFY MAKE_OPCODE(0x003C, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    uint16_t handle;
    uint8_t txFlowSpec[16];
    uint8_t rxFlowSpec[16];
} HciFlowSpecModifyParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.45 Enhanced Setup Synchronous Connection Command
#define HCI_ENHANCED_SETUP_SYNCHRONOUS_CONNECTION MAKE_OPCODE(0x003D, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    uint8_t codingFormat;
    uint16_t companyID;
    uint16_t vendorSpecificCodecID;
} HciCodeingFormat;

#define HCI_CODING_FORMAT_U_LAW_LOG 0x00
#define HCI_CODING_FORMAT_A_LAW_LOG 0x01
#define HCI_CODING_FORMAT_CVSD 0x02
#define HCI_CODING_FORMAT_TRANSPNT 0x03
#define HCI_CODING_FORMAT_LINEAR_PCM 0x04
#define HCI_CODING_FORMAT_MSBC 0x05
#define HCI_CODING_FORMAT_VENDOR_SPECIFIC 0xFF

#define HCI_PCM_DATA_FORMAT_NA 0x00
#define HCI_PCM_DATA_FORMAT_1_COMPLEMENT 0x01
#define HCI_PCM_DATA_FORMAT_2_COMPLEMENT 0x02
#define HCI_PCM_DATA_FORMAT_SIGN_MAGNITUDE 0x03
#define HCI_PCM_DATA_FORMAT_UNSIGNED 0x04

typedef struct {
    uint16_t connectionHandle;
    uint32_t transmitBandwidth;
    uint32_t receiveBandwidth;
    HciCodeingFormat transmitCodingFormat;
    HciCodeingFormat receiveCodingFormat;
    uint16_t transmitCodecFrameSize;
    uint16_t receiveCodecFrameSize;
    uint32_t inputBandwidth;
    uint32_t outputBandwidth;
    HciCodeingFormat inputCodingFormat;
    HciCodeingFormat outputCodingFormat;
    uint16_t inputCodedDataSize;
    uint16_t outputCodedDataSize;
    uint8_t inputPCMDataFormat;
    uint8_t outputPCMDataFormat;
    uint8_t inputPCMSamplePayloadMSBPosition;
    uint8_t outputPCMSamplePaylandMSBPosition;
    uint8_t inputDataPath;
    uint8_t outputDataPath;
    uint8_t inputTransportUnitSize;
    uint8_t outputTransportUnitSize;
    uint16_t maxLatency;
    uint16_t packetType;
    uint8_t retransmissionEffort;
} HciEnhancedSetupSynchronousConnectionParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.46 Enhanced Accept Synchronous Connection Request Command
#define HCI_ENHANCED_ACCEPT_SYNCHRONOUS_CONNECTION_REQUEST MAKE_OPCODE(0x003E, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    HciBdAddr bdAddr;
    uint32_t transmitBandwidth;
    uint32_t receiveBandwidth;
    HciCodeingFormat transmitCodingFormat;
    HciCodeingFormat receiveCodingFormat;
    uint16_t transmitCodecFrameSize;
    uint16_t receiveCodecFrameSize;
    uint32_t inputBandwidth;
    uint32_t outputBandwidth;
    HciCodeingFormat inputCodingFormat;
    HciCodeingFormat outputCodingFormat;
    uint16_t inputCodedDataSize;
    uint16_t outputCodedDataSize;
    uint8_t inputPCMDataFormat;
    uint8_t outputPCMDataFormat;
    uint8_t inputPCMSamplePayloadMSBPosition;
    uint8_t outputPCMSamplePaylandMSBPosition;
    uint8_t inputDataPath;
    uint8_t outputDataPath;
    uint8_t inputTransportUnitSize;
    uint8_t outputTransportUnitSize;
    uint16_t maxLatency;
    uint16_t packetType;
    uint8_t retransmissionEffort;
} HciEnhancedAcceptSynchronousConnectionRequestParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.47 Truncated Page Command
#define HCI_TRUNCATED_PAGE MAKE_OPCODE(0x003F, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    HciBdAddr bdAddr;
    uint8_t pageScanRepetitionMode;
    uint16_t clockOffset;
} HciTruncatedPageParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.48 Truncated Page Cancel Command
#define HCI_TRUNCATED_PAGE_CANCEL MAKE_OPCODE(0x0040, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    HciBdAddr bdAddr;
} HciTruncatedPageCancelParam;

typedef struct {
    uint8_t status;
    HciBdAddr bdAddr;
} HciTruncatedPageCancelReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.49 Set Connectionless Slave Broadcast Command
#define HCI_SET_CONNECTIONLESS_SLAVE_BROADCAST MAKE_OPCODE(0x0041, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    uint8_t enable;
    uint8_t ltAddr;
    uint8_t lpoAllowed;
    uint16_t packetType;
    uint16_t intervalMin;
    uint16_t intervalMax;
    uint16_t csbSupervisionTo;
} HciSetConnectionlessSlaveBroadcastParam;

typedef struct {
    uint8_t status;
    uint8_t ltAddr;
    uint16_t interval;
} HciSetConnectionlessSlaveBroadcastReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.50 Set Connectionless Slave Broadcast Receive Command
#define HCI_SET_CONNECTIONLESS_SLAVE_BROADCAST_RECEIVE MAKE_OPCODE(0x0042, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    uint8_t enable;
    HciBdAddr bdAddr;
    uint8_t ltAddr;
    uint16_t interval;
    uint32_t clockOffset;
    uint32_t nextConnectionlessSlaveBroadcastClock;
    uint16_t csbSupervisionTo;
    uint8_t remoteTimingAccuracy;
    uint8_t skip;
    uint16_t packetType;
    uint8_t afhChannelMap[10];
} HciSetConnectionlessSlaveBroadcastReceiveParam;

typedef struct {
    uint8_t status;
    HciBdAddr bdAddr;
    uint8_t ltAddr;
} HciSetConnectionlessSlaveBroadcastReceiveReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.51 Start Synchronization Train Command
#define HCI_START_SYNCHRONIZATION_TRAIN MAKE_OPCODE(0x0043, HCI_COMMAND_OGF_LINK_CONTROL)

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.52 Receive Synchronization Train Command
#define HCI_RECEIVE_SYNCHRONIZATION_TRAIN MAKE_OPCODE(0x0044, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    HciBdAddr bdAddr;
    uint16_t synchronizationscanTO;
    uint16_t syncScanWindow;
    uint16_t syncScanInterval;
} HciReceiveSynchronizationTrainParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.53 Remote OOB Extended Data Request Reply Command
#define HCI_REMOTE_OOB_EXTENDED_DATA_REQUEST_REPLY MAKE_OPCODE(0x0045, HCI_COMMAND_OGF_LINK_CONTROL)

typedef struct {
    HciBdAddr bdAddr;
    uint8_t c192[16];
    uint8_t r192[16];
    uint8_t c256[16];
    uint8_t r256[16];
} HciRemoteOobExtendedDataRequestReplyParam;

typedef struct {
    uint8_t status;
    HciBdAddr bdAddr;
} HciRemoteOobExtendedDataRequestReplyReturnParam;

#pragma pack(0)

#ifdef __cplusplus
}
#endif

#endif