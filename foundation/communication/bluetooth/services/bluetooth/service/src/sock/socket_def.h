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

#ifndef SOCKET_DEF_H
#define SOCKET_DEF_H

#include <stdint.h>

namespace OHOS {
namespace bluetooth {
// Socket default MTU.
static const int SOCK_DEF_RFC_MTU = 1500;
// Maximum number of socket service.
static const int SOCK_MAX_SERVICE_ID = 36;
// Invalid Socket fd.
static const int SOCK_INVALID_FD = -1;
// Maximum number of clients that can be connected.
static const int SOCK_MAX_CLIENT = 6;
// Require the connection to be encrypted.
static const int SOCK_FLAG_ENCRYPTION = 1;
// Require the connection to be authenticated.
static const int SOCK_FLAG_AUTHENTICATION = 1 << 1;
// Maximum number of socket server.
static const int SOCK_MAX_SERVER = 30;

// Transport event declarations
// The event is triggered when connection failed.
static const int RFCOMM_CONNECT_FAIL = 0x0001;
// The event is triggered when the disconnection process is successful.
static const int RFCOMM_DISCONNECT_SUCCESS = 0x0002;
// The event is triggered when the disconnection process fails.
static const int RFCOMM_DISCONNECT_FAIL = 0x0003;
// The event is triggered when peer or RFCOMM is available to receive data.
static const int RFCOMM_EV_FC_ON = 0x0004;

// SPP default UUID.
static constexpr uint16_t UUID_SERVCLASS_SERIAL_PORT = 0X1101;
// SPP version.
static const int SPP_PROFILE_VERSION = 0x0102;

/**
 * @brief Socket type.
 */
enum SocketType {
    SOCK_RFCOMM,   // RFCOMM socket
    SOCK_L2CAP,    // L2CAP socket
    SOCK_L2CAP_LE  // L2CAP_LE socket
};

/**
 * @brief Socket state.
 */
enum SocketState { INIT, LISTEN, CONNECTING, CONNECTED, DISCONNECTED, CLOSED };

/**
 * @brief Socket event.
 */
enum {
    SOCKET_SDP_DISCOVERY_RESULT,  // client SDP search completed.
    SOCKET_CLOSE,                 // close socket.
    SOCKET_ACCEPT_NEW,            // server accept a connection.
    SOCKET_CLOSE_COMPLETE,
};

/**
 * @brief Socket sdp index.
 */
enum { SOCK_SDP_IDX0 = 0, SOCK_SDP_IDX1 = 1, SOCK_SDP_IDX2 = 2 };

/**
 * @brief Service sends messages to app through socket.
 */
typedef struct {
    bool status;      // connect state
    uint8_t addr[6];  // remote device address
} SocketConnectInfo;
}  // namespace bluetooth
}  // namespace OHOS

#endif // SOCKET_DEF_H