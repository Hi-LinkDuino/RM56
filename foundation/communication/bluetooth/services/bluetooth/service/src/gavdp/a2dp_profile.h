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

/**
 * @file a2dp_profile.h
 *
 * @brief Declares the class of the a2dp profile, including attributes, and methods.
 *
 * @since 6
 */

#ifndef A2DP_PROFILE_H
#define A2DP_PROFILE_H

#include <cstdint>
#include <map>
#include <string>

#include "a2dp_codec/include/a2dp_codec_constant.h"
#include "a2dp_def.h"
#include "a2dp_profile_peer.h"
#include "a2dp_sdp.h"
#include "base_def.h"
#include "btstack.h"
#include "gap_if.h"
#include "interface_profile_a2dp_src.h"
#include "message.h"
#include "a2dp_shared_buffer.h"
#include "../stack/platform/include/queue.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief This class provides a set of methods to notify the service about status changed
 *
 * @since 6.0
 */
class A2dpProfileObserver {
public:
    /**
     * @brief A function to notify the service the connect status changed.
     *
     * @param[in]   addr: The address of peer device
     * @param[in]   state: The state of connection
     * @param[in]   context: the detail information of this callback
     * @since 6.0
     */
    virtual void OnConnectStateChanged(const BtAddr &addr, const int state, void *context) = 0;

    /**
     * @brief A function to notify the service the codec status changed.
     *
     * @param[in]   addr: The address of peer device
     * @param[in]   codecInfo: The codec information of stream
     * @param[in]   context: the detail information of this callback
     * @since 6.0
     */
    virtual void OnCodecStateChanged(const BtAddr &addr, const A2dpSrcCodecStatus codecInfo, void *context) = 0;

    /**
     * @brief A function to notify the service the connect status changed.
     *
     * @param[in]   addr: The address of peer device
     * @param[in]   state: The state of stream
     * @param[in]   context: the detail information of this callback
     * @since 6.0
     */
    virtual void OnAudioStateChanged(const BtAddr &addr, const int state, void *context) = 0;

    /**
     * @brief A destructor used to delete the <b>A2dpProfileObserver</b> instance.
     *
     * @since 6.0
     */
    virtual ~A2dpProfileObserver() = default;
};

/**
 * @brief This class provides a set of methods of a2dp profile
 *
 * @since 6.0
 */
class A2dpProfile {
public:
    /**
     * @brief A constructor used to create an <b>A2dpProfile</b> instance.
     *
     * @since 6.0
     */
    explicit A2dpProfile(const uint8_t role);

    /**
     * @brief A desstructor used to delete an <b>A2dpProfile</b> instance.
     *
     * @since 6.0
     */
    virtual ~A2dpProfile();

    /**
     * @brief A function used to enable the a2dp profile
     *
     * @since 6.0
     */
    void Enable();

    /**
     * @brief A function used to disable the a2dp profile
     *
     * @since 6.0
     */
    void Disable();

    /**
     * @brief A function used to connect the peer device
     *
     * @param[in] device The address of peer device
     * @return 0 Success; Otherwise is failed to connect
     * @since 6.0
     */
    int Connect(const BtAddr &device);

    /**
     * @brief A function used to disconnect the peer device
     *
     * @param[in] device The address of peer device
     * @return 0 Success; Otherwise is failed to connect
     * @since 6.0
     */
    int Disconnect(const BtAddr &device);

    /**
     * @brief A function used to suspend the stream
     *
     * @param[in] handle The local handle of stream
     * @param[in] suspend TRUE: Stop the stream;
     *                    FALSE: Suspend the stream
     * @return 0 Success; Otherwise is failed to connect
     * @since 6.0
     */
    int Stop(const uint16_t handle, const bool suspend);

    /**
     * @brief A function used to start the stream
     *
     * @param[in] handle The local handle of stream
     * @return 0 Success; Otherwise is failed to connect
     * @since 6.0
     */
    int Start(const uint16_t handle);

    /**
     * @brief A function used to close the stream
     *
     * @param[in] handle The local handle of stream
     * @return 0 Success; Otherwise is failed to connect
     * @since 6.0
     */
    int Close(const uint16_t handle) const;

    /**
     * @brief True if it has a streaming channel
     *
     * @return True if it has a streaming channel, Otherwise false.
     * @since 6.0
     */
    bool HasStreaming();

