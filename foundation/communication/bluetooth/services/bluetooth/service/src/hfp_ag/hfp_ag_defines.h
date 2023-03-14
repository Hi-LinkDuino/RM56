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

#ifndef HFP_AG_DEFINES_H
#define HFP_AG_DEFINES_H

#include <cstdint>
#include <string>

#include "log.h"

namespace OHOS {
namespace bluetooth {
#define HFP_AG_RETURN_IF_FAIL(ret)                                \
    do {                                                          \
        if (ret == BT_ALREADY) {                                  \
            LOG_WARN("[HFP AG]%{public}s():ret[%{public}d]", __FUNCTION__, ret);  \
        }                                                         \
        if ((ret != BT_NO_ERROR) && (ret != BT_ALREADY)) {        \
            LOG_ERROR("[HFP AG]%{public}s():ret[%{public}d]", __FUNCTION__, ret); \
            return ret;                                           \
        }                                                         \
    } while (0)

constexpr int HFP_AG_COMMAND_MTU = 512;

// SDP attribute defines
static constexpr uint16_t HFP_AG_SDP_ATTRIBUTE_DATA_STORES_OR_NETWORK = 0x0301;
static constexpr uint16_t HFP_AG_SDP_ATTRIBUTE_SUPPORTED_FEATURES = 0x0311;
static constexpr uint16_t HSP_AG_SDP_ATTRIBUTE_REMOTE_AUDIO_VOLUME_CONTROL = 0x0302;

// AG service name string
static const std::string HFP_AG_SERVER_SERVICE_NAME = "Handsfree Audio Gateway";

// AG service name string
static const std::string HSP_AG_SERVER_SERVICE_NAME = "Headset Audio Gateway";
// HSP support name string
static const std::string HSP_AG_STATE_SECTION_NAME = "HfpAgService";
static const std::string HSP_AG_STATE_PROPERY_NAME = "HspAgState";


// Service class UUID
static constexpr uint16_t HFP_AG_UUID_SERVCLASS_HFP_HF = 0X111E;
static constexpr uint16_t HFP_AG_UUID_SERVCLASS_HFP_AG = 0X111F;
static constexpr uint16_t HFP_AG_UUID_SERVCLASS_GENERIC_AUDIO = 0X1203;
static constexpr uint16_t HSP_UUID_SERVCLASS = 0X1108;
static constexpr uint16_t HSP_HS_UUID_SERVCLASS = 0X1131;
static constexpr uint16_t HSP_AG_UUID_SERVCLASS = 0X1112;

// HFP version
static constexpr uint16_t HFP_AG_HFP_VERSION_1_1 = 0x0101;
static constexpr uint16_t HFP_AG_HFP_VERSION_1_5 = 0x0105;
static constexpr uint16_t HFP_AG_HFP_VERSION_1_6 = 0x0106;
static constexpr uint16_t HFP_AG_HFP_VERSION_1_7 = 0x0107;
static constexpr uint16_t HSP_AG_HSP_VERSION_1_2 = 0x0102;

// Feature mask for HFP 1.6 (and below)
static constexpr uint32_t HFP_AG_1_6_FEATURES_MASK = 0x000003FF;

// HF features masks: using 16 ~ 0 bits
static constexpr uint16_t HFP_AG_HF_FEATURES_NONE = 0x0000;
static constexpr uint16_t HFP_AG_HF_FEATURES_SUPPORT_WBS = 0x0020;
static constexpr uint16_t HFP_AG_HF_FEATURES_BRSF_MASK = 0x001F;

// AG feature masks
static constexpr uint32_t HFP_AG_FEATURES_THREE_WAY = 0x00000001;              // Three-way calling
static constexpr uint32_t HFP_AG_FEATURES_ECNR = 0x00000002;                   // Echo cancellation/noise reduction
static constexpr uint32_t HFP_AG_FEATURES_VOICE_RECOGNITION = 0x00000004;      // Voice recognition
static constexpr uint32_t HFP_AG_FEATURES_IN_BAND_RING = 0x00000008;           // In-band ring tone
static constexpr uint32_t HFP_AG_FEATURES_VOICE_TAG = 0x00000010;              // Attach a phone number to a voice tag
static constexpr uint32_t HFP_AG_FEATURES_REJECT_CALL = 0x00000020;            // Ability to reject incoming call
static constexpr uint32_t HFP_AG_FEATURES_ENHANCED_CALL_STATUS = 0x00000040;   // Enhanced Call Status
static constexpr uint32_t HFP_AG_FEATURES_ENHANCED_CALL_CONTROL = 0x00000080;  // Enhanced Call Control
static constexpr uint32_t HFP_AG_FEATURES_EXTEND_ERROR_CODE = 0x00000100;      // Extended error codes
static constexpr uint32_t HFP_AG_FEATURES_CODEC_NEGOTIATION = 0x00000200;      // Codec Negotiation
static constexpr uint32_t HFP_AG_FEATURES_HF_INDICATORS = 0x00000400;          // HF Indicators
static constexpr uint32_t HFP_AG_FEATURES_SUPPORT_ESCO = 0x00000800;           // eSCO S4 (and T2) setting supported

// Hf feature masks
static constexpr uint32_t HFP_AG_HF_FEATURES_ECNR = 0x00000001;                   // Echo cancellation/noise reduction
static constexpr uint32_t HFP_AG_HF_FEATURES_THREE_WAY = 0x00000002;              // Three-way calling
static constexpr uint32_t HFP_AG_HF_FEATURES_CALLING_ID = 0x00000004;             // CLI presentation capability
static constexpr uint32_t HFP_AG_HF_FEATURES_VOICE_RECOGNITION = 0x00000008;      // Voice recognition
static constexpr uint32_t HFP_AG_HF_FEATURES_REMOTE_VOLUME_CONTROL = 0x00000010;  // Remote volume control
static constexpr uint32_t HFP_AG_HF_FEATURES_ENHANCED_CALL_STATUS = 0x00000020;   // Enhanced Call Status
static constexpr uint32_t HFP_AG_HF_FEATURES_ENHANCED_CALL_CONTROL = 0x00000040;  // Enhanced Call Control
static constexpr uint32_t HFP_AG_HF_FEATURES_CODEC_NEGOTIATION = 0x00000080;      // Codec Negotiation
static constexpr uint32_t HFP_AG_HF_FEATURES_HF_INDICATORS = 0x00000100;          // HF indicators
static constexpr uint32_t HFP_AG_HF_FEATURES_ESCO = 0x00000200;                   // eSCO S4 Settings Supported

// AG Proprietary features: using 31 ~ 16 bits
static constexpr uint32_t HFP_AG_FEATURES_SUPPORT_BTRH = 0x00010000;     // CCAP incoming call hold
static constexpr uint32_t HFP_AG_FEATURES_PASS_UNKNOWN_AT = 0x00020000;  // Pass unknown AT commands to app
static constexpr uint32_t HFP_AG_FEATURES_NO_SCO_CONTROL = 0x00040000;   // No SCO control performed by AG
static constexpr uint32_t HFP_AG_FEATURES_NO_ESCO = 0x00080000;          // Do not allow or use eSCO
static constexpr uint32_t HFP_AG_FEATURES_VOIP_CALL = 0x00100000;        // VoIP call

// Default AG features
static constexpr uint32_t HFP_AG_FEATURES_DEFAULT = HFP_AG_FEATURES_THREE_WAY |
                                                    HFP_AG_FEATURES_ECNR |
                                                    HFP_AG_FEATURES_VOICE_RECOGNITION |
                                                    HFP_AG_FEATURES_IN_BAND_RING |
                                                    HFP_AG_FEATURES_REJECT_CALL |
                                                    HFP_AG_FEATURES_ENHANCED_CALL_STATUS |
                                                    HFP_AG_FEATURES_EXTEND_ERROR_CODE |
                                                    HFP_AG_FEATURES_HF_INDICATORS |
                                                    HFP_AG_FEATURES_PASS_UNKNOWN_AT;

// Specific SDP AG features
static constexpr uint32_t HFP_AG_FEATURES_SDP_SPEC = HFP_AG_FEATURES_THREE_WAY |
                                                     HFP_AG_FEATURES_ECNR |
                                                     HFP_AG_FEATURES_VOICE_RECOGNITION |
                                                     HFP_AG_FEATURES_IN_BAND_RING |
                                                     HFP_AG_FEATURES_VOICE_TAG;

typedef struct {
    uint8_t remoteServerChannelNumber {0};
    uint16_t remoteVersion {0};
    uint16_t remoteFeatures {0};
    int remoteCodec {0};
} HfpAgRemoteSdpInfo;

// Ag
enum HfpAgCodecsType { HFP_AG_CODEC_NONE = 0x00, HFP_AG_CODEC_CVSD = 0x01, HFP_AG_CODEC_MSBC = 0x02 };

enum HfpAgEsco { HFP_AG_ESCO_S4, HFP_AG_ESCO_S1 };

enum HfpAGMsbc { HFP_AG_MSBC_T2, HFP_AG_MSBC_T1 };

enum HfpAgRoleType { HFP_AG_INITIATOR, HFP_AG_ACCEPTOR };

enum HfpAgHfIndicatorType { HFP_AG_HF_INDICATOR_DRIVER_SAFETY = 1, HFP_AG_HF_INDICATOR_BATTERY_LEVEL = 2 };

enum HfpAgCnumServiceType { HFP_AG_CNUM_SERVICE_VOICE = 4, HFP_AG_CNUM_SERVICE_FAX = 5 };

enum HfpAgCallNumberType { HFP_AG_CALL_NUMBER_UNKNOW = 0x81, HFP_AG_CALL_NUMBER_INTERNATIONAL = 0x91 };

enum HfpAgConnectState {
    HFP_AG_STATE_DISCONNECTED = 0,
    HFP_AG_STATE_CONNECTING,
    HFP_AG_STATE_DISCONNECTING,
    HFP_AG_STATE_CONNECTED
};

enum HfpAgAudioState {
    HFP_AG_AUDIO_STATE_DISCONNECTED = HFP_AG_STATE_CONNECTED,
    HFP_AG_AUDIO_STATE_CONNECTING,
    HFP_AG_AUDIO_STATE_DISCONNECTING,
    HFP_AG_AUDIO_STATE_CONNECTED
};

// Ag
enum HfpAgResultType {
    HFP_AG_RESULT_OK = 0,
    HFP_AG_RESULT_CONNECT = 1,
    HFP_AG_RESULT_RING = 2,
    HFP_AG_RESULT_NO_CARRIER = 3,
    HFP_AG_RESULT_ERROR = 4,
    HFP_AG_RESULT_NO_DIALTONE = 6,
    HFP_AG_RESULT_BUSY = 7,
    HFP_AG_RESULT_NO_ANSWER = 8,
    HFP_AG_RESULT_DELAYED = 9,
    HFP_AG_RESULT_BLOCKLISTED = 10,
    HFP_AG_RESULT_CME_ERROR = 11
};

// Ag
enum HfpAgExtError {
    HFP_AG_ERROR_AG_FAILURE = 0,
    HFP_AG_ERROR_NO_CONNECTION_TO_PHONE = 1,
    HFP_AG_ERROR_OPERATION_NOT_ALLOWED = 3,
    HFP_AG_ERROR_OPERATION_NOT_SUPPORTED = 4,
    HFP_AG_ERROR_PH_SIM_PIN_REQUIRED = 5,
    HFP_AG_ERROR_SIM_NOT_INSERTED = 10,
    HFP_AG_ERROR_SIM_PIN_REQUIRED = 11,
    HFP_AG_ERROR_SIM_PUK_REQUIRED = 12,
    HFP_AG_ERROR_SIM_FAILURE = 13,
    HFP_AG_ERROR_SIM_BUSY = 14,
    HFP_AG_ERROR_INCORRECT_PASSWORD = 16,
    HFP_AG_ERROR_SIM_PIN2_REQUIRED = 17,
    HFP_AG_ERROR_SIM_PUK2_REQUIRED = 18,
    HFP_AG_ERROR_MEMORY_FULL = 20,
    HFP_AG_ERROR_INVALID_INDEX = 21,
    HFP_AG_ERROR_MEMORY_FAILURE = 23,
    HFP_AG_ERROR_TEXT_STRING_TOO_LONG = 24,
    HFP_AG_ERROR_INVALID_CHARS_IN_TEXT_STRING = 25,
    HFP_AG_ERROR_DIAL_STRING_TO_LONG = 26,
    HFP_AG_ERROR_INVALID_CHARS_IN_DIAL_STRING = 27,
    HFP_AG_ERROR_NO_NETWORK_SERVICE = 30,
    HFP_AG_ERROR_NETWORK_TIMEOUT = 31,
    HFP_AG_ERROR_NETWORK_NOT_ALLOWED = 32
};

enum {
    HFP_AG_OK_RES = 0,
    HFP_AG_ERROR_RES,
    HFP_AG_RING_RES,
    HFP_AG_CLIP_RES,
    HFP_AG_BRSF_RES,
    HFP_AG_CMEE_RES,
    HFP_AG_BCS_RES,
    HFP_AG_SPK_RES,             // Handfree Uint speaker volume
    HFP_AG_MIC_RES,             // Handfree Uint microphone gain
    HFP_AG_INBAND_RING_RES,     // Handsfree Uint in-band ring tone
    HFP_AG_CIND_RES,            // Indicator response for AT+CIND
    HFP_AG_IND_RES,             // Audio Gateway's indicator value
    HFP_AG_BVRA_RES,            // Voice recognition control
    HFP_AG_CNUM_RES,            // Subscriber number information
    HFP_AG_CLCC_RES,            // List of calls*/
    HFP_AG_COPS_RES,            // Network operator name
    HFP_AG_IN_CALL_RES,         // Incoming call
    HFP_AG_IN_CALL_HELD_RES,    // Incoming call held
    HFP_AG_IN_CALL_ANS_RES,     // Incoming phone call answered
    HFP_AG_OUT_CALL_DIAL_RES,   // Outgoing phone call dialing
    HFP_AG_OUT_CALL_ALERT_RES,  // Outgoing phone call alerting
    HFP_AG_OUT_CALL_CONN_RES,   // Outgoing phone call answered
    HFP_AG_CALL_WAIT_RES,       // Call waiting notification
    HFP_AG_CALL_CANCEL_RES,     // Call canceled
    HFP_AG_TERM_CALL_RES,       // Terminate call
    HFP_AG_UNKNOWN_AT_RES,      // Unknown AT command
    HFP_AG_MULTI_CALL_RES,      // SLC in three way call
    HFP_AG_BIND_RES             // HandsFree Unit indicator
};

// AG indicators
enum HfpAgIndicatorType {
    HFP_AG_INDICATOR_SERVICE = 1,      // service indicator
    HFP_AG_INDICATOR_CALL,             // call indicator
    HFP_AG_INDICATOR_CALLSETUP,        // callsetup indicator
    HFP_AG_INDICATOR_CALLHELD,         // callheld indicator
    HFP_AG_INDICATOR_SIGNAL_STRENGTH,  // signal strength indicator
    HFP_AG_INDICATOR_ROAMING_STATE,    // roaming indicator
    HFP_AG_INDICATOR_BATTERY_LEVEL,    // battery indicator
};

// call indicator
enum HfpAgCallStatus {
    HFP_AG_CALL_INACTIVE = 0,  // call inactive
    HFP_AG_CALL_ACTIVE         // call active
};

// response hold state
enum HfpAgResponseHoldStatus {
    HFP_AG_RESPONSE_HOLD = 0,
    HFP_AG_RESPONSE_HOLD_ACCEPT,
    HFP_AG_RESPONSE_HOLD_REJECT,
};

// hfp ag mock state
enum HfpAgMockState {
    HFP_AG_MOCK_DEFAULT = 0,
    HFP_AG_MOCK,
};
// callsetup indicator
enum HfpCallSetupStatus {
    HFP_AG_CALLSETUP_NONE = 0,  // Not currently in call set up
    HFP_AG_CALLSETUP_INCOMING,  // Incoming call
    HFP_AG_CALLSETUP_OUTGOING,  // Outgoing call dialing
    HFP_AG_CALLSETUP_ALERTING   // Outgoing call alerting
};

// callheld indicator
enum HfpCallheldStatus {
    HFP_AG_CALLHELD_INACTIVE = 0,  // No held calls
    HFP_AG_CALLHELD_ACTIVE,        // Call held and call active
    HFP_AG_CALLHELD_NOACTIVE,      // Call held and no call active
};

// service indicator
enum HfpAgServiceStatus {
    HFP_AG_SERVICE_NOT_AVAILABLE = 0,  // Service not available
    HFP_AG_SERVICE_AVAILABLE           // Service available
};

// roam indicator
enum HfpAgRoamingState { HFP_AG_ROMAING_STATE_HOME = 0, HFP_AG_ROMAING_STATE_ROAMING };

enum HfpAgCallState {
    HFP_AG_CALL_STATE_ACTIVE = 0,
    HFP_AG_CALL_STATE_HELD,
    HFP_AG_CALL_STATE_DIALING,
    HFP_AG_CALL_STATE_ALERTING,
    HFP_AG_CALL_STATE_INCOMING,
    HFP_AG_CALL_STATE_WAITING,
    HFP_AG_CALL_STATE_DISCONNECTED,
    HFP_AG_CALL_STATE_DISCONNECTING,
    HFP_AG_CALL_STATE_IDLE
};

// Ag
enum HfpAgNumberType { HFP_AG_UNKNOWN_NUMBER = 0, HFP_AG_INTERNATIONAL_NUMBER };

// Ag
enum HfpAgInbandRingAction { HFP_AG_INBAND_RING_DISABLE = 0, HFP_AG_INBAND_RING_ENABLE = 1 };

// Ag
typedef struct {
    uint8_t index {0};
    uint8_t dir {0};
    uint8_t state {0};
    uint8_t mode {0};
    uint8_t mpty {0};
    uint8_t type {0};
    std::string number {""};
} HfpAgCallList;

typedef struct {
    int activeNum {0};
    int heldNum {0};
    int callState {0};
    std::string number {""};
    int type {0};
    std::string name {""};
} HfpAgPhoneState;

enum HfpAgNrecAction { HFP_AG_ECNR_ENABLE = 0, HFP_AG_ECNR_DISABLE };

enum HfpAgCmeeAction { HFP_AG_CMEE_ENABLE = 0, HFP_AG_CMEE_DISABLE };

enum HfpAgChldAction { HFP_AG_ACTION_CHLD_1 = 0, HFP_AG_ACTION_CHLD_2, HFP_AG_ACTION_CHLD_1X, HFP_AG_ACTION_CHLD_2X };

enum HfpAgBtrhAction {
    HFP_AG_ACTION_BTRH_0 = 0,  // Incoming call is put on hold in the AG
    HFP_AG_ACTION_BTRH_1 = 1,  // Held incoming call is accepted in the AG
    HFP_AG_ACTION_BTRH_2 = 2   // Held incoming call is rejected in the AG
};

enum HfpAgCliAction { HFP_AG_CLI_ENABLE = 0, HFP_AG_CLI_DISABLE };

enum HfpAgCcwaAction { HFP_AG_CCWA_ENABLE = 0, HFP_AG_CCWA_DISABLE };

enum {
    HFP_AG_NOTIFY_AG_OF_HF_SUPPORTED_INDICATORS = 0,
    HFP_AG_QUERY_AG_SUPPORTED_INDICATORS,
    HFP_AG_QUERY_AG_ENABLED_INDICATORS,
};

enum {
    HFP_AG_INVALID_EVT = 0,

