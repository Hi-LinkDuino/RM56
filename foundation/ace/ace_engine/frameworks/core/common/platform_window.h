/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_PLATFORM_WINDOW_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_PLATFORM_WINDOW_H

#include <functional>
#include <memory>

#include "base/memory/ace_type.h"
#include "base/utils/macros.h"
#include "base/utils/noncopyable.h"

namespace OHOS::Ace {

using AceVsyncCallback = std::function<void(uint64_t, uint32_t)>;
class AceView;
class RenderNode;

class ACE_EXPORT PlatformWindow {
public:
    PlatformWindow() = default;
    virtual ~PlatformWindow() = default;

    static std::unique_ptr<PlatformWindow> Create(AceView* aceView);

    virtual void Destroy() {}

    // Request next vsync.
    virtual void RequestFrame() = 0;

    // Register Vsync callback.
    virtual void RegisterVsyncCallback(AceVsyncCallback&& callback) = 0;

    // Attach root render node to container
    virtual void SetRootRenderNode(const RefPtr<RenderNode>& root) = 0;

private:
    ACE_DISALLOW_COPY_AND_MOVE(PlatformWindow);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_PLATFORM_WINDOW_H
