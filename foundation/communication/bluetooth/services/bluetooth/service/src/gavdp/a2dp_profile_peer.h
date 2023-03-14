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

#ifndef A2DP_PROFILE_PEER_H
#define A2DP_PROFILE_PEER_H

#include <cstdint>
#include <map>
#include <memory>
#include <string>

#include "a2dp_codec/include/a2dp_codec_wrapper.h"
#include "a2dp_codec/include/a2dp_codec_constant.h"
#include "a2dp_codec/include/a2dp_codec_factory.h"
#include "a2dp_def.h"
#include "a2dp_sdp.h"
#include "a2dp_state_machine.h"
#include "avdtp.h"
#include "base_def.h"
#include "btstack.h"
#include "interface_profile_a2dp_src.h"
#include "sdp.h"
#include "timer.h"
#include "../stack/platform/include/queue.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief The stream information associated with the a2dp profile.
 *
 * @since 6.0
 */
class A2dpStream {
public:
    /**
     * @brief A constructor used to create an <b>A2dpStream</b> instance.
     *
     * @since 6.0
     */
    A2dpStream();

    /**
     * @brief A desconstructor used to destroy an <b>A2dpStream</b> instance.
     *
     * @since 6.0
     */
    ~A2dpStream();

    /**
     * @brief A function to set the local handle of stream.
     *
     * @param[in] The local handle of stream
     * @since 6.0
     */
    void SetHandle(uint16_t hdl);

    /**
     * @brief A function to get the local handle of stream.
     *
     * @return The local handle of stream
     * @since 6.0
     */
    uint16_t GetHandle() const;

    /**
     * @brief A function to get the peer capability of stream.
     *
     * @return The detail of peer capability
     * @since 6.0
     */
    AvdtSepConfig GetPeerSEPInformation() const;

    /**
     * @brief A function to save the peer capability of stream.
     *
     * @param[in] The peer capability of stream
     * @since 6.0
     */
    void SetPeerCapability(AvdtSepConfig cap);

    /**
     * @brief A function to save the local codec information of stream.
     *
     * @param[in] The local codec information of stream
     * @since 6.0
     */
    void SetCodecInfo(CodecInfo data);

    /**
     * @brief A function to get the local codec information of stream.
     *
     * @return The local codec information of stream
     * @since 6.0
     */
    CodecInfo GetCodecInfo() const;

    /**
     * @brief A function to save the peer seid of stream.
     *
     * @param[in] The peer seid of stream
     * @since 6.0
     */
    void SetAcpSeid(uint8_t seid);

    /**
     * @brief A function to get the peer seid of stream.
     *
     * @param[in] The peer seid of stream
     * @since 6.0
     */
    uint8_t GetAcpSeid() const;

    /**
     * @brief A function to get the peer capability.
     *
     * @since 6.0
     */
    bool GetPeerCap() const;

    /**
     * @brief A function to get the peer capability.
     *
     * @since 6.0
     */
    void SetPeerCap(bool value);

private:
    uint16_t handle_ = 0;
    uint8_t acpSeid_ = 0;
    AvdtSepConfig peerCapability_ {};
    CodecInfo codecInfo_ {};
    bool peerCap_ = false;
};

class A2dpCodecDecoderObserver : public A2dpDecoderObserver {
public:
    ~A2dpCodecDecoderObserver() override = default;
    void DataAvailable(uint8_t *buf, uint32_t size) override;
};

/**
 * @brief This class provides a set of methods to manager the peer device information
 *
 * @since 6.0
 */
class A2dpProfilePeer {
public:
    /**
     * @brief A constructor used to create an <b>A2dpProfilePeer</b> instance.
     *
     * @since 6.0
     */
    A2dpProfilePeer(const BtAddr &addr, uint8_t localRole);

    /**
     * @brief A destructor used to delete an <b>A2dpProfilePeer</b> instance.
     *
     * @since 6.0
     */
    ~A2dpProfilePeer();

    /**
     * @brief A function used to get the information of peer SEP.
     *
     * @return The information of peer SEP
     * @since 6.0
     */
    AvdtSepInfo GetPeerSEPInformation() const;

    /**
     * @brief A function used to update the index of peer stream endpoint.
     *        sepIndex++
     * @since 6.0
     */
    void UpdateSepIndex();

    /**
     * @brief A function used to save the sep information of peer.
     *
     * @param[in] The information of peer SEP
     * @since 6.0
     */
    void SetPeerSepInfo(AvdtDiscover seps);

    /**
     * @brief A function used to save the capability information of peer and other information
     * related into local.
     *
     * @param[in] intSeid The handle of initiator
     * @param[in] acpSeid The handle of acceptor/peer
     * @param[in] cap The capability of peer stream endpoint
     * @param[in] role The role of local profile
     * @return true Save and matched config success
     * @return false Capability is invalid or configure is not matched
     * @since 6.0
     */
    bool SetPeerCapInfo(uint8_t intSeid, uint8_t acpSeid, AvdtSepConfig cap, uint8_t role);

