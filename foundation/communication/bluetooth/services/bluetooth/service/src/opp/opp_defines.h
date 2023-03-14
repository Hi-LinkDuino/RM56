/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef OPP_DEFINES_H
#define OPP_DEFINES_H

#include <string>
#include "../obex/obex_types.h"

namespace OHOS {
namespace bluetooth {
static const std::string OPP_SERVICE_NAME = "OBEX Object Push";
static const std::string OPP_SEND_FILE_THREAD_NAME = "oppSendFile";
static const std::string OPP_RECEIVE_FILE_PATH = "/data/service/el1/public/bluetooth/";

static constexpr int OPP_STATE_DISCONNECTED = 0;
static constexpr int OPP_STATE_CONNECTING = 1;
static constexpr int OPP_STATE_DISCONNECTING = 2;
static constexpr int OPP_STATE_CONNECTED = 3;

static constexpr int OPP_SERVICE_STARTUP_EVT = 1;
static constexpr int OPP_SERVICE_SHUTDOWN_EVT = 2;
static constexpr int OPP_CONNECT_REQ_EVT = 3;
static constexpr int OPP_DISCONNECT_REQ_EVT = 4;
static constexpr int OPP_CONNECTED_EVT = 5;
static constexpr int OPP_DISCONNECTED_EVT = 6;
static constexpr int OPP_SDP_CMPL_EVT = 7;
static constexpr int OPP_GAP_CMPL_EVT = 8;
static constexpr int OPP_CONNECTION_TIMEOUT_EVT = 9;
static constexpr int OPP_DISCONNECTION_TIMEOUT_EVT = 10;

static constexpr int OPP_REMOVE_STATE_MACHINE_EVT = 11;

static constexpr uint16_t OPP_UUID16 = 0X1105;
static constexpr uint16_t OPP_VERSION_NUMBER = 0x0102;

static constexpr uint16_t OPP_GOEP_L2CAP_PSM = 0x1023;
static constexpr uint16_t OPP_GOEP_L2CAP_PSM_ATTRIBUTE_ID = 0x0200;
static constexpr uint16_t OPP_GOEP_SUPPORTED_FORMATS_LIST_ATTRIBUTE_ID = 0x0303;

static const uint8_t OPP_SUPPORTED_FORMATS_LIST_DATA[] = {
    0x08, 0x01 /* vCard 2.1 */,
    0x08, 0x02 /* vCard 3.0 */,
    0x08, 0x03 /* vCal 1.0 */,
    0x08, 0x04 /* iCal 2.0 */,
    0x08, 0xFF /* Any type of object */
};

static constexpr int OPP_SDP_SUCCESS = 0;
static constexpr int OPP_SDP_FAILED = 1;

static constexpr int OPP_GAP_SUCCESS = 0;
static constexpr int OPP_GAP_FAILED = 1;

static const uint16_t OPP_PROTOCOL_DESCRIPTOR_NUMBER = 3;

static constexpr int INCOMING_CONNECT_TIMEOUT_MS = 20000;
static constexpr int INCOMING_FILE_TIMEOUT_MS = 50000;

static constexpr int OPP_OBEX_STATUS_IDLE = 0;
static constexpr int OPP_OBEX_STATUS_WAITING_CONTINUE = 1;
static constexpr int OPP_OBEX_STATUS_WAITING_END = 3;

static constexpr int OPP_TRANSFER_DIRECTION_OUTBOND = 0;
static constexpr int OPP_TRANSFER_DIRECTION_INBOND = 1;

static constexpr int OPP_TRANSFER_STATUS_PENDING = 0;
static constexpr int OPP_TRANSFER_STATUS_RUNNING = 1;
static constexpr int OPP_TRANSFER_STATUS_SUCCESS = 2;
static constexpr int OPP_TRANSFER_STATUS_FAILD = 3;

static constexpr int OPP_TRANSFER_CONFIRM_PENDING = 0;
static constexpr int OPP_TRANSFER_CONFIRM_ACCEPT = 1;
static constexpr int OPP_TRANSFER_CONFIRM_REJECT = 2;

// remote reject file type
static constexpr int OPP_TRANSFER_FAILED_UNSUPPORTED_TYPE = 0;
// remote forbidden file
static constexpr int OPP_TRANSFER_FAILED_FORBIDDEN = 1;
// remote not acceptable
static constexpr int OPP_TRANSFER_FAILED_NOT_ACCEPTABLE = 2;
// remote canceled file transfer
static constexpr int OPP_TRANSFER_FAILED_CANCELED = 3;
// connection faild
static constexpr int OPP_TRANSFER_FAILED_CONNECTION_FAILED = 4;
// no storage to save file
static constexpr int OPP_TRANSFER_FAILED_NO_STORAGE = 5;
// opp protocol error
static constexpr int OPP_TRANSFER_FAILED_PROTOCOL = 6;
// send command or file error
static constexpr int OPP_TRANSFER_FAILED_SEND = 7;
// unknown error
static constexpr int OPP_TRANSFER_FAILED_UNKNOWN = 8;

struct OppConfig {
    int rfcommMtu = OBEX_DEFAULT_MTU;
    int l2capMtu = OBEX_DEFAULT_MTU;
};

struct OppSdpInformation {
    uint16_t psm = 0;
    uint8_t rfcommNo = 0;
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // PAN_DEFINES_H
