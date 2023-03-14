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
 * @brief Defines basic adapter for classic adapter and ble adapter.
 *
 * @since 6
 */

/**
 * @file interface_adapter.h
 *
 * @brief basic adapter interface.
 *
 * @since 6
 */

#ifndef INTERFACE_ADAPTER_H
#define INTERFACE_ADAPTER_H

#include <string>
#include <vector>

#include "bt_def.h"
#include "bt_uuid.h"
#include "raw_address.h"

/**
 * @brief bluetooth adapter name Define
 */
const std::string ADAPTER_NAME_CLASSIC = "ClassicAdapter";
const std::string ADAPTER_NAME_BLE = "BleAdapter";

/**
 * @brief forward declaration for class Context in namespace utility
 */
namespace utility {
class Context;
}

namespace OHOS {
namespace bluetooth {
/**
 * @brief Represents basic adapter for classic and ble, including the common functions.
 *
 * @since 6
 */
class IAdapter {
public:
    /**
     * @brief A destructor used to delete the <b>IAdapter</b> instance.
     *
     * @since 6
     */
    virtual ~IAdapter() = default;

    /// gap
    /**
     * @brief Get local device address.
     *
     * @return Returns local device address.
     * @since 6
     */
    virtual std::string GetLocalAddress() const = 0;

    /**
     * @brief Get local device name.
     *
     * @return Returns local device name.
     * @since 6
     */
    virtual std::string GetLocalName() const = 0;

    /**
     * @brief Set local device name.
     *
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool SetLocalName(const std::string &name) const = 0;

    /**
     * @brief Set local device bondable mode.
     *
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool SetBondableMode(int mode) const = 0;

    /**
     * @brief Get local device bondable mode.
     *
     * @return Returns local device bondable mode.
     * @since 6
     */
    virtual int GetBondableMode() const = 0;

    /// remote device information
    /**
     * @brief Get remote device type.
     *
     * @param device Remote device address.
     * @return Returns remote device type.
     * @since 6
     */
    virtual int GetDeviceType(const RawAddress &device) const = 0;

    /**
     * @brief Get remote device name.
     *
     * @param device Remote device address.
     * @return Returns remote device name.
     * @since 6
     */
    virtual std::string GetDeviceName(const RawAddress &device) const = 0;

    /**
     * @brief Get remote device uuids.
     *
     * @param device Remote device address.
     * @return Returns remote device uuids vector.
     * @since 6
     */
    virtual std::vector<Uuid> GetDeviceUuids(const RawAddress &device) const = 0;

    /// pair
    /**
     * @brief Get paired devices.
     *
     * @return Returns paired devices vector.
     * @since 6
     */
    virtual std::vector<RawAddress> GetPairedDevices() const = 0;

    /**
     * @brief Get remote device uuids.
     *
     * @param device Remote device address.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool StartPair(const RawAddress &device) = 0;

    /**
     * @brief Check if device was bonded from local.
     *
     * @param device Remote device address.
     * @return Returns <b>true</b> if device was bonded from local;
     *         returns <b>false</b> if device was not bonded from local.
     * @since 6
     */
    virtual bool IsBondedFromLocal(const RawAddress &device) const = 0;

    /**
     * @brief Cancel pair operation.
     *
     * @param device Remote device address.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool CancelPairing(const RawAddress &device) = 0;

    /**
     * @brief Remove pair.
     *
     * @param device Remote device address.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool RemovePair(const RawAddress &device) = 0;

    /**
     * @brief Remove all pairs.
     *
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool RemoveAllPairs() = 0;

    /**
     * @brief Get device pair state.
     *
     * @param device Remote device address.
     * @return Returns device pair state.
     * @since 6
     */
    virtual int GetPairState(const RawAddress &device) const = 0;

    /**
     * @brief Set device pairing confirmation.
     *
     * @param device Remote device address.
     * @param accept Set gap accept flag.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool SetDevicePairingConfirmation(const RawAddress &device, bool accept) const = 0;

    /**
     * @brief Set device pair passkey.
     *
     * @param device Remote device address.
     * @param passkey Device passkey.
     * @param accept Set gap accept flag.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool SetDevicePasskey(const RawAddress &device, int passkey, bool accept) const = 0;

    /**
     * @brief Check device pair request reply.
     *
     * @param device Remote device address.
     * @param accept Set gap accept flag.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool PairRequestReply(const RawAddress &device, bool accept) const = 0;

    /// other
    /**
     * @brief Check if device acl connected.
     *
     * @param device Remote device address.
     * @return Returns <b>true</b> if device acl connected;
     *         returns <b>false</b> if device does not acl connect.
     * @since 6
     */
    virtual bool IsAclConnected(const RawAddress &device) const = 0;

    /**
     * @brief Check if device acl Encrypted.
     *
     * @param device Remote device address.
     * @return Returns <b>true</b> if device acl Encrypted;
     *         returns <b>false</b> if device does not acl Encrypt.
     * @since 6
     */
    virtual bool IsAclEncrypted(const RawAddress &device) const = 0;

    /**
     * @brief Get utility::Context pointer for adapter.
     *
     * @return Returns the pointer for adapter.
     * @since 6
     */
    virtual utility::Context *GetContext() = 0;
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // INTERFACE_ADAPTER_H