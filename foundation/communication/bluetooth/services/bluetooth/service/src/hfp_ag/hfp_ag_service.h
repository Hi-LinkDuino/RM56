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
 * @file hfp_ag_service.h
 *
 * @brief Declares HFP AG role service functions, including basic and observer functions.
 *
 * @since 6
 */

#ifndef HFP_AG_SERVICE_H
#define HFP_AG_SERVICE_H

#include <cstdint>
#include <map>
#include <memory>
#include <mutex>

#include "base_def.h"
#include "context.h"
#include "hfp_ag_message.h"
#include "hfp_ag_statemachine.h"
#include "interface_profile_hfp_ag.h"
#include "message.h"
#include "raw_address.h"
#include "timer.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief This HFP class provides a set of methods that is called by Framework API, and manage
 *        the state machine and the message queue.
 *
 * @since 6
 */
class HfpAgService : public IProfileHfpAg, public utility::Context {
public:
    /**
     * @brief Construct a new HfpAgService object.
     *
     * @since 6
     */
    HfpAgService();

    /**
     * @brief Destroy the HfpAgService object.
     *
     * @since 6
     */
    ~HfpAgService();

    /**
     * @brief Get the instance of the HfpAgService object.
     *
     * @return Returns the instance of the HfpAgService object.
     * @since 6
     */
    static HfpAgService *GetService();

    /**
     * @brief Get the Context object.
     *
     * @return Returns the Context object.
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
     * @param device The remote device
     * @return Returns the connect result code.
     * @since 6
     */
    int Connect(const RawAddress &device) override;

    /**
     * @brief Release the service level connection.
     *
     * @param device The remote device
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
     * @brief Get the state by the device.
     *
     * @param device The remote device.
     * @return Returns the connection state.
     * @since 6
     */
    int GetDeviceState(const RawAddress &device) override;

    /**
     * @brief Get the maximum number of connected devices.
     *
     * @return Returns the max connected devices number.
     * @since 6
     */
    int GetMaxConnectNum() override;

    /**
     * @brief Get the devices list of the specified states.
     *
     * @param states The specified states.
     * @return Returns the remote devices list of the specified states.
     * @since 6
     */
    std::vector<RawAddress> GetDevicesByStates(std::vector<int> states) override;

    /**
     * @brief Establish the audio connection.
     *
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool ConnectSco() override;

    /**
     * @brief Release the audio connection.
     *
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool DisconnectSco() override;

    /**
     * @brief Get the audio state of the specified device.
     *
     * @param device The remote device.
     * @return Returns the audio state.
     * @since 6
     */
    int GetScoState(const RawAddress &device) override;

    void UpdateMockCallList(int callState, const std::string &number, int type);

    /**
     * @brief This function used to Update changed phone call information.
     *
     * @param numActive Active call number.
     * @param numHeld Held call number.
     * @param callState Current call state.
     * @param number Phone call number.
     * @param type Type of phone call number.
     * @param name Name of phone call number.
     * @since 6
     */
    void PhoneStateChanged(int numActive, int numHeld, int callState, const std::string &number, int type,
        const std::string &name) override;

    /**
     * @brief Send response for querying standard list current calls by remote Hf device.
     *
     * @param index Index of the current call.
     * @param direction Direction of the current call.
     * @param status Status of the current call.
     * @param mode Source Mode of the current call.
     * @param mpty Is this call a member of a conference call.
     * @param number Phone call number.
     * @param type Type of phone call number.
     * @since 6
     */
    void ClccResponse(
        int index, int direction, int status, int mode, bool mpty, const std::string &number, int type) override;

    /**
     * @brief Open the voice recognition.
     *
     * @param device The Remote device.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool OpenVoiceRecognition(const RawAddress &device) override;

    /**
     * @brief Close the voice recognition.
     *
     * @param device The Remote device.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool CloseVoiceRecognition(const RawAddress &device) override;

    /**
     * @brief Set the active device for audio connection.
     *
     * @param device The remote device.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool SetActiveDevice(const RawAddress &device) override;

    bool IntoMock(int state) override;
    bool SendNoCarrier(const RawAddress &device) override;
    int GetMockState();
    std::vector<MockCall> GetCallList();

    /**
     * @brief Get the active device.
     *
     * @param device The remote active device.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    std::string GetActiveDevice() override;

    /**
     * @brief Notify the slc state change.
     *
     * @param device The remote device.
     * @param toState The current state.
     * @since 6
     */
    void NotifySlcStateChanged(const RawAddress &device, int toState);

