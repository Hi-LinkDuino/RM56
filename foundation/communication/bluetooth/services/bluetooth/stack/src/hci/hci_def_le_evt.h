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

#ifndef HCI_DEF_LE_EVT_H
#define HCI_DEF_LE_EVT_H

#include <stdint.h>

#include "hci_def_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.65.1 LE Connection Complete Event
#define HCI_LE_CONNECTION_COMPLETE_EVENT 0x01

#define HCI_PEER_ADDR_TYPE_PUBLIC 0x00
#define HCI_PEER_ADDR_TYPE_RANDOM 0x01

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
    uint8_t role;
    uint8_t peerAddressType;
    HciBdAddr peerAddress;
    uint16_t connInterval;
    uint16_t connLatency;
    uint16_t supervisionTimeout;
    uint8_t masterClockAccuracy;
} HciLeConnectionCompleteEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.65.2 LE Advertising Report Event
#define HCI_LE_ADVERTISING_REPORT_EVENT 0x02

typedef struct {
    uint8_t eventType;
    uint8_t addressType;
    HciBdAddr address;
    uint8_t lengthData;
    uint8_t *data;
    uint8_t rssi;
} HciLeAdvertisingReport;

typedef struct {
    uint8_t numReports;
    HciLeAdvertisingReport *reports;
} HciLeAdvertisingReportEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.65.3 LE Connection Update Complete Event
#define HCI_LE_CONNECTION_UPDATE_COMPLETE_EVENT 0x03

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
    uint16_t connInterval;
    uint16_t connLatency;
    uint16_t supervisionTimeout;
} HciLeConnectionUpdateCompleteEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.65.4 LE Read Remote Features Complete Event
#define HCI_LE_READ_REMOTE_FEATURES_COMPLETE_EVENT 0x04

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
    HciLeFeatures leFeatures;
} HciLeReadRemoteFeaturesCompleteEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.65.5 LE Long Term Key Request Event
#define HCI_LE_LONG_TERM_KEYREQUEST_EVENT 0x05

typedef struct {
    uint16_t connectionHandle;
    uint8_t randomNumber[8];
    uint16_t encryptedDiversifier;
} HciLeLongTermKeyRequestEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.65.6 LE Remote Connection Parameter Request Event
#define HCI_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_EVENT 0x06

typedef struct {
    uint16_t connectionHandle;
    uint16_t intervalMin;
    uint16_t IntervalMax;
    uint16_t latency;
    uint16_t timeout;
} HciLeRemoteConnectionParameterRequestEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.65.7 LE Data Length Change Event
#define HCI_LE_DATA_LENGTH_CHANGE_EVENT 0x07

typedef struct {
    uint8_t subeventCode;
    uint16_t connectionHandle;
    uint16_t maxTxOctets;
    uint16_t maxTxTime;
    uint16_t maxRxOctets;
    uint16_t maxRxTime;
} HciLeDataLengthChangeEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.65.8 LE Read Local P-256 Public Key Complete Event
#define HCI_LE_READ_LOCAL_P256_PUBLIC_KEY_COMPLETE_EVENT 0x08

typedef struct {
    uint8_t status;
    uint8_t localP256PublicKey[64];
} HciLeReadLocalP256PublicKeyCompleteEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.65.9 LE Generate DHKey Complete Event
#define HCI_LE_GENERATE_DHKEY_COMPLETE_EVENT 0x09

typedef struct {
    uint8_t status;
    uint8_t DHKey[32];
} HciLeGenerateDHKeyCompleteEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.65.10 LE Enhanced Connection Complete Event
#define HCI_LE_ENHANCED_CONNECTION_COMPLETE_EVENT 0x0A

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
    uint8_t role;
    uint8_t peerAddressType;
    HciBdAddr peerAddress;
    HciBdAddr localResolvablePrivateAddress;
    HciBdAddr peerResolvablePrivateAddress;
    uint16_t connInterval;
    uint16_t connLatency;
    uint16_t supervisionTimeout;
    uint8_t masterClockAccuracy;
} HciLeEnhancedConnectionCompleteEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.65.11 LE Directed Advertising Report Event
#define HCI_LE_DIRECTED_ADVERTISING_REPORT_EVENT 0x0B

typedef struct {
    uint8_t eventType;
    uint8_t addressType;
    HciBdAddr address;
    uint8_t directAddressType;
    HciBdAddr directAddress;
    uint8_t rssi;
} HciLeDirectedAdvertisingReport;

