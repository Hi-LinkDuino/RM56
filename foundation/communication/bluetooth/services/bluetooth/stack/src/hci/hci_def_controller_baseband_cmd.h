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

#ifndef HCI_DEF_CONTROLLER_BASEBAND_CMD_H
#define HCI_DEF_CONTROLLER_BASEBAND_CMD_H

#include <stdint.h>

#include "hci_def_cmd_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3 CONTROLLER & BASEBAND COMMANDS
#define HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND 0x03

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.1 Set Event Mask Command
#define HCI_SET_EVENT_MASK MAKE_OPCODE(0x0001, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

#define HCI_EVENT_MASK_DEFAULT 0x00001FFFFFFFFFFF
#define HCI_EVENT_MASK_SNIFF_SUBRATING_EVENT 0x0000200000000000
#define HCI_EVENT_MASK_EXTENDED_INQUIRY_RESULT_EVENT 0x0000400000000000
#define HCI_EVENT_MASK_ENCRYPTION_KEY_REFRESH_COMPLETE_EVENT 0x0000800000000000
#define HCI_EVENT_MASK_IO_CAPABILITY_REQUEST_EVENT 0x0001000000000000
#define HCI_EVENT_MASK_IO_CAPABILITY_RESPONSE_EVENT 0x0002000000000000
#define HCI_EVENT_MASK_USER_CONFIRMATION_REQUEST_EVENT 0x0004000000000000
#define HCI_EVENT_MASK_USER_PASSKEY_REQUEST_EVENT 0x0008000000000000
#define HCI_EVENT_MASK_REMOTE_OOB_DATA_REQUEST_EVENT 0x0010000000000000
#define HCI_EVENT_MASK_SIMPLE_PAIRING_COMPLETE_EVENT 0x0020000000000000
#define HCI_EVENT_MASK_LINK_SUPERVISION_TIMEOUT_CHANGED_EVENT 0x0080000000000000
#define HCI_EVENT_MASK_ENHANCED_FLUSH_COMPLETE_EVENT 0x0100000000000000
#define HCI_EVENT_MASK_USER_PASSKEY_NOTIFICATION_EVENT 0x0400000000000000
#define HCI_EVENT_MASK_KEYPRESS_NOTIFICATION_EVENT 0x0800000000000000
#define HCI_EVENT_MASK_REMOTE_HOST_SUPPORTED_FEATURES_NOTIFICATION 0x1000000000000000
#define HCI_EVENT_MASK_LE_META_EVENT 0x2000000000000000

#define HCI_EVENT_MASK_CORE_5_0                                                                                     \
    (HCI_EVENT_MASK_DEFAULT | HCI_EVENT_MASK_SNIFF_SUBRATING_EVENT | HCI_EVENT_MASK_EXTENDED_INQUIRY_RESULT_EVENT | \
        HCI_EVENT_MASK_ENCRYPTION_KEY_REFRESH_COMPLETE_EVENT | HCI_EVENT_MASK_IO_CAPABILITY_REQUEST_EVENT |         \
        HCI_EVENT_MASK_IO_CAPABILITY_RESPONSE_EVENT | HCI_EVENT_MASK_USER_CONFIRMATION_REQUEST_EVENT |              \
        HCI_EVENT_MASK_USER_PASSKEY_REQUEST_EVENT | HCI_EVENT_MASK_REMOTE_OOB_DATA_REQUEST_EVENT |                  \
        HCI_EVENT_MASK_SIMPLE_PAIRING_COMPLETE_EVENT | HCI_EVENT_MASK_LINK_SUPERVISION_TIMEOUT_CHANGED_EVENT |      \
        HCI_EVENT_MASK_ENHANCED_FLUSH_COMPLETE_EVENT | HCI_EVENT_MASK_USER_PASSKEY_NOTIFICATION_EVENT |             \
        HCI_EVENT_MASK_KEYPRESS_NOTIFICATION_EVENT | HCI_EVENT_MASK_REMOTE_HOST_SUPPORTED_FEATURES_NOTIFICATION |   \
        HCI_EVENT_MASK_LE_META_EVENT)

typedef struct {
    uint64_t eventMask;
} HciSetEventMaskParam;

typedef HciStatusParam HciSetEventMaskReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.2 Reset Command
#define HCI_RESET MAKE_OPCODE(0x0003, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef HciStatusParam HciResetReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.3 Set Event Filter Command
#define HCI_SET_EVENT_FILTER MAKE_OPCODE(0x0005, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t filterType;
    uint8_t filterConditionType;
    uint8_t condition;
} HciSetEventFilterParam;

typedef HciStatusParam HciSetEventFilterReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.4 Flush Command
#define HCI_FLUSH MAKE_OPCODE(0x0008, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint16_t connectionHandle;
} HciFlushParam;

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
} HciFlushReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.5 Read PIN Type Command
#define HCI_READ_PIN_TYPE MAKE_OPCODE(0x0009, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t status;
    uint8_t pinType;
} HciReadPinTypeReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.6 Write PIN Type Command
#define HCI_WRITE_PIN_TYPE MAKE_OPCODE(0x000A, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t pinType;
} HciWritePinTypeParam;

