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

#include "core/components/list/flutter_render_list_item.h"

#include <base/utils/device_type.h>
#include <base/utils/system_properties.h>

#include "core/components/common/painter/flutter_decoration_painter.h"
#include "core/components/common/painter/flutter_universal_painter.h"
#include "core/components/transform/flutter_render_transform.h"
#include "core/pipeline/base/flutter_render_context.h"

namespace OHOS::Ace {
namespace {

constexpr uint8_t GREY_START = 255;
constexpr uint8_t GREY_END = 38;
constexpr uint8_t DISABLED_OPACITY = 102;
constexpr uint32_t ACTIVE_BACKGROUND_COLOR = 0x330A59F7;
constexpr double CENTER_POINT = 2.0;
constexpr double OPACITY_START = 0.0;
constexpr double OPACITY_END = 0.6;
constexpr double HALF_SIZE = 0.5;
constexpr double FADE_START = 1.0;
constexpr double FADE_END = 0.0;
constexpr double SCALE_START = 1.0;
constexpr double SCALE_END = 0.1;

} // namespace

using namespace Flutter;

RenderLayer FlutterRenderListItem::GetRenderLayer()
{
    if (!transformLayer_) {
        transformLayer_ = AceType::MakeRefPtr<TransformLayer>(Matrix4::CreateIdentity(), 0.0, 0.0);
    }
    return AceType::RawPtr(transformLayer_);
}

void FlutterRenderListItem::Paint(RenderContext& context, const Offset& offset)
{
    if (GetRedraw()) {
        PaintStickyEffectNoTransparent(context, offset);
        return;
    }
    if (focusController_ && !isStretch_) {
        PaintWithFocusEffect(context, offset);
    } else {
        // restore layer
        if (transformLayer_) {
            transformLayer_->Update(Matrix4::CreateIdentity());
        }
        // restore opacity
        if (!renderDisplay_) {
            auto children = GetChildren();
            if (!children.empty()) {
                renderDisplay_ = AceType::DynamicCast<RenderDisplay>(children.front());
            }
        }
        if (renderDisplay_ && !makeCardTransition_) {
            if (disabled_) {
                renderDisplay_->UpdateOpacity(DISABLED_OPACITY);
            } else if (isStretch_) {
                renderDisplay_->UpdateOpacity(expandOpacity_);
            } else {
                renderDisplay_->UpdateOpacity(DEFAULT_OPACITY);
            }
        }
        RenderNode::Paint(context, offset);
    }
    if (isActive_) {
        PaintActiveBackground(context, offset);
    }
    PaintItemDivider(context);
}

void FlutterRenderListItem::OnPaintFinish()
{
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    auto accessibilityNode = GetAccessibilityNode().Upgrade();
    if (!accessibilityNode) {
        LOGE("FlutterRenderListItem::accessibilityNode is null");
        return;
    }
    if (!NearEqual(scale_, 1.0)) {
        Size size = GetLayoutSize();
        Offset globalOffset = GetGlobalOffset();
        Offset scaleCenter = Offset(globalOffset.GetX() + size.Width() / 2.0,
                                    globalOffset.GetY() + size.Height() / 2.0);
        size = size * scale_;
        accessibilityNode->SetScaleToChild(scale_);
        accessibilityNode->SetScaleCenterToChild(scaleCenter);
        globalOffset = Offset(size.Width() * (1 - scale_) / 2.0 +  globalOffset.GetX(),
                              size.Height() * (1 - scale_) / 2.0 +  globalOffset.GetY());
        for (const auto& item : GetChildren()) {
            item->NotifyPaintFinish();
        }
    }
#endif
    if (!IsFocused()) {
        return;
    }
    UpdateItemFocusRect(scale_);
    ShowFocusAnimation(IsFocused(), Rect(0.0, 0.0, 0.0, 0.0), scale_);
}

void FlutterRenderListItem::OnGlobalPositionChanged()
{
    if (focusController_ && !isStretch_) {
        UpdateTransformLayer();
    }
    RenderListItem::OnGlobalPositionChanged();
}

void FlutterRenderListItem::UpdateTransformLayer()
{
    float translateX = GetLayoutSize().Width() * HALF_SIZE * (DEFAULT_SCALE - scale_);
    float translateY = GetLayoutSize().Height() * HALF_SIZE * (DEFAULT_SCALE - scale_);
    Matrix4 translateMatrix = Matrix4::CreateTranslate(translateX, translateY, 0.0);
    Matrix4 scaleMatrix = Matrix4::CreateScale(scale_, scale_, 1.0);
    Matrix4 transform = translateMatrix * scaleMatrix;
    auto offset = GetGlobalOffset();
    transform = FlutterRenderTransform::GetTransformByOffset(transform, offset);

    if (!transformLayer_) {
        transformLayer_ = AceType::MakeRefPtr<TransformLayer>(transform, 0.0, 0.0);
    } else {
        transformLayer_->Update(transform);
    }
}

void FlutterRenderListItem::PaintActiveBackground(RenderContext& context, const Offset& offset)
{
    PaintUniversalBackground(context, offset, ACTIVE_BACKGROUND_COLOR);
}

void FlutterRenderListItem::PaintUniversalBackground(RenderContext& context, const Offset& offset, uint32_t colorVal)
{
    const auto renderContext = static_cast<FlutterRenderContext*>(&context);
    flutter::Canvas* canvas = renderContext->GetCanvas();
    if (canvas == nullptr) {
        LOGE("Paint canvas is null");
        return;
    }
    SkCanvas* skCanvas = canvas->canvas();
    if (skCanvas == nullptr) {
        LOGE("Paint skCanvas is null");
        return;
    }
    auto size = GetPaintSize();
    auto margin = GetMarginInPx();
    auto backgroundOffset = offset + margin.GetOffset();
    RRect paintRRect = RRect::MakeRect(Rect(backgroundOffset, size));
    paintRRect.SetCorner(focusAnimationRRect_.GetCorner());
    double dipScale = 1.0;
    auto pipelineContext = GetContext().Upgrade();
    if (pipelineContext) {
        dipScale = pipelineContext->GetDipScale();
    }
    FlutterUniversalPainter::DrawRRectBackground(skCanvas, paintRRect, colorVal, dipScale);
}

void FlutterRenderListItem::PaintWithFocusEffect(RenderContext& context, const Offset& offset)
{
    if (focusController_) {
        if (SystemProperties::GetDeviceType() == DeviceType::WATCH) {
            if (supportScale_) {
                scale_ = GetScaleFactor();
            }
            if (supportOpacity_) {
                opacity_ = static_cast<uint8_t>(std::round(GetOpacityFactor() * DEFAULT_OPACITY));
            }
        } else {
            scale_ = focusController_->GetScale();
            opacity_ = static_cast<uint8_t>(std::round(focusController_->GetOpacity() * DEFAULT_OPACITY));
        }
        decorationAlpha_ = focusController_->GetAlpha();
    }

    if (supportOpacity_) {
        if (!renderDisplay_) {
            auto children = GetChildren();
            if (!children.empty()) {
                renderDisplay_ = AceType::DynamicCast<RenderDisplay>(children.front());
            }
        }
        if (renderDisplay_ && !makeCardTransition_) {
            if (disabled_) {
                renderDisplay_->UpdateOpacity(DISABLED_OPACITY);
            } else {
                renderDisplay_->UpdateOpacity(opacity_);
            }
        }
    }

    UpdateTransformLayer();
    RenderNode::Paint(context, offset);

    // paint front decoration
    if (!NearZero(decorationAlpha_) && !makeCardTransition_) {
        auto decorationColor = Color::BLACK;
        auto itemTheme = GetTheme<ListItemTheme>();
        if (itemTheme) {
            decorationColor = itemTheme->GetClickColor();
        }
        if (clickColor_ != Color::TRANSPARENT) {
            decorationColor = clickColor_;
        }
        auto frontDecoration = AceType::MakeRefPtr<Decoration>();
        frontDecoration->SetBackgroundColor(decorationColor.BlendOpacity(decorationAlpha_));

        auto pipelineContext = GetContext().Upgrade();
        Border border = GetFocusBorder();
        Corner corner = GetRRect().GetCorner();
        border.SetTopLeftRadius(corner.topLeftRadius);
        border.SetTopRightRadius(corner.topRightRadius);
        border.SetBottomLeftRadius(corner.bottomLeftRadius);
        border.SetBottomRightRadius(corner.bottomRightRadius);
        frontDecoration->SetBorder(border);
        RefPtr<FlutterDecorationPainter> decorationPainter =
            AceType::MakeRefPtr<FlutterDecorationPainter>(frontDecoration, GetPaintRect(), GetPaintSize(),
                pipelineContext ? pipelineContext->GetDipScale() : 1.0);
        decorationPainter->SetMargin(GetMarginInPx());
        const auto renderContext = static_cast<FlutterRenderContext*>(&context);
        flutter::Canvas* canvas = renderContext->GetCanvas();
        decorationPainter->PaintDecoration(offset, canvas->canvas(), context);
    }
}

void FlutterRenderListItem::PaintItemDivider(RenderContext& context)
{
    if (!NeedDivider() || NearZero(dividerHeight_.Value())) {
        return;
    }

    Offset position = GetPosition();
    Size layoutSize = GetLayoutSize();
    bool isVertical = IsListVertical();
    double dividerWidth = NormalizeToPx(dividerHeight_);

    // calculate item size exclude divider
    double width = isVertical ? layoutSize.Width() : layoutSize.Width() - dividerWidth;
    double height = isVertical ? layoutSize.Height() - dividerWidth : layoutSize.Height();

    // calculate start point
    auto startOrigin = NormalizeToPx(dividerOrigin_);
    double startPointOffset = isVertical ? std::min(startOrigin, width) : std::min(startOrigin, height);
    double startPointX = isVertical ? position.GetX() + startPointOffset : position.GetX() + width;
    double startPointY = isVertical ? position.GetY() + height : position.GetY() + startPointOffset;

    // calculate end point
    double length = NormalizeToPx(dividerLength_);
    if (NearZero(length)) {
        length = isVertical ? width : height;
    }
    double endPointX = isVertical ? position.GetX() + std::min(width, startOrigin + length) : startPointX;
    double endPointY = isVertical ? startPointY : position.GetY() + std::min(height, startOrigin + length);

    flutter::Paint paint;
    flutter::PaintData paintData;
    paint.paint()->setStrokeWidth(dividerWidth);
    paint.paint()->setColor(dividerColor_.GetValue());

    const auto renderContext = static_cast<FlutterRenderContext*>(&context);
    flutter::Canvas* canvas = renderContext->GetCanvas();
    if (isVertical) {
        canvas->drawLine(startPointX, startPointY + dividerWidth / 2.0,
            endPointX, endPointY + dividerWidth / 2.0, paint, paintData);
    } else {
        canvas->drawLine(startPointX + dividerWidth / 2.0, startPointY,
            endPointX + dividerWidth / 2.0, endPointY, paint, paintData);
    }
}

void FlutterRenderListItem::PaintStickyEffect(RenderContext& context, const Offset& offset)
{
    RenderNode::Paint(context, offset);
    const auto renderContext = static_cast<FlutterRenderContext*>(&context);
    flutter::Canvas* canvas = renderContext->GetCanvas();
    if (canvas == nullptr) {
        LOGE("Canvas is null, save failed.");
        return;
    }
    canvas->save();
    Offset center;
    flutter::Paint paint;
    flutter::PaintData paintData;
    double width = GetLayoutSize().Width() + offset.GetX();
    double height = GetLayoutSize().Height() + offset.GetY();
    if (offset.GetY() <= 0.0) {
        center.SetX((GetLayoutSize().Width() + offset.GetX()) / CENTER_POINT);
        center.SetY(GetLayoutSize().Height() + offset.GetY() - GetStickyRadius());
        paint.paint()->setColor(Color::FromRGBO(GREY_END, GREY_END, GREY_END, OPACITY_END).GetValue());
        canvas->clipRect(0, 0, width, height, SkClipOp::kIntersect);
        canvas->drawCircle(center.GetX(), center.GetY(), GetStickyRadius(), paint, paintData);
    } else {
        double radius =
            GetStickyRadius() + (RADIUS_START - GetStickyRadius()) * (offset.GetY() / GetLayoutSize().Height());
        uint8_t grey = GREY_END + (GREY_START - GREY_END) * (offset.GetY() / GetLayoutSize().Height());
        double opacity = OPACITY_END + (OPACITY_START - OPACITY_END) * (offset.GetY() / GetLayoutSize().Height());
        center.SetX((GetLayoutSize().Width() + offset.GetX()) / CENTER_POINT);
        center.SetY(GetLayoutSize().Height() + offset.GetY() - radius);
        paint.paint()->setColor(Color::FromRGBO(grey, grey, grey, opacity).GetValue());
        canvas->clipRect(offset.GetX(), offset.GetY(), width, height, SkClipOp::kIntersect);
        canvas->drawCircle(center.GetX(), center.GetY(), radius, paint, paintData);
    }
    canvas->restore();
}

void FlutterRenderListItem::PaintStickyEffectNoTransparent(RenderContext& context, const Offset& offset)
{
    Size itemSize = GetLayoutSize();
    if (NearZero(itemSize.Height()) || NearZero(itemSize.Width())) {
        return;
    }
    RefPtr<RenderBox> box = nullptr;
    RefPtr<RenderNode> childNode = GetChildren().front();
    while (childNode) {
        box = AceType::DynamicCast<RenderBox>(childNode);
        if (box || childNode->GetChildren().empty()) {
            break;
        }
        childNode = childNode->GetChildren().front();
    }

    LOGD("[indexer] offset:%{public}lf %{public}s", offset.GetY(), itemSize.ToString().c_str());
    if (box) {
        RefPtr<Decoration> background = AceType::MakeRefPtr<Decoration>();
        auto backgroundOld = box->GetBackDecoration();
        if (offset.GetY() > 0.0) { // Next Sticky item.
            PaintNextSticky(background, offset);
        } else { // Current Sticky item.
            PaintCurrentSticky(background, offset);
            PaintFadeOutEffect(box, offset);
        }
        box->SetBackDecoration(background);
        RenderNode::Paint(context, offset);
        // reset background
        box->SetBackDecoration(backgroundOld);
    } else {
        RenderNode::Paint(context, offset);
    }
}

void FlutterRenderListItem::PaintCurrentSticky(const RefPtr<Decoration>& background, const Offset& offset)
{
    Size itemSize = GetLayoutSize();
    double mainOffset = offset.GetY();
    uint8_t gray = GREY_END;
    Color color = Color::FromRGB(gray, gray, gray);
    double radius = GetStickyRadius();
    if (GetStickyMode() != StickyMode::OPACITY) {
        Point center;
        center.SetX((itemSize.Width() + offset.GetX()) / CENTER_POINT);
        center.SetY(itemSize.Height() + offset.GetY() - radius);
        RefPtr<ArcBackground> arcBg = AceType::MakeRefPtr<ArcBackground>(center, radius);
        arcBg->SetColor(color);
        background->SetArcBackground(arcBg);
        gray *= (-mainOffset) / itemSize.Height();
    }
    if (offset.GetY() < 0.0 || GetStickyMode() == StickyMode::OPACITY) {
        color = Color::FromRGB(gray, gray, gray);
        background->SetBackgroundColor(color);
    }
}

void FlutterRenderListItem::PaintNextSticky(const RefPtr<Decoration>& background, const Offset& offset)
{
    Size itemSize = GetLayoutSize();
    double mainOffset = offset.GetY();
    double radius = GetStickyRadius();
    // calculate radius
    if (GetNearByOpacitySticky()) {
        mainOffset = 0.0; // Make the background same as current sticky item.
    } else {
        mainOffset = std::clamp(mainOffset, 0.0, itemSize.Height());
    }
    radius = radius + (RADIUS_START - radius) * pow(mainOffset / itemSize.Height(), 3.0);

    // calculate color
    uint8_t gray = GREY_END * (itemSize.Height() - mainOffset) / itemSize.Height();
    Color color = Color::FromRGB(gray, gray, gray);

    // calculate center
    Point center;
    center.SetX((itemSize.Width() + offset.GetX()) / CENTER_POINT);
    center.SetY(itemSize.Height() + offset.GetY() - radius);
    RefPtr<ArcBackground> arcBg = AceType::MakeRefPtr<ArcBackground>(center, radius);
    arcBg->SetColor(color);
    background->SetArcBackground(arcBg);
}

void FlutterRenderListItem::PaintFadeOutEffect(const RefPtr<RenderNode>& node, const Offset& offset)
{
    if (stickyMode_ != StickyMode::OPACITY || !node || node->GetChildren().empty()) {
        return;
    }
    auto opacityNode = AceType::DynamicCast<RenderDisplay>(node->GetChildren().front());
    if (!opacityNode || opacityNode->GetChildren().empty()) {
        return;
    }
    auto scaleNode = AceType::DynamicCast<RenderTransform>(opacityNode->GetChildren().front());
    if (!scaleNode || scaleNode->GetChildren().empty()) {
        return;
    }
    double height = GetLayoutSize().Height() / 2;
    double mainOffset = std::abs(offset.GetY());
    double opacity = std::max(FADE_END, FADE_START - mainOffset / height) * 255.0;
    double scale = SCALE_START - (std::min(SCALE_START, mainOffset / height) * SCALE_END);
    if (!makeCardTransition_) {
        opacityNode->UpdateOpacity(opacity);
    }
    scaleNode->ResetTransform();
    scaleNode->Scale(scale);
    LOGD("Sticky opacity:%lf scale:%lf %s", opacity, scale, offset.ToString().c_str());
}

bool FlutterRenderListItem::HasEffectiveTransform() const
{
    if (!transformLayer_) {
        return false;
    }
    return !transformLayer_->GetMatrix4().IsIdentityMatrix();
}

} // namespace OHOS::Ace
