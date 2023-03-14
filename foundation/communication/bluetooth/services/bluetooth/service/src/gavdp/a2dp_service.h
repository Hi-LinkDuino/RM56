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
 * @addtogroup bluetooth
 * @file a2dp_service.h
 *
 * @brief Declare the interface and function implementation of A2DP service and profile.
 *
 * @since 6
 */

#ifndef A2DP_SERVICE_H
#define A2DP_SERVICE_H

#include <cstdint>
#include <list>
#include <map>
#include <string>

#include "a2dp_def.h"
#include "a2dp_profile.h"
#include "a2dp_service_connection.h"
#include "a2dp_service_device.h"
#include "a2dp_sink.h"
#include "a2dp_source.h"
#include "base_observer_list.h"
#include "btstack.h"
#include "context.h"
#include "interface_profile.h"
#include "interface_profile_a2dp_src.h"
#include "message.h"
#include "raw_address.h"

namespace OHOS {
namespace bluetooth {
class ObserverProfile : public A2dpProfileObserver {
public:
    /**
     * @brief A constructor used to create an <b>ObserverProfile</b> instance.
     *
     * @param[in] role The role of profile
     * @since 6.0
     */
    explicit ObserverProfile(uint8_t role);

    /**
     * @brief A constructor used to create an <b>ObserverProfile</b> instance.
     *
     * @since 6.0
     */
    ObserverProfile()
    {}

    /**
     * @brief A desstructor used to create an <b>ObserverProfile</b> instance.
     *
     * @since 6.0
     */
    ~ObserverProfile() override = default;

    /**
     * @brief The connection status changes from connected to disconnected.
     * @param[in] addr  The address of Bluetooth remote device.
     * @param[in] state  The connect state of Bluetooth remote device.
     * @param[in] context  The context is used to send the event in the callback.
     * @since 6.0
     */
    void OnConnectStateChanged(const BtAddr &addr, const int state, void *context) override;

    /**
     * @brief Audio playing status change.
     * @param[in] addr  The address of Bluetooth remote device.
     * @param[in] state  The playing status of remote device after chang.
     * @param[in] context  The context is used to send the event in the callback.
     * @since 6.0
     */
    void OnAudioStateChanged(const BtAddr &addr, const int state, void *context) override;

    /**
     * @brief Audio playing status change.
     * @param[in] addr  The address of Bluetooth remote device.
     * @param[in] codecInfo  Codec information after chang.
     * @param[in] context  The context is used to send the event in the callback.
     * @since 6.0
     */
    void OnCodecStateChanged(const BtAddr &addr, const A2dpSrcCodecStatus codecInfo, void *context) override;

private:
    /**
     * @brief Process the message of  connect state changed .
     *
     * @param[in] btAddr The address of remote device
     * @param[in] deviceInfo The information of remote device
     * @return  The state of connected
     * @since 6.0
     */
    int ProcessConnectStateMessage(
        RawAddress btAddr, A2dpDeviceInfo *deviceInfo, const int connectPolicy, const int state, const uint16_t handle);

    /**
     * @brief Process the a2dp hdf load .
     *
     * @param[in] state The connection state
     * @return void
     * @since 6.0
     */
    void ProcessA2dpHdfLoad(const int state) const;
    /**
     * @brief Update the stateinfo.
     *
     * @param[in] msgCMD The value of what for message
     * @param[in] stateValue The value of state
     * @param[out] msg The address of message
     * @param[out] state The address of state
     * @since 6.0
     */
    static void UpdateStateInformation(utility::Message &msg, int &state, const int msgCMD, const int stateValue);
    uint8_t role_ {};
};

/**
 * @brief This class provides functions called by Framework API.
 *
 * @since 6.0
 */
class A2dpService : public IProfileA2dp, public utility::Context {
public:
    /**
     * @brief A constructor used to create an a2dpservice instance.
     *
     * @param name Service name.
     * @param version Profile version.
     * @param role Profile role.
     * @since 6.0
     */
    A2dpService(const std::string &name, const std::string version, const uint8_t role);