typedef HciStatusParam HciWritePinTypeReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.7 Create New Unit Key Command
#define HCI_CREATE_NEW_UNIT_KEY MAKE_OPCODE(0x000B, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef HciStatusParam HciCreateNewUnitKeyReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.8 Read Stored Link Key Command
#define HCI_READ_STORED_LINK_KEY MAKE_OPCODE(0x000D, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    HciBdAddr bdAddr;
    uint8_t readAllFlag;
} HciReadStoredLinkKeyParam;

typedef struct {
    uint8_t status;
    uint16_t maxNumKeys;
    uint16_t numKeysRead;
} HciReadStoredLinkKeyReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.9 Write Stored Link Key Command
#define HCI_WRITE_STORED_LINK_KEY MAKE_OPCODE(0x0011, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

#define BD_ADDR_SIZE 1

#define LINK_KEY_SIZE 16

typedef struct {
    uint8_t numKeysToWrite;
    HciBdAddr *bdAddr;
    uint8_t *linkKey;
} HciWriteStoredLinkKeyParam;

typedef struct {
    uint8_t status;
    uint8_t numKeysWritten;
} HciWriteStoredLinkKeyReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.10 Delete Stored Link Key Command
#define HCI_DELETE_STORED_LINK_KEY MAKE_OPCODE(0x0012, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    HciBdAddr bdAddr;
    uint8_t deleteAllFlag;
} HciDeleteStoredLinkKeyParam;

typedef struct {
    uint8_t status;
    uint16_t numKeysDeleted;
} HciDeleteStoredLinkKeyReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.11 Write Local Name Command
#define HCI_WRITE_LOCAL_NAME MAKE_OPCODE(0x0013, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t localName[248];
} HciWriteLocalNameParam;

typedef HciStatusParam HciWriteLocalNameReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.12 Read Local Name Command
#define HCI_READ_LOCAL_NAME MAKE_OPCODE(0x0014, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t status;
    uint8_t localName[248];
} HciReadLocalNameReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.13 Read Connection Accept Timeout Command
#define HCI_READ_CONNECTION_ACCEPT_TIMEOUT MAKE_OPCODE(0x0015, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t status;
    uint16_t connAcceptTimeout;
} HciReadConnectionAcceptTimeoutReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.14 Write Connection Accept Timeout Command
#define HCI_WRITE_CONNECTION_ACCEPT_TIMEOUT MAKE_OPCODE(0x0016, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t connAcceptTimeout;
} HciWriteConnectionAcceptTimeoutParam;

typedef HciStatusParam HciWriteConnectionAcceptTimeoutReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.15 Read Page Timeout Command
#define HCI_READ_PAGE_TIMEOUT MAKE_OPCODE(0x0017, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t status;
    uint16_t pageTimeout;
} HciReadPageTimeoutReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.16 Write Page Timeout Command
#define HCI_WRITE_PAGE_TIMEOUT MAKE_OPCODE(0x0018, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t pageTimeout;
} HciWritePageTimeoutParam;

typedef HciStatusParam HciWritePageTimeoutReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.17 Read Scan Enable Command
#define HCI_READ_SCAN_ENABLE MAKE_OPCODE(0x0019, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t status;
    uint8_t scanEnable;
} HciReadScanEnableReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.18 Write Scan Enable Command
#define HCI_WRITE_SCAN_ENABLE MAKE_OPCODE(0x001A, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t scanEnable;
} HciWriteScanEnableParam;

typedef HciStatusParam HciWriteScanEnableReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.19 Read Page Scan Activity Command
#define HCI_READ_PAGE_SCAN_ACTIVITY MAKE_OPCODE(0x001B, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t status;
    uint16_t pageScanInterval;
    uint16_t pageScanWindow;
} HciReadPageScanActivityReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.20 Write Page Scan Activity Command
#define HCI_WRITE_PAGE_SCAN_ACTIVITY MAKE_OPCODE(0x001C, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint16_t pageScanInterval;
    uint16_t pageScanWindow;
} HciWritePageScanActivityParam;

typedef HciStatusParam HciWritePageScanActivityReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.21 Read Inquiry Scan Activity Command
#define HCI_READ_INQUIRY_SCAN_ACTIVITY MAKE_OPCODE(0x001D, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t status;
    uint16_t inquiryScanInterval;
    uint16_t inquiryScanWindow;
} HciReadInquiryScanActivityReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.22 Write Inquiry Scan Activity Command
#define HCI_WRITE_INQUIRY_SCAN_ACTIVITY MAKE_OPCODE(0x001E, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint16_t inquiryScanInterval;
    uint16_t inquiryScanWindow;
} HciWriteInquiryScanActivityParam;

