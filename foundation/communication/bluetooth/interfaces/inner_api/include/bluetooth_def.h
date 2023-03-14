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

#ifndef BLUETOOTH_DEF_H
#define BLUETOOTH_DEF_H

#include <string>
#include <vector>
#include <stdint.h>

namespace OHOS {
namespace Bluetooth {
// Defined here are various status codes
/*********************************************
 *
 * Interface Return Value Macro Define
 *
 *********************************************/
constexpr int RET_NO_ERROR = 0;
constexpr int RET_NO_SUPPORT = -1;
constexpr int RET_BAD_PARAM = -2;
constexpr int RET_BAD_STATUS = -3;
constexpr int RET_NO_SPACE = -4;

/*********************************************
 *
 * Bluetoot UUID Macro Define
 *
 *********************************************/
const std::string BLUETOOTH_UUID_BASE_UUID = "00000000-0000-1000-8000-00805F9B34FB";
const std::string BLUETOOTH_UUID_A2DP_SINK = "0000110B-0000-1000-8000-00805F9B34FB";
const std::string BLUETOOTH_UUID_A2DP_SRC = "0000110A-0000-1000-8000-00805F9B34FB";
const std::string BLUETOOTH_UUID_GATT = "00001801-0000-1000-8000-00805F9B34FB";
const std::string BLUETOOTH_UUID_AVRCP_CT = "0000110E-0000-1000-8000-00805F9B34FB";
const std::string BLUETOOTH_UUID_AVRCP_TG = "0000110C-0000-1000-8000-00805F9B34FB";
const std::string BLUETOOTH_UUID_HFP_AG = "0000111F-0000-1000-8000-00805F9B34FB";
const std::string BLUETOOTH_UUID_HFP_HF = "0000111E-0000-1000-8000-00805F9B34FB";
const std::string BLUETOOTH_UUID_PBAP_PCE = "0000112E-0000-1000-8000-00805F9B34FB";
const std::string BLUETOOTH_UUID_PBAP_PSE = "0000112F-0000-1000-8000-00805F9B34FB";
const std::string BLUETOOTH_UUID_SPP = "00001101-0000-1000-8000-00805F9B34FB";
const std::string BLUETOOTH_UUID_PNP = "00001200-0000-1000-8000-00805F9B34FB";

/*********************************************
 *
 * Adapter Macro Define
 *
 *********************************************/
/**
 * @brief bt transport define
 * use to GetRemoteDevice(),GetPairedDevices()...
 */
enum BTTransport {
    ADAPTER_BREDR = 0,
    ADAPTER_BLE,
};
/**
 * @brief bt adapter/profile state define
 * use to GetBtConnectionState(),GetBtProfileConnState(),...
 */
enum BTStateID {
    STATE_TURNING_ON,
    STATE_TURN_ON,
    STATE_TURNING_OFF,
    STATE_TURN_OFF,
};
/**
 * @brief connect state define
 * use to ConnectionState()...
 */
enum class BTConnectState : int {
    CONNECTING,
    CONNECTED,
    DISCONNECTING,
    DISCONNECTED,
};
/**
 * @brief access permission define
 * use to
 * Get/SetPhonebookPermission(),Get/SetMessagePermission()
 */
enum class BTPermissionType : int {
    ACCESS_UNKNOWN,
    ACCESS_ALLOWED,
    ACCESS_FORBIDDEN,
};
/**
 * @brief connection strategy define
 * use to
 * SetConnectionStrategy(),GetConnectionStrategy()
 */
enum class BTStrategyType : int {
    CONNECTION_UNKNOWN,
    CONNECTION_ALLOWED,
    CONNECTION_FORBIDDEN,
};
/**
 * @brief profile id define
 * use to
 * GetBtProfileConnState(),GetProfileList()
 */
const uint32_t PROFILE_ID_GATT_CLIENT = 0x00000001;
const uint32_t PROFILE_ID_GATT_SERVER = 0x00000002;
const uint32_t PROFILE_ID_A2DP_SRC = 0x00000004;
const uint32_t PROFILE_ID_A2DP_SINK = 0x00000008;
const uint32_t PROFILE_ID_AVRCP_CT = 0x00000010;
const uint32_t PROFILE_ID_AVRCP_TG = 0x00000020;
const uint32_t PROFILE_ID_HFP_AG = 0x00000040;
const uint32_t PROFILE_ID_HFP_HF = 0x00000080;
const uint32_t PROFILE_ID_MAP_MCE = 0x00000100;
const uint32_t PROFILE_ID_MAP_MSE = 0x00000200;
const uint32_t PROFILE_ID_PBAP_PCE = 0x00000400;
const uint32_t PROFILE_ID_PBAP_PSE = 0x00000800;
const uint32_t PROFILE_ID_SPP = 0x00001000;
const uint32_t PROFILE_ID_DI = 0x00002000;

/**
 * @brief default value
 * use to
 * initialize mac address, name or other values.
 */
const std::string INVALID_MAC_ADDRESS = "00:00:00:00:00:00";
const std::string INVALID_NAME = "";
const int INVALID_VALUE = 0;
const int ADDRESS_LENGTH = 17;
const int ADDRESS_COLON_INDEX = 2;
const int ADDRESS_SEPARATOR_UNIT = 3;

/**
 * @brief discovery state
 * use to
 * GetBtScanMode(), SetBtScanMode()
 */
const int DISCOVERY_STARTED = 0x01;
const int DISCOVERYING = 0x02;
const int DISCOVERY_STOPED = 0x03;

/**
 * @brief pairing result
 * use to
 * OnPairStatusChanged()
 */
const int PAIR_NONE = 0x01;
const int PAIR_PAIRING = 0x02;
const int PAIR_PAIRED = 0x03;

/**
 * @brief user confirm type during paring process.
 * use to
 * OnPairConfirmed()
 */
const int PAIR_CONFIRM_TYPE_PIN_CODE = 0x01;
const int PAIR_CONFIRM_TYPE_PASSKEY_DISPLAY = 0x02;
const int PAIR_CONFIRM_TYPE_PASSKEY_INPUT = 0x03;
const int PAIR_CONFIRM_TYPE_NUMERIC = 0x04;
const int PAIR_CONFIRM_TYPE_CONSENT = 0x05;

/**
 * @brief discoverable mode and connect mode
 * use to
 * GetBtScanMode(), SetBtScanMode()
 */
const int SCAN_MODE_NONE = 0x00;
const int SCAN_MODE_CONNECTABLE = 0x01;
const int SCAN_MODE_GENERAL_DISCOVERABLE = 0x02;
const int SCAN_MODE_LIMITED_DISCOVERABLE = 0x03;
const int SCAN_MODE_CONNECTABLE_GENERAL_DISCOVERABLE = 0x04;
const int SCAN_MODE_CONNECTABLE_LIMITED_DISCOVERABLE = 0x05;

/**
 * @brief bondable mode define
 * use to
 * GetBondableMode(), SetBondableMode()
 */
const int BONDABLE_MODE_OFF = 0x00;
const int BONDABLE_MODE_ON = 0x01;

/**
 * @brief remote device's transport type
 * use to
 * GetTransportType()
 */
const int BT_TRANSPORT_BREDR = 0;
const int BT_TRANSPORT_BLE = 1;

/**
 * @brief remote device type
 * use to
 * GetDeviceType
 */
const int DEVICE_TYPE_UNKNOWN = -1;
const int DEVICE_TYPE_BREDR = 0;
const int DEVICE_TYPE_LE = 1;
const int DEVICE_TYPE_DUAL_MONO = 2;
/*********************************************
 *
 * GATT Macro Define
 *
 *********************************************/

constexpr uint8_t GATT_TRANSPORT_TYPE_AUTO = 0x0;
constexpr uint8_t GATT_TRANSPORT_TYPE_LE = 0x1;
constexpr uint8_t GATT_TRANSPORT_TYPE_CLASSIC = 0x2;

/** A GATT Attribute Permission.
 *  Define GATT Attribute permissions.
 */
enum class GattPermission : uint8_t {
    READABLE = 1 << 0, /**< readable */
    WRITABLE = 1 << 1,
    ENCRYPTION = 1 << 2,
    AUTHENTICATION = 1 << 3,
    AUTHORIZATION = 1 << 4
};

enum class GattConnectionPriority : int {
    BALANCED,
    HIGH,
    LOW_POWER
};

enum GattStatus {
    INVALID_REMOTE_DEVICE = -29,
    INCLUDE_SERVICE_NOT_FOUND,
    REFERENCED_BY_OTHER_SERVICE,
    INVALID_CHARACTERISTIC,
    INVALID_CHARACTERISTIC_DATA,
    INVALID_CHARACTERISTIC_DESCRIPTOR,
    INVALID_CHARACTERISTIC_DESCRIPTOR_DATA,
    NOT_ENOUGH_HANDLES,
    HANDLE_NOT_FOUND,
    INVALID_PARAMETER,
    INTERNAL_ERROR,
    REQUEST_NOT_SUPPORT,
    REMOTE_DEVICE_BUSY,
    MAX_CONNECTIONS,
    MAX_APPLICATIONS,
    DEVICE_ALREADY_BIND,
    INVALID_HANDLE,
    INVALID_OFFSET,
    READ_NOT_PERMITTED,
    WRITE_NOT_PERMITTED,
    INSUFFICIENT_ENCRYPTION,
    INSUFFICIENT_AUTHENTICATION,
    INSUFFICIENT_AUTHORIZATION,
    INSUFFICIENT_ENCRYPTION_KEY_SIZE,
    PREPARE_QUEUE_FULL,
    ATTRIBUTE_NOT_LONG,
    INVALID_ATTRIBUTE_VALUE_LENGTH,
    WRITE_REQUEST_REJECTED,
    GATT_FAILURE,
    GATT_SUCCESS
};

enum BtStatus{
    BT_SUCCESS = 0,
    BT_FAILURE = -1,
};

/*********************************************
 *
 * BLE Macro Define
 *
 *********************************************/
// Minimum value for advertising interval.
const int BLE_ADV_MIN_INTERVAL = 0x0020;
// Maximum value for advertising interval.
const int BLE_ADV_MAX_INTERVAL = 0x4000;
// Default value for advertising interval.
const int BLE_ADV_DEFAULT_INTERVAL = 0x03E8;

// Special advertising set handle used for the legacy advertising set.
const uint8_t BLE_LEGACY_ADVERTISING_HANDLE = 0x00;
// Special advertising set handle used as return or parameter to signify an invalid handle.
const uint8_t BLE_INVALID_ADVERTISING_HANDLE = 0xFF;

// Report delay millis default value
const int BLE_REPORT_DELAY_MILLIS = 5000;

// Definitions for UUID length constants.
const int BLE_UUID_LEN_16 = 2;
const int BLE_UUID_LEN_32 = 4;
const int BLE_UUID_LEN_128 = 16;
const int DEVICE_NAME_MAX_LEN = 26;

// BLE acl connect status
const int BLE_CONNECTION_STATE_DISCONNECTED = 0x00;
const int BLE_CONNECTION_STATE_CONNECTED = 0x01;
const int BLE_CONNECTION_STATE_ENCRYPTED_BREDR = 0x02;
const int BLE_CONNECTION_STATE_ENCRYPTED_LE = 0x04;

const int BLE_PAIR_NONE = 0x01;
const int BLE_PAIR_PAIRING = 0x02;
const int BLE_PAIR_PAIRED = 0x03;
const int BLE_PAIR_CANCELING = 0x04;

const int BLE_ADV_DEFAULT_OP_CODE = 0x00;
const int BLE_ADV_STOP_COMPLETE_OP_CODE = 0x01;
const int BLE_ADV_START_FAILED_OP_CODE = 0x02;

const uint8_t BLE_ADV_DATA_BYTE_FIELD_LEN = 3;
const uint8_t BLE_ADV_DATA_FIELD_TYPE_AND_LEN = 2;
const uint8_t BLE_ONE_BYTE_LEN = 8;

const int8_t BLE_ADV_TX_POWER_ULTRA_LOW_VALUE = -21;
const int8_t BLE_ADV_TX_POWER_LOW_VALUE = -15;
const int8_t BLE_ADV_TX_POWER_MEDIUM_VALUE = -7;
const int8_t BLE_ADV_TX_POWER_HIGH_VALUE = 1;

// Entended Advertising PHY
const uint8_t BLE_ADVERTISEMENT_PHY_NONE = 0x00;
const uint8_t BLE_ADVERTISEMENT_PHY_1M = 0x01;
const uint8_t BLE_ADVERTISEMENT_PHY_2M = 0x02;
const uint8_t BLE_ADVERTISEMENT_PHY_CODED = 0x03;

const uint8_t BLE_LEGACY_ADV_IND_WITH_EX_ADV = 0x13;
const uint8_t BLE_LEGACY_ADV_NONCONN_IND_WITH_EX_ADV = 0x10;

// Incomplete, more data to come
const uint8_t BLE_EX_SCAN_DATE_STATUS_INCOMPLETE_MORE = 0x20;
// Incomplete, data truncated, no more to come
const uint8_t BLE_EX_SCAN_DATE_STATUS_INCOMPLETE_NO_MORE = 0x40;

// Scan mode
typedef enum {
    SCAN_MODE_LOW_POWER = 0x00,
    SCAN_MODE_BALANCED = 0x01,
    SCAN_MODE_LOW_LATENCY = 0x02,
    SCAN_MODE_OP_P2_60_3000 = 0x03,
    SCAN_MODE_OP_P10_60_600 = 0x04,
    SCAN_MODE_OP_P25_60_240 = 0x05,
    SCAN_MODE_OP_P100_1000_1000 = 0x06,
    SCAN_MODE_INVALID
} SCAN_MODE;

// Phy type
typedef enum { PHY_LE_1M = 1, PHY_LE_2M = 2, PHY_LE_CODED = 3, PHY_LE_ALL_SUPPORTED = 255 } PHY_TYPE;

// BLE advertiser TX power level.
typedef enum {
    BLE_ADV_TX_POWER_ULTRA_LOW = 0x00,
    BLE_ADV_TX_POWER_LOW = 0x01,
    BLE_ADV_TX_POWER_MEDIUM = 0x02,
    BLE_ADV_TX_POWER_HIGH = 0x03
} BLE_ADV_TX_POWER_LEVEL;

// BLE advertiser flag
const uint8_t BLE_ADV_FLAG_NON_LIMIT_DISC     = 0x00;
const uint8_t BLE_ADV_FLAG_LIMIT_DISC         = 0x01 << 0;
const uint8_t BLE_ADV_FLAG_GEN_DISC           = 0x01 << 1;
const uint8_t BLE_ADV_FLAG_BREDR_NOT_SPT      = 0x01 << 2;
const uint8_t BLE_ADV_FLAG_DMT_CONTROLLER_SPT = 0x01 << 3;
const uint8_t BLE_ADV_FLAG_DMT_HOST_SPT       = 0x01 << 4;

// Bluetooth device type
typedef enum {
    BLE_BT_DEVICE_TYPE_UNKNOWN = -1,
    BLE_BT_DEVICE_TYPE_BREDR = 0,
    BLE_BT_DEVICE_TYPE_BLE = 1,
    BLE_BT_DEVICE_TYPE_DUMO = 2,
} BLE_BT_DEV_TYPE;

// BLE device address type
typedef enum {
    BLE_ADDR_TYPE_PUBLIC = 0x00,
    BLE_ADDR_TYPE_RANDOM = 0x01,
    BLE_ADDR_TYPE_NON_RPA = 0x02,
    BLE_ADDR_TYPE_RPA = 0x03,
    BLE_ADDR_TYPE_UNKNOWN = 0xFE,
    BLE_ADDR_TYPE_ANONYMOUS = 0xFF,
} BLE_ADDR_TYPE;

typedef enum {
    BLE_IO_CAP_OUT = 0x00,    // DisplayOnly
    BLE_IO_CAP_IO = 0x01,     // DisplayYesNo
    BLE_IO_CAP_IN = 0x02,     // KeyboardOnly
    BLE_IO_CAP_NONE = 0x03,   // NoInputNoOutput
    BLE_IO_CAP_KBDISP = 0x04  // Keyboard display
} BLE_IO_CAP;

// Adv status
typedef enum {
    BLE_ADV_STATE_IDLE,
    BLE_ADV_STATE_ADVERTISING,
} Ble_AdvState;

// The type of advertising data(not adv_type)
typedef enum {
    BLE_AD_TYPE_FLAG = 0x01,
    BLE_AD_TYPE_16SRV_PART = 0x02,
    BLE_AD_TYPE_16SRV_CMPL = 0x03,
    BLE_AD_TYPE_32SRV_PART = 0x04,
    BLE_AD_TYPE_32SRV_CMPL = 0x05,
    BLE_AD_TYPE_128SRV_PART = 0x06,
    BLE_AD_TYPE_128SRV_CMPL = 0x07,
    BLE_AD_TYPE_NAME_SHORT = 0x08,
    BLE_AD_TYPE_NAME_CMPL = 0x09,
    BLE_AD_TYPE_TX_PWR = 0x0A,
    BLE_AD_TYPE_DEV_CLASS = 0x0D,
    BLE_AD_TYPE_SM_TK = 0x10,
    BLE_AD_TYPE_SM_OOB_FLAG = 0x11,
    BLE_AD_TYPE_INT_RANGE = 0x12,
    BLE_AD_TYPE_SOL_SRV_UUID = 0x14,
    BLE_AD_TYPE_128SOL_SRV_UUID = 0x15,
    BLE_AD_TYPE_SERVICE_DATA = 0x16,
    BLE_AD_TYPE_PUBLIC_TARGET = 0x17,
    BLE_AD_TYPE_RANDOM_TARGET = 0x18,
    BLE_AD_TYPE_APPEARANCE = 0x19,
    BLE_AD_TYPE_ADV_INT = 0x1A,
    BLE_AD_TYPE_LE_DEV_ADDR = 0x1b,
    BLE_AD_TYPE_LE_ROLE = 0x1c,
    BLE_AD_TYPE_SPAIR_C256 = 0x1d,
    BLE_AD_TYPE_SPAIR_R256 = 0x1e,
    BLE_AD_TYPE_32SOL_SRV_UUID = 0x1f,
    BLE_AD_TYPE_32SERVICE_DATA = 0x20,
    BLE_AD_TYPE_128SERVICE_DATA = 0x21,
    BLE_AD_TYPE_LE_SECURE_CONFIRM = 0x22,
    BLE_AD_TYPE_LE_SECURE_RANDOM = 0x23,
    BLE_AD_TYPE_URI = 0x24,
    BLE_AD_TYPE_INDOOR_POSITION = 0x25,
    BLE_AD_TYPE_TRANS_DISC_DATA = 0x26,
    BLE_AD_TYPE_LE_SUPPORT_FEATURE = 0x27,
    BLE_AD_TYPE_CHAN_MAP_UPDATE = 0x28,
    BLE_AD_MANUFACTURER_SPECIFIC_TYPE = 0xFF,
} BLE_ADV_DATA_TYPE;

/*********************************************
 *
 * Map Define
 *
 *********************************************/
// Map Message Type
enum class MapMessageType : int {
    EMAIL = 0,
    SMS_GSM = 1,
    SMS_CDMA = 2,
    MMS = 3,
    IM = 4,
    INVALID = 5
};  // IM only support in bmessage ver1.1

// Map Message Status
enum class MapMessageStatus : int { READ, UNREAD, INVALID };  // default UNREAD

// Map BoolType , YES or NO
enum class MapBoolType : int { YES, NO, INVALID };  // default NO

// Map Message ReceptionStatus
enum class MapMsgReceptionStatus : int { COMPLETE, FRACTIONED, NOTIFICATION, INVALID };

// Map MsgDeliveryStatus
enum class MapMsgDeliveryStatus : int { DELIVERED, SENT, UNKNOWN, INVALID };

// Map MsgDirection
enum class MapMsgDirection : int { INCOMING, OUTGOING, OUTGOINGDRAFT, OUTGOINGPENDING, INVALID };

// Map FractionDeliverType
enum class MapFractionDeliverType : int { MORE, LAST, INVALID };

// Map FractionRequest Type
enum class MapFractionRequestType : uint8_t { FIRST = 0x00, NEXT = 0x01, INVALID = 0x02 };

// Map OnOff Type
enum class MapOnOffType : uint8_t { OFF = 0, ON = 1, INVALID = 3 };

// Map Charset Type
enum class MapCharsetType : uint8_t { NATIVE = 0x00, UTF_8 = 0x01, INVALID = 0x02 };

// Attachment Type
enum class MapAttachmentType : uint8_t { NO = 0x00, HAVE = 0x01, INVALID = 0x02 };

// Map StatusIndicator Type
enum class MapStatusIndicatorType : uint8_t {
    READ_STATUS = 0x00,
    DELETED_STATUS = 0x01,
    SET_EXTENDED_DATA = 0x02,
    INVALID = 0x03
};

// Map StatusValue Type
enum class MapStatusValueType : uint8_t { NO = 0x00, YES = 0x01 };

// Map ModifyText Type
enum class MapModifyTextType : uint8_t { REPLACE = 0x00, PREPEND = 0x01, INVALID = 0x02 };

// Map Action Type
enum class MapActionType : int {
    GET_UNREAD_MESSAGES,
    GET_SUPPORTED_FEATURES,
    SEND_MESSAGE,
    SET_NOTIFICATION_FILTER,
    GET_MESSAGES_LISTING,
    GET_MESSAGE,
    UPDATE_INBOX,
    GET_CONVERSATION_LISTING,
    SET_MESSAGE_STATUS,
    SET_OWNER_STATUS,
    GET_OWNER_STATUS,
    GET_MAS_INSTANCE_INFO
};

// Map ExecuteStatus Type
enum class MapExecuteStatus : int {
    SUCCEED = 0,
    CONTINUE = 1,
    BAD_PARAM = (-1),
    BAD_STATUS = (-2),
    NOT_SUPPORT = (-3)
};

// Supported Message Types
#define MAP_MCE_SUPPORTED_MESSAGE_TYPE_EMAIL 0x01
#define MAP_MCE_SUPPORTED_MESSAGE_TYPE_SMS_GSM 0x02
#define MAP_MCE_SUPPORTED_MESSAGE_TYPE_SMS_CDMA 0x04
#define MAP_MCE_SUPPORTED_MESSAGE_TYPE_MMS 0x08
#define MAP_MCE_SUPPORTED_MESSAGE_TYPE_IM 0x10
#define MAP_MCE_SUPPORTED_MESSAGE_TYPE_ALL 0x01F

// Api: MapClient::GetMessagesListing Application Parameters Mask defines
// FilterMessageType Mask, 1 is "filter out this type", 0 is "no filtering, get this type"
#define MAP_FILTER_MESSAGE_MASK_SMS_GSM 0b00000001
#define MAP_FILTER_MESSAGE_MASK_SMS_CDMA 0b00000010
#define MAP_FILTER_MESSAGE_MASK_EMAIL 0b00000100
#define MAP_FILTER_MESSAGE_MASK_MMS 0b00001000
#define MAP_FILTER_MESSAGE_MASK_IM 0b00010000
#define MAP_FILTER_MESSAGE_MASK_NO_FILTERING 0b00000000
// FilterReadStatus,  0b00000001 is get unread messages only,
// 0b00000010 is get read messages only
#define MAP_FILTER_READ_STATUS_MASK_UNREAD 0b00000001
#define MAP_FILTER_READ_STATUS_MASK_READ 0b00000010
#define MAP_FILTER_READ_STATUS_MASK_NO_FILTERING 0b00000000
// FilterPriority, 0b00000001 is get high priority messages only,
// 0b00000010 is get non-high priority messages only
#define MAP_FILTER_PRIORITY_MASK_HIGH 0b00000001
#define MAP_FILTER_PRIORITY_MASK_NO_HIGH 0b00000010
#define MAP_FILTER_PRIORITY_MASK_NO_FILTERING 0b00000000
// ParameterMask , Bits 0-20 has been used , Bits 21–31 Reserved for Future Use
#define MAP_GETMESSAGELIST_PARAMETER_MASK_REQUIRED 0b000000000000000000000000
#define MAP_GETMESSAGELIST_PARAMETER_MASK_PRESENT_ALL 0b000111111111111111111111
#define MAP_GETMESSAGELIST_PARAMETER_MASK_SUBJECT 0b000000000000000000000001
#define MAP_GETMESSAGELIST_PARAMETER_MASK_DATETIME 0b000000000000000000000010
#define MAP_GETMESSAGELIST_PARAMETER_MASK_SENDER_NAME 0b000000000000000000000100
#define MAP_GETMESSAGELIST_PARAMETER_MASK_SENDER_ADDRESSING 0b000000000000000000001000
#define MAP_GETMESSAGELIST_PARAMETER_MASK_RECIPIENT_NAME 0b000000000000000000010000
#define MAP_GETMESSAGELIST_PARAMETER_MASK_RECIPIENT_ADDRESSING 0b000000000000000000100000
#define MAP_GETMESSAGELIST_PARAMETER_MASK_TYPE 0b000000000000000001000000
#define MAP_GETMESSAGELIST_PARAMETER_MASK_SIZE 0b000000000000000010000000
#define MAP_GETMESSAGELIST_PARAMETER_MASK_RECEPTION_STATUS 0b000000000000000100000000
#define MAP_GETMESSAGELIST_PARAMETER_MASK_TEXT 0b000000000000001000000000
#define MAP_GETMESSAGELIST_PARAMETER_MASK_ATTACHMENT_SIZE 0b000000000000010000000000
#define MAP_GETMESSAGELIST_PARAMETER_MASK_PRIORITY 0b000000000000100000000000
#define MAP_GETMESSAGELIST_PARAMETER_MASK_READ 0b000000000001000000000000
#define MAP_GETMESSAGELIST_PARAMETER_MASK_SENT 0b000000000010000000000000
#define MAP_GETMESSAGELIST_PARAMETER_MASK_PROTECTED 0b000000000100000000000000
#define MAP_GETMESSAGELIST_PARAMETER_MASK_REPLYTO_ADDRESSING 0b000000001000000000000000
#define MAP_GETMESSAGELIST_PARAMETER_MASK_DELIVERY_STATUS 0b000000010000000000000000
#define MAP_GETMESSAGELIST_PARAMETER_MASK_CONVERSATION_ID 0b000000100000000000000000
#define MAP_GETMESSAGELIST_PARAMETER_MASK_CONVERSATION_NAME 0b000001000000000000000000
#define MAP_GETMESSAGELIST_PARAMETER_MASK_DIRECTION 0b000010000000000000000000
#define MAP_GETMESSAGELIST_PARAMETER_MASK_ATTACHMENT_MIME 0b000100000000000000000000

// Api: MapClient::GetConversationListing Application Parameters Mask defines
// ConvParameterMask , Bits 0-14 has been used, Bits 15–31 Reserved for Future Use
// Bit i is 1 indicates that the parameter related to Bit i shall be present in the requested Conversation-Listing.
// The reserved bits shall be set to 0 by the MCE and discarded by the MSE.
// If Bit 5 has the value 0, the Conversation-Listing in the response shall not contain any participant element
// and therefore the Bits 6–14 do not have any impact.
// If Bit 5 has the value 1, then at least one of the Bits 6–14 shall also have the value 1.
// If any of the Bits 6–14 has the value 1, Bit 5 shall have the value 1.
#define MAP_GETCONV_PARAMETER_MASK_ALL_ON 0b000000000111111111111111
#define MAP_GETCONV_PARAMETER_MASK_CONVERSATION_NAME 0b000000000000000000000001
#define MAP_GETCONV_PARAMETER_MASK_CONVERSATION_LAST_ACTIVITY 0b000000000000000000000010
#define MAP_GETCONV_PARAMETER_MASK_CONVERSATION_READ_STATUS 0b000000000000000000000100
#define MAP_GETCONV_PARAMETER_MASK_CONVERSATION_VERSION_COUNTER 0b000000000000000000001000
#define MAP_GETCONV_PARAMETER_MASK_CONVERSATION_SUMMARY 0b000000000000000000010000
#define MAP_GETCONV_PARAMETER_MASK_PARTICIPANTS 0b000000000000000000100000
#define MAP_GETCONV_PARAMETER_MASK_PARTICIPANT_UCI 0b000000000000000001000000
#define MAP_GETCONV_PARAMETER_MASK_PARTICIPANT_DISPLAY_NAME 0b000000000000000010000000
#define MAP_GETCONV_PARAMETER_MASK_PARTICIPANT_CHAT_STATE 0b000000000000000100000000
#define MAP_GETCONV_PARAMETER_MASK_PARTICIPANT_LAST_ACTIVITY 0b000000000000001000000000
#define MAP_GETCONV_PARAMETER_MASK_PARTICIPANT_X_BT_UID 0b000000000000010000000000
#define MAP_GETCONV_PARAMETER_MASK_PARTICIPANT_NAME 0b000000000000100000000000
#define MAP_GETCONV_PARAMETER_MASK_PARTICIPANT_PRESENCE_AVAILABILITY 0b000000000001000000000000
#define MAP_GETCONV_PARAMETER_MASK_PARTICIPANT_PRESENCE_TEXT 0b000000000010000000000000
#define MAP_GETCONV_PARAMETER_MASK_PARTICIPANT_PRIORITY 0b000000000100000000000000

// Api: MapClient::SetNotificationFilter input param mask defines
// Bit i is 0 indicates that the MSE shall not send the notification related to bit i for the current MAS.
// NotificationFilterMask ,Bits 0-14 has been used, Bits 15–31 Reserved for Future Use
#define MAP_NOTIFICATION_FILTER_MASK_ALL_OFF 0b000000000000000000000000
#define MAP_NOTIFICATION_FILTER_MASK_ALL_ON 0b000000000111111111111111
#define MAP_NOTIFICATION_FILTER_MASK_NEW_MESSAGE 0b000000000000000000000001
#define MAP_NOTIFICATION_FILTER_MASK_MESSAGE_DELETED 0b000000000000000000000010
#define MAP_NOTIFICATION_FILTER_MASK_MESSAGE_SHIFT 0b000000000000000000000100
#define MAP_NOTIFICATION_FILTER_MASK_SENDING_SUCCESS 0b000000000000000000001000
#define MAP_NOTIFICATION_FILTER_MASK_SENDING_FAILURE 0b000000000000000000010000
#define MAP_NOTIFICATION_FILTER_MASK_DELIVERY_SUCCESS 0b000000000000000000100000
#define MAP_NOTIFICATION_FILTER_MASK_DELIVERY_FAILURE 0b000000000000000001000000
#define MAP_NOTIFICATION_FILTER_MASK_MEMORY_FULL 0b000000000000000010000000
#define MAP_NOTIFICATION_FILTER_MASK_MEMORY_AVAILABLE 0b000000000000000100000000
#define MAP_NOTIFICATION_FILTER_MASK_READ_STATUS_CHANGED 0b000000000000001000000000
#define MAP_NOTIFICATION_FILTER_MASK_CONVERSATION_CHANGED 0b000000000000010000000000
#define MAP_NOTIFICATION_FILTER_MASK_PARTICIPANT_PRESENCE_CHANGED 0b000000000000100000000000
#define MAP_NOTIFICATION_FILTER_MASK_PARTICIPANT_CHAT_STATE_CHANGED 0b000000000001000000000000
#define MAP_NOTIFICATION_FILTER_MASK_MESSAGE_EXTENDED_DATA_CHANGED 0b000000000010000000000000
#define MAP_NOTIFICATION_FILTER_MASK_MESSAGE_REMOVED 0b000000000100000000000000

// Api: MapClient::GetSupportedFeatures action complete param mask defines
// Map Supported features mask
#define MAP_SUPPORTED_FEATURES_NOTIFICATION_REGISTRATION 0x00000001
#define MAP_SUPPORTED_FEATURES_NOTIFICATION_FEATURE 0x00000002
#define MAP_SUPPORTED_FEATURES_BROWSING 0x00000004
#define MAP_SUPPORTED_FEATURES_UPLOADING 0x00000008
#define MAP_SUPPORTED_FEATURES_DELETE_FEATURE 0x00000010
#define MAP_SUPPORTED_FEATURES_INSTANCE_INFO_FEATURE 0x00000020
#define MAP_SUPPORTED_FEATURES_EVENT_REPORT_1_1 0x00000040
#define MAP_SUPPORTED_FEATURES_EVENT_REPORT_1_2 0x00000080
#define MAP_SUPPORTED_FEATURES_MESSAGE_FORMAT_1_1 0x00000100
#define MAP_SUPPORTED_FEATURES_MESSAGELIST_FORMAT_1_1 0x00000200
#define MAP_SUPPORTED_FEATURES_PERSISTENT_MESSAGE_HANDLE 0x00000400
#define MAP_SUPPORTED_FEATURES_DATABASE_IDENTIFIER 0x00000800
#define MAP_SUPPORTED_FEATURES_FOLDOR_VERSION_COUNTER 0x00001000
#define MAP_SUPPORTED_FEATURES_CONVERSATION_VERSION_COUNTER 0x00002000
#define MAP_SUPPORTED_FEATURES_PARTICIPANT_PRESENCE_CHANGE_NOTIFICATION 0x00004000
#define MAP_SUPPORTED_FEATURES_PARTICIPANT_CHAT_STATE_CHANGE_NOTIFICATION 0x00008000
#define MAP_SUPPORTED_FEATURES_PBAP_CONTACT_CROSS_REFERENCE 0x00010000
#define MAP_SUPPORTED_FEATURES_NOTIFICATION_FILTER 0x00020000
#define MAP_SUPPORTED_FEATURES_UTC_OFFSET_TIMESTAMP_FORMAT 0x00040000
#define MAP_SUPPORTED_FEATURES_MAPSUPPORTED_FEATURES 0x00080000
#define MAP_SUPPORTED_FEATURES_CONVERSATION_LISTING 0x00100000
#define MAP_SUPPORTED_FEATURES_OWNER_STATUS 0x00200000
#define MAP_SUPPORTED_FEATURES_MESSAGE_FORWARDING 0x00400000

/*********************************************
 *
 * AVRCP Macro Define
 *
 *********************************************/

// The default label. The valid range is 0x00 ~ 0x0F.
#define AVRC_DEFAULT_LABEL (0x00)
// The invalid label.
#define AVRC_INVALID_LABEL (0x10)

/**
 * @brief This enumeration declares the types of the methods.
 *
 * @since 6
 */
enum AvrcCtActionType : uint8_t {
    AVRC_ACTION_TYPE_INVALID = 0x00,
    AVRC_ACTION_TYPE_PRESS_BUTTON,
    AVRC_ACTION_TYPE_RELEASE_BUTTON,
    AVRC_ACTION_TYPE_SET_ADDRESSED_PLAYER,
    AVRC_ACTION_TYPE_SET_BROWSED_PLAYER,
    AVRC_ACTION_TYPE_GET_CAPABILITIES,
    AVRC_ACTION_TYPE_GET_PLAYER_APP_SETTING_ATTRIBUTES,
    AVRC_ACTION_TYPE_GET_PLAYER_APP_SETTING_VALUES,
    AVRC_ACTION_TYPE_GET_PLAYER_APP_SETTING_CURRENT_VALUE,
    AVRC_ACTION_TYPE_SET_PLAYER_APP_SETTING_CURRENT_VALUE,
    AVRC_ACTION_TYPE_GET_PLAYER_APP_SETTING_ATTRIBUTE_TEXT,
    AVRC_ACTION_TYPE_GET_PLAYER_APP_SETTING_VALUE_TEXT,
    AVRC_ACTION_TYPE_GET_ELEMENT_ATTRIBUTES,
    AVRC_ACTION_TYPE_GET_PLAY_STATUS,
    AVRC_ACTION_TYPE_PLAY_ITEM,
    AVRC_ACTION_TYPE_ADD_TO_NOW_PLAYING,
    AVRC_ACTION_TYPE_CHANGE_PATH,
    AVRC_ACTION_TYPE_GET_FOLDER_ITEMS,
    AVRC_ACTION_TYPE_GET_ITEM_ATTRIBUTES,
    AVRC_ACTION_TYPE_GET_TOTAL_NUMBER_OF_ITEMS,
    AVRC_ACTION_TYPE_SET_ABSOLUTE_VOLUME,
    AVRC_ACTION_TYPE_NOTIFY_PLAYBACK_STATUS_CHANGED,
    AVRC_ACTION_TYPE_NOTIFY_TRACK_CHANGED,
    AVRC_ACTION_TYPE_NOTIFY_TRACK_REACHED_END,
    AVRC_ACTION_TYPE_NOTIFY_TRACK_REACHED_START,
    AVRC_ACTION_TYPE_NOTIFY_PLAYBACK_POS_CHANGED,
    AVRC_ACTION_TYPE_NOTIFY_PLAYER_APPLICATION_SETTING_CHANGED,
    AVRC_ACTION_TYPE_NOTIFY_NOW_PLAYING_CONTENT_CHANGED,
    AVRC_ACTION_TYPE_NOTIFY_AVAILABLE_PLAYERS_CHANGED,
    AVRC_ACTION_TYPE_NOTIFY_ADDRESSED_PLAYER_CHANGED,
    AVRC_ACTION_TYPE_NOTIFY_UIDS_CHANGED,
    AVRC_ACTION_TYPE_NOTIFY_VOLUME_CHANGED,
    AVRC_ACTION_TYPE_GET_MEDIA_PLAYER_LIST,
};

/**
 * @brief This enumeration declares the values of the supported button.
 * @see AV/C Panel Subunit Specification 1.23 Section 9.4 PASS THROUGH control command Table 9.21 - Operation id List
 *
 * @since 6
 */
enum AvrcKeyOperation : uint8_t {
    AVRC_KEY_OPERATION_VOLUME_UP = 0x41,     // Volume up.
    AVRC_KEY_OPERATION_VOLUME_DOWN = 0x42,   // Volume down.
    AVRC_KEY_OPERATION_MUTE = 0x43,          // Mute.
    AVRC_KEY_OPERATION_PLAY = 0x44,          // Play.
    AVRC_KEY_OPERATION_STOP = 0x45,          // Stop.
    AVRC_KEY_OPERATION_PAUSE = 0x46,         // Pause.
    AVRC_KEY_OPERATION_REWIND = 0x48,        // Rewind.
    AVRC_KEY_OPERATION_FAST_FORWARD = 0x49,  // Fast forward.
    AVRC_KEY_OPERATION_FORWARD = 0x4B,       // Forward.
    AVRC_KEY_OPERATION_BACKWARD = 0x4C,      // Backward.
    AVRC_KEY_OPERATION_INVALID = 0x7F,
};

/**
 * @brief This enumeration declares the values of the key state.
 *
 * @since 6
 */
enum AvrcTgKeyState : uint8_t {
    AVRC_KEY_STATE_PRESS = 0x00,    // The key is pushed.
    AVRC_KEY_STATE_RELEASE = 0x01,  // The key is released.
    AVRC_KEY_STATE_INVALID,         // Invalid key state.
};

/**
 * @brief This enumeration declares the values of the player application setting attributes.
 * @see Audio/Video Remote Control 1.6.2 Section 27 Appendix F: list of defined player application settings and values.
 *
 * @since 6
 */
enum AvrcPlayerAttribute : uint8_t {
    AVRC_PLAYER_ATTRIBUTE_ILLEGAL = 0x00,       // Illegal , Should not be used.
    AVRC_PLAYER_ATTRIBUTE_EQUALIZER = 0x01,     // Equalizer ON / OFF status.
    AVRC_PLAYER_ATTRIBUTE_REPEAT = 0x02,        // Repeat Mode status.
    AVRC_PLAYER_ATTRIBUTE_SHUFFLE = 0x03,       // Shuffle ON/OFF status.
    AVRC_PLAYER_ATTRIBUTE_SCAN = 0x04,          // Scan ON/OFF status.
    AVRC_PLAYER_ATTRIBUTE_RESERVED_MIN = 0x05,  // Reserved for future use. 0x05 - 0x7F
    AVRC_PLAYER_ATTRIBUTE_RESERVED_MAX = 0x7F,  // Reserved for future use. 0x05 - 0x7F
    AVRC_PLAYER_ATTRIBUTE_EXTENSION = 0x80,     // Provided for TG driven static media player menu extension.
                                                // Range: 0x80 - 0xFF
};

/**
 * @brief This enumeration declares the values of the Equalizer ON/OFF status.
 * @see Audio/Video Remote Control 1.6.2 Section 27 Appendix F: list of defined player application settings and values.
 *
 * @since 6
 */
enum AvrcEqualizer : uint8_t {
    AVRC_EQUALIZER_OFF = 0x01,
    AVRC_EQUALIZER_ON,
    AVRC_EQUALIZER_INVALID,
};

/**
 * @brief This enumeration declares the values of the Repeat Mode status.
 * @see Audio/Video Remote Control 1.6.2 Section 27 Appendix F: list of defined player application settings and values.
 *
 * @since 6
 */
enum AvrcRepeat : uint8_t {
    AVRC_REPEAT_OFF = 0x01,
    AVRC_REPEAT_SINGLE_TRACK,
    AVRC_REPEAT_ALL_TRACK,
    AVRC_REPEAT_GROUP,
    AVRC_REPEAT_INVALID,
};

/**
 * @brief This enumeration declares the values of the Shuffle ON/OFF status.
 * @see Audio/Video Remote Control 1.6.2 Section 27 Appendix F: list of defined player application settings and values.
 *
 * @since 6
 */
enum AvrcShuffle : uint8_t {
    AVRC_SHUFFLE_OFF = 0x01,
    AVRC_SHUFFLE_ALL_TRACKS,
    AVRC_SHUFFLE_GROUP,
    AVRC_SHUFFLE_INVALID,
};

/**
 * @brief This enumeration declares the values of the Scan ON/OFF status.
 * @see Audio/Video Remote Control 1.6.2 Section 27 Appendix F: list of defined player application settings and values.
 *
 * @since 6
 */
enum AvrcScan : uint8_t {
    AVRC_SCAN_OFF = 0x01,
    AVRC_SCAN_ALL_TRACKS,
    AVRC_SCAN_GROUP,
    AVRC_SCAN_INVALID,
};

/**
 * @brief This enumeration declares the values of the play status.
 *
 * @since 6
 */
enum AvrcPlayStatus : uint32_t {
    AVRC_PLAY_STATUS_STOPPED = 0x00,                      // Stopped.
    AVRC_PLAY_STATUS_PLAYING,                             // Playing.
    AVRC_PLAY_STATUS_PAUSED,                              // Paused.
    AVRC_PLAY_STATUS_FWD_SEEK,                            // Fwd seek.
    AVRC_PLAY_STATUS_REV_SEEK,                            // Rev seek.
    AVRC_PLAY_STATUS_RESERVED_MIN = 0x05,                 // Reserved for future use.
    AVRC_PLAY_STATUS_RESERVED_MAX = 0xFE,                 // Reserved for future use.
    AVRC_PLAY_STATUS_ERROR = 0xFF,                        // Error.
    AVRC_PLAY_STATUS_INVALID_SONG_LENGTH = 0xFFFFFFFF,    // If TG does not support, then TG shall return 0xFFFFFFFF.
    AVRC_PLAY_STATUS_INVALID_SONG_POSITION = 0xFFFFFFFF,  // If TG does not support, then TG shall return 0xFFFFFFFF.
};

/**
 * @brief This enumeration declares the values of folder direction.
 *
 * @since 6
 */
enum AvrcFolderDirection : uint8_t {
    AVRC_FOLDER_DIRECTION_UP = 0x00,       // Folder up.
    AVRC_FOLDER_DIRECTION_DOWN = 0x01,     // Folder down.
    AVRC_FOLDER_DIRECTION_INVALID = 0x02,  // Reserved.
};

/**
 * @brief This enumeration declares the values of the scope in which the UID of the media element item or folder item.
 * @see Audio/Video Remote Control 1.6.2 Section 6.10.1 Scope.
 *
 * @since 6
 */
enum AvrcMediaScope : uint8_t {
    AVRC_MEDIA_SCOPE_PLAYER_LIST = 0x00,   // Contains all available media players.
    AVRC_MEDIA_SCOPE_VIRTUAL_FILE_SYSTEM,  // Contains the media content of the browsed player.
    AVRC_MEDIA_SCOPE_SEARCH,               // The results of a search operation on the browsed player.
    AVRC_MEDIA_SCOPE_NOW_PLAYING,          // The Now Playing list (or queue) of the addressed player.
    AVRC_MEDIA_SCOPE_INVALID,
};

/**
 * @brief This enumeration declares the values of the browseable items.
 * @see Audio/Video Remote Control 1.6.2 Section 6.10.2 Browseable items.
 *
 * @since 6
 */
enum AvrcMediaType : uint8_t {
    AVRC_MEDIA_TYPE_MEDIA_PLAYER_ITEM = 0x01,
    AVRC_MEDIA_TYPE_FOLDER_ITEM,
    AVRC_MEDIA_TYPE_MEDIA_ELEMENT_ITEM,
    AVRC_MEDIA_TYPE_INVALID,
};

/**
 * @brief This enumeration declares the values of the attribute count.
 * @see Audio/Video Remote Control 1.6.2 Section 6.10.4.2 GetFolderItems - Attribute Count.
 *
 * @since 6
 */
enum AvrcAttributeCount {
    AVRC_ATTRIBUTE_COUNT_ALL = 0x00,
    AVRC_ATTRIBUTE_COUNT_NO = 0xFF,
};

/**
 * @brief This enumeration declares the values of the media attribute. These values are used to uniquely identify media
 * information.
 * @see Audio/Video Remote Control 1.6.2 Section 26 Appendix E: list of media attributes.
 *
 * @since 6
 */
enum AvrcMediaAttribute : uint8_t {
    // Not used.
    AVRC_MEDIA_ATTRIBUTE_NOT_USED = 0x00,
    // Text field representing the title, song name or content description coded per specified character set.
    AVRC_MEDIA_ATTRIBUTE_TITLE,
    // Text field representing artist(s), performer(s) or group coded per specified character set.
    AVRC_MEDIA_ATTRIBUTE_ARTIST_NAME,
    // Text field representing the title of the recording(source) from which the audio in the file is taken.
    AVRC_MEDIA_ATTRIBUTE_ALBUM_NAME,
    // Numeric ASCII string containing the order number of the audio - file on its original recording.O 0x5 Total
    // Number of Tracks.
    AVRC_MEDIA_ATTRIBUTE_TRACK_NUMBER,
    // Numeric ASCII string containing the total number of tracks or elements on the original recording.
    AVRC_MEDIA_ATTRIBUTE_TOTAL_NUMBER_OF_TRACKS,
    // Text field representing the category of the composition characterized by a particular style.
    AVRC_MEDIA_ATTRIBUTE_GENRE,
    // Numeric ASCII string containing the length of the audio file in milliseconds.(E.g .02 : 30 = 150000)
    AVRC_MEDIA_ATTRIBUTE_PLAYING_TIME,
    // BIP Image Handle.
    AVRC_MEDIA_ATTRIBUTE_DEFAULT_COVER_ART,
    // Reserved for future use.
    AVRC_MEDIA_ATTRIBUTE_RESERVED,
};

/**
 * @brief This enumeration declares the values of the major player type.
 * @see Audio/Video Remote Control 1.6.2 Section 6.10.2.1 Media player item - Major Player Type
 *
 * @since 6
 */
enum AvrcMediaMajorPlayerType : uint8_t {
    AVRC_MEDIA_MAJOR_PLAYER_TYPE_AUDIO = 0x01,               // Audio.
    AVRC_MEDIA_MAJOR_PLAYER_TYPE_VIDEO = 0x02,               // Video.
    AVRC_MEDIA_MAJOR_PLAYER_TYPE_BROADCASTING_AUDIO = 0x04,  // Broadcasting Audio.
    AVRC_MEDIA_MAJOR_PLAYER_TYPE_BROADCASTING_VIDEO = 0x08,  // Broadcasting Video.
    AVRC_MEDIA_MAJOR_PLAYER_TYPE_RESERVED = 0x10             // Reserved for future use.
};

/**
 * @brief This enumeration declares the values of the major player type.
 * @see Audio/Video Remote Control 1.6.2 Section 6.10.2.1 Media player item - Player Sub Type
 *
 * @since 6
 */
enum AvrcMediaPlayerSubType : uint32_t {
    AVRC_MEDIA_PLAYER_SUB_TYPE_AUDIO_BOOK = 0x00000001,  // Audio Book.
    AVRC_MEDIA_PLAYER_SUB_TYPE_PODCAST = 0x00000002,     // Pod cast.
    AVRC_MEDIA_PLAYER_SUB_TYPE_RESERVED = 0x00000004     // Reserved for future use.
};

/**
 * @brief This enumeration declares the values of character set.
 *
 * @since 6
 */
enum AvrcMediaCharacterSet : uint16_t {
    AVRC_MEDIA_CHARACTER_SET_UTF8 = 0x006A,  // The value of UTF-8 as defined in IANA character set document.
};

/**
 * @brief This enumeration declares the values of the folder type.
 * @see Audio/Video Remote Control 1.6.2 Section 6.10.2.2 Folder item - Folder Type
 *
 * @since 6
 */
enum AvrcMediaFolderType : uint8_t {
    AVRC_MEDIA_FOLDER_TYPE_MIXED = 0x00,  // Mixed.
    AVRC_MEDIA_FOLDER_TYPE_TITLES,        // Titles.
    AVRC_MEDIA_FOLDER_TYPE_ALBUMS,        // Albums.
    AVRC_MEDIA_FOLDER_TYPE_ARTISTS,       // Artists.
    AVRC_MEDIA_FOLDER_TYPE_GENRES,        // Genres.
    AVRC_MEDIA_FOLDER_TYPE_PLAY_LISTS,    // Play lists.
    AVRC_MEDIA_FOLDER_TYPE_YEARS,         // Years.
    AVRC_MEDIA_FOLDER_TYPE_RESERVED,      // Reserved for future use.
};

/**
 * @brief This enumeration declares the values of the is playable.
 * @see Audio/Video Remote Control 1.6.2 Section 6.10.2.2 Folder item - Is Playable
 *
 * @since 6
 */
enum AvrcMediaFolderPlayable : uint8_t {
    // The folder cannot be played. This means that the folder UID shall not be passed to either the PlayItem or
    // AddToNowPlaying commands.
    AVRC_MEDIA_FOLDER_PLAYABLE_NO = 0x00,
    // The folder can be played. The folder UID may be passed to the PlayItem and AddToNowPlaying(if supported)
    // commands. The media player behavior on playing a folder should be same as on the local user interface.
    AVRC_MEDIA_FOLDER_PLAYABLE_YES,
    AVRC_MEDIA_FOLDER_PLAYABLE_RESERVED,  // Reserved for future use.
};

/**
 * @brief This enumeration declares the values of the media type.
 * @see Audio/Video Remote Control 1.6.2 Section 6.10.2.3 Media element item - Media Type
 *
 * @since 6
 */
enum AvrcMediaElementType : uint8_t {
    AVRC_MEDIA_ELEMENT_TYPE_AUDIO = 0x00,  // Audio.
    AVRC_MEDIA_ELEMENT_TYPE_VIDEO,         // Video.
    AVRC_MEDIA_ELEMENT_TYPE_RESERVED,      // Reserved for future use.
};

/**
 * @brief This enumeration declares the values of the media type.
 *
 * @details An Absolute Volume is represented in one octet. The top bit (bit 7) is reserved for future addition (RFA).
 * The volume is specified as a percentage of the maximum. The value 0x0 corresponds to 0%. The value 0x7F corresponds
 * to 100%. Scaling should be applied to achieve values between these two. The existence of this scale does not impose
 * any restriction on the granularity of the volume control scale on the TG.
 * @see Audio/Video Remote Control 1.6.2 Section 6.13.1 Absolute Volume
 *
 * @since 6
 */
enum AvrcAbsoluteVolume : uint8_t {
    AVRC_ABSOLUTE_VOLUME_PERCENTAGE_0 = 0x00,    // 0%
    AVRC_ABSOLUTE_VOLUME_PERCENTAGE_100 = 0x7F,  // 100%
    AVRC_ABSOLUTE_VOLUME_INVALID = 0x80,
};

/**
 * @brief This enumeration declares the values of the "EVENT ID".
 * @see Audio/Video Remote Control 1.6.2 Section 28 Appendix H: list of defined notification events.
 *
 * @since 6
 */
enum AvrcEventId : uint8_t {
    AVRC_EVENT_ID_PLAYBACK_STATUS_CHANGED = 0x01,  // Change in playback status of the current track.
    AVRC_EVENT_ID_TRACK_CHANGED,                   // Change of current track.
    AVRC_EVENT_ID_TRACK_REACHED_END,               // Reached end of a track.
    AVRC_EVENT_ID_TRACK_REACHED_START,             // Reached start of a track.
    // Change in playback position. Returned after the specified playback notification change notification interval.
    AVRC_EVENT_ID_PLAYBACK_POS_CHANGED,
    AVRC_EVENT_ID_BATT_STATUS_CHANGED,                 // Change in battery status.
    AVRC_EVENT_ID_SYSTEM_STATUS_CHANGED,               // Change in system status.
    AVRC_EVENT_ID_PLAYER_APPLICATION_SETTING_CHANGED,  // Change in player application setting.
    AVRC_EVENT_ID_NOW_PLAYING_CONTENT_CHANGED,         // The content of the Now Playing list has changed.
    AVRC_EVENT_ID_AVAILABLE_PLAYERS_CHANGED,           // The available players have changed
    AVRC_EVENT_ID_ADDRESSED_PLAYER_CHANGED,            // The Addressed Player has been changed.
    AVRC_EVENT_ID_UIDS_CHANGED,                        // The UIDs have changed.
    AVRC_EVENT_ID_VOLUME_CHANGED,                      // The volume has been changed locally on the TG.
    AVRC_EVENT_ID_RESERVED = 0x0E,
};

/**
 * @bUnique identifier to identify an element on TG
 *
 * @since 6
 */
enum AvrcElementIdentifier : uint64_t {
    AVRC_ELEMENT_ATTRIBUTES_IDENTIFIER_PLAYING =
        0x0000000000000000,  // attribute information for the element which is current track in the TG device.
    AVRC_ELEMENT_ATTRIBUTES_IDENTIFIER_RESERVED =
        0x0000000000000001,  // All other values other than 0x0 are currently reserved.
};

/**
 * @brief This enumeration declares the allowed values for GetCapabilities Command.
 *
 * @since 6
 */
enum AvrcCapabilityId : uint8_t {
    AVRC_CAPABILITY_COMPANYID = 0x02,  // The list of CompanyID supported by TG.
    AVRC_CAPABILITY_EVENTID = 0x03,    // EventIDs defined in this specification to be supported by TG.
};

/**
 * @brief This enumeration declares the values of playback interval in seconds.
 *
 * @since 6
 */
enum AvrcPlaybackInterval : uint8_t {
    AVRC_PLAYBACK_INTERVAL_1_SEC = 0x01,
    AVRC_PLAYBACK_INTERVAL_5_SEC = 0x05,
    AVRC_PLAYBACK_INTERVAL_10_SEC = 0x0A,
    AVRC_PLAYBACK_INTERVAL_15_SEC = 0x0F,
    AVRC_PLAYBACK_INTERVAL_20_SEC = 0x14,
    AVRC_PLAYBACK_INTERVAL_25_SEC = 0x19,
    AVRC_PLAYBACK_INTERVAL_30_SEC = 0x1E,
};

/**
 * @brief This enumeration declares the error / status code of the vendor dependent command frame and the browsing
 * command frame.
 *
 * @since 6
 */
enum AvrcEsCode : uint8_t {
    AVRC_ES_CODE_INVALID = 0xFF,  // Reserved.
    // Invalid command, sent if TG received a PDU that it did not understand.
    AVRC_ES_CODE_INVALID_COMMAND = 0x00,
    // Invalid parameter, sent if the TG received a PDU with a parameter ID that it did not understand. This error code
    // applies to the following identifiers :
    // PDU ID.
    // Capability ID.
    // Event ID.
    // Player Application Setting Attribute ID.
    // Player Application Setting Value ID.
    // Element Attribute ID.
    AVRC_ES_CODE_INVALID_PARAMETER = 0x01,
    // Parameter content error.Sent if the parameter ID is understood, but content is wrong or corrupted.
    AVRC_ES_CODE_PARAMETER_CONTENT_ERROR = 0x02,
    // Internal Error - sent if there are error conditions not covered by a more specific error code.
    AVRC_ES_CODE_INTERNAL_ERROR = 0x03,
    // Operation completed without error.This is the status that should be returned if the operation was successful.
    AVRC_ES_CODE_NO_ERROR = 0x04,
    // UID Changed - The UIDs on the device have changed.
    AVRC_ES_CODE_UID_CHANGED = 0x05,
    // Reserved.
    AVRC_ES_CODE_RESERVED = 0x06,
    // Invalid Direction - The Direction parameter is invalid.
    AVRC_ES_CODE_INVALID_DIRECTION = 0x07,
    // Not a Directory - The UID provided does not refer to a folder item.
    AVRC_ES_CODE_NOT_A_DIRECTORY = 0x08,
    // Does Not Exist - The UID provided does not refer to any currently valid item.
    AVRC_ES_CODE_DOES_NOT_EXIST = 0x09,
    // Invalid Scope - The scope parameter is invalid.
    AVRC_ES_CODE_INVALID_SCOPE = 0x0A,
    // Range Out of Bounds - The start of range provided is not valid.
    AVRC_ES_CODE_RANGE_OUT_OF_BOUNDS = 0x0B,
    // Folder Item is not playable - The UID provided refers to a folder item which cannot be handled by this media
    // player.
    AVRC_ES_CODE_FOLDER_ITEM_IS_NOT_PLAYABLE = 0x0C,
    // Media in Use - The media is not able to be used for this operation at this time.
    AVRC_ES_CODE_MEDIA_IN_USE = 0x0D,
    // Now Playing List Full - No more items can be added to the Now Playing List.
    AVRC_ES_CODE_NOW_PLAYING_LIST_FULL = 0x0E,
    // Search Not Supported - The Browsed Media Player does not support search.
    AVRC_ES_CODE_SEARCH_NOT_SUPPORTED = 0x0F,
    // Search in Progress - A search operation is already in progress.
    AVRC_ES_CODE_SEARCH_IN_PROGRESS = 0x10,
    // Invalid Player Id - The specified Player Id does not refer to a valid player.
    AVRC_ES_CODE_INVALID_PLAYER_ID = 0x11,
    // Player Not Browsable - The Player Id supplied refers to a Media Player which does not support browsing.
    AVRC_ES_CODE_PLAYER_NOT_BROWSABLE = 0x12,
    // Player Not Addressed.The Player Id supplied refers to a player which is not currently addressed, and the command
    // is not able to be performed if the player is not set as addressed.
    AVRC_ES_CODE_PLAYER_NOT_ADDRESSED = 0x13,
    // No valid Search Results - The Search result list does not contain valid entries, e.g.after being invalidated due
    // to change of browsed player.
    AVRC_ES_CODE_NO_VALID_SEARCH_RESULTS = 0x14,
    // No available players.
    AVRC_ES_CODE_NO_AVAILABLE_PLAYERS = 0x15,
    // Addressed Player Changed.
    AVRC_ES_CODE_ADDRESSED_PLAYER_CHANGED = 0x16,

