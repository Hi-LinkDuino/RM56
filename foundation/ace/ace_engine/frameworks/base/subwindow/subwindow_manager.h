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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_SUBWINDOW_SUBWINDOW_MANAGER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_SUBWINDOW_SUBWINDOW_MANAGER_H

#include <mutex>
#include <unordered_map>

#include "base/memory/referenced.h"
#include "base/subwindow/subwindow.h"
#include "base/utils/macros.h"
#include "base/utils/noncopyable.h"

namespace OHOS::Ace {

constexpr int32_t MIN_SUBCONTAINER_ID = 1000000;

using SubwindowMap = std::unordered_map<int32_t, RefPtr<Subwindow>>;

class ACE_EXPORT SubwindowManager final : public NonCopyable {
public:
    // Get the instance
    static std::shared_ptr<SubwindowManager> GetInstance();

    void AddContainerId(uint32_t windowId, int32_t containerId);
    void RemoveContainerId(uint32_t windowId);
    int32_t GetContainerId(uint32_t windowId);

    void AddParentContainerId(int32_t containerId, int32_t parentContainerId);
    void RemoveParentContainerId(int32_t containerId);
    int32_t GetParentContainerId(int32_t containerId);

    void AddSubwindow(int32_t instanceId, RefPtr<Subwindow>);
    void RemoveSubwindow(int32_t instanceId);

    // Get the subwindow of instance, return the window or nullptr.
    const RefPtr<Subwindow> GetSubwindow(int32_t instanceId);

    void HideCurrentSubwindow();

    void SetCurrentSubwindowName(const std::string& currentSubwindow);
    std::string GetCurrentSubWindowName();

    void SetCurrentSubwindow(const RefPtr<Subwindow>& subwindow);

    const RefPtr<Subwindow>& GetCurrentWindow();

    void ShowMenu(const RefPtr<Component>& newComponent);
    void CloseMenu();
    void ClearMenu();

private:
    static std::mutex instanceMutex_;
    static std::shared_ptr<SubwindowManager> instance_;

    std::mutex mutex_;
    std::unordered_map<uint32_t, int32_t> containerMap_;

    std::mutex parentMutex_;
    std::unordered_map<int32_t, int32_t> parentContainerMap_;

    // Used to save the relationship between container and subwindow, it is 1:1
    std::mutex subwindowMutex_;
    SubwindowMap subwindowMap_;

    std::mutex currentSubwindowMutex_;
    std::string currentSubwindowName_;

    RefPtr<Subwindow> currentSubwindow_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_SUBWINDOW_SUBWINDOW_MANAGER_H
