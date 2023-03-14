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

#ifndef HFP_AG_SYSTEM_INTERFACE_H
#define HFP_AG_SYSTEM_INTERFACE_H

#include <string>
#include "base_def.h"
#include "hfp_ag_defines.h"
#include "telephony_observer.h"
#include "hfp_ag_message.h"
#include "system_ability_status_change_stub.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief  Defines the interfaces that is called by HFP.
 */
class SystemAbilityStatusChange : public SystemAbilityStatusChangeStub {
public:
    void OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;
    void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;
};

class HfpAgSystemInterface {
public:
    /**
     * @brief Get the Instance of the HfpAgSystemInterface.
     *
     * @return Returns the Instance of the HfpAgSystemInterface.
     */
    static HfpAgSystemInterface &GetInstance();

    /**
     * @brief Starts the module of the System interface.
     */
    void Start();

    /**
     * @brief Stops the module of the System interface.
     */
    void Stop();

    /**
     * @brief Receive the command from HF and reject the incoming call.
     *
     * @param address The address of the bluetooth device.
     */
    void RejectCall(const std::string &address) const;

    /**
     * @brief Receive the command from HF and dial out the call with the number.
     *
     * @param address The address of the bluetooth Device
     * @param number The number of the call.
     */
    void DialOutCall(const std::string &address, const std::string &number) const;

    /**
     * @brief Receive the command from HF and hangup the call.
     *
     * @param address The address of the bluetooth Device
     */
    void HangupCall(const std::string &address) const;

    /**
     * @brief Receive the command from HF and answer the incoming call.
     *
     * @param address The address of the bluetooth Device
     */
    void AnswerCall(const std::string &address) const;

    /**
     * @brief Instructs Telecom to play the specified DTMF tone for the current foreground call.
     *
     * @param dtmf The code of the DTMF.
     * @param address The address of the bluetooth device.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     */
    bool SendDtmf(int dtmf, const std::string &address) const;

    /**
     * @brief Receives the command of HF and instructs the telecom to hold the current call.
     *
     * @param chld The value of the chld.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     */
    bool HoldCall(int chld) const;

    /**
     * @brief Receives the command of HF and get the name of the network operator from the telecom.
     *
     * @return Returns the name of the network operator.
     */
    std::string GetNetworkOperator();

    /**
     * @brief Receives the command of HF and get the number of the phone from the telecom.
     *
     * @return Returns the number of the phone.
     */
    std::string GetSubscriberNumber();

    /**
     * @brief Receives the command of HF and asks the telecom to respone the current list through the CLCC.
     *
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     */
    bool QueryCurrentCallsList(const std::string &address);

    /**
     * @brief Receives the command of HF and asks the telecom to respone the current call state.
     */
    void QueryPhoneState() const;

    /**
     * @brief Receives the command of HF and starts the voice recognition.
     *
     * @param address The address of the bluetooth Device
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     */
    bool StartVoiceRecognition(const std::string &address) const;

    /**
     * @brief Receives the command of HF and stops the voice recognition.
     *
     * @param address The address of the bluetooth Device
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     */
    bool StopVoiceRecogition(const std::string &address) const;

    /**
     * @brief Set the parameter to the audio.
     *
     * @param parameters Audio parameters.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     */
    bool SetAudioParameters(const std::string &parameters) const;

    /**
     * @brief Set the stream volume.
     *
     * @param streamType The type of the stream.
     * @param volume The value of the volume.
     * @param flag The flag of volume.
     */
    void SetStreamVolume(int streamType, int volume, int flag) const;

    /**
     * @brief Get the last dialed number.
     *
     * @return Returns the last dialed number.
     */
    std::string GetLastDialNumber();

    /**
     * @brief Get the service state object.
     *
     * @return Returns the value of the state.
     */
    int GetServiceState();

    /**
     * @brief Get the signal strength.
     *
     * @return Returns the value of the signal.
     */
    int GetSignalStrength();

