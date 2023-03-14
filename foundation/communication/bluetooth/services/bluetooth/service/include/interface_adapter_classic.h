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
 * @brief Defines adapter classic, including observer and common functions.
 *
 * @since 6
 */

/**
 * @file interface_adapter_classic.h
 *
 * @brief Adapter classic interface.
 *
 * @since 6
 */

#ifndef INTERFACE_ADAPTER_CLASSIC_H
#define INTERFACE_ADAPTER_CLASSIC_H

#include "interface_adapter.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief Represents adapter classic observer.
 *
 * @since 6
 */
class IAdapterClassicObserver {
public:
    /**
     * @brief A destructor used to delete the <b>IAdapterClassicObserver</b> instance.
     *
     * @since 6
     */
    virtual ~IAdapterClassicObserver() = default;

    /**
     * @brief Discovery state changed observer.
     *
     * @param status Device discovery status.
     * @since 6
     */
    virtual void OnDiscoveryStateChanged(int status) = 0;

    /**
     * @brief Discovery result observer.
     *
     * @param device Remote device.
     * @since 6
     */
    virtual void OnDiscoveryResult(const RawAddress &device) = 0;

    /**
     * @brief Pair requester observer.
     *
     * @param device Remote device.
     * @since 6
     */
    virtual void OnPairRequested(const BTTransport transport, const RawAddress &device) = 0;

    /**
     * @brief Pair confirmed observer.
     *
     * @param device Remote device.
     * @param reqType Pair type.
     * @param number Paired passkey.
     * @since 6
     */
    virtual void OnPairConfirmed(const BTTransport transport, const RawAddress &device, int reqType, int number) = 0;

    /**
     * @brief Scan mode changed observer.
     *
     * @param mode Device scan mode.
     * @since 6
     */
    virtual void OnScanModeChanged(int mode) = 0;

    /**
     * @brief Device name changed observer.
     *
     * @param deviceName Device name.
     * @since 6
     */
    virtual void OnDeviceNameChanged(const std::string &deviceName) = 0;

    /**
     * @brief Device address changed observer.
     *
     * @param address Device address.
     * @since 6
     */
    virtual void OnDeviceAddrChanged(const std::string &address) = 0;
};

/**
 * @brief Represents remote device observer.
 *
 * @since 6
 */
class IClassicRemoteDeviceObserver {
public:
    /**
     * @brief A destructor used to delete the <b>IClassicRemoteDeviceObserver</b> instance.
     *
     * @since 6
     */
    virtual ~IClassicRemoteDeviceObserver() = default;

    /**
     * @brief Pair status changed observer.
     *
     * @param device Remote device.
     * @param status Remote device pair status.
     * @since 6
     */
    virtual void OnPairStatusChanged(const BTTransport transport, const RawAddress &device, int status) = 0;

    /**
     * @brief Remote uuid changed observer.
     *
     * @param device Remote device.
     * @param uuids Remote device uuids.
     * @since 6
     */
    virtual void OnRemoteUuidChanged(const RawAddress &device, const std::vector<Uuid> &uuids) = 0;

    /**
     * @brief Remote name changed observer.
     *
     * @param device Remote device.
     * @param deviceName Remote device name.
     * @since 6
     */
    virtual void OnRemoteNameChanged(const RawAddress &device, const std::string &deviceName) = 0;

    /**
     * @brief Remote alias changed observer.
     *
     * @param device Remote device.
     * @param alias Remote device alias.
     * @since 6
     */
    virtual void OnRemoteAliasChanged(const RawAddress &device, const std::string &alias) = 0;

    /**
     * @brief Remote cod changed observer.
     *
     * @param device Remote device.
     * @param cod Remote device cod.
     * @since 6
     */
    virtual void OnRemoteCodChanged(const RawAddress &device, int cod) = 0;

