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

#include "frameworks/core/components/svg/flutter_render_svg.h"

#include "frameworks/core/components/common/painter/flutter_svg_painter.h"
#include "frameworks/core/components/transform/flutter_render_transform.h"
#include "frameworks/core/pipeline/base/flutter_render_context.h"

namespace OHOS::Ace {

using namespace Flutter;

RenderLayer FlutterRenderSvg::GetRenderLayer()
{
    if (!transformLayer_) {
        transformLayer_ = AceType::MakeRefPtr<Flutter::TransformLayer>(Matrix4::CreateIdentity(), 0.0, 0.0);
    }
    return AceType::RawPtr(transformLayer_);
}

void FlutterRenderSvg::Paint(RenderContext& context, const Offset& offset)
{
    if (transformLayer_ && isRoot_) {
        transformLayer_->SetOpacityLayer(rootOpacity_);
    }
    UpdateTransformByGlobalOffset();
    RenderNode::Paint(context, offset);
}

void FlutterRenderSvg::PaintDirectly(RenderContext& context, const Offset& offset)
{
    const auto renderContext = static_cast<FlutterRenderContext*>(&context);
    flutter::Canvas* canvas = renderContext->GetCanvas();
    if (!canvas) {
        LOGE("Paint canvas is null");
        return;
    }
    SkCanvas* skCanvas = canvas->canvas();
    if (!skCanvas) {
        LOGE("Paint skCanvas is null");
        return;
    }

    bool isViewBox = GreatNotEqual(viewBox_.Width(), 0.0) && GreatNotEqual(viewBox_.Height(), 0.0);
    if (isViewBox) {
        double scale = std::min(GetLayoutSize().Width() / viewBox_.Width(),
                                GetLayoutSize().Height() / viewBox_.Height());
        double tx = GetLayoutSize().Width() * 0.5 - (viewBox_.Width() * 0.5 + viewBox_.Left()) * scale;
        double ty = GetLayoutSize().Height() * 0.5 - (viewBox_.Height() * 0.5 + viewBox_.Top()) * scale;
        auto transform = Matrix4::CreateScale(static_cast<float>(scale), static_cast<float>(scale), 1.0f);
        transform = Matrix4::CreateTranslate(static_cast<float>(tx), static_cast<float>(ty), 0.0f) * transform;
        skCanvas->save();
        skCanvas->concat(FlutterSvgPainter::ToSkMatrix(transform));
    }

    RenderSvgBase::PaintDirectly(context, offset);
    if (isViewBox) {
        skCanvas->restore();
    }
}

void FlutterRenderSvg::OnGlobalPositionChanged()
{
    UpdateTransformByGlobalOffset();
    RenderSvg::OnGlobalPositionChanged();
}

void FlutterRenderSvg::UpdateTransformByGlobalOffset()
{
    if (GreatNotEqual(viewBox_.Width(), 0.0) && GreatNotEqual(viewBox_.Height(), 0.0)) {
        double scale = std::min(GetLayoutSize().Width() / viewBox_.Width(),
                                GetLayoutSize().Height() / viewBox_.Height());
        double tx = GetLayoutSize().Width() * 0.5 - (viewBox_.Width() * 0.5 + viewBox_.Left()) * scale;
        double ty = GetLayoutSize().Height() * 0.5 - (viewBox_.Height() * 0.5 + viewBox_.Top()) * scale;
        auto transform = Matrix4::CreateScale(static_cast<float>(scale), static_cast<float>(scale), 1.0f);
        transform = FlutterRenderTransform::GetTransformByOffset(transform, GetTransformOffset(true));
        transform = Matrix4::CreateTranslate(static_cast<float>(tx), static_cast<float>(ty), 0.0f) * transform;
        if (transformLayer_ == nullptr) {
            transformLayer_ = AceType::MakeRefPtr<Flutter::TransformLayer>(Matrix4::CreateIdentity(), 0.0, 0.0);
        }
        transformLayer_->Update(transform);
    }
}

} // namespace OHOS::Ace
