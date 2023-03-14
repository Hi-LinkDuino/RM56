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

#ifndef PAN_DEFINES_H
#define PAN_DEFINES_H

#include <cstdint>
#include <string>

#include "gatt_data.h"
#include "log.h"
#include "l2cap_if.h"
#include "packet.h"

namespace OHOS {
namespace bluetooth {
static constexpr int PAN_SUCCESS = 0;
static constexpr int PAN_FAILURE = 1;

static constexpr int PAN_STATE_DISCONNECTED = 0;
static constexpr int PAN_STATE_CONNECTING = 1;
static constexpr int PAN_STATE_DISCONNECTING = 2;
static constexpr int PAN_STATE_CONNECTED = 3;

static constexpr int PAN_SERVICE_STARTUP_EVT = 1;
static constexpr int PAN_SERVICE_SHUTDOWN_EVT = 2;
static constexpr int PAN_API_CLOSE_EVT = 3;
static constexpr int PAN_INT_OPEN_EVT = 4;
static constexpr int PAN_INT_CLOSE_EVT = 5;
static constexpr int PAN_OPEN_CMPL_EVT = 6;
static constexpr int PAN_API_WRITE_DATA_EVT = 7;
static constexpr int PAN_INT_DATA_EVT = 8;
static constexpr int PAN_CONNECTION_TIMEOUT_EVT = 9;
static constexpr int PAN_DISCONNECTION_TIMEOUT_EVT = 10;

static constexpr int PAN_REMOVE_STATE_MACHINE_EVT = 11;

static constexpr int BNEP_L2CAP_START_EVT = 100;
static constexpr int BNEP_L2CAP_CONNECT_REQ_EVT = 101;
static constexpr int BNEP_L2CAP_CONNECT_RSP_EVT = 102;
static constexpr int BNEP_L2CAP_CONFIG_REQ_EVT = 103;
static constexpr int BNEP_L2CAP_CONFIG_RSP_EVT = 104;
static constexpr int BNEP_L2CAP_DISCONNECT_REQ_EVT = 105;
static constexpr int BNEP_L2CAP_DISCONNECT_RSP_EVT = 106;
static constexpr int BNEP_L2CAP_DISCONNECT_ABNORMAL_EVT = 107;
static constexpr int BNEP_L2CAP_DATA_EVT = 108;
static constexpr int BNEP_L2CAP_REMOTE_BUSY_EVT = 109;
static constexpr int BNEP_L2CAP_SECURITY_RESULT_EVT = 110;
static constexpr int BNEP_L2CAP_IND_SECURITY_RESULT_EVT = 111;
static constexpr int BNEP_L2CAP_END_EVT = 112;
static const std::string PAN_SERVICE_NAME = "Personal Area Network NAP";
static const std::string PAN_SERVICE_DESCRIPTION = "NAP";
static constexpr uint16_t BT_BNEP_PSM = 0x000F;

static constexpr int BNEP_CONN_STATE_UNUSED = 0;
static constexpr int BNEP_CONN_STATE_CONNECTING = 1;
static constexpr int BNEP_CONN_STATE_CONFIG = 2;
static constexpr int BNEP_CONN_STATE_CONNECTED = 3;
static constexpr int BNEP_CONN_STATE_DISCONNECTING = 4;
static constexpr int BNEP_CONN_STATE_SECURITY = 5;
static constexpr int BNEP_CONN_STATE_SETUP_CONNECTION = 6;

static constexpr uint8_t BNEP_CONN_FLAGS_IS_ORIG = 0x01;
static constexpr uint8_t BNEP_CONN_FLAGS_HIS_CFG_DONE = 0x02;
static constexpr uint8_t BNEP_CONN_FLAGS_MY_CFG_DONE = 0x04;
static constexpr uint8_t BNEP_CONN_FLAGS_ALL_CONFIGURED = 0x06;

static constexpr int BNEP_MTU_SIZE = 1691;
static constexpr int BNEP_UINT16_SIZE = 2;

static constexpr uint8_t BNEP_CONTROL_COMMAND_NOT_UNDERSTOOD_LENGTH = 3;
static constexpr uint8_t BNEP_CONTROL_RESPONSE_LENGTH = 4;
static constexpr uint8_t BNEP_GENERAL_ETHERNET_HEAD_LENGTH = 15;
static constexpr uint8_t BNEP_COMPRESSED_ETHERNET_HEAD_LENGTH = 3;
static constexpr uint8_t BNEP_COMPRESSED_ETHERNET_SRC_ONLY_HEAD_LENGTH = 9;
static constexpr uint8_t BNEP_COMPRESSED_ETHERNET_DEST_ONLY_HEAD_LENGTH = 9;

static constexpr uint8_t BNEP_MAX_PROTOCOL_FILTER = 5;
static constexpr uint8_t BNEP_MAX_MULTYCAST_FILTER = 5;
static constexpr uint8_t BNEP_PROTOCOL_FILTER_RANGE_LENGTH = 4;
static constexpr uint8_t BNEP_MULTYCAST_FILTER_RANGE_LENGTH = 12;

static constexpr uint8_t BNEP_GENERAL_ETHERNET = 0;
static constexpr uint8_t BNEP_CONTROL = 1;
static constexpr uint8_t BNEP_COMPRESSED_ETHERNET = 2;
static constexpr uint8_t BNEP_COMPRESSED_ETHERNET_SRC_ONLY = 3;
static constexpr uint8_t BNEP_COMPRESSED_ETHERNET_DEST_ONLY = 4;

static constexpr uint8_t BNEP_CONTROL_COMMAND_NOT_UNDERSTOOD = 0;
static constexpr uint8_t BNEP_SETUP_CONNECTION_REQUEST_MSG = 1;
static constexpr uint8_t BNEP_SETUP_CONNECTION_RESPONSE_MSG = 2;
static constexpr uint8_t BNEP_FILTER_NET_TYPE_SET_MSG = 3;
static constexpr uint8_t BNEP_FILTER_NET_TYPE_RESPONSE_MSG = 4;
static constexpr uint8_t BNEP_FILTER_MULTI_ADDR_SET_MSG = 5;
static constexpr uint8_t BNEP_FILTER_MULTI_ADDR_RESPONSE_MSG = 6;

static constexpr uint8_t BNEP_EXTENTION_CONTROL = 0;

static constexpr uint16_t BNEP_SETUP_SUCCESSFULL = 0x0000;
static constexpr uint16_t BNEP_SETUP_FAILED_INVALID_DEST_UUID = 0x0001;
static constexpr uint16_t BNEP_SETUP_FAILED_INVALID_SRC_UUID = 0x0002;
static constexpr uint16_t BNEP_SETUP_FAILED_INVALID_UUID_SIZE = 0x0003;
static constexpr uint16_t BNEP_SETUP_FAILED_CONN_NOT_ALLOWED = 0x0004;

static constexpr uint16_t BNEP_FILTER_SUCCESSFULL = 0x0000;
static constexpr uint16_t BNEP_FILTER_UNSUPPORTED_REQUEST = 0x0001;
static constexpr uint16_t BNEP_FILTER_FAILED_INVALID_NET_TYPE_RANGE = 0x0002;
static constexpr uint16_t BNEP_FILTER_FAILED_TOO_MANY_FILTER = 0x0003;
static constexpr uint16_t BNEP_SETUP_FAILED_SECURITY_ERROR = 0x0004;

static constexpr uint16_t BNEP_UUID_PANU = 0x1115;
static constexpr uint16_t BNEP_UUID_NAP = 0x1116;

static constexpr int BT_ADDRESS_LENGTH = 6;
static constexpr int BNEP_MAX_WAITING_SEND_DATA_LIST_NUMBER = 20;

static const char PAN_DEVICE_PATH[] = { "/dev/tun" };
static const char PAN_NETWORK_NAME[] = { "bt-pan" };
static const char PAN_NETWORK_IPV4_ADDRESS[] = { "192.168.44.1" };
static constexpr int PAN_NETWORK_IPV4_PREFIX_LENGTH = 24;
static constexpr int MAX_MASK_LENGTH = 32;
static constexpr int PAN_MAX_NETWORK_PACKET_SIZE = 1600;

struct PanL2capConnectionInfo {
    uint16_t lpsm = 0;
    uint16_t lcid = 0;
    int result = 0;
    uint8_t id = 0;
    L2capConfigInfo cfg;
    uint8_t reason = 0;
    uint8_t isBusy = 0;
};

struct EthernetHeader {
    uint8_t destAddr[BT_ADDRESS_LENGTH];
    uint8_t srcAddr[BT_ADDRESS_LENGTH];
    uint16_t protocol;
};

struct BnepProtocolFilterRange {
    uint16_t start;
    uint16_t end;
};

struct BnepMultycastFilterRange {
    uint8_t start[BT_ADDRESS_LENGTH];
    uint8_t end[BT_ADDRESS_LENGTH];
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // PAN_DEFINES_H