    /**
     * @brief True if it has a open channel
     *
     * @return True if it has a open channel, Otherwise false.
     * @since 6.0
     */
    bool HasOpen();

    /**
     * @brief Set the tag of the disalbe.
     *
     * @since 6.0
     */
    void SetDisalbeTag(bool tag);

    /**
     * @brief Get the tag of the disalbe.
     *
    * @return True if bt is disalbe, Otherwise false.
     * @since 6.0
     */
    bool GetDisalbeTag();

    /**
     * @brief A function used to close all the stream
     *
     * @since 6.0
     */
    void CloseAll();

    /**
     * @brief A function used to feedback close the stream for pts
     *
     * @param[in] handle The local handle of stream
     * @param[in] errorCode The error code of response of close
     * @return 0 Success; Otherwise is failed to connect
     * @since 6.0
     */
    int CloseRsp(const uint16_t handle, const uint8_t errorCode) const;

    /**
     * @brief A function used to send stream for pts
     *
     * @param[in] handle The local handle of stream
     * @return 0 Success; Otherwise is failed to connect
     * @since 6.0
     */
    static int WriteStream(const uint16_t handle, const uint8_t config, const uint8_t bitpool);

    /**
     * @brief A function used to send delay value to source,pts only
     *
     * @param[in] handle The local handle of stream
     * @param[in] delayValue The value of delay of sync audio with video
     * @return 0 Success; Otherwise is failed to connect
     * @since 6.0
     */
    static int SendDelay(const uint16_t handle, const uint16_t delayValue);

    /**
     * @brief A function used to reconfigrue the stream
     *
     * @param[in] handle The local handle of stream
     * @param[in] codecInfo The configure information
     * @return 0 Success; Otherwise is failed to connect
     * @since 6.0
     */
    int Reconfigure(const uint16_t handle, uint8_t *codecInfo) const;

    /**
     * @brief A function supplied to register observer of profile
     *
     * @param[in] observer The pointer of service callback function
     * @since 6.0
     */
    void RegisterObserver(A2dpProfileObserver *observer);

    /**
     * @brief A function to process service informaiton of peer
     *
     * @param[in] addr The address of peer device
     * @param[in] context The realated content of this class
     * @since 6.0
     */
    void ProcessSDPCallback(const BtAddr &addr, uint8_t result);

    /**
     * @brief A function to get the role of this profile
     *
     * @return The role of a2dp
     * @since 6.0
     */
    uint8_t GetRole() const;

    /**
     * @brief A function to deregister the pointer of callback
     *
     * @param[in] The pointer of callback of service
     * @since 6.0
     */
    static void DeregisterObserver(A2dpProfileObserver *observer);

    /**
     * @brief A function to delete the profile peer of address
     *
     * @param[in] The address of peer device
     * @return TRUE: SUCCESS; otherwise failed to delete
     * @since 6.0
     */
    bool DeletePeer(const BtAddr &peerAddress);

    /**
     * @brief A function to get the enable status of a2dp profile
     *
     * @return TRUE: Enable; otherwise disable
     * @since 6.0
     */
    bool GetProfileEnable() const;

    /**
     * @brief A function to set the enable status of a2dp profile
     *
     * @return TRUE: Enable; otherwise disable
     * @since 6.0
     */
    void SetProfileEnable(bool value);

    /**
     * @brief A function to get the enable status of a2dp profile
     *
     * @return TRUE: allowed to start; otherwise other stream is exited
     * @since 6.0
     */
    bool JudgeAllowedStreaming() const;

    /**
     * @brief A function to get the profile peer instance by peer addess
     *
     * @param[in] The address of peer device
     * @return The pointer of peer informtion instance
     * @since 6.0
     */
    A2dpProfilePeer *FindPeerByAddress(const BtAddr &peerAddress) const;

    /**
     * @brief A function to get the profile peer instance by stream handle
     *
     * @param[in] The handle of stream
     * @return The pointer of peer information instance
     * @since 6.0
     */
    A2dpProfilePeer *FindPeerByHandle(uint16_t handle) const;

    /**
     * @brief A function to get/create the profile peer instance by peer address
     *
     * @param[in] peerAddress The address of peer device
     * @param[in] sepType
     * @return The pointer of peer information instance
     * @since 6.0
     */
    A2dpProfilePeer *FindOrCreatePeer(const BtAddr &peerAddress, uint8_t localRole);

    /**
     * @brief A function to save the active device address of peer
     *
     * @param[in] The address of peer device
     * @since 6.0
     */
    void SetActivePeer(const BtAddr &peer);