    /**
     * @brief A function used to save the handle of stream for local profile.
     *
     * @param[in] The handle of stream for local profile
     * @since 6.0
     */
    void SetStreamHandle(uint16_t handle);

    /**
     * @brief A function used to save the seid of peer stream endpoint
     *
     * @param[in] The seid of peer stream endpoint
     * @since 6.0
     */
    void SetAcpSeid(uint8_t seid);

    /**
     * @brief A function used to save the seid of local stream endpoint
     *
     * @param[in] The seid of local stream endpoint
     * @since 6.0
     */
    void SetIntSeid(uint8_t seid);

    /**
     * @brief A function used to get the current seid of peer stream endpoint
     *
     * @return The current seid of peer stream endpoint
     * @since 6.0
     */
    uint8_t GetAcpSeid() const;

    /**
     * @brief A function used to get the current seid of local stream endpoint
     *
     * @return The current seid of local stream endpoint
     * @since 6.0
     */
    uint8_t GetIntSeid() const;

    /**
     * @brief A function used to get the number of peer stream endpoint
     *
     * @return The number of peer stream endpoint
     * @since 6.0
     */
    uint8_t GetPeerSepNum() const;

    /**
     * @brief A function used to get the handle of stream
     *
     * @return The handle of stream
     * @since 6.0
     */
    uint16_t GetStreamHandle() const;

    /**
     * @brief A function used to get the address of peer device
     *
     * @return The address of peer device
     * @since 6.0
     */
    BtAddr GetPeerAddress() const;

    /**
     * @brief A function used to get the capability of peer
     *
     * @return The the capability of peer
     * @since 6.0
     */
    AvdtSepConfig GetPeerCapabilityInfo() const;

    /**
     * @brief A function used to get state machine of profile
     *
     * @return The capability of peer
     * @since 6.0
     */
    A2dpStateMachine *GetStateMachine();

    /**
     * @brief A function used to get the service information of peer device
     *
     * @param[in] The address of peer
     * @param[in] The service attributes of peer
     * @param[in] The number of services
     * @param[in] The content of other information related with this class
     * @since 6.0
     */
    static void SDPServiceCallback(
        const BtAddr *addr, const SdpService *serviceArray, const uint16_t serviceNum, void *context);

    /**
     * @brief A function used to set the services class of peer is matched or not
     *
     * @param[in] true The service class of peer is matched
     * @param[in] false The service class of peer is not matched
     * @since 6.0
     */
    void SetSDPServiceCapability(bool value);

    /**
     * @brief A function used to get the services class of peer is matched or not
     *
     * @return true The service class of peer is matched
     * @return false The service class of peer is not matched
     * @since 6.0
     */
    bool GetSDPServiceCapability() const;

    /**
     * @brief A function used to set the avdtp version of peer
     *
     * @param[in] true The avdtp version of peer is ver1.3.2
     * @param[in] false The avdtp version of peer is not ver1.3.2
     * @since 6.0
     */
    void SetAvdtpVersion(bool value);

    /**
     * @brief A function used to get the avdtp version of peer
     *
     * @return true The avdtp version of peer is ver1.3.2
     * @return false The avdtp version of peer is not ver1.3.2
     * @since 6.0
     */
    bool GetAvdtpVersion() const;

    /**
     * @brief A function used to set the a2dp version of peer
     *
     * @param[in] true The a2dp version of peer is ver1.3.2
     * @param[in] false The a2dp version of peer is not ver1.3.2
     * @since 6.0
     */
    void SetA2dpVersion(bool value);

    /**
     * @brief A function used to get the a2dp version of peer
     *
     * @return true The a2dp version of peer is ver1.3.2
     * @return false The a2dp version of peer is not ver1.3.2
     * @since 6.0
     */
    bool GetA2dpVersion() const;

    /**
     * @brief A function used to get the codec configure of peer
     *
     * @return The configure of codec peer
     * @since 6.0
     */
    A2dpCodecFactory *GetCodecConfigure() const;

    /**
     * @brief A function used to get the configure matched
     *
     * @param[in] role The role of local profile
     * @return true Find the matched configure
     * @return false Not fine the matched configure
     * @since 6.0
     */
    bool JudgeCapabilityMatched(uint8_t role);

    /**
     * @brief A function to stop timeout
     *
     * @since 6.0
     */
    void StopSignalingTimer() const;

    /**
     * @brief A function to set timeout
     *
     * @param[in] ms The timer
     * @param[in] isPeriodic  TRUE: period; FALSE: once only
     * @since 6.0
     */
    void SetSignalingTimer(int ms, bool isPeriodic) const;

