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
#include "wifi_p2p_device_manager.h"
#include "wifi_logger.h"

namespace OHOS {
namespace Wifi {
void WifiP2pDeviceManager::Initialize()
{}

bool WifiP2pDeviceManager::AddDevice(const WifiP2pDevice &device)
{
    if (!device.IsValid()) {
        return false;
    }
    std::unique_lock<std::mutex> lock(deviceMutex);
    for (auto it = p2pDevices.begin(); it != p2pDevices.end(); it++) {
        if (*it == device) {
            return false;
        }
    }
    p2pDevices.push_back(device);
    return true;
}

bool WifiP2pDeviceManager::RemoveDevice(const std::string &deviceAddress)
{
    std::unique_lock<std::mutex> lock(deviceMutex);
    for (auto it = p2pDevices.begin(); it != p2pDevices.end(); it++) {
        if (it->GetDeviceAddress() == deviceAddress) {
            p2pDevices.erase(it);
            return true;
        }
    }
    return false;
}

bool WifiP2pDeviceManager::RemoveDevice(const WifiP2pDevice &device)
{
    return RemoveDevice(device.GetDeviceAddress());
}

int WifiP2pDeviceManager::ClearAll()
{
    std::unique_lock<std::mutex> lock(deviceMutex);
    int num = p2pDevices.size();
    p2pDevices.clear();
    return num;
}

int WifiP2pDeviceManager::GetDevicesList(std::vector<WifiP2pDevice> &devices)
{
    std::unique_lock<std::mutex> lock(deviceMutex);
    devices.assign(p2pDevices.begin(), p2pDevices.end());
    return p2pDevices.size();
}

bool WifiP2pDeviceManager::UpdateDevice(const WifiP2pDevice &device)
{
    if (!device.IsValid()) {
        return false;
    }
    std::unique_lock<std::mutex> lock(deviceMutex);
    for (auto it = p2pDevices.begin(); it != p2pDevices.end(); it++) {
        if (*it == device) {
            *it = device;
            return true;
        }
    }
    p2pDevices.push_back(device);
    return true;
}

bool WifiP2pDeviceManager::UpdateDeviceSupplicantInf(const WifiP2pDevice &device)
{
    if (!device.IsValid()) {
        return false;
    }
    std::unique_lock<std::mutex> lock(deviceMutex);
    for (auto it = p2pDevices.begin(); it != p2pDevices.end(); it++) {
        if (*it == device) {
            it->SetDeviceName(device.GetDeviceName());
            it->SetPrimaryDeviceType(device.GetPrimaryDeviceType());
            it->SetSecondaryDeviceType(device.GetSecondaryDeviceType());
            it->SetWpsConfigMethod(device.GetWpsConfigMethod());
            it->SetDeviceCapabilitys(device.GetDeviceCapabilitys());
            it->SetGroupCapabilitys(device.GetGroupCapabilitys());
            return true;
        }
    }
    /* add its if not found . be careful of the return value */
    p2pDevices.push_back(device);
    return true;
}

bool WifiP2pDeviceManager::UpdateDeviceGroupCap(const std::string &deviceAddress, uint32_t cap)
{
    if (deviceAddress.empty()) {
        return false;
    }
    std::unique_lock<std::mutex> lock(deviceMutex);
    for (auto it = p2pDevices.begin(); it != p2pDevices.end(); it++) {
        if (it->GetDeviceAddress() == deviceAddress) {
            it->SetGroupCapabilitys(cap);
            return true;
        }
    }
    return false;
}

bool WifiP2pDeviceManager::UpdateDeviceGroupCap(const WifiP2pDevice &device)
{
    if (!device.IsValid()) {
        return false;
    }

    return UpdateDeviceGroupCap(device.GetDeviceAddress(), device.GetGroupCapabilitys());
}

bool WifiP2pDeviceManager::UpdateDeviceStatus(const std::string &deviceAddress, P2pDeviceStatus status)
{
    if (deviceAddress.empty()) {
        return false;
    }

    std::unique_lock<std::mutex> lock(deviceMutex);
    for (auto it = p2pDevices.begin(); it != p2pDevices.end(); it++) {
        if (it->GetDeviceAddress() == deviceAddress) {
            it->SetP2pDeviceStatus(status);
            return true;
        }
    }
    return false;
}

bool WifiP2pDeviceManager::UpdateDeviceStatus(const WifiP2pDevice &device)
{
    if (!device.IsValid()) {
        return false;
    }

    return UpdateDeviceStatus(device.GetDeviceAddress(), device.GetP2pDeviceStatus());
}

WifiP2pDevice WifiP2pDeviceManager::GetDevices(const std::string &deviceAddress)
{
    std::unique_lock<std::mutex> lock(deviceMutex);
    for (auto it = p2pDevices.begin(); it != p2pDevices.end(); it++) {
        if (it->GetDeviceAddress() == deviceAddress) {
            return *it;
        }
    }
    WifiP2pDevice ret;
    return ret;
}

const std::string WifiP2pDeviceManager::GetDeviceName(const std::string &deviceAddress)
{
    WifiP2pDevice device = GetDevices(deviceAddress);
    return device.IsValid() ? device.GetDeviceName() : deviceAddress;
}
}  // namespace Wifi
}  // namespace OHOS
