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

#include "a2dp_service_state_machine.h"
#include "a2dp_service.h"
#include "a2dp_sink.h"
#include "a2dp_source.h"
#include "log.h"

namespace OHOS {
namespace bluetooth {
bool A2dpDisconnected::Dispatch(const utility::Message &msg)
{
    LOG_INFO("[A2dpDisconnected] %{public}s role[%u]\n", __func__, msg.arg1_);

    int ret = A2DP_SUCCESS;
    uint8_t role = msg.arg1_;
    A2dpProfile *pflA2dp = GetProfileInstance(role);
    BtAddr btAddr = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00};
    RawAddress rawAddr = *((RawAddress *)(msg.arg2_));
    rawAddr.ConvertToUint8(btAddr.addr);

    A2dpService *service = GetServiceInstance(msg.arg1_);
    switch (msg.what_) {
        case A2DP_MSG_CONNECT:
            Transition(A2DP_STATE_CONNECTING);
            ret = pflA2dp->Connect(btAddr);
            service->ProcessConnectFrameworkCallback(static_cast<int>(BTConnectState::CONNECTING), rawAddr);
            break;
        case A2DP_MSG_PROFILE_CONNECTED:
            Transition(A2DP_STATE_CONNECTED);
            break;
        case A2DP_MSG_PROFILE_CONNECTING:
            Transition(A2DP_STATE_CONNECTING);
            service->ProcessConnectFrameworkCallback(static_cast<int>(BTConnectState::CONNECTING), rawAddr);
            break;
        case A2DP_MSG_CONNECT_FORBIDDEN:
            ret = pflA2dp->Disconnect(btAddr);
            break;
        default:
            break;
    }

    return (ret == A2DP_SUCCESS) ? true : false;
}

bool A2dpDisconnecting::Dispatch(const utility::Message &msg)
{
    LOG_INFO("[A2dpDisconnecting] %{public}s role[%u]\n", __func__, msg.arg1_);

    switch (msg.what_) {
        case A2DP_MSG_PROFILE_DISCONNECTED:
            Transition(A2DP_STATE_DISCONNECTED);
            break;
        default:
            break;
    }
    return true;
}

bool A2dpConnected::Dispatch(const utility::Message &msg)
{
    int ret = A2DP_SUCCESS;
    uint8_t role = msg.arg1_;
    A2dpProfile *pflA2dp = GetProfileInstance(role);
    BtAddr btAddr = {};
    RawAddress rawAddr = *((RawAddress *)(msg.arg2_));
    A2dpService *service = GetServiceInstance(role);
    A2dpDeviceInfo *deviceInfo = nullptr;
    rawAddr.ConvertToUint8(btAddr.addr);
    if (pflA2dp == nullptr || service == nullptr) {
        return false;
    }
    deviceInfo = service->GetDeviceFromList(rawAddr);
    if (deviceInfo == nullptr) {
        LOG_ERROR("[A2dpConnected] %{public}s Can't get the device by address\n", __func__);
        return false;
    }
    switch (msg.what_) {
        case A2DP_MSG_DISCONNECT:
            Transition(A2DP_STATE_DISCONNECTING);
            ret = pflA2dp->Disconnect(btAddr);
            service->ProcessConnectFrameworkCallback(static_cast<int>(BTConnectState::DISCONNECTING), rawAddr);
            break;
        case A2DP_MSG_PROFILE_DISCONNECTED:
            Transition(A2DP_STATE_DISCONNECTED);
            break;
        case A2DP_MSG_PROFILE_DISCONNECTING:
            Transition(A2DP_STATE_DISCONNECTING);
            service->ProcessConnectFrameworkCallback(static_cast<int>(BTConnectState::DISCONNECTING), rawAddr);
            break;
        case A2DP_MSG_PROFILE_AUDIO_PLAY_START:
            UpdateDeviceInformation(rawAddr, true, role);
            break;
        case A2DP_MSG_PROFILE_AUDIO_PLAY_SUSPEND:
        case A2DP_MSG_PROFILE_AUDIO_PLAY_STOP:
            UpdateDeviceInformation(rawAddr, false, role);
            break;
        case A2DP_MSG_PROFILE_CONNECTING:
            Transition(A2DP_STATE_CONNECTING);
            service->ProcessConnectFrameworkCallback(static_cast<int>(BTConnectState::CONNECTING), rawAddr);
            UpdateDeviceInformation(rawAddr, false, role);
            break;
        default:
            break;
    }
    return (ret == A2DP_SUCCESS) ? true : false;
}

void A2dpConnected::UpdateDeviceInformation(RawAddress rawAddr, bool value, uint8_t role)
{
    LOG_INFO("[A2dpConnected] %{public}s PlayingStatus[%{public}d]\n", __func__, value);

    A2dpDeviceInfo *deviceInfo = nullptr;
    A2dpService *service = GetServiceInstance(role);

    if (service == nullptr) {
        LOG_ERROR("[A2dpConnected] %{public}s Can't get the service of a2dp\n", __func__);
        return;
    }

    deviceInfo = service->GetDeviceFromList(rawAddr);
    if (deviceInfo == nullptr) {
        LOG_ERROR("[A2dpConnected] %{public}s Can't get the device by address\n", __func__);
        return;
    }

    if (deviceInfo->GetPlayingState()) {
        deviceInfo->SetPlayingState(value);
    }
}

bool A2dpConnecting::Dispatch(const utility::Message &msg)
{
    LOG_INFO("[A2dpConnecting] %{public}s role[%u]\n", __func__, msg.arg1_);

    int ret = A2DP_SUCCESS;
    uint8_t role = msg.arg1_;
    A2dpProfile *pflA2dp = GetProfileInstance(role);
    BtAddr btAddr;
    RawAddress rawAddr = *((RawAddress *)(msg.arg2_));
    rawAddr.ConvertToUint8(btAddr.addr);
    A2dpService *service = GetServiceInstance(role);

    switch (msg.what_) {
        case A2DP_MSG_CONNECT_TIMEOUT:
            ret = pflA2dp->Disconnect(btAddr);
            break;
        case A2DP_MSG_PROFILE_CONNECTED:
            Transition(A2DP_STATE_CONNECTED);
            break;
        case A2DP_MSG_PROFILE_DISCONNECTED:
            Transition(A2DP_STATE_DISCONNECTED);
            break;
        case A2DP_MSG_PROFILE_DISCONNECTING:
            Transition(A2DP_STATE_DISCONNECTING);
            service->ProcessConnectFrameworkCallback(static_cast<int>(BTConnectState::DISCONNECTING), rawAddr);
            break;
        case A2DP_MSG_DISCONNECT:
            ret = pflA2dp->Disconnect(btAddr);
            break;
        case A2DP_MSG_CONNECT_FORBIDDEN:
            ret = pflA2dp->Disconnect(btAddr);
            break;
        default:
            break;
    }

    return (ret == A2DP_SUCCESS) ? true : false;
}

void A2dpStateManager::SetRole(uint8_t role)
{
    role_ = role;
}
}  // namespace bluetooth
}  // namespace OHOS