    // service start/stop
    HFP_AG_SERVICE_STARTUP_EVT = 1,
    HFP_AG_SERVICE_SHUTDOWN_EVT = 2,

    // service connect events
    HFP_AG_CONNECT_EVT = 10,
    HFP_AG_DISCONNECT_EVT = 11,
    HFP_AG_CONNECT_AUDIO_EVT = 20,
    HFP_AG_DISCONNECT_AUDIO_EVT = 21,
    HFP_AG_RETRY_CONNECT_AUDIO_EVT = 22,
    HFP_AG_REMOVE_STATE_MACHINE_EVT = 30,

    // service timeout events
    HFP_AG_CONNECTION_TIMEOUT_EVT,
    HFP_AG_DISCONNECT_TIMEOUT_EVT,
    HFP_AG_CONNECT_AUDIO_TIMEOUT_EVT,
    HFP_AG_DISCONNECT_AUDIO_TIMEOUT_EVT,

    // service normal events
    HFP_AG_OPEN_VOICE_RECOGNITION_EVT,
    HFP_AG_CLOSE_VOICE_RECOGNITION_EVT,
    HFP_AG_VOICE_RECOGNITION_RESULT_EVT,
    HFP_AG_SET_MICROPHONE_GAIN_EVT,
    HFP_AG_SET_VOLUME_EVT,
    HFP_AG_SET_INBAND_RING_TONE_EVT,
    HFP_AG_SEND_SUBSCRIBER_NUMBER_EVT,
    HFP_AG_SEND_NETWORK_OPERATOR_EVT,
    HFP_AG_CONTROL_OTHER_MODULES_EVT,
    HFP_AG_DIALING_OUT_RESULT,
    HFP_AG_CALL_STATE_CHANGE,
    HFP_AG_CALL_STATE_CHANGE_MOCK,
    HFP_AG_SEND_CCLC_RESPONSE,
    HFP_AG_NOTIFY_SERVICE_STATE,
    HFP_AG_NOTIFY_ROAM_STATE,
    HFP_AG_NOTIFY_SIGNAL_STRENGTH,
    HFP_AG_NOTIFY_BATTERY_LEVEL,
    HFP_AG_RESPONSE_CLCC_TIME_OUT_EVT,
    HFP_AG_VOICE_RECOGNITION_TIME_OUT_EVT,
    HFP_AG_DIAL_TIME_OUT_EVT,
    HFP_AG_NOTIFY_INDICATOR_EVT,
    HFP_AG_SEND_INCOMING_EVT,
    HFP_AG_SEND_CALL_SETUP_EVT,
    HFP_AG_SEND_CALL_HELD_EVT,
    HFP_AG_SEND_CALL_STATE_EVT,
    HFP_AG_SEND_RESPONSE_HOLD_STATE,

