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

/**
 * @addtogroup Bluetooth
 * @{
 *
 * @brief Defines remote device , including common functions.
 *
 */

/**
 * @file classic_remote_device.h
 *
 * @brief Classic remote device common functions.
 *
 */

#ifndef REMOTE_DEVICE_H
#define REMOTE_DEVICE_H

#include <memory>
#include <string>
#include <vector>

#include "base_def.h"
#include "bt_def.h"
#include "bt_uuid.h"
#include "classic_defs.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief Represents remote device.
 *
 */
class ClassicRemoteDevice {
public:
    /**
     * @brief A constructor used to create a <b>ClassicRemoteDevice</b> instance.
     *
     */
    ClassicRemoteDevice();

    /**
     * @brief A constructor used to create a <b>ClassicRemoteDevice</b> instance.
     *
     * @param addr Device address create an <b>ClassicRemoteDevice</b> instance.
     */
    explicit ClassicRemoteDevice(const std::string &addr);

    /**
     * @brief A destructor used to delete the <b>ClassicRemoteDevice</b> instance.
     *
     */
    ~ClassicRemoteDevice();

    /**
     * @brief Get device type.
     *
     * @return Returns device type.
     */
    int GetDeviceType() const;

    /**
     * @brief Get device address.
     *
     * @return Returns device address.
     */
    std::string GetAddress() const;

    /**
     * @brief Get remote device name.
     *
     * @return Returns remote device name.
     */
    std::string GetRemoteName() const;

    /**
     * @brief Get device alias name.
     *
     * @return Returns device alias name.
     */
    std::string GetAliasName() const;

    /**
     * @brief Get device class.
     *
     * @return Returns device class.
     */
    int GetDeviceClass() const;

    /**
     * @brief Get device uuids.
     *
     * @return Returns device uuids vector.
     */
    std::vector<Uuid> GetDeviceUuids() const;

    /**
     * @brief Get acl connection handle.
     *
     * @return Returns acl connection handle;
     */
    int GetConnectionHandle() const;

    /**
     * @brief Get paired status.
     *
     * @return Returns paired status.
     */
    int GetPairedStatus() const;

    /**
     * @brief Get link key.
     *
     * @return Returns link key.
     */
    std::vector<uint8_t> GetLinkKey() const;

    /**
     * @brief Get link key type.
     *
     * @return Returns link key type.
     */
    int GetLinkKeyType() const;

    /**
     * @brief Get IO capability.
     *
     * @return Returns IO capability.
     */
    int GetIoCapability() const;

    /**
     * @brief Get device flags.
     *
     * @return Returns flags.
     */
    uint8_t GetFlags() const;

    /**
     * @brief Check if remote device already paired.
     *
     * @return Returns <b>true</b> if remote device already paired;
     *         returns <b>false</b> if remote device not paired.
     */
    bool IsPaired() const;

    /**
     * @brief Get pair confirm state.
     *
     * @return Returns pair confirm state.
     */
    int GetPairConfirmState() const;

    /**
     * @brief Get pair confirm type.
     *
     * @return Returns pair confirm type.
     */
    int GetPairConfirmType() const;

    /**
     * @brief Get battery level.
     *
     * @return Returns batteryLevel.
     */
    int GetBatteryLevel() const;

    /**
     * @brief Set battery level.
     *
     * @param batteryLevel battery level.
     */
    void SetBatteryLevel(int batteryLevel);

    /**
     * @brief set rssi value.
     *
     * @param rssi Rssi value.
     */
    void SetRssi(int rssi);

    /**
     * @brief Set device type.
     *
     * @param type Device type.
     */
    void SetDeviceType(int type);

    /**
     * @brief Set device address.
     *
     * @param addr Device address.
     */
    void SetAddress(const std::string &addr);

    /**
     * @brief Set remote device name.
     *
     * @param name Device name.
     */
    void SetRemoteName(const std::string &name);

    /**
     * @brief Set alias name.
     *
     * @param name Device alias name.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool SetAliasName(const std::string &name);

    /**
     * @brief Set device class.
     *
     * @param deviceClass Device class.
     */
    void SetDeviceClass(int deviceClass);

