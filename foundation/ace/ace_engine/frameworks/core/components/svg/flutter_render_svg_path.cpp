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

#include "frameworks/core/components/svg/flutter_render_svg_path.h"

#include "include/utils/SkParsePath.h"

#include "frameworks/core/components/common/painter/flutter_svg_painter.h"
#include "frameworks/core/components/transform/flutter_render_transform.h"
#include "frameworks/core/pipeline/base/flutter_render_context.h"

namespace OHOS::Ace {

using namespace Flutter;

RenderLayer FlutterRenderSvgPath::GetRenderLayer()
{
    if (!transformLayer_) {
        transformLayer_ = AceType::MakeRefPtr<Flutter::TransformLayer>(Matrix4::CreateIdentity(), 0.0, 0.0);
    }
    return AceType::RawPtr(transformLayer_);
}

void FlutterRenderSvgPath::Paint(RenderContext& context, const Offset& offset)
{
    if (d_.empty()) {
        return;
    }
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

    if (transformLayer_ && NeedTransform()) {
        transformLayer_->Update(GetTransformMatrix4());
    }
    SkAutoCanvasRestore save(skCanvas, false);
    PaintMaskLayer(context, offset, offset);

    SkPath out;
    GetPath(out);
    UpdateGradient(fillState_);

    RenderInfo renderInfo = { AceType::Claim(this), offset, opacity_, true };
    FlutterSvgPainter::SetFillStyle(skCanvas, out, fillState_, renderInfo);
    FlutterSvgPainter::SetStrokeStyle(skCanvas, out, strokeState_, renderInfo);
    RenderNode::Paint(context, offset);
}

void FlutterRenderSvgPath::PaintDirectly(RenderContext& context, const Offset& offset)
{
    if (d_.empty()) {
        return;
    }
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
    if (NeedTransform()) {
        skCanvas->save();
        skCanvas->concat(FlutterSvgPainter::ToSkMatrix(GetTransformMatrix4()));
    }
    SkPath out;
    GetPath(out);
    UpdateGradient(fillState_);
    FlutterSvgPainter::SetFillStyle(skCanvas, out, fillState_, opacity_);
    FlutterSvgPainter::SetStrokeStyle(skCanvas, out, strokeState_, opacity_);
    if (NeedTransform()) {
        skCanvas->restore();
    }
}

void FlutterRenderSvgPath::UpdateMotion(const std::string& path, const std::string& rotate, double percent)
{
    if (!transformLayer_) {
        LOGE("transformLayer is null");
        return;
    }
    bool isSuccess = true;
    auto motionMatrix = FlutterSvgPainter::CreateMotionMatrix(path, rotate, percent, isSuccess);
    if (isSuccess) {
        auto transform = FlutterRenderTransform::GetTransformByOffset(motionMatrix, GetGlobalOffset());
        transformLayer_->Update(transform);
    }
}

Rect FlutterRenderSvgPath::GetPaintBounds(const Offset& offset)
{
    SkPath path;
    GetPath(path);
    auto& bounds = path.getBounds();
    return Rect(bounds.left(), bounds.top(), bounds.width(), bounds.height());
}

void FlutterRenderSvgPath::GetPath(SkPath& out)
{
    if (paths_.empty()) {
        SkParsePath::FromSVGString(d_.c_str(), &out);
    } else {
        SkPath path;
        SkPath ending;
        int32_t firstPart = (int)weight_;
        int32_t pathsSize = static_cast<int32_t>(paths_.size());
        bool ret = false;
        if (firstPart < 0 || firstPart > (pathsSize - 1)) {
            ret = false;
        } else if (firstPart == (pathsSize - 1)) {
            SkParsePath::FromSVGString(paths_[firstPart].c_str(), &path);
            SkParsePath::FromSVGString(paths_[firstPart - 1].c_str(), &ending);
            ret = ending.interpolate(path, 1.0f, &out);
        } else {
            float newWeight = weight_ - firstPart;
            SkParsePath::FromSVGString(paths_[firstPart + 1].c_str(), &path);
            SkParsePath::FromSVGString(paths_[firstPart].c_str(), &ending);
            ret = ending.interpolate(path, newWeight, &out);
        }
        if (!ret) {
            SkParsePath::FromSVGString(d_.c_str(), &out);
        }
    }
    if (fillState_.IsEvenodd()) {
        out.setFillType(SkPath::FillType::kEvenOdd_FillType);
    }
}

void FlutterRenderSvgPath::OnGlobalPositionChanged()
{
    if (transformLayer_ && NeedTransform()) {
        transformLayer_->Update(UpdateTransformMatrix4());
    }
    RenderNode::OnGlobalPositionChanged();
}

} // namespace OHOS::Ace