typedef struct {
    uint8_t numReports;
    HciLeDirectedAdvertisingReport *reports;
} HciLeDirectedAdvertisingReportEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.65.12 LE PHY Update Complete Event
#define HCI_LE_PHY_UPDATE_COMPLETE_EVENT 0x0C

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
    uint8_t txPhy;
    uint8_t rxPhy;
} HciLePhyUpdateCompleteEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.65.13 LE Extended Advertising Report Event
#define HCI_LE_EXTENDED_ADVERTISING_REPORT_EVENT 0x0D

#define HCI_PDU_TYPE_ADV_IND 4105
#define HCI_PDU_TYPE_ADV_DIRECT_IND 4161
#define HCI_PDU_TYPE_ADV_SCAN_IND 4104
#define HCI_PDU_TYPE_ADV_NONCONN_IND 4096
#define HCI_PDU_TYPE_SCAN_RSP_TO_AN_ADV_IND 4617
#define HCI_PDU_TYPE_SCAN_RSP_TO_AN_ADV_SCAN_IND 4616

typedef struct {
    uint16_t eventType;
    uint8_t addressType;
    HciBdAddr address;
    uint8_t primaryPHY;
    uint8_t secondaryPHY;
    uint8_t advertisingSID;
    uint8_t txPower;
    uint8_t rssi;
    uint16_t periodicAdvertisingInterval;
    uint8_t directAddressType;
    HciBdAddr directAddress;
    uint8_t dataLength;
    uint8_t *data;
} HciLeExtendedAdvertisingReport;

typedef struct {
    uint8_t numReports;
    HciLeExtendedAdvertisingReport *reports;
} HciLeExtendedAdvertisingReportEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.65.14 LE Periodic Advertising Sync Established Event
#define HCI_LE_PERIODIC_ADVERTISING_SYNC_ESTABLISHED_EVENT 0x0E

typedef struct {
    uint8_t status;
    uint8_t advertisingHandle;
    uint16_t connectionHandle;
    uint8_t numCompletedExtendedAdvertisingEvents;
} HciLePeriodicAdvertisingSyncEstablishedEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.65.15 LE Periodic Advertising Report Event
#define HCI_LE_PERIODIC_ADVERTISING_REPORT_EVENT 0x0F

typedef struct {
    uint8_t subeventCode;
    uint16_t syncHandle;
    uint8_t txPower;
    uint8_t rssi;
    uint8_t unused;
    uint8_t dataStatus;
    uint8_t dataLength;
    uint8_t *data;
} HciLePeriodicAdvertisingReportEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.65.16 LE Periodic Advertising Sync Lost Event
#define HCI_LE_PERIODIC_ADVERTISING_SYNC_LOST_EVENT 0x10

typedef struct {
    uint8_t subeventCode;
    uint16_t syncHandle;
} HciLePeriodicAdvertisingSyncLostEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.65.17 LE Scan Timeout Event
#define HCI_LE_SCAN_TIMEOUT_EVENT 0x11

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.65.18 LE Advertising Set Terminated Event
#define HCI_LE_ADVERTISING_SET_TERMINATED_EVENT 0x12

typedef struct {
    uint8_t status;
    uint8_t advertisingHandle;
    uint16_t connectionHandle;
    uint8_t numCompletedExtendedAdvertisingEvents;
} HciLeAdvertisingSetTerminatedEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.65.19 LE Scan Request Received Event
#define HCI_LE_SCAN_REQUEST_RECEIVED_EVENT 0x13

typedef struct {
    uint8_t advertisingHandle;
    uint8_t scannerAddressType;
    HciBdAddr scannerAddress;
} HciLeScanRequestReceivedEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.65.20 LE Channel Selection Algorithm Event
#define HCI_LE_CHANNEL_SELECTION_ALGORITHM_EVENT 0x14

typedef struct {
    uint16_t connectionHandle;
    uint8_t ChannelSelectionAlgorithm;
} HciLeChannelSelectionAlgorithmEventParam;

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.7.75 Authenticated Payload Timeout Expired Event
#define HCI_AUTHENTICATED_PAYLOAD_TIMEOUT_EXPIRED_EVENT 0x57

typedef struct {
    uint16_t connectionHandle;
} HciAuthenticatedPayloadTimeoutExpiredEventParam;

#pragma pack(0)

#ifdef __cplusplus
}
#endif

#endif