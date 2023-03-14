/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_DM_CONSTANTS_H
#define OHOS_DM_CONSTANTS_H

#include <string>

namespace OHOS {
namespace DistributedHardware {
const std::string DM_PKG_NAME = "ohos.distributedhardware.devicemanager";
const std::string DM_SESSION_NAME = "ohos.distributedhardware.devicemanager.resident";

const std::string DISCOVER_STATUS_KEY = "persist.distributed_hardware.device_manager.discover_status";
const std::string DISCOVER_STATUS_ON = "1";
const std::string DISCOVER_STATUS_OFF = "0";
const std::string AUTH_LOAD_JSON_KEY = "devicemanager_auth_components";
const std::string ADAPTER_LOAD_JSON_KEY = "devicemanager_adapter_components";
const std::string AUTH_JSON_TYPE_KEY = "AUTHENTICATE";
const std::string CPYPTO_JSON_TYPE_KEY = "CPYPTO";
const std::string PROFILE_JSON_TYPE_KEY = "PROFILE";
const std::string DECISION_JSON_TYPE_KEY = "DECISION";

const int32_t DM_NAPI_SUBSCRIBE_CAPABILITY_DDMP = 0;
const int32_t DM_NAPI_SUBSCRIBE_CAPABILITY_OSD = 1;
const int32_t MIN_PIN_TOKEN = 10000000;
const int32_t MAX_PIN_TOKEN = 90000000;
const int32_t MIN_PIN_CODE = 100000;
const int32_t MAX_PIN_CODE = 999999;
const int32_t DISCOVER_STATUS_LEN = 20;
const int32_t COMMON_CALLBACK_MAX_SIZE = 200;

enum {
    DM_OK = 0,
    DM_FAILED = 1000,
    DM_TIME_OUT,
    DM_NOT_INIT,
    DM_INT_MULTIPLE,
    DM_INIT_FAILED,
    DM_UNINIT_FAILED,
    DM_POINT_NULL,
    DM_INPUT_PARA_EMPTY,
    DM_NO_PERMISSION,
    DM_INVALID_VALUE,
    DM_FLATTEN_OBJECT,
    DM_MALLOC_ERROR,
    DM_COPY_FAILED,
    DM_WRITE_FAILED,
    DM_DISCOVERY_FAILED,
    DM_MAKE_SHARED_FAIL,
    DM_SERVICE_NOT_READY,
    DM_DEVICE_ALREADY_TRUSTED,
    ERR_DM_KEY_ALREADY_EXISTS,
    ERR_DM_INPUT_PARA_INVALID,
    ERR_DM_SUBSCRIBE_DP_EVENTS,
    ERR_DM_UNSUBSCRIBE_DP_EVENTS,
    DM_IPC_FAILED = 2000,
    DM_IPC_TRANSACTION_FAILED,
    DM_IPC_FLATTEN_OBJECT,
    DM_IPC_COPY_FAILED,
    DM_IPC_SEND_REQUEST_FAILED,
    DM_IPC_NOT_REGISTER_FUNC,
    DM_IPC_RESPOND_ERROR,
    DM_IPC_WRITE_TOKEN_ERROR,
    DM_DISCOVERY_REPEATED,
    DM_AUTH_NOT_SUPPORT,
    DM_AUTH_BUSINESS_BUSY,
    DM_AUTH_INPUT_FAILED,
    DM_AUTH_STATE_FAILED,
    DM_AUTH_DEVICE_AUTHED,
    DM_AUTH_OPEN_SESSION_FAILED,
    DM_AUTH_PEER_REJECT,
    DM_AUTH_NOT_AUTH,
    DM_AUTH_DONT_AUTH,
    DM_AUTH_NOT_START,
    DM_SOFTBUS_FAILED = 3000,
    DM_SOFTBUS_CREATE_SESSION_SERVER_FAILED,
    DM_HICHAIN_FAILED = 4000,
    DM_HICHAIN_GROUP_CREATE_FAILED,
    DM_HICHAIN_MEMBER_ADD_FAILED,
    DM_HICHAIN_CREATE_CHANNEL_FAILED,
};
const std::string TARGET_PKG_NAME_KEY = "targetPkgName";
const std::string HOST_PKG_NAME_KEY = "hostPackageName";
const std::string TAG_REQUESTER = "REQUESTER";
const std::string TAG_TOKEN = "TOKEN";
const std::string TAG_HOST = "HOST";
const std::string TAG_TARGET = "TARGET";
const std::string TAG_VISIBILITY = "VISIBILITY";
const std::string TAG_GROUPIDS = "GROUPIDLIST";
const std::string TAG_REPLY = "REPLY";
const std::string TAG_NET_ID = "NETID";
const std::string TAG_GROUP_ID = "groupId";
const std::string TAG_GROUP_NAME = "GROUPNAME";
const std::string TAG_REQUEST_ID = "REQUESTID";
const std::string TAG_DEVICE_ID = "DEVICEID";
const std::string TAG_LOCAL_DEVICE_ID = "LOCALDEVICEID";
const std::string TAG_DEVICE_TYPE = "DEVICETYPE";
const std::string TAG_APP_NAME = "APPNAME";
const std::string TAG_APP_DESCRIPTION = "APPDESC";
const std::string TAG_APP_ICON = "APPICON";
const std::string TAG_APP_THUMBNAIL = "APPTHUM";
const std::string TAG_INDEX = "INDEX";
const std::string TAG_SLICE_NUM = "SLICE";
const std::string TAG_THUMBNAIL_SIZE = "THUMSIZE";
const std::string TAG_AUTH_TYPE = "AUTHTYPE";
const std::string TAG_CRYPTO_SUPPORT = "CRYPTOSUPPORT";
const std::string TAG_CRYPTO_NAME = "CRYPTONAME";
const std::string TAG_CRYPTO_VERSION = "CRYPTOVERSION";
const std::string TAG_VER = "ITF_VER";
const std::string TAG_TYPE = "MSG_TYPE";
const std::string DM_ITF_VER_1_0 = "1.0";
const std::string DM_ITF_VER = "1.1";
const std::string TAG = "DM_MSG_CODEC";
const std::string APP_NAME_KEY = "appName";
const std::string APP_DESCRIPTION_KEY = "appDescription";
const std::string APP_ICON_KEY = "appIcon";
const std::string APP_THUMBNAIL_KEY = "appThumbnailKey";
const std::string APP_THUMBNAIL_LEN = "appThumbnailLen";
const std::string APP_THUMBNAIL = "appThumbnail";
const std::string CANCEL_DISPLAY_KEY = "cancelPinCodeDisplay";
const int32_t MSG_MAX_SIZE = 45 * 1024;
const int32_t AUTH_REPLY_ACCEPT = 0;
const int32_t ENCRYPT_TAG_LEN = 32;
const int32_t SERVICE_INIT_TRY_MAX_NUM = 200;
const int32_t SLEEP_TIME_MS = 50000; // 50ms

// pin
const int32_t DISPLAY_OWNER_SYSTEM = 0;
const int32_t DISPLAY_OWNER_OTHER = 1;
const int32_t BUSINESS_FA_MIRGRATION = 0;
const int32_t BUSINESS_RESOURCE_ACCESS = 1;

// json
const std::string AUTH_TYPE = "authType";
const std::string TOKEN = "token";
const std::string PIN_TOKEN = "pinToken";
const std::string PIN_CODE_KEY = "pinCode";
const std::string NFC_CODE_KEY = "nfcCode";
const std::string QR_CODE_KEY = "qrCode";
const std::string TAG_AUTH_TOKEN = "authToken";
const int32_t AUTH_TYPE_PIN = 1;
const int32_t AUTH_TYPE_SCAN = 2;
const int32_t AUTH_TYPE_TOUCH = 3;
const int32_t DEFAULT_PIN_CODE = 0;
const int32_t DEFAULT_PIN_TOKEN = 0;

// Softbus
const int32_t SOFTBUS_CHECK_INTERVAL = 100000; // 100ms
const uint32_t SOFTBUS_SUBSCRIBE_ID_PREFIX_LEN = 16;
const int32_t SOFTBUS_SUBSCRIBE_ID_MASK = 0x0000FFFF;
const int32_t SOFTBUS_DISCOVER_DEVICE_INFO_MAX_SIZE = 20;
const int32_t AUTH_SESSION_SIDE_SERVER = 0;
const int32_t AUTH_SESSION_SIDE_CLIENT = 1;
const static char *DM_CAPABILITY_OSD = "osdCapability";

// HiChain
const int32_t DEVICE_UUID_LENGTH = 65;
const int32_t GROUP_TYPE_IDENTICAL_ACCOUNT_GROUP = 1;
const int32_t GROUP_TYPE_PEER_TO_PEER_GROUP = 256;
const int32_t GROUP_VISIBILITY_PUBLIC = -1;
const int64_t MIN_REQUEST_ID = 1000000000;
const int64_t MAX_REQUEST_ID = 9999999999;
const int32_t FIELD_EXPIRE_TIME_VALUE = 7;
const int32_t GROUP_VISIBILITY_IS_PUBLIC = -1;
const int32_t GROUP_VISIBILITY_IS_PRIVATE = 0;
const std::string DEVICE_ID = "DEVICE_ID";
const std::string WIFI_IP = "WIFI_IP";
const std::string WIFI_PORT = "WIFI_PORT";
const std::string BR_MAC = "BR_MAC";
const std::string BLE_MAC = "BLE_MAC";
const std::string ETH_IP = "ETH_IP";
const std::string ETH_PORT = "ETH_PORT";

// ACE
const int32_t ACE_X = 50;
const int32_t ACE_Y = 300;
const int32_t ACE_WIDTH = 720;
const int32_t ACE_HEIGHT = 694;
const std::string EVENT_CONFIRM = "EVENT_CONFIRM";
const std::string EVENT_CANCEL = "EVENT_CANCEL";
const std::string EVENT_INIT = "EVENT_INIT";
const std::string EVENT_CONFIRM_CODE = "0";
const std::string EVENT_CANCEL_CODE = "1";
const std::string EVENT_INIT_CODE = "2";

// timer
const std::string TIMER_PREFIX = "deviceManagerTimer:";
const std::string AUTHENTICATE_TIMEOUT_TASK = TIMER_PREFIX + "authenticate";
const std::string NEGOTIATE_TIMEOUT_TASK = TIMER_PREFIX + "negotiate";
const std::string CONFIRM_TIMEOUT_TASK = TIMER_PREFIX + "confirm";
const std::string SHOW_TIMEOUT_TASK = TIMER_PREFIX + "show";
const std::string INPUT_TIMEOUT_TASK = TIMER_PREFIX + "input";
const std::string ADD_TIMEOUT_TASK = TIMER_PREFIX + "add";
const std::string WAIT_NEGOTIATE_TIMEOUT_TASK = TIMER_PREFIX + "waitNegotiate";
const std::string WAIT_REQUEST_TIMEOUT_TASK = TIMER_PREFIX + "waitRequest";
const std::string STATE_TIMER_PREFIX = "stateTimer_";
const int32_t TIMER_PREFIX_LENGTH = 19;
const int32_t TIMER_DEFAULT = 0;
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DM_CONSTANTS_H
