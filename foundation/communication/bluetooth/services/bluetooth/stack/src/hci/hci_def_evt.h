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

#ifndef HCI_DEF_EVT_H
#define HCI_DEF_EVT_H

#include <stdint.h>

#include "hci_def_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.1 Inquiry Complete Event
#define HCI_INQUIRY_COMPLETE_EVENT 0x01

typedef HciStatusParam HciInquiryCompleteEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.2 Inquiry Result Event
#define HCI_INQUIRY_RESULT_EVENT 0x02

typedef struct {
    HciBdAddr bdAddr;
    uint8_t pageScanRepetitionMode;
    uint8_t reserved1;
    uint8_t reserved2;
    uint8_t classOfDevice[3];
    uint16_t clockOffset;
} HciInquiryResult;

typedef struct {
    uint8_t numResponses;
    HciInquiryResult *responses;
} HciInquiryResultEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.3 Connection Complete Event
#define HCI_CONNECTION_COMPLETE_EVENT 0x03

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
    HciBdAddr bdAddr;
    uint8_t linkType;
    uint8_t encryptionEnabled;
} HciConnectionCompleteEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.4 Connection Request Event
#define HCI_CONNECTION_REQUEST_EVENT 0x04

#define HCI_LINK_TYPE_SCO 0x00
#define HCI_LINK_TYPE_ACL 0x01
#define HCI_LINK_TYPE_ESCO 0x02

typedef struct {
    HciBdAddr bdAddr;
    uint8_t classOfDevice[3];
    uint8_t linkType;
} HciConnectionRequestEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.5 Disconnection Complete Event
#define HCI_DISCONNECT_COMPLETE_EVENT 0x05

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
    uint8_t reason;
} HciDisconnectCompleteEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.6 Authentication Complete Event
#define HCI_AUTHENTICATION_COMPLETE_EVENT 0x06

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
} HciAuthenticationCompleteEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.7 Remote Name Request Complete Event
#define HCI_REMOTE_NAME_REQUEST_COMPLETE_EVENT 0x07

typedef struct {
    uint8_t status;
    HciBdAddr bdAddr;
    uint8_t remoteName[248];
} HciRemoteNameRequestCompleteEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.8 Encryption Change Event
#define HCI_ENCRYPTION_CHANGE_EVENT 0x08

#define LINK_LEVEL_ENCRYPTION_OFF 0x00
#define LINK_LEVEL_ENCRYPTION_ON_E0 0x01
#define LINK_LEVEL_ENCRYPTION_ON_AES_CCM 0x02

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
    uint8_t encryptionEnabled;
} HciEncryptionChangeEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.9 Change Connection Link Key Complete Event
#define HCI_CHANGE_CONNECTION_LINK_KEY_COMPLETE_EVENT 0x09

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
} HciChangeConnectionLinkKeyCompleteEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.10 Master Link Key Complete Event
#define HCI_MASTER_LINK_KEY_COMPLETE_EVENT 0x0A

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
    uint8_t keyFlag;
} HciMasterLinkKeyCompleteEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.11 Read Remote Supported Features Complete Event
#define HCI_READ_REMOTE_SUPPORTED_FEATURES_COMPLETE_EVENT 0x0B

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
    HciLmpFeatures lmpFeatures;
} HciReadRemoteSupportedFeaturesCompleteEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.12 Read Remote Version Information Complete Event
#define HCI_READ_REMOTE_VERSION_INFORMATION_COMPLETE_EVENT 0x0C

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
    uint8_t version;
    uint16_t manufacturerName;
    uint16_t subVersion;
} HciReadRemoteVersionInformationCompleteEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.13 QoS Setup Complete Event
#define HCI_QOS_SETUP_COMPLETE_EVENT 0x0D

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
    uint8_t flags;
    uint8_t serviceType;
    uint32_t tokenRate;
    uint32_t peakBandwidth;
    uint32_t latency;
    uint32_t delayVariation;
} HciQosSetupCompleteEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.14 Command Complete Event
#define HCI_COMMAND_COMPLETE_EVENT 0x0E

typedef struct {
    uint8_t numHciCommandPackets;
    uint16_t commandOpcode;
} HciCommandCompleteEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.15 Command Status Event
#define HCI_COMMAND_STATUS_EVENT 0x0F

