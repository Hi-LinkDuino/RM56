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

#ifndef HCI_DEF_LE_CMD_H
#define HCI_DEF_LE_CMD_H

#include <stdint.h>

#include "hci_def_cmd_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8 LE CONTROLLER COMMANDS
#define HCI_COMMAND_OGF_LE_CONTROLLER 0x08

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.1 LE Set Event Mask Command
#define HCI_LE_SET_EVENT_MASK MAKE_OPCODE(0x0001, HCI_COMMAND_OGF_LE_CONTROLLER)

#define LE_EVENT_MASK_LE_CONNECTION_COMPLETE_EVENT 0x0000000000000001
#define LE_EVENT_MASK_LE_ADVERTISING_REPORT_EVENT 0x0000000000000002
#define LE_EVENT_MASK_LE_CONNECTION_UPDATE_COMPLETE_EVENT 0x0000000000000004
#define LE_EVENT_MASK_LE_READ_REMOTE_FEATURES_COMPLETE_EVENT 0x0000000000000008
#define LE_EVENT_MASK_LE_LONG_TERM_KEY_REQUEST_EVENT 0x0000000000000010
#define LE_EVENT_MASK_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_EVENT 0x0000000000000020
#define LE_EVENT_MASK_LE_DATA_LENGTH_CHANGE_EVENT 0x0000000000000040
#define LE_EVENT_MASK_LE_READ_LOCAL_P256_PUBLIC_KEY_COMPLETE_EVENT 0x0000000000000080
#define LE_EVENT_MASK_LE_GENERATE_DHKEY_COMPLETE_EVENT 0x0000000000000100
#define LE_EVENT_MASK_LE_ENHANCED_CONNECTION_COMPLETE_EVENT 0x0000000000000200
#define LE_EVENT_MASK_LE_DIRECTED_ADVERTISING_REPORT_EVENT 0x0000000000000400
#define LE_EVENT_MASK_LE_PHY_UPDATE_COMPLETE_EVENT 0x0000000000000800
#define LE_EVENT_MASK_LE_EXTENDED_ADVERTISING_REPORT_EVENT 0x0000000000001000
#define LE_EVENT_MASK_LE_PERIODIC_ADVERTISING_SYNC_ESTABLISHED_EVENT 0x0000000000002000
#define LE_EVENT_MASK_LE_PERIODIC_ADVERTISING_REPORT_EVENT 0x0000000000004000
#define LE_EVENT_MASK_LE_PERIODIC_ADVERTISING_SYNC_LOST_EVENT 0x0000000000008000
#define LE_EVENT_MASK_LE_EXTENDED_SCAN_TIMEOUT_EVENT 0x0000000000010000
#define LE_EVENT_MASK_LE_EXTENDED_ADVERTISING_SET_TERMINATED_EVENT 0x0000000000020000
#define LE_EVENT_MASK_LE_SCAN_REQUEST_RECEIVED_EVENT 0x0000000000040000
#define LE_EVENT_MASK_LE_CHANNEL_SELECTION_ALGORITHM_EVENT 0x0000000000080000

#define LE_EVENT_MASK_DEFAULT 0x000000000000001F

#define LE_EVENT_MASK_CORE_4_0 LE_EVENT_MASK_DEFAULT

#define LE_EVENT_MASK_CORE_4_1 (LE_EVENT_MASK_CORE_4_0 | LE_EVENT_MASK_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_EVENT)

#define LE_EVENT_MASK_CORE_4_2                                                                                        \
    (LE_EVENT_MASK_CORE_4_1 | LE_EVENT_MASK_LE_DATA_LENGTH_CHANGE_EVENT |                                             \
        LE_EVENT_MASK_LE_READ_LOCAL_P256_PUBLIC_KEY_COMPLETE_EVENT | LE_EVENT_MASK_LE_GENERATE_DHKEY_COMPLETE_EVENT | \
        LE_EVENT_MASK_LE_ENHANCED_CONNECTION_COMPLETE_EVENT | LE_EVENT_MASK_LE_DIRECTED_ADVERTISING_REPORT_EVENT)

