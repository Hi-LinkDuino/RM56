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

#include "core/components/rating/render_rating.h"

#include <cmath>

#include "base/log/log.h"
#include "base/utils/utils.h"
#include "core/components/image/render_image.h"
#include "core/components/rating/rating_component.h"
#include "core/event/ace_event_helper.h"

namespace OHOS::Ace {

void RenderRating::Update(const RefPtr<Component>& component)
{
    RefPtr<RatingComponent> rating = AceType::DynamicCast<RatingComponent>(component);
    if (rating) {
        Initialize();
        if (rating->GetStarNum() != starNum_) {
            hoverColorMap_.clear();
            starNum_ = rating->GetStarNum();
        }
        bool isUpdateAll = rating->GetUpdateType() == UpdateType::ALL;
        if (isUpdateAll) {
            ratingScore_ = rating->GetRatingScore();
        }
        if (rating->GetOnChange()) {
            onChangeRating = *rating->GetOnChange();
        }
        stepSize_ = rating->GetStepSize();
        paddingHorizontal_ = rating->GetPaddingHorizontal();
        paddingVertical_ = rating->GetPaddingVertical();
        defaultHeight_ = rating->GetDefaultHeight();

        foregroundSrc_ = rating->GetForegroundSrc();
        secondarySrc_ = rating->GetSecondarySrc();
        backgroundSrc_ = rating->GetBackgroundSrc();
        foregroundSrc1_ = rating->GetForegroundSrc();
        secondarySrc1_ = rating->GetSecondarySrc();
        backgroundSrc1_ = rating->GetBackgroundSrc();
        foregroundResourceId_ = rating->GetForegroundResourceId();
        secondaryResourceId_ = rating->GetSecondaryResourceId();
        backgroundResourceId_ = rating->GetBackgroundResourceId();

        focusBorderWidth_ = rating->GetFocusBorderWidth();
        focusBorderRadius_ = rating->GetFocusBorderRadius();
        hoverColor_ = rating->GetHoverColor();
        starColorActive_ = rating->GetStarColorActive().GetValue();
        starColorInactive_ = rating->GetStarColorInactive().GetValue();

        ProcessAttributes(isUpdateAll);

        designedAspectRatio_ = rating->GetDesignedStarAspectRatio() * starNum_;
        isIndicator_ = rating->GetIndicator();
        needReverse_ = (rating->GetTextDirection() == TextDirection::RTL);
        rtlFlip_ = rating->IsRtlFlip();
        onScoreChange_ = AceAsyncEvent<void(const std::string)>::Create(rating->GetChangeEventId(), context_);

        RefPtr<ImageComponent> foregroundImage = rating->GetForegroundImage();
        foregroundImage->SetResourceId(foregroundResourceId_);
        UpdateRenderImage(foregroundImage, ImageFit::FILL, foregroundSrc_, renderForeground_, starColorActive_);

        RefPtr<ImageComponent> secondaryImage = rating->GetSecondaryImage();
        secondaryImage->SetResourceId(secondaryResourceId_);
        UpdateRenderImage(secondaryImage, ImageFit::FILL, secondarySrc_, renderSecondary_, starColorInactive_);

        RefPtr<ImageComponent> backgroundImage = rating->GetBackgroundImage();
        backgroundImage->SetResourceId(backgroundResourceId_);
        UpdateRenderImage(backgroundImage, ImageFit::FILL, backgroundSrc_, renderBackground_, starColorInactive_);
    }
    InitAccessibilityEventListener();
    MarkNeedLayout();
}

void RenderRating::PerformLayout()
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    width_ = GetLayoutParam().GetMaxSize().Width();
    height_ = GetLayoutParam().GetMaxSize().Height();
    paddingHorizontal_ = Dimension(NormalizeToPx(paddingHorizontal_), DimensionUnit::PX);
    paddingVertical_ = Dimension(NormalizeToPx(paddingVertical_), DimensionUnit::PX);
    defaultHeight_ = Dimension(NormalizeToPx(defaultHeight_), DimensionUnit::PX);
    focusBorderWidth_ = Dimension(NormalizeToPx(focusBorderWidth_), DimensionUnit::PX);
    focusBorderRadius_ = Dimension(NormalizeToPx(focusBorderRadius_), DimensionUnit::PX);
    auto scale = context->GetViewScale();
    ACE_DCHECK(!NearZero(scale));
    AdaptLayoutSize(scale);
    SetLayoutSize(GetLayoutParam().GetMaxSize());
    CalculateRatingSize();
    singleWidth_ = ratingSize_.Width() / starNum_;
    if (singleWidth_ <= paddingHorizontal_.Value() * 2 && !NearZero(singleWidth_)) {
        paddingHorizontal_.SetValue(DEFAULT_RATING_HORIZONTAL_PADDING);
    }
    if (ratingSize_.Height() <= paddingVertical_.Value() * 2 && !NearZero(singleWidth_)) {
        paddingVertical_.SetValue(DEFAULT_RATING_VERTICAL_PADDING);
    }
    CalculateImageWidth();
    LayoutAllRenderImage();
}