    /**
     * @brief A function to get the active device address of peer
     *
     * @return The address of active device
     * @since 6.0
     */
    const BtAddr &GetActivePeer() const;

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
    void SetConfigure(const BtAddr &addr);

    /**
     * @brief A function to build codec information
     *
     * @param[in] index The index of codec
     * @param[out] data The codec information
     * @since 6.0
     */
    static void BuildCodecInfo(A2dpCodecIndex index, uint8_t *data);

    /**
     * @brief A function to create stream endpoint information of local profile
     *
     * @param[in] role The role of profile
     * @param[out] cfg The sep information to regist to avdtp
     * @since 6.0
     */
    static void CreateSEPConfigureInfo(uint8_t role);

    /**
     * @brief A function to process the event of avdtp
     *
     * @param[in] addr The address of peer device
     * @param[in] message The message from avdtp
     * @since 6.0
     */
    void ProcessAvdtpCallback(const BtAddr &addr, const utility::Message &message);

    /**
     * @brief A function to notify the service the connect status changed.
     *
     * @param[in]   addr: The address of peer device
     * @param[in]   state: The state of connection
     * @param[in]   context: the detail information of this callback
     * @since 6.0
     */
    void ConnectStateChangedNotify(const BtAddr &addr, const int state, void *context);

    /**
     * @brief A function to notify the service the connect status changed.
     *
     * @param[in]   addr: The address of peer device
     * @param[in]   state: The state of connection
     * @param[in]   context: the detail information of this callback
     * @since 6.0
     */
    void AudioStateChangedNotify(const BtAddr &addr, const int state, void *context) const;

    /**
     * @brief A function to notify the service the codec status changed.
     *
     * @param[in]   addr: The address of peer device
     * @param[in]   context: the detail information of this callback
     * @since 6.0
     */
    void CodecChangedNotify(const BtAddr &addr, void *context) const;

    /**
     * @brief A function to process timeout this class
     *
     * @since 6.0
     */
    void ProcessSignalingTimeoutCallback(const BtAddr &addr) const;

    /**
     * @brief A function to process timeout this class
     *
     * @param[in] addr: The address of peer device
     * @param[in] info: User configure information
     * @return Return the result setted.
     * @since 6.0
     */
    int SetUserCodecConfigure(const BtAddr &addr, const A2dpSrcCodecInfo &info);

    /**
     * @brief A function to judge the active device
     *
     * @param[in] addr: The address of peer device
     * @since 6.0
     */
    bool IsActiveDevice(const BtAddr &addr) const;

    /**
     * @brief A function to clear the active device
     *
     * @since 6.0
     */
    void ClearActiveDevice();

    /**
     * @brief A function to enable/disable the active device
     *
     * @param[in] addr: The address of peer device
     * @param[in] value: True is enable optional codec; False is disable optional codec.
     * @since 6.0
     */
    bool EnableOptionalCodec(const BtAddr &addr, bool value);

    /**
     * @brief Notify to encoder pcm data.
     * @param[in] addr: The address of peer device
     * @since 6.0
     */
    void NotifyEncoder(const BtAddr &addr) const;

    /**
     * @brief Notify to decode frame data.
     * @param[in] addr: The address of peer device
     * @since 6.0
     */
    void NotifyDecoder(const BtAddr &addr) const;

    /**
     * @brief Get the information of the current rendered position.
     * @param[out] dalayValue is the delayed time
     * @param[out] sendDataSize is the data size that has been sent
     * @param[out] timeStamp is the current time stamp
     * @since 6.0
     */
    void GetRenderPosition(uint16_t &delayValue, uint16_t &sendDataSize, uint32_t &timeStamp);

    /**
     * @brief Enqueue the frame packet to the packet queue.
     * @param[in] packet Pointer of the packet
     * @param[in] frames Number of frames
     * @param[in] bytes Size of bytes
     * @param[in] pktTimeStamp The current time stamp
     * @since 6.0
     */
    void EnqueuePacket(const Packet *packet, size_t frames, uint32_t bytes, uint32_t pktTimeStamp);

    /**
     * @brief Dequeue the frame packet to the peer.
     * @since 6.0
     */
    void DequeuePacket();

    /**
     * @brief Set the pcm data to the shared buffer.
     * @param buf The pointer of the data.
     * @param size The size of the data
     * @since 6.0
     */
    uint32_t SetPcmData(const uint8_t *buf, uint32_t size);

