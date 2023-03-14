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

#ifndef CLASSIC_DEFS_H
#define CLASSIC_DEFS_H

#include "gap_if.h"
#include "raw_address.h"

/*
 * @brief The bluetooth system.
 */
namespace OHOS {
namespace bluetooth {
constexpr int SUCCESS = 0;
constexpr int FAILED = -1;

constexpr int REMOTE_TYPE_BREDR = 0;
constexpr int REMOTE_TYPE_LE = 1;
constexpr int REMOTE_TYPE_BREDR_LE = 2;

const std::string DEFAULT_DEVICE_NAME = "BluetoothDevice";
const std::string DEFAULT_PASSKEY = "12345678";
constexpr int DEFAULT_CLASS_OF_DEVICE = 0x2A0104;
constexpr int DEFAULT_SCANMODE_DURATION_MILLIS = 120000;
constexpr int DEFAULT_DISCOVERY_TIMEOUT_MS = 12800;
constexpr int DISCOVERY_DEVICE_LIST_MAX = 30;
constexpr int DEFAULT_INQ_MAX_DURATION = 10;
constexpr int DEFAULT_INQ_MIN_DURATION = 1;

constexpr int DEFAULT_HW_TIMEOUT = 5000;
constexpr int MILLISECOND_UNIT = 1000;
constexpr int MOVE_ONE_BYTE = 8;
constexpr int COD_SIZE = 3;

constexpr int PAIR_CANCELING = 0x04;
constexpr int PAIR_INVALID_LINK_KEY_TYPE = -1;
constexpr int PAIR_LINK_KEY_SIZE = 0x10;
constexpr int PAIR_PINCODE_SIZE = 0x10;
constexpr int PAIR_CONFIRM_TYPE_INVALID = 0x00;
constexpr int PAIR_CONFIRM_TYPE_OOB = 0x06;
constexpr int PAIR_CONFIRM_STATE_INVALID = 0x00;
constexpr int PAIR_CONFIRM_STATE_USER_CONFIRM = 0x01;
constexpr int PAIR_CONFIRM_STATE_USER_CONFIRM_REPLY = 0x02;

constexpr int NOERROR = 0;
constexpr int BADPARAM = -1000;
constexpr int BADSTATUS = -1001;
constexpr int TIMEOUT = -1002;
constexpr int OS_ERROR = -1003;
constexpr int NOMEMORY = -1004;
constexpr int NOSPACE = -1005;
constexpr int CREATE_FILE = -1006;
constexpr int CONFIG_ERROR = -1007;
constexpr int DEVICE_ERROR = -1008;

constexpr int CONNECTION_STATE_DISCONNECTED = 0x00;
constexpr int CONNECTION_STATE_CONNECTED = 0x01;
constexpr int CONNECTION_STATE_ENCRYPTED_BREDR = 0x02;
constexpr int CONNECTION_STATE_ENCRYPTED_LE = 0x04;

constexpr int MAC_ADDRESS_LENGTH = 6;

constexpr int SDP_UUDIID_NUM = 1;
constexpr int HEX_FORMAT_SIZE = 3;

constexpr int UUID128_BYTES_TYPE = 16;
constexpr int UUID32_BYTES_TYPE = 4;
constexpr int UUID16_BYTES_TYPE = 2;

/// Bluetooth Properties
constexpr int BT_PROPERTY_BDNAME = 0x01;
constexpr int BT_PROPERTY_BDADDR = 0x02;
constexpr int BT_PROPERTY_CLASS_OF_DEVICE = 0x03;
constexpr int BT_PROPERTY_ADAPTER_BONDED_MODE = 0x04;
constexpr int BT_PROPERTY_ADAPTER_DISCOVERABLE_TIMEOUT = 0x05;

/// Bluetooth device name's max length
constexpr int MAX_LOC_BT_NAME_LEN = 248;

/// Extend inquiry response max length
constexpr int MAX_EXTEND_INQUIRY_RESPONSE_LEN = 240;

constexpr int EXTEND_INQUIRY_RESPONSE_LENGTH_SIZE = 1;
constexpr int EXTEND_INQUIRY_RESPONSE_TYPE_SIZE = 1;

constexpr int BTM_ACL_DISCONNECT_REASON = 0x13;
constexpr int BTM_ACL_CONNECT_PAGE_TIMEOUT = 0x04;
constexpr int BTM_ACL_CONNECTION_TIMEOUT = 0x08;

/// Service class UUID
constexpr uint16_t PNP_SERVICE_CLASS_UUID = 0x1200;

/// The value range of Class of Device
constexpr int CLASS_OF_DEVICE_RANGE = 0xFFFFFF;

/// Class of Device defines
constexpr uint32_t CLASS_OF_DEVICE_MASK = 0x07FF;
constexpr uint32_t CLASS_OF_DEVICE_UNCLASSIFIED = ((0x1F) << 8);
constexpr uint32_t CLASS_OF_DEVICE_HID_KEYBOARD = 0x0540;
constexpr uint32_t CLASS_OF_DEVICE_HID_POINTING = 0x0580;
constexpr uint32_t CLASS_OF_DEVICE_HID_COMBO    = 0x05C0;
constexpr uint32_t CLASS_OF_DEVICE_HID_MAJOR    = 0x0700;
constexpr uint32_t CLASS_OF_DEVICE_AV_HEADSETS  = 0x0404;
constexpr uint32_t CLASS_OF_DEVICE_AV_HANDSFREE = 0x0408;
constexpr uint32_t CLASS_OF_DEVICE_AV_HEADPHONES = 0x0418;
constexpr uint32_t CLASS_OF_DEVICE_AV_PORTABLE_AUDIO = 0x041C;
constexpr uint32_t CLASS_OF_DEVICE_AV_HIFI_AUDIO = 0x0428;

/// GAP callback event type
typedef enum {
    GAP_INQUIRY_RESULT_EVT,
    GAP_INQUIRY_RESULT_RSSI_EVT,
    GAP_EXTENDED_INQUIRY_RESULT_EVT,
    GAP_INQUIRY_COMPLETE_EVT,
    GAP_REMOTE_NAME_CALLBACK_EVT,
    GAP_SSP_CONFIRM_REQ_EVT,
    GAP_PIN_CODE_REQ_EVT,
    GAP_REMOTE_OOB_REQ_EVT,
    GAP_LINK_KEY_REQ_EVT,
    GAP_LINK_KEY_NOTIFICATION_EVT,
    GAP_SIMPLE_PAIR_COMPLETE_EVT,
    GAP_IO_CAPABILITY_REQ_EVT,
    GAP_IO_CAPABILITY_RSP_EVT,
    GAP_AUTHENTICATION_COMPLETE_EVT,
    GAP_ENCRYPTION_CHANGE_CALLBACK_EVT,
    GAP_AUTHORIZE_IND_CALLBACK_EVT,
} GAP_CB_EVENT;

typedef union {
    struct InquiryResultParam {
        const BtAddr addr;
        uint32_t classOfDevice;
    } iniquiryResultParam_;

    struct InquiryResultRssiParam {
        const BtAddr addr;
        uint32_t classOfDevice;
        int8_t rssi;
    } iniquiryResultRssiParam_;

    struct ExtendedInquiryResultParam {
        const BtAddr addr;
        uint32_t classOfDevice;
        int8_t rssi;
        uint8_t eir[MAX_EXTEND_INQUIRY_RESPONSE_LEN];
    } extendedInquiryResultParam_;

    struct InquiryCompleteParam {
        uint8_t status;
    } inquiryCompleteParam_;

    struct RemoteNameCallbackParam {
        uint8_t status;
        const BtAddr addr;
        const uint8_t name[248];
    } remoteNameCallbackParam_;

    struct UserConfirmReqParam {
        const BtAddr addr;
        uint32_t number;
        int reqType;
    } userConfirmReqParam_;

    struct RemoteOobReqParam {
        const BtAddr addr;
    } remoteOobReqParam_;

    struct PinCodeReqParam {
        const BtAddr addr;
    } pinCodeReqParam_;

    struct LinkKeyReqParam {
        const BtAddr addr;
    } linkKeyReqParam_;

    struct LinkKeyNotificationParam {
        const BtAddr addr;
        const uint8_t linkKey[PAIR_LINK_KEY_SIZE];
        uint8_t keyType;
    } linkKeyNotificationParam_;

    struct SimplePairCompleteParam {
        const BtAddr addr;
        uint8_t status;
    } simplePairCompleteParam_;

    struct IoCapabilityReqParam {
        const BtAddr addr;
    } ioCapabilityReqParam_;

    struct IoCapabilityRspParam {
        const BtAddr addr;
        uint8_t ioCapability;
    } ioCapabilityRspParam_;

    struct AuthenticationCompleteParam {
        const BtAddr addr;
        uint8_t status;
    } authenticationCompleteParam_;

    struct EncryptionChangeCallbackParam {
        const BtAddr addr;
        uint8_t status;
    } encryptionChangeCallbackParam_;

    struct AuthorizeIndParam {
        const BtAddr addr;
        GAP_Service service;
    } authorizeIndParam_;
} GapCallbackParam;
}  // namespace bluetooth
}  // namespace OHOS
#endif  // CLASSIC_DEFS_H