#define LE_EVENT_MASK_CORE_5_0                                                                                       \
    (LE_EVENT_MASK_CORE_4_2 | LE_EVENT_MASK_LE_PHY_UPDATE_COMPLETE_EVENT |                                           \
        LE_EVENT_MASK_LE_EXTENDED_ADVERTISING_REPORT_EVENT |                                                         \
        LE_EVENT_MASK_LE_PERIODIC_ADVERTISING_SYNC_ESTABLISHED_EVENT |                                               \
        LE_EVENT_MASK_LE_PERIODIC_ADVERTISING_REPORT_EVENT | LE_EVENT_MASK_LE_PERIODIC_ADVERTISING_SYNC_LOST_EVENT | \
        LE_EVENT_MASK_LE_EXTENDED_SCAN_TIMEOUT_EVENT | LE_EVENT_MASK_LE_EXTENDED_ADVERTISING_SET_TERMINATED_EVENT |  \
        LE_EVENT_MASK_LE_SCAN_REQUEST_RECEIVED_EVENT | LE_EVENT_MASK_LE_CHANNEL_SELECTION_ALGORITHM_EVENT)

typedef struct {
    uint64_t leEventMask;
} HciLeSetEventMaskParam;

typedef HciStatusParam HciLeSetEventMaskReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.2 LE Read Buffer Size Command
#define HCI_LE_READ_BUFFER_SIZE MAKE_OPCODE(0x0002, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t status;
    uint16_t hcLeAclDataPacketLength;
    uint8_t hcTotalNumLeDataPackets;
} HciLeReadBufferSizeReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.3 LE Read Local Supported Features Command
#define HCI_LE_READ_LOCAL_SUPPORTED_FEATURES MAKE_OPCODE(0x0003, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t status;
    HciLeFeatures leFeatures;
} HciLeReadLocalSupportedFeaturesReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.4 LE Set Random Address Command
#define HCI_LE_SET_RANDOM_ADDRESS MAKE_OPCODE(0x0005, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t randomAddess[6];
} HciLeSetRandomAddressParam;

typedef HciStatusParam HciLeSetRandomAddressReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.5 LE Set Advertising Parameters Command
#define HCI_LE_SET_ADVERTISING_PARAMETERS MAKE_OPCODE(0x0006, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint16_t advertisingIntervalMin;
    uint16_t advertisingIntervalMax;
    uint8_t advertisingType;
    uint8_t ownAddressType;
    uint8_t peerAddressType;
    HciBdAddr peerAddress;
    uint8_t advertisingChannelMap;
    uint8_t advertisingFilterPolicy;
} HciLeSetAdvertisingParametersParam;

typedef HciStatusParam HciLeSetAdvertisingParametersReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.6 LE Read Advertising Channel Tx Power Command
#define HCI_LE_READ_ADVERTISING_CHANNEL_TX_POWER MAKE_OPCODE(0x0007, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t status;
    uint8_t transmitPowerLevel;
} HciLeReadAdvertisingChannelTxPowerReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.7 LE Set Advertising Data Command
#define HCI_LE_SET_ADVERTISING_DATA MAKE_OPCODE(0x0008, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t advertisingDataLen;
    uint8_t advertisingData[31];
} HciLeSetAdvertisingDataParam;

typedef HciStatusParam HciLeSetAdvertisingDataReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.8 LE Set Scan Response Data Command
#define HCI_LE_SET_SCAN_RESPONSE_DATA MAKE_OPCODE(0x0009, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t scanResponseDataLength;
    uint8_t scanResponseData[31];
} HciLeSetScanResponseDataParam;

typedef HciStatusParam HciLeSetScanResponseDataReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.9 LE Set Advertising Enable Command
#define HCI_LE_SET_ADVERTISING_ENABLE MAKE_OPCODE(0x000A, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t advertisingEnable;
} HciLeSetAdvertisingEnableParam;

typedef HciStatusParam HciLeSetAdvertisingEnableReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.10 LE Set Scan Parameters Command
#define HCI_LE_SET_SCAN_PARAMETERS MAKE_OPCODE(0x000B, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t leScanType;
    uint16_t leScanInterval;
    uint16_t leScanWindow;
    uint8_t ownAddressType;
    uint8_t scanningFilterPolicy;
} HciLeSetScanParametersParam;

typedef HciStatusParam HciLeSetScanParametersReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.11 LE Set Scan Enable Command
#define HCI_LE_SET_SCAN_ENABLE MAKE_OPCODE(0x000C, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t leScanEnable;
    uint8_t filterDuplicates;
} HciLeSetScanEnableParam;

