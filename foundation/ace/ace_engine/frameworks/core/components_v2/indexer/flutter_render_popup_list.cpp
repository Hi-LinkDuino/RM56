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

#include "core/components_v2/indexer/flutter_render_popup_list.h"

#include "base/utils/utils.h"
#include "core/pipeline/base/scoped_canvas_state.h"

namespace OHOS::Ace::V2 {
RenderLayer FlutterRenderPopupList::GetRenderLayer()
{
    if (!layer_) {
        layer_ = AceType::MakeRefPtr<Flutter::ClipLayer>(
            0.0, GetLayoutSize().Width(), 0.0, GetLayoutSize().Height(), Flutter::Clip::HARD_EDGE);
    }
    return AceType::RawPtr(layer_);
}

bool FlutterRenderPopupList::IsRepaintBoundary() const
{
    return true;
}

void FlutterRenderPopupList::Paint(RenderContext& context, const Offset& offset)
{
    const auto& layoutSize = GetLayoutSize();
    if (layer_) {
        layer_->SetClip(0, layoutSize.Width(), 0, layoutSize.Height(), Flutter::Clip::HARD_EDGE);
    }

    if (renderBox_) {
        PaintChild(renderBox_, context, offset);
    }

    for (const auto& child : items_) {
        PaintChild(child, context, offset);
    }
}
} // namespace OHOS::Ace::V2