    // stack sdp events
    HFP_AG_SDP_DISCOVERY_RESULT_SUCCESS = 98,
    HFP_AG_SDP_DISCOVERY_RESULT_FAIL = 99,

    // stack rfcomm events
    HFP_AG_CONNECT_REQUEST_EVT = 100,
    HFP_AG_CONNECTED_EVT = 110,
    HFP_AG_DISCONNECTED_EVT = 111,
    HFP_AG_CONNECT_FAILED_EVT = 112,
    HFP_AG_DISCONNECT_FAILED_EVT = 113,
    HFP_AG_DATA_AVAILABLE_EVT = 114,

    // service level connection established event
    HFP_AG_CODEC_NEGOTIATION_FAILED = 197,
    HFP_AG_SETUP_CODEC_CVSD = 198,
    HFP_AG_SLC_ESTABLISHED_EVT = 199,
    HFP_AG_CODEC_NEGOTIATED_EVT = 200,

    // stack audio events
    HFP_AG_AUDIO_CONNECT_REQUEST_EVT = 201,
    HFP_AG_AUDIO_CONNECTING_EVT = 202,
    HFP_AG_AUDIO_DISCONNECTING_EVT = 203,
    HFP_AG_AUDIO_CONNECTED_EVT = 220,
    HFP_AG_AUDIO_DISCONNECTED_EVT = 221,
    HFP_AG_AUDIO_CONNECT_FAILED_EVT,
    HFP_AG_AUDIO_DISCONNECT_FAILED_EVT,

