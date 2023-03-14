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

#ifndef A2DP_DEF_H
#define A2DP_DEF_H

#include <cstddef>
#include <cstdint>

#include "a2dp_codec/include/a2dp_codec_constant.h"
#include "avdtp.h"
#include "bt_def.h"
#include "btstack.h"

namespace OHOS {
namespace bluetooth {
/**
 * SBC Codec Specific Information Element
 */
#define A2DP_ROLE_SOURCE 0
#define A2DP_ROLE_SINK 1
#define A2DP_ROLE_INT 2
#define A2DP_ROLE_ACP 3

/* the timeout to wait for open req after setconfig for incoming connections */
#ifndef A2DP_SIGNALLING_TIMEOUT_MS
#define A2DP_SIGNALLING_TIMEOUT_MS (8 * 1000) /* 8 seconds */
#endif

/// Service class uuid for A2DP.
#define A2DP_SERVICE_CLASS_UUID (0x110A)

/// Number of items when add service class id list.
#define A2DP_SERVICE_CLASS_ID_LIST_NUMBER (0x0001)

/// Service class uuid for A2DP Sink.
#define A2DP_SINK_SERVICE_CLASS_UUID (0x110B)
/// Service class uuid for A2DP Source.
#define A2DP_SERVICE_CLASS_UUID (0x110A)
/// Service class uuid for AVDTP.
#define A2DP_PROTOCOL_UUID_AVDTP (0x0019)
/// Service class uuid for L2CAP.
#define A2DP_PROTOCOL_UUID_L2CAP (0x0100)
/// Number of items when add protocol descriptor.
#define A2DP_PROTOCOL_DESCRIPTOR_LIST_NUMBER (0x0002)
/// Number of items when add bluetooth profile descriptor list.
#define A2DP_BLUETOOTH_PROFILE_DESCRIPTOR_LIST_NUMBER (0x0001)
/// Profile uuid for A2DP.
#define A2DP_PROFILE_UUID (0x110D)
/// The Version of A2DP Profile is 1.3.
#define A2DP_PROFILE_REV_1_3 (0x0103)

/* Time to wait for signalling from ACP */
#ifndef A2DP_ACCEPT_SIGNALLING_TIMEOUT_MS
#define A2DP_ACCEPT_SIGNALLING_TIMEOUT_MS (2 * 1000) /* 2 seconds */
#endif

/* Time to wait for close transport channel from INT */
#ifndef A2DP_ACCEPT_CLOSING_SIGNALLING_TIMEOUT_MS
#define A2DP_ACCEPT_CLOSING_SIGNALLING_TIMEOUT_MS (3 * 1000) /* 3 seconds */
#endif

#define A2DP_MAX_AVDTP_MTU_SIZE 1024  // MTU=1005, AVDTP Header:12, L2CAP:4

#define A2DP_STREAM_HANDLES_NUMBER 1  // number of start stream

struct CallbackParameter {
    uint8_t role;
    bool srcRole;
    uint16_t handle;
};

struct CodecInfo {
    A2dpCodecIndex codecIndex;
    uint8_t codecInfo[A2DP_CODEC_SIZE];  // Codec codecInfo
};

enum ServiceStatus {
    STREAM_CONNECTING,
    STREAM_CONNECT,
    STREAM_DISCONNECTING,
    STREAM_DISCONNECT,
    STREAM_CONNECT_FAILED,
    STREAM_DISCONNECT_FAILED,
    STREAM_TIMEOUT
};

enum EventStream {
    EVT_SDP_DISC,
    EVT_SDP_CFM,
    EVT_CONNECT_REQ,
    EVT_CONNECT_IND,
    EVT_CONNECT_CFM,
    EVT_DISCOVER_REQ,
    EVT_DISCOVER_CFM,
    EVT_SETCONFIG_REQ,
    EVT_SETCONFIG_IND,
    EVT_SETCONFIG_CFM,
    EVT_GET_ALLCAP_REQ,
    EVT_GET_ALLCAP_IND,
    EVT_GET_ALLCAP_CFM,
    EVT_GET_CAP_REQ,
    EVT_GET_CAP_IND,
    EVT_GET_CAP_CFM,
    EVT_OPEN_REQ,
    EVT_OPEN_IND,
    EVT_OPEN_CFM,
    EVT_START_REQ,
    EVT_START_IND,
    EVT_START_CFM,
    EVT_GETCONFIG_REQ,
    EVT_GETCONFIG_IND,
    EVT_GETCONFIG_CFM,
    EVT_RECONFIG_REQ,
    EVT_RECONFIG_IND,
    EVT_RECONFIG_CFM,
    EVT_SUSPEND_REQ,
    EVT_SUSPEND_IND,
    EVT_SUSPEND_CFM,
    EVT_DISCONNECT_REQ,
    EVT_DISCONNECT_IND,
    EVT_DISCONNECT_CFM,
    EVT_CLOSE_REQ,
    EVT_CLOSE_IND,
    EVT_CLOSE_CFM,
    EVT_DELAY_REQ,
    EVT_DELAY_IND,
    EVT_DELAY_CFM,
    EVT_ABORT_REQ,
    EVT_ABORT_IND,
    EVT_ABORT_CFM,
    EVT_WRITE_CFM,
    EVT_TIME_OUT,
    EVT_CLOSE_TRANS_IND
};

struct StreamInfo {
    BtAddr addr;
    uint16_t handle;
    uint8_t acpSeid;
    uint8_t label;
    uint8_t errCode;
};

struct ConfigureStream {
    BtAddr addr;
    uint8_t intSeid;
    uint8_t acpSeid;
    AvdtSepConfig cfg;
};

struct ConfigrueRsp {
    AvdtCatetory category;
    uint16_t handle;
    uint16_t scbIndex;
    uint8_t label;
    BtAddr addr;
    uint8_t codecInfo[AVDT_CODEC_SIZE];
    uint8_t role;
};

struct GetConfigrueRsp {
    AvdtCatetory category;
    uint16_t handle;
    uint16_t scbIndex;
    uint8_t label;
    BtAddr addr;
    AvdtSepConfig cfg;
};

struct SepDetail {
    BtAddr addr;
    AvdtStreamConfig cfg;
};

struct ConnectReq {
    BtAddr addr;
    uint8_t errCode;
};

struct DelayReportInfo {
    BtAddr addr;
    uint16_t handle;
    uint16_t delayValue;
};

union A2dpAvdtMsgData {
    StreamInfo stream;
    ConnectReq connectInfo;
    ConfigureStream configStream;
    ConfigrueRsp configRsp;
    GetConfigrueRsp getConfigureRsp;
    DelayReportInfo delayReportInfo;
    AvdtSepInfo discoverData[AVDT_NUM_SEPS];
    SepDetail sepDetail;
    AvdtCtrlData msg;
};

struct A2dpAvdtMsg {
    uint16_t handle;
    uint8_t role;
    uint8_t event;
    A2dpAvdtMsgData a2dpMsg;
};

enum A2dpEvent {
    A2DP_INVALID_EVT = 0,
    A2DP_CONNECT_EVT,
    A2DP_DISCONNECT_EVT,
    A2DP_AVDTP_EVT,
    A2DP_SDP_EVT,
    A2DP_TIMEOUT_EVT,
};

enum A2dpConnectNum {
    A2DP_CONNECT_NUM_MAX = 6,
    A2DP_CONNECT_NUM_UN_MAX,
};

enum A2dpMessage {
    A2DP_MSG_CONNECT,
    A2DP_MSG_DISCONNECT,
    A2DP_MSG_CONNECT_TIMEOUT,
    A2DP_MSG_CONNECT_FORBIDDEN,
    A2DP_MSG_PROFILE_CONNECTED,
    A2DP_MSG_PROFILE_CONNECTING,
    A2DP_MSG_PROFILE_DISCONNECTED,
    A2DP_MSG_PROFILE_DISCONNECTING,
    A2DP_MSG_PROFILE_AUDIO_PLAY_START,
    A2DP_MSG_PROFILE_AUDIO_PLAY_SUSPEND,
    A2DP_MSG_PROFILE_AUDIO_PLAY_STOP,
    A2DP_MSG_PROFILE_CODEC_CHANGE,
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // A2DP_DEF_H