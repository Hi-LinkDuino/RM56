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

#include "core/components/form/form_window.h"

#include "base/log/log.h"

namespace OHOS::Ace {

void FormWindow::RequestFrame()
{
    auto context = outSidePipelineContext_.Upgrade();
    if (!context) {
        LOGE("form could not request frame due to null context");
        return;
    }
    auto window = context->GetWindow();
    if (!window) {
        LOGE("form could not request frame due to null window");
        return;
    }

    window->RequestFrame();
}

void FormWindow::SetVsyncCallback(AceVsyncCallback&& callback)
{
    auto context = outSidePipelineContext_.Upgrade();
    if (!context) {
        LOGE("form set vsync callback fail due to null context");
        return;
    }
    auto window = context->GetWindow();
    if (!window) {
        LOGE("form set vsync callback fail due to null window");
        return;
    }
    window->SetVsyncCallback(std::move(callback));
}

} // namespace OHOS::Ace