    /**
     * @brief A destructor used to delete the a2dpservice instance.
     *
     * @since 6.0
     */
    ~A2dpService();

    /**
     * @brief Enable the target service.
     *
     * @since 6.0
     */
    void Enable() override;

    /**
     * @brief Disable the target service.
     *
     * @since 6.0
     */
    void Disable() override;

    /**
     * @brief Get the instance of the A2DP source or sink object.
     *
     * @return Returns the instance of the A2DP source or sink object.
     * @since 6.0
     */
    utility::Context *GetContext() override;

    /**
     * @brief Connect to the peer bluetooth device.
     *
     * @param device The address of the peer bluetooth device.
     * @return Returns <b>BT_NO_ERROR</b> Perform normal connection processing.
     *         Returns <b>RET_BAD_STATUS</b> Target device is on connected,or connecting.
     *         Returns <b>RET_NO_SUPPORT</b> Target device is not allowed to connect,or the connection fails.
     * @since 6.0
     */
    int Connect(const RawAddress &device) override;

    /**
     * @brief Disconnect with the peer bluetooth service.
     *
     * @param device The address of the peer bluetooth device.
     * @return Returns <b>BT_NO_ERROR</b> if perform normal disconnection processing.
     *         Returns <b>RET_BAD_PARAM</b> if target device is not in the device list.
     *         Returns <b>BT_OPERATION_FAILED</b> if target device is on disconnected,or disconnecting.
     *         Returns <b>RET_NO_SUPPORT</b> if disconnection fails.
     * @since 6.0
     */
    int Disconnect(const RawAddress &device) override;

    /**
     * @brief Get connected device.
     *
     * @return Returns Connected devices.
     * @since 6.0
     */
    std::list<RawAddress> GetConnectDevices() override;

    /**
     * @brief Get connect state of local device.
     *
     * @return Returns <b>PROFILE_STATE_DISCONNECTED</b> if device connection state is disconnected;
     *         Returns <b>PROFILE_STATE_DISCONNECTING</b> if device connection state is disconnecting;
     *         Returns <b>PROFILE_STATE_CONNECTED</b> if device connection state is connected;
     *         Returns <b>PROFILE_STATE_CONNECTING</b> if device connection state is connecting;
     * @since 6.0
     */
    int GetConnectState() override;

    /**
     * @brief Get max number of connected device.
     *
     * @return Returns max connect number.
     * @since 6.0
     */
    int GetMaxConnectNum() override;

    /**
     * @brief Get devices by connection states.
     *
     * @param states The connection states of the bluetooth device.
     * @return Returns devices that match the connection states.
     * @since 6.0
     */
    std::vector<RawAddress> GetDevicesByStates(std::vector<int> &states) const override;

    /**
     * @brief Get device connection state by address.
     *
     * @param device The address of the peer bluetooth device.
     * @return Returns <b>DISCONNECTED</b> if device connect state is disconnected;
     *         Returns <b>DISCONNECTING</b> if device connect state is disconnecting;
     *         Returns <b>CONNECTED</b> if device connect state is connected;
     *         Returns <b>CONNECTING</b> if device connect state is connecting;
     *         Returns <b>INVALID_STATUS</b> if target device is not in device list;
     * @since 6.0
     */
    int GetDeviceState(const RawAddress &device) const override;

    /**
     * @brief Get device playing state by address when target device is on connected.
     *
     * @param device The address of the peer bluetooth device.
     * @return Returns <b>1</b> if device is on playing;
     *         Returns <b>0</b> if device is not on playing;
     * @since 6.0
     */
    int GetPlayingState(const RawAddress &device) const override;

    /**
     * @brief Set target device as active device.
     *
     * @param device The address of the peer bluetooth device.
     * @return Returns <b>RET_NO_ERROR</b> Target device has already been active, or perform normal setting processing.
     *         Returns <b>RET_BAD_STATUS</b> Target device is not on connected, or not in device list.
     * @since 6.0
     */
    int SetActiveSinkDevice(const RawAddress &device) override;

