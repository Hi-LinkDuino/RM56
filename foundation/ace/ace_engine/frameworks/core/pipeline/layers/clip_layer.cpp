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

#include "core/pipeline/layers/clip_layer.h"

#include "base/log/dump_log.h"

namespace OHOS::Ace::Flutter {

void ClipLayer::AddToScene(SceneBuilder& builder, double x, double y)
{
    if (isPath_) {
        fml::RefPtr<flutter::CanvasPath> path = flutter::CanvasPath::Create();
        path->addPath(canvasPath_.get(), x + x_, y + y_);
        builder.PushClipPath(path.get(), static_cast<int32_t>(clipBehavior_));
    } else {
        flutter::RRect rrect;
        rrect.sk_rrect = rrect_.sk_rrect.makeOffset(x + x_, y + y_);
        builder.PushClipRRect(rrect, static_cast<int32_t>(clipBehavior_));
    }

    if (isMask_) {
        if (isSvgMask_) {
            builder.PushSvgMask(svgDom_, svgX_, svgY_, scaleX_, scaleY_);
        } else if (isGradientMask_) {
            builder.PushGradientColorMask(maskPaint_);
        } else if (isPathMask_) {
            builder.PushPathMask(maskPaint_, maskPath_);
        }
    }

    AddChildToScene(builder, x + x_, y + y_);
    if (isMask_) {
        builder.Pop();
    }
    builder.Pop();
}

void ClipLayer::SetClip(double left, double right, double top, double bottom, Clip clipBehavior)
{
    rrect_.sk_rrect = SkRRect::MakeRect(SkRect::MakeLTRB(static_cast<SkScalar>(left), static_cast<SkScalar>(top),
        static_cast<SkScalar>(right), static_cast<SkScalar>(bottom)));
    clipBehavior_ = clipBehavior;
    isPath_ = false;
}

void ClipLayer::SetClipRRect(const RRect& rrect, Clip clipBehavior)
{
    Rect rect = rrect.GetRect();
    SkVector radii[4] = { { 0.0, 0.0 }, { 0.0, 0.0 }, { 0.0, 0.0 }, { 0.0, 0.0 } };
    radii[SkRRect::kUpperLeft_Corner] = GetSkRadii(rrect.GetCorner().topLeftRadius);
    radii[SkRRect::kUpperRight_Corner] = GetSkRadii(rrect.GetCorner().topRightRadius);
    radii[SkRRect::kLowerLeft_Corner] = GetSkRadii(rrect.GetCorner().bottomLeftRadius);
    radii[SkRRect::kLowerRight_Corner] = GetSkRadii(rrect.GetCorner().bottomRightRadius);
    rrect_.sk_rrect.setRectRadii(SkRect::MakeXYWH(rect.Left(), rect.Top(), rect.Width(), rect.Height()), radii);
    clipBehavior_ = clipBehavior;
    isPath_ = false;
}

void ClipLayer::SetClipRRect(const Rect& rect, double x, double y, Clip clipBehavior)
{
    rrect_.sk_rrect = SkRRect::MakeRectXY(SkRect::MakeXYWH(rect.Left(), rect.Top(), rect.Width(), rect.Height()),
        SkDoubleToScalar(x), SkDoubleToScalar(y));
    clipBehavior_ = clipBehavior;
    isPath_ = false;
}

void ClipLayer::SetClipRRect(const flutter::RRect& rrect)
{
    rrect_.sk_rrect = rrect.sk_rrect;
    isPath_ = false;
}

void ClipLayer::SetClipPath(const fml::RefPtr<flutter::CanvasPath>& canvasPath, Clip clipBehavior)
{
    canvasPath_ = canvasPath;
    clipBehavior_ = clipBehavior;
    isPath_ = true;
}

void ClipLayer::SetClipPath(const fml::RefPtr<flutter::CanvasPath>& canvasPath)
{
    canvasPath_ = canvasPath;
    isPath_ = true;
}

SkVector ClipLayer::GetSkRadii(const Radius& radius)
{
    SkVector fRadii;
    fRadii.set(
        SkDoubleToScalar(std::max(radius.GetX().Value(), 0.0)), SkDoubleToScalar(std::max(radius.GetY().Value(), 0.0)));
    return fRadii;
}

void ClipLayer::CancelMask()
{
    isMask_ = false;
}

void ClipLayer::SetSvgMask(const sk_sp<SkSVGDOM>& svgDom, double x, double y, double scaleX, double scaleY)
{
    isMask_ = true;
    isSvgMask_ = true;
    isGradientMask_ = false;
    isPathMask_ = false;

    svgX_ = x;
    svgY_ = y;
    scaleX_ = scaleX;
    scaleY_ = scaleY;
    svgDom_ = svgDom;
}

void ClipLayer::SetColorMask(const SkPaint& maskPaint)
{
    isMask_ = true;
    isSvgMask_ = false;
    isGradientMask_ = true;
    isPathMask_ = false;
    maskPaint_ = maskPaint;
}

void ClipLayer::SetPathMask(const SkPaint& maskPaint, const SkPath& path)
{
    isMask_ = true;
    isSvgMask_ = false;
    isGradientMask_ = false;
    isPathMask_ = true;
    maskPaint_ = maskPaint;
    maskPath_ = path;
}

void ClipLayer::Dump()
{
    OffsetLayer::Dump();
    if (DumpLog::GetInstance().GetDumpFile()) {
        SkRect rect = rrect_.sk_rrect.getBounds();
        DumpLog::GetInstance().AddDesc(
            "ClipRect: (", rect.left(), ",", rect.right(), ") -", rect.width(), "x", rect.height());
        SkVector leftTop = rrect_.sk_rrect.radii(SkRRect::Corner::kUpperLeft_Corner);
        SkVector rightTop = rrect_.sk_rrect.radii(SkRRect::Corner::kUpperRight_Corner);
        SkVector leftBottom = rrect_.sk_rrect.radii(SkRRect::Corner::kLowerLeft_Corner);
        SkVector rightBottom = rrect_.sk_rrect.radii(SkRRect::Corner::kLowerRight_Corner);
        DumpLog::GetInstance().AddDesc("leftTopCorner: (", leftTop.fX, ",", leftTop.fY, ")", "rightTopCorner: (",
            rightTop.fX, ",", rightTop.fY, ")");
        DumpLog::GetInstance().AddDesc("leftBottomCorner: (", leftBottom.fX, ",", leftBottom.fY, ")",
            "rightBottomCorner: (", rightBottom.fX, ",", rightBottom.fY, ")");
    }
}

} // namespace OHOS::Ace::Flutter
