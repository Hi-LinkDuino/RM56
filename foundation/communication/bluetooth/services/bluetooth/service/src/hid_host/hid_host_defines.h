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

#ifndef HID_HOST_DEFINES_H
#define HID_HOST_DEFINES_H

#include <cstdint>
#include <string>

#include "gatt_data.h"
#include "log.h"
#include "l2cap_if.h"
#include "packet.h"

namespace OHOS {
namespace bluetooth {
static constexpr int HID_HOST_STATE_DISCONNECTED = 0;
static constexpr int HID_HOST_STATE_CONNECTING = 1;
static constexpr int HID_HOST_STATE_DISCONNECTING = 2;
static constexpr int HID_HOST_STATE_CONNECTED = 3;

static constexpr int HID_HOST_SUCCESS = 0;
static constexpr int HID_HOST_FAILURE = 1;

static constexpr int HID_HOST_DEVICE_TYPE_UNKNOWN = 0;
static constexpr int HID_HOST_DEVICE_TYPE_BREDR = 1;
static constexpr int HID_HOST_DEVICE_TYPE_BLE = 2;

static constexpr int HID_HOST_HOGP_STATE_UNUSED = 0;
static constexpr int HID_HOST_HOGP_STATE_CONNECTING = 1;
static constexpr int HID_HOST_HOGP_STATE_DISCOVERING = 2;
static constexpr int HID_HOST_HOGP_STATE_CONNECTED = 3;
static constexpr int HID_HOST_HOGP_STATE_DISCONNECTING = 4;

// Hid host event
static constexpr int HID_HOST_INVALID_EVT = 0;
// Service start/stop
static constexpr int HID_HOST_SERVICE_STARTUP_EVT = 1;
static constexpr int HID_HOST_SERVICE_SHUTDOWN_EVT = 2;
// Device open/close
static constexpr int HID_HOST_API_OPEN_EVT = 3;
static constexpr int HID_HOST_API_CLOSE_EVT = 4;
static constexpr int HID_HOST_INT_OPEN_EVT = 5;
static constexpr int HID_HOST_INT_CLOSE_EVT = 6;
static constexpr int HID_HOST_INT_DATA_EVT = 7;
static constexpr int HID_HOST_INT_CTRL_DATA = 8;
static constexpr int HID_HOST_INT_HANDSK_EVT = 9;
// SDP completes
static constexpr int HID_HOST_SDP_CMPL_EVT = 10;
// Write device action. can be SET/GET/DATA transaction.
static constexpr int HID_HOST_API_WRITE_DEV_EVT = 11;
static constexpr int HID_HOST_OPEN_CMPL_EVT = 12;
// Remove state machine
static constexpr int HID_HOST_REMOVE_STATE_MACHINE_EVT = 13;
// Connection/disconnection timeout
static constexpr int HID_HOST_CONNECTION_TIMEOUT_EVT = 14;
static constexpr int HID_HOST_DISCONNECTION_TIMEOUT_EVT = 15;
// L2cap event
static constexpr int HID_HOST_L2CAP_START_EVT = 100;
static constexpr int HID_HOST_L2CAP_CONNECT_REQ_EVT = 101;
static constexpr int HID_HOST_L2CAP_CONNECT_RSP_EVT = 102;
static constexpr int HID_HOST_L2CAP_CONFIG_REQ_EVT = 103;
static constexpr int HID_HOST_L2CAP_CONFIG_RSP_EVT = 104;
static constexpr int HID_HOST_L2CAP_DISCONNECT_REQ_EVT = 105;
static constexpr int HID_HOST_L2CAP_DISCONNECT_RSP_EVT = 106;
static constexpr int HID_HOST_L2CAP_DISCONNECT_ABNORMAL_EVT = 107;
static constexpr int HID_HOST_L2CAP_DATA_EVT = 108;
static constexpr int HID_HOST_L2CAP_REMOTE_BUSY_EVT = 109;
static constexpr int HID_HOST_L2CAP_SECURITY_RESULT_EVT = 110;
static constexpr int HID_HOST_L2CAP_IND_SECURITY_RESULT_EVT = 111;
static constexpr int HID_HOST_L2CAP_END_EVT = 112;
// Hogp event
static constexpr int HID_HOST_HOGP_START_EVT = 200;
static constexpr int HID_HOST_HOGP_CONNECTION_STATE_CHANGED_EVT = 201;
static constexpr int HID_HOST_HOGP_SERVICES_DISCOVERED_EVT = 202;
static constexpr int HID_HOST_HOGP_STOP_THREAD_EVT = 203;
static constexpr int HID_HOST_HOGP_END_EVT = 204;

static constexpr int HID_HOST_CONN_STATE_UNUSED = 0;
static constexpr int HID_HOST_CONN_STATE_CONNECTING_CTRL = 1;
static constexpr int HID_HOST_CONN_STATE_CONNECTING_INTR = 2;
static constexpr int HID_HOST_CONN_STATE_CONFIG = 3;
static constexpr int HID_HOST_CONN_STATE_CONNECTED = 4;
static constexpr int HID_HOST_CONN_STATE_DISCONNECTING = 5;
static constexpr int HID_HOST_CONN_STATE_SECURITY = 6;

static constexpr int HID_HOST_RESERVED_REPORT = 0;
static constexpr int HID_HOST_INPUT_REPORT = 1;
static constexpr int HID_HOST_OUTPUT_REPORT = 2;
static constexpr int HID_HOST_FEATURE_REPORT = 3;

struct HidHostL2capConnectionInfo {
    uint16_t lpsm = 0;
    uint16_t lcid = 0;
    int result = 0;
    uint8_t id = 0;
    L2capConfigInfo cfg;
    uint8_t reason = 0;
    uint8_t isBusy = 0;
};

struct SendHidData {
    uint8_t type = 0;
    uint8_t param = 0;
    uint16_t data = 0;
    uint8_t reportId = 0;
};

struct PnpInformation {
    uint16_t vendorId = 0;
    uint16_t productId = 0;
    uint16_t version = 0;
};

static constexpr uint16_t HID_HOST_INVALID = 0xffff;

struct HidInformation {
    uint16_t attrMask = 0;

