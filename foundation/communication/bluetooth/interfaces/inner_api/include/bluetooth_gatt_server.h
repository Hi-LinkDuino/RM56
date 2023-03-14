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
 * @file bluetooth_gatt_server.h
 *
 * @brief gatt server interface.
 *
 * @since 6
 *
 */

#ifndef BLUETOOTH_GATT_SERVER_H
#define BLUETOOTH_GATT_SERVER_H

#include "bluetooth_def.h"
#include "bluetooth_gatt_service.h"
#include "bluetooth_remote_device.h"

namespace OHOS {
namespace Bluetooth {
/**
 * @brief Class for Gatt Server callback functions.
 *
 * @since 6
 *
 */
class GattServerCallback {
public:
    /**
     * @brief The callback function to notify connection state update.
     *
     * @param device Remote device object.
     * @param state Connection state.
     * @since 6
     *
     */
    virtual void OnConnectionStateUpdate(const BluetoothRemoteDevice &device, int state) = 0;

    /**
     * @brief The callback function to notify service add.
     *
     * @param Service Added service object.
     * @param ret Result of service add.
     * @since 6
     *
     */
    virtual void OnServiceAdded(GattService *Service, int ret)
    {}

    /**
     * @brief The callback function to notify characteristic read request.
     *
     * @param device Remote device object.
     * @param characteristic Characteristic object.
     * @param requestId Result of request.
     * @since 6
     *
     */
    virtual void OnCharacteristicReadRequest(
        const BluetoothRemoteDevice &device, GattCharacteristic &characteristic, int requestId)
    {}

    /**
     * @brief The callback function to notify characteristic write request.
     *
     * @param device Remote device object.
     * @param characteristic Characteristic object.
     * @param requestId Result of request.
     * @since 6
     *
     */
    virtual void OnCharacteristicWriteRequest(
        const BluetoothRemoteDevice &device, GattCharacteristic &characteristic, int requestId)
    {}

    /**
     * @brief The callback function to notify descriptor read request.
     *
     * @param device Remote device object.
     * @param characteristic Characteristic object.
     * @param requestId Result of request.
     * @since 6
     *
     */
    virtual void OnDescriptorReadRequest(const BluetoothRemoteDevice &device, GattDescriptor &descriptor, int requestId)
    {}

    /**
     * @brief The callback function to notify descriptor write request.
     *
     * @param device Remote device object.
     * @param characteristic Characteristic object.
     * @param requestId Result of request.
     * @since 6
     *
     */
    virtual void OnDescriptorWriteRequest(
        const BluetoothRemoteDevice &device, GattDescriptor &descriptor, int requestId)
    {}

    /**
     * @brief The callback function to notify mtu update.
     *
     * @param device Remote device object.
     * @param mtu Current mtu.
     * @since 6
     *
     */
    virtual void OnMtuUpdate(const BluetoothRemoteDevice &device, int mtu)
    {}
    /**
     * @brief The callback function to notify characteristic changed.
     *
     * @param device Remote device object.
     * @since 6
     *
     */
    virtual void OnNotificationCharacteristicChanged(const BluetoothRemoteDevice &device, int result)
    {}
    /**
     * @brief The callback function to notify connection parameter changed
     *
     * @param device Remote device object.
     * @param interval Interval object.
     * @param latency Latency object.
     * @param timeout Timeout object.
     * @param status Status object.
     * @since 6
     *
     */
    virtual void OnConnectionParameterChanged(
        const BluetoothRemoteDevice &device, int interval, int latency, int timeout, int status)
    {}

    /**
     * @brief A destructor of GattServerCallback.
     *
     * @since 6
     *
     */
    virtual ~GattServerCallback()
    {}
};

/**
 * @brief Class for Gatt Server API.
 *
 * @since 6
 *
 */
class BLUETOOTH_API GattServer {
public:
    /**
     * @brief A constructor of GattServerCallback.
     *
     * @param device GattServerCallback callback object.
     * @since 6
     *
     */
    explicit GattServer(GattServerCallback &callback);
    /**
     * @brief The function to add service.
     *
     * @param service Service object to add.
     * @return int    api accept status.
     * @since 6
     *
     */
    int AddService(GattService &service);
    /**
     * @brief The function to remove service.
     *
     * @param service Service object to remove.
     * @return int    api accept status.
     * @since 6
     *
     */
    int RemoveGattService(const GattService &service);
    /**
     * @brief The function to clear all services.
     *
     * @since 6
     *
     */
    void ClearServices();
    /**
     * @brief The function to clear all services.
     *
     * @return int.
     * @since 6
     *
     */
    int Close();
    /**
     * @brief The function to get service by UUID.
     *
     * @param uuid UUID of service.
     * @param isPrimary Type of service.
     * @return service.
     * @since 6
     *
     */
    std::optional<std::reference_wrapper<GattService>> GetService(const UUID &uuid, bool isPrimary);
    /**
     * @brief The function to get all services.
     *
     * @return list of services.
     * @since 6
     *
     */
    std::list<GattService> &GetServices();
    /**
     * @brief The function to notify characteristic change.
     *
     * @param device Remote device object.
     * @param characteristic Characteristic object.
     * @param confirm Confirm the change.
     * @return int    api accept status.
     * @since 6
     *
     */
    int NotifyCharacteristicChanged(
        const BluetoothRemoteDevice &device, const GattCharacteristic &characteristic, bool confirm);
    /**
     * @brief The function to send responce.
     *
     * @param device Remote device object.
     * @param requestId Result of the request.
     * @param status Current status.
     * @param offset Offset object.
     * @param value Value object.
     * @param length Length of value.
     * @return int   api accept status.
     * @since 6
     *
     */
    int SendResponse(
        const BluetoothRemoteDevice &device, int requestId, int status, int offset, const uint8_t *value, int length);

    /**
     * @brief The function to cancel connection.
     *
     * @param device Remote device object.
     * @since 6
     *
     */
    void CancelConnection(const BluetoothRemoteDevice &device);

    /**
     * @brief A destructor of GattServer.
     *
     * @since 6
     *
     */
    ~GattServer();
    /**
     * @brief The function to delete constructor of GattServer.
     *
     * @since 6
     *
     */
    GattServer() = delete;

    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(GattServer);

private:
    BLUETOOTH_DECLARE_IMPL();
};
} // namespace Bluetooth
} // namespace OHOS
#endif  // BLUETOOTH_GATT_SERVER_H