typedef HciStatusParam HciWriteInquiryScanActivityReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.23 Read Authentication Enable Command
#define HCI_READ_AUTHENTICATION_ENABLE MAKE_OPCODE(0x001F, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t status;
    uint8_t authenticationEnable;
} HciReadAuthenticationEnableReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.24 Write Authentication Enable Command
#define HCI_WRITE_AUTHENTICATION_ENABLE MAKE_OPCODE(0x0020, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t authenticationEnable;
} HciWriteAuthenticationEnableParam;

typedef HciStatusParam HciWriteAuthenticationEnableReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.25 Read Class of Device Command
#define HCI_READ_CLASS_OF_DEVICE MAKE_OPCODE(0x0023, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t status;
    uint8_t classofDevice[3];
} HciReadClassofDeviceReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.26 Write Class of Device Command
#define HCI_WRITE_CLASS_OF_DEVICE MAKE_OPCODE(0x0024, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t classofDevice[3];
} HciWriteClassofDeviceParam;

typedef HciStatusParam HciWriteClassofDeviceReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.27 Read Voice Setting Command
#define HCI_READ_VOICE_SETTING MAKE_OPCODE(0x0025, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t status;
    uint16_t voiceSetting;
} HciReadVoiceSettingReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.28 Write Voice Setting Command
#define HCI_WRITE_VOICE_SETTING MAKE_OPCODE(0x0026, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint16_t voiceSetting;
} HciWriteVoiceSettingParam;

typedef HciStatusParam HciWriteVoiceSettingParamReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 6.12 VOICE SETTING
#define HCI_VOICE_SETTING_AIR_CODING_FORMAT_CVSD 0x0000
#define HCI_VOICE_SETTING_AIR_CODING_FORMAT_ULAW 0x0001
#define HCI_VOICE_SETTING_AIR_CODING_FORMAT_ALAW 0x0002
#define HCI_VOICE_SETTING_AIR_CODING_FORMAT_TRANSPARENT_DATA 0x0003
#define HCI_VOICE_SETTING_INPUT_SAMPLE_SIZE_8_BIT 0x0000
#define HCI_VOICE_SETTING_INPUT_SAMPLE_SIZE_16_BIT 0x0020
#define HCI_VOICE_SETTING_INPUT_DATA_FORMAT_1S_COMPLEMENT 0x0000
#define HCI_VOICE_SETTING_INPUT_DATA_FORMAT_2S_COMPLEMENT 0x0040
#define HCI_VOICE_SETTING_INPUT_DATA_FORMAT_SIGN_MAGNITUDE 0x0080
#define HCI_VOICE_SETTING_INPUT_DATA_FORMAT_UNSIGNED 0x00C0
#define HCI_VOICE_SETTING_INPUT_CODING_LINEAR 0x0000
#define HCI_VOICE_SETTING_INPUT_CODING_ULAW 0x0100
#define HCI_VOICE_SETTING_INPUT_CODING_ALAW 0x0200

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.29 Read Automatic Flush Timeout Command
#define HCI_READ_AUTOMATIC_FLUSH_TIMEOUT MAKE_OPCODE(0x0027, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint16_t connectionHandle;
} HciReadAutomaticFlushTimeoutParam;

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
    uint16_t flushTimeout;
} HciReadAutomaticFlushTimeoutReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.30 Write Automatic Flush Timeout Command
#define HCI_WRITE_AUTOMATIC_FLUSH_TIMEOUT MAKE_OPCODE(0x0028, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint16_t connectionHandle;
    uint16_t flushTimeout;
} HciWriteAutomaticFlushTimeoutParam;

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
} HciWriteAutomaticFlushTimeoutReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.31 Read Num Broadcast Retransmissions Command
#define HCI_READ_NUM_BROADCAST_RETRANSMISSIONS MAKE_OPCODE(0x0029, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t status;
    uint8_t numBroadcastRetransmissions;
} HciReadNumBroadcastRetransmissionsReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.32 Write Num Broadcast Retransmissions Command
#define HCI_WRITE_NUM_BROADCAST_RETRANSMISSIONS MAKE_OPCODE(0x002A, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t numBroadcastRetransmissions;
} HciWriteNumBroadcastRetransmissionsParam;

typedef HciStatusParam HciWriteNumBroadcastRetransmissionsReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.33 Read Hold Mode Activity Command
#define HCI_READ_HOLD_MODE_ACTIVITY MAKE_OPCODE(0x002B, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t status;
    uint8_t holdModeActivity;
} HciReadHoldModeActivityReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.34 Write Hold Mode Activity Command
#define HCI_WRITE_HOLD_MODE_ACTIVITY MAKE_OPCODE(0x002C, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t holdModeActivity;
} HciWriteHoldModeActivityParam;