    uint16_t supTimeout = 0;
    uint16_t ssrMaxLatency = HID_HOST_INVALID;
    uint16_t ssrMinTout = HID_HOST_INVALID;
    uint16_t relNum = 0;
    uint8_t ctryCode = 0;
    uint8_t subClass = 0;
    uint16_t hparsVer = 0;
    std::string serviceName = "";
    std::string serviceDescription = "";
    std::string providerName = "";
    uint16_t descLength = 0;
    std::unique_ptr<uint8_t[]> descInfo = nullptr;
};

struct HogpReport {
    uint8_t reportId = 0;
    uint8_t reportType = HID_HOST_RESERVED_REPORT;
    std::unique_ptr<Characteristic> character = nullptr;
    std::unique_ptr<Descriptor> config = nullptr;
};

static constexpr uint16_t SDP_ATTRIBUTE_VENDOR_ID = 0x0201;
static constexpr uint16_t SDP_ATTRIBUTE_PRODUCT_ID = 0x0202;
static constexpr uint16_t SDP_ATTRIBUTE_VERSION = 0x0203;

static constexpr uint16_t ATTR_ID_HID_DEVICE_RELNUM = 0x0200;
static constexpr uint16_t ATTR_ID_HID_PARSER_VERSION = 0x0201;
static constexpr uint16_t ATTR_ID_HID_DEVICE_SUBCLASS = 0x0202;
static constexpr uint16_t ATTR_ID_HID_COUNTRY_CODE = 0x0203;
static constexpr uint16_t ATTR_ID_HID_VIRTUAL_CABLE = 0x0204;
static constexpr uint16_t ATTR_ID_HID_RECONNECT_INITIATE = 0x0205;
static constexpr uint16_t ATTR_ID_HID_DESCRIPTOR_LIST = 0x0206;
static constexpr uint16_t ATTR_ID_HID_LANGUAGE_ID_BASE = 0x0207;
static constexpr uint16_t ATTR_ID_HID_SDP_DISABLE = 0x0208;
static constexpr uint16_t ATTR_ID_HID_BATTERY_POWER = 0x0209;
static constexpr uint16_t ATTR_ID_HID_REMOTE_WAKE = 0x020A;
static constexpr uint16_t ATTR_ID_HID_PROFILE_VERSION = 0x020B;
static constexpr uint16_t ATTR_ID_HID_LINK_SUPERVISION_TO = 0x020C;
static constexpr uint16_t ATTR_ID_HID_NORMALLY_CONNECTABLE = 0x020D;
static constexpr uint16_t ATTR_ID_HID_BOOT_DEVICE = 0x020E;
static constexpr uint16_t ATTR_ID_HID_SSR_HOST_MAX_LAT = 0x020F;
static constexpr uint16_t ATTR_ID_HID_SSR_HOST_MIN_TOUT = 0x0210;

static constexpr uint16_t HID_VIRTUAL_CABLE = 0x0001;
static constexpr uint16_t HID_NORMALLY_CONNECTABLE = 0x0002;
static constexpr uint16_t HID_RECONN_INIT = 0x0004;
static constexpr uint16_t HID_SDP_DISABLE = 0x0008;
static constexpr uint16_t HID_BATTERY_POWER = 0x0010;
static constexpr uint16_t HID_REMOTE_WAKE = 0x0020;
static constexpr uint16_t HID_SUP_TOUT_AVLBL = 0x0040;
static constexpr uint16_t HID_SSR_MAX_LATENCY = 0x0080;
static constexpr uint16_t HID_SSR_MIN_TOUT = 0x0100;

static constexpr uint8_t DP_TYPE_DES = 6;
static constexpr uint8_t SDP_DE_TYPE_STRING = 4;
static constexpr uint8_t HID_SDP_DESCRIPTOR_SIZE_BIT = 3;
static constexpr uint8_t SDP_SIZE_MASK = 0x07;
static constexpr uint8_t SDP_DE_SIZE_VAR_8 = 5;
static constexpr uint8_t SDP_DE_SIZE_VAR_16 = 6;
static constexpr uint8_t SDP_UINT8_LENGTH = 1;
static constexpr uint8_t SDP_UINT16_LENGTH = 2;

static constexpr uint8_t DESCRIPTOR_LIST_SDP_HEAD_LENGTH = 6;
static constexpr uint8_t DESCRIPTOR_LIST_SDP_OFFSET = 4;

static constexpr uint8_t ONE_BYTE_OFFSET = 8;

static constexpr int HID_HOST_SDP_SUCCESS = 0;
static constexpr int HID_HOST_SDP_FAILD = 1;

/* Define PSMs for HID host */
static constexpr uint16_t HID_CONTROL_PSM = 0x0011;
static constexpr uint16_t HID_INTERRUPT_PSM = 0x0013;

static constexpr uint16_t HID_HOST_MTU = 640;

static constexpr uint8_t HID_HOST_CONN_FLAGS_IS_ORIG = 0x01;
static constexpr uint8_t HID_HOST_CONN_FLAGS_HIS_CTRL_CFG_DONE = 0x02;
static constexpr uint8_t HID_HOST_CONN_FLAGS_MY_CTRL_CFG_DONE = 0x04;
static constexpr uint8_t HID_HOST_CONN_FLAGS_HIS_INTR_CFG_DONE = 0x08;
static constexpr uint8_t HID_HOST_CONN_FLAGS_MY_INTR_CFG_DONE = 0x10;
static constexpr uint8_t HID_HOST_CONN_FLAGS_ALL_CONFIGURED = 0x1E;

static constexpr uint8_t HID_HOST_PDU_HEADER_LENGTH = 1;

static constexpr uint16_t HID_HOST_UUID_SERVCLASS_PNP = 0x1200;
static constexpr uint16_t HID_HOST_UUID_SERVCLASS_HID = 0x1124;
static constexpr uint16_t HID_HOST_UUID_SERVCLASS_LE_HID = 0x1812;
static constexpr uint16_t HID_HOST_UUID_GATT_PNP_ID = 0x2A50;
static constexpr uint16_t HID_HOST_UUID_GATT_HID_INFORMATION = 0x2A4A;
static constexpr uint16_t HID_HOST_UUID_GATT_HID_REPORT_MAP = 0x2A4B;
static constexpr uint16_t HID_HOST_UUID_GATT_HID_REPORT = 0x2A4D;
static constexpr uint16_t HID_HOST_UUID_GATT_CLIENT_CHAR_CONFIG = 0x2902;
static constexpr uint16_t HID_HOST_UUID_GATT_REPORT_REFERENCE = 0x2908;

// For PTS(HOGP/RH/HGRF/BV-02-I and HOGP/RH/HGRF/BV-10-I)
static constexpr uint16_t HID_HOST_UUID_BATTERY_SERVICE = 0x180F;
static constexpr uint16_t HID_HOST_UUID_GATT_BATTERY_LEVEL = 0x2A19;
static constexpr uint16_t HID_HOST_UUID_GATT_EXT_RPT_REF = 0x2907;
static constexpr uint16_t HID_HOST_UUID_GATT_EXT_CFG_REF = 0x2902;

static constexpr int HID_HOST_PNP_ID_SIZE = 7;
static constexpr int HID_HOST_HID_INFORMATION_SIZE = 4;
static constexpr int HID_HOST_REPORT_REFERENCE_SIZE = 2;
static constexpr int HID_HOST_CLIENT_CHAR_CONFIG_SIZE = 2;

static constexpr uint8_t HID_HOST_CTRY_CODE_OFFSET = 2;

static constexpr int HOGP_GATT_THREAD_WAIT_TIMEOUT = 5;

inline static constexpr uint16_t HID_HOST_CLASSID_NUM = 1;
inline static constexpr int HID_HOST_ATTRIBUTE_LENGTH_UINT8 = 1;
inline static constexpr int HID_HOST_ATTRIBUTE_LENGTH_UINT16 = 2;

/* HID data types
*/
static constexpr uint8_t HID_HOST_DATA_TYPE_HANDSHAKE = 0;
static constexpr uint8_t HID_HOST_DATA_TYPE_CONTROL = 1;
static constexpr uint8_t HID_HOST_DATA_TYPE_GET_REPORT = 4;
static constexpr uint8_t HID_HOST_DATA_TYPE_SET_REPORT = 5;
static constexpr uint8_t HID_HOST_DATA_TYPE_GET_PROTOCOL = 6;
static constexpr uint8_t HID_HOST_DATA_TYPE_SET_PROTOCOL = 7;
static constexpr uint8_t HID_HOST_DATA_TYPE_GET_IDLE = 8;
static constexpr uint8_t HID_HOST_DATA_TYPE_SET_IDLE = 9;
static constexpr uint8_t HID_HOST_DATA_TYPE_DATA = 10;
static constexpr uint8_t HID_HOST_DATA_TYPE_DATAC = 11;

static constexpr uint8_t HID_HOST_HANDSHAKE_ERROR = 5;

/* HID shift operation */
static constexpr uint8_t HID_HOST_SHIFT_OPRATURN_4 = 4;
static constexpr uint8_t HID_HOST_SHIFT_OPRATURN_8 = 8;

static const char UHID_DEVICE_PATH[] = { "/dev/uhid" };
}  // namespace bluetooth
}  // namespace OHOS
#endif  // HID_HOST_DEFINES_H