    // User custom error code.
    AVRC_ES_CODE_NOTIFICATION_CHANGED = 0xFF,
};

/*********************************************
 *
 * HFP Definitions
 *
 *********************************************/
/**
 * @brief sco connect state define
 * use to notify sco connection observers.
 */
enum class HfpScoConnectState : int { SCO_DISCONNECTED = 3, SCO_CONNECTING, SCO_DISCONNECTING, SCO_CONNECTED };

/*********************************************
 *
 * A2DP Definitions
 *
 *********************************************/

/**
 * @brief a2dp optional codec support state define
 *
 * @since 6
 */
enum A2DP_OPTIONAL_SUPPORT_STATE {
    A2DP_OPTIONAL_NOT_SUPPORT,
    A2DP_OPTIONAL_SUPPORT,
    A2DP_OPTIONAL_SUPPORT_UNKNOWN,
};

/**
 * @brief a2dp optional codec enable state define
 *
 * @since 6
 */
enum A2DP_OPTIONAL_ENABLE_STATE { A2DP_OPTIONAL_DISABLE, A2DP_OPTIONAL_ENABLE, A2DP_OPTIONAL_ENABLE_UNKNOWN };

/**
 * @brief a2dp playing state define
 *
 * @since 6
 */
enum A2DP_PLAYING_STATE { A2DP_NOT_PLAYING, A2DP_IS_PLAYING };

/**
 * @brief A2dp codec type enum.
 *
 * @since 6

 */
enum A2dpUserCodecType : uint8_t {
    A2DP_CODEC_TYPE_SBC_USER = 0,
    A2DP_CODEC_TYPE_MPEG1_USER = 0x01,
    A2DP_CODEC_TYPE_AAC_USER = 0x01 << 1,
    A2DP_CODEC_TYPE_ATRAC_USER = 0x01 << 2,
    A2DP_CODEC_TYPE_NONA2DP_USER = 0xFF
};

/**
 * @brief A2dp codec priority enum.
 *
 * @since 6
 */
enum A2dpUserCodecPriority : uint32_t {
    A2DP_CODEC_PRIORITY_DISABLED_USER = 0,
    A2DP_CODEC_PRIORITY_DEFAULT_USER,
    A2DP_CODEC_PRIORITY_HIGHEST_USER = 1000 * 1000,
};

/**
 * @brief A2dp codec sample rate enum.
 *
 * @since 6
 */
enum A2dpUserCodecSampleRate : uint32_t {
    A2DP_SAMPLE_RATE_NONE_USER = 0x0,
    A2DP_SBC_SAMPLE_RATE_48000_USER = 0x1 << 4, /* octet0 b4 */
    A2DP_SBC_SAMPLE_RATE_44100_USER = 0x1 << 5, /* octet0 b5 */
    A2DP_SBC_SAMPLE_RATE_32000_USER = 0x1 << 6, /* octet0 b6 */
    A2DP_SBC_SAMPLE_RATE_16000_USER = 0x1 << 7, /* octet0 b7 */
    A2DP_SBC_SAMPLE_RATE_MSK_USER = 0xF0,
    A2DP_AAC_SAMPLE_RATE_OCTET1_44100_USER = 0x01,
    A2DP_AAC_SAMPLE_RATE_OCTET1_32000_USER = 0x01 << 1,
    A2DP_AAC_SAMPLE_RATE_OCTET1_24000_USER = 0x01 << 2,
    A2DP_AAC_SAMPLE_RATE_OCTET1_22050_USER = 0x01 << 3,
    A2DP_AAC_SAMPLE_RATE_OCTET1_16000_USER = 0x01 << 4,
    A2DP_AAC_SAMPLE_RATE_OCTET1_12000_USER = 0x01 << 5,
    A2DP_AAC_SAMPLE_RATE_OCTET1_11025_USER = 0x01 << 6,
    A2DP_AAC_SAMPLE_RATE_OCTET1_8000_USER = 0x01 << 7,
    A2DP_AAC_SAMPLE_RATE_OCTET1_MSK_USER = 0xFF,
    A2DP_AAC_SAMPLE_RATE_OCTET2_96000_USER = 0x01 << 12,
    A2DP_AAC_SAMPLE_RATE_OCTET2_88200_USER = 0x01 << 13,
    A2DP_AAC_SAMPLE_RATE_OCTET2_64000_USER = 0x01 << 14,
    A2DP_AAC_SAMPLE_RATE_OCTET2_48000_USER = 0x01 << 15,
    A2DP_AAC_SAMPLE_RATE_OCTET2_MSK_USER = 0xF0,
};

/**
 * @brief A2dp codec channel mode enum.
 *
 * @since 6
 */
enum A2dpUserCodecChannelMode : uint8_t {
    A2DP_CHANNEL_MODE_NONE_USER = 0x0,
    A2DP_SBC_CHANNEL_MODE_JOINT_STEREO_USER = 0x1, /* octet0 b0 */
    A2DP_SBC_CHANNEL_MODE_STEREO_USER = 0x1 << 1,  /* octet0 b1 */
    A2DP_SBC_CHANNEL_MODE_DUAL_USER = 0x1 << 2,    /* octet0 b2 */
    A2DP_SBC_CHANNEL_MODE_MONO_USER = 0x1 << 3,    /* octet0 b3 */
    A2DP_SBC_CHANNEL_MODE_MSK_USER = 0x0F,
    A2DP_AAC_CHANNEL_MODE_OCTET2_DOUBLE_USER = 0x01 << 2, /* octet2 b2 */
    A2DP_AAC_CHANNEL_MODE_OCTET2_SINGLE_USER = 0x01 << 3, /* octet2 b3 */
    A2DP_AAC_CHANNEL_MODE_OCTET2_MSK_USER = 0x0C,
};

/**
 * @brief A2dp codec bits per sample enum.
 *
 * @since 6
 */
enum A2dpUserCodecBitsPerSample : uint8_t {
    A2DP_SAMPLE_BITS_NONE_USER = 0x0,
    A2DP_SAMPLE_BITS_16_USER = 0x1 << 0,
    A2DP_SAMPLE_BITS_24_USER = 0x1 << 1,
    A2DP_SAMPLE_BITS_32_USER = 0x1 << 2,
    A2DP_SAMPLE_BITS_MSK_USER = 0x06,
    A2DP_AAC_SAMPLE_BITS_OCTET3_BIT0_USER = 0x01,      /* octet3 b0 */
    A2DP_AAC_SAMPLE_BITS_OCTET3_BIT1_USER = 0x01 << 1, /* octet3 b1 */
    A2DP_AAC_SAMPLE_BITS_OCTET3_BIT2_USER = 0x01 << 2, /* octet3 b2 */
    A2DP_AAC_SAMPLE_BITS_OCTET3_BIT3_USER = 0x01 << 3, /* octet3 b3 */
    A2DP_AAC_SAMPLE_BITS_OCTET3_BIT4_USER = 0x01 << 4, /* octet3 b4 */
    A2DP_AAC_SAMPLE_BITS_OCTET3_BIT5_USER = 0x01 << 5, /* octet3 b5 */
    A2DP_AAC_SAMPLE_BITS_OCTET3_BIT6_USER = 0x01 << 6, /* octet3 b6 */
    A2DP_AAC_SAMPLE_BITS_OCTET4_BIT0_USER = 0x01,      /* octet4 b0 */
    A2DP_AAC_SAMPLE_BITS_OCTET4_BIT1_USER = 0x01 << 1, /* octet4 b1 */
    A2DP_AAC_SAMPLE_BITS_OCTET4_BIT2_USER = 0x01 << 2, /* octet4 b2 */
    A2DP_AAC_SAMPLE_BITS_OCTET4_BIT3_USER = 0x01 << 3, /* octet4 b3 */
    A2DP_AAC_SAMPLE_BITS_OCTET4_BIT4_USER = 0x01 << 4, /* octet4 b4 */
    A2DP_AAC_SAMPLE_BITS_OCTET4_BIT5_USER = 0x01 << 5, /* octet4 b5 */
    A2DP_AAC_SAMPLE_BITS_OCTET4_BIT6_USER = 0x01 << 6, /* octet4 b6 */
    A2DP_AAC_SAMPLE_BITS_OCTET4_BIT7_USER = 0x01 << 7, /* octet4 b6 */
    A2DP_AAC_SAMPLE_BITS_OCTET5_BIT0_USER = 0x01,      /* octet5 b0 */
    A2DP_AAC_SAMPLE_BITS_OCTET5_BIT1_USER = 0x01 << 1, /* octet5 b1 */
    A2DP_AAC_SAMPLE_BITS_OCTET5_BIT2_USER = 0x01 << 2, /* octet5 b2 */
    A2DP_AAC_SAMPLE_BITS_OCTET5_BIT3_USER = 0x01 << 3, /* octet5 b3 */
    A2DP_AAC_SAMPLE_BITS_OCTET5_BIT4_USER = 0x01 << 4, /* octet5 b4 */
    A2DP_AAC_SAMPLE_BITS_OCTET5_BIT5_USER = 0x01 << 5, /* octet5 b5 */
    A2DP_AAC_SAMPLE_BITS_OCTET5_BIT6_USER = 0x01 << 6, /* octet5 b6 */
    A2DP_AAC_SAMPLE_BITS_OCTET5_BIT7_USER = 0x01 << 7, /* octet5 b6 */
};

/**
 * @brief Pbap action Type.
 *
 * @since 6
 */
enum PbapActionType : uint16_t {
    PBAP_ACTION_PULLPHONEBOOKSIZE,     // pull phonebooksize
    PBAP_ACTION_PULLPHONEBOOK,         // pull phonebook
    PBAP_ACTION_SETPHONEBOOK,          // set  phonebook
    PBAP_ACTION_PULLVCARDLISTINGSIZE,  // pull vcard listing size
    PBAP_ACTION_PULLVCARDLISTING,      // pull vcard listing
    PBAP_ACTION_PULLVCARDENTRY,        // pull vcard entry
    PBAP_ACTION_ABORT,                 // abort downloading
};

// PBAP_PHONEBOOK_NAME
#define PBAP_PHONEBOOK_PB u"/telecom/pb.vcf"
#define PBAP_PHONEBOOK_ICH u"/telecom/ich.vcf"
#define PBAP_PHONEBOOK_OCH u"/telecom/och.vcf"
#define PBAP_PHONEBOOK_MCH u"/telecom/mch.vcf"
#define PBAP_PHONEBOOK_CCH u"/telecom/cch.vcf"
#define PBAP_PHONEBOOK_SPD u"/telecom/spd.vcf"
#define PBAP_PHONEBOOK_FAV u"/telecom/fav.vcf"
#define PBAP_PHONEBOOK_PB1 u"/SIM1/telecom/pb.vcf"
#define PBAP_PHONEBOOK_ICH1 u"/SIM1/telecom/ich.vcf"
#define PBAP_PHONEBOOK_OCH1 u"/SIM1/telecom/och.vcf"
#define PBAP_PHONEBOOK_MCH1 u"/SIM1/telecom/mch.vcf"
#define PBAP_PHONEBOOK_CCH1 u"/SIM1/telecom/cch.vcf"

enum PbapOrderType : uint8_t {
    PBAP_ORDER_INDEXED = 0x00,       // indexed order
    PBAP_ORDER_ALPHANUMERIC = 0x01,  // alphabetical order
    PBAP_ORDER_PHONETIC = 0x02,      // phonetic order
};

enum PbapFormatType : uint8_t {
    PBAP_FORMAT_VCARD2_1 = 0x00,  // vCard Version
    PBAP_FORMAT_VCARD3_0 = 0x01,  // vCard Version
};

// 5.1.4.1 PropertySelector {PropertyMask (64-bit value)}
#define PBAP_PROPERTY_VERSION (1ULL << 0)                // vCard Version
#define PBAP_PROPERTY_FN (1ULL << 1)                     // Formatted Name
#define PBAP_PROPERTY_N (1ULL << 2)                      // Structured Presentation of Name
#define PBAP_PROPERTY_PHOTO (1ULL << 3)                  // Associated Image or Photo
#define PBAP_PROPERTY_BDAY (1ULL << 4)                   // Birthday
#define PBAP_PROPERTY_ADR (1ULL << 5)                    // Delivery Address
#define PBAP_PROPERTY_LABEL (1ULL << 6)                  // Delivery
#define PBAP_PROPERTY_TEL (1ULL << 7)                    // Telephone Number
#define PBAP_PROPERTY_EMAIL (1ULL << 8)                  // Electronic Mail Address
#define PBAP_PROPERTY_MAILER (1ULL << 9)                 // Electronic Mail
#define PBAP_PROPERTY_TZ (1ULL << 10)                    // Time Zone
#define PBAP_PROPERTY_GEO (1ULL << 11)                   // Geographic Position
#define PBAP_PROPERTY_TITLE (1ULL << 12)                 // Job
#define PBAP_PROPERTY_ROLE (1ULL << 13)                  // Role within the Organization
#define PBAP_PROPERTY_LOGO (1ULL << 14)                  // Organization Logo
#define PBAP_PROPERTY_AGENT (1ULL << 15)                 // vCard of Person Representing
#define PBAP_PROPERTY_ORG (1ULL << 16)                   // Name of Organization
#define PBAP_PROPERTY_NOTE (1ULL << 17)                  // Comments
#define PBAP_PROPERTY_REV (1ULL << 18)                   // Revision
#define PBAP_PROPERTY_SOUND (1ULL << 19)                 // Pronunciation of Name
#define PBAP_PROPERTY_URL (1ULL << 20)                   // Uniform Resource Locator
#define PBAP_PROPERTY_UID (1ULL << 21)                   // Unique ID
#define PBAP_PROPERTY_KEY (1ULL << 22)                   // Public Encryption Key
#define PBAP_PROPERTY_NICKNAME (1ULL << 23)              // Nickname
#define PBAP_PROPERTY_CATEGORIES (1ULL << 24)            // Categories
#define PBAP_PROPERTY_PROID (1ULL << 25)                 // Product ID
#define PBAP_PROPERTY_CLASS (1ULL << 26)                 // Class information
#define PBAP_PROPERTY_SORT_STRING (1ULL << 27)           // String used for sorting operations
#define PBAP_PROPERTY_X_IRMC_CALL_DATETIME (1ULL << 28)  // Time stamp
#define PBAP_PROPERTY_X_BT_SPEEDDIALKEY (1ULL << 29)     // Speed-dial shortcut
#define PBAP_PROPERTY_X_BT_UCI (1ULL << 30)              // Uniform Caller Identifier
#define PBAP_PROPERTY_X_BT_UID (1ULL << 31)              // Bluetooth Contact Unique Identifier
#define PBAP_PROPERTY_PROPRIENTARY_FILTER (1ULL << 39)   // Bluetooth Proprientary

enum PbapSelectorOperatorType : uint8_t {
    PBAP_SELECTOR_OPERATOR_OR = 0x00,   // SelectorOperator Or
    PBAP_SELECTOR_OPERATOR_AND = 0x01,  // SelectorOperator And
};

enum PbapSearchPropertyType : uint8_t {
    PBAP_SEARCH_PROPERTY_NAME = 0x00,    // name
    PBAP_SEARCH_PROPERTY_NUMBER = 0x01,  // number
    PBAP_SEARCH_PROPERTY_SOUND = 0x02,   // sound
};

const uint8_t PBAP_FLAG_GO_TO_ROOT = 0x02;  // go back to root
const uint8_t PBAP_FLAG_GO_DOWN = 0x02;     // go down
const uint8_t PBAP_FLAG_GO_UP = 0x03;       // go up

enum PbapResponseCode : uint8_t {
    PBAP_CONTINUE = 0x90,                    // 100 Continue
    PBAP_SUCCESS = 0xA0,                     // 200 OK, Success
    PBAP_CREATED = 0xA1,                     // 201 Created
    PBAP_PBAP_ACCEPTED = 0xA2,               // 202 Accepted
    PBAP_NON_AUTH = 0xA3,                    // 203 Non-Authoritative Information
    PBAP_NO_CONTENT = 0xA4,                  // 204 No Content
    PBAP_RESET_CONTENT = 0xA5,               // 205 Reset Content
    PBAP_PARTIAL_CONTENT = 0xA6,             // 206 Partial Content
    PBAP_MULTIPLE_CHOICES = 0xB0,            // 300 Multiple Choices
    PBAP_MOVED_PERMANENTLY = 0xB1,           // 301 Moved Permanently
    PBAP_MOVED_TEMPORARILY = 0xB2,           // 302 Moved temporarily
    PBAP_SEE_OTHER = 0xB3,                   // 303 See Other
    PBAP_NOT_MODIFIED = 0xB4,                // 304 Not modified
    PBAP_USE_PROXY = 0xB5,                   // 305 Use Proxy
    PBAP_BAD_REQUEST = 0xC0,                 // 400 Bad Request - server couldn’t understand request
    PBAP_UNAUTHORIZED = 0xC1,                // 401 Unauthorized
    PBAP_PAYMENT_REQUIRED = 0xC2,            // 402 Payment required
    PBAP_FORBIDDEN = 0xC3,                   // 403 Forbidden - operation is understood but refused
    PBAP_NOT_FOUND = 0xC4,                   // 404 Not Found
    PBAP_METHOD_NOT_ALLOWED = 0xC5,          // 405 Method not allowed
    PBAP_NOT_ACCEPTABLE = 0xC6,              // 406 Not Acceptable
    PBAP_PROXY_AUTH_REQUIRED = 0xC7,         // 407 Proxy Authentication required
    PBAP_REQUEST_TIME_OUT = 0xC8,            // 408 Request Time Out
    PBAP_CONFLICT = 0xC9,                    // 409 Conflict
    PBAP_GONE = 0xCA,                        // 410 Gone
    PBAP_LENGTH_REQUIRED = 0xCB,             // 411 Length Required
    PBAP_PRECONDITION_FAILED = 0xCC,         // 412 Precondition failed
    PBAP_REQUESTED_ENTITY_TOO_LARGE = 0xCD,  // 413 Requested entity too large
    PBAP_REQUEST_URL_TOO_LARGE = 0xCE,       // 414 Request URL too large
    PBAP_UNSUPPORTED_MEDIA_TYPE = 0xCF,      // 415 Unsupported media type
    PBAP_INTERNAL_SERVER_ERROR = 0xD0,       // 500 Internal Server Error
    PBAP_NOT_IMPLEMENTED = 0xD1,             // 501 Not Implemented
    PBAP_BAD_GATEWAY = 0xD2,                 // 502 Bad Gateway
    PBAP_SERVICE_UNAVAILABLE = 0xD3,         // 503 Service Unavailable
    PBAP_GATEWAY_TIMEOUT = 0xD4,             // 504 Gateway Timeout
    PBAP_HTTP_VERSION_NOT_SUPPORTED = 0xD5,  // 505 HTTP version not supported
    PBAP_DATABASE_FULL = 0xE0,               // Database Full
    PBAP_DATABASE_LOCKED = 0xE1              // Database Locked
};

#define IS_BT_ENABLED() \
    ((BluetoothHost::GetDefaultHost().GetBtState() == (int)(BTStateID::STATE_TURN_ON)) ? true : false)
#define IS_BLE_ENABLED() (BluetoothHost::GetDefaultHost().IsBleEnabled())

#ifdef BLUETOOTH_EXPORT
#define BLUETOOTH_API __attribute__((visibility("default")))
#else
#define BLUETOOTH_API
#endif
} // namespace Bluetooth
} // namespace OHOS
#endif  // BLUETOOTH_DEF_H