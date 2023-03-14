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

#ifndef BLE_PROPERTIES_H
#define BLE_PROPERTIES_H

#include <mutex>
#include <string>
#include <vector>

#include "base_observer_list.h"
#include "ble_config.h"
#include "ble_defs.h"
#include "interface_adapter_ble.h"

namespace OHOS {
namespace bluetooth {
/**
 * @BleProperties to save and get classic properties.
 */
class BleProperties {
public:
    /**
     * @brief Constructor.
     */
    BleProperties(BleProperties &) = delete;

    /**
     * @brief Constructor.
     */
    BleProperties &operator=(const BleProperties &) = delete;

    /**
     * @brief Get ble Properties instance.
     *
     * @return @c advertiser instance.
     */
    static BleProperties &GetInstance();

    bool SetLocalAddress(const std::string &addr) const;
    bool SetLocalName(const std::string &name) const;
    int SetBondableMode(const int mode) const;
    bool SetIoCapability(const int ioCapability) const;

    int GetBondableMode() const;
    int GetIoCapability() const;
    static int GetAppearance();
    std::string GetLocalAddress() const;
    std::string GetLocalName() const;
    std::string GetPasskey() const;

    bool LoadBleConfigInfo() const;
    bool ConfigBleProperties() const;
    static bool SetBleRoles(uint8_t roles);
    static int GetBleRoles();

    static bool SetPasskey(const std::string &passkey);
    static bool SetBleModel1Level(int level);
    static bool SetBleModel2Level(int level);
    static bool SetBleSecurity(bool security);
    static bool SetBleScanMode(int scanmode);

    bool SaveDefaultValues() const;
    bool GetAddrFromController() const;

    void RegisterBleAdapterObserver(BaseObserverList<IAdapterBleObserver> &observer) const;
    void DeregisterBleAdapterObserver(IAdapterBleObserver &observer) const;

private:
    BleProperties();
    ~BleProperties();

    void ReadBleHostInfo() const;
    bool UpdateConfig(int type) const;
    DECLARE_IMPL();
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // BLE_PROPERTIES_H