    /**
     * @brief Notify the audio state change.
     *
     * @param device The remote device.
     * @param toState The current state.
     * @since 6
     */
    void NotifyAudioStateChanged(const RawAddress &device, int toState);

    /**
     * @brief Notify the active device change.
     *
     * @param device The active device.
     * @since 6
     */
    void NotifyCurrentActiveDevice(const RawAddress &device);

    /**
     * @brief Notify the value of the HF enhanced driver safety.
     *
     * @param device The bluetooth device address.
     * @param indValue The value of the HF enhanced driver safety.
     * @since 6
     */
    void NotifyHfEnhancedDriverSafety(const RawAddress &device, int indValue);

    /**
     * @brief Notify the value of the HF battery level.
     *
     * @param device The bluetooth device address.
     * @param indValue The value of the HF battery level.
     * @since 6
     */
    void NotifyHfBatteryLevel(const RawAddress &device, int indValue);

    /**
     * @brief Register HfpAgServiceObserver instance.
     *
     * @param observer HfpAgServiceObserver instance.
     * @since 6
     */
    void RegisterObserver(HfpAgServiceObserver &observer) override;

    /**
     * @brief Deregister HfpAgServiceObserver instance.
     *
     * @param observer HfpAgServiceObserver instance.
     * @since 6
     */
    void DeregisterObserver(HfpAgServiceObserver &observer) override;

    /**
     * @brief Send the event of the HFP AG role.
     *
     * @param event The event of the HFP AG role.
     * @since 6
     */
    void PostEvent(const HfpAgMessage &event);

    /**
     * @brief Process the event of the HFP AG role.
     *
     * @param event The event of the HFP AG role.
     * @since 6
     */
    void ProcessEvent(const HfpAgMessage &event);

    /**
     * @brief Notify the ag indicator state changed.
     *
     * @param what The name of the event.
     * @param state the service state of the ag indicator
     * @since 6
     */
    void NotifyAgIndicatorStateChanged(int what, int state);

    /**
     * @brief Notify the ag incoming state changed.
     *
     * @param what The name of the event.
     * @param number incoming number.
     * @param type call type
     * @since 9
     */
    void NotifyAgIncomingStateChanged(int what, std::string number, int type);

    void SendMockCmd(const HfpAgMessage &event);

    /**
     * @brief Notify the ag call response hold state changed.
     *
     * @param what The name of the event.
     * @param state responsw hold state
     * @since 9
     */
    void NotifyAgResponseHoldStateChanged(int what, int state);

    /**
     * @brief Send binp bumber.
     *
     * @param number voice number.
     * @since 9
     */
    void SendBinpNumber(std::string number);

    /**
     * @brief Dial out call by HF.
     *
     * @param address The remote device address.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool DialOutCallByHf(const std::string &address);

    /**
     * @brief Dialing out call time out.
     *
     * @since 6
     */
    void DialOutCallTimeOut();

    /**
     * @brief Open the voice recognition by HF.
     *
     * @param address The remote device address.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool OpenVoiceRecognitionByHf(const std::string &address);

    /**
     * @brief Close the voice recognition by HF.
     *
     * @param address The remote device address.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool CloseVoiceRecognitionByHf(const std::string &address);

    /**
     * @brief Voice recognition time out.
     *
     * @since 6
     */
    void VoiceRecognitionTimeOut();

    /**
     * @brief Set the response CLCC timer.
     *
     * @param address The remote device address.
     * @since 6
     */
    void SetResponseClccTimer(const std::string &address);

    /**
     * @brief Responsing CLCC time out.
     *
     * @since 6
     */
    void ResponseClccTimeOut();

    /**
     * @brief Service level connection status change.
     *
     * @param address The remote device address.
     * @param toState The current state.
     * @since 6
     */
    void SlcStateChanged(const std::string &address, int toState);