typedef HciStatusParam HciWriteHoldModeActivityReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.35 Read Transmit Power Level Command
#define HCI_READ_TRANSMIT_POWER_LEVEL MAKE_OPCODE(0x002D, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint16_t connectionHandle;
    uint8_t type;
} HciReadTransmitPowerLevelParam;

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
    uint8_t transmitPowerLevel;
} HciReadTransmitPowerLevelReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.36 Read Synchronous Flow Control Enable Command
#define HCI_READ_SYNCHRONOUS_FLOW_CONTROL_ENABLE MAKE_OPCODE(0x002E, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t status;
    uint8_t synchronousFlowControlEnable;
} HciReadSynchronousFlowControlEnableReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.37 Write Synchronous Flow Control Enable Command
#define HCI_WRITE_SYNCHRONOUS_FLOW_CONTROL_ENABLE MAKE_OPCODE(0x002F, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t synchronousFlowControlEnable;
} HciWriteSynchronousFlowControlEnableParam;

typedef HciStatusParam HciWriteSynchronousFlowControlEnableReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.38 Set Controller To Host Flow Control Command
#define HCI_SET_CONTROLLER_TO_HOST_FLOW_CONTROL MAKE_OPCODE(0x0031, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t flowControlEnable;
} HciSetControllerToHostFlowControlParam;

typedef HciStatusParam HciSetControllerToHostFlowControlReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.39 Host Buffer Size Command
#define HCI_HOST_BURRER_SIZE MAKE_OPCODE(0x0033, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint16_t hostAclDataPacketLength;
    uint8_t hostSynchronousDataPacketLength;
    uint16_t hostTotalNumAclDataPackets;
    uint16_t hostTotalNumSynchronousDataPackets;
} HciHostBufferSizeCmdParam;

typedef HciStatusParam HciHostBufferSizeReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.40 Host Number Of Completed Packets Command
#define HCI_HOST_NUMBER_OF_COMPLETED_PACKETS MAKE_OPCODE(0x0035, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t numberOfHandles;
    uint8_t *connectionHandle;
    uint8_t *hostNumOfCompletedPackets;
} HciHostNumberOfCompletedPacketsParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.41 Read Link Supervision Timeout Command
#define HCI_READ_LINK_SUPERVISION_TIMEOUT MAKE_OPCODE(0x0036, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint16_t handle;
} HciReadLinkSupervisionTimeoutParam;

typedef struct {
    uint8_t status;
    uint16_t handle;
    uint16_t linkSupervisionTimeout;
} HciReadLinkSupervisionTimeoutReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.42 Write Link Supervision Timeout Command
#define HCI_WRITE_LINK_SUPERVISION_TIMEOUT MAKE_OPCODE(0x0037, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint16_t handle;
    uint16_t linkSupervisionTimeout;
} HciWriteLinkSupervisionTimeoutParam;

typedef struct {
    uint8_t status;
    uint16_t handle;
} HciWriteLinkSupervisionTimeoutReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.43 Read Number Of Supported IAC Command
#define HCI_READ_NUMBER_OF_SUPPORTED_IAC MAKE_OPCODE(0x0038, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t status;
    uint8_t numSupportIac;
} HciReadNumberOfSupportedIacReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.44 Read Current IAC LAP Command
#define HCI_READ_CURRENT_IAC_LAP MAKE_OPCODE(0x0039, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t status;
    uint8_t numCurrentIAC;
    uint32_t *iacLAP;
} HciReadCurrentIacLapReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.45 Write Current IAC LAP Command
#define HCI_WRITE_CURRENT_IAC_LAP MAKE_OPCODE(0x003A, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

#define IACLAP_SIZE 3

typedef struct {
    uint8_t numCurrentIAC;
    uint32_t *iacLAP;
} HciWriteCurrentIacLapParam;

typedef HciStatusParam HciWriteCurrentIacLapReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.46 Set AFH Host Channel Classification Command
#define HCI_SET_AFH_HOST_CHANNEL_CLASSIFICATION MAKE_OPCODE(0x003F, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t afhHostChannelClassification[10];
} HciSetAfhHostChannelClassificationParam;

typedef HciStatusParam HciSetAfhHostChannelClassificationReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.47 Read Inquiry Scan Type Command
#define HCI_READ_INQUIRY_SCAN_TYPE MAKE_OPCODE(0x0042, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t status;
    uint8_t inquiryScanType;
} HciReadInquiryScanTypeReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.48 Write Inquiry Scan Type Command
#define HCI_WRITE_INQUIRY_SCAN_TYPE MAKE_OPCODE(0x0043, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t scanType;
} HciWriteInquiryScanTypeParam;