void RenderRating::CalculateRatingSize()
{
    ratingSize_ = GetLayoutSize();
    if (IsTv()) {
        double realAspectRatio = Size::CalcRatio(GetLayoutSize());
        if (realAspectRatio > designedAspectRatio_) {
            ratingSize_.SetWidth(ratingSize_.Height() * designedAspectRatio_);
        } else {
            ratingSize_.SetHeight(ratingSize_.Width() / designedAspectRatio_);
        }
    }
}

void RenderRating::InitAccessibilityEventListener()
{
    auto accessibilityNode = accessibilityNode_.Upgrade();
    if (accessibilityNode) {
        accessibilityNode->SetScrollableState(true);
        accessibilityNode->SetActionScrollForward([wp = AceType::WeakClaim(this)]() {
            auto rating = wp.Upgrade();
            if (rating) {
                rating->HandleActionScroll(true);
                return true;
            }
            return false;
        });
        accessibilityNode->SetActionScrollBackward([wp = AceType::WeakClaim(this)]() {
            auto rating = wp.Upgrade();
            if (rating) {
                rating->HandleActionScroll(false);
                return true;
            }
            return false;
        });
        accessibilityNode->SetAccessibilityValue(drawScore_, 0.0, starNum_);
    }
}

void RenderRating::UpdateAccessibilityAttr()
{
    auto accessibilityNode = accessibilityNode_.Upgrade();
    if (accessibilityNode) {
        accessibilityNode->SetAccessibilityValue(drawScore_, 0.0, starNum_);
        auto context = context_.Upgrade();
        if (context) {
            AccessibilityEvent ratingEvent;
            ratingEvent.nodeId = accessibilityNode->GetNodeId();
            ratingEvent.eventType = "selected";
            ratingEvent.componentType = "rating";
            ratingEvent.currentItemIndex = drawScore_;
            ratingEvent.itemCount = starNum_;
            context->SendEventToAccessibility(ratingEvent);
        }
    }
}

void RenderRating::HandleActionScroll(bool isForward)
{
    if (isForward) {
        StarMovingForward();
    } else {
        StarMovingBackward();
    }
}

void RenderRating::CalculateImageWidth()
{
    ConstrainScore(drawScore_, 0.0, starNum_);
    double secondaryStar = (ceil(drawScore_) - drawScore_);
    secondaryImageWidth_ = secondaryStar * (singleWidth_ - 2 * paddingHorizontal_.Value()) +
                           paddingHorizontal_.Value() * ceil(secondaryStar);
    foregroundImageWidth_ = ceil(drawScore_) * singleWidth_ - secondaryImageWidth_;
    backgroundImageWidth_ = ratingSize_.Width() - foregroundImageWidth_ - secondaryImageWidth_;
}

void RenderRating::LayoutAllRenderImage()
{
    double imageVerticalOffset = ratingSize_.Width() * paddingVertical_.Value() / defaultHeight_.Value() / starNum_;
    double ratingHeight = ratingSize_.Height() - imageVerticalOffset * 2;

    paintForeground_ = !NearZero(foregroundImageWidth_);
    if (paintForeground_) {
        LayoutRenderImage(renderForeground_, Size(singleWidth_, ratingHeight));
    }

    paintSecondary_ = !NearZero(secondaryImageWidth_);
    if (paintSecondary_) {
        LayoutRenderImage(renderSecondary_, Size(singleWidth_, ratingHeight));
    }

    paintBackground_ = !NearZero(backgroundImageWidth_);
    if (paintBackground_) {
        LayoutRenderImage(renderBackground_, Size(singleWidth_, ratingHeight));
    }
}

