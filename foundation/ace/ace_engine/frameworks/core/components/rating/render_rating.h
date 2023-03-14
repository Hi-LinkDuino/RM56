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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_RATING_RENDER_RATING_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_RATING_RENDER_RATING_H

#include "base/utils/system_properties.h"
#include "core/components/common/layout/constants.h"
#include "core/components/focus_animation/render_focus_animation.h"
#include "core/components/image/image_component.h"
#include "core/components/image/render_image.h"
#include "core/gestures/click_recognizer.h"
#include "core/gestures/drag_recognizer.h"
#include "core/pipeline/base/constants.h"
#include "core/pipeline/base/render_node.h"

class SkCanvas;

namespace OHOS::Ace {
namespace {

constexpr double EPSILON = 0.000001;

enum class OperationEvent {
    RATING_TOUCH_EVENT = 0,
    RATING_KEY_EVENT,
    RATING_MOUSE_EVENT,
};

} // namespace

class RenderRating : public RenderNode {
    DECLARE_ACE_TYPE(RenderRating, RenderNode);

public:
    static RefPtr<RenderNode> Create();
    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;

    // focus event
    void SetFocusEventFlag(bool isFocus)
    {
        operationEvent_ = isFocus ? OperationEvent::RATING_KEY_EVENT : OperationEvent::RATING_TOUCH_EVENT;
    }
    const Rect& GetAnimationRect() const
    {
        return animationRect_;
    }
    bool GetIndicator() const
    {
        return isIndicator_;
    }
    double GetRatingScore() const
    {
        return ratingScore_;
    }
    int32_t GetStarNum() const
    {
        return starNum_;
    }
    double GetStepSize() const
    {
        return stepSize_;
    }
    std::string GetForegroundSrc() const
    {
        return foregroundSrc1_;
    }
    std::string GetSecondarySrc() const
    {
        return secondarySrc1_;
    }
    std::string GetBackgroundSrc() const
    {
        return backgroundSrc1_;
    }
    bool HandleFocusEvent(const KeyEvent& keyEvent);
    bool IsTv() const
    {
        return SystemProperties::GetDeviceType() == DeviceType::TV;
    }
    bool IsPhone() const
    {
        return SystemProperties::GetDeviceType() == DeviceType::PHONE;
    }
    void CalculateRatingSize();
    void FireChangeEvent();
    void SetFocusAnimation(const RefPtr<RenderFocusAnimation>& focusAnimation)
    {
        focusAnimation_ = focusAnimation;
    }
    const RefPtr<RenderFocusAnimation>& GetFocusAnimation() const
    {
        return focusAnimation_;
    }
    void RequestFocusAnimation(const Offset& animationOffset, const Size& animationSize, double borderRadius);

    Offset GetStarOffset(double imageVerticalOffset);

    void RequestFocusAnimationForPhone();

protected:
    struct ImageAreaProperties final {
        ImageAreaProperties(
            double repeatNum, const Size& imageAreaSize, const Offset& drawOffset, const Offset& clipOffset)
            : repeatNum(repeatNum), imageAreaSize(imageAreaSize), drawOffset(drawOffset), clipOffset(clipOffset)
        {}
        ~ImageAreaProperties() = default;

        double repeatNum = 0.0;
        Size imageAreaSize;
        Offset drawOffset;
        Offset clipOffset;
    };
    void AdaptLayoutSize(double scale);
    void HandleTouchEvent(const Offset& updatePoint);
    void InitAccessibilityEventListener();
    void UpdateAccessibilityAttr();
    void HandleActionScroll(bool isForward);
    bool StarMovingForward();
    bool StarMovingBackward();
    void OnMouseHoverEnterTest() override;
    void OnMouseHoverExitTest() override;
    bool MouseHoverTest(const Point& parentLocalPoint) override;
    virtual void PaintFocus(
        const Offset& offset, double rrectRadius, const Size& boardSize, RenderContext& context) {}
    virtual void PaintFocus(
        const Offset& offset, double rrectRadius, const Size& boardSize, SkCanvas* canvas) {}
    static void ConstrainScore(double& score, double lowerBoundary, double upperBoundary);