typedef HciStatusParam HciLeSetScanEnableReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.12 LE Create Connection Command
#define HCI_LE_CREATE_CONNECTION MAKE_OPCODE(0x000D, HCI_COMMAND_OGF_LE_CONTROLLER)

#define INITIATOR_FILTER_NO_WHITE_LIST 0x00
#define INITIATOR_FILTER_USE_WHITE_LIST 0x01

#define PEER_ADDR_TYPE_PUBLIC 0x00
#define PEER_ADDR_TYPE_RANDOM 0x01
#define PEER_ADDR_TYPE_PUBLIC_IDENTITY 0x02
#define PEER_ADDR_TYPE_RANDOM_IDENTITY 0x03

#define LOCAL_ADDR_TYPE_PUBLIC 0x00
#define LOCAL_ADDR_TYPE_RANDOM 0x01
#define LOCAL_ADDR_TYPE_RPA_OR_PUBLIC 0x02
#define LOCAL_ADDR_TYPE_RPA_OR_RANDOM 0x03

typedef struct {
    uint16_t leScanInterval;
    uint16_t leScanWindow;
    uint8_t initiatorFilterPolicy;
    uint8_t peerAddressType;
    HciBdAddr peerAddress;
    uint8_t ownAddressType;
    uint16_t connIntervalMin;
    uint16_t connIntervalMax;
    uint16_t connLatency;
    uint16_t supervisionTimeout;
    uint16_t minimumCELength;
    uint16_t maximumCELength;
} HciLeCreateConnectionParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.13 LE Create Connection Cancel Command
#define HCI_LE_CREATE_CONNECTION_CANCEL MAKE_OPCODE(0x000E, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef HciStatusParam HciLeCreateConnectionCancelReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.14 LE Read WL Size Command
#define HCI_LE_READ_WHITE_LIST_SIZE MAKE_OPCODE(0x000F, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t status;
    uint8_t whiteListSize;
} HciLeReadWhiteListSizeReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.15 LE Clear WL Command
#define HCI_LE_CLEAR_WHITE_LIST MAKE_OPCODE(0x0010, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef HciStatusParam HciLeClearWhiteListReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.16 LE Add Device To WL Command
#define HCI_LE_ADD_DEVICE_TO_WHITE_LIST MAKE_OPCODE(0x0011, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t addressType;
    HciBdAddr address;
} HciLeAddDeviceToWhiteListParam;

typedef HciStatusParam HciLeAddDeviceToWhiteListReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.17 LE Remove Device From WL Command
#define HCI_LE_REMOVE_DEVICE_FROM_WHITE_LIST MAKE_OPCODE(0x0012, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t addressType;
    HciBdAddr address;
} HciLeRemoveDeviceFromWhiteListParam;

typedef HciStatusParam HciLeRemoveDeviceFromWhiteListReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.18 LE Connection Update Command
#define HCI_LE_CONNECTION_UPDATE MAKE_OPCODE(0x0013, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint16_t connectionHandle;
    uint16_t connIntervalMin;
    uint16_t connIntervalMax;
    uint16_t connLatency;
    uint16_t supervisionTimeout;
    uint16_t minimumCELength;
    uint16_t maximumCELength;
} HciLeConnectionUpdateParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.19 LE Set Host Channel Classification Command
#define HCI_LE_SET_HOST_CHANNEL_CLASSIFICATION MAKE_OPCODE(0x0014, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t channelMap[5];
} HciLeSetHostChannelClassificationParam;

typedef HciStatusParam HciLeSetHostChannelClassificationReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.20 LE Read Channel Map Command
#define HCI_LE_READ_CHANNEL_MAP MAKE_OPCODE(0x0015, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint16_t connectionHandle;
} HciLeReadChannelMapParam;

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
    uint8_t channelMap[5];
} HciLeReadChannelMapReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.21 LE Read Remote Features Command
#define HCI_LE_READ_REMOTE_FEATURES MAKE_OPCODE(0x0016, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint16_t connectionHandle;
} HciLeReadRemoteFeaturesParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.22 LE Encrypt Command
#define HCI_LE_ENCRYPT MAKE_OPCODE(0x0017, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t key[16];
    uint8_t plaintextData[16];
} HciLeEncryptParam;