    /**
     * @brief Audio State change.
     *
     * @param address The remote device address.
     * @param fromState The previous state.
     * @param toState The current state.
     * @since 6
     */
    void ScoStateChanged(const std::string &address, int fromState, int toState);

    /**
     * @brief Check if it is inbang ringing.
     *
     * @return Returns if is inBanding ring or not.
     * @since 6
     */
    bool IsInbandRinging();

    /**
     * @brief Notify the status of the registration.
     *
     * @param status the status of the registration.
     * @since 6
     */
    void NotifyRegistrationStatusChanged(int status);

    /**
     * @brief Notify the signal strength.
     *
     * @param signalStrength The signal strength.
     * @since 6
     */
    void NotifySignalStrengthChanged(int signalStrength);

    /**
     * @brief Notify the roam status.
     *
     * @param status The roam status.
     * @since 6
     */
    void NotifyRoamingStatusChanged(int status);

    /**
     * @brief Notify the battery level.
     *
     * @param batteryLevel The battery level.
     * @since 6
     */
    void NotifyBatteryLevelChanged(int batteryLevel);

    /**
     * @brief Notify the volume change.
     *
     * @param type The type of the volume
     * @param volume The value of the volume.
     * @since 6
     */
    void NotifyVolumeChanged(int type, int volume);

    /**
     * @brief Set in-band ring.
     * @param action If <b>true</b> set in-band ring enabled, else disabled.
     * @since 6
     */
    void SetInbandRing(bool action);

    /**
     * @brief respones ok to hf.
     * @since 9
     */
    void ResponesOK(const std::string &address);

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
     * @brief Release single audio connection.
     *
     * @param device The remote device address.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool DisconnectSingleSco(const std::string &address);

    /**
     * @brief Get the connected and connecting device number.
     *
     * @return Returns the number of the Connected Device.
     * @since 6
     */
    int GetConnectedDeviceNum() const;

    /**
     * @brief Get the max connection devices number.
     *
     * @return Returns the max connection devices number.
     * @since 6
     */
    int GetMaxConnectionDevicesNum() const;

    /**
     * @brief Check whether device is connected.
     *
     * @param address: The remote device address.
     * @return Returns <b>true</b> if the is connected; returns <b>false</b> if not connected.
     * @since 6
     */
    bool IsConnected(const std::string &address) const;

    /**
     * @brief Clear active device.
     *
     * @since 6
     */
    void ClearActiveDevice();

    /**
     * @brief Check whether device is active device.
     *
     * @param address: The remote device address.
     * @return Returns <b>true</b> if device is active device; returns <b>false</b> if not.
     * @since 6
     */
    bool IsActiveDevice(const std::string &address) const;

    /**
     * @brief Modify active device.
     *
     * @param address: new active device address.
     * @since 6
     */
    void ModifyActiveDevice(const std::string &newAddress);

    /**
     * @brief Check whether audio is connected.
     *
     * @return Returns true if the audio is connected else return false.
     * @return Returns <b>true</b> if the audio is connected; returns <b>false</b> if audio is not connected.
     * @since 6
     */
    bool IsAudioConnected() const;

    /**
     * @brief Check if audio is idle.
     *
     * @return Returns <b>true</b> if the audio is idle; returns <b>false</b> if audio is not idle.
     * @since 6
     */
    bool IsAudioIdle() const;

    /**
     * @brief Check if sco is used by call.
     *
     * @return Returns <b>true</b> if sco used by call; returns <b>false</b> if not.
     * @since 6
     */
    bool IsIncall();

    /**
     * @brief Send the event to each device's state machine.
     *
     * @param event The event of the HFP AG role.
     * @since 6
     */
    void SendEventToEachStateMachine(const HfpAgMessage &event) const;

    /**
     * @brief Update ag indicators.
     *
     * @param event The event of the HFP AG role.
     * @since 6
     */
    void UpdateAgIndicators() const;

    /**
     * @brief Process connect or connect request event of the HFP AG role.
     *
     * @param event The connect or connect request event.
     * @since 6
     */
    void ProcessConnectEvent(const HfpAgMessage &event);