void RenderRating::UpdateRatingBar()
{
    UpdateAccessibilityAttr();
    MarkNeedLayout();
}

void RenderRating::LayoutRenderImage(const RefPtr<RenderImage>& renderImage, const Size& imageComponentSize)
{
    LayoutParam imageLayoutParam;
    imageLayoutParam.SetFixedSize(imageComponentSize);
    renderImage->SetNeedLayout(true);
    renderImage->Layout(imageLayoutParam);
}

bool RenderRating::StarMovingBackward()
{
    bool movingOut =
        (!needReverse_ && NearEqual(drawScore_, stepSize_)) || (needReverse_ && NearEqual(drawScore_, starNum_));
    if (movingOut) {
        return false;
    }
    drawScore_ = needReverse_ ? (drawScore_ + stepSize_) : (drawScore_ - stepSize_);
    ConstrainScore(drawScore_, stepSize_, starNum_);
    ratingScore_ = drawScore_;
    UpdateAccessibilityAttr();
    MarkNeedLayout();
    return true;
}

bool RenderRating::StarMovingForward()
{
    bool movingOut =
        (!needReverse_ && NearEqual(drawScore_, starNum_)) || (needReverse_ && NearEqual(drawScore_, stepSize_));
    if (movingOut) {
        return false;
    }
    drawScore_ = needReverse_ ? (drawScore_ - stepSize_) : (drawScore_ + stepSize_);
    ConstrainScore(drawScore_, stepSize_, starNum_);
    ratingScore_ = drawScore_;
    UpdateAccessibilityAttr();
    MarkNeedLayout();
    return true;
}

void RenderRating::RequestFocusAnimation(const Offset& animationOffset, const Size& animationSize, double borderRadius)
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("Pipeline context upgrade fail!");
        return;
    }
    focusAnimation_ = context->GetRenderFocusAnimation();
    if (!focusAnimation_) {
        LOGE("focusAnimation is null!");
        return;
    }
    context->ShowFocusAnimation(RRect::MakeRRect(Rect(Offset(), animationSize), borderRadius, borderRadius),
        Color::FromRGB(255, 255, 255), animationOffset);
}

bool RenderRating::HandleFocusEvent(const KeyEvent& keyEvent)
{
    if (isIndicator_) {
        return false;
    }
    auto context = context_.Upgrade();
    ACE_DCHECK(context);
    operationEvent_ = OperationEvent::RATING_KEY_EVENT;
    switch (keyEvent.code) {
        case KeyCode::TV_CONTROL_LEFT:
            return StarMovingBackward();
        case KeyCode::TV_CONTROL_RIGHT:
            return StarMovingForward();
        case KeyCode::TV_CONTROL_ENTER:
        case KeyCode::TV_CONTROL_CENTER:
        case KeyCode::KEY_NUMPAD_ENTER:
            // fire current score via change event when ENTER key is clicked, whether the score changes or not
            FireChangeEvent();
            return true;
        default:
            return false;
    }
}

void RenderRating::UpdateRenderImage(const RefPtr<ImageComponent>& imageComponent, const ImageFit& imageFit,
    const std::string& imageSrc, RefPtr<RenderImage>& renderImage, uint32_t svgColor)
{
    imageComponent->SetImageFit(imageFit);
    if (!imageSrc.empty()) {
        imageComponent->SetResourceId(InternalResource::ResourceId::NO_ID);
        // Handle situation of resource changes from internal to outer.
        imageComponent->SetColor(Color::TRANSPARENT);
    } else if (sourceErrorColor_ != Color::TRANSPARENT) {
        imageComponent->SetImageFill(sourceErrorColor_); // use [sourceErrorColor_] for all stars to show source error
    } else {
        // this color only takes effects when using internal svg resources
        imageComponent->SetImageFill(std::make_optional<Color>(svgColor));
    }
    imageComponent->SetSrc(imageSrc);
    imageComponent->SetFitMaxSize(true);
    if (needReverse_ && rtlFlip_) {
        imageComponent->SetTextDirection(TextDirection::RTL);
        imageComponent->SetMatchTextDirection(true);
    }

    renderImage = AceType::DynamicCast<RenderImage>(imageComponent->CreateRenderNode());
    renderImage->Attach(GetContext());
    renderImage->SetDirectPaint(true);
    renderImage->Update(imageComponent);

    AddChild(renderImage);
}