typedef struct {
    uint8_t status;
    uint8_t encryptedData[16];
} HciLeEncryptReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.23 LE Rand Command
#define HCI_LE_RAND MAKE_OPCODE(0x0018, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t status;
    uint8_t randomNumber[8];
} HciLeRandReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.24 LE Start Encryption Command
#define HCI_LE_START_ENCRYPTION MAKE_OPCODE(0x0019, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint16_t connectionHandle;
    uint8_t randomNumber[8];
    uint16_t encryptDiversifier;
    uint8_t longTermKey[16];
} HciLeStartEncryptionParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.25 LE Long Term Key Request Reply Command
#define HCI_LE_LONG_TERM_KEY_REQUEST_REPLY MAKE_OPCODE(0x001A, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint16_t connectionHandle;
    uint8_t longTermKey[16];
} HciLeLongTermKeyRequestReplyParam;

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
} HciLeLongTermKeyRequestReplyReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.26 LE Long Term Key Request Negative Reply Command
#define HCI_LE_LONG_TERM_KEY_REQUEST_NEGATIVE_REPLY MAKE_OPCODE(0x001B, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint16_t connectionHandle;
} HciLeLongTermKeyRequestNegativeReplyParam;

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
} HciLeLongTermKeyRequestNegativeReplyReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.27 LE Read Supported States Command
#define HCI_LE_READ_SUPPORTED_STATES MAKE_OPCODE(0x001C, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t status;
    uint64_t leStates;
} HciLeReadSupportedStatesReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.28 LE Receiver Test Command
#define HCI_LE_RECEIVER_TEST MAKE_OPCODE(0x001D, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t rxChannel;
} HciLeReceiverTestParam;

typedef HciStatusParam HciLeReceiverTestReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.29 LE Transmitter Test Command
#define HCI_LE_TRANSMITTER_TEST MAKE_OPCODE(0x001E, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t txChannel;
    uint8_t lengthOfTestData;
    uint8_t packetPayload;
} HciLeTransmitterTestParam;

typedef HciStatusParam HciLeTransmitterTestReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.30 LE Test End Command
#define HCI_LE_TEST_END MAKE_OPCODE(0x001F, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t status;
    uint16_t numberOfPackets;
} HciLeTestEndReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.31 LE Remote Connection Parameter Request Reply Command
#define HCI_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_REPLY MAKE_OPCODE(0x0020, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint16_t connectionHandle;
    uint16_t intervalMin;
    uint16_t intervalMax;
    uint16_t latency;
    uint16_t timeout;
    uint16_t minimumCELength;
    uint16_t maximumCELength;
} HciLeRemoteConnectionParameterRequestReplyParam;

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
} HciLeRemoteConnectionParameterRequestReplyReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.32 LE Remote Connection Parameter Request Negative Reply Command
#define HCI_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_NEGATIVE_REPLY MAKE_OPCODE(0x0021, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint16_t connectionHandle;
    uint8_t reason;
} HciLeRemoteConnectionParameterRequestNegativeReplyParam;

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
} HciLeRemoteConnectionParameterRequestNegativeReplyReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.33 LE Set Data Length Command
#define HCI_LE_SET_DATA_LENGTH MAKE_OPCODE(0x0022, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint16_t connectionHandle;
    uint16_t txOctets;
    uint16_t txTime;
} HciLeSetDataLengthParam;

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
} HciLeSetDataLengthReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.34 LE Read Suggested Default Data Length Command
#define HCI_LE_READ_SUGGESTED_DEFAULT_DATA_LENGTH MAKE_OPCODE(0x0023, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t status;
    uint16_t suggestedMaxTxOctets;
    uint16_t suggestedMaxTxTime;
} HciLeReadSuggestedDefaultDataLengthReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.35 LE Write Suggested Default Data Length Command
#define HCI_LE_WRITE_SUGGESTED_DEFAULT_DATA_LENGTH MAKE_OPCODE(0x0024, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint16_t suggestedMaxTxOctets;
    uint16_t suggestedMaxTxTime;
} HciLeWriteSuggestedDefaultDataLengthParam;

