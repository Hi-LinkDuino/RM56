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
 * @file bluetooth_hfp_hf.h
 *
 * @brief Declares HFP HF role framework functions, including basic and observer functions.
 *
 * @since 6
 */

#ifndef BLUETOOTH_HFP_HF_H
#define BLUETOOTH_HFP_HF_H

#include <string>
#include <vector>
#include <memory>

#include "bluetooth_def.h"
#include "bluetooth_types.h"
#include "bluetooth_remote_device.h"
#include "bluetooth_hf_call.h"

namespace OHOS {
namespace Bluetooth {
/**
 * @brief Class for HandsFree Unit observer functions.
 *
 * @since 6
 */
class HandsFreeUnitObserver {
public:
    /**
     * @brief The observer function to notify connection state changed.
     *
     * @param device Remote device object.
     * @param state Connection state.
     * @since 6
     */
    virtual void OnConnectionStateChanged(const BluetoothRemoteDevice &device, int state)
    {}

    /**
     * @brief The observer function to notify audio connection state changed.
     *
     * @param device Remote device object.
     * @param state Connection state.
     * @since 6
     */
    virtual void OnScoStateChanged(const BluetoothRemoteDevice &device, int state)
    {}

    /**
     * @brief The observer function to notify call object changed.
     *
     * @param device Remote device object.
     * @param call Call object.
     * @since 6
     */
    virtual void OnCallChanged(const BluetoothRemoteDevice &device, const HandsFreeUnitCall &call)
    {}

    /**
     * @brief The observer function to notify signal strength changed.
     *
     * @param device Remote device object.
     * @param batteryLevel Signal strength.
     * @since 6
     */
    virtual void OnSignalStrengthChanged(const BluetoothRemoteDevice &device, int signal)
    {}

    /**
     * @brief The observer function to notify registration status changed.
     *
     * @param device Remote device object.
     * @param status Registration status.
     * @since 6
     */
    virtual void OnRegistrationStatusChanged(const BluetoothRemoteDevice &device, int status)
    {}

    /**
     * @brief The observer function to notify roaming status changed.
     *
     * @param device Remote device object.
     * @param status Roaming status.
     * @since 6
     */
    virtual void OnRoamingStatusChanged(const BluetoothRemoteDevice &device, int status)
    {}

    /**
     * @brief The observer function to notify operator selection name changed.
     *
     * @param device Remote device object.
     * @param name Operator selection name.
     * @since 6
     */
    virtual void OnOperatorSelectionChanged(const BluetoothRemoteDevice &device, const std::string &name)
    {}

    /**
     * @brief The observer function to notify subscriber number changed.
     *
     * @param device Remote device object.
     * @param number Subscriber number.
     * @since 6
     */
    virtual void OnSubscriberNumberChanged(const BluetoothRemoteDevice &device, const std::string &number)
    {}

    /**
     * @brief The observer function to notify voice recognition status changed.
     *
     * @param device Remote device object.
     * @param status Voice recognition status.
     * @since 6
     */
    virtual void OnVoiceRecognitionStatusChanged(const BluetoothRemoteDevice &device, int status)
    {}

    /**
     * @brief The observer function to notify inBand ring tone status changed.
     *
     * @param device Remote device object.
     * @param status InBand ring tone status.
     * @since 6
     */
    virtual void OnInBandRingToneChanged(const BluetoothRemoteDevice &device, int status)
    {}

    /**
     * @brief Destroy the HandsFreeUnitObserver object.
     *
     * @since 6
     */
    virtual ~HandsFreeUnitObserver()
    {}
};

/**
 * @brief Class for HandsFree Unit API.
 *
 * @since 6
 */
class BLUETOOTH_API HandsFreeUnit {
public:
    /**
     * @brief Get the instance of HandsFreeAudioGateway object.
     *
     * @return Returns the pointer to the HandsFreeAudioGateway instance.
     * @since 6
     */
    static HandsFreeUnit *GetProfile();

    /**
     * @brief Initiate the establishment of an audio connection to remote AG device.
     *
     * @param  device Remote device object.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool ConnectSco(const BluetoothRemoteDevice &device);

    /**
     * @brief Release the audio connection from remote HF device.
     *
     * @param  device Remote device object.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool DisconnectSco(const BluetoothRemoteDevice &device);

    /**
     * @brief Get remote AG device list which are in the specified states.
     *
     * @param states List of remote device states.
     * @return Returns the list of devices.
     * @since 6
     */
    std::vector<BluetoothRemoteDevice> GetDevicesByStates(std::vector<int> states) const;

