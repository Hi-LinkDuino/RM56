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
 * @file interface_profile_gatt_server.h
 *
 * @brief Declares GATT server role interface profile functions.
 *
 * @since 6
 *
 */
#ifndef INTERFACE_PROFILE_GATT_SERVER_H
#define INTERFACE_PROFILE_GATT_SERVER_H

#include "interface_profile.h"
#include "bt_def.h"
#include "gatt_data.h"
#include <vector>

namespace OHOS {
namespace bluetooth {
/**
 * @brief Class for IGattServerCallback functions.
 *
 * @since 6
 *
 */
class IGattServerCallback {
public:
    /**
     * @brief The callback function to notify characteristic read.
     *
     * @param device Remote device object.
     * @param characteristic Characteristic object.
     * @return int callback accept status.
     * @since 6
     *
     */
    virtual void OnCharacteristicReadRequest(const GattDevice &device, const Characteristic &characteristic) {}

    virtual void OnCharacteristicReadByUuidRequest(const GattDevice &device, const Characteristic &characteristic) {}

    /**
     * @brief The callback function to notify characteristic write.
     *
     * @param device Remote device object.
     * @param characteristic Characteristic object.
     * @param needRespones Respones status.
     * @return int callback accept status.
     * @since 6
     *
     */
    virtual void OnCharacteristicWriteRequest(
        const GattDevice &device, const Characteristic &characteristic, bool needRespones) {}

    /**
     * @brief The callback function to notify descriptor read.
     *
     * @param device Remote device object.
     * @param descriptor Descriptor object.
     * @return int callback accept status.
     * @since 6
     *
     */
    virtual void OnDescriptorReadRequest(const GattDevice &device, const Descriptor &descriptor) {}

    /**
     * @brief The callback function to notify descriptor write.
     *
     * @param device Remote device object.
     * @param descriptor Descriptor object.
     * @return int callback accept status.
     * @since 6
     *
     */
    virtual void OnDescriptorWriteRequest(const GattDevice &device, const Descriptor &descriptor) {}

    /**
     * @brief The callback function to notify confirm notify status.
     *
     * @param device Remote device object.
     * @param characteristic Characteristic object.
     * @since 6
     *
     */
    virtual void OnNotifyConfirm(const GattDevice &device, const Characteristic &characteristic, int result) {}
    /**
     * @brief The callback function to notify connection state changed.
     *
     * @param device Remote device object.
     * @param ret Result of the callback.
     * @param state Current state.
     * @since 6
     *
     */
    virtual void OnConnectionStateChanged(const GattDevice &device, int ret, int state) = 0;
    /**
     * @brief The callback function to notify mtu changed.
     *
     * @param device Remote device object.
     * @param mtu Current mtu.
     * @since 6
     *
     */
    virtual void OnMtuChanged(const GattDevice &device, int mtu) {}
    /**
     * @brief The callback function to notify service add.
     *
     * @param ret Result of the callback.
     * @param services Service object.
     * @since 6
     *
     */
    virtual void OnAddService(int ret, const Service &services) {}

    virtual void OnServiceChanged(const Service &services) {}

    virtual void OnConnectionParameterChanged(
        const GattDevice &device, int interval, int latency, int timeout, int status) {}

    /**
     * @brief Destroy the IGattServerCallback.
     *
     * @since 6
     *
     */
    virtual ~IGattServerCallback()
    {}
};

/**
 * @brief Class for IProfileGattServer functions.
 *
 * @since 6
 *
 */
class IProfileGattServer : public IProfile {
public:
    /**
     * @brief The function to register application.
     *
     * @param callback Callback object.
     * @return int api accept status.
     * @since 6
     *
     */
    virtual int RegisterApplication(std::shared_ptr<IGattServerCallback> callback) = 0;
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
     * @brief The function to add service.
     *
     * @param appId Application id.
     * @param services Services object.
     * @return int api accept status.
     * @since 6
     *
     */
    virtual int AddService(int appId, Service &service) = 0;
    /**
     * @brief The function to remove service.
     *
     * @param appId Application id.
     * @param services Services object.
     * @return int api accept status.
     * @since 6
     *
     */
    virtual int RemoveService(int appId, const Service &services) = 0;
    /**
     * @brief The function to clear services.
     *
     * @param appId Application id.
     * @return int api accept status.
     * @since 6
     *
     */
    virtual int ClearServices(int appId) = 0;
    /**
     * @brief The function to notify client.
     *
     * @param device Remote device object.
     * @param characteristic Characteristic object.
     * @param needConfirm Confirm need status.
     * @return int api accept status.
     * @since 6
     *
     */
    virtual int NotifyClient(const GattDevice &device, Characteristic &characteristic, bool needConfirm = false) = 0;
    /**
     * @brief The function to respond characteristic read.
     *
     * @param device Remote device object.
     * @param characteristic Characteristic object.
     * @param ret Result of the callback.
     * @return int api accept status.
     * @since 6
     *
     */
    virtual int RespondCharacteristicRead(const GattDevice &device, Characteristic &characteristic, int ret) = 0;

    virtual int RespondCharacteristicReadByUuid(const GattDevice &device, Characteristic &characteristic, int ret) = 0;
    /**
     * @brief The function to respond characteristic write.
     *
     * @param device Remote device object.
     * @param characteristic Characteristic object.
     * @param ret Result of the callback.
     * @return int api accept status.
     * @since 6
     *
     */
    virtual int RespondCharacteristicWrite(const GattDevice &device, const Characteristic &characteristic, int ret) = 0;
    /**
     * @brief The function to respond descriptor read.
     *
     * @param device Remote device object.
     * @param descriptor Descriptor object.
     * @param ret Result of the callback.
     * @return int api accept status.
     * @since 6
     *
     */
    virtual int RespondDescriptorRead(const GattDevice &device, Descriptor &descriptor, int ret) = 0;
    /**
     * @brief The function to respond descriptor write.
     *
     * @param device Remote device object.
     * @param descriptor Descriptor object.
     * @param ret Result of the callback.
     * @return int api accept status.
     * @since 6
     *
     */
    virtual int RespondDescriptorWrite(const GattDevice &device, const Descriptor &descriptor, int ret) = 0;
    /**
     * @brief The function to cancel connection.
     *
     * @param device Remote device object.
     * @return int api accept status.
     * @since 6
     *
     */
    virtual int CancelConnection(const GattDevice &device) = 0;

private:
    using IProfile::Connect;
    using IProfile::Disconnect;
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // INTERFACE_PROFILE_GATT_SERVER_H