    /**
     * @brief Get active device.
     * @return Returns active device.
     * @since 6.0
     */
    const RawAddress &GetActiveSinkDevice() const override;

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
    int SetConnectStrategy(const RawAddress &device, int strategy) override;

    /**
     * @brief Get connection strategy of peer bluetooth device.
     *
     * @param device The address of the peer bluetooth device.
     * @return Returns <b>CONNECTION_ALLOWED</b> if the peer device is allowed to connect.
     *         Returns <b>CONNECTION_FORBIDDEN</b> if the peer device is not allowed to connect.
     *         Returns <b>CONNECTION_UNKNOWN</b> if the connection policy is unknown.
     * @since 6.0
     */
    int GetConnectStrategy(const RawAddress &device) const override;

    /**
     * @brief Send delay reporting.
     *
     * @param device The address of the peer bluetooth device.
     * @param delayValue The delay value.
     * @return Returns <b>RET_NO_ERROR</b> if the operation is successful.
     *         Returns <b>RET_BAD_STATUS</b> if the operation fails.
     */
    int SendDelay(const RawAddress &device, uint16_t delayValue) override;

    /**
     * @brief Get codec status information of connected device.
     *
     * @param device The address of the bluetooth device.
     * @return Returns codec status information of connected device.
     * @since 6.0
     */
    A2dpSrcCodecStatus GetCodecStatus(const RawAddress &device) const override;

    /**
     * @brief Set the codec encoding preferences of the specified device.
     *
     * @param device The address of the bluetooth device.
     * @param info The codec encoding information.
     * @return Return the result setted.
     * @since 6.0
     */
    int SetCodecPreference(const RawAddress &device, const A2dpSrcCodecInfo &info) override;

    /**
     * @brief Set whether the optional codec is valid.
     *
     * @param device The address of the bluetooth device.
     * @param isEnable Set true if the optional codec is valid.
     *                 Set false if the optional codec is invalid.
     * @since 6.0
     */
    void SwitchOptionalCodecs(const RawAddress &device, bool isEnable) override;

    /**
     * @brief Get whether the peer bluetooth device supports optional codec.
     *
     * @param device The address of the bluetooth device.
     * @return Returns <b>A2DP_OPTIONAL_SUPPORT</b> The device supports optional codec.
     *         Returns <b>A2DP_OPTIONAL_NOT_SUPPORT</b> The device doesn't support optional codec.
     *         Returns <b>A2DP_OPTIONAL_SUPPORT_UNKNOWN</b> Don't know if the device support optional codec.
     * @since 6.0
     */
    int GetOptionalCodecsSupportState(const RawAddress &device) const override;

    /**
     * @brief Audio start streaming.
     *
     * @param device The address of the bluetooth device.
     * @return Returns <b>RET_NO_ERROR</b> if the operation is successful.
     *         Returns <b>RET_BAD_STATUS</b> if the operation fails, or device is not in device list.
     * @since 6.0
     */
    int StartPlaying(const RawAddress &device) override;

    /**
     * @brief Audio suspend streaming.
     *
     * @param device The address of the bluetooth device.
     * @return Returns <b>RET_NO_ERROR</b> if the operation is successful.
     *         Returns <b>RET_BAD_STATUS</b> if the operation fails, or device is not in device list.
     * @since 6.0
     */
    int SuspendPlaying(const RawAddress &device) override;

    /**
     * @brief Audio stop streaming.
     *
     * @param device The address of the bluetooth device.
     * @return Returns <b>RET_NO_ERROR</b> if the operation is successful.
     *         Returns <b>RET_BAD_STATUS</b> if the operation fails, or device is not in device list.
     * @since 6.0
     */
    int StopPlaying(const RawAddress &device) override;

    /**
     * @brief Register observer function of framework.
     *
     * @param observer The observer function pointer of framework.
     * @since 6.0
     */
    void RegisterObserver(IA2dpObserver *observer) override;

    /**
     * @brief Deregister observer function of framework.
     *
     * @since 6.0
     */
    void DeregisterObserver(IA2dpObserver *observer) override;