    void SetRatingEvent(OperationEvent operationEvent)
    {
        operationEvent_ = operationEvent;
    }

    OperationEvent GetRatingEvent() const
    {
        return operationEvent_;
    }
    std::function<void(double)> onChangeRating;
    void SetOnChange(const std::function<void(double)>& value)
    {
        onChangeRating = value;
    }

    int32_t starNum_ = DEFAULT_RATING_STAR_NUM;
    double ratingScore_ = DEFAULT_RATING_SCORE;
    double stepSize_ = DEFAULT_RATING_STEP_SIZE;
    double width_ = DEFAULT_RATING_WIDTH;
    double height_ = DEFAULT_RATING_HEIGHT;
    double designedAspectRatio_ = 1.0;
    std::string foregroundSrc_;
    std::string secondarySrc_;
    std::string backgroundSrc_;
    std::string foregroundSrc1_;
    std::string secondarySrc1_;
    std::string backgroundSrc1_;
    InternalResource::ResourceId foregroundResourceId_ = InternalResource::ResourceId::RATE_STAR_SMALL_ON_SVG;
    InternalResource::ResourceId secondaryResourceId_ = InternalResource::ResourceId::RATE_STAR_SMALL_OFF_SVG;
    InternalResource::ResourceId backgroundResourceId_ = InternalResource::ResourceId::RATE_STAR_SMALL_OFF_SVG;
    std::function<void(const std::string&)> onScoreChange_;
    bool isIndicator_ = false;
    bool needReverse_ = false;
    bool rtlFlip_ = true;
    Size ratingSize_;
    Offset offsetDelta_;
    Color sourceErrorColor_ = Color::TRANSPARENT; // [sourceErrorColor_] is applied when not all three sources are set

    // properties for phone platform
    Dimension focusBorderWidth_;
    Dimension focusBorderRadius_;
    bool onHover_ = false;
    Color hoverColor_ = Color::TRANSPARENT;
    uint32_t starColorActive_ = 0xFFF7CE00;
    uint32_t starColorInactive_ = 0x19000000;
    Offset hoverOffset_;

    // RenderImage
    void UpdateRenderImage(const RefPtr<ImageComponent>& imageComponent, const ImageFit& imageFit,
        const std::string& imageSrc, RefPtr<RenderImage>& renderImage, uint32_t svgColor);
    void LayoutRenderImage(const RefPtr<RenderImage>& renderImage, const Size& imageComponentSize);
    void LayoutAllRenderImage();
    void UpdateRatingBar();
    void ProcessAttributes(bool isUpdateAll);
    void CalculateImageWidth();

    double singleWidth_ = 0.0;
    double picWidth_ = 0.0;
    double drawScore_ = DEFAULT_RATING_SCORE;
    double foregroundImageWidth_ = 0.0;
    double secondaryImageWidth_ = 0.0;
    double backgroundImageWidth_ = 0.0;
    Dimension paddingHorizontal_;
    Dimension paddingVertical_;
    Dimension defaultHeight_;
    RefPtr<RenderImage> renderForeground_;
    RefPtr<RenderImage> renderSecondary_;
    RefPtr<RenderImage> renderBackground_;
    bool paintForeground_ = true;
    bool paintSecondary_ = true;
    bool paintBackground_ = true;

    // Gesture event
    void Initialize();
    void HandleDragUpdate(const Offset& startPoint);
    void HandleDragEnd();
    void OnTouchTestHit(
        const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result) override;
    void HandleClick(const Offset& clickPosition);
    void CreateColorAnimation(const Color& from, const Color& to, int32_t starIndex, bool isHoverExists);
    void PlayEventEffectAnimation(int32_t starIndex, bool isHoverExists = false);

    RefPtr<DragRecognizer> dragDetector_;
    RefPtr<ClickRecognizer> clickDetector_;

    Rect animationRect_;

    RefPtr<RenderFocusAnimation> focusAnimation_;
    OperationEvent operationEvent_ { OperationEvent::RATING_TOUCH_EVENT };
    RefPtr<Animator> eventEffectController_;
    std::unordered_map<int32_t, Color> hoverColorMap_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_RATING_RENDER_RATING_H
