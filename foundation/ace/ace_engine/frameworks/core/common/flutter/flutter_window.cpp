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

#include "core/common/flutter/flutter_window.h"

#include <functional>

#include "flutter/runtime/window_manager.h"

#include "base/log/log.h"
#include "core/common/ace_view.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

std::unique_ptr<PlatformWindow> PlatformWindow::Create(AceView* view)
{
    if (view != nullptr) {
        return std::make_unique<Platform::FlutterWindow>(view->GetInstanceId());
    } else {
        return nullptr;
    }
}

namespace Platform {

FlutterWindow::FlutterWindow(int32_t instanceId) : instanceId_(instanceId) {}

FlutterWindow::~FlutterWindow() {}

void FlutterWindow::Destroy()
{
    auto window = flutter::WindowManager::GetWindow(instanceId_);
    if (window != nullptr) {
        window->SetBeginFrameCallback(nullptr);
    }
    vsyncCallbacks_.clear();
}

void FlutterWindow::RequestFrame()
{
    auto window = flutter::WindowManager::GetWindow(instanceId_);
    if (window != nullptr) {
        window->ScheduleFrame();
        if (!window->HasBeginFrameCallback()) {
            window->SetBeginFrameCallback(std::bind(&FlutterWindow::OnVsyncCallback, this, std::placeholders::_1));
        }
    }
}

void FlutterWindow::RegisterVsyncCallback(AceVsyncCallback&& callback)
{
    vsyncCallbacks_.emplace_back(std::move(callback));
}

void FlutterWindow::SetRootRenderNode(const RefPtr<RenderNode>& root) {}

void FlutterWindow::OnVsyncCallback(uint64_t timeStampNanos)
{
    for (const auto& vsyncCallback : vsyncCallbacks_) {
        vsyncCallback(timeStampNanos, 0);
    }
}

} // namespace Platform
} // namespace OHOS::Ace