void RenderRating::OnTouchTestHit(
    const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    dragDetector_->SetCoordinateOffset(coordinateOffset);
    clickDetector_->SetCoordinateOffset(coordinateOffset);

    result.emplace_back(dragDetector_);
    result.emplace_back(clickDetector_);
}

void RenderRating::HandleClick(const Offset& clickPosition)
{
    HandleTouchEvent(clickPosition);
    FireChangeEvent();
    RequestFocusAnimationForPhone();
}

void RenderRating::HandleDragUpdate(const Offset& updatePoint)
{
    HandleTouchEvent(updatePoint);
}

void RenderRating::HandleDragEnd()
{
    FireChangeEvent();
    RequestFocusAnimationForPhone();
}

void RenderRating::HandleTouchEvent(const Offset& updatePoint)
{
    if (isIndicator_ || NearZero(singleWidth_)) {
        return;
    }

    double wholeStarNum = 0.0;
    double posInSingle = 0.0;
    operationEvent_ = OperationEvent::RATING_TOUCH_EVENT;
    if (needReverse_) {
        double ratingWidth = GetLayoutSize().Width();
        wholeStarNum = floor((ratingWidth - updatePoint.GetX()) / singleWidth_);
        posInSingle = ratingWidth - updatePoint.GetX() - wholeStarNum * singleWidth_;
    } else {
        wholeStarNum = floor(updatePoint.GetX() / singleWidth_);
        posInSingle = updatePoint.GetX() - wholeStarNum * singleWidth_;
    }
    if (posInSingle < paddingHorizontal_.Value()) {
        ratingScore_ = wholeStarNum;
    } else if (posInSingle >= paddingHorizontal_.Value() && posInSingle <= singleWidth_ - paddingHorizontal_.Value()) {
        ratingScore_ =
            wholeStarNum + (posInSingle - paddingHorizontal_.Value()) / (singleWidth_ - paddingHorizontal_.Value() * 2);
    } else if (posInSingle > singleWidth_ - paddingHorizontal_.Value()) {
        ratingScore_ = wholeStarNum + 1;
    }
    ratingScore_ = (ratingScore_ > starNum_) ? starNum_ : ratingScore_;
    ratingScore_ = (ratingScore_ > stepSize_) ? ratingScore_ : stepSize_;

    double newScore = ceil(ratingScore_ / stepSize_) * stepSize_;
    newScore = (newScore > starNum_) ? starNum_ : newScore;
    if (NearEqual(newScore, drawScore_)) {
        return;
    }
    drawScore_ = newScore;
    ConstrainScore(drawScore_, stepSize_, starNum_);
    UpdateRatingBar();
}

void RenderRating::Initialize()
{
    dragDetector_ = AceType::MakeRefPtr<HorizontalDragRecognizer>();
    dragDetector_->SetOnDragUpdate([weak = WeakClaim(this)](const DragUpdateInfo& info) {
        auto rating = weak.Upgrade();
        if (rating) {
            rating->HandleDragUpdate(info.GetLocalLocation());
        }
    });
    dragDetector_->SetOnDragEnd([weak = WeakClaim(this)](const DragEndInfo& info) {
        auto rating = weak.Upgrade();
        if (rating) {
            rating->HandleDragEnd();
        }
    });
    clickDetector_ = AceType::MakeRefPtr<ClickRecognizer>();
    clickDetector_->SetOnClick([weak = WeakClaim(this)](const ClickInfo& info) {
        auto rating = weak.Upgrade();
        if (rating) {
            rating->HandleClick(info.GetLocalLocation());
        }
    });
}

