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

#include "a2dp_service.h"

#include <cstring>
#include "adapter_config.h"
#include "class_creator.h"
#include "log.h"
#include "log_util.h"
#include "profile_config.h"
#include "profile_service_manager.h"
#include "securec.h"
#include "idevmgr_hdi.h"

constexpr const char *AUDIO_BLUETOOTH_SERVICE_NAME = "audio_bluetooth_hdi_service";

namespace OHOS {
namespace bluetooth {
std::recursive_mutex g_a2dpServiceMutex {};
ObserverProfile::ObserverProfile(uint8_t role)
{
    role_ = role;
}

void ObserverProfile::OnConnectStateChanged(const BtAddr &addr, const int state, void *context)
{
    LOG_INFO("[ObserverProfile] %{public}s: state[%{public}d] role[%u]\n", __func__, state, role_);

    A2dpService *service = GetServiceInstance(role_);
    A2dpProfile *pflA2dp = GetProfileInstance(role_);

    if (service == nullptr || pflA2dp == nullptr) {
        LOG_INFO("[ObserverProfile] %{public}s Can't get the instance of service\n", __func__);
        return;
    }

    RawAddress btAddr = bluetooth::RawAddress::ConvertToString(addr.addr);
    A2dpDeviceInfo *deviceInfo = service->GetDeviceFromList(btAddr);
    if (deviceInfo == nullptr) {
        if (((static_cast<CallbackParameter*>(context))->role) == A2DP_ROLE_ACP && (state == STREAM_CONNECTING)) {
            service->ConnectManager().AddDevice(btAddr, static_cast<int>(BTConnectState::CONNECTING));
            service->ProcessConnectFrameworkCallback(static_cast<int>(BTConnectState::CONNECTING), btAddr);
        }
        return;
    }
    int connectPolicy = service->GetConnectStrategy(btAddr);
    int connectState = ProcessConnectStateMessage(btAddr, deviceInfo, connectPolicy, state,
        (static_cast<CallbackParameter*>(context))->handle);
    HILOGI("Active device(%{public}s)", GetEncryptAddr(service->GetActiveSinkDevice().GetAddress()).c_str());

    if (connectState == static_cast<int>(BTConnectState::DISCONNECTED)) {
        if (strcmp(btAddr.GetAddress().c_str(), service->GetActiveSinkDevice().GetAddress().c_str()) == 0) {
            LOG_INFO("[ObserverProfile] %{public}s Remove the active device\n", __func__);
            RawAddress removeActive("");
            service->UpdateActiveDevice(removeActive);
        }
        service->ConnectManager().DeleteDevice(btAddr);
    } else if (connectState == static_cast<int>(BTConnectState::CONNECTED)) {
        LOG_INFO("[ObserverProfile] %{public}s Add the active device\n", __func__);
        service->UpdateActiveDevice(btAddr);
    }

    if ((connectState == static_cast<int>(BTConnectState::CONNECTED)) ||
        (connectState == static_cast<int>(BTConnectState::DISCONNECTED))) {
        service->ProcessConnectFrameworkCallback(connectState, btAddr);
        ProcessA2dpHdfLoad(connectState);
    }

    service->CheckDisable();
    return;
}

void ObserverProfile::ProcessA2dpHdfLoad(const int state) const
{
    LOG_INFO("[ObserverProfile] %{public}s state:%{public}d \n", __func__, state);
    A2dpService *service = GetServiceInstance(role_);
    std::vector<int> states = {static_cast<int>(BTConnectState::CONNECTED)};
    std::vector<RawAddress> devices = service->GetDevicesByStates(states);

    if (state == static_cast<int>(BTConnectState::CONNECTED) && devices.size() == 1) {
        auto devmgr = OHOS::HDI::DeviceManager::V1_0::IDeviceManager::Get();
        if (devmgr != nullptr) {
            LOG_INFO("[ObserverProfile] %{public}s, loadDevice of a2dp HDF", __func__);
            devmgr->LoadDevice(AUDIO_BLUETOOTH_SERVICE_NAME);
        }
    }
    if (state == static_cast<int>(BTConnectState::DISCONNECTED) && devices.size() == 0) {
        auto devmgr = OHOS::HDI::DeviceManager::V1_0::IDeviceManager::Get();
        if (devmgr != nullptr) {
            LOG_INFO("[ObserverProfile] %{public}s, UnloadDevice of a2dp HDF", __func__);
            devmgr->UnloadDevice(AUDIO_BLUETOOTH_SERVICE_NAME);
        }
    }
}

int ObserverProfile::ProcessConnectStateMessage(
    RawAddress btAddr, A2dpDeviceInfo *deviceInfo, const int connectPolicy, const int state, const uint16_t handle)
{
    LOG_INFO("[ObserverProfile] %{public}s \n", __func__);
    int connectState = RET_BAD_STATUS;
    utility::Message msg(A2DP_MSG_PROFILE_DISCONNECTED, role_, &(btAddr));
    switch (state) {
        case STREAM_CONNECT:
            if ((int)BTStrategyType::CONNECTION_FORBIDDEN == connectPolicy) {
                UpdateStateInformation(
                    msg, connectState, A2DP_MSG_CONNECT_FORBIDDEN, static_cast<int>(BTConnectState::CONNECTING));
            } else {
                deviceInfo->SetHandle(handle);
                UpdateStateInformation(
                    msg, connectState, A2DP_MSG_PROFILE_CONNECTED, static_cast<int>(BTConnectState::CONNECTED));
            }
            break;
        case STREAM_CONNECTING:
            if ((int)BTStrategyType::CONNECTION_FORBIDDEN == connectPolicy) {
                UpdateStateInformation(
                    msg, connectState, A2DP_MSG_CONNECT_FORBIDDEN, static_cast<int>(BTConnectState::CONNECTING));
            } else {
                UpdateStateInformation(
                    msg, connectState, A2DP_MSG_PROFILE_CONNECTING, static_cast<int>(BTConnectState::CONNECTING));
            }
            break;
        case STREAM_DISCONNECT:
            UpdateStateInformation(
                msg, connectState, A2DP_MSG_PROFILE_DISCONNECTED, static_cast<int>(BTConnectState::DISCONNECTED));
            break;
        case STREAM_DISCONNECTING:
            UpdateStateInformation(
                msg, connectState, A2DP_MSG_PROFILE_DISCONNECTING, static_cast<int>(BTConnectState::DISCONNECTING));
            break;
        case STREAM_CONNECT_FAILED:
            UpdateStateInformation(
                msg, connectState, A2DP_MSG_PROFILE_DISCONNECTED, static_cast<int>(BTConnectState::DISCONNECTED));
            break;
        default:
            break;
    }
    if (connectState != RET_BAD_STATUS) {
        deviceInfo->SetConnectState(connectState);
        if (static_cast<int>(BTConnectState::DISCONNECTED) == connectState && deviceInfo->GetPlayingState()) {
            deviceInfo->SetPlayingState(false);
        }
        deviceInfo->GetStateMachine()->ProcessMessage(msg);
    }
    return connectState;
}

void ObserverProfile::UpdateStateInformation(utility::Message &msg, int &state,
    const int msgCMD, const int stateValue)
{
    std::lock_guard<std::recursive_mutex> lock(g_a2dpServiceMutex);

    LOG_INFO("[ObserverProfile] %{public}s msgCMD(%{public}d) stateValue(%{public}d)\n", __func__, msgCMD, stateValue);
    msg.what_ = msgCMD;
    state = stateValue;
}

void ObserverProfile::OnAudioStateChanged(const BtAddr &addr, const int state, void *context)
{
    LOG_INFO("[ObserverProfile] %{public}s role(%u) state(%{public}d)\n", __func__, role_, state);

    A2dpService *service = GetServiceInstance(role_);
    RawAddress btAddr = bluetooth::RawAddress::ConvertToString(addr.addr);
    int error = RET_NO_ERROR;

    if (service == nullptr) {
        LOG_ERROR("[ObserverProfile] %{public}s Can't get the instance of service\n", __func__);
        return;
    }

    A2dpDeviceInfo *deviceInfo = service->GetDeviceFromList(btAddr);
    utility::Message msgData(state, role_, &(btAddr));

    if (deviceInfo != nullptr) {
        RawAddress rawAddr = bluetooth::RawAddress::ConvertToString(deviceInfo->GetDevice().addr);
        if (state == A2DP_NOT_PLAYING) {
            deviceInfo->SetPlayingState(false);
        } else {
            deviceInfo->SetPlayingState(true);
        }
        service->ProcessPlayingFrameworkCallback(state, error, rawAddr);
    } else {
        LOG_INFO("[ObserverProfile] %{public}s role[%u] Not find the device\n", __func__, role_);
    }
}

void ObserverProfile::OnCodecStateChanged(const BtAddr &addr, const A2dpSrcCodecStatus codecInfo, void *context)
{
    LOG_INFO("[ObserverProfile] %{public}s role[%u]\n", __func__, role_);

    RawAddress btAddr = bluetooth::RawAddress::ConvertToString(addr.addr);
    A2dpService *service = GetServiceInstance(role_);
    int error = RET_NO_ERROR;

    if (service == nullptr) {
        LOG_ERROR("[ObserverProfile] %{public}s Can't get the instance of service\n", __func__);
        return;
    }

    A2dpDeviceInfo *deviceInfo = service->GetDeviceFromList(btAddr);
    if (deviceInfo == nullptr) {
        LOG_ERROR("[ObserverProfile] %{public}s role[%u] Not find the device\n", __func__, role_);
        return;
    }

    deviceInfo->SetCodecStatus(codecInfo);
    RawAddress rawAddr = bluetooth::RawAddress::ConvertToString(deviceInfo->GetDevice().addr);

    utility::Message msg(A2DP_MSG_PROFILE_CODEC_CHANGE, role_, &(btAddr));
    deviceInfo->GetStateMachine()->ProcessMessage(msg);

    service->ProcessCodecFrameworkCallback(codecInfo.codecInfo, error, rawAddr);
}

A2dpService::A2dpService(const std::string& name, const std::string version, const uint8_t role) : utility::Context(name, version)
{
    LOG_INFO("[A2dpService] %{public}s role[%u]\n", __func__, role);

    name_ = name;
    version_ = version;
    role_ = role;
    int value = 0;

    profileId_ = PROFILE_ID_A2DP_SRC;
    profileObserver_ = ObserverProfile(role);
    connectManager_ = A2dpConnectManager(role);
    if (role == A2DP_ROLE_SOURCE) {
        AdapterConfig::GetInstance()->GetValue(SECTION_A2DP_SRC_SERVICE, PROPERTY_MAX_CONNECTED_DEVICES, value);
        profileId_ = PROFILE_ID_A2DP_SRC;
    } else {
        AdapterConfig::GetInstance()->GetValue(SECTION_A2DP_SNK_SERVICE, PROPERTY_MAX_CONNECTED_DEVICES, value);
        profileId_ = PROFILE_ID_A2DP_SINK;
    }

    LOG_INFO("[A2dpService] %{public}s role[%u] config max devices(%{public}d)\n", __func__, role, value);
    if (value > 0 && value < A2DP_CONNECT_NUM_MAX) {
        maxConnectNumSnk_ = value;
    }
    LOG_INFO("[A2dpService] %{public}s maxConnectDevices(%{public}d)\n", __func__, maxConnectNumSnk_);
}

A2dpService::~A2dpService()
{
    LOG_INFO("[A2dpService] %{public}s role[%u]\n", __func__, role_);
    for (auto it = a2dpDevices_.begin(); it != a2dpDevices_.end(); it++) {
        if (it->second != nullptr) {
            delete it->second;
            it->second = nullptr;
        }
    }
    a2dpDevices_.clear();
}

utility::Context *A2dpService::GetContext()
{
    return this;
}

void A2dpService::Enable()
{
    std::lock_guard<std::recursive_mutex> lock(g_a2dpServiceMutex);

    LOG_INFO("[A2dpService] %{public}s role_[%u]\n", __func__, role_);
    A2dpProfile *pflA2dp = GetProfileInstance(role_);
    if (pflA2dp == nullptr) {
        LOG_WARN("[A2dpService] %{public}s Failed to get profile instance\n", __func__);
        GetContext()->OnEnable(name_, false);
        return;
    }

    if (role_ == A2DP_ROLE_SOURCE) {
        profileId_ = PROFILE_ID_A2DP_SRC;
    } else {
        profileId_ = PROFILE_ID_A2DP_SINK;
    }

    pflA2dp->RegisterObserver(&profileObserver_);

    GetDispatcher()->PostTask(std::bind(&A2dpService::EnableService, this));
}

void A2dpService::Disable()
{
    std::lock_guard<std::recursive_mutex> lock(g_a2dpServiceMutex);
    HILOGI("[A2dpService] role_[%{public}u]", role_);

    GetDispatcher()->PostTask(std::bind(&A2dpService::DisableService, this));
}

int A2dpService::Connect(const RawAddress &device)
{
    HILOGI("[address:%{public}s] role[%{public}u]", GET_ENCRYPT_ADDR(device), role_);

    int ret = RET_NO_ERROR;
    std::lock_guard<std::recursive_mutex> lock(g_a2dpServiceMutex);

    if (connectManager_.JudgeConnectExit(device, role_)) {
        LOG_INFO("[A2dpService]The device is connected as another role");
        ret = RET_BAD_STATUS;
    } else if (!connectManager_.JudgeConnectedNum()) {
        LOG_INFO("[A2dpService]The count of connected device is max");
        ret = RET_BAD_STATUS;
    } else if (static_cast<int>(BTConnectState::CONNECTED) == (GetDeviceState(device)) ||
               (static_cast<int>(BTConnectState::CONNECTING) == GetDeviceState(device))) {
        LOG_INFO("[A2dpService]Device have been connected");
        ret = RET_BAD_STATUS;
    } else if ((int)BTStrategyType::CONNECTION_FORBIDDEN == GetConnectStrategy(device)) {
        ret = RET_NO_SUPPORT;
    } else {
        utility::Message event(A2DP_CONNECT_EVT);
        PostEvent(event, const_cast<RawAddress &>(device));
    }

    return ret;
}

int A2dpService::Disconnect(const RawAddress &device)
{
    LOG_INFO("[A2dpService] %{public}s role[%u]\n", __func__, role_);

    int ret = RET_NO_ERROR;
    A2dpDeviceInfo *info = nullptr;
    std::lock_guard<std::recursive_mutex> lock(g_a2dpServiceMutex);

    auto iter = a2dpDevices_.find(device.GetAddress().c_str());
    if (iter == a2dpDevices_.end()) {
        LOG_INFO("[A2dpService]Can't find the statemachine");
        ret = RET_BAD_STATUS;
    } else {
        info = iter->second;
        if ((static_cast<int>(BTConnectState::DISCONNECTED) == info->GetConnectState()) ||
            (static_cast<int>(BTConnectState::DISCONNECTING) == info->GetConnectState())) {
            LOG_INFO("[A2dpService]Device have been disconnected");
            ret = BT_OPERATION_FAILED;
        } else {
            utility::Message event(A2DP_DISCONNECT_EVT);
            PostEvent(event, const_cast<RawAddress &>(device));
        }
    }

    return ret;
}

void A2dpService::ProcessAvdtpCallback(const BtAddr &addr, utility::Message &message) const
{
    LOG_INFO("[A2dpService] %{public}s \n", __func__);

    uint8_t role = (static_cast<A2dpAvdtMsg*>(message.arg2_))->role;
    message.what_ = (static_cast<A2dpAvdtMsg*>(message.arg2_))->event;
    A2dpProfile *instance = GetProfileInstance(role);
    if (instance == nullptr) {
        LOG_WARN("[A2dpService] %{public}s Failed to get profile instance\n", __func__);
        return;
    }
    instance->ProcessAvdtpCallback(addr, message);
}

void A2dpService::ProcessTimeoutCallback(uint8_t role, const BtAddr &addr) const
{
    LOG_INFO("[A2dpService] %{public}s role(%u)\n", __func__, role);

    A2dpProfile *instance = GetProfileInstance(role);
    if (instance == nullptr) {
        LOG_WARN("[A2dpService] %{public}s Failed to get profile instance\n", __func__);
        return;
    }

    instance->ProcessSignalingTimeoutCallback(addr);
}

void A2dpService::ProcessSDPFindCallback(const BtAddr &addr, const uint8_t result, A2dpProfile *instance) const
{
    LOG_INFO("[A2dpService] %{public}s \n", __func__);
    std::lock_guard<std::recursive_mutex> lock(g_a2dpServiceMutex);

    if (instance == nullptr) {
        LOG_WARN("[A2dpService] %{public}s Failed to get profile instance\n", __func__);
        return;
    }
    instance->ProcessSDPCallback(addr, result);
}

std::list<RawAddress> A2dpService::GetConnectDevices()
{
    LOG_INFO("[A2dpService] %{public}s\n", __func__);
    std::list<RawAddress> devList;
    int connectionState = static_cast<int>(BTConnectState::DISCONNECTED);
    std::lock_guard<std::recursive_mutex> lock(g_a2dpServiceMutex);

    for (auto it = a2dpDevices_.begin(); it != a2dpDevices_.end(); it++) {
        if (it->second == nullptr) {
            continue;
        }
        connectionState = it->second->GetConnectState();
        if (connectionState == static_cast<int>(BTConnectState::CONNECTED)) {
            devList.push_back(RawAddress::ConvertToString(it->second->GetDevice().addr));
        }
    }
    return devList;
}

int A2dpService::GetConnectState()
{
    LOG_INFO("[A2dpService] %{public}s\n", __func__);

    int ret = PROFILE_STATE_DISCONNECTED;
    int state = static_cast<int>(BTConnectState::DISCONNECTED);
    std::lock_guard<std::recursive_mutex> lock(g_a2dpServiceMutex);

    if (!a2dpDevices_.empty()) {
        for (auto itr : GetDeviceList()) {
            state = itr.second->GetConnectState();

            switch (state) {
                case static_cast<int>(BTConnectState::CONNECTING):
                    ret |= PROFILE_STATE_CONNECTING;
                    break;
                case static_cast<int>(BTConnectState::CONNECTED):
                    ret |= PROFILE_STATE_CONNECTED;
                    break;
                case static_cast<int>(BTConnectState::DISCONNECTING):
                    ret |= PROFILE_STATE_DISCONNECTING;
                    break;
                case static_cast<int>(BTConnectState::DISCONNECTED):
                    ret |= PROFILE_STATE_DISCONNECTED;
                    break;
                default:
                    break;
            }
        }
    }

    return ret;
}

void A2dpService::ProcessMessage(const utility::Message &msg) const
{
    LOG_INFO("[A2dpService] %{public}s\n", __func__);
}

void A2dpService::EnableService()
{
    bool ret = true;
    A2dpProfile *instance = GetProfileInstance(role_);

    if (instance == nullptr) {
        LOG_WARN("[A2dpService] %{public}s Failed to get profile instance\n", __func__);
        return;
    }

    instance->Enable();
    GetContext()->OnEnable(name_, ret);
}

void A2dpService::DisableService()
{
    bool ret = true;
    A2dpProfile *instance = GetProfileInstance(role_);
    if (instance == nullptr) {
        LOG_WARN("[A2dpService] %{public}s Failed to get profile instance\n", __func__);
        return;
    }
    instance->SetDisalbeTag(true);
    isDoDisable = true;

    if (instance->HasStreaming()) {
        auto curDevice = a2dpDevices_.find(activeDevice_.GetAddress().c_str());
        if (curDevice != a2dpDevices_.end()) {
            instance->Stop(curDevice->second->GetHandle(), true);
        }
    } else {
        if (instance->HasOpen()) {
            instance->CloseAll();
        } else {
            instance->Disable();
        }
    }

    if (GetConnectState() != PROFILE_STATE_DISCONNECTED) {
        return;
    }

    for (auto it = a2dpDevices_.begin(); it != a2dpDevices_.end(); it++) {
        if (it->second != nullptr) {
            delete it->second;
            it->second = nullptr;
        }
    }

    a2dpDevices_.clear();
    instance->DeregisterObserver(&profileObserver_);
    GetContext()->OnDisable(name_, ret);
    isDoDisable = false;
    instance->SetDisalbeTag(false);
}

std::vector<RawAddress> A2dpService::GetDevicesByStates(std::vector<int>& states) const
{
    std::vector<RawAddress> devices = {};
    std::lock_guard<std::recursive_mutex> lock(g_a2dpServiceMutex);

    for (auto it = a2dpDevices_.begin(); it != a2dpDevices_.end(); it++) {
        int connectionState = 0;
        connectionState = it->second->GetConnectState();
        if (FindStateMatched(states, connectionState)) {
            devices.push_back(RawAddress::ConvertToString(it->second->GetDevice().addr));
        }
    }

    return devices;
}

bool A2dpService::FindStateMatched(std::vector<int> states, int connectState) const
{
    int length = states.size();
    for (int i = 0; i < length; i++) {
        int state = 0;
        state = states[i];
        if (connectState == state) {
            return true;
        }
    }

    return false;
}

int A2dpService::GetDeviceState(const RawAddress &device) const
{
    A2dpDeviceInfo *info = nullptr;
    std::lock_guard<std::recursive_mutex> lock(g_a2dpServiceMutex);
    auto iter = a2dpDevices_.find(device.GetAddress().c_str());
    if (iter == a2dpDevices_.end()) {
        LOG_ERROR("[A2dpService]Can't find the statemachine");
        return static_cast<int>(BTConnectState::DISCONNECTED);
    } else {
        info = iter->second;
    }

    return info->GetConnectState();
}

int A2dpService::GetPlayingState(const RawAddress &device) const
{
    A2dpDeviceInfo *info = nullptr;
    std::lock_guard<std::recursive_mutex> lock(g_a2dpServiceMutex);
    auto iter = a2dpDevices_.find(device.GetAddress().c_str());
    if (iter == a2dpDevices_.end()) {
        LOG_ERROR("[A2dpService]Can't find the statemachine");
        return RET_BAD_STATUS;
    } else {
        info = iter->second;
    }

    return info->GetPlayingState();
}

int A2dpService::SetActiveSinkDevice(const RawAddress &device)
{
    A2dpProfile *pflA2dp = GetProfileInstance(role_);
    BtAddr btAddr = {};
    RawAddress rawAddr(device.GetAddress());
    rawAddr.ConvertToUint8(btAddr.addr);
    std::lock_guard<std::recursive_mutex> lock(g_a2dpServiceMutex);

    if (pflA2dp == nullptr) {
        LOG_ERROR("[A2dpService] %{public}s Failed to get profile instance\n", __func__);
        return RET_BAD_STATUS;
    }

    auto iter = a2dpDevices_.find(device.GetAddress().c_str());
    if (iter == a2dpDevices_.end()) {
        LOG_ERROR("[A2dpService]There is no statemachine");
        return RET_BAD_STATUS;
    } else {
        if (iter->second->GetConnectState() != static_cast<int>(BTConnectState::CONNECTED)) {
            LOG_ERROR("[A2dpService]The device is not connected");
            return RET_BAD_STATUS;
        }
    }

    auto curDevice = a2dpDevices_.find(activeDevice_.GetAddress().c_str());
    if (strcmp(device.GetAddress().c_str(), activeDevice_.GetAddress().c_str()) == 0) {
        LOG_ERROR("[A2dpService]The device is already active");
        pflA2dp->Start(curDevice->second->GetHandle());
    } else {
        if (curDevice != a2dpDevices_.end() && curDevice->second != nullptr) {
            if (pflA2dp->Stop(curDevice->second->GetHandle(), true)) {
                pflA2dp->Start(iter->second->GetHandle());
            }
        } else {
            pflA2dp->Start(iter->second->GetHandle());
        }
        pflA2dp->SetActivePeer(btAddr);
        UpdateActiveDevice(rawAddr);
    }

    return RET_NO_ERROR;
}

void A2dpService::ActiveDevice()
{
    std::lock_guard<std::recursive_mutex> lock(g_a2dpServiceMutex);
    A2dpProfile *pflA2dp = GetProfileInstance(role_);
    if (pflA2dp == nullptr) {
        LOG_ERROR("[A2dpService] %{public}s Failed to get profile instance\n", __func__);
        return;
    }
    auto curDevice = a2dpDevices_.find(activeDevice_.GetAddress().c_str());
    if (curDevice != a2dpDevices_.end()) {
        pflA2dp->Start(curDevice->second->GetHandle());
    }
}

const RawAddress &A2dpService::GetActiveSinkDevice() const
{
    std::lock_guard<std::recursive_mutex> lock(g_a2dpServiceMutex);

    HILOGI("address(%{public}s)", GetEncryptAddr(activeDevice_.GetAddress()).c_str());

    return activeDevice_;
}

int A2dpService::SetConnectStrategy(const RawAddress &device, int strategy)
{
    bool returnValue = false;
    IProfileConfig *config = ProfileConfig::GetInstance();
    int value = 0;

    switch (BTStrategyType(strategy)) {
        case BTStrategyType::CONNECTION_UNKNOWN:
            return config->RemoveProperty(device.GetAddress(),
                SECTION_CONNECTION_POLICIES,
                (role_ == A2DP_ROLE_SOURCE) ? PROPERTY_A2DP_CONNECTION_POLICY : PROPERTY_A2DP_SINK_CONNECTION_POLICY);
        case BTStrategyType::CONNECTION_ALLOWED:
            value = (int)BTStrategyType::CONNECTION_ALLOWED;
            break;
        case BTStrategyType::CONNECTION_FORBIDDEN:
            value = (int)BTStrategyType::CONNECTION_FORBIDDEN;
            break;
        default:
            return RET_BAD_STATUS;
    }

    returnValue = config->SetValue(device.GetAddress(),
        SECTION_CONNECTION_POLICIES,
        (role_ == A2DP_ROLE_SOURCE) ? PROPERTY_A2DP_CONNECTION_POLICY : PROPERTY_A2DP_SINK_CONNECTION_POLICY,
        value);
    int ret = returnValue ? RET_NO_ERROR : RET_BAD_STATUS;

    if (returnValue && ((int)BTStrategyType::CONNECTION_ALLOWED == strategy)) {
        if (static_cast<int>(BTConnectState::DISCONNECTED) == (GetDeviceState(device))) {
            Connect(device);
        }
    } else if (returnValue && ((int)BTStrategyType::CONNECTION_FORBIDDEN == strategy)) {
        if (static_cast<int>(BTConnectState::DISCONNECTED) != (GetDeviceState(device)) &&
            static_cast<int>(BTConnectState::DISCONNECTING) != (GetDeviceState(device))) {
            Disconnect(device);
        }
    } else {
        LOG_ERROR("[A2dpService]Strategy set failed");
        ret = RET_BAD_STATUS;
    }

    return ret;
}

int A2dpService::GetConnectStrategy(const RawAddress &device) const
{
    IProfileConfig *config = ProfileConfig::GetInstance();
    int value = 0;
    std::lock_guard<std::recursive_mutex> lock(g_a2dpServiceMutex);

    if (!config->GetValue(device.GetAddress(),
        SECTION_CONNECTION_POLICIES,
        (role_ == A2DP_ROLE_SOURCE) ? PROPERTY_A2DP_CONNECTION_POLICY : PROPERTY_A2DP_SINK_CONNECTION_POLICY,
        value)) {
        HILOGI("%{public}s connection policy not found", GET_ENCRYPT_ADDR(device));
        return (int)BTStrategyType::CONNECTION_UNKNOWN;
    }

    if (value == (int)BTStrategyType::CONNECTION_ALLOWED) {
        return (int)BTStrategyType::CONNECTION_ALLOWED;
    } else {
        return (int)BTStrategyType::CONNECTION_FORBIDDEN;
    }
}

int A2dpService::SendDelay(const RawAddress &device, uint16_t delayValue)
{
    LOG_INFO("[A2dpService] %{public}s\n", __func__);
    uint16_t handle = 0;
    std::lock_guard<std::recursive_mutex> lock(g_a2dpServiceMutex);

    auto iter = a2dpDevices_.find(device.GetAddress().c_str());
    if (iter == a2dpDevices_.end()) {
        LOG_ERROR("[A2dpService]Can't find the statemachine");
        return RET_BAD_STATUS;
    } else {
        handle = iter->second->GetHandle();
        LOG_INFO("[A2dpService] %{public}s handle [%u]", __func__, handle);
    }

    A2dpProfile *pflA2dp = GetProfileInstance(role_);
    if (pflA2dp == nullptr) {
        LOG_ERROR("[A2dpService] %{public}s Failed to get profile instance. role_(%u)\n", __func__, role_);
        return RET_BAD_STATUS;
    }

    int ret = pflA2dp->SendDelay(handle, delayValue);
    return ret;
}

A2dpSrcCodecStatus A2dpService::GetCodecStatus(const RawAddress &device) const
{
    A2dpSrcCodecStatus codecStatus;
    A2dpDeviceInfo *info = nullptr;
    std::lock_guard<std::recursive_mutex> lock(g_a2dpServiceMutex);

    for (auto bdr : GetDeviceList()) {
        if (bdr.first == device.GetAddress().c_str()) {
            info = bdr.second;
            break;
        }
    }

    if (info != nullptr) {
        codecStatus = info->GetCodecStatus();
    } else {
        LOG_ERROR("[A2dpService] device is not found in list");
    }

    return codecStatus;
}

int A2dpService::SetCodecPreference(const RawAddress &device, const A2dpSrcCodecInfo &info)
{
    LOG_INFO("[A2dpService] %{public}s\n", __func__);
    std::string addr = device.GetAddress();

    if (!strcmp(INVALID_MAC_ADDRESS.c_str(), device.GetAddress().c_str())) {
        addr = GetActiveSinkDevice().GetAddress();
    }
    if (!strcmp(INVALID_MAC_ADDRESS.c_str(), addr.c_str())) {
        LOG_INFO("[A2dpService] %{public}s : invalid device\n", __func__);
        return RET_BAD_PARAM;
    }

    A2dpSrcCodecStatus codecStatus = GetCodecStatus(RawAddress(addr));
    A2dpSrcCodecInfo currentInfo = codecStatus.codecInfo;
    if (!IsLocalCodecInfo(codecStatus, info)) {
        LOG_ERROR("[A2dpService] %{public}s : device's local info doesn't include codecinfo \n", __func__);
        return RET_BAD_PARAM;
    }

    if ((IsSimilarCodecConfig(currentInfo, info))) {
        LOG_ERROR("[A2dpService] %{public}s : current codec information doesn't change \n", __func__);
        return RET_BAD_PARAM;
    }

    LOG_INFO("[A2dpService] %{public}s codec information is valid\n", __func__);

    A2dpProfile *pflA2dp = GetProfileInstance(role_);
    BtAddr btAddr = {};
    RawAddress rawAddr(addr);
    rawAddr.ConvertToUint8(btAddr.addr);

    int ret = RET_BAD_STATUS;
    if (pflA2dp != nullptr) {
        ret = pflA2dp->SetUserCodecConfigure(btAddr, info);
    }
    return ret;
}

void A2dpService::SwitchOptionalCodecs(const RawAddress &device, bool isEnable)
{
    if ((int)A2DP_OPTIONAL_SUPPORT != GetOptionalCodecsSupportState(device)) {
        LOG_ERROR("[A2dpService] %{public}s : optional codec is not support\n", __func__);
        return;
    }

    std::string addr = device.GetAddress();
    if (!strcmp(INVALID_MAC_ADDRESS.c_str(), device.GetAddress().c_str())) {
        addr = GetActiveSinkDevice().GetAddress();
    }
    if (!strcmp(INVALID_MAC_ADDRESS.c_str(), addr.c_str())) {
        LOG_ERROR("[A2dpService] %{public}s : invalid device\n", __func__);
        return;
    }

    A2dpSrcCodecStatus codecStatus = GetCodecStatus(addr);
    if (isEnable != (A2DP_CODEC_TYPE_SBC_USER == codecStatus.codecInfo.codecType)) {
        LOG_ERROR("[A2dpService] : current optional codec is the same as user setting\n");
        return;
    }

    A2dpProfile *pflA2dp = GetProfileInstance(role_);
    BtAddr btAddr = {};
    RawAddress rawAddr(addr);
    rawAddr.ConvertToUint8(btAddr.addr);

    if (pflA2dp == nullptr) {
        LOG_ERROR("[A2dpService] %{public}s Failed to get profile instance. role_(%u)\n", __func__, role_);
        return;
    }
    pflA2dp->EnableOptionalCodec(btAddr, isEnable);
}

int A2dpService::GetOptionalCodecsSupportState(const RawAddress &device) const
{
    IProfileConfig *config = ProfileConfig::GetInstance();
    int value = 0;
    std::lock_guard<std::recursive_mutex> lock(g_a2dpServiceMutex);

    if (!config->GetValue(
        device.GetAddress(), SECTION_CODE_CS_SUPPORT, PROPERTY_A2DP_SUPPORTS_OPTIONAL_CODECS, value)) {
        HILOGI("%{public}s %{public}s not found", GET_ENCRYPT_ADDR(device),
            PROPERTY_A2DP_SUPPORTS_OPTIONAL_CODECS.c_str());
        return (int)A2DP_OPTIONAL_SUPPORT_UNKNOWN;
    }

    if (value) {
        return (int)A2DP_OPTIONAL_SUPPORT;
    } else {
        return (int)A2DP_OPTIONAL_NOT_SUPPORT;
    }
}

int A2dpService::StartPlaying(const RawAddress &device)
{
    uint16_t handle = 0;
    std::lock_guard<std::recursive_mutex> lock(g_a2dpServiceMutex);

    auto iter = a2dpDevices_.find(device.GetAddress().c_str());
    if (iter == a2dpDevices_.end()) {
        LOG_ERROR("[A2dpService]Can't find the statemachine");
        return RET_BAD_STATUS;
    } else {
        handle = iter->second->GetHandle();
        LOG_INFO("[A2dpService] %{public}s handle [%u]", __func__, handle);
    }

    if (iter->second->GetPlayingState()) {
        LOG_INFO("[A2dpService] Device is on playing");
        return RET_NO_ERROR;
    }

    A2dpProfile *pflA2dp = GetProfileInstance(role_);
    if (pflA2dp == nullptr) {
        LOG_ERROR("[A2dpService] %{public}s Failed to get profile instance. role_(%u)\n", __func__, role_);
        return RET_BAD_STATUS;
    }

    int ret = pflA2dp->Start(handle);
    return ret;
}

int A2dpService::SuspendPlaying(const RawAddress &device)
{
    uint16_t handle = 0;
    std::lock_guard<std::recursive_mutex> lock(g_a2dpServiceMutex);

    auto iter = a2dpDevices_.find(device.GetAddress().c_str());
    if (iter == a2dpDevices_.end()) {
        LOG_ERROR("[A2dpService]Can't find the statemachine");
        return RET_BAD_STATUS;
    } else {
        handle = iter->second->GetHandle();
    }

    if (!iter->second->GetPlayingState()) {
        LOG_ERROR("[A2dpService] Device is not on playing");
        return RET_NO_ERROR;
    }

    LOG_INFO("[A2dpService] %{public}s handle [%u]", __func__, handle);
    A2dpProfile *pflA2dp = GetProfileInstance(role_);
    if (pflA2dp == nullptr) {
        LOG_ERROR("[A2dpService] %{public}s Failed to get profile instance. role_(%u)\n", __func__, role_);
        return RET_BAD_STATUS;
    }

    int ret = pflA2dp->Stop(handle, true);
    return ret;
}

int A2dpService::StopPlaying(const RawAddress &device)
{
    uint16_t handle = 0;
    std::lock_guard<std::recursive_mutex> lock(g_a2dpServiceMutex);

    auto iter = a2dpDevices_.find(device.GetAddress().c_str());
    if (iter == a2dpDevices_.end()) {
        LOG_ERROR("[A2dpService]Can't find the statemachine");
        return RET_BAD_STATUS;
    } else {
        handle = iter->second->GetHandle();
    }

    if (!iter->second->GetPlayingState()) {
        LOG_ERROR("[A2dpService] Device is not on playing");
        return RET_NO_ERROR;
    }

    LOG_INFO("[A2dpService] %{public}s handle [%u]", __func__, handle);
    A2dpProfile *pflA2dp = GetProfileInstance(role_);
    if (pflA2dp == nullptr) {
        LOG_ERROR("[A2dpService] %{public}s Failed to get profile instance. role_(%u)\n", __func__, role_);
        return RET_BAD_STATUS;
    }

    int ret = pflA2dp->Stop(handle, false);
    return ret;
}

void A2dpService::RegisterObserver(IA2dpObserver *observer)
{
    LOG_INFO("[A2dpService] %{public}s\n", __func__);
    std::lock_guard<std::recursive_mutex> lock(g_a2dpServiceMutex);

    a2dpFramworkCallback_.Register(*observer);
}

void A2dpService::DeregisterObserver(IA2dpObserver *observer)
{
    LOG_INFO("[A2dpService] %{public}s\n", __func__);
    std::lock_guard<std::recursive_mutex> lock(g_a2dpServiceMutex);

    a2dpFramworkCallback_.Deregister(*observer);
}

int A2dpService::WriteFrame(const uint8_t *data, uint32_t size)
{
    LOG_INFO("[A2dpService] %{public}s\n", __func__);

    A2dpProfile *profile = GetProfileInstance(role_);
    if (profile != nullptr) {
        if (profile->SetPcmData(data, size) == 0) {
            LOG_ERROR("[A2dpService] %{public}s Failed to write frame. role_(%u)\n", __func__, role_);
            return RET_NO_SPACE;
        }
    } else {
        LOG_ERROR("[A2dpService] %{public}s Failed to get profile instance. role_(%u)\n", __func__, role_);
        return RET_BAD_STATUS;
    }
    return RET_NO_ERROR;
}

void A2dpService::GetRenderPosition(uint16_t &delayValue, uint16_t &sendDataSize, uint32_t &timeStamp)
{
    LOG_INFO("[A2dpService] %{public}s\n", __func__);

    A2dpProfile *profile = GetProfileInstance(role_);
    if (profile != nullptr) {
        profile->GetRenderPosition(delayValue, sendDataSize, timeStamp);
    } else {
        LOG_ERROR("[A2dpService] %{public}s Failed to get profile instance. role_(%u)\n", __func__, role_);
    }
}

int A2dpService::GetMaxConnectNum()
{
    LOG_INFO("[A2dpService] %{public}s\n", __func__);
    std::lock_guard<std::recursive_mutex> lock(g_a2dpServiceMutex);

    return maxConnectNumSnk_;
}

void A2dpService::ProcessConnectFrameworkCallback(int state, const RawAddress &device)
{
    LOG_INFO("[A2dpService] %{public}s\n", __func__);

    a2dpFramworkCallback_.ForEach(
        [device, state](IA2dpObserver &observer) { observer.OnConnectionStateChanged(device, state); });
}

void A2dpService::ProcessPlayingFrameworkCallback(int playingState, int error, const RawAddress &device)
{
    LOG_INFO("[A2dpService] %{public}s\n", __func__);

    a2dpFramworkCallback_.ForEach([device, playingState, error](IA2dpObserver &observer) {
        observer.OnPlayingStatusChaned(device, playingState, error);
    });
}

void A2dpService::ProcessCodecFrameworkCallback(
    const bluetooth::A2dpSrcCodecInfo &info, int error, const RawAddress &device)
{
    LOG_INFO("[A2dpService] %{public}s\n", __func__);

    a2dpFramworkCallback_.ForEach(
        [device, info, error](IA2dpObserver &observer) { observer.OnConfigurationChanged(device, info, error); });
}

A2dpDeviceInfo *A2dpService::GetDeviceFromList(const RawAddress &device)
{
    LOG_INFO("[A2dpService] %{public}s\n", __func__);
    std::lock_guard<std::recursive_mutex> lock(g_a2dpServiceMutex);

    auto iter = a2dpDevices_.find(device.GetAddress().c_str());
    if (iter == a2dpDevices_.end()) {
        LOG_ERROR("[A2dpService]Can't find the statemachine");
        return nullptr;
    } else {
        return iter->second;
    }
}

std::map<std::string, A2dpDeviceInfo *> A2dpService::GetDeviceList() const
{
    LOG_INFO("[A2dpService] %{public}s\n", __func__);
    std::lock_guard<std::recursive_mutex> lock(g_a2dpServiceMutex);

    return a2dpDevices_;
}

void A2dpService::AddDeviceToList(std::string address, A2dpDeviceInfo *deviceInfo)
{
    LOG_INFO("[A2dpService] %{public}s\n", __func__);
    std::lock_guard<std::recursive_mutex> lock(g_a2dpServiceMutex);

    a2dpDevices_.insert(std::make_pair(address, deviceInfo));
}

void A2dpService::DeleteDeviceFromList(const RawAddress &device)
{
    LOG_INFO("[A2dpService] %{public}s\n", __func__);

    std::map<std::string, bluetooth::A2dpDeviceInfo *>::iterator it;
    A2dpDeviceInfo *deviceInfo = nullptr;
    std::lock_guard<std::recursive_mutex> lock(g_a2dpServiceMutex);

    for (it = a2dpDevices_.begin(); it != a2dpDevices_.end(); it++) {
        if (strcmp(it->first.c_str(), device.GetAddress().c_str()) == RET_NO_ERROR) {
            deviceInfo = it->second;
            delete deviceInfo;
            a2dpDevices_.erase(it);
            break;
        }
        LOG_INFO("[A2dpService]%{public}s device[%{public}s]\n", __func__, it->first.c_str());
    }
}

void A2dpService::PostEvent(utility::Message event, RawAddress &device)
{
    GetDispatcher()->PostTask(std::bind(&A2dpService::ProcessEvent, this, event, device));
}

void A2dpService::ProcessEvent(utility::Message event, RawAddress &device)
{
    HILOGI("peerAddr(%{public}s)", GET_ENCRYPT_ADDR(device));
    BtAddr addr = {};
    device.ConvertToUint8(addr.addr);
    A2dpAvdtMsg data = {};
    utility::Message msg(event.what_, 0, &data);

    switch (event.what_) {
        case A2DP_CONNECT_EVT:
            if (connectManager_.A2dpConnect(device)) {
                LOG_INFO("[A2dpService] Start connect peer\n");
            } else {
                LOG_ERROR("[A2dpService] Start connect failed\n");
            }
            break;
        case A2DP_DISCONNECT_EVT:
            if (connectManager_.A2dpDisconnect(device)) {
                LOG_INFO("[A2dpService] Start disconnect peer\n");
            } else {
                LOG_ERROR("[A2dpService] Start disconnect failed\n");
            }
            break;
        case A2DP_AVDTP_EVT:
            if (event.arg2_ != nullptr) {
                (void)memcpy_s(msg.arg2_, sizeof(data), event.arg2_, sizeof(data));
                ProcessAvdtpCallback(addr, msg);
                delete static_cast<A2dpAvdtMsg*>(event.arg2_);
                event.arg2_ = nullptr;
            }
            break;
        case A2DP_SDP_EVT:
            if (event.arg2_ != nullptr) {
                ProcessSDPFindCallback(addr, event.arg1_, static_cast<A2dpProfile*>(event.arg2_));
            }
            break;
        case A2DP_TIMEOUT_EVT:
            ProcessTimeoutCallback(event.arg1_, addr);
            break;
        default:
            break;
    }
}

A2dpConnectManager A2dpService::ConnectManager() const
{
    std::lock_guard<std::recursive_mutex> lock(g_a2dpServiceMutex);

    return connectManager_;
}

void A2dpService::UpdateOptCodecStatus(const RawAddress &device)
{
    LOG_INFO("[A2dpService] %{public}s\n", __func__);

    bool supportOptCodec = false;
    bool mandatoryCodec = false;
    A2dpSrcCodecStatus codecStatus = GetCodecStatus(device);
    int supportState = GetOptionalCodecsSupportState(device);

    for (A2dpSrcCodecInfo codecInfo : codecStatus.codecInfoConfirmedCap) {
        if (codecInfo.codecType == A2DP_CODEC_TYPE_SBC_USER) {
            mandatoryCodec = true;
        } else {
            supportOptCodec = true;
        }
    }

    if (!mandatoryCodec) {
        return;
    }

    if ((supportState == A2DP_OPTIONAL_SUPPORT_UNKNOWN) ||
        ((supportState == A2DP_OPTIONAL_SUPPORT) && (supportOptCodec == false)) ||
        ((supportState == A2DP_OPTIONAL_NOT_SUPPORT) && (supportOptCodec))) {
        SetOptionalCodecsSupportState(device, supportOptCodec);
    }
}

void A2dpService::CheckDisable()
{
    LOG_INFO("[A2dpService] %{public}s isDoDisable(%{public}d), state(%{public}d)\n", __func__, isDoDisable, GetConnectState());

    A2dpProfile *instance = GetProfileInstance(role_);
    std::lock_guard<std::recursive_mutex> lock(g_a2dpServiceMutex);

    if (isDoDisable && (GetConnectState() == PROFILE_STATE_DISCONNECTED)) {
        isDoDisable = false;

        for (auto it = a2dpDevices_.begin(); it != a2dpDevices_.end(); it++) {
            if (it->second != nullptr) {
                delete it->second;
                it->second = nullptr;
            }
        }
        a2dpDevices_.clear();

        instance->DeregisterObserver(&profileObserver_);
        GetContext()->OnDisable(name_, true);
        instance->SetDisalbeTag(false);
    }
}

void A2dpService::UpdateActiveDevice(const RawAddress &device)
{
    LOG_INFO("[A2dpService] %{public}s\n", __func__);
    std::lock_guard<std::recursive_mutex> lock(g_a2dpServiceMutex);

    activeDevice_ = device;
}

void A2dpService::SetOptionalCodecsSupportState(const RawAddress &device, int state)
{
    IProfileConfig *config = ProfileConfig::GetInstance();
    std::lock_guard<std::recursive_mutex> lock(g_a2dpServiceMutex);

    if (state == (int)A2DP_OPTIONAL_SUPPORT_UNKNOWN) {
        config->RemoveProperty(device.GetAddress(), SECTION_CODE_CS_SUPPORT, PROPERTY_A2DP_SUPPORTS_OPTIONAL_CODECS);
    } else {
        LOG_INFO("[A2dpService] %{public}s ProfileConfig SetValue state(%{public}d)\n", __func__, state);
        config->SetValue(device.GetAddress(), SECTION_CODE_CS_SUPPORT, PROPERTY_A2DP_SUPPORTS_OPTIONAL_CODECS, state);
    }
}

bool A2dpService::IsSimilarCodecConfig(A2dpSrcCodecInfo codecInfo, A2dpSrcCodecInfo newInfo) const
{
    LOG_INFO("[A2dpService] %{public}s\n", __func__);

    if (newInfo.codecType != codecInfo.codecType) {
        return false;
    }

    if ((newInfo.sampleRate == A2DP_SAMPLE_RATE_NONE_USER) || (codecInfo.sampleRate == A2DP_SAMPLE_RATE_NONE_USER)) {
        newInfo.sampleRate = codecInfo.sampleRate;
    }

    if ((newInfo.bitsPerSample == A2DP_SAMPLE_BITS_NONE_USER) ||
        (codecInfo.bitsPerSample == A2DP_SAMPLE_BITS_NONE_USER)) {
        newInfo.bitsPerSample = codecInfo.bitsPerSample;
    }

    if ((newInfo.channelMode == A2DP_CHANNEL_MODE_NONE_USER) ||
        (codecInfo.channelMode == A2DP_CHANNEL_MODE_NONE_USER)) {
        newInfo.channelMode = codecInfo.channelMode;
    }

    if ((newInfo.sampleRate == codecInfo.sampleRate) && (newInfo.bitsPerSample == codecInfo.bitsPerSample) &&
        (newInfo.channelMode == codecInfo.channelMode)) {
        LOG_INFO("[A2dpService] %{public}s similar config\n", __func__);
        return true;
    }

    return false;
}

bool A2dpService::IsLocalCodecInfo(A2dpSrcCodecStatus codecStatus, A2dpSrcCodecInfo codecInformation) const
{
    LOG_INFO("[A2dpService] %{public}s\n", __func__);

    LOG_INFO("[A2dpService] %{public}s Set codecType(%{public}u)\n", __func__, codecInformation.codecType);
    LOG_INFO("[A2dpService] %{public}s Set sampleRate(%{public}u)\n", __func__, codecInformation.sampleRate);
    LOG_INFO("[A2dpService] %{public}s Set bitsPerSample(%{public}u)\n", __func__, codecInformation.bitsPerSample);
    LOG_INFO("[A2dpService] %{public}s Set channelMode(%{public}u)\n", __func__, codecInformation.channelMode);

    for (A2dpSrcCodecInfo info : codecStatus.codecInfoLocalCap) {
        LOG_INFO("[A2dpService] %{public}s codecType(%{public}u)\n", __func__, info.codecType);
        LOG_INFO("[A2dpService] %{public}s sampleRate(%{public}u)\n", __func__, info.sampleRate);
        LOG_INFO("[A2dpService] %{public}s bitsPerSample(%{public}u)\n", __func__, info.bitsPerSample);
        LOG_INFO("[A2dpService] %{public}s channelMode(%{public}u)\n", __func__, info.channelMode);
        if ((info.codecType == codecInformation.codecType)
            && ((info.sampleRate & codecInformation.sampleRate) == codecInformation.sampleRate)
            && ((info.bitsPerSample & codecInformation.bitsPerSample) == codecInformation.bitsPerSample)
            && ((info.channelMode & codecInformation.channelMode) == codecInformation.channelMode)) {
            return true;
        }
    }
    return false;
}

A2dpService *GetServiceInstance(uint8_t role)
{
    LOG_INFO("[A2dpService] %{public}s role(%u) \n", __func__, role);
    IProfileManager *servManager = IProfileManager::GetInstance();
    A2dpService *service = nullptr;

    if (role == A2DP_ROLE_SOURCE) {
        service = static_cast<A2dpService *>(servManager->GetProfileService(PROFILE_NAME_A2DP_SRC));
    } else {
        service = static_cast<A2dpService *>(servManager->GetProfileService(PROFILE_NAME_A2DP_SINK));
    }
    return service;
}

A2dpProfile *GetProfileInstance(uint8_t role)
{
    LOG_INFO("[A2dpProfile] %{public}s role(%u)\n", __func__, role);
    A2dpProfile *profile = nullptr;

    if (role == AVDT_ROLE_SNK) {
        profile = A2dpSnkProfile::GetInstance();
    } else {
        profile = A2dpSrcProfile::GetInstance();
    }
    return profile;
}
}  // namespace bluetooth
}  // namespace OHOS