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

#include "core/common/window.h"

#include "base/log/log.h"
#include "base/utils/macros.h"

namespace OHOS::Ace {

Window::Window(std::unique_ptr<PlatformWindow> platformWindow) : platformWindow_(std::move(platformWindow))
{
    if (platformWindow_ != nullptr) {
        auto&& callback = [this](uint64_t nanoTimestamp, uint32_t frameCount) {
            OnVsync(nanoTimestamp, frameCount);
        };
        platformWindow_->RegisterVsyncCallback(callback);
        LOGI("Window Created success.");
    } else {
        LOGE("Window Created failed, platformWindow is nullptr.");
    }
}

void Window::RequestFrame()
{
    if (!onShow_) {
        LOGI("window is not show, stop request frame");
        return;
    }
    if (!isRequestVsync_ && platformWindow_ != nullptr) {
        LOGD("request next vsync");
        platformWindow_->RequestFrame();
        isRequestVsync_ = true;
    }
}

void Window::SetRootRenderNode(const RefPtr<RenderNode>& root)
{
    if (platformWindow_ == nullptr) {
        LOGE("SetRootRenderNode failed, platformWindow is nullptr.");
        return;
    }
    platformWindow_->SetRootRenderNode(root);
}

void Window::OnVsync(uint64_t nanoTimestamp, uint32_t frameCount)
{
    isRequestVsync_ = false;

    for (auto& callback : callbacks_) {
        if (callback) {
            callback(nanoTimestamp, frameCount);
        }
    }
}

} // namespace OHOS::Ace
