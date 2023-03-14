/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_COMMON_FLUTTER_FLUTTER_WINDOW_H
#define FOUNDATION_ACE_FRAMEWORKS_COMMON_FLUTTER_FLUTTER_WINDOW_H

#include <vector>

#include "base/utils/noncopyable.h"
#include "core/common/platform_window.h"

namespace OHOS::Ace::Platform {

class FlutterWindow final : public PlatformWindow {
public:
    explicit FlutterWindow(int32_t instanceId);
    ~FlutterWindow() override;
    void Destroy() override;
    // Platform window interface
    void RequestFrame() override;
    void RegisterVsyncCallback(AceVsyncCallback&& callback) override;
    void SetRootRenderNode(const RefPtr<RenderNode>& root) override;

    // Vsync callback
    void OnVsyncCallback(uint64_t timeStampNanos);

private:
    std::vector<AceVsyncCallback> vsyncCallbacks_;
    int32_t instanceId_;

    ACE_DISALLOW_COPY_AND_MOVE(FlutterWindow);
};

} // namespace OHOS::Ace::Platform

#endif // FOUNDATION_ACE_FRAMEWORKS_COMMON_FLUTTER_FLUTTER_WINDOW_H