    /**
     * @brief Get the roaming state.
     *
     * @return Returns the value of the roaming.
     */
    int GetRoamState();

    /**
     * @brief Get the battery level.
     *
     * @return Returns the value of the battery level.
     */
    int GetBatteryLevel();

    /**
     * @brief Notify ag indicator from the phone.
     */
    void QueryAgIndicator();

    /**
     * @brief Update ag indicator from the phone.
     */
    void UpdateAgIndicator() const;

    /**
     * @brief update vluetooth call list
     *
     */
    void UpdateCallList();

    /**
     * @brief Get the Call With State object
     *
     * @param state call state
     * @return int call id
     */
    int GetCallWithState(int state) const;

    /**
     * @brief Get voice numner.
     */
    void GetVoiceNumber();

    /**
     * @brief Get response hold state.
     */
    void GetResponseHoldState(std::string address);

    /**
     * @brief Get response hold state.
     */
    void SetResponseHoldState(std::string address, int btrh);

    /**
     * @brief Get response hold state.
     */
    void HandlePhoneStateMock(std::string number, int state, int type);

    /**
     * @brief Notify the subscription state changed.
     *
     * @param state The network availabel flag.
     */
    void OnSubscriptionStateChanged(int state);

    /**
     * @brief Notify the signal strength.
     *
     * @param signal The signal strength.(Always between 0 and 5)
     */
    void OnSignalStrengthChanged(int signal);

    /**
     * @brief Notify the roam state.
     *
     * @param state The phone roaming flag.
     */
    void OnRoamStateChanged(int state);

    /**
     * @brief Notify the battery level.
     *
     * @param level Battery level.(Ranges from 0 to 5)
     */
    void OnBatteryLevel(int level);

    /**
     * @brief Send the hf indicator.
     *
     * @param address The bluetooth device address.
     * @param indId The id of the indicator.
     * @param indValue The value of the indicator.
     */
    void SendHfIndicator(const std::string &address, int indId, int indValue) const;

    /**
     * @brief Set the active calls Number.
     *
     * @param number The number of active calls.
     */
    void SetActiveCallNumber(int number);

    /**
     * @brief Get the active calls number.
     *
     * @return Returns the number of active calls.
     */
    int GetActiveCallNumber() const;

    /**
     * @brief Set the held calls number.
     *
     * @param number The number of held calls.
     */
    void SetHeldCallNumber(int number);

    /**
     * @brief Get the Held Call Number.
     *
     * @return Returns the number of held call.
     */
    int GetHeldCallNumber() const;

    /**
     * @brief Set the call state.
     *
     * @param state The status of the call.
     */
    void SetCallState(int state);

    /**
     * @brief Get the call state.
     *
     * @return Returns the status of the call.
     */
    int GetCallState() const;

    /**
     * @brief Check if it is in a call.
     *
     * @return Returns if is in a call flag.
     */
    bool IsInCall() const;

    /**
     * @brief Check if there is an incoming call.
     *
     * @return Returns is incoming call flag.
     */
    bool IsRinging() const;

    /**
     * @brief handle chld command
     *
     * @param chld chld operation
     * @return true operation success
     * @return false operation failed
     */
    bool HandleChld(int chld) const;

private:
    /**
     * @brief Construct a new HfpAgSystemInterface object.
     */
    HfpAgSystemInterface();

    /**
     * @brief Destroy the HfpAgSystemInterface object.
     */
    ~HfpAgSystemInterface() = default;

    /**
     * @brief Send the service state to the service.
     */
    void SendServiceStateToService() const;

    /**
     * @brief Send the service state to the service.
     */
    void SendIncomingCallToService(std::string number, int type) const;

    /**
     * @brief send cmd in mock state
     *
     * @param evt ag event
     */
    void SendMockEvent(HfpAgMessage evt) const;

    /**
     * @brief send mock clip command
     *
     * @param call number
     */
    void SendClip(std::string number) const;

    /**
     * @brief Send the service state to the service.
     */
    void SendCallSetupToService(int state) const;