typedef struct {
    uint8_t status;
    uint8_t numHciCommandPackets;
    uint16_t commandOpcode;
} HciCommandStatusEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.16 Hardware Error Event
#define HCI_HARDWARE_ERROR_EVENT 0x10

typedef struct {
    uint8_t hardwareCode;
} HciHardwareErrorEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.17 Flush Occurred Event
#define HCI_FLUSH_OCCURRED_EVENT 0x11

typedef struct {
    uint16_t handle;
} HciFlushOccurredEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.18 Role Change Event
#define HCI_ROLE_CHANGE_EVENT 0x12

typedef struct {
    uint8_t status;
    HciBdAddr bdAddr;
    uint8_t newRole;
} HciRoleChangeEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.19 Number Of Completed Packets Event
#define HCI_NUMBER_OF_COMPLETED_PACKETS_EVENT 0x13

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.20 Mode Change Event
#define HCI_MODE_CHANGE_EVENT 0x14

#define ACTIVE_MODE 0x00
#define HOLD_MODE 0x01
#define SNIFF_MODE 0x02

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
    uint8_t currentMode;
    uint16_t interval;
} HciModeChangeEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.21 Return Link Keys Event
#define HCI_RETURN_LINK_KEYS_EVENT 0x15

typedef struct {
    HciBdAddr bdAddr;
    uint8_t linkKey[16];
} HciReturnLinkKeys;

typedef struct {
    uint8_t numKeys;
    HciReturnLinkKeys *responses;
} HciReturnLinkKeysEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.22 PIN Code Request Event
#define HCI_PIN_CODE_REQUEST_EVENT 0x16

typedef struct {
    HciBdAddr bdAddr;
} HciPinCodeRequestEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.23 Link Key Request Event
#define HCI_LINK_KEY_REQUEST_EVENT 0x17

typedef struct {
    HciBdAddr bdAddr;
} HciLinkKeyRequestEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.24 Link Key Notification Event
#define HCI_LINK_KEY_NOTIFICATION_EVENT 0x18

#define COMBINATION_KEY 0x00
#define LOCAL_UNIT_KEY 0x01
#define REMOTE_UNIT_KEY 0x02
#define DEBUG_COMBINATION_KE 0x03
#define UNAUTHENTICATED_COMBINATION_KEY_GENERATED_FROM_P192 0x04
#define AUTHENTICATED_COMBINATION_KEY_GENERATED_FROM_P192 0x05
#define CHANGED_COMBINATION_KEY 0x06
#define UNAUTHENTICATED_COMBINATION_KEY_GENERATED_FROM_P256 0x07
#define AUTHENTICATED_COMBINATION_KEY_GENERATED_FROM_P256 0x08

typedef struct {
    HciBdAddr bdAddr;
    uint8_t linkKey[16];
    uint8_t keyType;
} HciLinkKeyNotificationEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.26 Data Buffer Overflow Event
#define HCI_DATA_BUFFER_OVERFLOW_EVENT 0x1A

typedef struct {
    uint8_t linkType;
} HciDataBufferOverflowEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.27 Max Slots Change Event
#define HCI_MAX_SLOTS_CHANGE_EVENT 0x1B

typedef struct {
    uint16_t connectionHandle;
    uint8_t lmpMaxSlots;
} HciMaxSlotsChangeEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.28 Read Clock Offset Complete Event
#define HCI_READ_CLOCK_OFFSET_COMPLETE_EVENT 0x1C

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
    uint16_t clockOffset;
} HciReadClockOffsetCompleteEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.29 Connection Packet Type Changed Event
#define HCI_CONNECTION_PACKET_TYPE_CHANGED_EVENT 0x1D

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
    uint16_t packetType;
} HciConnectionPacketTypeChangedEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.30 QoS Violation Event
#define HCI_QOS_VIOLATION_EVENT 0x1E

typedef struct {
    uint16_t handle;
} HciQosViolationEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.31 Page Scan Repetition Mode Change Event
#define HCI_PAGE_SCAN_REPETITION_MODE_CHANGE_EVENT 0x20

typedef struct {
    HciBdAddr bdAddr;
    uint8_t pageScanRepetitionMode;
} HciPageScanRepetitionModeChangeEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.32 Flow Specification Complete Event
#define HCI_FLOW_SPECIFICATION_COMPLETE_EVENT 0x21

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
    uint8_t flags;
    uint8_t flowdirection;
    uint8_t serviceType;
    uint32_t tokenRate;
    uint32_t tokenBucketSize;
    uint32_t peakBandwidth;
    uint32_t accessLatency;
} HciFlowSpecificationCompleteEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.33 Inquiry Result with RSSI Event
#define HCI_INQUIRY_RESULT_WITH_RSSI_EVENT 0x22