    /**
     * @brief Write PCM data to A2dp Service
     * @param[in] data is the address of the input data
     * @param[in] size is the size of the input data
     * @since 6.0
     */
    int WriteFrame(const uint8_t *data, uint32_t size) override;

    /**
     * @brief Get the information of the current rendered position.
     * @param[out] dalayValue is the delayed time
     * @param[out] sendDataSize is the data size that has been sent
     * @param[out] timeStamp is the current time stamp
     * @since 6.0
     */
    void GetRenderPosition(uint16_t &delayValue, uint16_t &sendDataSize, uint32_t &timeStamp) override;
    /**
     * @brief Get boject pointer of A2dpConnectManager.
     *
     * @return Returns boject pointer of A2dpConnectManager.
     * @since 6.0
     */
    A2dpConnectManager ConnectManager() const;

    /**
     * @brief Process connect callback function pointer of framework.
     * @param state The connection state of the bluetooth device.
     * @param device The address of the bluetooth device.
     * @since 6.0
     */
    void ProcessConnectFrameworkCallback(int state, const RawAddress &device);

    /**
     * @brief Process playing callback function pointer of framework.
     * @param playingState The new playing state of the bluetooth device.
     * @param device The address of the bluetooth device.
     * @param error The playing error state of the bluetooth device.
     * @since 6.0
     */
    void ProcessPlayingFrameworkCallback(int playingState, int error, const RawAddress &device);

    /**
     * @brief Process codec callback function pointer of framework.
     * @param info The new codec config information of the bluetooth device.
     * @param device The address of the bluetooth device.
     * @param error The codec error state of the bluetooth device.
     * @since 6.0
     */
    void ProcessCodecFrameworkCallback(const bluetooth::A2dpSrcCodecInfo &info, int error, const RawAddress &device);

    /**
     * @brief Get device information from device list by device address.
     *
     * @param device The address of the bluetooth device.
     * @return Returns device information from device list.
     * @since 6.0
     */
    A2dpDeviceInfo *GetDeviceFromList(const RawAddress &device);

    /**
     * @brief Get device list that saves devices information.
     * @return Returns device list.
     * @since 6.0
     */
    std::map<std::string, A2dpDeviceInfo *> GetDeviceList() const;

    /**
     * @brief Add device information to device list.
     *
     * @param address The address of the bluetooth device.
     * @param deviceInfo The device information.
     * @since 6.0
     */
    void AddDeviceToList(std::string address, A2dpDeviceInfo *deviceInfo);

    /**
     * @brief Delete device information from device list.
     *
     * @param device The address of the bluetooth device.
     * @since 6.0
     */
    void DeleteDeviceFromList(const RawAddress &device);

    /**
     * @brief : Post the events.
     *
     * @param event : The event of the a2dp
     * @since 6.0
     */
    void PostEvent(utility::Message event, RawAddress &device);

    /**
     * @brief Process the events.
     *
     * @param event : The event of the a2dp
     * @since 6.0
     */
    void ProcessEvent(utility::Message event, RawAddress &device);

    /**
     * @brief Update active device
     *
     * @param device The address of the peer device.
     * @since 6.0
     */
    void UpdateActiveDevice(const RawAddress &device);

    /**
     * @brief Update optional codec status.
     *
     * @param device The address of the bluetooth device.
     * @since 6.0
     */
    void UpdateOptCodecStatus(const RawAddress &device);

    /**
     * @brief Check if do disable, and clear device list when doing disable.
     *
     * @since 6.0
     */
    void CheckDisable();

    /**
     * @brief  Activate remote device
     *
     * @since 6.0
     */
    void ActiveDevice();

private:
    /**
     * @brief Process the events.
     *
     * @param event : The event of the a2dp
     * @since 6.0
     */
    void ProcessMessage(const utility::Message &msg) const;

    /**
     * @brief Enable the service.
     *
     * @since 6.0
     */
    void EnableService();

    /**
     * @brief Disable the service
     *
     * @since 6.0
     */
    void DisableService();

