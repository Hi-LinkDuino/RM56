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

#include "core/components/root/flutter_render_root.h"

#include "flutter/lib/ui/text/font_collection.h"
#include "flutter/lib/ui/text/paragraph_builder.h"
#include "flutter/runtime/window_manager.h"

#include "core/pipeline/base/flutter_render_context.h"

namespace OHOS::Ace {

using namespace Flutter;

void FlutterRenderRoot::Paint(RenderContext& context, const Offset& offset)
{
    auto canvas = static_cast<FlutterRenderContext&>(context).GetCanvas();
    if (!canvas || !canvas->canvas()) {
        LOGE("Paint canvas is null");
        return;
    }
    auto skColor = SkColorSetARGB(bgColor_.GetAlpha(), bgColor_.GetRed(), bgColor_.GetGreen(), bgColor_.GetBlue());
    auto pipelineContext = GetContext().Upgrade();
    if (pipelineContext && (pipelineContext->IsJsCard() || pipelineContext->IsJsPlugin())) {
        canvas->canvas()->drawColor(skColor);
    } else {
        canvas->canvas()->clear(skColor);
    }
    if (pipelineContext) {
        pipelineContext->SetRootBgColor(bgColor_);
    }
    RenderNode::Paint(context, offset);
}

void FlutterRenderRoot::FinishRender(const std::unique_ptr<DrawDelegate>& delegate, const Rect& dirty)
{
    if (delegate) {
        delegate->DrawFrame(layer_, dirty);
    }
}

RenderLayer FlutterRenderRoot::GetRenderLayer()
{
    if (!layer_ || isReset_) {
        matrix_.SetScale(scale_, scale_, scale_);
        layer_ = AceType::MakeRefPtr<TransformLayer>(matrix_, 0.0, 0.0);
    }
    return AceType::RawPtr(layer_);
}

void FlutterRenderRoot::DumpLayerTree()
{
    layer_->DumpTree(0);
}

} // namespace OHOS::Ace