typedef struct {
    HciBdAddr bdAddr;
    uint8_t pageScanRepetitionMode;
    uint8_t reserved;
    uint8_t classOfDevice[3];
    uint16_t clockOffset;
    uint8_t rssi;
} HciInquiryResultWithRssi;

typedef struct {
    uint8_t numResponses;
    HciInquiryResultWithRssi *responses;
} HciInquiryResultWithRssiEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.34 Read Remote Extended Features Complete Event
#define HCI_READ_REMOTE_EXTENDED_FEATURES_COMPLETE_EVENT 0x23

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
    uint8_t pageNumber;
    uint8_t maximumPageNumber;
    uint8_t extendedLMPFeatures[LMP_FEATURES_SIZE];
} HciReadRemoteExtendedFeaturesCompleteEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.35 Synchronous Connection Complete Event
#define HCI_SYNCHRONOUS_CONNECTION_COMPLETE_EVENT 0x2C

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
    HciBdAddr bdAddr;
    uint8_t linkType;
    uint8_t transmissionInterval;
    uint8_t retransmissionWindow;
    uint16_t rxPacketLength;
    uint16_t txPacketLength;
    uint8_t airMode;
} HciSynchronousConnectionCompleteEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.36 Synchronous Connection Changed Event
#define HCI_SYNCHRONOUS_CONNECTION_CHANGED_EVENT 0x2D

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
    uint8_t transmissionInterval;
    uint8_t retransmissionWindow;
    uint16_t rxPacketLength;
    uint16_t txPacketLength;
} HciSynchronousConnectionChangedEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.37 Sniff Subrating Event
#define HCI_SNIFF_SUBRATING_EVENT 0x2E

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
    uint16_t maximumTransmitLatency;
    uint16_t maximumReceiveLatency;
    uint16_t minimumRemoteTimeout;
    uint16_t minimumLocalTimeout;
} HciSniffSubratingEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.38 Extended Inquiry Result Event
#define HCI_EXTENDED_INQUIRY_RESULT_EVENT 0x2F

typedef struct {
    uint8_t numResponses;
    HciBdAddr bdAddr;
    uint8_t pageScanRepetitionMode;
    uint8_t reserved;
    uint8_t classofDevice[3];
    uint16_t clockOffset;
    uint8_t rssi;
    uint8_t extendedInquiryResponse[240];
} HciExtendedInquiryResultEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.39 Encryption Key Refresh Complete Event
#define HCI_ENCRYPTION_KEY_REFRESH_COMPLETE_EVENT 0x30

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
} HciEncryptionKeyRefreshCompleteEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.40 IO Capability Request Event
#define HCI_IO_CAPABILITY_REQUEST_EVENT 0x31

typedef struct {
    HciBdAddr bdAddr;
} HciIoCapabilityRequestEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.41 IO Capability Response Event
#define HCI_IO_CAPABILITY_RESPONSE_EVENT 0x32

typedef struct {
    HciBdAddr bdAddr;
    uint8_t IOCapability;
    uint8_t OOBDataPresent;
    uint8_t authenticationRequirements;
} HciIoCapabilityResponseEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.42 User Confirmation Request Event
#define HCI_USER_CONFIRMATION_REQUEST_EVENT 0x33

typedef struct {
    HciBdAddr bdAddr;
    uint32_t numericValue;
} HciUserConfirmationRequestEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.43 User Passkey Request Event
#define HCI_USER_PASSKEY_REQUEST_EVENT 0x34

typedef struct {
    HciBdAddr bdAddr;
} HciUserPasskeyRequestEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.44 Remote OOB Data Request Event
#define HCI_REMOTE_OOB_DATA_REQUEST_EVENT 0x35

typedef struct {
    HciBdAddr bdAddr;
} HciRemoteOobDataRequestEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.45 Simple Pairing Complete Event
#define HCI_SIMPLE_PAIRING_COMPLETE_EVENT 0x36

typedef struct {
    uint8_t status;
    HciBdAddr bdAddr;
} HciSimplePairingCompleteEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.46 Link Supervision Timeout Changed Event
#define HCI_LINK_SUPERVISION_TIMEOUT_CHANGED_EVENT 0x38