    /**
     * @brief Process profile's callback information.
     *
     * @param addr The address of the bluetooth device.
     * @param message The message from avdtp
     * @since 6.0
     */
    void ProcessAvdtpCallback(const BtAddr &addr, utility::Message &message) const;

    /**
     * @brief Process profile's  timerout callback information.
     *
     * @param role The role of the bluetooth profile.
     * @since 6.0
     */
    void ProcessTimeoutCallback(uint8_t role, const BtAddr &addr) const;

    /**
     * @brief Process profile's callback information.
     *
     * @param addr The address of the bluetooth device.
     * @param context The pointer of profile.
     * @since 6.0
     */
    void ProcessSDPFindCallback(const BtAddr &addr, const uint8_t result, A2dpProfile *instance) const;

    /**
     * @brief Set whether the peer bluetooth device supports optional codec.
     *
     * @param device The address of the bluetooth device.
     * @param Returns <b>A2DP_OPTIONAL_SUPPORT</b> The device supports optional codec.
     *         Returns <b>A2DP_OPTIONAL_NOT_SUPPORT</b> The device doesn't support optional codec.
     *         Returns <b>A2DP_OPTIONAL_SUPPORT_UNKNOWN</b> Don't know if the device support optional codec.
     * @since 6.0
     */
    void SetOptionalCodecsSupportState(const RawAddress &device, int state);

    /**
     * @brief Find the state matched
     *
     * @param states The states searched
     * @param connectState The state of device connected
     * @return Returns <b>true</b> if the codec configuration is valid.
     *         Returns <b>false</b> if the codec configuration is invalid.
     * @since 6.0
     */
    bool FindStateMatched(std::vector<int> states, int connectState) const;

    /**
     * @brief Check if other codec configuration is similar with current configuration.
     *
     * @param codecInfo  codec configuration saved.
     * @param newInfo  Another codec configuration.
     * @return Returns <b>true</b> if other codec configuration is similar with current configuration.
     *         Returns <b>false</b> if other codec configuration is not similar with current configuration.
     * @since 6.0
     */
    bool IsSimilarCodecConfig(A2dpSrcCodecInfo codecInfo, A2dpSrcCodecInfo newInfo) const;

    /**
     * @brief Check if the codec information matches local information.
     *
     * @param codecInfo The codec information.
     * @param codecStatus The codec status information of local device.
     * @return Returns <b>true</b> codec information matches confirmed information.
     *         Returns <b>false</b> codec information doesn't match local information.
     * @since 6.0
     */
    bool IsLocalCodecInfo(A2dpSrcCodecStatus codecStatus, A2dpSrcCodecInfo codecInformation) const;

    int maxConnectNumSnk_ = A2DP_CONNECT_NUM_MAX;             // max number of connected peer device
    uint8_t role_ = A2DP_ROLE_SOURCE;                         // A2DP role
    uint32_t profileId_ = PROFILE_ID_A2DP_SRC;                // profile ID
    RawAddress activeDevice_ {};                               // Address to active device
    A2dpConnectManager connectManager_ {};                     // A2dpConnectManager's object pointer.
    ObserverProfile profileObserver_ {};                       // Observer pointer of profile.
    std::string name_ = "";                                   // service name
    std::string version_ = "";                                // profile's version
    std::map<std::string, A2dpDeviceInfo *> a2dpDevices_ {};   // devicelist
    BaseObserverList<IA2dpObserver> a2dpFramworkCallback_ {};  // callback of framework.
    bool isDoDisable = false;                                 // if device will disable
};

/**
 * @brief A function to get profile instance
 *
 * @param[in] role The role of profile
 * @since 6.0
 */
A2dpProfile *GetProfileInstance(uint8_t role);

/**
 * @brief A function to get profile instance
 *
 * @param[in] role The role of profile
 * @since 6.0
 */
A2dpService *GetServiceInstance(uint8_t role);
}  // namespace bluetooth
}  // namespace OHOS
#endif  // A2DP_SERVICE_H