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

#ifndef HFP_HF_DEFINES_H
#define HFP_HF_DEFINES_H

#include <cstdint>
#include <string>

#include "log.h"

namespace OHOS {
namespace bluetooth {
#define HFP_HF_RETURN_IF_FAIL(ret)                                \
    do {                                                          \
        if ((ret) == BT_ALREADY) {                                  \
            LOG_WARN("[HFP HF]%{public}s():ret[%{public}d]", __FUNCTION__, ret);  \
        }                                                         \
        if (((ret) != BT_NO_ERROR) && ((ret) != BT_ALREADY)) {        \
            LOG_ERROR("[HFP HF]%{public}s():ret[%{public}d]", __FUNCTION__, ret); \
            return ret;                                           \
        }                                                         \
    } while (0)

static constexpr int HFP_HF_COMMAND_MTU = 512;
static constexpr int HFP_HF_OUTGOING_CALL_ID = -1;

// SDP attribute defines
static constexpr uint16_t HFP_HF_SDP_ATTRIBUTE_DATA_STORES_OR_NETWORK = 0x0301;
static constexpr uint16_t HFP_HF_SDP_ATTRIBUTE_SUPPORTED_FEATURES = 0x0311;

// HF service name string
static const std::string HFP_HF_SERVER_SERVICE_NAME = "Handsfree Unit";

static const std::string HSP_HS_SERVER_SERVICE_NAME = "Headset";

static const std::string HSP_HS_STATE_SECTION_NAME = "HfpHfService";
static const std::string HSP_HS_STATE_PROPERY_NAME = "HspHsState";

// Service class UUID
static constexpr uint16_t HFP_HF_UUID_SERVCLASS_HFP_HF = 0X111E;static constexpr uint16_t HFP_HF_UUID_SERVCLASS_HFP_AG = 0X111F;
static constexpr uint16_t HFP_HF_UUID_SERVCLASS_GENERIC_AUDIO = 0X1203;
static constexpr uint16_t HSP_UUID_SERVCLASS = 0x1108;
static constexpr uint16_t HFP_HF_UUID_SERVCLASS_HSP_HS = 0x1131;
static constexpr uint16_t HFP_HF_UUID_SERVCLASS_HSP_AG = 0x1112;

// HFP version
static constexpr uint16_t HFP_HF_HFP_VERSION_1_1 = 0x0101;
static constexpr uint16_t HFP_HF_HFP_VERSION_1_5 = 0x0105;
static constexpr uint16_t HFP_HF_HFP_VERSION_1_6 = 0x0106;
static constexpr uint16_t HFP_HF_HFP_VERSION_1_7 = 0x0107;

// HSP version
static constexpr uint16_t HFP_HF_HSP_VERSION_1_2 = 0x0102;

// HF features masks: using 16 ~ 0 bits
static constexpr uint16_t HFP_HF_AG_FEATURES_NONE = 0x0000;
static constexpr uint16_t HFP_HF_AG_FEATURES_SUPPORT_WBS = 0x0020;
static constexpr uint16_t HFP_HF_AG_FEATURES_BRSF_MASK = 0x001F;

// Hf feature masks
static constexpr uint32_t HFP_HF_FEATURES_ECNR = 0x00000001;                   // Echo cancellation/noise reduction
static constexpr uint32_t HFP_HF_FEATURES_THREE_WAY = 0x00000002;              // Three-way calling
static constexpr uint32_t HFP_HF_FEATURES_CALLING_ID = 0x00000004;             // CLI presentation capability
static constexpr uint32_t HFP_HF_FEATURES_VOICE_RECOGNITION = 0x00000008;      // Voice recognition
static constexpr uint32_t HFP_HF_FEATURES_REMOTE_VOLUME_CONTROL = 0x00000010;  // Remote volume control
static constexpr uint32_t HFP_HF_FEATURES_ENHANCED_CALL_STATUS = 0x00000020;   // Enhanced Call Status
static constexpr uint32_t HFP_HF_FEATURES_ENHANCED_CALL_CONTROL = 0x00000040;  // Enhanced Call Control
static constexpr uint32_t HFP_HF_FEATURES_CODEC_NEGOTIATION = 0x00000080;      // Codec Negotiation
static constexpr uint32_t HFP_HF_FEATURES_HF_INDICATORS = 0x00000100;          // HF indicators
static constexpr uint32_t HFP_HF_FEATURES_ESCO = 0x00000200;                   // eSCO S4 Settings Supported

// AG feature masks
static constexpr uint32_t HFP_HF_AG_FEATURES_THREE_WAY = 0x00000001;             // Three-way calling
static constexpr uint32_t HFP_HF_AG_FEATURES_ECNR = 0x00000002;                  // Echo cancellation/noise reduction
static constexpr uint32_t HFP_HF_AG_FEATURES_VOICE_RECOGNITION = 0x00000004;     // Voice recognition
static constexpr uint32_t HFP_HF_AG_FEATURES_IN_BAND_RING = 0x00000008;          // In-band ring tone
static constexpr uint32_t HFP_HF_AG_FEATURES_VOICE_TAG = 0x00000010;             // Attach a phone number to a voice tag
static constexpr uint32_t HFP_HF_AG_FEATURES_REJECT_CALL = 0x00000020;           // Ability to reject incoming call
static constexpr uint32_t HFP_HF_AG_FEATURES_ENHANCED_CALL_STATUS = 0x00000040;  // Enhanced Call Status
static constexpr uint32_t HFP_HF_AG_FEATURES_ENHANCED_CALL_CONTROL = 0x00000080;  // Enhanced Call Control
static constexpr uint32_t HFP_HF_AG_FEATURES_EXTEND_ERROR_CODE = 0x00000100;      // Extended error codes
static constexpr uint32_t HFP_HF_AG_FEATURES_CODEC_NEGOTIATION = 0x00000200;      // Codec Negotiation
static constexpr uint32_t HFP_HF_AG_FEATURES_HF_INDICATORS = 0x00000400;          // HF Indicators
static constexpr uint32_t HFP_HF_AG_FEATURES_SUPPORT_ESCO = 0x00000800;           // eSCO S4 (and T2) setting supported

// Default HF features
static constexpr uint32_t HFP_HF_FEATURES_DEFAULT = HFP_HF_FEATURES_ECNR |
                                                    HFP_HF_FEATURES_THREE_WAY |
                                                    HFP_HF_FEATURES_CALLING_ID |
                                                    HFP_HF_FEATURES_VOICE_RECOGNITION |
                                                    HFP_HF_FEATURES_REMOTE_VOLUME_CONTROL |
                                                    HFP_HF_FEATURES_ENHANCED_CALL_STATUS |
                                                    HFP_HF_FEATURES_HF_INDICATORS |
                                                    HFP_HF_FEATURES_ENHANCED_CALL_CONTROL;

// Specific SDP HF features
static constexpr uint32_t HFP_HF_FEATURES_SDP_SPEC = HFP_HF_FEATURES_ECNR |
                                                     HFP_HF_FEATURES_THREE_WAY |
                                                     HFP_HF_FEATURES_CALLING_ID |
                                                     HFP_HF_FEATURES_VOICE_RECOGNITION |
                                                     HFP_HF_FEATURES_REMOTE_VOLUME_CONTROL;

typedef struct {
    uint8_t remoteServerChannelNumber {0};
    uint8_t remoteNetwork {0};
    uint16_t remoteVersion {0};
    uint16_t remoteFeatures {0};
    int remoteCodec {0};
} HfpHfRemoteSdpInfo;

typedef struct {
    uint16_t index {0};
    uint16_t dir {0};
    uint16_t status {0};
    uint16_t mode {0};
    uint16_t mprty {0};
    std::string number {""};
    uint16_t type {0};
} HfpHfCurrentCallData;

// Hf
enum HfpHfCodecsType { HFP_HF_CODEC_NONE, HFP_HF_CODEC_CVSD, HFP_HF_CODEC_MSBC };

enum HfpHfVolumeType { HFP_HF_VOLUME_TYPE_SPK, HFP_HF_VOLUME_TYPE_MIC };

enum HfpHfNetworkValue { HFP_HF_NETWORK_VALUE_NONE, HFP_HF_NETWORK_VALUE_HAVE };

enum HfpHfRoleType { HFP_HF_INITIATOR, HFP_HF_ACCEPTOR };

enum HfpHfIndicatorType { HFP_HF_INDICATOR_DRIVER_SAFETY = 1, HFP_HF_INDICATOR_BATTERY_LEVEL = 2 };

enum HfpHfAgIndicatorType {
    HFP_HF_AG_INDICATOR_CALL = 1,
    HFP_HF_AG_INDICATOR_CALLSETUP,
    HFP_HF_AG_INDICATOR_REGISTRATION_STATUS,
    HFP_HF_AG_INDICATOR_SIGNAL_STRENGTH,
    HFP_HF_AG_INDICATOR_ROAMING_STATE,
    HFP_HF_AG_INDICATOR_BATTERY_LEVEL,
    HFP_HF_AG_INDICATOR_CALLHELD
};

enum HfpHfConnectState {
    HFP_HF_STATE_DISCONNECTED = 0,
    HFP_HF_STATE_CONNECTING,
    HFP_HF_STATE_DISCONNECTING,
    HFP_HF_STATE_CONNECTED
};

enum HfpHfAudioState {
    HFP_HF_AUDIO_STATE_DISCONNECTED = HFP_HF_STATE_CONNECTED,
    HFP_HF_AUDIO_STATE_CONNECTING,
    HFP_HF_AUDIO_STATE_DISCONNECTING,
    HFP_HF_AUDIO_STATE_CONNECTED
};

enum {
    HFP_HF_INVALID_EVT = 0,