void RenderRating::FireChangeEvent()
{
    if (onScoreChange_) {
        std::string param = std::string(R"("change",{"rating":)").append(std::to_string(drawScore_).append("}"));
        onScoreChange_(param);
    }
    if (onChangeRating) {
        onChangeRating(static_cast<double>(drawScore_));
    }
}

void RenderRating::AdaptLayoutSize(double scale)
{
    auto rawImageSize = renderForeground_->Measure();
    picWidth_ = rawImageSize.Width();
    if (width_ > 0.0 && height_ > 0.0) {
        return;
    }
    if (!rawImageSize.IsValid()) {
        return;
    }
    double ratio = Size::CalcRatio(rawImageSize);
    if (height_ > 0.0) {
        width_ = height_ * ratio * starNum_;
        return;
    }
    if (width_ > 0.0) {
        height_ = width_ / starNum_ / ratio;
        return;
    }
    width_ = starNum_ * picWidth_ / scale;
    height_ = renderForeground_->GetLayoutSize().Height();
}

void RenderRating::ProcessAttributes(bool isUpdateAll)
{
    if (starNum_ <= 0) {
        starNum_ = DEFAULT_RATING_STAR_NUM;
    }
    if (IsTv()) {
        stepSize_ = 1.0;
    } else {
        if (stepSize_ <= 0.0) {
            stepSize_ = DEFAULT_RATING_STEP_SIZE;
        } else if (stepSize_ > starNum_) {
            stepSize_ = starNum_;
        }
    }
    bool scoreExceedsStarNum = false;
    if (ratingScore_ < 0.0) {
        ratingScore_ = DEFAULT_RATING_SCORE;
    } else if (ratingScore_ > starNum_) {
        ratingScore_ = starNum_;
        scoreExceedsStarNum = true;
    }
    if (paddingHorizontal_.Value() < 0.0) {
        paddingHorizontal_.SetValue(DEFAULT_RATING_HORIZONTAL_PADDING);
    }
    if (paddingVertical_.Value() < 0.0) {
        paddingVertical_.SetValue(DEFAULT_RATING_VERTICAL_PADDING);
    }
    bool existEmptySrc = foregroundSrc_.empty() || secondarySrc_.empty() || backgroundSrc_.empty();
    bool isAllSrcEmpty = foregroundSrc_.empty() && secondarySrc_.empty() && backgroundSrc_.empty();
    if (existEmptySrc && !isAllSrcEmpty) {
        foregroundSrc_.clear();
        secondarySrc_.clear();
        backgroundSrc_.clear();
        sourceErrorColor_ = Color(starColorInactive_);
    }
    if (isUpdateAll) {
        if (scoreExceedsStarNum) {
            drawScore_ = starNum_;
            return;
        }
        drawScore_ = Round(ratingScore_ / stepSize_) * stepSize_;
    }
}

void RenderRating::ConstrainScore(double& score, double lowerBoundary, double upperBoundary)
{
    if (GreatOrEqual(score, upperBoundary)) {
        score = upperBoundary;
    }
    if (LessOrEqual(score, lowerBoundary)) {
        score = lowerBoundary;
    }
}

void RenderRating::OnMouseHoverEnterTest()
{
    operationEvent_ = OperationEvent::RATING_MOUSE_EVENT;
    if (focusAnimation_) {
        focusAnimation_->CancelFocusAnimation();
    }
    onHover_ = true;
}

void RenderRating::OnMouseHoverExitTest()
{
    operationEvent_ = OperationEvent::RATING_TOUCH_EVENT;
    if (onHover_) {
        PlayEventEffectAnimation(0, true);
        onHover_ = false;
    }
}