typedef HciStatusParam HciWriteInquiryScanTypeReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.49 Read Inquiry Mode Command
#define HCI_READ_INQUIRY_MODE MAKE_OPCODE(0x0044, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t status;
    uint8_t inquiryMode;
} HciReadInquiryModeReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.50 Write Inquiry Mode Command
#define HCI_WRITE_INQUIRY_MODE MAKE_OPCODE(0x0045, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

#define HCI_INQUIRY_MODE_STANDARD 0x00
#define HCI_INQUIRY_MODE_RSSI 0x01
#define HCI_INQUIRY_MODE_EIR 0x02

typedef struct {
    uint8_t inquiryMode;
} HciWriteInquiryModeParam;

typedef HciStatusParam HciWriteInquiryModeReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.51 Read Page Scan Type Command
#define HCI_READ_PAGE_SCAN_TYPE MAKE_OPCODE(0x0046, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t status;
    uint8_t pageScanType;
} HciReadPageScanTypeReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.52 Write Page Scan Type Command
#define HCI_WRITE_PAGE_SCAN_TYPE MAKE_OPCODE(0x0047, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t pageScanType;
} HciWritePageScanTypeParam;

typedef HciStatusParam HciWritePageScanTypeReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.53 Read AFH Channel Assessment Mode Command
#define HCI_READ_AFH_CHANNEL_ASSESSMENT_MODE MAKE_OPCODE(0x0048, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t status;
    uint8_t afhChannelAssessmentMode;
} HciReadAfhChannelAssessmentModeReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.54 Write AFH Channel Assessment Mode Command
#define HCI_WRITE_AFH_CHANNEL_ASSESSMENT_MODE MAKE_OPCODE(0x0049, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t afhChannelAssessmentMode;
} HciWriteAfhChannelAssessmentModeParam;

typedef HciStatusParam HciWriteAfhChannelAssessmentModeReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.55 Read Extended Inquiry Response Command
#define HCI_READ_EXTENDED_INQUIRY_RESPONSE MAKE_OPCODE(0x0051, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t status;
    uint8_t fecRequired;
    uint16_t extendedInquiryResponse[120];
} HciReadExtendedInquiryResponseReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.56 Write Extended Inquiry Response Command
#define HCI_WRITE_EXTENDED_INQUIRY_RESPONSE MAKE_OPCODE(0x0052, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t fecRequired;
    uint8_t extendInquiryRes[240];
} HciWriteExtendedInquiryResponseParam;

typedef HciStatusParam HciWriteExtendedInquiryResponseReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.57 Refresh Encryption Key Command
#define HCI_REFRESH_ENCRYPTION_KEY MAKE_OPCODE(0x0053, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint16_t connectionHandle;
} HciRefreshEncryptionKeyParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.58 Read Simple Pairing Mode Command
#define HCI_READ_SIMPLE_PAIRING_MODE MAKE_OPCODE(0x0055, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t status;
    uint8_t simplePairingMode;
} HciReadSimplePairingModeReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.59 Write Simple Pairing Mode Command
#define HCI_WRITE_SIMPLE_PAIRING_MODE MAKE_OPCODE(0x0056, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

#define SIMPLE_PAIRING_DISABLED 0x00
#define SIMPLE_PAIRING_ENABLED 0x01

typedef struct {
    uint8_t simplePairingMode;
} HciWriteSimplePairingModeParam;

typedef HciStatusParam HciWriteSimplePairingModeReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.60 Read Local OOB Data Command
#define HCI_READ_LOCAL_OOB_DATA MAKE_OPCODE(0x0057, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t status;
    uint8_t C[16];
    uint8_t R[16];
} HciReadLocalOOBDataReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.61 Read Inquiry Response Transmit Power Level Command
#define HCI_READ_INQUIRY_RESPONSE_TRANSMIT_POWER_LEVEL MAKE_OPCODE(0x0058, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t status;
    uint8_t txPower;
} HciReadInquiryResponseTransmitPowerLevelReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.62 Write Inquiry Transmit Power Level Command
#define HCI_WRITE_INQUIRY_TRANSMIT_POWER_LEVEL MAKE_OPCODE(0x0059, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t txPower;
} HciWriteInquiryTransmitPowerLevelParam;

typedef HciStatusParam HciWriteInquiryTransmitPowerLevelReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.63 Send Keypress Notification Command
#define HCI_SEND_KEY_PRESS_NOTIFICATION MAKE_OPCODE(0x0060, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    HciBdAddr bdAddr;
    uint8_t notificationType;
} HciSendKeypressNotificationParam;

typedef struct {
    uint8_t status;
    HciBdAddr bdAddr;
} HciSendKeypressNotificationReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.64 Read Default Erroneous Data Reporting Command
#define HCI_READ_DEFAULT_ERRONEOUS_DATA_REPORTING MAKE_OPCODE(0x005A, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t status;
    uint8_t erroneousDataReporting;
} HciReadDefaultErroneousDataReportingReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.65 Write Default Erroneous Data Reporting Command
#define HCI_WRITE_DEFAULT_ERRONEOUS_DATA_REPORTING MAKE_OPCODE(0x005B, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t erroneousDataReporting;
} HciWriteDefaultErroneousDataReportingParam;

