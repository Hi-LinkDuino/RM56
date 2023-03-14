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

#ifndef BLE_CONFIG_H
#define BLE_CONFIG_H

#include <cstring>
#include <vector>

#include "adapter_device_config.h"
#include "adapter_device_info.h"

/*
 * @brief The Bluetooth subsystem.
 */
namespace OHOS {
namespace bluetooth {
/**
 * @brief BLE config.
 */
class BleConfig {
public:
    static BleConfig &GetInstance();

    bool LoadConfigInfo() const;
    bool Save() const;

    std::string GetLocalAddress() const;
    std::string GetLocalName() const;
    int GetIoCapability() const;
    std::string GetLoaclPasskey() const;
    int GetBleRoles() const;
    int GetBleModel1Level() const;
    int GetBleModel2Level() const;
    bool GetBleSecurity() const;
    int GetBleScanMode() const;
    int GetAppearance() const;
    int GetBleLocalAddrType() const;
    int GetBleAddrType() const;

    bool SetLocalAddress(const std::string &addr) const;
    bool SetLocalName(const std::string &name) const;
    bool SetBleRoles(uint8_t roles) const;
    bool SetPasskey(const std::string &passkey) const;
    bool SetBleModel1Level(int level) const;
    bool SetBleModel2Level(int level) const;
    bool SetBleSecurity(bool security) const;
    bool SetBleScanMode(int scanmode) const;
    bool SetBleLocalAddrType(int localAddrType) const;

    /// local
    bool SetLocalIdentityAddr(const std::string &addr) const;
    bool SetLocalIrk(const std::string &irk) const;
    bool SetLocalLtk(const std::string &section, const std::string &ltk) const;
    bool SetLocalKeySize(const std::string &section, const std::string &keysize) const;
    bool SetLocalEdivRand(const std::string &section, const std::string &ediv, const std::string &rand) const;
    bool SetLocalCsrk(const std::string &section, const std::string &csrk) const;
    bool SetLocalSignCounter(const std::string &section, uint32_t signCounter) const;

    std::string GetLocalIrk() const;
    std::string GetLocalLtk(const std::string &section) const;
    std::string GetLocalEdiv(const std::string &section) const;
    std::string GetLocalRand(const std::string &section) const;
    std::string GetLocalCsrk(const std::string &section) const;
    uint32_t GetLocalSignCounter(const std::string &section) const;

    /// peer
    bool SetPeerKeyType(const std::string &section, const std::string &keytype) const;
    bool SetPeerLtk(const std::string &section, const std::string &ltk) const;
    bool SetPeerKeySize(const std::string &section, const std::string &keysize) const;
    bool SetPeerEdivRand(const std::string &section, const std::string &ediv, const std::string &rand) const;
    bool SetPeerIdentityAddr(const std::string &section, uint8_t type, const std::string &peerAddress) const;
    bool SetPeerIrk(const std::string &section, const std::string &irk) const;
    bool SetPeerCsrk(const std::string &section, const std::string &csrk) const;
    bool SetPeerSignCounter(const std::string &section, uint32_t signCounter) const;

    std::string GetPeerLtk(const std::string &section) const;
    std::string GetPeerEdiv(const std::string &section) const;
    std::string GetPeerRand(const std::string &section) const;
    std::string GetPeerIdentityAddr(const std::string &section) const;
    std::string GetPeerIrk(const std::string &section) const;
    std::string GetPeerCsrk(const std::string &section) const;
    uint32_t GetPeerSignCounter(const std::string &section) const;

    /// Api for get paired device info.
    std::string GetPeerName(const std::string &subSection) const;
    std::string GetPeerAlias(const std::string &subSection) const;
    int GetPeerDeviceType(const std::string &subSection) const;
    int GetPeerDeviceIoCapability(const std::string &subSection) const;
    int GetPeerAddressType(const std::string &subSection) const;

    bool SetPeerName(const std::string &subSection, const std::string &name) const;
    bool SetPeerDeviceType(const std::string &subSection, const int type) const;
    bool SetPeerDeviceIoCapability(const std::string &subSection, const int io) const;
    bool SetPeerAddressType(const std::string &subSection, const int type) const;
    bool RemovePairedDevice(const std::string &subSection) const;

    std::vector<std::string> GetPairedAddrList() const;

private:
    /**
     * @brief Constructor.
     */
    BleConfig();

    /**
     * @brief Destructor.
     */
    ~BleConfig();
    /**
     * @brief Constructor.
     */
    BleConfig(BleConfig &) = delete;

    /**
     * @brief Constructor.
     */
    BleConfig &operator=(const BleConfig &) = delete;
    /**
     * @brief Ble config single instance.
     */
    IAdapterDeviceConfig *config_ = nullptr;
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // BLE_CONFIG_H
