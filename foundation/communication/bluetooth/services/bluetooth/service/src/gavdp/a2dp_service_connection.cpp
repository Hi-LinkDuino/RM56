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

#include "a2dp_service_connection.h"

#include "a2dp_service.h"
#include "log.h"
#include "log_util.h"

namespace OHOS {
namespace bluetooth {
A2dpConnectManager::A2dpConnectManager(uint8_t role)
{
    role_ = role;
}

bool A2dpConnectManager::A2dpConnect(const RawAddress &device)
{
    HILOGI("[address:%{public}s] role[%{public}u]", GET_ENCRYPT_ADDR(device), role_);

    A2dpService *service = GetServiceInstance(role_);
    A2dpDeviceInfo *info = nullptr;

    if (service == nullptr) {
        LOG_ERROR("[A2dpConnectManager] %{public}s Can't get the instance of service\n", __func__);
        return false;
    }

    for (auto bdr : service->GetDeviceList()) {
        if (bdr.first == device.GetAddress().c_str()) {
            info = bdr.second;
            break;
        }
    }

    if (info == nullptr) {
        info = AddDevice(device, static_cast<int>(BTConnectState::DISCONNECTED));
    }

    if (info == nullptr) {
        LOG_ERROR("[A2dpConnectManager] %{public}s role[%u] Can't add new device\n", __func__, role_);
        return false;
    }
    RawAddress deviceObj = device;
    utility::Message msg(A2DP_MSG_CONNECT, role_, &deviceObj);

    info->SetConnectState(static_cast<int>(BTConnectState::CONNECTING));
    info->GetStateMachine()->ProcessMessage(msg);

    return true;
}

bool A2dpConnectManager::A2dpDisconnect(const RawAddress &device)
{
    LOG_INFO("[A2dpConnectManager] %{public}s\n", __func__);

    A2dpService *service = GetServiceInstance(role_);

    if (service == nullptr) {
        LOG_ERROR("[A2dpConnectManager] %{public}s Can't get the instance of service\n", __func__);
        return false;
    }

    A2dpDeviceInfo *deviceInfo = service->GetDeviceFromList(device);
    if (deviceInfo == nullptr) {
        LOG_ERROR("[A2dpConnectManager] %{public}s role[%u] Not find the device\n", __func__, role_);
        return false;
    }
    // send message to statemachine
    utility::Message msg(A2DP_MSG_DISCONNECT, role_, &const_cast<RawAddress &>(device));

    deviceInfo->SetConnectState(static_cast<int>(BTConnectState::DISCONNECTING));
    deviceInfo->GetStateMachine()->ProcessMessage(msg);

    return true;
}

bool A2dpConnectManager::JudgeConnectedNum() const
{
    LOG_INFO("[A2dpConnectManager] %{public}s\n", __func__);

    A2dpService *service = GetServiceInstance(role_);

    if (service == nullptr) {
        LOG_ERROR("[A2dpConnectManager] %{public}s Can't get the instance of service\n", __func__);
        return false;
    }

    bool ret = false;
    int connectCnt = 0;

    if (service->GetDeviceList().empty()) {
        ret = true;
        return ret;
    } else {
        for (auto itr : service->GetDeviceList()) {
            LOG_INFO("[connectCnt] %u\n", connectCnt);
            int connectionState = 0;
            connectionState = itr.second->GetConnectState();
            if (connectionState == static_cast<int>(BTConnectState::CONNECTED)) {
                connectCnt++;
            }
        }
    }

    LOG_INFO("[connectCnt] %u\n", connectCnt);
    ret = ((connectCnt >= service->GetMaxConnectNum()) ? false : true);
    return ret;
}

A2dpDeviceInfo *A2dpConnectManager::AddDevice(const RawAddress &device, int state)
{
    LOG_INFO("[A2dpConnectManager] %{public}s role[%u]\n", __func__, role_);

    A2dpDeviceInfo *deviceInfo = nullptr;

    A2dpService *service = GetServiceInstance(role_);
    if (service == nullptr) {
        LOG_INFO("[A2dpConnectManager] %{public}s Can't get the instance of service\n", __func__);
        return deviceInfo;
    }

    auto peerDevice = std::make_unique<A2dpDeviceInfo>(device);
    deviceInfo = peerDevice.release();
    if (deviceInfo != nullptr) {
        deviceInfo->GetStateMachine()->SetRole(role_);
        deviceInfo->SetConnectState(state);
        service->AddDeviceToList(device.GetAddress().c_str(), deviceInfo);
    }

    return deviceInfo;
}

void A2dpConnectManager::DeleteDevice(const RawAddress &device)
{
    LOG_INFO("[A2dpConnectManager] %{public}s role[%u]\n", __func__, role_);
    A2dpService *service = GetServiceInstance(role_);
    if (service == nullptr) {
        LOG_INFO("[A2dpConnectManager] %{public}s Can't get the instance of service\n", __func__);
        return;
    }
    service->DeleteDeviceFromList(device);
}

bool A2dpConnectManager::JudgeConnectExit(const RawAddress &device, uint8_t role)
{
    LOG_INFO("[A2dpConnectManager] %{public}s\n", __func__);

    A2dpService *service = nullptr;
    A2dpDeviceInfo *info = nullptr;
    bool ret = false;

    if (role == A2DP_ROLE_SOURCE) {
        service = GetServiceInstance(A2DP_ROLE_SOURCE);
    } else {
        service = GetServiceInstance(A2DP_ROLE_SINK);
    }

    if (service != nullptr) {
        std::map<std::string, A2dpDeviceInfo *> devList = service->GetDeviceList();
        auto iter = devList.find(device.GetAddress().c_str());
        if (iter == devList.end()) {
            LOG_INFO("[A2dpService]Can't find the statemachine");
        } else {
            info = iter->second;
            if ((static_cast<int>(BTConnectState::CONNECTED) == info->GetConnectState()) ||
                (static_cast<int>(BTConnectState::CONNECTING) == info->GetConnectState())) {
                LOG_ERROR("[A2dpService]Device have been connected as source role");
                ret = true;
            }
        }
    }
    return ret;
}
}  // namespace bluetooth
}  // namespace OHOS