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

#include "core/components/scroll_bar/rosen_render_scroll_bar.h"

#include "core/components/scroll_bar/scroll_bar_proxy.h"
#include "core/pipeline/base/rosen_render_context.h"

namespace OHOS::Ace {

void RosenRenderScrollBar::Paint(RenderContext& context, const Offset& offset)
{
    if (displayMode_ == DisplayMode::OFF) {
        LOGD("DisplayMode is off, or opacity is zero, needn't paint.");
        return;
    }
    for (auto& child : GetChildren()) {
        child->MarkNeedRender();
    }
    RenderNode::Paint(context, offset);
}

} // namespace OHOS::Ace