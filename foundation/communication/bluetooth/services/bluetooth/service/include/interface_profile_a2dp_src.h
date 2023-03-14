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
 * @file interface_profile_a2dp_src.h
 *
 * @brief A2dp codec configuration information of a2dp source
 *
 * @since 6
 */

#ifndef INTERFACE_PROFILE_A2DP_SRC_H
#define INTERFACE_PROFILE_A2DP_SRC_H

#include "interface_profile.h"

namespace OHOS {
namespace bluetooth {
struct A2dpSrcCodecInfo {
    // Codec priority
    uint32_t codecPriority;

    // Codec type
    uint8_t codecType;

    // Codec sample
    uint32_t sampleRate;

    // Codec bits per sample
    uint32_t bitsPerSample;

    // Codec channel mode
    uint8_t channelMode;

    // Codec specific value1
    uint64_t codecSpecific1;

    // Codec specific value2
    uint64_t codecSpecific2;

    // Codec specific value3
    uint64_t codecSpecific3;

    // Codec specific value4
    uint64_t codecSpecific4;
};

/**
 * @brief The codec configuration and capability information of a2dp source
 *
 * @since 6.0
 */
struct A2dpSrcCodecStatus {
    // current codec information
    A2dpSrcCodecInfo codecInfo {};

    // local codec information
    std::vector<A2dpSrcCodecInfo> codecInfoLocalCap {};

    // Local device and peer confirmed codec information
    std::vector<A2dpSrcCodecInfo> codecInfoConfirmedCap {};
};

/**
 * @brief Callback function api of A2DP service, including connection, disconnection.
 *
 * @since 6.0
 */
class IA2dpObserver {
public:
    /**
     * @brief A destructor used to delete the A2DP Service Observer instance.
     *
     * @since 6.0
     */
    virtual ~IA2dpObserver() = default;

    /**
     * @brief The callback function after device's playing state changed.
     *
     * @param device  the remote bluetooth device.
     * @param playingState  the playing state after changing.
     * @param error  the error information.
     * @since 6.0
     */
    virtual void OnPlayingStatusChaned(const RawAddress &device, int playingState, int error) {};

    /**
     * @brief The callback function after device's codec information changed.
     *
     * @param device  the remote bluetooth device.
     * @param info  the device's codec information.
     * @param error  the error information.
     * @since 6.0
     */
    virtual void OnConfigurationChanged(const RawAddress &device, const A2dpSrcCodecInfo &info, int error) {};

    /**
     * @brief ConnectionState Changed observer.
     * @param device bluetooth device address.
     * @param state Connection state.
     * @since 6.0
     */
    virtual void OnConnectionStateChanged(const RawAddress &remoteAddr, int state) {};
};

/**
 * @brief This class provides functions called by Framework API.
 *
 * @since 6.0
 */
class IProfileA2dp : public IProfile {
public:
    /**
     * @brief Get a2dp source service instance.
     *
     * @return Returns an instance of a2dp source service.
     * @since 6.0
     */
    static IProfileA2dp *GetSrcProfile();

    /**
     * @brief Get a2dp sink service instance.
     *
     * @return Returns an instance of a2dp sink service.
     * @since 6.0
     */
    static IProfileA2dp *GetSnkProfile();

    /**
     * @brief Get devices by connection states.
     *
     * @param states The connection states of the bluetooth device.
     * @return Returns devices that match the connection states.
     * @since 6.0
     */
    virtual std::vector<RawAddress> GetDevicesByStates(std::vector<int> &states) const = 0;

    /**
     * @brief Get device connection state by address.
     *
     * @param device The address of the peer bluetooth device.
     * @return Returns <b>A2DP_DISCONNECTED</b> if device connect state is disconnected;
     *         Returns <b>A2DP_DISCONNECTING</b> if device connect state is disconnecting;
     *         Returns <b>A2DP_CONNECTED</b> if device connect state is connected;
     *         Returns <b>A2DP_CONNECTING</b> if device connect state is connecting;
     *         Returns <b>A2DP_INVALID_STATUS</b> if target device is not in device list;
     * @since 6.0
     */
    virtual int GetDeviceState(const RawAddress &device) const = 0;

    /**
     * @brief Get device playing state by address when target device is on connected.
     *
     * @param device The address of the peer bluetooth device.
     * @return Returns <b>1</b> if device is on playing;
     *         Returns <b>0</b> if device is not on playing;
     * @since 6.0
     */
    virtual int GetPlayingState(const RawAddress &device) const = 0;

    /**
     * @brief Set target device as active device.
     *
     * @param device The address of the peer bluetooth device.
     * @return Returns <b>RET_NO_ERROR</b> Target device has already been active, or perform normal setting processing.
     *         Returns <b>RET_BAD_STATUS</b> Target device is not on connected, or set fails.
     * @since 6.0
     */
    virtual int SetActiveSinkDevice(const RawAddress &device) = 0;

    /**
     * @brief Get active device.
     * @return Returns active device.
     * @since 6.0
     */
    virtual const RawAddress &GetActiveSinkDevice() const = 0;