typedef HciStatusParam HciWriteDefaultErroneousDataReportingReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.66 Enhanced Flush Command
#define HCI_ENHANCED_FLUSH MAKE_OPCODE(0x005F, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint16_t handle;
    uint8_t packetType;
} HciEnhancedFlushParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.67 Read Logical Link Accept Timeout Command
#define HCI_READ_LOGICAL_LINK_ACCEPT_TIMEOUT MAKE_OPCODE(0x0061, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t status;
    uint16_t logicalLinkAcceptTimeout;
} HciReadLogicalLinkAcceptTimeoutReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.68 Write Logical Link Accept Timeout Command
#define HCI_WRITE_LOGICAL_LINK_ACCEPT_TIMEOUT MAKE_OPCODE(0x0062, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint16_t logicalLinkAcceptTimeout;
} HciWriteLogicalLinkAcceptTimeoutParam;

typedef HciStatusParam HciWriteLogicalLinkAcceptTimeoutReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.69 Set Event Mask Page 2 Command
#define HCI_SET_EVENT_MASK_PAGE_2 MAKE_OPCODE(0x0063, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint64_t eventMaskPage2;
} HciSetEventMaskPage2Param;

typedef HciStatusParam HciSetEventMaskPage2ReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.70 Read Location Data Command
#define HCI_READ_LOCATION_DATA MAKE_OPCODE(0x0064, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t status;
    uint8_t locationDomainAware;
    uint16_t locationDomain;
    uint8_t locationOptions;
    uint8_t locationDomainOptions;
} HciReadLocationDataReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.71 Write Location Data Command
#define HCI_WRITE_LOCATION_DATA MAKE_OPCODE(0x0065, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t locationDomainAware;
    uint16_t locationDomain;
    uint8_t locationOptions;
    uint8_t locationDomainOptions;
} HciWriteLocationDataParam;

typedef HciStatusParam HciWriteLocationDataReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.72 Read Flow Control Mode Command
#define HCI_READ_FLOW_CONTROL_MODE MAKE_OPCODE(0x0066, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t status;
    uint8_t flowControlMode;
} HciReadFlowControlModeReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.73 Write Flow Control Mode Command
#define HCI_WRITE_FLOW_CONTROL_MODE MAKE_OPCODE(0x0067, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t flowControlMode;
} HciWriteFlowControlModeParam;

typedef HciStatusParam HciWriteFlowControlModeReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.74 Read Enhanced Transmit Power Level Command
#define HCI_READ_ENHANCED_TRANSMIT_POWER_LEVEL MAKE_OPCODE(0x0068, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint16_t connectionHandle;
    uint8_t type;
} HciReadEnhancedTransmitPowerLevelParam;

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
    uint8_t transmitPowerLevelGFSK;
    uint8_t transmitPowerLevelDQPSK;
    uint8_t transmitPowerLevel8DPSK;
} HciReadEnhancedTransmitPowerLevelReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.75 Read Best Effort Flush Timeout Command
#define HCI_READ_BEST_EFFORT_FLUSH_TIMEOUT MAKE_OPCODE(0x0069, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint16_t logicalLinkHandle;
} HciReadBestEffortFlushTimeoutParam;

typedef struct {
    uint8_t status;
    uint32_t bestEffortFlushTimeout;
} HciReadBestEffortFlushTimeoutReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.76 Write Best Effort Flush Timeout Command
#define HCI_WRITE_BEST_EFFORT_FLUSH_TIMEOUT MAKE_OPCODE(0x006A, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint16_t logicalLinkHandle;
    uint32_t bestEffortFlushTimeout;
} HciWriteBestEffortFlushTimeoutParam;

typedef HciStatusParam HciWriteBestEffortFlushTimeoutReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.77 Short Range Mode Command
#define HCI_SHORT_RANGE_MODE MAKE_OPCODE(0x006B, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t physicalLinkHandle;
    uint8_t shortRangeMode;
} HciShortRangeModeParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.78 Read LE Host Support Command
#define HCI_READ_LE_HOST_SUPPORT MAKE_OPCODE(0x006C, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t status;
    uint8_t leSupportedHost;
    uint8_t simultaneousLEHost;
} HciReadLeHostSupportReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.79 Write LE Host Support Command
#define HCI_WRITE_LE_HOST_SUPPORT MAKE_OPCODE(0x006D, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

#define HCI_LE_SUPPORTED_HOST_DISABLED 0x00
#define HCI_LE_SUPPORTED_HOST_ENABLED 0x01

#define HCI_SIMULTANEOUS_LE_HOST_DISABLED 0x00

typedef struct {
    uint8_t leSupportedHost;
    uint8_t simultaneousLeHost;
} HciWriteLeHostSupportParam;

