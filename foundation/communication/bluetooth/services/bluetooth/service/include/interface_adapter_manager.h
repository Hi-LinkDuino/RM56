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
 * @brief Defines adapter manager, including observer and common functions.
 *
 * @since 6
 */

/**
 * @file interface_adapter_manager.h
 *
 * @brief Adapter manager interface.
 *
 * @since 6
 */

#ifndef INTERFACE_ADAPTER_MANAGER
#define INTERFACE_ADAPTER_MANAGER

#include "interface_adapter.h"
#include <memory>

namespace OHOS {
namespace bluetooth {
/**
 * @brief Represents adapter state change observer during enable/disable.
 *
 * @since 6
 */
class IAdapterStateObserver {
public:
    /**
     * @brief A destructor used to delete the <b>IAdapterStateObserver</b> instance.
     *
     * @since 6
     */
    virtual ~IAdapterStateObserver() = default;

    /**
     * @brief IAdapterStateObserver state change function.
     *
     * @param transport Transport type when state change.
     * @param state Change to the new state.
     * @since 6
     */
    virtual void OnStateChange(const BTTransport transport, const BTStateID state) = 0;
};

/**
 * @brief System state define, using to OnSystemChange()...
 */
enum class BTSystemState : int {
    ON,
    OFF,
};

/**
 * @brief Represents system state change observer during start/stop/factoryReset/reset.
 *
 * @since 6
 */
class ISystemStateObserver {
public:
    /**
     * @brief A destructor used to delete the <b>ISystemStateObserver</b> instance.
     *
     * @since 6
     */
    virtual ~ISystemStateObserver() = default;

    /**
     * @brief OnSystemStateChange state change function.
     *
     * @param transport Transport type when state change.
     * @param state Change to
     * @since 6
     */
    virtual void OnSystemStateChange(const BTSystemState state) = 0;
};

/**
 * @brief Represents interface adapter manager.
 *
 * @since 6
 */
class BLUETOOTH_API IAdapterManager {
public:
    /**
     * @brief A destructor used to delete the <b>IAdapterManager</b> instance.
     *
     * @since 6
     */
    virtual ~IAdapterManager() = default;

    /**
     * @brief Get adapter manager singleton instance pointer.
     *
     * @return Returns the singleton instance pointer.
     * @since 6
     */
    static IAdapterManager *GetInstance();

    /**
     * @brief Reset bluetooth service.
     *
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual void Reset() const = 0;

    /**
     * @brief Start bluetooth service.
     *
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool Start() = 0;

    /**
     * @brief Stop bluetooth service.
     *
     * @since 6
     */
    virtual void Stop() const = 0;

    /**
     * @brief Factory reset bluetooth service.
     *
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool FactoryReset() const = 0;

    /**
     * @brief Enable bluetooth service.
     *
     * @param transport Enable classic or ble.
     * @return Returns <b>true</b> if the operation is accepted;
     *         returns <b>false</b> if the operation is rejected.
     * @since 6
     */
    virtual bool Enable(const BTTransport transport) const = 0;

    /**
     * @brief Disable bluetooth service.
     *
     * @param transport Disable classic or ble.
     * @return Returns <b>true</b> if the operation is accepted;
     *         returns <b>false</b> if the operation is rejected.
     * @since 6
     */
    virtual bool Disable(const BTTransport transport) const = 0;

    /**
     * @brief Get adapter enable/disable state.
     *
     * @param transport Disable classic or ble.
     * @return Returns adapter enable/disable state.
     * @since 6
     */
    virtual BTStateID GetState(const BTTransport transport) const = 0;

    /**
     * @brief Get adapter connects state.
     *
     * @return Returns adapter connects state.
     * @since 6
     */
    virtual BTConnectState GetAdapterConnectState() const = 0;

    /**
     * @brief Register adapter state observer.
     *
     * @param observer Class IAdapterStateObserver pointer to register observer.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool RegisterStateObserver(IAdapterStateObserver &observer) const = 0;

    /**
     * @brief Deregister adapter state observer.
     *
     * @param observer Class IAdapterStateObserver pointer to deregister observer.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool DeregisterStateObserver(IAdapterStateObserver &observer) const = 0;

    /**
     * @brief Register system state observer.
     *
     * @param observer Class ISystemStateObserver pointer to register observer.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool RegisterSystemStateObserver(ISystemStateObserver &observer) const = 0;

    /**
     * @brief Deregister system state observer.
     *
     * @param observer Class ISystemStateObserver pointer to deregister observer.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool DeregisterSystemStateObserver(ISystemStateObserver &observer) const = 0;

    /**
     * @brief Get max audio connected devices number.
     *
     * @return Returns max device number that audio can connect.
     * @since 6
     */
    virtual int GetMaxNumConnectedAudioDevices() const = 0;

    /**
     * @brief Set phonebook permission for device.
     *
     * @param address Device address which is setted permission.
     * @param permission Permission grade.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool SetPhonebookPermission(const std::string &address, BTPermissionType permission) const = 0;

    /**
     * @brief Get phonebook permission for device.
     *
     * @param address Device address which is setted permission.
     * @return Returns Permission grade.
     * @since 6
     */
    virtual BTPermissionType GetPhonebookPermission(const std::string &address) const = 0;

    /**
     * @brief Set message permission for device.
     *
     * @param address Device address which is setted permission.
     * @param permission Permission grade.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool SetMessagePermission(const std::string &address, BTPermissionType permission) const = 0;

    /**
     * @brief Get message permission for device.
     *
     * @param address Device address which is setted permission.
     * @return Returns Permission grade.
     * @since 6
     */
    virtual BTPermissionType GetMessagePermission(const std::string &address) const = 0;

    /**
     * @brief Get classic adapter or ble adapter.
     *
     * @param transport classic or adapter.
     * @return Returns Basic adapter pointer.
     * @since 6
     */
    virtual IAdapter *GetAdapter(const BTTransport transport) const = 0;

    /**
     * @brief Get power mode.
     *
     * @param address Device address.
     * @return Returns power mode grade.
     *         BTPowerMode::MODE_INVALID = 0x00,
     *         BTPowerMode::MODE_ACTIVE = 0x100,
     *         BTPowerMode::MODE_SNIFF_LEVEL_LOW = 0x201,
     *         BTPowerMode::MODE_SNIFF_LEVEL_MID = 0x202,
     *         BTPowerMode::MODE_SNIFF_LEVEL_HIG = 0x203,
     * @since 6
     */
    virtual int GetPowerMode(const std::string &address) const = 0;
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // INTERFACE_ADAPTER_MANAGER