    /**
     * @brief Send the call held state to the service.
     */
    void SendCallHeldToService(int state) const;

    /**
     * @brief Send the calle state to the service.
     */
    void SendCallStateToService(int state) const;

    /**
     * @brief Send response hold state to the service.
     */
    void SendResponseHoldStateToService(int state) const;

    /**
     * @brief Send the roam state to the service.
     */
    void SendRoamStateToService() const;

    /**
     * @brief Send the signal strength to the service.
     */
    void SendSignalStrengthToService() const;

    /**
     * @brief Send the battery level to the service.
     */
    void SendBatteryLevelToService() const;

    /**
     * @brief Register a TelephonyObserver to the telephony subsystem.
     */
    void RegisterObserver();

    /**
     * @brief Unregister a TelephonyObserver to the telephony subsystem.
     */
    void UnregisterObserver();

    /**
     * @brief add for pts chld command
     *
     * @param chld  chle operation
     * @return true success
     * @return false  failed
     */
    bool HandleChldMock(int chld) const;

    /**
     * @brief add for pts get call list
     *
     * @param address device address
     * @return true success
     * @return false failed
     */
    bool HandleClccMock(std::string address) const;

    /**
     * @brief Convert from string to u16string
     */

    inline std::u16string Str8ToStr16(const std::string& str) const;

    /**
     * @brief Convert from u16string to string
     */
    inline std::string Str16ToStr8(const std::u16string& str) const;

    // The state of the service.
    int serviceState_ {0};

    // The signal strength.
    int signalStrength_ {0};

    // The roam state.
    int roamState_ {0};

    // The battery level.
    int batteryLevel_ {0};

    // The number of active call.
    int activeNum_ {0};

    // The number of held call.
    int heldNum_ {0};

    // The status of the call.
    int callState_ {HFP_AG_CALL_STATE_DISCONNECTED};

    // The primary slot Id
    int slotId_ {0};

    // The number of the sim card
    std::string subscriberNumber_ {""};

    // The vendor name
    std::string operatorName_ {""};

    class AgTelephonyObserver;

    // For pts mock call list
    std::vector<MockCall> callList_;

    // bluetooth call list
    std::vector<BluetoothCall> bluetoothCallList_;

    sptr<SystemAbilityStatusChange> statusChangeListener_;

     // The observer that implements TelephonyObserver
    OHOS::sptr<AgTelephonyObserver> observer_ {nullptr};
};
class HfpAgSystemInterface::AgTelephonyObserver : public OHOS::Telephony::TelephonyObserver {
public:
    explicit AgTelephonyObserver(HfpAgSystemInterface &interface) : interface_(interface)
    {}
    ~AgTelephonyObserver() override
    {}

    void OnCellularDataConnectStateUpdated(int32_t slotId, int32_t dataState, int32_t networkType) override
    {}
    void OnCallStateUpdated(int32_t slotId, int32_t callState, const std::u16string &phoneNumber) override
    {}
    void OnSignalInfoUpdated(int32_t slotId,
        const std::vector<OHOS::sptr<OHOS::Telephony::SignalInformation>> &vec) override;

    void OnNetworkStateUpdated(int32_t slotId, const OHOS::sptr<OHOS::Telephony::NetworkState> &networkState) override;

    void OnCellInfoUpdated(int32_t slotId,
        const std::vector<OHOS::sptr<OHOS::Telephony::CellInformation>> &vec) override
    {}
    void OnSimStateUpdated(int32_t slotId, OHOS::Telephony::CardType type, OHOS::Telephony::SimState state,
        OHOS::Telephony::LockReason reason) override
    {}
    void OnCellularDataFlowUpdated(int32_t slotId, int32_t dataFlowType) override
    {}

private:
    HfpAgSystemInterface &interface_;
    BT_DISALLOW_COPY_AND_ASSIGN(AgTelephonyObserver);
};
}  // namespace bluetooth
}  // namespace OHOS
#endif // HFP_AG_SYSTEM_INTERFACE_H