typedef HciStatusParam HciWriteLeHostSupportReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.80 Set MWS Channel Parameters Command
#define HCI_SET_MWS_CHANNEL_PARAMETERS MAKE_OPCODE(0x006E, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t mwsChannelEnable;
    uint16_t mwsRXCenterFrequency;
    uint16_t mwsTXCenterFrequency;
    uint16_t mwsRXChannelBandwidth;
    uint16_t mwsTXChannelBandwidth;
    uint8_t mwsChannelType;
} HciSetMwsChannelParametersParam;

typedef HciStatusParam HciSetMwsChannelParametersReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.81 Set External Frame Configuration Command
#define HCI_SET_EXTERNAL_FRAME_CONFIGURATION MAKE_OPCODE(0x006F, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint16_t extFrameDuration;
    uint16_t extFrameSyncAssertOffset;
    uint16_t extFrameSyncAssertJitter;
    uint8_t extNumPeriods;
    uint16_t *periodDuration;
    uint8_t *periodType;
} HciSetExternalFrameConfigurationParam;

typedef HciStatusParam HciSetExternalFrameConfigurationReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.82 Set MWS Signaling Command
#define HCI_SET_MWS_SIGNALING MAKE_OPCODE(0x0070, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint16_t mwsRXAssertOffset;
    uint16_t mwsRXAssertJitter;
    uint16_t mwsRXDeassertOffset;
    uint16_t mwsRXDeassertJitter;
    uint16_t mwsTXAssertOffset;
    uint16_t mwsTXAssertJitter;
    uint16_t mwsTXDeassertOffset;
    uint16_t mwsTXDeassertJitter;
    uint16_t mwsPatternAssertOffset;
    uint16_t mwsPatternAssertJitter;
    uint16_t mwsInactivityDurationAssertOffset;
    uint16_t mwsInactivityDurationAssertJitter;
    uint16_t mwsScanFrequencyAssertOffset;
    uint16_t mwsScanFrequencyAssertJitter;
    uint16_t mwsPriorityAssertOffsetRequest;
} HciSetMwsSignalingParam;

typedef struct {
    uint8_t status;
    uint16_t bluetoothRxPriorityAssertOffset;
    uint16_t bluetoothRxPriorityAssertJitter;
    uint16_t bluetoothRxPriorityDeassertOffset;
    uint16_t bluetoothRxPriorityDeassertJitter;
    uint16_t rxPriorityAssertOffset;
    uint16_t rxPriorityAssertJitter;
    uint16_t rxPriorityDeassert_Offset;
    uint16_t rxPriorityDeassertJitter;
    uint16_t bluetoothTxOnAssertOffset;
    uint16_t bluetoothTxOnAssertJitter;
    uint16_t bluetoothTxOnDeassertOffset;
    uint16_t bluetoothTxOnDeassertJitter;
    uint16_t txOnAssertOffset;
    uint16_t txOnAssertJitter;
    uint16_t txOnDeassertOffset;
    uint16_t txOnDeassertJitter;
} HciSetMwsSignalingReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.83 Set MWS Transport Layer Command
#define HCI_SET_MWS_TRANSPORT_LAYER MAKE_OPCODE(0x0071, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t transportLayer;
    uint32_t toMwsBaudRate;
    uint32_t fromMwsBaudRate;
} HciSetMwsTransportLayerParam;

typedef HciStatusParam HciSetMwsTransportLayerReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.84 Set MWS Scan Frequency Table Command
#define HCI_SET_MWS_SCAN_FREQUENCY_TABLE MAKE_OPCODE(0x0072, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t numScanFrequencies;
    uint16_t *scanFrequencyLow;
    uint16_t *scanFrequencyHigh;
} HciSetMwsScanFrequencyTableParam;

typedef HciStatusParam HciSetMwsScanFrequencyTableReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.85 Set MWS_PATTERN Configuration Command
#define HCI_SET_MWS_PATTERN_CONFIGURATION MAKE_OPCODE(0x0073, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t mwsPatternIndex;
    uint8_t mwsPatternNumIntervals;
    uint16_t *mwsPatternIntervalDuration;
    uint8_t *mwsPatternIntervalType;
} HciSetMwsPatternConfigurationParam;

typedef HciStatusParam HciSetMwsPatternConfigurationReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.86 Set Reserved LT_ADDR Command
#define HCI_SET_RESERVED_LT_ADDR MAKE_OPCODE(0x0074, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t ltAddr;
} HciSetReservedLtAddrParam;

typedef struct {
    uint8_t status;
    uint8_t ltAddr;
} HciSetReservedLtAddrReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.87 Delete Reserved LT_ADDR Command
#define HCI_DELETE_RESERVED_LT_ADDR MAKE_OPCODE(0x0075, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t ltAddr;
} HciDeleteReservedLtAddrParam;

