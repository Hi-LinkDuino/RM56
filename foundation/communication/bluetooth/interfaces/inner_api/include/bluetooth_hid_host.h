/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
 */

/**
 * @file bluetooth_hid_host.h
 *
 * @brief Declares HID HOST role framework functions, including basic and observer functions.
 *
 */
#ifndef BLUETOOTH_HID_HOST_H
#define BLUETOOTH_HID_HOST_H

#include <string>
#include <vector>
#include <memory>

#include "bluetooth_def.h"
#include "bluetooth_types.h"
#include "bluetooth_remote_device.h"
namespace OHOS {
namespace Bluetooth {
/**
 * @brief Class for Hid Host observer functions.
 *
 */
class HidHostObserver {
public:
    /**
     * @brief The observer function to notify connection state changed.
     *
     * @param device Remote device object.
     * @param state Connection state.
     */
    virtual void OnConnectionStateChanged(const BluetoothRemoteDevice &device, int state)
    {}

    /**
     * @brief Destroy the HidHostObserver object.
     *
     */
    virtual ~HidHostObserver()
    {}
};

/**
 * @brief Class for Hid Host API.
 *
 */
class BLUETOOTH_API HidHost {
public:
    /**
     * @brief Get the instance of HidHost object.
     *
     * @return Returns the pointer to the HidHost instance.
     */
    static HidHost *GetProfile();

    /**
     * @brief Get remote Hid device list which are in the specified states.
     *
     * @param states List of remote device states.
     * @return Returns the list of devices.
     */
    std::vector<BluetoothRemoteDevice> GetDevicesByStates(std::vector<int> states);

    /**
     * @brief Get the connection state of the specified remote Hid device.
     *
     * @param device Remote device object.
     * @return Returns the connection state of the remote device.
     */
    int GetDeviceState(const BluetoothRemoteDevice &device);

    /**
     * @brief Initiate the establishment of a service level connection to remote Hid device.
     *
     * @param device Remote device object.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     */
    bool Connect(const BluetoothRemoteDevice &device);

    /**
     * @brief Release the connection from remote Hid device.
     *
     * @param device Remote device object.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     */
    bool Disconnect(const BluetoothRemoteDevice &device);

        /**
     * @brief Register Hid Host observer instance.
     *
     * @param observer Hid Host observer instance.
     */
    void RegisterObserver(HidHostObserver *observer);

    /**
     * @brief Deregister Hid Host observer instance.
     *
     * @param observer Hid Host observer instance.
     */
    void DeregisterObserver(HidHostObserver *observer);

    /**
     * @brief Hid Host VCUnplug.
     *
     * @param observer Hid Host device id size type.
     */
    void HidHostVCUnplug(std::string device, uint8_t id, uint16_t size, uint8_t type);

    /**
     * @brief Hid Host Send Data.
     *
     * @param observer Hid Host device id size type.
     */
    void HidHostSendData(std::string device, uint8_t id, uint16_t size, uint8_t type);

    /**
     * @brief Hid Host Set Report.
     *
     * @param observer Hid Host device type size report.
     */
    void HidHostSetReport(std::string device, uint8_t type, uint16_t size, uint8_t report);

    /**
     * @brief Hid Host Get Report.
     *
     * @param observer Hid Host device id size type.
     */
    void HidHostGetReport(std::string device, uint8_t id, uint16_t size, uint8_t type);

    /**
     * @brief Static Hid Host observer instance.
     *
     */
    static HidHostObserver *instance_;

private:
    HidHost();
    ~HidHost();
    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(HidHost);
    BLUETOOTH_DECLARE_IMPL();
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // BLUETOOTH_HID_HOST_H