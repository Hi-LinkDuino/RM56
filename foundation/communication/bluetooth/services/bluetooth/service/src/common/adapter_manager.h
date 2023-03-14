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

#ifndef ADAPTER_MANAGER_H
#define ADAPTER_MANAGER_H

#include <memory>
#include <string>
#include <vector>
#include "bt_def.h"
#include "interface_adapter.h"
#include "interface_adapter_manager.h"
#include "util/context.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief Represents adapter manager.
 *
 * @since 6
 */
const static int CLASSIC_ENABLE_STATE_BIT = 0x04;
class AdapterManager : public IAdapterManager {
public:
    /**
     * @brief Get adapter manager singleton instance pointer.
     *
     * @return Returns the singleton instance pointer.
     * @since 6
     */
    static AdapterManager *GetInstance();

    // framework function
    /**
     * @brief Get classic adapter or ble adapter.
     *
     * @param transport Adapter transport.
     * @return Returns Basic adapter pointer.
     * @since 6
     */
    IAdapter *GetAdapter(const BTTransport transport) const override;

    /**
     * @brief bluetooth adapter start.
     *
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool Start() override;

    /**
     * @brief Stop bluetooth service.
     *
     * @since 6
     */
    void Stop() const override;

    /**
     * @brief Reset bluetooth service.
     *
     * @since 6
     */
    void Reset() const override;

    /**
     * @brief Factory reset bluetooth service.
     *
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool FactoryReset() const override;

    /**
     * @brief Enable bluetooth service.
     *
     * @param transport Adapter transport.
     * @return Returns <b>true</b> if the operation is accepted;
     *         returns <b>false</b> if the operation is rejected.
     * @since 6
     */
    bool Enable(const BTTransport transport) const override;

    /**
     * @brief Disable bluetooth service.
     *
     * @param transport Adapter transport.
     * @return Returns <b>true</b> if the operation is accepted;
     *         returns <b>false</b> if the operation is rejected.
     * @since 6
     */
    bool Disable(const BTTransport transport) const override;

    /**
     * @brief Get adapter enable/disable state.
     *
     * @param transport Adapter transport.
     * @return Returns adapter enable/disable state.
     * @since 6
     */
    BTStateID GetState(const BTTransport transport) const override;

    /**
     * @brief Get adapter connects state.
     *
     * @return Returns adapter connects state.
     * @since 6
     */
    BTConnectState GetAdapterConnectState() const override;

    /**
     * @brief Register adapter state observer.
     *
     * @param observer Class IAdapterStateObserver pointer to register observer.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool RegisterStateObserver(IAdapterStateObserver &observer) const override;

    /**
     * @brief Deregister adapter state observer.
     *
     * @param observer Class IAdapterStateObserver pointer to deregister observer.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool DeregisterStateObserver(IAdapterStateObserver &observer) const override;

    /**
     * @brief Register system state observer.
     *
     * @param observer Class ISystemStateObserver pointer to register observer.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool RegisterSystemStateObserver(ISystemStateObserver &observer) const override;

    /**
     * @brief Deregister system state observer.
     *
     * @param observer Class ISystemStateObserver pointer to deregister observer.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool DeregisterSystemStateObserver(ISystemStateObserver &observer) const override;

    /**
     * @brief Get max audio connected devices number.
     *
     * @return Returns max device number that audio can connect.
     * @since 6
     */
    int GetMaxNumConnectedAudioDevices() const override;

    /**
     * @brief Set phonebook permission for device.
     *
     * @param address Device address which is setted permission.
     * @param permission permission grade.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool SetPhonebookPermission(const std::string &address, BTPermissionType permission) const override;

    /**
     * @brief Get phonebook permission for device.
     *
     * @param address Device address which is setted permission.
     * @return Returns permission grade.
     * @since 6
     */
    BTPermissionType GetPhonebookPermission(const std::string &address) const override;

    /**
     * @brief Set message permission for device.
     *
     * @param address Device address which is setted permission.
     * @param permission permission grade.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool SetMessagePermission(const std::string &address, BTPermissionType permission) const override;

    /**
     * @brief Get message permission for device.
     *
     * @param address Device address which is setted permission.
     * @return Returns Permission grade.
     * @since 6
     */
    BTPermissionType GetMessagePermission(const std::string &address) const override;

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
    int GetPowerMode(const std::string &address) const override;

    /**
     * @brief Stop bluetooth adapter and profile service.
     *
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool AdapterStop() const;

    /**
     * @brief Clear all storage.
     *
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool ClearAllStorage() const;

    /**
     * @brief System state change.
     *
     * @param state Change to a new state.
     * @since 6
     */
    void OnSysStateChange(const std::string &state) const;

    /**
     * @brief System state exit.
     *
     * @param state Exit the old state.
     * @since 6
     */
    void OnSysStateExit(const std::string &state) const;

    /**
     * @brief Adapter state change.
     *
     * @param transport Adapter transport.
     * @param state Change to a new state.
     * @since 6
     */
    void OnAdapterStateChange(const BTTransport transport, const BTStateID state) const;

    /**
     * @brief Profile services enable complete notify.
     *
     * @param transport Adapter transport.
     * @param ret Profile services enable operation result.
     * @since 6
     */
    void OnProfileServicesEnableComplete(const BTTransport transport, const bool ret) const;

    /**
     * @brief Profile services disable complete notify.
     *
     * @param transport Adapter transport.
     * @param ret Profile services disable operation result.
     * @since 6
     */
    void OnProfileServicesDisableComplete(const BTTransport transport, const bool ret) const;

    /**
     * @brief Pair devices remove notify.
     *
     * @param transport Adapter transport.
     * @param devices The vector of removed devices.
     * @since 6
     */
    void OnPairDevicesRemoved(const BTTransport transport, const std::vector<RawAddress> &devices) const;

private:
    AdapterManager();
    ~AdapterManager();
    void CreateAdapters() const;
    std::string GetSysState() const;
    bool OutputSetting() const;
    void RegisterHciResetCallback();
    void DeregisterHciResetCallback() const;
    void RemoveDeviceProfileConfig(const BTTransport transport, const std::vector<RawAddress> &devices) const;

    static void HciFailedReset(void *context);

    BT_DISALLOW_COPY_AND_ASSIGN(AdapterManager);
    DECLARE_IMPL();
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // ADAPTER_MANAGER_H