typedef struct {
    uint16_t connectionHandle;
    uint16_t linkSupervisionTimeout;
} HciLinkSupervisionTimeoutChangedEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.47 Enhanced Flush Complete Event
#define HCI_ENHANCED_FLUSH_COMPLETE_EVENT 0x39

typedef struct {
    uint16_t handle;
} HciEnhancedFlushCompleteEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.48 User Passkey Notification Event
#define HCI_USER_PASSKEY_NOTIFICATION_EVENT 0x3B

typedef struct {
    HciBdAddr bdAddr;
    uint32_t passkey;
} HciUserPasskeyNotificationEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.49 Keypress Notification Event
#define HCI_KEYPRESS_NOTIFICATION_EVENT 0x3C

typedef struct {
    HciBdAddr bdAddr;
    uint8_t notificationType;
} HciKeypressNotificationEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.50 Remote Host Supported Features Notification Event
#define HCI_REMOTE_HOST_SUPPORTED_FEATURES_NOTIFICATION_EVENT 0x3D

typedef struct {
    HciBdAddr addr;
    uint8_t hostSupportedFeatures[8];
} HciRemoteHostSupportedFeaturesNotificationEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.51 Physical Link Complete Event
#define HCI_PHYSICAL_LINK_COMPLETE_EVENT 0x40

typedef struct {
    uint8_t status;
    uint8_t physicalLinkHandle;
} HciPhysicalLinkCompleteEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.52 Channel Selected Event
#define HCI_CHANNEL_SELECTED_EVENT 0x41

typedef struct {
    uint8_t physicalLinkHandle;
} HciChannelSelectedEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.53 Disconnection Physical Link Complete Event
#define HCI_DISCONNECTION_PHYSICAL_LINK_COMPLETE_EVENT 0x42

typedef struct {
    uint8_t status;
    uint8_t physicalLinkHandle;
    uint8_t reason;
} HciDisconnectionPhysicalLinkCompleteEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.54 Physical Link Loss Early Warning Event
#define HCI_PHYSICAL_LINK_LOSS_EARLY_WARNING_EVENT 0x43

typedef struct {
    uint8_t physicalLinkHandle;
    uint8_t linkLossReason;
} HciPhysicalLinkLossEarlyWarningEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.55 Physical Link Recovery Event
#define HCI_PHYSICAL_LINK_RECOVERY_EVENT 0x44

typedef struct {
    uint8_t physicalLinkHandle;
} HciPhysicalLinkRecoveryEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.56 Logical Link Complete Event
#define HCI_LOGICAL_LINK_COMPLETE_EVENT 0x45

typedef struct {
    uint8_t status;
    uint16_t logicalLinkHandle;
    uint8_t physicalLinkHandle;
    uint8_t txFlowSpecID;
} HciLogicalLinkCompleteEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.57 Disconnection Logical Link Complete Event
#define HCI_DISCONNECTION_LOGICAL_LINK_COMPLETE_EVENT 0x46

typedef struct {
    uint8_t status;
    uint16_t logicalLinkHandle;
    uint8_t reason;
} HciDisconnectionLogicalLinkCompleteEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.58 Flow Spec Modify Complete Event
#define HCI_FLOW_SPEC_MODIFY_COMPLETE_EVENT 0x47

typedef struct {
    uint8_t status;
    uint16_t handle;
} HciFlowSpecModifyCompleteEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.59 Number Of Completed Data Blocks Event
#define HCI_NUMBER_OF_COMPLETED_DATA_BLOCKS_EVENT 0x48

typedef struct {
    uint16_t handle;
    uint16_t numOfCompletedPackets;
    uint16_t numOfCompletedBlocks;
} HciNumberOfCompletedDataBlocks;

typedef struct {
    uint16_t totalNumDataBlocks;
    uint8_t NumberofHandles;
    HciNumberOfCompletedDataBlocks *responses;
} HciNumberOfCompletedDataBlocksEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.60 Short Range Mode Change Complete Event
#define HCI_SHORT_RANGE_MODE_CHANGE_COMPLETE_EVENT 0x4C

typedef struct {
    uint8_t status;
    uint8_t physicalLinkHandle;
    uint8_t shortRangeModeState;
} HciShortRangeModeChangeCompleteEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.61 AMP Status Change Event
#define HCI_AMP_STATUS_CHANGE_EVENT 0x4D

