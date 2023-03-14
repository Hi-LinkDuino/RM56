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

#ifndef HFP_HF_AUDIO_CONNECTION_H
#define HFP_HF_AUDIO_CONNECTION_H

#include <cstdint>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "base_def.h"
#include "btm.h"
#include "btstack.h"
#include "hfp_hf_defines.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief Class for Audio connection required by HandfreeUnit.
 */
class HfpHfAudioConnection {
public:
    struct AudioDevice {
        std::string addr {""};
        uint8_t linkType {LINK_TYPE_SCO};
        uint16_t handle {0};
        int role {ROLE_INVALID};
        int lastConnectResult {CONNECT_NONE};
        int lastParam {SETTING_NONE};
    };

    typedef struct {
        uint8_t status {0};
        uint16_t connectionHandle {0};
        BtAddr addr {};
    } HfpScoConnectionCompleteParam;

    typedef struct {
        BtAddr addr {};
        uint8_t linkType {0};
    } HfpScoConnectionRequestParam;

    typedef struct {
        uint8_t status {0};
        uint16_t connectionHandle {0};
        uint8_t reason {0};
    } HfpScoDisconnectionCompleteParam;

    /**
     * @brief Register audio callback, start listen.
     */
    static int Register();

    /**
     * @brief Deregister audio callback, stop listen.
     */
    static int Deregister();

    /**
     * @brief Check whether audio is connected.
     *
     * @return Returns true if the audio is connected else return false.
     * @return Returns <b>true</b> if the audio is connected; returns <b>false</b> if audio is not connected.
     */
    static bool IsAudioConnected();

    /**
     * @brief Construct a new HfpAgAudioConnection object as default.
     */
    HfpHfAudioConnection() = default;

    /**
     * @brief Destroy the HfpAgAudioConnection object as default.
     */
    ~HfpHfAudioConnection() = default;

    /**
     * @brief Connect sco.
     *
     * @return Returns audio connect result.
     */
    int ConnectAudio() const;

    /**
     * @brief Disconnect sco.
     *
     * @return Returns audio disconnect result.
     */
    int DisconnectAudio() const;

    /**
     * @brief Accept sco connect request.
     *
     * @return Returns accept audio connect result.
     */
    int AcceptAudioConnection() const;

    /**
     * @brief Reject sco connect request.
     *
     * @return Returns reject audio connect result.
     */
    int RejectAudioConnection() const;

    /**
     * @brief Set the Remote Address.
     *
     * @param addr Remote device address.
     */
    void SetRemoteAddr(const std::string &addr);

    /**
     * @brief Set the Support Features.
     *
     * @param escoSupport Esco support flag.
     * @param escoS4Support Esco S4 support flag.
     * @param inUseCodec Codec in use.
     */
    void SetSupportFeatures(bool escoSupport, bool escoS4Support, int inUseCodec);

private:
    /**
     * @brief Connect complete callback.
     *
     * @param param Audio connection complete parameters.
     * @param context Audio connection context.
     */
    static void OnConnectCompleted(const BtmScoConnectionCompleteParam *param, void *context);

    /**
     * @brief Connection change callback.
     *
     * @param param Audio connection change parameters.
     * @param context Audio connection context.
     */
    static void OnConnectionChanged(const BtmScoConnectionChangedParam *param, void *context);

    /**
     * @brief Disconnect complete callback.
     *
     * @param param Audio disconnection complete parameters.
     * @param context Audio connection context.
     */
    static void OnDisconnectCompleted(const BtmScoDisconnectionCompleteParam *param, void *context);

    /**
     * @brief Connect request callback.
     *
     * @param param Audio connection request parameters.
     * @param context Audio connection context.
     */
    static void OnConnectRequest(const BtmScoConnectionRequestParam *param, void *context);

    /**
     * @brief Voice setting callback.
     *
     * @param status Write voice setting status.
     * @param context Audio connection context.
     */
    static void OnWriteVoiceSettingCompleted(uint8_t status, void *context);

    /**
     * @brief Process connect completed event on hfp thread.
     *
     * @param parameters connect completed parameters.
     */
    static void ProcessOnConnectCompleted(HfpScoConnectionCompleteParam parameters);

    /**
     * @brief Process connect completed fail event on hfp thread.
     *
     * @param parameters connect completed parameters.
     */
    static void ProcessOnConnectCompletedFail(
        std::vector<HfpHfAudioConnection::AudioDevice>::iterator dev, const std::string &address);

    /**
     * @brief Process disconnect completed event on hfp thread.
     *
     * @param parameters disconnect completed parameters.
     */
    static void ProcessOnDisconnectCompleted(HfpScoDisconnectionCompleteParam parameters);

    /**
     * @brief Process connect request event on hfp thread.
     *
     * @param parameters connect request parameters.
     */
    static void ProcessOnConnectRequest(HfpScoConnectionRequestParam parameters);