typedef HciStatusParam HciLeWriteSuggestedDefaultDataLengthReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.36 LE Read Local P-256 Public Key Command
#define HCI_LE_READ_LOCAL_P256_PUBLIC_KEY MAKE_OPCODE(0x0025, HCI_COMMAND_OGF_LE_CONTROLLER)

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.37 LE Generate DHKey Command
#define HCI_LE_GENERATE_DHKEY MAKE_OPCODE(0x0026, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t remoteP256PublicKey[64];
} HciLeGenerateDHKeyParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.38 LE Add Device To Resolving List Command
#define HCI_LE_ADD_DEVICE_TO_RESOLVING_LIST MAKE_OPCODE(0x0027, HCI_COMMAND_OGF_LE_CONTROLLER)

#define HCI_PEER_IDENTITY_ADDRESS_TYPE_PUBLIC 0x00
#define HCI_PEER_IDENTITY_ADDRESS_TYPE_RANDOM 0x01

typedef struct {
    uint8_t peerIdentityAddrType;
    HciBdAddr peerIdentityAddress;
    uint8_t peerIrk[16];
    uint8_t localIrk[16];
} HciLeAddDeviceToResolvingListParam;

typedef HciStatusParam HciLeAddDeviceToResolvingListReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.39 LE Remove Device From Resolving List Command
#define HCI_LE_REMOVE_DEVICE_FROM_RESOLVING_LIST MAKE_OPCODE(0x0028, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t peerIdentityAddrType;
    HciBdAddr peerIdentityAddress;
} HciLeRemoveDeviceFromResolvingListParam;

typedef HciStatusParam HciLeRemoveDeviceFromResolvingListReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.40 LE Clear Resolving List Command
#define HCI_LE_CLEAR_RESOLVING_LIST MAKE_OPCODE(0x0029, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef HciStatusParam HciLeClearResolvingListReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.41 LE Read Resolving List Size Command
#define HCI_LE_READ_RESOLVING_LIST_SIZE MAKE_OPCODE(0x002A, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t status;
    uint8_t resolvingListSize;
} HciLeReadResolvingListSizeReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.42 LE Read Peer Resolvable Address Command
#define HCI_LE_READ_PEER_RESOLVABLE_ADDRESS MAKE_OPCODE(0x002B, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t peerIdentityAddressType;
    HciBdAddr peerIdentityAddress;
} HciLeReadPeerResolvableAddressParam;

typedef struct {
    uint8_t status;
    HciBdAddr peerResolvableAddress;
} HciLeReadPeerResolvableAddressReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.43 LE Read Local Resolvable Address Command
#define HCI_LE_READ_LOCAL_RESOLVABLE_ADDRESS MAKE_OPCODE(0x002C, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t peerIdentityAddressType;
    HciBdAddr peerIdentityAddress;
} HciLeReadLocalResolvableAddressParam;

typedef struct {
    uint8_t status;
    HciBdAddr localResolvableAddress;
} HciLeReadLocalResolvableAddressReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.44 LE Set Address Resolution Enable Command
#define HCI_LE_SET_ADDRESS_RESOLUTION_ENABLE MAKE_OPCODE(0x002D, HCI_COMMAND_OGF_LE_CONTROLLER)

#define ADDRESS_RESOLUTION_IN_CONTROLLER_DISABLED 0x00
#define ADDRESS_RESOLUTION_IN_CONTROLLER_ENABLEED 0x01

typedef struct {
    uint8_t addressResolutionEnable;
} HciLeSetAddressResolutionEnableParam;

typedef HciStatusParam HciLeSetAddressResolutionEnableReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.45 LE Set Resolvable Private Address Timeout Command
#define HCI_LE_SET_RESOLVABLE_PRIVATE_ADDRESS_TIMEOUT MAKE_OPCODE(0x002E, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint16_t rpaTimeout;
} HciLeSetResolvablePrivateAddressTimeoutParam;

typedef HciStatusParam HciLeSetResolvablePrivateAddressTimeoutReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.46 LE Read Maximum Data Length Command
#define HCI_LE_READ_MAXIMUM_DATA_LENGTH MAKE_OPCODE(0x002F, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t status;
    uint16_t supportedMaxTxOctets;
    uint16_t supportedMaxTxTime;
    uint16_t supportedMaxRxOctets;
    uint16_t supportedMaxRxTime;
} HciLeReadMaximumDataLengthReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.47 LE Read PHY Command
#define HCI_LE_READ_PHY MAKE_OPCODE(0x0030, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint16_t connectionHandle;
} HciLeReadPhyParam;

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
    uint8_t txPhy;
    uint8_t rxPhy;
} HciLeReadPhyReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.48 LE Set Default PHY Command
#define HCI_LE_SET_DEFAULT_PHY MAKE_OPCODE(0x0031, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t allPhys;
    uint8_t txPhys;
    uint8_t rxPhys;
} HciLeSetDefaultPhyParam;