typedef struct {
    uint8_t status;
    uint8_t ampStatus;
} HciAmpStatusChangeEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.62 AMP Start Test Event
#define HCI_AMP_START_TEST_EVENT 0x49

typedef struct {
    uint8_t status;
    uint8_t testScenario;
} HciAmpStartTestEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.63 AMP Test End Event
#define HCI_AMP_TEST_END_EVENT 0x4A

typedef struct {
    uint8_t status;
    uint8_t testScenario;
} HciAmpTestEndEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.64 AMP Receiver Report Event
#define HCI_AMP_RECEIVER_REPORT_EVENT 0x4B

typedef struct {
    uint8_t controllerType;
    uint8_t reason;
    uint32_t eventType;
    uint16_t numberOfFrames;
    uint16_t numberOfErrorFrames;
    uint32_t numberOfBits;
    uint32_t numberOfErrorBits;
} HciAmpReceiverReportEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.65 LE Meta Event
#define HCI_LE_META_EVENT 0x3E

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.66 Triggered Clock Capture Event
#define HCI_TRIGGERED_CLOCK_CAPTURE_EVENT 0x4E

typedef struct {
    uint16_t connectionHandle;
    uint8_t whichClock;
    uint32_t clock;
    uint16_t slotOffset;
} HciTriggeredClockCaptureEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.67 Synchronization Train Complete Event
#define HCI_SYNCHRONIZATION_TRAIN_COMPLETE_EVENT 0x4F

typedef struct {
    uint8_t status;
} HciSynchronizationTrainCompleteEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.68 Synchronization Train Received Event
#define HCI_SYNCHRONIZATION_TRAIN_RECEIVED_EVENT 0x50

typedef struct {
    uint8_t status;
    HciBdAddr bdAdddr;
    uint32_t clockOffset;
    uint8_t afhChannelMap[10];
    uint8_t ltAddr;
    uint32_t nextBroadcastInstant;
    uint16_t connectionlessSlaveBroadcastInterval;
    uint8_t serviceData;
} HciSynchronizationTrainReceivedEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.69 Connectionless Slave Broadcast Receive Event
#define HCI_CONNECTIONLESS_SLAVE_BROADCAST_RECEIVE_EVENT 0x51

typedef struct {
    HciBdAddr bdAddr;
    uint8_t ltAddr;
    uint32_t clk;
    uint32_t offset;
    uint8_t receiveStatus;
    uint8_t fragment;
    uint8_t dataLength;
    uint8_t *data;
} HciConnectionlessSlaveBroadcastReceiveEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.70 Connectionless Slave Broadcast Timeout Event
#define HCI_CONNECTIONLESS_SLAVE_BROADCAST_TIMEOUT_EVENT 0x52

typedef struct {
    HciBdAddr bdAddr;
    uint8_t ltAddr;
} HciConnectionlessSlaveBroadcastTimeoutEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.71 Truncated Page Complete Event
#define HCI_TRUNCATED_PAGE_COMPLETE_EVENT 0x53

typedef struct {
    uint8_t status;
    HciBdAddr bdAddr;
} HciTruncatedPageCompleteEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.72 Slave Page Response Timeout Event
#define HCI_SLAVE_PAGE_RESPONSE_TIMEOUT_EVENT 0x54

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.73 Connectionless Slave Broadcast Channel Map Change Event
#define HCI_CONNECTIONLESS_SLAVE_BROADCAST_CHANNEL_MAP_CHANGE_EVENT 0x55

typedef struct {
    uint8_t channelMap[10];
} HciConnectionlessSlaveBroadcastChannelMapChangeEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.74 Inquiry Response Notification Event
#define HCI_INQUIRY_RESPONSE_NOTIFICATION_EVENT 0x56

typedef struct {
    uint8_t lap : 3;
    uint8_t rssi;
} HciInquiryResponseNotificationEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.76 SAM Status Change Event
#define HCI_SAM_STATUS_CHANGE_EVENT 0x58

typedef struct {
    uint16_t connectionHandle;
    uint8_t localSamIndex;
    uint8_t localSamTxAvailability;
    uint8_t localSamRxAvailability;
    uint8_t remoteSamIndex;
    uint8_t remoteSamTxAvailability;
    uint8_t remoteSamRxAvailability;
} HciSamStatusChangeEventParam;

#pragma pack(0)

#ifdef __cplusplus
}
#endif

#endif