    /**
     * @brief Convert bluetooth address to BtAddr.
     *
     * @param address Remote device address.
     * @return Returns remote device address in stack.
     */
    static BtAddr ConvertToBtAddr(const std::string &address);

    /**
     * @brief Get the Device By Addr object.
     *
     * @param addr Remote device address.
     * @return Returns audio device or nullopt.
     */
    static std::vector<HfpHfAudioConnection::AudioDevice>::iterator GetDeviceByAddr(const std::string &addr);

    /**
     * @brief Get the Device By Handle object.
     *
     * @param handle Remote device handle.
     * @return Returns audio device or nullopt.
     */
    static std::vector<HfpHfAudioConnection::AudioDevice>::iterator GetDeviceByHandle(uint16_t handle);

    /**
     * @brief Accept SCO by MSBC.
     *
     * @param btAddr Bt address.
     * @return Returns accept SCO by MSBC result.
     */
    static int AcceptByMsbc(BtAddr btAddr);

    /**
     * @brief Accept SCO by CVSD.
     *
     * @param dev Audio device.
     * @param btAddr Bt address.
     * @return Returns accept SCO by CVSD result.
     */
    int AcceptByCvsd(AudioDevice &dev, BtAddr btAddr) const;

    /**
     * @brief Connect SCO by CVSD.
     *
     * @param dev Audio device.
     * @param btAddr Bt address.
     * @param cvsdEscoFailed Flag for last connect status by cvsd.
     * @return Returns connect SCO by CVSD result.
     */
    int ConnectByCvsd(AudioDevice &dev, BtAddr btAddr, bool cvsdEscoFailed) const;

    BT_DISALLOW_COPY_AND_ASSIGN(HfpHfAudioConnection);

    // Btm SCO callbacks
    static BtmScoCallbacks g_cbs;

    // active device address
    static std::string g_activeAddr;

    // device vectors
    static std::vector<HfpHfAudioConnection::AudioDevice> g_audioDevices;

    // Packet types
    static inline constexpr int PACKET_TYPE_HV1 = 0x00000001;
    static inline constexpr int PACKET_TYPE_HV2 = 0x00000002;
    static inline constexpr int PACKET_TYPE_HV3 = 0x00000004;
    static inline constexpr int PACKET_TYPE_EV3 = 0x00000008;
    static inline constexpr int PACKET_TYPE_EV4 = 0x00000010;
    static inline constexpr int PACKET_TYPE_EV5 = 0x00000020;
    static inline constexpr int PACKET_TYPE_NO_2_EV3 = 0x00000040;
    static inline constexpr int PACKET_TYPE_NO_3_EV3 = 0x00000080;
    static inline constexpr int PACKET_TYPE_NO_2_EV5 = 0x00000100;
    static inline constexpr int PACKET_TYPE_NO_3_EV5 = 0x00000200;

    static inline constexpr int PACKET_TYPE_CVSD = PACKET_TYPE_HV1 | PACKET_TYPE_HV2 | PACKET_TYPE_HV3 |
                                                   PACKET_TYPE_EV3 | PACKET_TYPE_EV4 | PACKET_TYPE_EV5 |
                                                   PACKET_TYPE_NO_3_EV3 | PACKET_TYPE_NO_2_EV5 | PACKET_TYPE_NO_3_EV5;

    static inline constexpr int PACKET_TYPE_MSBC_T1 =
        PACKET_TYPE_EV3 | PACKET_TYPE_NO_3_EV3 | PACKET_TYPE_NO_2_EV5 | PACKET_TYPE_NO_3_EV5 | PACKET_TYPE_NO_2_EV3;

    static inline constexpr int PACKET_TYPE_MSBC_T2 =
        PACKET_TYPE_EV3 | PACKET_TYPE_NO_3_EV3 | PACKET_TYPE_NO_2_EV5 | PACKET_TYPE_NO_3_EV5;

    // Transmit&Receive bandwidth
    static inline constexpr int TRANSMIT_BANDWIDTH = 0x00001f40;  // Decimal 8000, 64 kbits/sec
    static inline constexpr int RECEIVE_BANDWIDTH = 0x00001f40;   // Decimal 8000, 64 kbits/sec

    // CVSD SCO settings
    static inline constexpr int CVSD_SCO_PACKET_TYPE = PACKET_TYPE_HV1 | PACKET_TYPE_HV3;
    static inline constexpr int CVSD_SCO_MAX_LATENCY = 0x00000ffff;
    static inline constexpr int CVSD_SCO_RETRANSMISSION_EFFORT = 0x000000ff;

    // CVSD ESCO settings
    static inline constexpr int CVSD_ESCO_MAX_LATENCY_S1 = 0x00000007;
    static inline constexpr int CVSD_ESCO_MAX_LATENCY_S4 = 0x0000000C;
    static inline constexpr int CVSD_ESCO_RETRANSMISSION_EFFORT_S1 = 0x00000001;
    static inline constexpr int CVSD_ESCO_RETRANSMISSION_EFFORT_S4 = 0x00000002;

