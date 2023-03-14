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

#include "core/components/tab_bar/flutter_render_tab_bar_item.h"

#include "core/components/common/painter/flutter_decoration_painter.h"
#include "core/pipeline/base/flutter_render_context.h"

namespace OHOS::Ace {

using namespace Flutter;
namespace {

constexpr double CLICKED_RADIUS = 8.0;

} // namespace

RenderLayer FlutterRenderTabBarItem::GetRenderLayer()
{
    if (!layer_) {
        layer_ = AceType::MakeRefPtr<ClipLayer>(
            0.0, GetLayoutSize().Width(), 0.0, GetLayoutSize().Height(), Clip::HARD_EDGE);
    }
    return AceType::RawPtr(layer_);
}

void FlutterRenderTabBarItem::Paint(RenderContext& context, const Offset& offset)
{
    layer_->SetClip(0.0, GetLayoutSize().Width(), 0.0, GetLayoutSize().Height(), Clip::HARD_EDGE);
    RenderNode::Paint(context, offset);

    auto frontDecoration = AceType::MakeRefPtr<Decoration>();
    frontDecoration->SetBackgroundColor(Color::BLACK.BlendOpacity(hoverOpacity_));
    frontDecoration->SetBorderRadius(Radius(Dimension(CLICKED_RADIUS, DimensionUnit::VP)));

    auto pipelineContext = context_.Upgrade();
    if (!pipelineContext) {
        return;
    }

    pipelineContext->AddDirtyLayoutNode(AceType::Claim(this));
    RefPtr<FlutterDecorationPainter> decorationPainter = AceType::MakeRefPtr<FlutterDecorationPainter>(
        frontDecoration, GetPaintRect(), GetPaintRect().GetSize(), pipelineContext->GetDipScale());

    const auto renderContext = static_cast<FlutterRenderContext*>(&context);
    flutter::Canvas* canvas = renderContext->GetCanvas();
    if (canvas == nullptr) {
        LOGE("Paint canvas is null");
        return;
    }
    decorationPainter->PaintDecoration(offset, canvas->canvas(), context);
}

} // namespace OHOS::Ace