typedef HciStatusParam HciLeSetDefaultPhyReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.49 LE Set PHY Command
#define HCI_LE_SET_PHY MAKE_OPCODE(0x0032, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint16_t connectionHandle;
    uint8_t allPhys;
    uint8_t txPhys;
    uint8_t rxPhys;
    uint16_t phyOptions;
} HciLeSetPhyParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.50 LE Enhanced Receiver Test Command
#define HCI_LE_ENHANCED_RECEIVER_TEST MAKE_OPCODE(0x0033, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t rxChannel;
    uint8_t phy;
    uint8_t modulationIndex;
} HciLeEnhancedReceiverTestParam;

typedef HciStatusParam HciLeEnhancedReceiverTestReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.51 LE Enhanced Transmitter Test Command
#define HCI_LE_ENHANCED_TRANSMITTER_TEST MAKE_OPCODE(0x0034, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t txChannel;
    uint8_t lengthOfTestData;
    uint8_t packetPayload;
    uint8_t phy;
} HciLeEnhancedTransmitterTestParam;

typedef HciStatusParam HciLeEnhancedTransmitterTestReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.52 LE Set Advertising Set Random Address Command
#define HCI_LE_SET_ADVERTISING_SET_RANDOM_ADDRESS MAKE_OPCODE(0x0035, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t advertisingHandle;
    uint8_t randomAddress[6];
} HciLeSetAdvertisingSetRandomAddressParam;

typedef HciStatusParam HciLeSetAdvertisingSetRandomAddressReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.53 LE Set Extended Advertising Parameters Command
#define HCI_LE_SET_EXTENDED_ADVERTISING_PARAMETERS MAKE_OPCODE(0x0036, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t advertisingHandle;
    uint16_t advertisingEventProperties;
    uint8_t priAdvertisingIntervalMin[3];
    uint8_t priAdvertisingIntervalMax[3];
    uint8_t priAdvertisingChannelMap;
    uint8_t ownAddressType;
    uint8_t peerAddressType;
    uint8_t peerAddress[6];
    uint8_t advertisingFilterPolicy;
    uint8_t advertisingTxPower;
    uint8_t priAdvertisingPHY;
    uint8_t secondaryAdvertisingMaxSkip;
    uint8_t secondaryAdvertisingPHY;
    uint8_t advertisingSID;
    uint8_t scanRequestNotificationEnable;
} HciLeSetExtendedAdvertisingParametersParam;

typedef struct {
    uint8_t status;
    uint8_t selectedTxPower;
} HciLeSetExtendedAdvertisingParametersReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.54 LE Set Extended Advertising Data Command
#define HCI_LE_SET_EXTENDED_ADVERTISING_DATA MAKE_OPCODE(0x0037, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t advertisingHandle;
    uint8_t operation;
    uint8_t fragmentPreference;
    uint8_t advertisingDataLength;
    const uint8_t *advertisingData;
} HciLeSetExtendedAdvertisingDataParam;

typedef HciStatusParam HciLeSetExtendedAdvertisingDataReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.55 LE Set Extended Scan Response Data Command
#define HCI_LE_SET_EXTENDED_SCAN_RESPONSE_DATA MAKE_OPCODE(0x0038, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t advertisingHandle;
    uint8_t operation;
    uint8_t fragmentPreference;
    uint8_t scanResponseDataLength;
    const uint8_t *scanResponseData;
} HciLeSetExtendedScanResponseDataParam;

typedef HciStatusParam HciLeSetExtendedScanResponseDataReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.56 LE Set Extended Advertising Enable Command
#define HCI_LE_SET_EXTENDED_ADVERTISING_ENABLE MAKE_OPCODE(0x0039, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t adverHandle;
    uint16_t duration;
    uint8_t maxExtendAdvertisingEvents;
} HciLeExtendedAdvertisingParamSet;