    /**
     * @brief Get the pcm data from the shared buffer.
     * @param buf The pointer of the data.
     * @param size The size of the data
     * @since 6.0
     */
    uint32_t GetPcmData(uint8_t *buf, uint32_t size);

    /**
     * @brief A function to notify the delay report value changed.
     *
     * @param[in]   addr: The address of peer device
     * @param[in]   delayValue: The delay value
     */
    void DelayReportNotify(const BtAddr &addr, const uint16_t delayValue);
private:
    /**
     * @brief Get the instance of SDP.
     *
     * @return return to the instance of SDP
     * @since 6.0
     */
    A2dpSdpManager GetSDPInstance(void) const;

    /**
     * @brief A function to update the number of peer device.
     *
     * @param[in]   true: add new device; false: disconnect the device
     * @return true: Success;  false: The number of connected is max
     * @since 6.0
     */
    static bool UpdateNumberPeerDevice(bool plus);

    /**
     * @brief Get the number of connected peer devices.
     *
     * @return   The number of connected peer device
     * @since 6.0
     */
    static uint8_t GetConnectedPeerDevice();

    /**
     * @brief A function to clear the number of peer device.
     *
     * @since 6.0
     */
    static void ClearNumberPeerDevice();

    /**
     * @brief A function to update the register of gap.
     *
     * @param[in]   true: Regist; false: Not regist
     * @since 6.0
     */
    static void UpdateGapRegisterInfo(bool value);

    /**
     * @brief A function to get the regist information of gap.
     *
     * @return   true: Regist; false: Not regist
     * @since 6.0
     */
    static bool GetGapRegisterInfo();

    /**
     * @brief The function is used to register the security of profile to GAP
     *
     * @param[in] direction The direction of incoming or outcoming
     * @param[in] serviceId The source or sink service id of a2dp
     * @param[in] protocolId The id of avdtp
     * @param[in] channelId  The channel id
     * @param[in] securityMode The mode of register security
     * @return zero is successful
     * @since 6.0
     */
    static int RegisterServiceSecurity(GAP_ServiceConnectDirection direction, GAP_Service serviceId,
        GAP_SecMultiplexingProtocol protocolId, GapSecChannel channelId, uint16_t securityMode);

    /**
     * @brief The function is used to deregister the security of profile to GAP
     *
     * @param[in] direction The direction of incoming or outcoming
     * @param[in] serviceId The source or sink service id of a2dp
     * @return zero is successful
     * @since 6.0
     */
    static int DeregisterServiceSecurity(GAP_ServiceConnectDirection direction, GAP_Service serviceId);

    /**
     * @brief A function to reset the delay report value.
     *
     * @param[in]   addr: The address of peer device
     */
    void ResetDelayValue(const BtAddr &device);

    BT_DISALLOW_COPY_AND_ASSIGN(A2dpProfile);
    A2dpProfile() = delete;
    bool enable_ = false;
    uint8_t role_ = A2DP_ROLE_SOURCE;
    static uint8_t g_linkNum;
    static bool g_registGap;
    BtAddr activePeer_ {};
    std::map<std::string, A2dpProfilePeer *> peers_ {};
    ConfigureStream configureStream_ = {};
    A2dpSdpManager sdpInstance_ {};
    A2dpProfileObserver *a2dpSvcCBack_ {nullptr};
    Queue *packetQueue_ = nullptr; // queue of packets to be sent
    A2dpSharedBuffer *buffer_ = nullptr;
    bool isDoDisable_ = false;
    uint16_t delayValue_ = 0;
};
/**
 * @brief A function to receive stream data.
 *
 * @param[in] handle The handle of stream(after configured)
 * @param[in] pkt The content of stream
 * @param[in] timeStamp The timestamp of frame
 * @param[in] pt The pakeckt type of frame
 * @param[in] streamHandle The codec index
 * @since 6.0
 */
void ProcessSinkStream(uint16_t handle, Packet *pkt, uint32_t timeStamp, uint8_t pt, uint16_t streamHandle);
/**
 * @brief A function to clean packet data.
 *
 * @param[in] data The packet data to be deleted
 * @since 6.0
 */
void CleanPacketData(void *data);

struct PacketData {
    Packet *packet;
    size_t frames;
    uint32_t bytes;
    uint32_t pktTimeStamp;
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // A2DP_PROFILE_H