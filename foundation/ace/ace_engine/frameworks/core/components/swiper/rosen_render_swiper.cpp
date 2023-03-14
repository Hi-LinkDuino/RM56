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

#include "core/components/swiper/rosen_render_swiper.h"

#include "render_service_client/core/ui/rs_node.h"
#include "third_party/skia/include/core/SkPaint.h"
#include "third_party/skia/include/effects/SkGradientShader.h"

#include "base/utils/system_properties.h"
#include "core/components/align/render_align.h"
#include "core/components/common/properties/alignment.h"
#include "core/components/common/properties/decoration.h"
#include "core/components/text/text_component.h"
#include "core/pipeline/base/rosen_render_context.h"

namespace OHOS::Ace {
namespace {

constexpr Dimension INDICATOR_POINT_PADDING_TOP = 9.0_vp;
constexpr uint32_t GRADIENT_COLOR_SIZE = 3;
constexpr int32_t INDICATOR_FOCUS_PADDING_START_SIZE = 2;

} // namespace

void RosenRenderSwiper::Update(const RefPtr<Component>& component)
{
    RenderSwiper::Update(component);
    if (auto rsNode = GetRSNode()) {
        rsNode->SetClipToFrame(true);
    }
}

void RosenRenderSwiper::Paint(RenderContext& context, const Offset& offset)
{
    RenderNode::Paint(context, offset);

    if (auto rsNode = static_cast<RosenRenderContext*>(&context)->GetRSNode()) {
        rsNode->SetPaintOrder(true);
    }

    if (!indicator_) {
        LOGD("swiper has not default indicator");
        return;
    }
    // indicator style in tv is different.
    if (SystemProperties::GetDeviceType() != DeviceType::TV) {
        DrawIndicator(context, offset);
    } else {
        if (indicator_->GetIndicatorMask()) {
            PaintMask(context, offset);
        }
        PaintIndicator(context, offset);
    }

    if (isPaintedFade_) {
        PaintFade(context, offset);
    }
}

void RosenRenderSwiper::PaintFade(RenderContext& context, const Offset& offset)
{
    const auto renderContext = static_cast<RosenRenderContext*>(&context);
    if (!renderContext) {
        return;
    }
    auto canvas = renderContext->GetCanvas();
    if (canvas == nullptr) {
        return;
    }
    canvas->save();
    canvas->translate(offset.GetX(), offset.GetY());
    PaintShadow(canvas, offset);
    canvas->restore();
}

void RosenRenderSwiper::PaintShadow(SkCanvas* canvas, const Offset& offset)
{
    static constexpr double FADE_MAX_DISTANCE = 2000.0f;
    static constexpr double FADE_MAX_TRANSLATE = 40.0f;
    static constexpr double FADE_MAX_RADIUS = 2.0f;
    static constexpr double FADE_ALPHA = 0.45f;
    static constexpr double FADE_SCALE_RATE = 0.2f;

    bool isVertical = axis_ == Axis::VERTICAL;
    double height = swiperHeight_;
    double width = swiperWidth_;
    double centerX = 0.0;
    double centerY = 0.0;
    double fadeTranslate = dragDelta_ * FADE_SCALE_RATE;
    double radius = 0.0;
    if (GreatNotEqual(dragDelta_, 0.0)) {
        fadeTranslate = fadeTranslate > FADE_MAX_TRANSLATE ? FADE_MAX_TRANSLATE : fadeTranslate;
        if (isVertical) {
            centerY = -FADE_MAX_DISTANCE + dragDelta_ / FADE_SCALE_RATE;
            if (centerY > (-width * FADE_MAX_RADIUS)) {
                centerY = -width * FADE_MAX_RADIUS;
            }
            centerX = width / 2;
        } else {
            centerX = -FADE_MAX_DISTANCE + dragDelta_ / FADE_SCALE_RATE;
            if (centerX > (-FADE_MAX_RADIUS * height)) {
                centerX = (-FADE_MAX_RADIUS * height);
            }
            centerY = height / 2;
        }
        radius = sqrt(pow(centerX, 2) + pow(centerY, 2));

    } else {
        fadeTranslate = fadeTranslate > -FADE_MAX_TRANSLATE ? fadeTranslate : -FADE_MAX_TRANSLATE;
        if (isVertical) {
            centerY = height + FADE_MAX_DISTANCE + dragDelta_ / FADE_SCALE_RATE;
            if (centerY < (height + width * FADE_MAX_RADIUS)) {
                centerY = height + width * FADE_MAX_RADIUS;
            }
            centerX = width / 2;
            radius = sqrt(pow(centerY - height, 2) + pow(centerX, 2));
        } else {
            centerX = width + FADE_MAX_DISTANCE + dragDelta_ / FADE_SCALE_RATE;
            if (centerX < (width + FADE_MAX_RADIUS * height)) {
                centerX = width + FADE_MAX_RADIUS * height;
            }
            centerY = height / 2;
            radius = sqrt(pow(centerX - width, 2) + pow(centerY, 2));
        }
    }

    Offset center = Offset(centerX, centerY);
    SkPaint painter;
    painter.setColor(fadeColor_.GetValue());
    painter.setAlphaf(FADE_ALPHA);
    painter.setBlendMode(SkBlendMode::kSrcOver);
    if (isVertical) {
        canvas->drawCircle(center.GetX(), center.GetY() + fadeTranslate, radius, painter);
    } else {
        canvas->drawCircle(center.GetX() + fadeTranslate, center.GetY(), radius, painter);
    }
}

void RosenRenderSwiper::UpdateIndicator()
{
    if (!indicator_) {
        LOGD("swiper has not default indicator");
        return;
    }

    double size = NormalizeToPx(indicator_->GetSize());
    double selectedSize = NormalizeToPx(indicator_->GetSelectedSize());
    double width = selectedSize + size * (itemCount_ - 1) +
                   indicator_->GetIndicatorPointPadding().Value() * scale_ * (itemCount_ - 1);
    double indicatorWidth;
    double indicatorHeight;
    if (digitalIndicator_) {
        LayoutDigitalIndicator();
        Size digitalIndicatorSize = renderDigitalIndicator_->GetLayoutSize();
        indicatorWidth = digitalIndicatorSize.Width();
        indicatorHeight = digitalIndicatorSize.Height();
    } else if (axis_ == Axis::HORIZONTAL) {
        indicatorWidth = width;
        indicatorHeight = selectedSize;
    } else {
        indicatorWidth = selectedSize;
        indicatorHeight = width;
    }

    Offset position;
    if (indicator_->GetLeft().Value() != SwiperIndicator::DEFAULT_POSITION) {
        int32_t left = GetValidEdgeLength(swiperWidth_, indicatorWidth, indicator_->GetLeft());
        position.SetX(left);
    } else if (indicator_->GetRight().Value() != SwiperIndicator::DEFAULT_POSITION) {
        int32_t right = GetValidEdgeLength(swiperWidth_, indicatorWidth, indicator_->GetRight());
        position.SetX(swiperWidth_ - indicatorWidth - right);
    } else {
        if (axis_ == Axis::HORIZONTAL) {
            position.SetX((swiperWidth_ - indicatorWidth) / 2.0);
        } else {
            indicatorWidth += NormalizeToPx(INDICATOR_POINT_PADDING_TOP);
            position.SetX(swiperWidth_ - indicatorWidth);
        }
    }

    if (indicator_->GetTop().Value() != SwiperIndicator::DEFAULT_POSITION) {
        int32_t top = GetValidEdgeLength(swiperHeight_, indicatorHeight, indicator_->GetTop());
        position.SetY(top);
    } else if (indicator_->GetBottom().Value() != SwiperIndicator::DEFAULT_POSITION) {
        int32_t bottom = GetValidEdgeLength(swiperHeight_, indicatorHeight, indicator_->GetBottom());
        position.SetY(swiperHeight_ - indicatorHeight - bottom);
    } else {
        if (axis_ == Axis::HORIZONTAL) {
            indicatorHeight += NormalizeToPx(INDICATOR_POINT_PADDING_TOP);
            position.SetY(swiperHeight_ - indicatorHeight);
        } else {
            position.SetY((swiperHeight_ - indicatorHeight) / 2.0);
        }
    }

    indicatorPosition_ = position;
}

void RosenRenderSwiper::LayoutDigitalIndicator()
{
    LayoutParam innerLayout;
    innerLayout.SetMaxSize(Size(Size::INFINITE_SIZE, Size::INFINITE_SIZE));

    std::string indicatorText =
        (axis_ == Axis::HORIZONTAL)
            ? std::to_string(currentIndex_ + 1).append("/").append(std::to_string(itemCount_))
            : std::to_string(currentIndex_ + 1).append("\n/\n").append(std::to_string(itemCount_));
    RefPtr<TextComponent> textComponent = AceType::MakeRefPtr<TextComponent>(indicatorText);
    renderDigitalIndicator_ = AceType::DynamicCast<RenderText>(textComponent->CreateRenderNode());
    auto textStyle = indicator_->GetDigitalIndicatorTextStyle();
    textStyle.SetTextAlign(TextAlign::CENTER);
    textComponent->SetTextStyle(textStyle);
    renderDigitalIndicator_->Attach(GetContext());
    renderDigitalIndicator_->Update(textComponent);
    renderDigitalIndicator_->Layout(innerLayout);
}

void RosenRenderSwiper::PaintIndicator(RenderContext& context, const Offset& offset)
{
    if (digitalIndicator_) {
        LayoutDigitalIndicator();
        renderDigitalIndicator_->RenderWithContext(context, indicatorPosition_);
        return;
    }

    CanvasDrawIndicator(context, offset);
}

void RosenRenderSwiper::CanvasDrawIndicator(RenderContext& context, const Offset& offset)
{
    auto canvas = static_cast<RosenRenderContext*>(&context)->GetCanvas();
    if (!canvas) {
        LOGE("Paint canvas is null");
        return;
    }

    SkPaint paint;
    paint.setAntiAlias(true);

    IndicatorProperties indicatorProperties = PrepareIndicatorProperties();
    Offset center = indicatorPosition_ + indicatorProperties.centerPadding;
    double targetIndex = currentIndex_;
    if (needReverse_) {
        targetIndex = itemCount_ - currentIndex_ - 1;
    }
    for (int32_t i = 0; i < itemCount_; i++) {
        if (i != targetIndex) {
            center += indicatorProperties.normalPaddingStart;
            paint.setColor(indicatorProperties.normalColor);
            canvas->drawCircle(center.GetX() + offset.GetX(), center.GetY() + offset.GetY(),
                indicatorProperties.normalPointRadius, paint);
            center += indicatorProperties.normalPaddingEnd;
        } else {
            center += indicatorProperties.selectedPaddingStart;
            paint.setColor(indicatorProperties.selectedColor);
            canvas->drawCircle(center.GetX() + offset.GetX(), center.GetY() + offset.GetY(),
                indicatorProperties.selectedPointRadius, paint);
            center += indicatorProperties.selectedPaddingEnd;
        }
    }
}

RosenRenderSwiper::IndicatorProperties RosenRenderSwiper::PrepareIndicatorProperties() const
{
    uint32_t normalColor = indicator_->GetColor().GetValue();
    uint32_t selectedColor = indicator_->GetSelectedColor().GetValue();
    double normalPointRadius = NormalizeToPx(indicator_->GetSize()) / 2.0;
    double selectedPointRadius = NormalizeToPx(indicator_->GetSelectedSize()) / 2.0;
    double indicatorPointPadding = indicator_->GetIndicatorPointPadding().Value() * scale_;
    if (axis_ == Axis::HORIZONTAL) {
        return IndicatorProperties(Offset(normalPointRadius, 0.0),
            Offset(normalPointRadius + indicatorPointPadding, 0.0), Offset(selectedPointRadius, 0.0),
            Offset(selectedPointRadius + indicatorPointPadding, 0.0), Offset(0.0, selectedPointRadius), normalColor,
            selectedColor, normalPointRadius, selectedPointRadius, indicatorPointPadding);
    } else {
        return IndicatorProperties(Offset(0.0, normalPointRadius),
            Offset(0.0, normalPointRadius + indicatorPointPadding), Offset(0.0, selectedPointRadius),
            Offset(0.0, selectedPointRadius + indicatorPointPadding), Offset(selectedPointRadius, 0.0), normalColor,
            selectedColor, normalPointRadius, selectedPointRadius, indicatorPointPadding);
    }
}

void RosenRenderSwiper::PaintMask(RenderContext& context, const Offset& offset) const
{
    auto canvas = static_cast<RosenRenderContext*>(&context)->GetCanvas();
    if (canvas == nullptr) {
        LOGE("Paint canvas is null");
        return;
    }

    SkPaint paint;
    paint.setAntiAlias(true);
    canvas->save();

    std::vector<GradientColor> gradientColors = std::vector<GradientColor>(GRADIENT_COLOR_SIZE);
    gradientColors[0].SetColor(Color(0x00000000));
    gradientColors[1].SetColor(Color(0xff000000));
    gradientColors[2].SetColor(Color(0xff000000));
    SkPoint pts[2] = { SkPoint::Make(0.0f, 0.0f), SkPoint::Make(0.0f, 0.0f) };
    if (axis_ == Axis::HORIZONTAL) {
        pts[0] = SkPoint::Make(SkDoubleToScalar(offset.GetX()),
            SkDoubleToScalar(offset.GetY() + indicatorPosition_.GetY() - NormalizeToPx(9.0_vp)));
        pts[1] = SkPoint::Make(SkDoubleToScalar(offset.GetX()),
            SkDoubleToScalar(offset.GetY() + indicatorPosition_.GetY() + NormalizeToPx(15.0_vp)));
    } else {
        pts[0] = SkPoint::Make(SkDoubleToScalar(offset.GetX() + indicatorPosition_.GetX() - NormalizeToPx(9.0_vp)),
            SkDoubleToScalar(offset.GetY()));
        pts[1] = SkPoint::Make(SkDoubleToScalar(offset.GetX() + indicatorPosition_.GetX() + NormalizeToPx(15.0_vp)),
            SkDoubleToScalar(offset.GetY()));
    }
    LOGD("gradient--beginPoint x: %{public}f, y: %{public}f", pts[0].x(), pts[0].y());
    LOGD("gradient--endPoint x: %{public}f, y: %{public}f", pts[1].x(), pts[1].y());
    SkColor colors[gradientColors.size()];
    for (uint32_t i = 0; i < gradientColors.size(); ++i) {
        const auto& gradientColor = gradientColors[i];
        colors[i] = gradientColor.GetColor().GetValue();
    }
    const float pos[] = { 0.0f, 0.75f, 1.0f };

#ifdef USE_SYSTEM_SKIA
    paint.setShader(SkGradientShader::MakeLinear(pts, colors, pos, gradientColors.size(), SkShader::kClamp_TileMode));
#else
    paint.setShader(SkGradientShader::MakeLinear(pts, colors, pos, gradientColors.size(), SkTileMode::kClamp));
#endif
    if (axis_ == Axis::HORIZONTAL) {
        canvas->drawRect({ offset.GetX(), offset.GetY() + indicatorPosition_.GetY() - NormalizeToPx(9.0_vp),
                             offset.GetX() + GetLayoutSize().Width(),
                             offset.GetY() + indicatorPosition_.GetY() + NormalizeToPx(15.0_vp) },
            paint);
    } else {
        canvas->drawRect({ offset.GetX() + indicatorPosition_.GetX() - NormalizeToPx(9.0_vp), offset.GetY(),
                             offset.GetX() + indicatorPosition_.GetX() + NormalizeToPx(15.0_vp),
                             offset.GetY() + GetLayoutSize().Height() },
            paint);
    }
}

void RosenRenderSwiper::DrawIndicator(RenderContext& context, const Offset& offset)
{
    if (digitalIndicator_) {
        if (swiperIndicatorData_.textBoxRender) {
            swiperIndicatorData_.textBoxRender->RenderWithContext(context, indicatorPosition_ + offset);
        }
    } else {
        if (swiperIndicatorData_.isPressed || swiperIndicatorData_.isHovered) {
            DrawIndicatorBackground(context, offset);
        }
        DrawIndicatorItems(context, offset);
    }
}

void RosenRenderSwiper::DrawIndicatorBackground(RenderContext& context, const Offset& offset)
{
    auto canvas = static_cast<RosenRenderContext*>(&context)->GetCanvas();
    if (!canvas) {
        LOGE("Paint canvas is null");
        return;
    }

    SkPaint paint;
    paint.setAntiAlias(true);

    paint.setColor(swiperIndicatorData_.indicatorPaintData.color.GetValue());
    paint.setAlphaf(opacityValue_);
    SkRRect rRect;
    Offset position = swiperIndicatorData_.indicatorPaintData.position;
    double radius = swiperIndicatorData_.indicatorPaintData.radius;
    rRect.setRectXY(
        SkRect::MakeIWH(swiperIndicatorData_.indicatorPaintData.width, swiperIndicatorData_.indicatorPaintData.height),
        radius, radius);
    rRect.offset(position.GetX() + offset.GetX(), position.GetY() + offset.GetY());
    canvas->drawRRect(rRect, paint);
}

void RosenRenderSwiper::DrawIndicatorItems(RenderContext& context, const Offset& offset)
{
    auto canvas = static_cast<RosenRenderContext*>(&context)->GetCanvas();
    if (!canvas) {
        LOGE("Paint canvas is null");
        return;
    }

    SkPaint paint;
    paint.setAntiAlias(true);

    InitMoveRange();
    IndicatorOffsetInfo pointInfo;
    SkRRect rRect;
    GetRRect(rRect, pointInfo.focusStart, pointInfo.focusEnd, offset);

    for (int32_t i = 0; i < itemCount_; i++) {
        // calculate point offset
        pointInfo.animationMove.Reset();
        GetIndicatorPointMoveOffset(i, pointInfo.animationMove);
        pointInfo.center = swiperIndicatorData_.indicatorItemData[i].center + indicatorPosition_;

        // hide point of indicator
        if (HideIndicatorPoint(i, pointInfo, offset)) {
            continue;
        }
        // paint point of indicator, and point adsorbent
        paint.setColor(indicator_->GetColor().GetValue());
        canvas->drawCircle(pointInfo.center.GetX() + offset.GetX() - pointInfo.animationMove.GetX(),
            pointInfo.center.GetY() + offset.GetY() - pointInfo.animationMove.GetY(),
            swiperIndicatorData_.indicatorItemData[i].radius, paint);
    }

    paint.setColor(indicator_->GetSelectedColor().GetValue());
    canvas->drawRRect(rRect, paint);
}

void RosenRenderSwiper::GetIndicatorPointMoveOffset(int32_t index, Offset& animationMove)
{
    if (NearZero(indicatorPointOffset_)) {
        return;
    }

    if ((index <= moveStartIndex_ && index >= moveEndIndex_) || (index >= moveStartIndex_ && index <= moveEndIndex_)) {
        double move = indicatorPointOffset_ * swiperIndicatorData_.indicatorItemData[index].radius;
        if (index != moveStartIndex_ && index != moveEndIndex_) {
            // the middle points should move distance of diameter
            move *= 2.0;
        }
        if (axis_ == Axis::HORIZONTAL) {
            animationMove.SetX(move);
        } else {
            animationMove.SetY(move);
        }
    }
}

void RosenRenderSwiper::GetRRect(SkRRect& rRect, double& startOffset, double& endOffset, const Offset& offset)
{
    // calculate focus move distance
    double tailOffset =
        (GetIndicatorSpringStatus() == SpringStatus::FOCUS_SWITCH) ? indicatorSwitchTailOffset_ : indicatorTailOffset_;
    Offset focusMove;
    Offset focusStretch;
    double focusStartPadding =
        INDICATOR_FOCUS_PADDING_START_SIZE * swiperIndicatorData_.indicatorItemData[moveStartIndex_].radius;
    double focusMoveLength = swiperIndicatorData_.pointPadding + focusStartPadding;
    double focusMoveDistance =
        focusMoveLength * (animationDirect_ > 0 ? tailOffset : indicatorHeadOffset_);
    double recStretch = focusMoveLength * (indicatorHeadOffset_ - tailOffset) * animationDirect_;
    (axis_ == Axis::HORIZONTAL) ? focusMove.SetX(focusMoveDistance) : focusMove.SetY(focusMoveDistance);
    (axis_ == Axis::HORIZONTAL) ? focusStretch.SetX(recStretch) : focusStretch.SetY(recStretch);

    // paint focus of indicator
    Offset position = swiperIndicatorData_.indicatorItemData[moveStartIndex_].position + indicatorPosition_;
    double radius = swiperIndicatorData_.indicatorItemData[moveStartIndex_].radius;
    auto rectWH = SkRect::MakeWH(
        swiperIndicatorData_.indicatorItemData[moveStartIndex_].width + focusStretch.GetX(),
        swiperIndicatorData_.indicatorItemData[moveStartIndex_].height + focusStretch.GetY());
    rRect.setRectXY(rectWH, radius, radius);
    rRect.offset(
        position.GetX() + offset.GetX() + focusMove.GetX(), position.GetY() + offset.GetY() + focusMove.GetY());

    // rrect range
    if (axis_ == Axis::HORIZONTAL) {
        startOffset = position.GetX() + offset.GetX() + focusMove.GetX();
        endOffset = startOffset + swiperIndicatorData_.indicatorItemData[moveStartIndex_].width + focusStretch.GetX();
    } else {
        startOffset = position.GetY() + offset.GetY() + focusMove.GetY();
        endOffset = startOffset + swiperIndicatorData_.indicatorItemData[moveStartIndex_].height + focusStretch.GetY();
    }
}

bool RosenRenderSwiper::HideIndicatorPoint(int32_t index, const IndicatorOffsetInfo& pointInfo, const Offset& offset)
{
    if (index == moveStartIndex_ || index == moveEndIndex_) {
        if (index == moveStartIndex_ && NearZero(indicatorHeadOffset_)) {
            return true;
        }
        double pointStart;
        double radius = swiperIndicatorData_.indicatorItemData[index].radius;
        if (axis_ == Axis::HORIZONTAL) {
            pointStart = pointInfo.center.GetX() + offset.GetX() - pointInfo.animationMove.GetX() - radius;
        } else {
            pointStart = pointInfo.center.GetY() + offset.GetY() - pointInfo.animationMove.GetY() - radius;
        }
        double pointEnd = pointStart + swiperIndicatorData_.indicatorItemData[index].radius * 2.0;
        if (pointInfo.focusStart < pointStart && pointInfo.focusEnd > pointEnd) {
            return true;
        }
        if (index == moveEndIndex_ && indicatorHeadOffset_ >= focusStretchMaxTime_ &&
            ((animationDirect_ > 0 && pointInfo.focusEnd >= pointEnd) ||
            (animationDirect_ < 0 && pointInfo.focusStart <= pointStart))) {
            return true;
        }
    }
    return false;
}

void RosenRenderSwiper::InitMoveRange()
{
    moveStartIndex_ = currentIndex_;
    moveEndIndex_ = targetIndex_;
    if (needReverse_) {
        moveStartIndex_ = itemCount_ - currentIndex_ - 1;
        moveEndIndex_ = itemCount_ - targetIndex_ - 1;
    }

    // 1. drag indicator 2.drag content zone
    if (isDragStart_) {
        moveEndIndex_ = moveStartIndex_ + animationDirect_;
    }
}

} // namespace OHOS::Ace