typedef struct {
    uint8_t enable;
    uint8_t numberofSets;
    HciLeExtendedAdvertisingParamSet *sets;
} HciLeSetExtendedAdvertisingEnableParam;

typedef HciStatusParam HciLeSetExtendedAdvertisingEnableReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.57 LE Read Maximum Advertising Data Length Command
#define HCI_LE_READ_MAXIMUM_ADVERTISING_DATA_LENGTH MAKE_OPCODE(0x003A, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t status;
    uint16_t maximumAdvertisingDataLength;
} HciLeReadMaximumAdvertisingDataLengthReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.58 LE Read Number of Supported Advertising Sets Command
#define HCI_LE_READ_NUMBER_OF_SUPPORTED_ADVERTISING_SETS MAKE_OPCODE(0x003B, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t status;
    uint8_t numSupportedAdvertisingSets;
} HciLeReadNumberofSupportedAdvertisingSetsReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.59 LE Remove Advertising Set Command
#define HCI_LE_REMOVE_ADVERTISING_SET MAKE_OPCODE(0x003C, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t advertisingHandle;
} HciLeRemoveAdvertisingSetParam;

typedef HciStatusParam HciLeRemoveAdvertisingSetReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.60 LE Clear Advertising Sets Command
#define HCI_LE_CLEAR_ADVERTISING_SETS MAKE_OPCODE(0x003D, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef HciStatusParam HciLeClearAdvertisingSetsReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.61 LE Set Periodic Advertising Parameters Command
#define HCI_LE_SET_PERIODIC_ADVERTISING_PARAMETERS MAKE_OPCODE(0x003E, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t advertisingHandle;
    uint16_t periodicAdvertisingIntervalMin;
    uint16_t periodicAdvertisingIntervalMax;
    uint16_t periodicAdvertisingProperties;
} HciLeSetPeriodicAdvertisingParametersParam;

typedef HciStatusParam HciLeSetPeriodicAdvertisingParametersReturnParameters;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.62 LE Set Periodic Advertising Data Command
#define HCI_LE_SET_PERIODIC_ADVERTISING_DATA MAKE_OPCODE(0x003F, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t advertisingHandle;
    uint8_t operation;
    uint8_t advertisingDataLength;
    uint8_t advertisingData;
} HciLeSetPeriodicAdvertisingDataParam;

typedef HciStatusParam HciLeSetPeriodicAdvertisingDataReturnParameters;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.63 LE Set Periodic Advertising Enable Command
#define HCI_LE_SET_PERIODIC_ADVERTISING_ENABLE MAKE_OPCODE(0x0040, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t enable;
    uint8_t advertisingHandle;
} HciLeSetPeriodicAdvertisingEnableParam;

typedef HciStatusParam HciLeSetPeriodicAdvertisingEnableReturnParameters;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.64 LE Set Extended Scan Parameters Command
#define HCI_LE_SET_EXTENDED_SCAN_PARAMETERS MAKE_OPCODE(0x0041, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t scanType;
    uint16_t scanInterval;
    uint16_t scanWindow;
} HciLeExtendedScanParametersSet;

typedef struct {
    uint8_t ownAddressType;
    uint8_t ScanningFilterPolicy;
    uint8_t ScanningPHYs;
    HciLeExtendedScanParametersSet *sets;
} HciLeSetExtendedScanParametersParam;

typedef HciStatusParam HciLeSetExtendedScanParametersReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.65 LE Set Extended Scan Enable Command
#define HCI_LE_SET_EXTENDED_SCAN_ENABLE MAKE_OPCODE(0x0042, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t enable;
    uint8_t filterDuplicates;
    uint16_t duration;
    uint16_t period;
} HciLeSetExtendedScanEnableParam;

typedef HciStatusParam HciLeSetExtendedScanEnableReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.66 LE Extended Create Connection Command
#define HCI_LE_EXTENDED_CREATE_CONNECTION MAKE_OPCODE(0x0043, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint16_t scanInterval;
    uint16_t scanWindow;
    uint16_t connIntervalMin;
    uint16_t connIntervalMax;
    uint16_t connLatency;
    uint16_t supervisionTimeout;
    uint16_t minimumCELength;
    uint16_t maximumCELength;
} HciLeConnectionParamSet;