    /**
     * @brief Set connection strategy for peer bluetooth device.
     *        If peer device is connected and the policy is set not allowed,then perform disconnect operation.
     *        If peer device is disconnected and the policy is set allowed,then perform connect operation.
     *
     * @param device The address of the peer bluetooth device.
     * @param strategy The device connect strategy.
     * @return Returns <b>RET_NO_ERROR</b> if the operation is successful.
     *         Returns <b>RET_BAD_STATUS</b> if the operation fails.
     * @since 6.0
     */
    virtual int SetConnectStrategy(const RawAddress &device, int strategy) = 0;

    /**
     * @brief Get connection strategy of peer bluetooth device.
     *
     * @param device The address of the peer bluetooth device.
     * @return Returns <b>CONNECTION_ALLOWED</b> if the peer device is allowed to connect.
     *         Returns <b>CONNECTION_FORBIDDEN</b> if the peer device is not allowed to connect.
     *         Returns <b>CONNECTION_UNKNOWN</b> if the connection policy is unknown.
     * @since 6.0
     */
    virtual int GetConnectStrategy(const RawAddress &device) const = 0;

    /**
     * @brief Send delay reporting.
     *
     * @param device The address of the peer bluetooth device.
     * @param delayValue The delay value.
     * @return Returns <b>RET_NO_ERROR</b> if the operation is successful.
     *         Returns <b>RET_BAD_STATUS</b> if the operation fails.
     */
    virtual int SendDelay(const RawAddress &device, uint16_t delayValue) = 0;

    /**
     * @brief Get codec status information of connected device.
     *
     * @param device The address of the bluetooth device.
     * @return Returns codec status information of connected device.
     * @since 6.0
     */
    virtual A2dpSrcCodecStatus GetCodecStatus(const RawAddress &device) const = 0;

    /**
     * @brief Set the codec encoding preferences of the specified device.
     *
     * @param device The address of the bluetooth device.
     * @param info The codec encoding information.
     * @return Return
     * @since 6.0
     */
    virtual int SetCodecPreference(const RawAddress &device, const A2dpSrcCodecInfo &info) = 0;

    /**
     * @brief Set whether enables the optional codec.
     *
     * @param device The address of the bluetooth device.
     * @param isEnable Set true if enables the optional codec and set optional codec's priority high.
     *                 Set false if disables the optional codec and set optional codec's priority low.
     * @since 6.0
     */
    virtual void SwitchOptionalCodecs(const RawAddress &device, bool isEnable) = 0;

    /**
     * @brief Get whether the peer bluetooth device supports optional codec.
     *
     * @param device The address of the bluetooth device.
     * @return Returns <b>A2DP_OPTIONAL_SUPPORT</b> The device supports optional codec.
     *         Returns <b>A2DP_OPTIONAL_NOT_SUPPORT</b> The device doesn't support optional codec.
     *         Returns <b>A2DP_OPTIONAL_SUPPORT_UNKNOWN</b> Don't know if the device support optional codec.
     * @since 6.0
     */
    virtual int GetOptionalCodecsSupportState(const RawAddress &device) const = 0;

    /**
     * @brief Audio start streaming.
     *
     * @param device The address of the bluetooth device.
     * @return Returns <b>RET_NO_ERROR</b> if the operation is successful.
     *         Returns <b>RET_BAD_STATUS</b> if the operation fails, or device is not in device list.
     * @since 6.0
     */
    virtual int StartPlaying(const RawAddress &device) = 0;

    /**
     * @brief Audio suspend streaming.
     *
     * @param device The address of the bluetooth device.
     * @return Returns <b>RET_NO_ERROR</b> if the operation is successful.
     *         Returns <b>RET_BAD_STATUS</b> if the operation fails, or device is not in device list.
     * @since 6.0
     */
    virtual int SuspendPlaying(const RawAddress &device) = 0;

    /**
     * @brief Audio stop streaming.
     *
     * @param device The address of the bluetooth device.
     * @return Returns <b>RET_NO_ERROR</b> if the operation is successful.
     *         Returns <b>RET_BAD_STATUS</b> if the operation fails, or device is not in device list.
     * @since 6.0
     */
    virtual int StopPlaying(const RawAddress &device) = 0;

    /**
     * @brief Register observer function of framework.
     *
     * @param observer The observer function pointer of framework.
     * @since 6.0
     */
    virtual void RegisterObserver(IA2dpObserver *observer) = 0;

    /**
     * @brief Deregister observer function of framework.
     *
     * @since 6.0
     */
    virtual void DeregisterObserver(IA2dpObserver *observer) = 0;

    /**
     * @brief Send frame data.
     * @param[in] data: The address of the data
     * @param[in] size: The size of the data
     * @since 6.0
     */
    virtual int WriteFrame(const uint8_t *data, uint32_t size) = 0;

    /**
     * @brief Get the current rendered position.
     * @param[out] delayValue: The delayed time
     * @param[out] sendDataSize: The size of the data sent
     * @param[out] timeStamp: The time of the current position
     * @since 6.0
     */
    virtual void GetRenderPosition(uint16_t &delayValue, uint16_t &sendDataSize, uint32_t &timeStamp) = 0;
};
/**
 * @brief This class provides functions called by Framework API for a2dp source.
 *
 * @since 6.0
 */
class IProfileA2dpSrc : public IProfileA2dp {};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // INTERFACE_PROFILE_A2DP_SRC_H