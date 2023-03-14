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

#ifndef OHOS_WIFI_P2P_DEFINE_H
#define OHOS_WIFI_P2P_DEFINE_H

#include <any>
#include <string>
#include "wifi_msg.h"

namespace OHOS {
namespace Wifi {
#ifdef PRODUCT_RK
const std::string P2P_INTERFACE("wlan0");
#else
const std::string P2P_INTERFACE("p2p0");
#endif
/* The timeout interval of enable p2p */
constexpr long ENABLE_P2P_TIMED_OUT__INTERVAL = 5000;
/* The time of clears service requests processed in records. */
constexpr long REMOVE_SERVICE_REQUEST_RECORD = 3000;

enum class P2pStateMachineState : unsigned char {
    NO_SUPPORT,
    ENABLING,
    ENABLED,
    DISABLED,
};

enum class P2pStatus {
    SUCCESS = 0,
    INFORMATION_IS_CURRENTLY_UNAVAILABLE,
    INCOMPATIBLE_PARAMETERS,
    LIMIT_REACHED,
    INVALID_PARAMETERS,
    UNABLE_TO_ACCOMMODATE_REQUEST,
    PREVIOUS_PROTOCOL_ERROR,
    NO_COMMON_CHANNELS,
    UNKNOWN_P2P_GROUP,
    BOTH_DEVICE_INDICATED_INTENT_15,
    INCOMPATIBLE_PROVISIONING_METHOD,
    REJECTED_BY_USER,
    UNKNOWN = -9999,
};

enum class P2pStatusCode : int {
    SUCCESS = 0,
    FAIL_INFORMATION_IS_CURRENTLY_UNAVAILABLE = 1,
    FAIL_INCOMPATIBLE_PARAMETERS = 2,
    FAIL_LIMIT_REACHED = 3,
    FAIL_INVALID_PARAMETERS = 4,
    FAIL_UNABLE_TO_ACCOMMODATE_REQUEST = 5,
    FAIL_PREVIOUS_PROTOCOL_ERROR = 6,
    FAIL_NO_COMMON_CHANNELS = 7,
    FAIL_UNKNOWN_P2P_GROUP = 8,
    FAIL_BOTH_DEVICE_INDICATED_INTENT_15 = 9,
    FAIL_INCOMPATIBLE_PROVISIONING_METHOD = 10,
    FAIL_REJECTED_BY_USER = 11,
    SUCCESS_DEFERRED = 12,
};

enum class WpsDevPasswordId : short {
    DEFAULT = 0,
    USER_SPECIFIED = 1,
    MACHINE_SPECIFIED = 2,
    REKEY = 3,
    PUSHBUTTON = 4,
    REGISTRAR_SPECIFIED = 5,
    NFC_CONNECTION_HANDOVER = 7,
    P2PS_DEFAULT = 8,
};

enum class P2pConfigErrCode {
    SUCCESS = 0,
    MAC_EMPTY = 1,
    MAC_NOT_FOUND = 2,
    ERR_MAC_FORMAT = 3,
    ERR_INTENT = 4,
    ERR_SIZE_NW_NAME = 5,
};

enum class P2P_STATE_MACHINE_CMD {
    /* service to state machine */
    CMD_P2P_ENABLE = 0,
    CMD_P2P_DISABLE,
    CMD_DEVICE_DISCOVERS,
    CMD_STOP_DEVICE_DISCOVERS,
    CMD_DISCOVER_SERVICES,
    CMD_STOP_DISCOVER_SERVICES,
    CMD_REQUEST_SERVICE,
    CMD_PUT_LOCAL_SERVICE,
    CMD_DEL_LOCAL_SERVICE,
    CMD_START_LISTEN,
    CMD_STOP_LISTEN,
    CMD_FORM_GROUP,
    CMD_REMOVE_GROUP,
    CMD_DELETE_GROUP,
    CMD_CONNECT,
    CMD_DISCONNECT,
    CMD_SET_DEVICE_NAME,         /* set device name */
    CMD_SET_WFD_INFO,            /* set wifi-display info */
    CMD_CANCEL_CONNECT,          /* cancel connect */
    CMD_HID2D_CREATE_GROUP, /* hid2d create group */
    CMD_HID2D_CONNECT,

    /* monitor to state machine */
    WPA_CONNECTED_EVENT = 100,  // result of connect
    WPA_CONN_FAILED_EVENT,       // failed to connected WPA
    P2P_EVENT_DEVICE_FOUND,
    P2P_EVENT_DEVICE_LOST,
    P2P_EVENT_GO_NEG_REQUEST,  // received GO negotiation request
    P2P_EVENT_GO_NEG_SUCCESS,
    P2P_EVENT_GO_NEG_FAILURE,
    P2P_EVENT_INVITATION_RECEIVED,
    P2P_EVENT_INVITATION_RESULT,
    P2P_EVENT_GROUP_FORMATION_SUCCESS,
    P2P_EVENT_GROUP_FORMATION_FAILURE,
    P2P_EVENT_GROUP_STARTED,
    P2P_EVENT_GROUP_REMOVED,
    P2P_EVENT_PROV_DISC_PBC_REQ,
    P2P_EVENT_PROV_DISC_PBC_RESP,
    P2P_EVENT_PROV_DISC_ENTER_PIN,
    P2P_EVENT_PROV_DISC_SHOW_PIN,
    P2P_EVENT_FIND_STOPPED,
    P2P_EVENT_SERV_DISC_REQ,
    P2P_EVENT_SERV_DISC_RESP,
    P2P_EVENT_PROV_DISC_FAILURE,
    AP_STA_DISCONNECTED,
    AP_STA_CONNECTED,

    /* if the requests to WPA is not synchronization need protected by a timeout mechanism */
    ENABLE_P2P_TIMED_OUT = 200,
    INTERNAL_CONN_USER_CONFIRM,     // the user confirmed
    INTERNAL_CONN_USER_ACCEPT,      // the user chooses to agree
    PEER_CONNECTION_USER_REJECT,    // the user chooses to reject
    INTERNAL_CONN_USER_TIME_OUT,
    CREATE_GROUP_TIMED_OUT,
    EXCEPTION_TIMED_OUT,             /* P2P exception timeout */
    DISABLE_P2P_TIMED_OUT,
    REMOVE_SERVICE_REQUEST_RECORD,
};

using HandlerMethod = void(P2P_STATE_MACHINE_CMD, int, int, const std::any &);
} // namespace Wifi
} // namespace OHOS

#endif