#define LE_1M_PHY 0x01
#define LE_2M_PHY 0x02
#define LE_CODED_PHY 0x04

typedef struct {
    uint8_t initiatingFilterPolicy;
    uint8_t ownAddressType;
    uint8_t peerAddressType;
    HciBdAddr peerAddress;
    uint8_t initiatingPhys;
    HciLeConnectionParamSet *sets;
} HciLeExtendedCreateConnectionParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.68 LE Periodic Advertising Create Sync Cancel Command
#define HCI_LE_PERIODIC_ADVERTISING_CREATE_SYNC_CANCEL MAKE_OPCODE(0x0045, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef HciStatusParam HciLePeriodicAdvertisingCreateSyncCancelReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.69 LE Periodic Advertising Terminate Sync Command
#define HCI_LE_PERIODIC_ADVERTISING_TERMINATE_SYNC MAKE_OPCODE(0x0046, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint16_t syncHandle;
} HciLePeriodicAdvertisingTerminateSyncParam;

typedef HciStatusParam HciLePeriodicAdvertisingTerminateSyncReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.70 LE Add Device To Periodic Advertiser List Command
#define HCI_LE_ADD_DEVICE_TO_PERIODIC_ADVERTISER_LIST MAKE_OPCODE(0x0047, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t advertiserAddressType;
    uint8_t advertiserAddress[6];
    uint8_t advertisingSid;
} HciLeAddDeviceToPeriodicAdvertiserListParam;

typedef HciStatusParam HciLeAddDeviceToPeriodicAdvertiserListReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.71 LE Remove Device From Periodic Advertiser List Command
#define HCI_LE_REMOVE_DEVICE_FROM_PERIODIC_ADVERTISER_LIST MAKE_OPCODE(0x0048, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t advertiserAddressType;
    uint8_t advertiserAddress[6];
    uint8_t advertisingSid;
} HciLeRemoveDeviceFromPeriodicAdvertiserListParam;

typedef HciStatusParam HciLeRemoveDeviceFromPeriodicAdvertiserListReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.72 LE Clear Periodic Advertiser List Command
#define HCI_LE_CLEAR_PERIODIC_ADVERTISER_LIST MAKE_OPCODE(0x0049, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef HciStatusParam HciLeClearPeriodicAdvertiserListReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.73 LE Read Periodic Advertiser List Size Command
#define HCI_LE_READ_PERIODIC_ADVERTISER_LIST_SIZE MAKE_OPCODE(0x004A, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t status;
    uint8_t periodicAdvertiserListSize;
} HciLeReadPeriodicAdvertiserListSizeReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.74 LE Read Transmit Power Command
#define HCI_LE_READ_TRANSMIT_POWER MAKE_OPCODE(0x004B, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t status;
    uint8_t minTxPower;
    uint8_t maxTxPower;
} HciLeReadTransmitPowerReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.75 LE Read RF Path Compensation Command
#define HCI_LE_READ_RF_PATH_COMPENSATION MAKE_OPCODE(0x004C, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint8_t status;
    uint16_t rfTxPathCompensationValue;
    uint16_t rfRxPathCompensationValue;
} HciLeReadRfPathCompensationReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.76 LE Write RF Path Compensation Command
#define HCI_LE_WRITE_RF_PATH_COMPENSATION MAKE_OPCODE(0x004D, HCI_COMMAND_OGF_LE_CONTROLLER)

typedef struct {
    uint16_t rfTxPathCompensationValue;
    uint16_t rfRxPathCompensationValue;
} HciLeWriteRfPathCompensationParam;

typedef HciStatusParam HciLeWriteRfPathCompensationReturnParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.77 LE Set Privacy Mode Command
#define HCI_LE_SET_PRIVACY_MODE MAKE_OPCODE(0x004E, HCI_COMMAND_OGF_LE_CONTROLLER)

#define HCI_NETWORK_PRIVACY_MODE 0x00
#define HCI_DEVICE_PRIVACY_MODE 0x01

typedef struct {
    uint8_t peerIdentityAddressType;
    HciBdAddr peerIdentityAddress;
    uint8_t privacyMode;
} HciLeSetPrivacyModeParam;

typedef HciStatusParam HciLeSetPrivacyModeReturnParam;

#pragma pack(0)

#ifdef __cplusplus
}
#endif

#endif