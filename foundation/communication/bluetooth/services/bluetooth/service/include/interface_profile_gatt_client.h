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
 *
 */

/**
 * @file interface_profile_gatt_client.h
 *
 * @brief Declares GATT client role interface profile functions.
 *
 * @since 6
 *
 */
#ifndef INTERFACE_PROFILE_GATT_CLIENT_H
#define INTERFACE_PROFILE_GATT_CLIENT_H

#include "interface_profile.h"
#include "gatt_data.h"
#include <vector>

namespace OHOS {
namespace bluetooth {
/**
 * @brief Class for IGattClientCallback functions.
 *
 * @since 6
 *
 */
class IGattClientCallback {
public:
    /**
     * @brief The callback function to notify services changed.
     *
     * @param services Service list.
     * @since 6
     *
     */
    virtual void OnServicesChanged(const std::vector<Service> &services) = 0;
    /**
     * @brief The callback function to notify characteristic read.
     *
     * @param ret Result of the callback.
     * @param characteristic Characteristic object.
     * @since 6
     *
     */
    virtual void OnCharacteristicRead(int ret, const Characteristic &characteristic) = 0;
    /**
     * @brief The callback function to notify characteristic write.
     *
     * @param ret Result of the callback.
     * @param characteristic Characteristic object.
     * @since 6
     *
     */
    virtual void OnCharacteristicWrite(int ret, const Characteristic &characteristic) = 0;
    /**
     * @brief The callback function to notify characteristic changed.
     *
     * @param characteristic Characteristic object.
     * @param needConfirm Status of confirm need.
     * @since 6
     *
     */
    virtual void OnCharacteristicChanged(const Characteristic &characteristic) = 0;
    /**
     * @brief The callback function to notify descriptor read.
     *
     * @param ret Result of the callback.
     * @param descriptor Descriptor object.
     * @since 6
     *
     */
    virtual void OnDescriptorRead(int ret, const Descriptor &descriptor) = 0;
    /**
     * @brief The callback function to notify descriptor write.
     *
     * @param ret Result of the callback.
     * @param descriptor Descriptor object.
     * @since 6
     *
     */
    virtual void OnDescriptorWrite(int ret, const Descriptor &descriptor) = 0;
    /**
     * @brief The callback function to notify connection state changed.
     *
     * @param state Current state.
     * @param newState New state.
     * @since 6
     *
     */
    virtual void OnConnectionStateChanged(int state, int newState) = 0;
    /**
     * @brief The callback function to notify mtu changed.
     *
     * @param state Current state.
     * @param mtu Current mtu.
     * @since 6
     *
     */
    virtual void OnMtuChanged(int state, int mtu) = 0;

    virtual void OnServicesDiscovered(int status) = 0;
    /**
     * @brief Destroy the IGattClientCallback.
     *
     * @since 6
     *
     */
    virtual ~IGattClientCallback()
    {}

    virtual void OnConnectionParameterChanged(int interval, int latency, int timeout, int status) = 0;
};

/**
 * @brief Class for IProfileGattClient functions.
 *
 * @since 6
 *
 */
class IProfileGattClient : public IProfile {
public:
    /**
     * @brief The function to register application.
     *
     * @param callback Callback object.
     * @param addr Address object.
     * @param transport Transport type.
     * @return int api accept status.
     * @since 6
     *
     */
    virtual int RegisterApplication(IGattClientCallback &callback, const RawAddress &addr, uint8_t transport) = 0;

    virtual int RegisterSharedApplication(IGattClientCallback &callback, const RawAddress &addr, uint8_t transport) = 0;
    /**
     * @brief The function to deregister application.
     *
     * @param appId Application id.
     * @return int api accept status.
     * @since 6
     *
     */
    virtual int DeregisterApplication(int appId) = 0;
    /**
     * @brief The function to connect.
     *
     * @param appId Application id.
     * @return int api accept status.
     * @since 6
     *
     */
    virtual int Connect(int appId, bool autoConnect) = 0;
    /**
     * @brief The function to disconnect.
     *
     * @param appId Application id.
     * @return int api accept status.
     * @since 6
     *
     */
    virtual int Disconnect(int appId) = 0;
    /**
     * @brief The function to discovery services.
     *
     * @param appId Application id.
     * @return int api accept status.
     * @since 6
     *
     */
    virtual int DiscoveryServices(int appId) = 0;
    /**
     * @brief The function to read characteristic.
     *
     * @param appId Application id.
     * @param characteristic Characteristic object.
     * @return int api accept status.
     * @since 6
     *
     */
    virtual int ReadCharacteristic(int appId, const Characteristic &characteristic) = 0;

    virtual int ReadCharacteristicByUuid(int appId, const Uuid &uuid) = 0;
    /**
     * @brief The function to write characteristic.
     *
     * @param appId Application id.
     * @param characteristic Characteristic object.
     * @param withoutRespond Respond status.
     * @return int api accept status.
     * @since 6
     *
     */
    virtual int WriteCharacteristic(int appId, Characteristic &characteristic, bool withoutRespond = false) = 0;
    /**
     * @brief The function to signed write characteristic.
     *
     * @param appId Application id.
     * @param characteristic Characteristic object.
     * @param withoutRespond Respond status.
     * @return int api accept status.
     * @since 6
     *
     */
    virtual int SignedWriteCharacteristic(int appId, Characteristic &characteristic) = 0;

    /**
     * @brief The function to read descriptor.
     *
     * @param appId Application id.
     * @param descriptor Descriptor object.
     * @return int api accept status.
     * @since 6
     *
     */
    virtual int ReadDescriptor(int appId, const Descriptor &descriptor) = 0;
    /**
     * @brief The function to write descriptor.
     *
     * @param appId Application id.
     * @param descriptor Descriptor object.
     * @return int api accept status.
     * @since 6
     *
     */
    virtual int WriteDescriptor(int appId, Descriptor &descriptor) = 0;
    /**
     * @brief The function to request exchange mtu.
     *
     * @param appId Application id.
     * @param mtu Expected mtu.
     * @return int api accept status.
     * @since 6
     *
     */
    virtual int RequestExchangeMtu(int appId, int mtu) = 0;
    /**
     * @brief The function to get all devices.
     *
     * @return std::vector<GattDevice> devices list.
     * @since 6
     *
     */
    virtual std::vector<GattDevice> GetAllDevice() = 0;

    virtual int RequestConnectionPriority(int appId, int connPriority) = 0;

    virtual std::vector<Service> GetServices(int appId) = 0;

private:
    using IProfile::Connect;
    using IProfile::Disconnect;
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // INTERFACE_PROFILE_GATT_CLIENT_H