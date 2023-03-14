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
 * @file interface_profile_hfp_hf.h
 *
 * @brief Declares HFP HF role interface profile functions, including basic and observer functions.
 *
 * @since 6
 */

#ifndef INTERFACE_PROFILE_HFP_HF_H
#define INTERFACE_PROFILE_HFP_HF_H

#include "interface_profile.h"
#include "hands_free_unit_calls.h"
#include <optional>
#include <vector>

namespace OHOS {
namespace bluetooth {
/**
 * @brief Class for HfpHfServiceObserver functions.
 *
 * @since 6
 */
class HfpHfServiceObserver {
public:
    /**
     * @brief Destroy the HfpHfServiceObserver object as default.
     *
     * @since 6
     */
    virtual ~HfpHfServiceObserver() = default;

    /**
     * @brief The observer function to notify connection state changed.
     *
     * @param device Remote device object.
     * @param state Connection state.
     * @since 6
     */
    virtual void OnConnectionStateChanged(const RawAddress &device, int state)
    {}

    /**
     * @brief The observer function to notify audio connection state changed.
     *
     * @param device Remote device object.
     * @param state Connection state.
     * @since 6
     */
    virtual void OnScoStateChanged(const RawAddress &device, int state)
    {}

    /**
     * @brief The observer function to notify call object changed.
     *
     * @param device Remote device object.
     * @param call Call object.
     * @since 6
     */
    virtual void OnCallChanged(const RawAddress &device, const HandsFreeUnitCalls &call)
    {}

    /**
     * @brief The observer function to notify battery level changed.
     *
     * @param device Remote device object.
     * @param batteryLevel Battery level.
     * @since 6
     */
    virtual void OnBatteryLevelChanged(const RawAddress &device, int batteryLevel)
    {}

    /**
     * @brief The observer function to notify signal strength changed.
     *
     * @param device Remote device object.
     * @param batteryLevel Signal strength.
     * @since 6
     */
    virtual void OnSignalStrengthChanged(const RawAddress &device, int signal)
    {}

    /**
     * @brief The observer function to notify registration status changed.
     *
     * @param device Remote device object.
     * @param status Registration status.
     * @since 6
     */
    virtual void OnRegistrationStatusChanged(const RawAddress &device, int status)
    {}

    /**
     * @brief The observer function to notify roaming status changed.
     *
     * @param device Remote device object.
     * @param status Roaming status.
     * @since 6
     */
    virtual void OnRoamingStatusChanged(const RawAddress &device, int status)
    {}

    /**
     * @brief The observer function to notify operator selection name changed.
     *
     * @param device Remote device object.
     * @param name Operator selection name.
     * @since 6
     */
    virtual void OnOperatorSelectionChanged(const RawAddress &device, const std::string &name)
    {}

    /**
     * @brief The observer function to notify subscriber number changed.
     *
     * @param device Remote device object.
     * @param number Subscriber number.
     * @since 6
     */
    virtual void OnSubscriberNumberChanged(const RawAddress &device, const std::string &number)
    {}

    /**
     * @brief The observer function to notify voice recognition status changed.
     *
     * @param device Remote device object.
     * @param status Voice recognition status.
     * @since 6
     */
    virtual void OnVoiceRecognitionStatusChanged(const RawAddress &device, int status)
    {}

    /**
     * @brief The observer function to notify inBand ring tone status changed.
     *
     * @param device Remote device object.
     * @param status InBand ring tone status.
     * @since 6
     */
    virtual void OnInBandRingToneChanged(const RawAddress &device, int status)
    {}
};

/**
 * @brief Class for IProfileHfpHf API.
 *
 * @since 6
 */
class IProfileHfpHf : public IProfile {
public:
    /**
     * @brief Destroy the IProfileHfpHf object as default.
     *
     * @since 6
     */
    virtual ~IProfileHfpHf() = default;

    /**
     * @brief Initiate the establishment of an audio connection to remote AG device.
     *
     * @param  device Remote device object.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool ConnectSco(const RawAddress &device) = 0;

    /**
     * @brief Release the audio connection from remote HF device.
     *
     * @param  device Remote device object.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool DisconnectSco(const RawAddress &device) = 0;

    /**
     * @brief Get remote AG device list which are in the specified states.
     *
     * @param states List of remote device states.
     * @return Returns the list of devices.
     * @since 6
     */
    virtual std::vector<RawAddress> GetDevicesByStates(std::vector<int> states) = 0;