    /**
     * @brief A function to process timeout switched thread
     *
     * @param[in] The role of profile(AVDT_ROLE_SRC/AVDT_ROLE_SNK)
     * @since 6.0
     */
    void SignalingTimeoutCallback(uint8_t role) const;

    /**
     * @brief A function to process timeout this class
     *
     * @since 6.0
     */
    void ProcessSignalingTimeoutCallback();

    /**
     * @brief A function to save the command sent just
     *
     * @param[in] The command sent just
     * @since 6.0
     */
    void SetCurrentCmd(uint8_t cmd);

    /**
     * @brief A function to get the command sent just
     *
     * @return The command sent just
     * @since 6.0
     */
    uint8_t GetCurrentCmd() const;

    /**
     * @brief A function to process timeout this class
     *
     * @param[in] info: User configure information
     * @since 6.0
     */
    int SetUserCodecConfigure(const A2dpSrcCodecInfo &info);

    /**
     * @brief A function to enable/disable the active device
     *
     * @param[in] value: True is enable optional codec; False is disable optional codec.
     * @since 6.0
     */
    bool EnableOptionalCodec(bool value);

    /**
     * @brief Notify to encoder pcm data.
     * @param[in] addr: The address of peer device
     * @since 6.0
     */
    void NotifyEncoder();

    /**
     * @brief Notify to decode frame data.
     * @since 6.0
     */
    void NotifyDecoder();

    /**
     * @brief Notify to update audio reconfigure.
     * @since 6.0
     */
    void NotifyAudioConfigChanged() const;

    /**
     * @brief Update the configure by user configure triggered.
     * @since 6.0
     */
    void UpdateConfigure();

    /**
     * @brief Update the configure by user configure triggered.
     * @since 6.0
     */
    AvdtSepConfig GetReconfig() const;

    /**
     * @brief Get the restart status.
     * @since 6.0
     */
    bool GetRestart() const;

    /**
     * @brief Set the restart status.
     * @since 6.0
     */
    void SetRestart(bool value);

    /**
     * @brief Get the reconfig tag status.
     * @since 6.0
     */
    bool GetReconfigTag() const;

    /**
     * @brief Set the reconfig tag status.
     * @since 6.0
     */
    void SetReconfigTag(bool value);

    /**
     * @brief A function to get the codec status.
     *
     * @since 6.0
     */
    A2dpSrcCodecStatus GetCodecStatus() const;

    /**
     * @brief A function to set the optional support config.
     *
     * @param A2DP_OPTIONAL_NOT_SUPPORT: Not support optional codec; A2DP_OPTIONAL_SUPPORT: support optional codec
     * @since 6.0
     */
    void SetOptionalCodecsSupportState(int state) const;

    /**
     * @brief A function to get the configure of active stream
     *
     * @param[in] The address of active device
     * @return The configure of active stream
     * @since 6.0
     */
    const ConfigureStream &GetConfigure() const;

    /**
     * @brief A function to set the configure of active stream
     *
     * @param[in] The address of active device
     * @since 6.0
     */
    void SetConfigure();

    /**
     * @brief A function to set the init role is source or not
     *
     * @param[in] The value of init is source or not
     * @since 6.0
     */
    void SetInitSide(bool value);

    /**
     * @brief A function to get the init role is source or not
     *
     * @return The value of init is source or not
     * @since 6.0
     */
    bool GetInitSide() const;

    /**
     * @brief A function to set indication status
     *
     * @param[in] The value of indication is disconnect or not
     * @since 6.0
     */
    void SetDisconnectIndication(bool value);

    /**
     * @brief A function to get indication status
     *
     * @return The value of indication is disconnect or not
     * @since 6.0
     */
    bool GetDisconnectIndication() const;

    /**
     * @brief A function to get the number of capability is got
     *
     * @return The result of capability is got complete
     * @since 6.0
     */
    bool GetPeerCapComplete(uint8_t role) const;

    /**
     * @brief A function to set the number of capability is got
     *
     * @return The number of capability is got
     * @since 6.0
     */
    void SetNumberOfPeerCap(bool value);

    /**
     * @brief A function to set peer mtu size
     *
     * @param[in] The value of mtu size
     * @return -
     * @since 6.0
     */
    void UpdatePeerMtu(uint16_t mtu);

    /**
     * @brief A function to set peer edr
     *
     * @param[in] The value of edr
     * @return -
     * @since 6.0
     */
    void UpdatePeerEdr(uint8_t edr);

