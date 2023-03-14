/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef BLE_DEFS_H
#define BLE_DEFS_H

#include <string>

#include "bt_def.h"
#include "bt_uuid.h"
#include "gap_le_if.h"

/*
 * @brief The bluetooth system.
 */
namespace OHOS {
namespace bluetooth {
/// const
constexpr uint8_t BLE_IRK_HEX_ELN = 0x10;
constexpr uint8_t BLE_IRK_RAND_HEX_LEN = 0x03;
constexpr uint8_t BLE_IRK_RAND_ELN = 0x1F;
constexpr uint8_t BLE_IRK_RAND_LEFT_SHIFT = 0x04;
constexpr uint8_t SHORTENED_LOCAL_NAME = 0X08;
constexpr uint8_t COMPLETE_LOCAL_NAME = 0X09;
constexpr uint8_t MANUFACTURER_SPECIFIC_DATA = 0XFF;
constexpr uint8_t TX_POWER_LEVEL = 0x0A;
constexpr uint8_t SERVICE_DATA_16_BIT_UUID = 0X16;
constexpr uint8_t SERVICE_DATA_32_BIT_UUID = 0X20;
constexpr uint8_t SERVICE_DATA_128_BIT_UUID = 0X21;
constexpr uint8_t BLE_DIV_RESULT_TWO = 0x02;

/// Maximum advertising data length that can fit in a legacy PDU.
constexpr uint8_t BLE_LEGACY_ADV_DATA_LEN_MAX = 0x1F;
constexpr uint8_t BLE_LEGACY_SCAN_RSP_DATA_LEN_MAX = 0x1F;
constexpr uint8_t BLE_EX_ADV_PAYLOAD_DATA_LEN = 0XBF;

/// Regular scan params
constexpr uint16_t BLE_SCAN_MODE_LOW_POWER_WINDOW_MS = 512;
constexpr uint16_t BLE_SCAN_MODE_LOW_POWER_INTERVAL_MS = 5120;
constexpr uint16_t BLE_SCAN_MODE_BALANCED_WINDOW_MS = 1024;
constexpr uint16_t BLE_SCAN_MODE_BALANCED_INTERVAL_MS = 4096;
constexpr uint16_t BLE_SCAN_MODE_LOW_LATENCY_WINDOW_MS = 4096;
constexpr uint16_t BLE_SCAN_MODE_LOW_LATENCY_INTERVAL_MS = 4096;
constexpr uint16_t BLE_SCAN_MODE_OP_P2_60_3000_WINDOW_MS = 60;
constexpr uint16_t BLE_SCAN_MODE_OP_P2_60_3000_INTERVAL_MS = 3000;
constexpr uint16_t BLE_SCAN_MODE_OP_P10_60_600_WINDOW_MS = 60;
constexpr uint16_t BLE_SCAN_MODE_OP_P10_60_600_INTERVAL_MS = 600;
constexpr uint16_t BLE_SCAN_MODE_OP_P25_60_240_WINDOW_MS = 60;
constexpr uint16_t BLE_SCAN_MODE_OP_P25_60_240_INTERVAL_MS = 240;
constexpr uint16_t BLE_SCAN_MODE_OP_P100_1000_1000_WINDOW_MS = 1000;
constexpr uint16_t BLE_SCAN_MODE_OP_P100_1000_1000_INTERVAL_MS = 1000;

/// Batch scan params
constexpr uint16_t BLE_SCAN_MODE_BATCH_LOW_POWER_WINDOW_MS = 500;
constexpr uint16_t BLE_SCAN_MODE_BATCH_LOW_POWER_INTERVAL_MS = 45000;
constexpr uint16_t BLE_SCAN_MODE_BATCH_BALANCED_WINDOW_MS = 1000;
constexpr uint16_t BLE_SCAN_MODE_BATCH_BALANCED_INTERVAL_MS = 45000;
constexpr uint16_t BLE_SCAN_MODE_BATCH_LOW_LATENCY_WINDOW_MS = 1500;
constexpr uint16_t BLE_SCAN_MODE_BATCH_LOW_LATENCY_INTERVAL_MS = 45000;
constexpr uint16_t BLE_SCAN_MODE_BATCH_OP_P2_60_3000_WINDOW_MS = 60;
constexpr uint16_t BLE_SCAN_MODE_BATCH_OP_P2_60_3000_INTERVAL_MS = 3000;
constexpr uint16_t BLE_SCAN_MODE_BATCH_OP_P10_60_600_WINDOW_MS = 60;
constexpr uint16_t BLE_SCAN_MODE_BATCH_OP_P10_60_600_INTERVAL_MS = 600;
constexpr uint16_t BLE_SCAN_MODE_BATCH_OP_P25_60_240_WINDOW_MS = 60;
constexpr uint16_t BLE_SCAN_MODE_BATCH_OP_P25_60_240_INTERVAL_MS = 240;
constexpr uint16_t BLE_SCAN_MODE_BATCH_OP_P100_1000_1000_WINDOW_MS = 1000;
constexpr uint16_t BLE_SCAN_MODE_BATCH_OP_P100_1000_1000_INTERVAL_MS = 1000;
constexpr float BLE_SCAN_UNIT_TIME = 0.625;

/// Invalid MAC Address
const std::string BLE_INVALID_MAC_ADDRESS = "00:00:00:00:00:00";
const std::string BLE_DEFAULT_DEVICE_NAME = "BluetoothDevice";
constexpr uint8_t BLE_DEFAULT_IO = 0x01;
const std::string BLE_DEFAULT_LOCAL_PASSKEY = "0x12346";
constexpr uint8_t BLE_DEFAULT_ROLES = 0x0F;
constexpr uint8_t BLE_DEFAULT_MODEL1_LEVEL = 0x00;
constexpr uint8_t BLE_DEFAULT_MODEL2_LEVEL = 0x00;
constexpr bool BLE_DEFAULT_SECURITY = false;
constexpr uint8_t BLE_DEFAULT_SCAN_MODE = 0x03;

constexpr uint8_t BLE_NON_RES_PRI_ADDR = 0x3F;
constexpr uint8_t BLE_STATIC_PRI_ADDR = 0xC0;

constexpr int BTM_ACL_DISCONNECT_REASON = 0x13;
constexpr int BLE_THREAD_WAIT_TIMEOUT = 5;

constexpr int BLE_CHANGE_RPA_ADDRESS_INTERVAL = 15 * 60 * 1000;
constexpr uint16_t GATT_UUID_GAP_DEVICE_NAME = 0x2A00;

const Uuid::UUID128Bit DEFAULT_UUID_MASK = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

/// Filter status
typedef enum {
    BLE_SCAN_FILTER_STATUS_IDLE = 0,
    BLE_SCAN_FILTER_STATUS_WORKING,
    BLE_SCAN_FILTER_STATUS_BAD,
} FILTER_STATUS;

/// Advertising status
typedef enum {
    ADVERTISE_NOT_STARTED = -1,
    ADVERTISE_SUCCESS = 0,
    ADVERTISE_FAILED_DATA_TOO_LARGE,
    ADVERTISE_FAILED_TOO_MANY_ADVERTISERS,
    ADVERTISE_FAILED_ALREADY_STARTED,
    ADVERTISE_FAILED_INTERNAL_ERROR,
    ADVERTISE_FAILED_FEATURE_UNSUPPORTED
} ADVERTISE_STATUS;

/// Local config
typedef enum {
    BLE_CONFIG_LOCAL_NAME = 0x00,
    BLE_CONFIG_LOCAL_ADDRESS = 0x01,
    BLE_CONFIG_ADAPTER_BONDED_MODE = 0x02,
} BLE_CONFIG;

/// ble bondable mode
typedef enum { BLE_BONDABLE_MODE_NONE = 0x00, BLE_BONDABLE_MODE_ON = 0x01 } BLE_BONDABLE_MODE;

/// Scan callback type
typedef enum { CALLBACK_TYPE_ALL_MATCHES = 0x00, CALLBACK_TYPE_FIRST_MATCH = 0x01 } CALLBACK_TYPE;

/// stop all adv type
typedef enum {
    STOP_ADV_TYPE_SINGLE = 0x00,
    STOP_ADV_TYPE_ALL = 0x01,
    STOP_ADV_TYPE_RESOLVING_LIST = 0x02
} STOP_ALL_ADV_TYPE;

typedef enum { STOP_SCAN_TYPE_NOR = 0x00, STOP_SCAN_TYPE_RESOLVING_LIST = 0x01 } STOP_SCAN_TYPE;

/// Scan status
typedef enum {
    SCAN_NOT_STARTED = -1,
    SCAN_SUCCESS = 0,
    SCAN_FAILED_ALREADY_STARTED,
    SCAN_FAILED_APPLICATION_REGISTRATION_FAILED,
    SCAN_FAILED_INTERNAL_ERROR,
    SCAN_FAILED_FEATURE_UNSUPPORTED,
    SCAN_FAILED_OUT_OF_HARDWARE_RESOURCES
} SCAN_STATUS;

typedef enum {
    BLE_APPEARANCE_UNKNOWN = 0x0000,
    BLE_APPEARANCE_GENERIC_PHONE = 0x0040,
    BLE_APPEARANCE_GENERIC_COMPUTER = 0x0080,
    BLE_APPEARANCE_GENERIC_WATCH = 0x00C0,
    BLE_APPEARANCE_SPORTS_WATCH = 0x00C1,
    BLE_APPEARANCE_GENERIC_CLOCK = 0x0100,
    BLE_APPEARANCE_GENERIC_DISPLAY = 0x0140,
    BLE_APPEARANCE_GENERIC_REMOTE = 0x0180,
    BLE_APPEARANCE_GENERIC_EYEGLASSES = 0x01C0,
    BLE_APPEARANCE_GENERIC_TAG = 0x0200,
    BLE_APPEARANCE_GENERIC_KEYRING = 0x0240,
    BLE_APPEARANCE_GENERIC_MEDIA_PLAYER = 0x0280,
    BLE_APPEARANCE_GENERIC_BARCODE_SCANNER = 0x02C0,
    BLE_APPEARANCE_GENERIC_THERMOMETER = 0x0300,
    BLE_APPEARANCE_THERMOMETER_EAR = 0x0301,
    BLE_APPEARANCE_GENERIC_HEART_RATE = 0x0340,
    BLE_APPEARANCE_HEART_RATE_BELT = 0x0341,
    BLE_APPEARANCE_GENERIC_BLOOD_PRESSURE = 0x0380,
    BLE_APPEARANCE_BLOOD_PRESSURE_ARM = 0x0381,
    BLE_APPEARANCE_BLOOD_PRESSURE_WRIST = 0x0382,
    BLE_APPEARANCE_GENERIC_HID = 0x03C0,
    BLE_APPEARANCE_HID_KEYBOARD = 0x03C1,
    BLE_APPEARANCE_HID_MOUSE = 0x03C2,
    BLE_APPEARANCE_HID_JOYSTICK = 0x03C3,
    BLE_APPEARANCE_HID_GAMEPAD = 0x03C4,
    BLE_APPEARANCE_HID_DIGITIZER_TABLET = 0x03C5,
    BLE_APPEARANCE_HID_CARD_READER = 0x03C6,
    BLE_APPEARANCE_HID_DIGITAL_PEN = 0x03C7,
    BLE_APPEARANCE_HID_BARCODE_SCANNER = 0x03C8,
    BLE_APPEARANCE_GENERIC_GLUCOSE = 0x0400,
    BLE_APPEARANCE_GENERIC_WALKING = 0x0440,
    BLE_APPEARANCE_WALKING_IN_SHOE = 0x0441,
    BLE_APPEARANCE_WALKING_ON_SHOE = 0x0442,
    BLE_APPEARANCE_WALKING_ON_HIP = 0x0443,
    BLE_APPEARANCE_GENERIC_CYCLING = 0x0480,
    BLE_APPEARANCE_CYCLING_COMPUTER = 0x0481,
    BLE_APPEARANCE_CYCLING_SPEED = 0x0482,
    BLE_APPEARANCE_CYCLING_CADENCE = 0x0483,
    BLE_APPEARANCE_CYCLING_POWER = 0x0484,
    BLE_APPEARANCE_CYCLING_SPEED_CADENCE = 0x0485,
    BLE_APPEARANCE_GENERIC_PULSE_OXIMETER = 0x0C40,
    BLE_APPEARANCE_PULSE_OXIMETER_FINGERTIP = 0x0C41,
    BLE_APPEARANCE_PULSE_OXIMETER_WRIST = 0x0C42,
    BLE_APPEARANCE_GENERIC_WEIGHT = 0x0C80,
    BLE_APPEARANCE_GENERIC_PERSONAL_MOBILITY_DEVICE = 0x0CC0,
    BLE_APPEARANCE_POWERED_WHEELCHAIR = 0x0CC1,
    BLE_APPEARANCE_MOBILITY_SCOOTER = 0x0CC2,
    BLE_APPEARANCE_GENERIC_CONTINUOUS_GLUCOSE_MONITOR = 0x0D00,
    BLE_APPEARANCE_GENERIC_INSULIN_PUMP = 0x0D40,
    BLE_APPEARANCE_INSULIN_PUMP_DURABLE_PUMP = 0x0D41,
    BLE_APPEARANCE_INSULIN_PUMP_PATCH_PUMP = 0x0D44,
    BLE_APPEARANCE_INSULIN_PEN = 0x0D48,
    BLE_APPEARANCE_GENERIC_MEDICATION_DELIVERY = 0x0D80,
    BLE_APPEARANCE_GENERIC_OUTDOOR_SPORTS = 0x1440,
    BLE_APPEARANCE_OUTDOOR_SPORTS_LOCATION = 0x1441,
    BLE_APPEARANCE_OUTDOOR_SPORTS_LOCATION_AND_NAV = 0x1442,
    BLE_APPEARANCE_OUTDOOR_SPORTS_LOCATION_POD = 0x1443,
    BLE_APPEARANCE_OUTDOOR_SPORTS_LOCATION_POD_AND_NAV = 0x1444
} BLE_APPEARANCE;

/// GAP BLE callback event type
typedef enum {
    BLE_GAP_ADV_DATA_SET_COMPLETE_EVT = 0,       /// advertising data set complete
    BLE_GAP_ADV_SCAN_RSP_DATA_SET_COMPLETE_EVT,  /// scan response data set complete
    BLE_GAP_ADV_PARAM_SET_COMPLETE_EVT,          /// advertising parameter set complete
    BLE_GAP_ADV_READ_TXPOWER_EVT,                /// advertising read tx power
    BLE_GAP_ADV_START_COMPLETE_EVT,              /// start advertising complete
    BLE_GAP_ADV_STOP_COMPLETE_EVT,               /// stop adv complete
    BLE_GAP_ADV_DATA_RAW_SET_COMPLETE_EVT,       /// raw advertising data set complete

    BLE_GAP_EX_ADV_SET_RAND_ADDR_RESULT_EVT,           /// extend advertising data set complete
    BLE_GAP_EX_ADV_PARAM_SET_COMPLETE_EVT,             /// extend advertising parameter set complete
    BLE_GAP_EX_ADV_DATA_SET_COMPLETE_EVT,              /// extend advertising data set complete
    BLE_GAP_EX_ADV_SCAN_RSP_DATA_SET_COMPLETE_EVT,     /// extend scan response data set complete
    BLE_GAP_EX_ADV_START_COMPLETE_EVT,                 /// start extend advertising complete
    BLE_GAP_EX_ADV_STOP_COMPLETE_EVT,                  /// stop extend advertising complete
    BLE_GAP_EX_ALL_ADV_STOP_COMPLETE_EVT,              /// stop all extend advertising complete
    BLE_GAP_EX_RESOLVING_LIST_ADV_STOP_COMPLETE_EVT,   /// stop all resolving list extend advertising complete
    BLE_GAP_EX_RESOLVING_LIST_ADV_START_COMPLETE_EVT,  /// start all resolving list extend advertising complete
    BLE_GAP_EX_ADV_REMOVE_HANDLE_RESULT_EVT,           /// extend advertising remove handle complete
    BLE_GAP_EX_ADV_CLEAR_HANDLE_RESULT_EVT,            /// extend advertising clear handle complete
    BLE_GAP_EX_ADV_SCAN_REQUEST_RECEIVED_EVT,          /// extend advertising scan request received complete
    BLE_GAP_EX_ADC_TERMINATED_ADV_SET_EVT,             /// extend advertising terminated adv set event

    BLE_GAP_SCAN_PARAM_SET_COMPLETE_EVT,                 /// scan parameters set complete
    BLE_GAP_SCAN_RESULT_EVT,                             /// one scan result ready each time
    BLE_GAP_SCAN_DELAY_REPORT_RESULT_EVT,                /// one scan result ready each time
    BLE_GAP_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT,          /// raw scan response data set complete
    BLE_GAP_SCAN_START_COMPLETE_EVT,                     /// start scan complete
    BLE_GAP_SCAN_STOP_COMPLETE_EVT,                      /// stop scan complete
    BLE_GAP_RESOLVING_LIST_ADV_SCAN_STOP_COMPLETE_EVT,   /// stop all resolvint list scan complete
    BLE_GAP_RESOLVING_LIST_ADV_SCAN_START_COMPLETE_EVT,  /// start all resolvint list scan complete

    BLE_GAP_EX_SCAN_PARAM_SET_COMPLETE_EVT,                 /// scan parameters set complete
    BLE_GAP_EX_SCAN_RESULT_EVT,                             /// one scan result ready each time
    BLE_GAP_EX_SCAN_DELAY_REPORT_RESULT_EVT,                /// one scan result ready each time
    BLE_GAP_EX_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT,          /// raw scan response data set complete
    BLE_GAP_EX_SCAN_START_COMPLETE_EVT,                     /// start scan complete
    BLE_GAP_EX_SCAN_STOP_COMPLETE_EVT,                      /// stop scan complete
    BLE_GAP_EX_RESOLVING_LIST_ADV_SCAN_STOP_COMPLETE_EVT,   /// stop all resolvint list scan complete
    BLE_GAP_EX_RESOLVING_LIST_ADV_SCAN_START_COMPLETE_EVT,  /// start all resolvint list scan complete

    BLE_GATT_DEVICE_INFO_REGISTER_STSTUS,  /// device information
    BLE_GATT_GAP_SERVICE_REGISTER_STSTUS,  /// gap service

    BLE_GAP_ENCRYPTION_COMPLETE_EVT,
    BLE_GAP_LE_LOCAL_ENCRYPTION_KEY_REQ_EVT,
    BLE_GAP_LE_REMOTE_ENCRYPTION_KEY_REQ_EVT,
    BLE_GAP_LE_SIGN_COUNTER_CHANGE_NOTIF_EVT,
    BLE_GAP_LE_REQ_SIGNING_ALGORITHM_INFO_EVT,
    BLE_GAP_LE_PAIR_FEATURE_REQ_EVT,
    BLE_GAP_LE_PAIR_FEATURE_IND_EVT,
    BLE_GAP_LE_PAIR_METHOD_NOTI_EVT,
    BLE_GAP_LE_PAIR_KEY_PRESS_NOTI_EVT,
    BLE_GAP_LE_PAIR_PASSKEY_REQ_EVT,
    BLE_GAP_LE_PAIR_PASSKEY_NOTI_EVT,
    BLE_GAP_LE_PAIR_OOB_REQ_EVT,
    BLE_GAP_LE_PAIR_SC_OOB_REQ_EVT,
    BLE_GAP_LE_PAIR_SC_USER_CONFIRM_REQ_EVT,
    BLE_GAP_LE_PAIR_COMELETE_EVT,
    BLE_GAP_LE_PAIR_KEY_NOTI_EVT,
    BLE_GAP_LE_REQUEST_SECURITY_RESULT,
} BLE_GAP_CB_EVENT;

typedef union {
    struct EncryptionComplete {
        uint8_t status;
        BtAddr peerAddr;
    } encryptionComplete_;

    struct LeLocalEncryptionKeyReqEvent {
        BtAddr addr;
        uint64_t rand;
        uint16_t ediv;
    } leLocalEncryptionKeyReqEvent_;

    struct LeRemoteEncryptionKeyReqEvent {
        BtAddr addr;
    } leRemoteEncryptionKeyReqEvent_;

    struct LeSignCounterChangeNotification {
        BtAddr addr;
        GAP_SignCounterType type;
        uint32_t counter;
    } leSignCounterChangeNotification_;

    struct GapRequestSigningAlgorithmInfo {
        BtAddr addr;
    } gapRequestSigningAlgorithmInfo_;

    struct LePairFeatureReq {
        BtAddr peerAddr;
        bool localPair;
    } lePairFeatureReq_;

    struct LePairFeatureInd {
        BtAddr addr;
        GapLePairFeature remoteFrature;
    } lePairFeatureInd_;

    struct LePairMethodNotify {
        BtAddr addr;
        uint8_t pairMethod;
    } lePairMethodNotify_;

    struct LePairKeyPressNotification {
        BtAddr addr;
        uint8_t pressType;
    } lePairKeyPressNotification_;

    struct LePairPassKeyReq {
        BtAddr addr;
    } lePairPassKeyReq_;

    struct LePairPassKeyNotification {
        BtAddr addr;
        uint32_t number;
    } lePairPassKeyNotification_;

    struct LePairOobReq {
        BtAddr addr;
    } lePairOobReq_;

    struct LePairScOobReq {
        BtAddr addr;
    } lePairScOob_req_;

    struct LePairScUserConfirmReq {
        BtAddr addr;
        uint32_t number;
    } lePairScUserConfirmReq_;

    struct LePairComplete {
        BtAddr addr;
        uint8_t result;
        uint8_t keyType;
    } lePairComplete_;

    struct LePairKeyNotify {
        BtAddr addr;
        LeEncKey remoteEncKey;
        bool hasRemoteEncKey;
        LeIdKey remoteIdKey;
        bool hasRemoteIdKey;
        LeSignKey remoteSignKey;
        bool hasRemoteSignKey;
        LeEncKey localEncKey;
        bool hasLocalEncKey;
        LeSignKey localSignKey;
        bool hasLocalSignKey;
    } lePairKeyNotify_;

    struct LeRequestSecurityResult {
        BtAddr addr;
        uint8_t result;
        GAP_LeSecurityStatus status;
    } leRequestSecurityResult_;
} BleGapCallbackParam;

/// Features supported by the controller.
/// BLUETOOTH SPECIFICATION Version 5.0 | Vol 6, Part B - 4.6
typedef enum {
    LE_ENCRYPTION = 0,
    CONNECTION_PARAMETERS_REQUEST_PROCEDURE,
    EXTENDED_REJECT_INDICATION,
    SLAVE_INITIATED_FEATURES_EXCHANGE,
    LE_PING,
    LE_DATA_PACKET_LENGTH_EXTENSION,
    LL_PRIVACY,
    EXTENDED_SCANNER_FILTER_POLICIES,
    LE_2M_PHY,
    STABLE_MODULATION_INDEX_TRANSMITTER,
    STABLE_MODULATION_INDEX_RECEIVER,
    LE_CODED_PHY,
    LE_EXTENDED_ADVERTISING,
    LE_PERIODIC_ADVERTISING,
    CHANNEL_SELECTION_ALGORITHM_2,
    LE_POWER_CLASS
} FEATURE_SUPPORTED;

/// Ble scan type
typedef enum {
    BLE_SCAN_TYPE_PASSIVE = 0x00,  /// Passive scan
    BLE_SCAN_TYPE_ACTIVE = 0x01,   /// Active scan
} BLE_SCAN_TYPE;

/// Ble scan mode
typedef enum {
    BLE_SCAN_MODE_NON_DISC = 0x00,  /// Non Discoverable
    BLE_SCAN_MODE_GENERAL = 0x01,   /// General
    BLE_SCAN_MODE_LIMITED = 0x02,   /// Limited
    BLE_SCAN_MODE_ALL = 0x03,       /// ALL
} BLE_SCAN_MODE;

/// Ble scan filter type
typedef enum {
    BLE_SCAN_FILTER_ALLOW_ALL = 0x00,  /// Accept all :
    /// 1. advertisement packets except directed advertising packets not addressed to this device (default).
    BLE_SCAN_FILTER_ALLOW_ONLY_WLST = 0x01,  /// Accept only :
    /// 1. advertisement packets from devices where the advertiser’s address is in the WH list.
    /// 2. Directed advertising packets which are not addressed for this device shall be ignored.
    BLE_SCAN_FILTER_ALLOW_UND_RPA_DIR = 0x02,  /// Accept all :
    /// 1. undirected advertisement packets, and
    /// 2. directed advertising packets where the initiator address is a resolvable private address, and
    /// 3. directed advertising packets addressed to this device.
    BLE_SCAN_FILTER_ALLOW_WLIST_PRA_DIR = 0x03,  /// Accept all :
    /// 1. advertisement packets from devices where the advertiser’s address is in the WH list, and
    /// 2. directed advertising packets where the initiator address is a resolvable private address, and
    /// 3. directed advertising packets addressed to this device.
} BLE_SCAN_FILTER;

/// Ble scan duplicate type
typedef enum {
    BLE_SCAN_DUPLICATE_DISABLE =
        0x00,  /// the Link Layer should generate advertising reports to the host for each packet received
    BLE_SCAN_DUPLICATE_ENABLE = 0x01,  /// the Link Layer should filter out duplicate advertising reports to the Host
    BLE_SCAN_DUPLICATE_MAX = 0x02,     /// 0x02 – 0xFF, Reserved for future use
} BLE_SCAN_DUPLICATE;

/// Ble scan parameters
typedef struct {
    BLE_SCAN_TYPE scanType;            /// Scan type
    BLE_ADDR_TYPE ownAddrType;         /// Owner address type
    BLE_SCAN_FILTER scanFilterPolicy;  /// Scan filter policy
    uint16_t scanInterval;             /// Scan interval. This is defined as the time interval from
    /// when the Controller started its last LE scan until it begins the subsequent LE scan.
    /// Range: 0x0004 to 0x4000 Default: 0x0010 (10 ms)
    /// Time = N * 0.625 msec
    /// Time Range: 2.5 msec to 10.24 seconds
    uint16_t scanWindow;  /// Scan window. The duration of the LE scan. LE_Scan_Window
    /// shall be less than or equal to LE_Scan_Interval
    /// Range: 0x0004 to 0x4000 Default: 0x0010 (10 ms)
    /// Time = N * 0.625 msec
    /// Time Range: 2.5 msec to 10240 msec
    BLE_SCAN_DUPLICATE scanDuplicate;  /// The Scan_Duplicates parameter controls whether the Link Layer should filter
    /// out duplicate advertising reports (BLE_SCAN_DUPLICATE_ENABLE) to the Host,
    /// or if the Link Layer should generate advertising reports for each packet received
} BleScanParams;

typedef enum {
    ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY = 0x00,  /// Allow both scan and connection requests from anyone
    ADV_FILTER_ALLOW_SCAN_WLST_CON_ANY,   /// Allow both scan req from WH List devices and connection req fro anyone
    ADV_FILTER_ALLOW_SCAN_ANY_CON_WLST,   /// Allow both scan req from anyone and connection req from WH List devices
    ADV_FILTER_ALLOW_SCAN_WLST_CON_WLST,  /// Allow scan and connection requests from WH List devices
} BLE_ADV_FILTER;

/// Advertising channel mask
typedef enum {
    ADV_CHNL_37 = 0x01,
    ADV_CHNL_38 = 0x02,
    ADV_CHNL_39 = 0x04,
    ADV_CHNL_ALL = 0x07,
} BLE_ADV_CHANNEL;

/// Advertising mode
typedef enum {
    ADV_TYPE_IND = 0x00,
    ADV_TYPE_DIRECT_IND_HIGH = 0x01,
    ADV_TYPE_SCAN_IND = 0x02,
    ADV_TYPE_NONCONN_IND = 0x03,
    ADV_TYPE_DIRECT_IND_LOW = 0x04,
} BLE_ADV_TYPE;

/// Scan Event Type
typedef enum {
    SCAN_ADV_IND = 0x00,
    SCAN_ADV_DIRECT_IND = 0x01,
    SCAN_ADV_SCAN_IND = 0x02,
    SCAN_ADV_NONCONN_IND = 0x03,
    SCAN_SCAN_RSP = 0x04,
} BLE_SCAN_EVENT_TYPE;

/// Advertising parameters
typedef struct {
    uint16_t advMinInterval;  /// Minimum advertising interval for
    /// undirected and low duty cycle directed advertising.
    /// Range: 0x0020 to 0x4000 Default: N = 0x0800 (1.28 second)
    /// ime = N * 0.625 msec Time Range: 20 ms to 10.24 sec
    uint16_t advMaxInterval;  /// Maximum advertising interval for
    /// undirected and low duty cycle directed advertising.
    /// Range: 0x0020 to 0x4000 Default: N = 0x0800 (1.28 second)
    /// Time = N * 0.625 msec Time Range: 20 ms to 10.24 sec Advertising max interval
    BLE_ADV_TYPE advType;            /// Advertising type
    BLE_ADDR_TYPE ownAddrType;       /// Owner bluetooth device address type
    BtAddr peerAddr;                 /// Peer device bluetooth device address
    BLE_ADDR_TYPE peerAddrType;      /// Peer device bluetooth device address type
    BLE_ADV_CHANNEL channelMap;      /// Advertising channel map
    BLE_ADV_FILTER advFilterPolicy;  /// Advertising filter policy
} BleAdvParams;
}  // namespace bluetooth
}  // namespace OHOS
#endif