    /**
     * @brief Get the connection state of the specified remote AG device.
     *
     * @param device Remote device object.
     * @return Returns the connection state of the remote device.
     * @since 6
     */
    int GetDeviceState(const BluetoothRemoteDevice &device) const;

    /**
     * @brief Get the Audio connection state of the specified remote AG device.
     *
     * @param device Remote device object.
     * @return Returns the Audio connection state.
     * @since 6
     */
    int GetScoState(const BluetoothRemoteDevice &device) const;

    /**
     * @brief Send DTMF tone code to remote AG device.
     *
     * @param device Remote device object.
     * @param code DTMF tone code.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool SendDTMFTone(const BluetoothRemoteDevice &device, uint8_t code);

    /**
     * @brief Initiate the establishment of a service level connection to remote AG device.
     *
     * @param device Remote device object.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool Connect(const BluetoothRemoteDevice &device);

    /**
     * @brief Release the audio connection from remote AG device.
     *
     * @param device Remote device object.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool Disconnect(const BluetoothRemoteDevice &device);

    /**
     * @brief Open voice recognition.
     *
     * @param device Remote device object.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool OpenVoiceRecognition(const BluetoothRemoteDevice &device);

    /**
     * @brief Close voice recognition.
     *
     * @param device Remote device object.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool CloseVoiceRecognition(const BluetoothRemoteDevice &device);

    /**
     * @brief Get a list of all existing calls.
     *
     * @param device Remote device object.
     * @return The list of all existing calls.
     * @since 6
     */
    std::vector<HandsFreeUnitCall> GetExistingCalls(const BluetoothRemoteDevice &device);

    /**
     * @brief Accept an incoming call.
     *
     * @param device Remote device object.
     * @param flag Types of calls accepted.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool AcceptIncomingCall(const BluetoothRemoteDevice &device, int flag);

    /**
     * @brief Hold an active call.
     *
     * @param device Remote device object.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool HoldActiveCall(const BluetoothRemoteDevice &device);

    /**
     * @brief Reject an incoming call.
     *
     * @param device Remote device object.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool RejectIncomingCall(const BluetoothRemoteDevice &device);

    /**
     * @brief send key pressed event.
     *
     * @param device Remote device object.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 9
     */

    bool SendKeyPressed(const BluetoothRemoteDevice &device);

    /**
     * @brief Handle an incoming call.
     *
     * @param device Remote device object.
     * @param flag handle action
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 9
     */
    bool HandleIncomingCall(const BluetoothRemoteDevice &device, int flag);

    /**
     * @brief Hande multi calll.
     *
     * @param device Remote device object.
     * @param flag handle action
     * @param index call index
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 9
     */
    bool HandleMultiCall(const BluetoothRemoteDevice &device, int flag, int index);

    /**
     * @brief dial last number.
     *
     * @param device Remote device object.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 9
     */
    bool DialLastNumber(const BluetoothRemoteDevice &device);

    /**
     * @brief dial memory number.
     *
     * @param device Remote device object.
     * @param index memory number index
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 9
     */
    bool DialMemory(const BluetoothRemoteDevice &device, int index);


    /**
     * @brief send voice tag.
     *
     * @param device Remote device object.
     * @param index voice tag index.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 9
     */

    bool SendVoiceTag(const BluetoothRemoteDevice &device, int index);

    /**
     * @brief brief Finish a specified active call.
     *
     * @param device Remote device object.
     * @param call Call object.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool FinishActiveCall(const BluetoothRemoteDevice &device, const HandsFreeUnitCall &call);

    /**
     * @brief Start dial with specified call number.
     *
     * @param device Remote device object.
     * @param number Call's number to dial.
     * @return The object of dial out call or nullopt.
     * @since 6
     */
    std::optional<HandsFreeUnitCall> StartDial(const BluetoothRemoteDevice &device, const std::string &number);

    /**
     * @brief Register HandsFree Unit observer instance.
     *
     * @param observer HandsFreeUnitObserver instance.
     * @since 6
     */
    void RegisterObserver(HandsFreeUnitObserver *observer);

    /**
     * @brief Deregister HandsFree Unit observer instance.
     *
     * @param observer HandsFreeUnitObserver instance.
     * @since 6
     */
    void DeregisterObserver(HandsFreeUnitObserver *observer);

    /**
     * @brief Static HandsFree Unit observer instance.
     *
     * @since 6
     */
    static HandsFreeUnit *instance_;

private:
    HandsFreeUnit();
    ~HandsFreeUnit();
    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(HandsFreeUnit);
    BLUETOOTH_DECLARE_IMPL();
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // BLUETOOTH_HFP_HF_H
