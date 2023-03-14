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

#ifndef MAP_MSE_TYPES_H
#define MAP_MSE_TYPES_H

#include <cstdint>
#include <string>
#include "bt_def.h"
#include "log.h"

namespace OHOS {
namespace bluetooth {
#define MAP_MSE_L2CAP_ENABLE
#define MSE_LOG_DEBUG(fmt, ...) LOG_DEBUG("[MAP MSE]:" fmt, ##__VA_ARGS__)
#define MSE_LOG_INFO(fmt, ...) LOG_INFO("[MAP MSE]:" fmt, ##__VA_ARGS__)
#define MSE_LOG_ERROR(fmt, ...) LOG_ERROR("[MAP MSE]:" fmt, ##__VA_ARGS__)
enum Result { FAIL = 0, SUCCEED = 1 };
enum TagId {};
enum ObexCallBack {
    ON_CONNECTED,
    ON_DISCONNECTED,
    ON_GET,
    ON_PUT,
    ON_ABORT,
    ON_SET_PATH,
    ON_TRANSPORT_DISCONNECTED,
    ON_TRANSPORT_ERROR
};
enum class MessageType { NONE, EMAIL, SMS_GSM, SMS_CDMA, MMS, IM };
enum class Attachment : uint8_t { OFF = 0x00, ON = 0x01 };
enum class Transparent : uint8_t { OFF = 0x00, ON = 0x01 };
enum class Retry : uint8_t { OFF = 0x00, ON = 0x01 };
enum class NewMessage : uint8_t { OFF = 0x00, ON = 0x01 };
enum class NotificationStatus : uint8_t { OFF = 0x00, ON = 0x01 };
enum class Charset : uint8_t { NATIVE = 0x00, UTF_8 = 0x01 };
enum class FractionRequest : uint8_t { FIRST = 0x00, NEXT = 0x01 };
enum class FractionDeliver : uint8_t { MORE = 0x00, LAST = 0x01 };
enum class StatusIndicator : uint8_t { READ_STATUS = 0x00, DELETED_STATUS = 0x01, SET_EXTENDED_DATA = 0x02 };
enum class StatusValue : uint8_t { NO = 0x00, YES = 0x01 };
enum class ModifyText : uint8_t { REPLACE = 0x00, PREPEND = 0x01 };
static const std::string MAP_MSE_TYPE_EMAIL = "EMAIL";
static const std::string MAP_MSE_TYPE_SMS_GSM = "SMS_GSM";
static const std::string MAP_MSE_TYPE_SMS_CDMA = "SMS_CDMA";
static const std::string MAP_MSE_TYPE_MMS = "MMS";
static const std::string MAP_MSE_TYPE_IM = "IM";
/// map contract
static const std::string MAP_MSE_FOLDER_NAME_INBOX = "inbox";
static const std::string MAP_MSE_FOLDER_NAME_SENT = "sent";
static const std::string MAP_MSE_FOLDER_NAME_OUTBOX = "outbox";
static const std::string MAP_MSE_FOLDER_NAME_DRAFT = "draft";
static const std::string MAP_MSE_FOLDER_NAME_DELETED = "deleted";
static const std::string MAP_MSE_FOLDER_NAME_OTHER = "other";
enum ImFolderId {
    MAP_MSE_FOLDER_ID_OTHER = 0,
    MAP_MSE_FOLDER_ID_INBOX = 1,
    MAP_MSE_FOLDER_ID_SENT = 2,
    MAP_MSE_FOLDER_ID_DRAFT = 3,
    MAP_MSE_FOLDER_ID_OUTBOX = 4,
    MAP_MSE_FOLDER_ID_DELETED = 5
};
// SDP Record for the Message Access Service on the MSE Device
static const uint16_t MAS_SERVICE_CLASS_ID_NUMBER = 0x0001;
static const uint16_t MAS_SERVICE_CLASS_UUID = 0x1132;
static const uint16_t MAP_PROFILE_VERSION12 = 0x0102;
static const uint16_t MAP_PROFILE_VERSION13 = 0x0103;
static const uint16_t MAP_PROFILE_VERSION14 = 0x0104;
static const uint16_t MAP_PROFILE_DESCRIPTOR_UUID = 0x1134;
static const uint16_t MAP_PROFILE_DESCRIPTOR_NUMBER = 0x0001;
static const uint16_t PROTOCOL_DESCRIPTOR_NUMBER = 0x0003;
static const uint16_t PROTOCOL_DESCRIPTOR_PARAMETER_NUMBER = 0x01;
static const uint16_t MAS_BROWSE_GROUP_UUID_NUMBER = 0x0001;

// Supported Message Types
static const uint8_t MAP_SUPPORTED_MESSAGE_TYPE_EMAIL = 0x01;
static const uint8_t MAP_SUPPORTED_MESSAGE_TYPE_SMS_GSM = 0x02;
static const uint8_t MAP_SUPPORTED_MESSAGE_TYPE_SMS_CDMA = 0x04;
static const uint8_t MAP_SUPPORTED_MESSAGE_TYPE_MMS = 0x08;
static const uint8_t MAP_SUPPORTED_MESSAGE_TYPE_IM = 0x10;
static const uint16_t MAP_SUPPORTED_MESSAGE_TYPE_ATTRIBUTE_ID = 0x0316;

// GoepL2capPsm (MAP v1.2 and later)
static const uint16_t GOEP_L2CAP_PSM_ATTRIBUTE_ID = 0x0200;

// MASInstanceID
static const uint16_t MAS_INSTANCE_ATTRIBUTE_ID = 0x0315;

// incoming connect time out 25 sec
static const uint16_t MAP_MSE_INCOMING_TIME_OUT = 25000;
static const uint16_t MAP_MSE_DELAY_GRANT_TIME = 300;

// Map service name and type
static const std::string SMS_MMS_NAME = "SMS_MMS";
static const std::string EMAIL_NAME = "EMAIL";
static const std::string IM_NAME = "IM";

// Map Supported Features
static const uint16_t MAP_SUPPORTED_FEATURES_ATTRIBUTE_ID = 0x0317;
static const uint32_t MAP_SUPPORTED_FEATURES_V12 = 0x0000007F;
static const uint32_t MAP_SUPPORTED_FEATURES_V13 = 0x000603FF;
static const uint32_t MAP_SUPPORTED_FEATURES_V14 = 0x007EFFFF;
static const uint32_t MAP_SUPPORTED_FEATURES_DEFAULT = 0x0000001F;

static const uint32_t MAP_FEATURE_NOTIFICATION_REGISTRATION_BIT = uint32_t(1) << 0;
static const uint32_t MAP_FEATURE_NOTIFICATION_BIT = uint32_t(1) << 1;
static const uint32_t MAP_FEATURE_BROWSING_BIT = uint32_t(1) << 2;
static const uint32_t MAP_FEATURE_UPLOADING_BIT = uint32_t(1) << 3;
static const uint32_t MAP_FEATURE_DELETE_BIT = uint32_t(1) << 4;
static const uint32_t MAP_FEATURE_INSTANCE_INFORMATION_BIT = uint32_t(1) << 5;
static const uint32_t MAP_FEATURE_EXTENDED_EVENT_REPORT_11_BIT = uint32_t(1) << 6;
static const uint32_t MAP_FEATURE_EVENT_REPORT_V12_BIT = uint32_t(1) << 7;
static const uint32_t MAP_FEATURE_MESSAGE_FORMAT_V11_BIT = uint32_t(1) << 8;
static const uint32_t MAP_FEATURE_MESSAGE_LISTING_FORMAT_V11_BIT = uint32_t(1) << 9;
static const uint32_t MAP_FEATURE_PERSISTENT_MESSAGE_HANDLE_BIT = uint32_t(1) << 10;
static const uint32_t MAP_FEATURE_DATABASE_INDENTIFIER_BIT = uint32_t(1) << 11;
static const uint32_t MAP_FEATURE_FOLDER_VERSION_COUNTER_BIT = uint32_t(1) << 12;
static const uint32_t MAP_FEATURE_CONVERSATION_VERSION_COUNTER_BIT = uint32_t(1) << 13;
static const uint32_t MAP_FEATURE_PARTICIPANT_PRESENCE_CHANGE_BIT = uint32_t(1) << 14;
static const uint32_t MAP_FEATURE_PARTICIPANT_CHAT_STATE_CHANGE_BIT = uint32_t(1) << 15;

static const uint32_t MAP_FEATURE_PBAP_CONTACT_CROSS_REFERENCE_BIT = uint32_t(1) << 16;
static const uint32_t MAP_FEATURE_NOTIFICATION_FILTERING_BIT = uint32_t(1) << 17;
static const uint32_t MAP_FEATURE_DEFINED_TIMESTAMP_FORMAT_BIT = uint32_t(1) << 18;
static const uint32_t MAP_FEATURE_CONNECT_REQUEST_BIT = uint32_t(1) << 19;
static const uint32_t MAP_FEATURE_CONVERSATIONLISTING_BIT = uint32_t(1) << 20;
static const uint32_t MAP_FEATURE_OWNER_STATUS_BIT = uint32_t(1) << 21;
static const uint32_t MAP_FEATURE_MESSAGE_FORWARDING_BIT = uint32_t(1) << 22;

static const std::string MAP_V10 = "1.0";
static const std::string MAP_V11 = "1.1";
static const std::string MAP_V12 = "1.2";

// Event Report versions
static const uint8_t MAP_EVENT_REPORT_V10 = 0x10;  // MAP spec 1.1
static const uint8_t MAP_EVENT_REPORT_V11 = 0x11;  // MAP spec 1.2
static const uint8_t MAP_EVENT_REPORT_V12 = 0x12;  // MAP spec 1.3 'to be' incl. IM

// Message Format versions
static const uint8_t MAP_MESSAGE_FORMAT_V10 = 0x10;  // MAP spec below 1.3
static const uint8_t MAP_MESSAGE_FORMAT_V11 = 0x11;  // MAP spec 1.3

// Message Listing Format versions
static const uint8_t MAP_MESSAGE_LISTING_FORMAT_V10 = 0x10;  // MAP spec below 1.3
static const uint8_t MAP_MESSAGE_LISTING_FORMAT_V11 = 0x11;  // MAP spec 1.3

// Message handle mask
static const uint64_t MESSAGE_HANDLE_MASK = 0xff00000000000000;
static const uint64_t MESSAGE_HANDLE_MMS_MASK = 0x0100000000000000;
static const uint64_t MESSAGE_HANDLE_EMAIL_MASK = 0x0200000000000000;
static const uint64_t MESSAGE_HANDLE_SMS_GSM_MASK = 0x0400000000000000;
static const uint64_t MESSAGE_HANDLE_SMS_CDMA_MASK = 0x0800000000000000;
static const uint64_t MESSAGE_HANDLE_IM_MASK = 0x1000000000000000;

// Database name
static const std::string DEFAULT_MAP_MSE_DB_FILE = "map_mse.s3db";

// Table name
static const std::string MAP_MSE_TABLE_ACCOUNT = "account";
static const std::string MAP_MSE_TABLE_MESSAGE = "message";
static const std::string MAP_MSE_TABLE_FOLDER = "folder";
static const std::string MAP_MSE_TABLE_CONVERSATION = "conversation";
static const std::string MAP_MSE_TABLE_PARTICIPANT = "participant";
}  // namespace bluetooth
}  // namespace OHOS

#endif  // MAP_MSE_TYPES_H