    /**
     * @brief Get the connection state of the specified remote AG device.
     *
     * @param device Remote device object.
     * @return Returns the connection state of the remote device.
     * @since 6
     */
    virtual int GetDeviceState(const RawAddress &device) = 0;

    /**
     * @brief Get the Audio connection state of the specified remote AG device.
     *
     * @param device Remote device object.
     * @return Returns the Audio connection state.
     * @since 6
     */
    virtual int GetScoState(const RawAddress &device) = 0;

    /**
     * @brief Send DTMF tone code to remote AG device.
     *
     * @param device Remote device object.
     * @param code DTMF tone code.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool SendDTMFTone(const RawAddress &device, uint8_t code) = 0;

    /**
     * @brief Open voice recognition.
     *
     * @param device Remote device object.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool OpenVoiceRecognition(const RawAddress &device) = 0;

    /**
     * @brief Close voice recognition.
     *
     * @param device Remote device object.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool CloseVoiceRecognition(const RawAddress &device) = 0;

    /**
     * @brief Get a list of current all existing calls.
     *
     * @param device Remote device object.
     * @return The list of all existing calls.
     * @since 6
     */
    virtual std::vector<HandsFreeUnitCalls> GetCurrentCallList(const RawAddress &device) = 0;

    /**
     * @brief Accept an incoming call.
     *
     * @param device Remote device object.
     * @param flag Types of calls accepted.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool AcceptIncomingCall(const RawAddress &device, int flag) = 0;

    /**
     * @brief Hold an active call.
     *
     * @param device Remote device object.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool HoldActiveCall(const RawAddress &device) = 0;

    /**
     * @brief Reject an incoming call.
     *
     * @param device Remote device object.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool RejectIncomingCall(const RawAddress &device) = 0;

    /**
     * @brief Send key pressed event.
     *
     * @param device Remote device object.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 9
     */
    virtual bool SendKeyPressed(const RawAddress &device) = 0;

    /**
     * @brief Handle an incoming call.
     *
     * @param device Remote device object.
     * @param flag Types of calls handle.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 9
     */
    virtual bool HandleIncomingCall(const RawAddress &device, int flag) = 0;

    /**
     * @brief Handle multi call.
     *
     * @param device Remote device object.
     * @param flag Types of calls handle.
     * @param index call index
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 9
     */
    virtual bool HandleMultiCall(const RawAddress &device, int flag, int index) = 0;

    /**
     * @brief Dial last number.
     *
     * @param device Remote device object.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 9
     */
    virtual bool DialLastNumber(const RawAddress &device) = 0;

    /**
     * @brief Dial memory number.
     *
     * @param device Remote device object.
     * @param index Memory index.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 9
     */
    virtual bool DialMemory(const RawAddress &device, int index) = 0;

    /**
     * @brief Send voice tag.
     *
     * @param device Remote device object.
     * @param index voice tag index.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 9
     */
    virtual bool SendVoiceTag(const RawAddress &device, int index) = 0;

    /**
     * @brief Finish a specified active call.
     *
     * @param device Remote device object.
     * @param call Call object.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool FinishActiveCall(const RawAddress &device, const HandsFreeUnitCalls &call) = 0;

    /**
     * @brief Start dial with specified call number.
     *
     * @param device Remote device object.
     * @param number Call's number to dial.
     * @return The object of dial out call or nullopt.
     * @since 6
     */
    virtual std::optional<HandsFreeUnitCalls> StartDial(const RawAddress &device, const std::string &number) = 0;

    /**
     * @brief Register HandsFree Unit observer instance.
     *
     * @param observer HandsFreeUnitObserver instance.
     * @since 6
     */
    virtual void RegisterObserver(HfpHfServiceObserver &observer) = 0;

    /**
     * @brief Deregister HandsFree Unit observer instance.
     *
     * @param observer HandsFreeUnitObserver instance.
     * @since 6
     */
    virtual void DeregisterObserver(HfpHfServiceObserver &observer) = 0;
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // INTERFACE_PROFILE_HFP_HF_H