    bool SendPacket(const Packet *packet, size_t frames, uint32_t bytes, uint32_t pktTimeStamp) const;

private:
    BT_DISALLOW_COPY_AND_ASSIGN(A2dpProfilePeer);
    /**
     * @brief Reconfigure
     * @param[in] The information of reconfigure
     * @since 6.0
     */
    void Reconfigure(bool close);

    /**
     * @brief A function used to register the local stream endpoint to avdtp
     *
     * @param[in] addr The address of peer device
     * @param[in] role The role of local profile
     * @since 6.0
     */
    void RegisterSEPConfigureInfo(const BtAddr &addr, uint8_t role);

    /**
     * @brief A function used to update the peer capability information
     *
     * @param[in] acpSeid The seid of peer device
     * @param[in] cap The capability information
     * @param[in] role The role of local profile
     * @since 6.0
     */
    void UpdatePeerCapabilityInfo(uint8_t acpSeid, AvdtSepConfig cap, uint8_t role);

    /**
     * @brief A function to find peer capability
     *
     * @param[in] The capability of peer device
     * @param[out] The property of selectable capability
     * @since 6.0
     */
    static void FindPeerSelectableCapabilityMatched(A2dpSrcCodecInfo &selectableCap, AvdtSepConfig cap);

    /**
     * @brief A function to find capability matched
     *
     * @param[in] config The configure information
     * @param[in] role The role of local profile
     * @since 6.0
     */
    bool FindCapabilityMatched(const A2dpCodecConfig &config, const uint8_t role);

    /**
     * @brief Change the information of user configure
     *
     * @param[in] THe information of user set
     * @param[out] The configure information to set
     * @since 6.0
     */
    void ChangeUserConfigureInfo(
        A2dpCodecCapability &userConfig, uint8_t *codecInfo, const A2dpSrcCodecInfo info);

    /**
     * @brief Select the information of codec
     *
     * @param[in] THe index of user set
     * @param[out] userConfig The configure information to set
     * @param[out] codecInfo The information of codec selected
     * @since 6.0
     */
    void SelectCodecInfo(A2dpCodecCapability &userConfig, uint8_t *codecInfo, A2dpCodecIndex index);

    /**
     * @brief A function used to parse the service information of peer
     *
     * @param[in] addr The address of peer device
     * @param[in] serviceArray The service information of peer profile
     * @param[in] serviceNum The number of service
     * @param[in] role The role of the peer
     * @since 6.0
     */
    static void ParseSDPInformation(
        const BtAddr &addr, const SdpService *serviceArray, uint16_t serviceNum, uint8_t role);

    /**
     * @brief A function to create codecindex list with priority
     *
     * @since 6.0
     */
    void CreateDefaultCodecPriority();

    /**
     * @brief Init
     * @param[in] addr The address of peer device
     * @param[in] localRole The role of peer is source or sink
     * @since 6.0
     */
    void A2dpProfilePeerInit(const BtAddr &addr, const uint8_t localRole);

    bool avdtpVer132_ = false;
    bool a2dpVer132_ = false;
    bool isService_ = false;
    bool restart_ = false;
    uint8_t peerSepIndex_ = 0;
    uint8_t selectbleStreamIndex_ = 0;
    uint8_t localRole_ = 0;  // SEP type of local profile
    uint8_t acpId_ = 0;
    uint8_t intId_ = 0;
    uint8_t peerNumSeps_ = 0;
    uint8_t currentCmd_ = EVT_SDP_DISC;
    uint8_t sourceStreamNum_ = (A2DP_SOURCE_CODEC_INDEX_MAX - A2DP_SOURCE_CODEC_INDEX_SBC);
    uint8_t sinkStreamNum_ = (A2DP_SINK_CODEC_INDEX_MAX - A2DP_SINK_CODEC_INDEX_MIN);
    uint16_t streamHandle_ = 0;
    BtAddr peerAddress_ {};
    A2dpStream streamCtrl_[A2DP_CODEC_INDEX_MAX - A2DP_SOURCE_CODEC_INDEX_SBC];
    A2dpCodecFactory *codecConfig_ = nullptr;
    std::map<A2dpCodecIndex, A2dpCodecPriority> defaultCodecPriorities_ {};
    ConfigureStream configureStream_ = {};
    A2dpStateMachine stateMachine_ {};
    AvdtSepInfo peerSepInfo_[AVDT_NUM_SEPS] {};
    AvdtSepConfig codecReconfig_ {};
    std::unique_ptr<utility::Timer> signalingTimer_ = nullptr;
    uint16_t mtu_ = 0;
    bool isInitSide_ = false;
    bool disconnectInd_ = false;
    std::unique_ptr<A2dpDecoderObserver> decoderObserver_ = nullptr;
    uint8_t capNumber_ = 0;
    uint8_t edr_ = 0;
    bool codecAACConfig = false;
    bool reconfigTag_ = false;
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // A2DP_PROFILE_PEER_H