typedef struct {
    uint8_t status;
    uint8_t ltAddr;
} HciDeleteReservedLtAddrReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.88 Set Connectionless Slave Broadcast Data Command
#define HCI_SET_CONNECTIONLESS_SLAVE_BROADCAST_DATA MAKE_OPCODE(0x0076, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t ltAddr;
    uint8_t fragment;
    uint8_t dataLength;
    uint8_t *data;
} HciSetConnectionlessSlaveBroadcastDataParam;

typedef struct {
    uint8_t status;
    uint8_t ltAddr;
} HciSetConnectionlessSlaveBroadcastDataReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.89 Read Synchronization Train Parameters Command
#define HCI_READ_SYNCHRONIZATION_TRAIN_PARAMETERS MAKE_OPCODE(0x0077, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t status;
    uint16_t syncTrainInterval;
    uint32_t synchronizationtrainTO;
    uint8_t serviceData;
} HciReadSynchronizationTrainParametersReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.90 Write Synchronization Train Parameters Command
#define HCI_WRITE_SYNCHRONIZATION_TRAIN_PARAMETERS MAKE_OPCODE(0x0078, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint16_t intervalMin;
    uint16_t intervalMax;
    uint32_t synchronizationtrainTO;
    uint8_t serviceData;
} HciWriteSynchronizationTrainParametersParam;

typedef struct {
    uint8_t status;
    uint8_t syncTrainInterval;
} HciWriteSynchronizationTrainParametersReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.91 Read Secure Connections Host Support Command
#define HCI_READ_SECURE_CONNECTIONS_HOST_SUPPORT MAKE_OPCODE(0x0079, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t status;
    uint8_t secureConnectionsHostSupport;
} HciReadSecureConnectionsHostSupportReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.92 Write Secure Connections Host Support Command
#define HCI_WRITE_SECURE_CONNECTIONS_HOST_SUPPORT MAKE_OPCODE(0x007A, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

#define HCI_SECURE_CONNECTIONS_HOST_SUPPORT_DISABLED 0x00
#define HCI_SECURE_CONNECTIONS_HOST_SUPPORT_ENABLED 0x01

typedef struct {
    uint8_t secureConnectionsHostSupport;
} HciWriteSecureConnectionsHostSupportParam;

typedef HciStatusParam HciWriteSecureConnectionsHostSupportReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.93 Read Authenticated Payload Timeout Command
#define HCI_READ_AUTHENTICATED_PAYLOAD_TIMEOUT MAKE_OPCODE(0x007B, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint16_t connectionHandle;
} HciReadAuthenticatedPayloadTimeoutParam;

typedef struct {
    uint8_t status;
    uint32_t connectionHandle;
    uint16_t authenticatedPayloadTimeout;
} HciReadAuthenticatedPayloadTimeoutReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.94 Write Authenticated Payload Timeout Command
#define HCI_WRITE_AUTHENTICATED_PAYLOAD_TIMEOUT MAKE_OPCODE(0x007C, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint16_t connectionHandle;
    uint16_t authenticatedPayloadTimeout;
} HciWriteAuthenticatedPayloadTimeoutParam;

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
} HciWriteAuthenticatedPayloadTimeoutReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.95 Read Local OOB Extended Data Command
#define HCI_READ_LOCAL_OOB_EXTENDED_DATA MAKE_OPCODE(0x007D, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t status;
    uint8_t c192[16];
    uint8_t r192[16];
    uint8_t c256[16];
    uint8_t r256[16];
} HciReadLocalOobExtendedDataReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.96 Read Extended Page Timeout Command
#define HCI_READ_EXTENDED_PAGE_TIMEOUT MAKE_OPCODE(0x007E, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t status;
    uint16_t extendedPageTimeout;
} HciReadExtendedPageTimeoutReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.97 Write Extended Page Timeout Command
#define HCI_WRITE_EXTENDED_PAGE_TIMEOUT MAKE_OPCODE(0x007F, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint16_t extendedPageTimeout;
} HciWriteExtendedPageTimeoutParam;

typedef HciStatusParam HciWriteExtendedPageTimeoutReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.98 Read Extended Inquiry Length Command
#define HCI_READ_EXTENDED_INQUIRY_LENGTH MAKE_OPCODE(0x0080, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint8_t status;
    uint16_t extendedInquiryLength;
} HciReadExtendedInquiryLengthReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.99 Write Extended Inquiry Length Command
#define HCI_WRITE_EXTENDED_INQUIRY_LENGTH MAKE_OPCODE(0x0081, HCI_COMMAND_OGF_CONTTOLLER_AND_BASEBAND)

typedef struct {
    uint16_t extendedInquiryLength;
} HciWriteExtendedInquiryLengthParam;

typedef HciStatusParam HciWriteExtendedInquiryLengthReturnParam;

#pragma pack(0)

#ifdef __cplusplus
}
#endif

#endif