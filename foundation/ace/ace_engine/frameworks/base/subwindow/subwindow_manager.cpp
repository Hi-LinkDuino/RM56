/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "base/subwindow/subwindow_manager.h"

#include <memory>
#include <mutex>

#include "unistd.h"

#include "base/log/log.h"
#include "base/memory/ace_type.h"
#include "core/common/container.h"

namespace OHOS::Ace {

std::mutex SubwindowManager::instanceMutex_;
std::shared_ptr<SubwindowManager> SubwindowManager::instance_;

std::shared_ptr<SubwindowManager> SubwindowManager::GetInstance()
{
    std::lock_guard<std::mutex> lock(instanceMutex_);
    if (!instance_) {
        instance_ = std::make_shared<SubwindowManager>();
    }
    return instance_;
}

void SubwindowManager::AddContainerId(uint32_t windowId, int32_t containerId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto result = containerMap_.try_emplace(windowId, containerId);
    if (!result.second) {
        LOGW("Already have container of this windowId, windowId: %{public}u", windowId);
    }
}

void SubwindowManager::RemoveContainerId(uint32_t windowId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    containerMap_.erase(windowId);
}

int32_t SubwindowManager::GetContainerId(uint32_t windowId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto result = containerMap_.find(windowId);
    if (result != containerMap_.end()) {
        return result->second;
    } else {
        return -1;
    }
}

void SubwindowManager::AddParentContainerId(int32_t containerId, int32_t parentContainerId)
{
    LOGI("Container id is %{public}d, parent id is %{public}d.", containerId, parentContainerId);
    std::lock_guard<std::mutex> lock(parentMutex_);
    auto result = parentContainerMap_.try_emplace(containerId, parentContainerId);
    if (!result.second) {
        LOGW("Already have container of this %{public}d", containerId);
    }
}

void SubwindowManager::RemoveParentContainerId(int32_t containerId)
{
    std::lock_guard<std::mutex> lock(parentMutex_);
    parentContainerMap_.erase(containerId);
}

int32_t SubwindowManager::GetParentContainerId(int32_t containerId)
{
    std::lock_guard<std::mutex> lock(parentMutex_);
    auto result = parentContainerMap_.find(containerId);
    if (result != parentContainerMap_.end()) {
        return result->second;
    } else {
        return 0;
    }
}

void SubwindowManager::AddSubwindow(int32_t instanceId, RefPtr<Subwindow> subwindow)
{
    if (!subwindow) {
        LOGE("Add subwindow failed, the subwindow is null.");
        return;
    }
    LOGI("Add subwindow into map, instanceId is %{public}d, subwindow id is %{public}d.", instanceId,
        subwindow->GetSubwindowId());
    std::lock_guard<std::mutex> lock(subwindowMutex_);
    auto result = subwindowMap_.try_emplace(instanceId, subwindow);
    if (!result.second) {
        LOGE("Add failed of this instance %{public}d", instanceId);
        return;
    }
    LOGI("Add subwindow success of this instance %{public}d.", instanceId);
}

void SubwindowManager::RemoveSubwindow(int32_t instanceId)
{
    LOGI("Remove subwindow of this instance %{public}d", instanceId);
    std::lock_guard<std::mutex> lock(subwindowMutex_);
    int res = static_cast<int>(subwindowMap_.erase(instanceId));
    if (res == 0) {
        LOGW("Remove subwindow of instance %{public}d failed.", instanceId);
    }
}

const RefPtr<Subwindow> SubwindowManager::GetSubwindow(int32_t instanceId)
{
    LOGI("Get subwindow of instance %{public}d.", instanceId);
    std::lock_guard<std::mutex> lock(subwindowMutex_);
    auto result = subwindowMap_.find(instanceId);
    if (result != subwindowMap_.end()) {
        return result->second;
    } else {
        return nullptr;
    }
}

void SubwindowManager::SetCurrentSubwindowName(const std::string& currentSubwindowName)
{
    std::lock_guard<std::mutex> lock(currentSubwindowMutex_);
    currentSubwindowName_ = currentSubwindowName;
}

std::string SubwindowManager::GetCurrentSubWindowName()
{
    std::lock_guard<std::mutex> lock(currentSubwindowMutex_);
    return currentSubwindowName_;
}

void SubwindowManager::SetCurrentSubwindow(const RefPtr<Subwindow>& subwindow)
{
    std::lock_guard<std::mutex> lock(currentSubwindowMutex_);
    currentSubwindow_ = subwindow;
}

const RefPtr<Subwindow>& SubwindowManager::GetCurrentWindow()
{
    std::lock_guard<std::mutex> lock(currentSubwindowMutex_);
    return currentSubwindow_;
}

void SubwindowManager::ShowMenu(const RefPtr<Component>& newComponent)
{
    auto containerId = Container::CurrentId();
    auto subwindow = GetSubwindow(containerId);
    if (!subwindow) {
        LOGI("Subwindow is null, add a new one.");
        subwindow = Subwindow::CreateSubwindow(containerId);
        subwindow->InitContainer();
        AddSubwindow(containerId, subwindow);
    }
    subwindow->ShowMenu(newComponent);
}

void SubwindowManager::CloseMenu()
{
    auto subwindow = GetCurrentWindow();
    if (subwindow) {
        subwindow->CloseMenu();
    }
}

void SubwindowManager::ClearMenu()
{
    auto subwindow = GetCurrentWindow();
    if (subwindow) {
        subwindow->ClearMenu();
    }
}

} // namespace OHOS::Ace
