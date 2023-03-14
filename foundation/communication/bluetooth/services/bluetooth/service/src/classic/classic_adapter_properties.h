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
 * @brief Defines adapter classic properties.
 *
 */

/**
 * @file classic_adapter_properties.h
 *
 * @brief Adapter classic properties.
 *
 */

#ifndef CLASSIC_ADAPTER_PROPERTIES_H
#define CLASSIC_ADAPTER_PROPERTIES_H

#include <mutex>
#include <string>
#include <vector>

#include "base_observer_list.h"
#include "bt_uuid.h"
#include "btstack.h"
#include "classic_bluetooth_data.h"
#include "classic_config.h"
#include "classic_remote_device.h"
#include "dispatcher.h"
#include "interface_adapter_classic.h"
#include "log.h"
#include "log_util.h"
#include "raw_address.h"
#include "timer.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief AdapterProperties to save and get classic properties.
 *
 */
class ClassicAdapterProperties {
public:
    /**
     * @brief Get classic adapter properties singleton instance pointer.
     *
     * @return Returns the singleton instance pointer.
     */
    static ClassicAdapterProperties &GetInstance();

    /**
     * @brief Load config info.
     *
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool LoadConfigInfo();

    /**
     * @brief Initialize scan, bondable, security mode.
     *
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool InitMode();

    /**
     * @brief Set local class and name.
     *
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool ConfigProperties();

    /**
     * @brief Set bondable mode.
     *
     * @param mode Bondable mode.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool SetBondableMode(int mode);

    /**
     * @brief Set discoverable timeout.
     *
     * @param time Discoverable timeout.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool SetDiscoverableTimeout(int time);

    /**
     * @brief Set Io capability.
     *
     * @param ioCapability Device Io capability.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool SetIoCapability(int ioCapability);

    /**
     * @brief Set local address.
     *
     * @param addr Device address.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool SetLocalAddress(const std::string &addr);

    /**
     * @brief Set local name.
     *
     * @param name Device name.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool SetLocalName(const std::string &name);

    /**
     * @brief Set local device class.
     *
     * @param deviceClass Device class.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool SetLocalDeviceClass(int deviceClass);

    /**
     * @brief Set security mode.
     *
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool SetSecurityMode();

    /**
     * @brief Get passkey.
     *
     * @return Returns passkey.
     */
    std::string GetPasskey() const;

    /**
     * @brief Get bondable mode.
     *
     * @return Returns bondable mode.
     */
    int GetBondableMode() const;

    /**
     * @brief Get discoverable timeout.
     *
     * @return Returns discoverable timeout.
     */
    int GetDiscoverableTimeout() const;

    /**
     * @brief Get device Io capability.
     *
     * @return Returns device Io capability.
     */
    int GetIoCapability() const;

    /**
     * @brief Get local address.
     *
     * @return Returns local address.
     */
    std::string GetLocalAddress() const;

    /**
     * @brief Get local name.
     *
     * @return Returns local name.
     */
    std::string GetLocalName() const;

    /**
     * @brief Get local device class.
     *
     * @return Returns local device class.
     */
    int GetLocalDeviceClass() const;

    /**
     * @brief Get paired address list.
     *
     * @return Returns address list.
     */
    std::vector<std::string> GetPairedAddrList() const;

    /**
     * @brief Set local name scallback.
     *
     * @param result Set result.
     */
    void SetLocalNameCallback(int result);

    /**
     * @brief Register classic adapter observer.
     *
     * @param observer Class ClassicAdapterObserver pointer to register observer.
     */
    void RegisterClassicAdapterObserver(IAdapterClassicObserver &observer);

    /**
     * @brief Deregister classic adapter observer.
     *
     */
    void DeregisterClassicAdapterObserver(IAdapterClassicObserver &observer);

    /**
     * @brief Get paired device.
     *
     * @param addr Device address.
     * @return Returns paired device.
     */
    std::shared_ptr<ClassicRemoteDevice> GetPairedDevice(std::string addr);

    /**
     * @brief Save paired device info.
     *
     * @param remote Remote device.
     */
    void SavePairedDeviceInfo(std::shared_ptr<ClassicRemoteDevice> remote);

    /**
     * @brief Remove paired device info.
     *
     * @param addr Remote device address.
     */
    void RemovePairedDeviceInfo(std::string addr) const;

    /**
     * @brief Save config file.
     *
     */
    void SaveConfigFile() const;

    /**
     * @brief Save local support uuids.
     *
     * @param uuids Device uuids.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool SaveSupportUuids(const std::vector<Uuid> &uuids);

private:
    /**
     * @brief A constructor used to create a <b>ClassicAdapterProperties</b> instance.
     *
     */
    ClassicAdapterProperties();

    /**
     * @brief A destructor used to delete the <b>ClassicAdapterProperties</b> instance.
     *
     */
    ~ClassicAdapterProperties();

    /**
     * @brief Load host info from config file.
     *
     */
    void LoadHostInfo();

    /**
     * @brief Read mac address from controller.
     *
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool ReadAddrFromController();

    /**
     * @brief Update config.
     *
     * @param type Properties type.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool UpdateConfig(int type);

    /**
     * @brief Set eir data.
     *
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool SetEirData();

    void SendDeviceNameChanged(const std::string &deviceName);
    void SendDeviceAddrChanged(const std::string &address);

    ClassicConfig &config_;

    int cod_ {DEFAULT_CLASS_OF_DEVICE};
    std::string passkey_ {DEFAULT_PASSKEY};
    int bondableMode_ {INVALID_VALUE};
    int securityMode_ {SEC_MODE_2};
    int ioCapability_ {GAP_IO_DISPLAYYESNO};
    int discoverableTimeout_ {DEFAULT_SCANMODE_DURATION_MILLIS};
    std::string macAddr_ {INVALID_MAC_ADDRESS};
    std::string deviceName_ {DEFAULT_DEVICE_NAME};
    std::vector<Uuid> uuids_ {};
    BaseObserverList<IAdapterClassicObserver> adapterObservers_ {};
    std::recursive_mutex propertiesMutex_ {};
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // CLASSIC_ADAPTER_PROPERTIES_H