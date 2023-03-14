/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "dfx/ui_view_bounds.h"

#if ENABLE_DEBUG
#include "core/render_manager.h"

namespace OHOS {
UIViewBounds* UIViewBounds::GetInstance()
{
    static UIViewBounds instance;
    return &instance;
}

void UIViewBounds::SetShowState(bool show)
{
    if (showViewBounds_ != show) {
        showViewBounds_ = show;
        // when state changed, need redraw all view in screen
        RenderManager::GetInstance().RefreshScreen();
    }
}
} // namespace OHOS
#endif // ENABLE_DEBUG