bool RenderRating::MouseHoverTest(const Point& parentLocalPoint)
{
    auto context = context_.Upgrade();
    if (!context) {
        return false;
    }
    if (NearZero(singleWidth_)) {
        LOGW("mouseHoverTest failed, singleWidth is zero");
        return false;
    }
    if (!InTouchRectList(parentLocalPoint, GetTouchRectList())) {
        OnMouseHoverExitTest();
        return false;
    }
    auto localPoint = parentLocalPoint - GetPaintRect().GetOffset();
    double offsetDeltaX = (GetLayoutSize().Width() - ratingSize_.Width()) / 2.0;
    int32_t starIndex = (int32_t)floor((localPoint.GetX() - offsetDeltaX) / singleWidth_);
    double hoverOffsetX = starIndex * singleWidth_ + offsetDelta_.GetX();
    if (!onHover_ || !NearEqual(hoverOffset_.GetX(), hoverOffsetX)) {
        hoverOffset_ = Offset(hoverOffsetX, offsetDelta_.GetY());
        PlayEventEffectAnimation(starIndex);
    }
    OnMouseHoverEnterTest();
    context->AddToHoverList(AceType::WeakClaim(this).Upgrade());
    return true;
}

Offset RenderRating::GetStarOffset(double imageVerticalOffset)
{
    double focusScore = NearEqual(drawScore_, floor(drawScore_)) ? drawScore_ - 1 : floor(drawScore_);
    focusScore = needReverse_ ? starNum_ - ceil(drawScore_) : focusScore;
    ConstrainScore(focusScore, 0.0, starNum_ - 1);
    // total offset of focus border is twice the width of [focusBorderWidth_], border-width + padding-width
    return Offset(singleWidth_ * focusScore + imageVerticalOffset, imageVerticalOffset);
}

void RenderRating::RequestFocusAnimationForPhone()
{
    double imageVerticalOffset = ratingSize_.Width() * paddingVertical_.Value() / defaultHeight_.Value() / starNum_;
    double borderWidth = focusBorderWidth_.Value() * 2;
    double focusWidth = singleWidth_ - borderWidth * 2;
    double focusHeight = ratingSize_.Height() - borderWidth * 2;
    RequestFocusAnimation(GetStarOffset(imageVerticalOffset) + Offset(borderWidth, borderWidth) + GetGlobalOffset(),
        Size(focusWidth, focusHeight), EPSILON);
}

void RenderRating::PlayEventEffectAnimation(int32_t starIndex, bool isHoverExists)
{
    if (!eventEffectController_) {
        eventEffectController_ = AceType::MakeRefPtr<Animator>(context_);
    }
    if (!eventEffectController_->IsStopped()) {
        eventEffectController_->Stop();
    }
    eventEffectController_->ClearInterpolators();
    eventEffectController_->ClearStopListeners();
    eventEffectController_->SetDuration(HOVER_DURATION);
    eventEffectController_->SetFillMode(FillMode::FORWARDS);
    if (hoverColorMap_.find(starIndex) == hoverColorMap_.end()) {
        hoverColorMap_.emplace(starIndex, Color::TRANSPARENT);
    }
    for (auto& item : hoverColorMap_) {
        if (item.first == starIndex && !isHoverExists) {
            CreateColorAnimation(item.second, hoverColor_, item.first, isHoverExists);
        } else {
            CreateColorAnimation(item.second, Color::TRANSPARENT, item.first, isHoverExists);
        }
    }
    eventEffectController_->Forward();
}

void RenderRating::CreateColorAnimation(const Color& from, const Color& to, int32_t starIndex, bool isHoverExists)
{
    if (from == to) {
        return;
    }
    auto animation = AceType::MakeRefPtr<KeyframeAnimation<Color>>();
    auto colorFrameStart = AceType::MakeRefPtr<Keyframe<Color>>(0.0f, from);
    auto colorFrameEnd = AceType::MakeRefPtr<Keyframe<Color>>(1.0f, to);
    if (isHoverExists && from.GetValue() < hoverColor_.GetValue()) {
        colorFrameEnd->SetCurve(Curves::FAST_OUT_SLOW_IN);
    } else {
        colorFrameEnd->SetCurve(Curves::FRICTION);
    }
    animation->AddKeyframe(colorFrameStart);
    animation->AddKeyframe(colorFrameEnd);
    animation->AddListener([weakNode = AceType::WeakClaim(this), starIndex](const Color& value) {
        auto node = weakNode.Upgrade();
        if (node) {
            node->hoverColorMap_[starIndex] = value;
            node->MarkNeedRender();
        }
    });
    eventEffectController_->AddInterpolator(animation);
}

} // namespace OHOS::Ace