    // service start/stop
    HFP_HF_SERVICE_STARTUP_EVT = 1,
    HFP_HF_SERVICE_SHUTDOWN_EVT = 2,

    // service connect events
    HFP_HF_CONNECT_EVT = 10,
    HFP_HF_DISCONNECT_EVT = 11,
    HFP_HF_CONNECT_AUDIO_EVT = 20,
    HFP_HF_DISCONNECT_AUDIO_EVT = 21,
    HFP_HF_RETRY_CONNECT_AUDIO_EVT = 22,
    HFP_HF_REMOVE_STATE_MACHINE_EVT = 30,
    HFP_HF_INTERACTIVE_EVT,

    // service timeout events
    HFP_HF_CONNECTION_TIMEOUT_EVT,
    HFP_HF_DISCONNECT_TIMEOUT_EVT,
    HFP_HF_CONNECT_AUDIO_TIMEOUT_EVT,
    HFP_HF_DISCONNECT_AUDIO_TIMEOUT_EVT,

    HFP_HF_SEND_DTMF_EVT,
    HFP_HF_ACCEPT_CALL_EVT,
    HFP_HF_HOLD_CALL_EVT,
    HFP_HF_REJECT_CALL_EVT,
    HFP_HF_SEND_KEY_PRESSED,
    HFP_HF_HANDLE_INCOMING_CALL_EVT,
    HFP_HF_HANDLE_MULTI_CALL_EVT,
    HFP_HF_DIAL_LAST_NUMBER,
    HFP_HF_DIAL_MEMORY,
    HFP_HF_SEND_VOICE_TAG,
    HFP_HF_FINISH_CALL_EVT,
    HFP_HF_DIAL_CALL_EVT,
    HFP_HF_OPEN_VOICE_RECOGNITION_EVT,
    HFP_HF_CLOSE_VOICE_RECOGNITION_EVT,
    HFP_HF_SET_VOLUME_EVT,
    HFP_HF_BATTERY_LEVEL_CHANGED_EVT,
    HFP_HF_ENHANCED_DRIVER_SAFETY_CHANGED_EVT,

