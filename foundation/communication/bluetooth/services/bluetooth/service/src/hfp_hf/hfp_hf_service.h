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
 * @addtogroup Bluetooth
 * @{
 *
 * @brief Defines a bluetooth system that provides basic bluetooth connection and profile functions,
 *        including A2DP, AVRCP, BLE, GATT, HFP, MAP, PBAP, and SPP, etc.
 *
 * @since 6
 */

/**
 * @file hfp_hf_service.h
 *
 * @brief Declares HFP AG role service functions, including basic and observer functions.
 *
 * @since 6
 */

#ifndef HFP_HF_SERVICE_H
#define HFP_HF_SERVICE_H

#include <optional>

#include "context.h"
#include "hfp_hf_message.h"
#include "hfp_hf_statemachine.h"
#include "interface_profile_hfp_hf.h"
#include "message.h"
#include "raw_address.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief This HFP class provides a set of methods that is called by Framework API, and manage
 *        the state machine and the message queue.
 *
 * @since 6
 */
class HfpHfService : public IProfileHfpHf, public utility::Context {
public:
    /**
     * @brief Get the instance of the HfpHfService object.
     *
     * @return Returns the instance of the HfpHfService object.
     * @since 6
     */
    static HfpHfService *GetService();

    /**
     * @brief Construct a new HfpHfService object.
     *
     * @since 6
     */
    HfpHfService();

    /**
     * @brief Destroy the HfpHfService object.
     *
     * @since 6
     */
    ~HfpHfService();

    /**
     * @brief Get the instance of the HfpAgService object.
     *
     * @return Returns the instance of the HfpAgService object.
     * @since 6
     */
    utility::Context *GetContext() override;

    /**
     * @brief Enable the target service.
     *
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    void Enable() override;

    /**
     * @brief Disable the target service.
     *
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    void Disable() override;

    /**
     * @brief Finish the shut down service.
     *
     * @since 6
     */
    void ShutDownDone(bool isAllDisconnected);

    /**
     * @brief Establish the service level connection.
     *
     * @param device The remote device.
     * @return Returns the connect result code.
     * @since 6
     */
    int Connect(const RawAddress &device) override;

    /**
     * @brief Release the service level connection.
     *
     * @param device The remote device.
     * @return Returns the disconnect result code.
     * @since 6
     */
    int Disconnect(const RawAddress &device) override;

    /**
     * @brief Get the connected devices list.
     *
     * @return Returns the connected devices list.
     * @since 6
     */
    std::list<RawAddress> GetConnectDevices() override;

    /**
     * @brief Get the connection state.
     *
     * @return Returns the connection state.
     * @since 6
     */
    int GetConnectState() override;

    /**
     * @brief Get the maximum number of connected devices.
     *
     * @return Returns the max connected devices number.
     * @since 6
     */
    int GetMaxConnectNum() override;

    /**
     * @brief Establish the audio connection.
     *
     * @param device The remote device.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool ConnectSco(const RawAddress &device) override;

    /**
     * @brief Release the audio connection.
     *
     * @param device The remote device.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool DisconnectSco(const RawAddress &device) override;

    /**
     * @brief Get the devices list of the specified states.
     *
     * @param states The specified states.
     * @return Returns the remote devices list of the specified states.
     * @since 6
     */
    std::vector<RawAddress> GetDevicesByStates(std::vector<int> states) override;

    /**
     * @brief Get the state by the device.
     *
     * @param device: The remote device.
     * @return Returns the connection state.
     * @since 6
     */
    int GetDeviceState(const RawAddress &device) override;

    /**
     * @brief Get the audio state of the specified device.
     *
     * @param device The remote device.
     * @return Returns the audio state.
     * @since 6
     */
    int GetScoState(const RawAddress &device) override;

    /**
     * @brief Send DTMF tone code to remote AG device.
     *
     * @param device Remote device object.
     * @param code DTMF tone code.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool SendDTMFTone(const RawAddress &device, uint8_t code) override;

    /**
     * @brief Get a list of current all existing calls.
     *
     * @param device Remote device object.
     * @return The list of all existing calls.
     * @since 6
     */
    std::vector<HandsFreeUnitCalls> GetCurrentCallList(const RawAddress &device) override;

    /**
     * @brief Accept an incoming call.
     *
     * @param device Remote device object.
     * @param flag Types of calls accepted.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool AcceptIncomingCall(const RawAddress &device, int flag) override;

    /**
     * @brief Hold an active call.
     *
     * @param device Remote device object.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool HoldActiveCall(const RawAddress &device) override;

    /**
     * @brief Reject an incoming call.
     *
     * @param device Remote device object.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool RejectIncomingCall(const RawAddress &device) override;

    /**
     * @brief Send key pressed event.
     *
     * @param device Remote device object.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 9
     */
    bool SendKeyPressed(const RawAddress &device) override;

    /**
     * @brief handle an incoming call.
     *
     * @param device Remote device object.
     * @param flag Types of incoming calls handle.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 9
     */
    bool HandleIncomingCall(const RawAddress &device, int flag) override;

    /**
     * @brief handle multi call.
     *
     * @param device Remote device object.
     * @param flag Types of incoming calls handle.
     * @param index call index
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 9
     */
    bool HandleMultiCall(const RawAddress &device, int flag, int index) override;

    /**
     * @brief Dial last number.
     *
     * @param device Remote device object.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 9
     */
    bool DialLastNumber(const RawAddress &device) override;

    /**
     * @brief Dial memory.
     *
     * @param device Remote device object.
     * @param index Memory index.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 9
     */
    bool DialMemory(const RawAddress &device, int index) override;

    /**
     * @brief Send voice tag.
     *
     * @param device Remote device object.
     * @param index voice tag index.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 9
     */
    bool SendVoiceTag(const RawAddress &device, int index) override;

    /**
     * @brief Finish a specified active call.
     *
     * @param device Remote device object.
     * @param call Call object.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool FinishActiveCall(const RawAddress &device, const HandsFreeUnitCalls &call) override;

    /**
     * @brief Start dial with specified call number.
     *
     * @param device Remote device object.
     * @param number Call's number to dial.
     * @return The object of dial out call or nullopt.
     * @since 6
     */
    std::optional<HandsFreeUnitCalls> StartDial(const RawAddress &device, const std::string &number) override;

    /**
     * @brief Register HfpHfServiceObserver instance.
     *
     * @param observer HfpHfServiceObserver instance.
     * @since 6
     */
    void RegisterObserver(HfpHfServiceObserver &observer) override;

    /**
     * @brief Deregister HfpHfServiceObserver instance.
     *
     * @param observer HfpHfServiceObserver instance.
     * @since 6
     */
    void DeregisterObserver(HfpHfServiceObserver &observer) override;

    /**
     * @brief Set the Hf Volume.
     *
     * @param volume The value of the volume.
     * @param type The type of the volume.
     * @since 6
     */
    void SetHfVolume(int volume, int type);

    /**
     * @brief Open voice recognition.
     *
     * @param device The remote device.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool OpenVoiceRecognition(const RawAddress &device) override;

    /**
     * @brief Close voice recognition.
     *
     * @param device The remote device.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool CloseVoiceRecognition(const RawAddress &device) override;

    /**
     * @brief Send the event of the HFP HF role.
     *
     * @param event The event of the HFP HF role.
     * @since 6
     */
    void PostEvent(const HfpHfMessage &event);

    /**
     * @brief Process the event of the HFP HF role.
     *
     * @param event The event of the HFP HF role.
     * @since 6
     */
    void ProcessEvent(const HfpHfMessage &event);

    /**
     * @brief Notify the HF battery level change.
     *
     * @param batteryLevel The value of the battery level.
     * @since 6
     */
    void BatteryLevelChanged(int batteryLevel);

    /**
     * @brief Notify the HF enhanced driver safety change.
     *
     * @param state The state of the enhanced driver safety
     * @since 6
     */
    void EnhancedDriverSafety(int state);

    /**
     * @brief Notify the connection state change.
     *
     * @param device The remote device.
     * @param state The state of the connection.
     * @since 6
     */
    void NotifyStateChanged(const RawAddress &device, int state);

    /**
     * @brief Notify the audio connection state change.
     *
     * @param device The remote device.
     * @param state The state of the audio connection.
     * @since 6
     */
    void NotifyScoStateChanged(const RawAddress &device, int state);

    /**
     * @brief Notify the call change.
     *
     * @param device The remote device.
     * @param call The changed call.
     * @since 6
     */
    void NotifyCallChanged(const RawAddress &device, const HandsFreeUnitCalls &call);

    /**
     * @brief Notify the battery level change.
     *
     * @param device The remote device.
     * @param batteryLevel The battery level.
     * @since 6
     */
    void NotifyBatteryLevelChanged(const RawAddress &device, int batteryLevel);

    /**
     * @brief Notify the signal strength change.
     *
     * @param device The remote device.
     * @param signal The signal strength.
     * @since 6
     */
    void NotifySignalStrengthChanged(const RawAddress &device, int signal);

    /**
     * @brief Notify the registration status change.
     *
     * @param device The remote device.
     * @param status The registration status.
     * @since 6
     */
    void NotifyRegistrationStatusChanged(const RawAddress &device, int status);

    /**
     * @brief Notify the roam status change.
     *
     * @param device The remote device.
     * @param status The roam state.
     * @since 6
     */
    void NotifyRoamingStatusChanged(const RawAddress &device, int status);

    /**
     * @brief Notify the network operator name change.
     *
     * @param device The remote device.
     * @param name The network operator name.
     * @since 6
     */
    void NotifyOperatorSelectionChanged(const RawAddress &device, const std::string &name);

    /**
     * @brief Notify the subscriber number change.
     *
     * @param device : The remote device.
     * @param number The subscriber number.
     * @since 6
     */
    void NotifySubscriberNumberChanged(const RawAddress &device, const std::string &number);

    /**
     * @brief Notify the voice recognition state change.
     *
     * @param device : The remote device.
     * @param status : The voice recognition state.
     * @since 6
     */
    void NotifyVoiceRecognitionStatusChanged(const RawAddress &device, int status);

    /**
     * @brief Notify the inBand ring tone.
     *
     * @param device : The remote device.
     * @param status : The status of the inBand ring.
     * @since 6
     */
    void NotifyInBandRingTone(const RawAddress &device, int status);

    /**
     * @brief Send the AT command.
     * @param device : The remote device.
     * @param cmdId : The id of the at command
     * @param arg : The string of the at command
     * @since 6
     */
    void SendAtCommand(const RawAddress &device, int cmdId, const std::string &arg);

private:
    /**
     * @brief Service startup.
     *
     * @since 6
     */
    void StartUp();

    /**
     * @brief Service shutdown.
     *
     * @since 6
     */
    void ShutDown();

    /**
     * @brief Get the connected and connecting device number.
     *
     * @return Returns the number of the Connected Device.
     * @since 6
     */
    int GetConnectionsDeviceNum() const;

    /**
     * @brief Get the max connection devices number.
     *
     * @return Returns the max connection devices number.
     * @since 6
     */
    int GetMaxConnectionsDeviceNum() const;

    /**
     * @brief Check whether device is connected.
     *
     * @param address: The remote device address.
     * @return Returns <b>true</b> if the is connected; returns <b>false</b> if not connected.
     * @since 6
     */
    bool IsConnected(const std::string &address) const;

    /**
     * @brief Is the sco connected.
     *
     * @return Returns if the audio is connected or not.
     * @since 6
     */
    bool IsScoConnected() const;

    /**
     * @brief Send the event to each state machine.
     *
     * @param event The event of the HFP.
     * @since 6
     */
    void SendEventToEachStateMachine(const HfpHfMessage &event) const;

    /**
     * @brief Process connect or connect request event of the HFP HF role.
     *
     * @param event The connect or connect request event.
     * @since 6
     */
    void ProcessConnectEvent(const HfpHfMessage &event);

    /**
     * @brief Process remove state machine event of the HFP HF role.
     *
     * @param address The remote device address.
     * @since 6
     */
    void ProcessRemoveStateMachine(const std::string &address);

    /**
     * @brief Process default event of the HFP HF role.
     *
     * @param event The default event.
     * @since 6
     */
    void ProcessDefaultEvent(const HfpHfMessage &event) const;

    //  service status
    bool isStarted_ {false};

    //  service status
    bool isShuttingDown_ {false};

    // the maximum number of connection devices.
    static inline constexpr int MAX_CONNECTIONS_NUM {6};

    // the maximum number of connection devices.
    int maxConnectionsNum_ {MAX_CONNECTIONS_NUM};

    // the map of the device and sate machine
    std::map<const std::string, std::unique_ptr<HfpHfStateMachine>> stateMachines_ {};

    // the mutex variable
    std::recursive_mutex mutex_ {};

    // The observer list.
    std::list<HfpHfServiceObserver *> observers_ {};

    // const state map
    const std::map<const int, const int> stateMap_ = {
        {HFP_HF_STATE_DISCONNECTED, static_cast<int>(BTConnectState::DISCONNECTED)},
        {HFP_HF_STATE_CONNECTING, static_cast<int>(BTConnectState::CONNECTING)},
        {HFP_HF_STATE_DISCONNECTING, static_cast<int>(BTConnectState::DISCONNECTING)},
        {HFP_HF_STATE_CONNECTED, static_cast<int>(BTConnectState::CONNECTED)}
    };

    BT_DISALLOW_COPY_AND_ASSIGN(HfpHfService);
    DECLARE_IMPL();
};
}  // namespace bluetooth
}  // namespace OHOS
#endif // HFP_HF_SERVICE_H