    HFP_AG_RING_TIMEOUT_EVT,
    HFP_AG_PROCESS_CKPD_EVT,
    HFP_AG_RESPONE_OK_EVT,
    HFP_AG_GET_VOICE_NUMBER,
    HFP_AG_SEND_BINP_EVT,
    HFP_AG_GET_BTRH_EVT,
    HFP_AG_SET_BTRH_EVT,
    HFP_AG_SEND_BTRH_EVT,
    HFP_AG_SEND_NO_CARRIER,
    HFP_AG_START_MOCK,

    HFP_AG_MOCK_RING,
    HFP_AG_MOCK_CLIP,
};

constexpr int HFP_AG_HF_FOUND = 1;
constexpr int HFP_AG_HS_FOUND = 2;
constexpr int HFP_AG_HF_HS_FOUND = 3;

constexpr int HSP_AG_STATE_BOTH = 0X01;
constexpr int HSP_AG_STATE_HSP = 0X02;
constexpr int HSP_AG_STATE_NONE = 0X03;

// This is the message type that Hfp controls other modules.
enum {
    HFP_AG_MSG_TYPE_TYPE_NONE,
    HFP_AG_MSG_TYPE_ANSWER_CALL,
    HFP_AG_MSG_TYPE_HANGUP_CALL,
    HFP_AG_MSG_TYPE_VOLUME_CHANGED,
    HFP_AG_MSG_TYPE_DIAL_CALL,
    HFP_AG_MSG_TYPE_SEND_DTMF,
    HFP_AG_MSG_TYPE_NOISE_REDUCTION,
    HFP_AG_MSG_TYPE_AT_WBS,
    HFP_AG_MSG_TYPE_AT_CHLD,
    HFP_AG_MSG_TYPE_SUBSCRIBER_NUMBER_REQUEST,
    HFP_AG_MSG_TYPE_AT_CIND,
    HFP_AG_MSG_TYPE_AT_COPS,
    HFP_AG_MSG_TYPE_AT_CLCC,
    HFP_AG_MSG_TYPE_AT_UNKNOWN,
    HFP_AG_MSG_TYPE_KEY_PRESSED,
    HFP_AG_MSG_TYPE_AT_BIND,
    HFP_AG_MSG_TYPE_AT_BIEV,
    HFP_AG_MSG_TYPE_AT_BIA,
    HFP_AG_MSG_TYPE_QUERY_AG_INDICATOR,
    HFP_AG_MSG_TYPE_VR_CHANGED,
};

enum HfpAgVolumeType { HFP_AG_VOLUME_TYPE_SPK, HFP_AG_VOLUME_TYPE_MIC };

enum { HFP_AG_WBS_NONE, HFP_AG_WBS_NO, HFP_AG_WBS_YES };

enum { HFP_AG_SUCCESS = 0, HFP_AG_FAILURE };

enum { HFP_AG_HF_INDICATOR_ENHANCED_DRIVER_SAFETY_ID = 1, HFP_AG_HF_INDICATOR_BATTERY_LEVEL_ID = 2 };

enum { HFP_AG_HF_VR_ClOSED, HFP_AG_HF_VR_OPENED };

enum { HFP_AG_NUMBER_DIAL, HFP_AG_MEMORY_DIAL, HFP_AG_LAST_NUMBER_REDIAL };

enum BTCallDirection {
    BT_CALL_DIRECTION_OUT = 0,
    BT_CALL_DIRECTION_IN,
    BT_CALL_DIRECTION_UNKNOW
};

struct MockCall {
    std::string number = "";
    int callstate = 0;
    int type = 0;
};

struct BluetoothCall {
    std::string number = "";
    int callstate = 0;
    int precallstate = 0;
    int type = 0;
    int callid = 0;
};

constexpr int CALL_TYPE_DEFAULT = 129;
constexpr int CALL_TYPE_VOIP = 225;

constexpr int ATCHLD_RELEASE_ALL_HELD_CALLS = 0;
constexpr int ATCHLD_RELEASE_ACTIVE_ACCPET_OTHER = 1;
constexpr int ATCHLD_RELEASE_HOLD_ACCPET_OTHER = 2;
constexpr int ATCHLD_ADD_CALL_TO_CONVERSATION = 3;
constexpr int ATCHLD_CONNECT_TWO_CALL = 4;
constexpr int ATCHLD_RELEASE_INDEX_ONE = 11;
constexpr int ATCHLD_RELEASE_INDEX_TWO = 12;
constexpr int ATCHLD_CONSULTATION_INDEX_ONE = 21;
constexpr int ATCHLD_CONSULTATION_INDEX_TWO = 22;

constexpr int HFP_AG_CALL_NUM_TWO = 2;
}  // namespace bluetooth
}  // namespace OHOS
#endif // HFP_AG_DEFINES_H