    // stack sdp events
    HFP_HF_SDP_DISCOVERY_RESULT_SUCCESS = 98,
    HFP_HF_SDP_DISCOVERY_RESULT_FAIL = 99,

    // stack rfcomm events
    HFP_HF_CONNECT_REQUEST_EVT = 100,
    HFP_HF_CONNECTED_EVT = 110,
    HFP_HF_DISCONNECTED_EVT = 111,
    HFP_HF_CONNECT_FAILED_EVT = 112,
    HFP_HF_DISCONNECT_FAILED_EVT = 113,
    HFP_HF_DATA_AVAILABLE_EVT = 114,

    // service level connection established event
    HFP_HF_SETUP_CODEC_CVSD = 198,
    HFP_HF_SLC_ESTABLISHED_EVT = 199,

    // stack audio events
    HFP_HF_AUDIO_CONNECT_REQUEST_EVT = 201,
    HFP_HF_AUDIO_CONNECTING_EVT = 202,
    HFP_HF_AUDIO_DISCONNECTING_EVT = 203,
    HFP_HF_AUDIO_CONNECTED_EVT = 220,
    HFP_HF_AUDIO_CONNECT_FAILED_EVT = 221,
    HFP_HF_AUDIO_DISCONNECTED_EVT,
    HFP_HF_AUDIO_DISCONNECT_FAILED_EVT,

