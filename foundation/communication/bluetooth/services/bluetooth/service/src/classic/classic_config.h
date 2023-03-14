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
 * @brief Defines classic config functions.
 *
 */

/**
 * @file classic_config.h
 *
 * @brief Classic config.
 *
 */

#ifndef CLASSIS_CONFIG_H
#define CLASSIS_CONFIG_H

#include <mutex>
#include <string>

#include "adapter_device_config.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief Represents classic config.
 *
 */
class ClassicConfig {
public:
    /**
     * @brief Get classic config singleton instance pointer.
     *
     * @return Returns the singleton instance pointer.
     */
    static ClassicConfig &GetInstance();

    /**
     * @brief Load config info.
     *
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool LoadConfigFile() const;

    /**
     * @brief Save config.
     *
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool Save() const;

    /**
     * @brief Get security mode.
     *
     * @return Returns security mode.
     */
    int GetSecurityMode() const;

    /**
     * @brief Get local passkey.
     *
     * @return Returns local passkey.
     */
    std::string GetLocalPasskey() const;

    /**
     * @brief Get device Io capability.
     *
     * @return Returns device Io capability.
     */
    int GetIoCapability() const;

    /**
     * @brief Get local device class.
     *
     * @return Returns local device class.
     */
    int GetLocalDeviceClass() const;

    /**
     * @brief Get discoverable timeout.
     *
     * @return Returns discoverable timeout.
     */
    int GetDiscoverableTimeout() const;

    /**
     * @brief Get local name.
     *
     * @return Returns local name.
     */
    std::string GetLocalName() const;

    /**
     * @brief Set local device class.
     *
     * @param cod Device cod.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool SetLocalDeviceClass(int cod) const;

    /**
     * @brief Set discoverable timeout.
     *
     * @param time Discoverable timeout.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool SetDiscoverableTimeout(int time) const;

    /**
     * @brief Set local name.
     *
     * @param name Device name.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool SetLocalName(const std::string &name) const;

    /**
     * @brief Set local address.
     *
     * @param addr Device address.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool SetLocalAddress(const std::string &addr) const;

    /// Api for get paired device info.
    /**
     * @brief Get remote device name.
     *
     * @param subSection Device address.
     * @return Returns remote device name.
     */
    std::string GetRemoteName(const std::string &subSection) const;

    /**
     * @brief Get remote device alias.
     *
     * @param subSection Device address.
     * @return Returns remote device alias.
     */
    std::string GetRemoteAlias(const std::string &subSection) const;

    /**
     * @brief Get remote device link key.
     *
     * @param subSection Device address.
     * @return Returns remote device link key.
     */
    std::string GetRemoteLinkkey(const std::string &subSection) const;

    /**
     * @brief Get remote device type.
     *
     * @param subSection Device address.
     * @return Returns remote device type.
     */
    int GetRemoteDeviceType(const std::string &subSection) const;

    /**
     * @brief Get remote device link key type.
     *
     * @param subSection Device address.
     * @return Returns remote device link key type.
     */
    int GetRemoteLinkkeyType(const std::string &subSection) const;

    /**
     * @brief Get remote device class.
     *
     * @param subSection Device address.
     * @return Returns remote device class.
     */
    int GetRemoteDeviceClass(const std::string &subSection) const;

    /**
     * @brief Get remote device Io capability.
     *
     * @param subSection Device address.
     * @return Returns remote device Io capability.
     */
    int GetRemoteDeviceIoCapability(const std::string &subSection) const;

    /**
     * @brief Get paired device address list.
     *
     * @return Returns paired device address list.
     */
    std::vector<std::string> GetPairedAddrList() const;

    /**
     * @brief Get remote device pair flag.
     *
     * @param subSection Device address.
     * @return Returns remote device pair flag.
     */
    bool GetRemoteDevicePairFlag(const std::string &subSection) const;

    /**
     * @brief Get remote device if bond from local.
     *
     * @param subSection Device address.
     * @return Returns remote device if bond from local.
     */
    bool GetRemoteDeviceBondFromLocal(const std::string &subSection) const;

    /**
     * @brief Set remote device name.
     *
     * @param subSection Device address.
     * @param name Device name.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool SetRemoteName(const std::string &subSection, const std::string &name) const;

    /**
     * @brief Set remote device alias.
     *
     * @param subSection Device address.
     * @param name Device alias.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool SetRemoteAlias(const std::string &subSection, const std::string &name) const;

    /**
     * @brief Set remote device type.
     *
     * @param subSection Device address.
     * @param type Device type.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool SetRemoteDeviceType(const std::string &subSection, int type) const;

    /**
     * @brief Set remote device link key.
     *
     * @param subSection Device address.
     * @param linkKey Link key.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool SetRemoteLinkkey(const std::string &subSection, const std::string &linkKey) const;

    /**
     * @brief Set remote device link key type.
     *
     * @param subSection Device address.
     * @param type Link key type.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool SetRemoteLinkkeyType(const std::string &subSection, int type) const;

    /**
     * @brief Set remote device class.
     *
     * @param subSection Device address.
     * @param cod Device cod.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool SetRemoteDeviceClass(const std::string &subSection, int cod) const;

    /**
     * @brief Set remote device Io capabilityass.
     *
     * @param subSection Device address.
     * @param io Device Io capability.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool SetRemoteDeviceIoCapability(const std::string &subSection, int io) const;

    /**
     * @brief Set remote device pair flag.
     *
     * @param subSection Device address.
     * @param flag Pair flag.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool SetRemoteDevicePairFlag(const std::string &subSection, bool flag) const;

    /**
     * @brief Set remote remote device if bond from local.
     *
     * @param subSection Device address.
     * @param flag Device if bond from local.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool SetRemoteDeviceBondFromLocal(const std::string &subSection, bool flag) const;

    /**
     * @brief Remove paired device.
     *
     * @param subSection Device address.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool RemovePairedDevice(const std::string &subSection) const;

    /**
     * @brief Set Remote device uuids.
     *
     * @param uuids remote device support uuids.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool SetRemoteUuids(const std::string &subSection, const std::string &uuids) const;

    /**
     * @brief Get remote device uuids.
     *
     * @param subSection Device address.
     * @return Returns remote device uuids.
     */
    std::string GetRemoteUuids(const std::string &subSection) const;

private:
    /**
     * @brief A constructor used to create a <b>ClassicConfig</b> instance.
     *
     */
    ClassicConfig();
    ClassicConfig(const ClassicConfig &config);
    ClassicConfig &operator=(const ClassicConfig &config);

    /**
     * @brief A destructor used to delete the <b>ClassicConfig</b> instance.
     *
     */
    ~ClassicConfig();

    IAdapterDeviceConfig *config_ {};
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // CLASSIS_CONFIG_H