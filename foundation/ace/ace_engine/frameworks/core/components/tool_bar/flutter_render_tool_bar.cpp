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

#include "core/components/tool_bar/flutter_render_tool_bar.h"

namespace OHOS::Ace {

using namespace Flutter;

RenderLayer FlutterRenderToolBar::GetRenderLayer()
{
    if (!layer_) {
        layer_ = AceType::MakeRefPtr<ClipLayer>(
            0.0, GetLayoutSize().Width(), 0.0, GetLayoutSize().Height(), Clip::HARD_EDGE);
    }
    return AceType::RawPtr(layer_);
}

void FlutterRenderToolBar::Paint(RenderContext& context, const Offset& offset)
{
    layer_->SetClip(0.0, GetLayoutSize().Width(), 0.0, GetLayoutSize().Height(), Clip::HARD_EDGE);
    RenderToolBar::Paint(context, offset);
}

} // namespace OHOS::Ace