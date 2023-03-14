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

#include "a2dp_service_device.h"

#include "context.h"
#include "log.h"
#include "memory.h"

namespace OHOS {
namespace bluetooth {
std::recursive_mutex g_deviceMutex {};

A2dpDeviceInfo::A2dpDeviceInfo(const RawAddress &device)
{
    LOG_INFO("[A2dpDeviceInfo] %{public}s\n", __func__);
    currentConnectState_ = static_cast<int>(BTConnectState::DISCONNECTED);
    device.ConvertToUint8(peerAddress_.addr);
}

A2dpDeviceInfo::~A2dpDeviceInfo()
{
    LOG_INFO("[A2dpDeviceInfo] %{public}s\n", __func__);
}

BtAddr A2dpDeviceInfo::GetDevice() const
{
    LOG_INFO("[A2dpDeviceInfo] %{public}s\n", __func__);

    return peerAddress_;
}

A2dpStateManager *A2dpDeviceInfo::GetStateMachine()
{
    LOG_INFO("[A2dpDeviceInfo] %{public}s\n", __func__);

    return &state_;
}

void A2dpDeviceInfo::SetCodecStatus(A2dpSrcCodecStatus codecStatusInfo)
{
    LOG_INFO("[A2dpDeviceInfo] %{public}s\n", __func__);
    std::lock_guard<std::recursive_mutex> lock(g_deviceMutex);

    codecStatus_ = codecStatusInfo;
}

A2dpSrcCodecStatus A2dpDeviceInfo::GetCodecStatus() const
{
    LOG_INFO("[A2dpDeviceInfo] %{public}s\n", __func__);
    std::lock_guard<std::recursive_mutex> lock(g_deviceMutex);

    return codecStatus_;
}

void A2dpDeviceInfo::SetPlayingState(bool state)
{
    LOG_INFO("[A2dpDeviceInfo] %{public}s playState(%{public}d)\n", __func__, state);
    std::lock_guard<std::recursive_mutex> lock(g_deviceMutex);

    isPlaying_ = state;
}

bool A2dpDeviceInfo::GetPlayingState() const
{
    LOG_INFO("[A2dpDeviceInfo] %{public}s\n", __func__);
    std::lock_guard<std::recursive_mutex> lock(g_deviceMutex);

    return isPlaying_;
}

void A2dpDeviceInfo::SetConnectState(int state)
{
    std::lock_guard<std::recursive_mutex> lock(g_deviceMutex);
    LOG_INFO("[SettState state] = %{public}d\n", state);
    LOG_INFO("[currentConnectState_] = %{public}d\n", currentConnectState_);

    currentConnectState_ = state;
}

int A2dpDeviceInfo::GetConnectState() const
{
    std::lock_guard<std::recursive_mutex> lock(g_deviceMutex);

    LOG_INFO("[currentConnectState_] = %{public}d\n", currentConnectState_);
    return currentConnectState_;
}

void A2dpDeviceInfo::SetHandle(uint16_t handleInfo)
{
    std::lock_guard<std::recursive_mutex> lock(g_deviceMutex);
    LOG_INFO("[A2dpDeviceInfo] %{public}s handle_[%u]\n", __func__, handleInfo);

    handle_ = handleInfo;
}

uint16_t A2dpDeviceInfo::GetHandle() const
{
    std::lock_guard<std::recursive_mutex> lock(g_deviceMutex);
    LOG_INFO("[A2dpDeviceInfo] %{public}s handle_[%u]\n", __func__, handle_);

    return handle_;
}
}  // namespace bluetooth
}  // namespace OHOS