    // MSBC ESCO settings
    static inline constexpr int CVSD_ESCO_PACKET_TYPE_S1 = PACKET_TYPE_CVSD;
    static inline constexpr int CVSD_ESCO_PACKET_TYPE_S4 = PACKET_TYPE_CVSD;
    static inline constexpr int MSBC_ESCO_PACKET_TYPE_T1 = PACKET_TYPE_MSBC_T1;
    static inline constexpr int MSBC_ESCO_PACKET_TYPE_T2 = PACKET_TYPE_MSBC_T2;
    static inline constexpr int MSBC_ESCO_MAX_LATENCY_T1 = 0x00000008;
    static inline constexpr int MSBC_ESCO_MAX_LATENCY_T2 = 0x0000000D;
    static inline constexpr int MSBC_ESCO_RETRANSMISSION_EFFORT = 0x00000002;

    // Voice settings
    static inline constexpr int BTM_VOICE_SETTING_CVSD = 0x00000060;
    static inline constexpr int BTM_VOICE_SETTING_TRANS = 0x00000063;

    // (e)sco parameter set
    static inline constexpr int SETTING_NONE = 0x00000000;
    static inline constexpr int MSBC_ESCO_T2 = 0x00000001;
    static inline constexpr int MSBC_ESCO_T1 = 0x00000002;
    static inline constexpr int CVSD_ESCO_S4 = 0x00000004;
    static inline constexpr int CVSD_ESCO_S1 = 0x00000008;
    static inline constexpr int CVSD_SCO = 0x00000010;

    // role type
    static inline constexpr int ROLE_INVALID = 0x00000000;
    static inline constexpr int ROLE_INITIATOR = 0x00000001;
    static inline constexpr int ROLE_ACCEPTOR = 0x00000002;

    // connect result
    static inline constexpr int CONNECT_NONE = 0x00000000;
    static inline constexpr int CONNECT_SUCCESS = 0x00000001;
    static inline constexpr int CONNECT_FAIL = 0x00000002;

    // HCI result
    static inline constexpr int REJECT_DUE_TO_LIMITED_RESOURCES = 0x0000000D;
    static inline constexpr int REMOTE_USER_TERMINATED_CONNECTION = 0x00000013;

    // MSBC ESCO T2 parameters set
    static inline const BtmCreateEscoConnectionParam MSBC_T2_PARAM = {
        {{0}, 0},
        TRANSMIT_BANDWIDTH,
        RECEIVE_BANDWIDTH,
        CODEC_MSBC_T2,
        MSBC_ESCO_MAX_LATENCY_T2,
        MSBC_ESCO_PACKET_TYPE_T2,
        MSBC_ESCO_RETRANSMISSION_EFFORT
    };

    // MSBC ESCO T1 parameters set
    static inline const BtmCreateEscoConnectionParam MSBC_T1_PARAM = {
        {{0}, 0},
        TRANSMIT_BANDWIDTH,
        RECEIVE_BANDWIDTH,
        CODEC_MSBC_T1,
        MSBC_ESCO_MAX_LATENCY_T1,
        MSBC_ESCO_PACKET_TYPE_T1,
        MSBC_ESCO_RETRANSMISSION_EFFORT
    };

    // CVSD ESCO S4 parameters set
    static inline const BtmCreateEscoConnectionParam CVSD_ESCO_S4_PARAM = {
        {{0}, 0},
        TRANSMIT_BANDWIDTH,
        RECEIVE_BANDWIDTH,
        CODEC_CVSD,
        CVSD_ESCO_MAX_LATENCY_S4,
        CVSD_ESCO_PACKET_TYPE_S4,
        CVSD_ESCO_RETRANSMISSION_EFFORT_S4
    };

    // CVSD ESCO S1 parameters set
    static inline const BtmCreateEscoConnectionParam CVSD_ESCO_S1_PARAM = {
        {{0}, 0},
        TRANSMIT_BANDWIDTH,
        RECEIVE_BANDWIDTH,
        CODEC_CVSD,
        CVSD_ESCO_MAX_LATENCY_S1,
        CVSD_ESCO_PACKET_TYPE_S1,
        CVSD_ESCO_RETRANSMISSION_EFFORT_S1
    };

    // CVSD SCO parameters set
    static inline const BtmCreateScoConnectionParam CVSD_SCO_PARAM = {
        {{0}, 0},
        TRANSMIT_BANDWIDTH,
        RECEIVE_BANDWIDTH,
        CVSD_SCO_MAX_LATENCY,
        BTM_VOICE_SETTING_CVSD,
        CVSD_SCO_RETRANSMISSION_EFFORT,
        CVSD_SCO_PACKET_TYPE
    };

    std::string remoteAddr_ {""};
    bool escoSupport_ {false};
    bool escoS4Support_ {false};
    int inUseCodec_ {HFP_HF_CODEC_CVSD};
};
}  // namespace bluetooth
}  // namespace OHOS
#endif // HFP_HF_AUDIO_CONNECTION_H