    /**
     * @brief Set bonded from local.
     *
     * @param flag Advertiser flag.
     */
    void SetBondedFromLocal(bool flag);

    /**
     * @brief Set acl connect state.
     *
     * @param connectState Acl connect state.
     */
    void SetAclConnectState(int connectState);

    /**
     * @brief Set device uuids.
     *
     * @param uuids Device uuids.
     */
    void SetDeviceUuids(const std::vector<Uuid> &uuids);

    /**
     * @brief Set acl connection handle.
     *
     * @param handle Acl connection handle.
     */
    void SetConnectionHandle(int handle);

    /**
     * @brief Set link key.
     *
     * @param linkKey Link key.
     */
    void SetLinkKey(const std::vector<uint8_t> &linkKey);

    /**
     * @brief Set link key type.
     *
     * @param linkKey Link key type.
     */
    void SetLinkKeyType(int linkKeyType);

    /**
     * @brief Set IO capability.
     *
     * @param io IO capability
     */
    void SetIoCapability(int io);

    /**
     * @brief Set device flags.
     *
     * @param flags Device flags.
     */
    void SetFlags(uint8_t flags);

    /**
     * @brief Set device manufacturer-specific data.
     *
     * @param data Manufacturer-specific data.
     * @param length Data length.
     */
    void SetManufacturerSpecificData(const std::vector<uint8_t> &data);

    /**
     * @brief Set device Tx power.
     *
     * @param power Device Tx power.
     */
    void SetTxPower(uint8_t power);

    /**
     * @brief Set device URI.
     *
     * @param uri Device URI.
     */
    void SetURI(const std::string &uri);

    /**
     * @brief Set pair confirm state.
     *
     * @param state Pair confirm state.
     */
    void SetPairConfirmState(int state);

    /**
     * @brief Set pair confirm type.
     *
     * @param type Pair confirm type.
     */
    void SetPairConfirmType(int type);

    /**
     * @brief Check if device acl connected.
     *
     * @return Returns <b>true</b> if device acl connected;
     *         returns <b>false</b> if device does not acl connect.
     */
    bool IsAclConnected() const;

    /**
     * @brief Check if device acl encrypted.
     *
     * @return Returns <b>true</b> if device acl encrypted;
     *         returns <b>false</b> if device does not acl encrypted.
     */
    bool IsAclEncrypted() const;

    /**
     * @brief Check if device was bonded from local.
     *
     * @return Returns <b>true</b> if device was bonded from local;
     *         returns <b>false</b> if device was not bonded from local.
     */
    bool IsBondedFromLocal() const;

    /**
     * @brief Delete link key.
     *
     */
    void DeleteLinkKey();

    /**
     * @brief Set paired status.
     *
     * @param status Paired status.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool SetPairedStatus(int status);

    /**
     * @brief Save paired device info.
     *
     */
    void SavePairedDeviceInfo();

    /**
     * @brief Remote paired device info.
     *
     */
    void RemotePairedInfo();

    void SetNameNeedGet(bool nameUnknowned);
    bool GetNameNeedGet() const;
    bool CheckCod(uint32_t cod) const;

private:
    bool bondedFromLocal_ {};
    bool nameNeedGet_ {};
    bool aclConnected_ {};
    uint8_t flags_ {};
    uint8_t txPower_ {};

    int cod_ {};
    int rssi_ {};
    int deviceType_ {REMOTE_TYPE_BREDR};
    int connectionHandle_ {};
    int pairState_ {};
    int pairConfirmType_ {PAIR_CONFIRM_TYPE_INVALID};
    int pairConfirmState_ {PAIR_CONFIRM_STATE_INVALID};
    int linkKeyType_ {PAIR_INVALID_LINK_KEY_TYPE};
    int ioCapability_ {};
    int batteryLevel_ {};

    std::string macAddr_ {INVALID_MAC_ADDRESS};
    std::string deviceName_ {INVALID_NAME};
    std::string aliasName_ {INVALID_NAME};
    std::string uri_ {};
    std::vector<uint8_t> manuSpecData_ {};
    std::vector<uint8_t> linkKey_ {};
    std::vector<Uuid> uuids_ {};

    BT_DISALLOW_COPY_AND_ASSIGN(ClassicRemoteDevice);
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // REMOTE_DEVICE_H