    /**
     * @brief Remote battery level changed observer.
     *
     * @param device Remote device.
     * @param batteryLevel Remote device battery Level.
     * @since 6
     */
    virtual void OnRemoteBatteryLevelChanged(const RawAddress &device, int batteryLevel) = 0;
};

/**
 * @brief Represents classic adapter interface.
 *
 * @since 6
 */
class IAdapterClassic : public IAdapter {
public:
    /**
     * @brief A destructor used to delete the <b>IAdapterClassic</b> instance.
     *
     * @since 6
     */
    virtual ~IAdapterClassic() = default;

    /**
     * @brief Get remote device class.
     *
     * @param device Remote device.
     * @return Returns remote device class.
     * @since 6
     */
    virtual int GetDeviceClass(const RawAddress &device) const = 0;

    /**
     * @brief Get remote device alias name.
     *
     * @param device Remote device
     * @return Returns remote device alias name.
     * @since 6
     */
    virtual std::string GetAliasName(const RawAddress &device) const = 0;

    /**
     * @brief Set remote device alias name.
     *
     * @param device Remote device which set alias name.
     * @param name Alias name.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool SetAliasName(const RawAddress &device, const std::string &name) const = 0;

    /**
     * @brief Get remote device battery level.
     *
     * @param device Remote device
     * @return Returns remote device battery level.
     * @since 6
     */
    virtual int GetDeviceBatteryLevel(const RawAddress &device) const = 0;

    /**
     * @brief Set remote device battery level.
     *
     * @param device Remote device
     * @param batteryLevel battery level
     * @since 6
     */
    virtual void SetDeviceBatteryLevel(const RawAddress &device, int batteryLevel) const = 0;

    /**
     * @brief Register remote device observer.
     *
     * @param observer Class IClassicRemoteDeviceObserver pointer to register observer.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool RegisterRemoteDeviceObserver(IClassicRemoteDeviceObserver &observer) const = 0;

    /**
     * @brief Deregister remote device observer.
     *
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool DeregisterRemoteDeviceObserver(IClassicRemoteDeviceObserver &observer) const = 0;

    /**
     * @brief Set device scan mode.
     *
     * @param mode Scan mode.
     * @param duration Scan time.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool SetBtScanMode(int mode, int duration) = 0;

    /**
     * @brief Get device scan mode.
     *
     * @return Returns bluetooth scan mode.
     * @since 6
     */
    virtual int GetBtScanMode() const = 0;

    /**
     * @brief Get local device class.
     *
     * @return Returns local device class.
     * @since 6
     */
    virtual int GetLocalDeviceClass() const = 0;

    /**
     * @brief Set local device class.
     *
     * @param deviceClass Device class.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool SetLocalDeviceClass(int deviceClass) const = 0;

    /**
     * @brief Get device address.
     *
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool StartBtDiscovery() = 0;

    /**
     * @brief Cancel device discovery.
     *
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool CancelBtDiscovery() = 0;

    /**
     * @brief Check if device is discovering.
     *
     * @return Returns <b>true</b> if device is discovering;
     *         returns <b>false</b> if device is not discovering.
     * @since 6
     */
    virtual bool IsBtDiscovering() const = 0;

    /**
     * @brief Get device discovery end time.
     *
     * @return Returns device discovery end time.
     * @since 6
     */
    virtual long GetBtDiscoveryEndMillis() const = 0;

    /**
     * @brief Set device pair pin.
     *
     * @param device Remote device address.
     * @param pinCode Pin code.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool SetDevicePin(const RawAddress &device, const std::string &pinCode) const = 0;

    /**
     * @brief Register classic adapter observer.
     *
     * @param observer Class IAdapterClassicObserver pointer to register observer.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool RegisterClassicAdapterObserver(IAdapterClassicObserver &observer) const = 0;

    /**
     * @brief Deregister classic adapter observer.
     *
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool DeregisterClassicAdapterObserver(IAdapterClassicObserver &observer) const = 0;
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // INTERFACE_ADAPTER_CLASSIC_H