    HFP_HF_SEND_AT_COMMAND_EVT
};

enum {
    HFP_HF_TYPE_NONE,
    HFP_HF_TYPE_NETWORK_STATE,
    HFP_HF_TYPE_NETWORK_ROAM,
    HFP_HF_TYPE_NETWORK_SIGNAL,
    HFP_HF_TYPE_BATTERY_LEVEL,
    HFP_HF_TYPE_CURRENT_OPERATOR,
    HFP_HF_TYPE_CALL_STATE,
    HFP_HF_TYPE_CALL_SETUP_STATE,
    HFP_HF_TYPE_CALL_HELD_STATE,
    HFP_HF_TYPE_HOLD_RESULT,
    HFP_HF_TYPE_CALLING_LINE_IDENTIFICATION,
    HFP_HF_TYPE_CALL_WAITING,
    HFP_HF_TYPE_CURRENT_CALLS,
    HFP_HF_TYPE_SET_VOLUME,
    HFP_HF_TYPE_AT_CMD_RESULT,
    HFP_HF_TYPE_SUBSCRIBER_NUMBER,
    HFP_HF_TYPE_INBAND_RING,
    HFP_HF_TYPE_LAST_NUMBER,
    HFP_HF_TYPE_RING_ALERT,
    HFP_HF_TYPE_UNKNOWN,
    HFP_HF_TYPE_QUERY_CURRENT_CALLS_DONE,
    HFP_HF_TYPE_VOICE_RECOGNITION_CHANGED,
    HFP_HF_TYPE_OPEN_VR_RESULT,
    HFP_HF_TYPE_CLOSE_VR_RESULT
};

enum {
    HFP_HF_AT_RESULT_OK = 0,
    HFP_HF_AT_RESULT_ERROR,
    HFP_HF_AT_RESULT_NO_CARRIER,
    HFP_HF_AT_RESULT_BUSY,
    HFP_HF_AT_RESULT_NO_ANSWER,
    HFP_HF_AT_RESULT_DELAYED,
    HFP_HF_AT_RESULT_BLOCKLISTED,
    HFP_HF_AT_RESULT_CME,
};

enum { HFP_HF_NETWORK_STATE_NOT_AVAILABLE, HFP_HF_NETWORK_STATE_AVAILABLE };

enum { HFP_HF_SUCCESS, HFP_HF_FAILURE };

enum HfpHfAcceptCallAction {
    HFP_HF_ACCEPT_CALL_ACTION_NONE = 0,
    HFP_HF_ACCEPT_CALL_ACTION_HOLD = 1,
    HFP_HF_ACCEPT_CALL_ACTION_FINISH = 2
};

enum class HfpHfHandleIncomingCalAction {
    HFP_HF_HOLD_INCOMING_ACTION = 0,
    HFP_HF_ACCEPT_HOLD_ACTION = 1,
    HFP_HF_REJECT_HOLD_ACTION = 2
};

enum HfpHfChldAction {
    HFP_HF_CHLD_ACTION_0 = 0, /* Releases all held calls. */
    HFP_HF_CHLD_ACTION_1 = 1, /* Releases all active calls and accepts the other call.
                                 Releases specified active call only (<idx>). */
    HFP_HF_CHLD_ACTION_2 = 2, /* Places all active calls on hold and accepts the other call.
                                 Request private consultation mode with specified call (<idx>). */
    HFP_HF_CHLD_ACTION_3 = 3, /* Adds a held call to the conversation. */
    HFP_HF_CHLD_ACTION_4 = 4  /* Explicit Call Transfer. */
};

enum HfpHfCallState {
    HFP_HF_CALL_STATE_ACTIVE = 0,
    HFP_HF_CALL_STATE_HELD,
    HFP_HF_CALL_STATE_DIALING,
    HFP_HF_CALL_STATE_ALERTING,
    HFP_HF_CALL_STATE_INCOMING,
    HFP_HF_CALL_STATE_WAITING,
    HFP_HF_CALL_STATE_RESPONSE_HELD,
    HFP_HF_CALL_STATE_FINISHED
};

enum HfpHfMptyType { HFP_HF_MPTY_TYPE_SINGLE = 0, HFP_HF_MPTY_TYPE_MULTI };

enum HfpHfDirectionType { HFP_HF_DIRECTION_TYPE_OUTGOING = 0, HFP_HF_DIRECTION_TYPE_INCOMING };

enum { HFP_HF_VR_STATE_CLOSED, HFP_HF_VR_STATE_OPENED };

constexpr int HFP_HF_HFAG_FOUND = 1;
constexpr int HFP_HF_HSAG_FOUND = 2;
constexpr int HFP_HF_HFAG_HSAG_FOUND = 3;

constexpr int HSP_HS_STATE_BOTH = 0X01;
constexpr int HSP_HS_STATE_HSP = 0X02;
constexpr int HSP_HS_STATE_NONE = 0X03;
}  // namespace bluetooth
}  // namespace OHOS
#endif // HFP_HF_DEFINES_H