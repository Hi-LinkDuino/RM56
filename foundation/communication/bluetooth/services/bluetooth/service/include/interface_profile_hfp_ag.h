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
 * @file interface_profile_hfp_ag.h
 *
 * @brief Declares HFP AG role interface profile functions, including basic and observer functions.
 *
 * @since 6
 */

#ifndef INTERFACE_PROFILE_HFP_AG_H
#define INTERFACE_PROFILE_HFP_AG_H

#include "interface_profile.h"
#include <vector>
#include <string>

namespace OHOS {
namespace bluetooth {
/**
 * @brief Class for HfpAgServiceObserver functions.
 *
 * @since 6
 */
class HfpAgServiceObserver {
public:
    /**
     * @brief Destroy the HfpAgServiceObserver Observer object.
     *
     * @since 6
     */
    virtual ~HfpAgServiceObserver() = default;

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
     * @param state Audio connection state.
     * @since 6
     */
    virtual void OnScoStateChanged(const RawAddress &device, int state)
    {}

    /**
     * @brief The observer function to notify active device changed.
     *
     * @param device Remote active device object.
     * @since 6
     */
    virtual void OnActiveDeviceChanged(const RawAddress &device)
    {}

    /**
     * @brief The observer function to notify enhanced driver safety changed.
     *
     * @param device Remote device object.
     * @param indValue Enhanced driver safety value.
     * @since 6
     */
    virtual void OnHfEnhancedDriverSafetyChanged(const RawAddress &device, int indValue)
    {}

    /**
     * @brief The observer function to notify battery level changed.
     *
     * @param device Remote device object.
     * @param indValue Battery level value.
     * @since 6
     */
    virtual void OnHfBatteryLevelChanged(const RawAddress &device, int indValue)
    {}
};

/**
 * @brief Class for IProfileHfpAg API.
 *
 * @since 6
 */
class IProfileHfpAg : public IProfile {
public:
    /**
     * @brief Destroy the IProfileHfpAg object as default.
     *
     * @since 6
     */
    virtual ~IProfileHfpAg() = default;

    /**
     * @brief Get remote HF device list which are in the specified states.
     *
     * @param states List of remote device states.
     * @return Returns the list of devices.
     * @since 6
     */
    virtual std::vector<RawAddress> GetDevicesByStates(std::vector<int> states) = 0;

    /**
     * @brief Get the connection state of the specified remote HF device.
     *
     * @param device Remote device object.
     * @return Returns the connection state of the remote device.
     * @since 6
     */
    virtual int GetDeviceState(const RawAddress &device) = 0;

    /**
     * @brief Get the Audio connection state of the specified remote HF device.
     *
     * @param device Remote device object.
     * @return Returns the Audio connection state.
     * @since 6
     */
    virtual int GetScoState(const RawAddress &device) = 0;

    /**
     * @brief Initiate the establishment of an audio connection to remote active HF device.
     *
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool ConnectSco() = 0;

    /**
     * @brief Release the audio connection from remote active HF device.
     *
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool DisconnectSco() = 0;

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
    virtual void PhoneStateChanged(
        int numActive, int numHeld, int callState, const std::string &number, int type, const std::string &name) = 0;

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
    virtual void ClccResponse(
        int index, int direction, int status, int mode, bool mpty, const std::string &number, int type) = 0;

    /**
     * @brief Open the voice recognition.
     *
     * @param device Remote device object.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool OpenVoiceRecognition(const RawAddress &device) = 0;

    /**
     * @brief Close the voice recognition.
     *
     * @param device Remote device object.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool CloseVoiceRecognition(const RawAddress &device) = 0;

    /**
     * @brief Set the active device for audio connection.
     *
     * @param device Remote device object.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool SetActiveDevice(const RawAddress &device) = 0;

    /**
     * @brief Set mock state.
     *
     * @param state mock state.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 9
     */
    virtual bool IntoMock(int state) = 0;

    /**
     * @brief Set no carrier.
     *
     * @param device Remote device object..
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 9
     */
    virtual bool SendNoCarrier(const RawAddress &device) = 0;

    /**
     * @brief Get the active device object.
     *
     * @param device Remote active device object.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual std::string GetActiveDevice() = 0;

    /**
     * @brief Register HandsFree AudioGateway observer instance.
     *
     * @param observer HandsFree AudioGateway observer instance.
     * @since 6
     */
    virtual void RegisterObserver(HfpAgServiceObserver &observer) = 0;

    /**
     * @brief Deregister HandsFree AudioGateway observer instance.
     *
     * @param observer HandsFree AudioGateway observer instance.
     * @since 6
     */
    virtual void DeregisterObserver(HfpAgServiceObserver &observer) = 0;
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // INTERFACE_PROFILE_HFP_AG_H