    /**
     * @brief Process remove state machine event of the HFP AG role.
     *
     * @param address The remote device address.
     * @since 6
     */
    void ProcessRemoveStateMachine(const std::string &address);

    /**
     * @brief Process default event of the HFP AG role.
     *
     * @param event The default event.
     * @since 6
     */
    void ProcessDefaultEvent(const HfpAgMessage &event) const;

    /**
     * @brief Process voice recognition time out event of the HFP AG role.
     *
     * @param event The voice recognition time out event.
     * @since 6
     */
    void ProcessVoiceRecognitionTimeOut(const HfpAgMessage &event);

    /**
     * @brief Process response CLCC time out event of the HFP AG role.
     *
     * @param event The response CLCC time out event.
     * @since 6
     */
    void ProcessResponseClccTimeOut(const HfpAgMessage &event);

    /**
     * @brief Check if voice recognition available.
     *
     * @param address Open voice recognition device address.
     * @return Returns if voice recognition should to be open.
     * @since 6
     */
    bool IsVoiceRecognitionAvailable(const std::string &address) const;

    /**
     * @brief Process response dial time out event of the HFP AG role.
     *
     * @since 6
     */
    void ProcessDialOutTimeOut();

    /**
     * @brief Open the voice recognition internal
     *
     * @param device The Remote device address.
     * @param isRequestByHf true if VR request by hf, else false.
     * @since 6
     */
    void SendOpenVoiceEvent(const std::string &address, bool isRequestByHf);

    /**
     * @brief Close the voice recognition internal
     *
     * @param device The Remote device address.
     * @since 6
     */
    void SendCloseVoiceEvent(const std::string &address);

    // service status
    bool isStarted_ {false};

    // service status
    bool isShuttingDown_ {false};

    // The maximum default number of connection devices.
    inline static constexpr int MAX_DEFAULT_CONNECTIONS_NUM {6};

    // The maximum number of connected devices.
    int maxConnectedNum_ {MAX_DEFAULT_CONNECTIONS_NUM};

    // The time of dialing out timeout
    inline static constexpr int DIALING_OUT_TIMEOUT_TIME {10000};

    // The time of voice recognition timeout
    inline static constexpr int VOICE_RECOGNITION_TIMEOUT_TIME {5000};

    // The time of response clcc timeout
    inline static constexpr int RESPONSE_CLCC_TIMEOUT_TIME {5000};

    // The empty address
    inline static const std::string NULL_ADDRESS {""};

    // The mutex variable
    std::recursive_mutex mutex_ {};

    // The flag for Voice recognition opened.
    bool isVrOpened_ {false};

    // The map of the device and sate machine.
    std::map<const std::string, std::unique_ptr<HfpAgStateMachine>> stateMachines_ {};

    // The list of the observer pointer.
    std::list<HfpAgServiceObserver *> observers_ {};

    // dialing out time out.
    std::unique_ptr<utility::Timer> dialingOutTimeout_ {nullptr};

    // voice recognition time out.
    std::unique_ptr<utility::Timer> voiceRecognitionTimeout_ {nullptr};

    // Response Clcc time out.
    std::unique_ptr<utility::Timer> ResponseClccTimeout_ {nullptr};

    // The status of the inband ringing.
    bool isInbandRinging_ {false};

    // The device address of the querying clcc.
    std::string queryClccAddress_ {NULL_ADDRESS};

    // const state map
    const std::map<const int, const int> stateMap_ = {
        {HFP_AG_STATE_DISCONNECTED, static_cast<int>(BTConnectState::DISCONNECTED)},
        {HFP_AG_STATE_CONNECTING, static_cast<int>(BTConnectState::CONNECTING)},
        {HFP_AG_STATE_DISCONNECTING, static_cast<int>(BTConnectState::DISCONNECTING)},
        {HFP_AG_STATE_CONNECTED, static_cast<int>(BTConnectState::CONNECTED)}
    };

    int mockState_ {0};
    std::vector<MockCall> callList_;

    BT_DISALLOW_COPY_AND_ASSIGN(HfpAgService);
};
}  // namespace bluetooth
}  // namespace OHOS
#endif // HFP_AG_SERVICE_H