/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SLIDER_RENDER_SLIDER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SLIDER_RENDER_SLIDER_H

#include <functional>

#include "core/animation/animator.h"
#include "core/animation/curve_animation.h"
#include "core/components/slider/block_component.h"
#include "core/components/slider/slider_component.h"
#include "core/components/text/render_text.h"
#include "core/components/text/text_component.h"
#include "core/gestures/click_recognizer.h"
#include "core/gestures/drag_recognizer.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

constexpr double DEFAULT_VALUE = 0.0;
constexpr double DEFAULT_MAX = 1.0;
constexpr double DEFAULT_MIN = 0.0;
constexpr double DEFAULT_STEP = 1.0;
const Dimension SLIDER_PADDING_DP = 13.5_vp;

enum class SliderEvent {
    MOVE_START = 0,
    MOVE_MOVING = 1,
    MOVE_END = 2,
    CLICK = 3,
    ACCESSIBILITY = 4,
    FOCUS = 5,
};

using TouchRegionPoint = Offset;
using Vertex = Offset;

// The region layout is shown below, and only needs two point can settle the rectangle.
//   (0,0)------------(10,0)
//        |          |
//        |    .(5,5)|
//        |          |
//  (0,10)------------(10,10)
class TouchRegion {
public:
    TouchRegion() = default;
    TouchRegion(TouchRegionPoint topLeftPoint, TouchRegionPoint bottomRightPoint)
        : bottomRightPoint_(bottomRightPoint), topLeftPoint_(topLeftPoint)
    {}
    ~TouchRegion() = default;

    bool ContainsInRegion(double x, double y)
    {
        return LessOrEqual(topLeftPoint_.GetX(), x) && LessOrEqual(topLeftPoint_.GetY(), y) &&
               GreatOrEqual(bottomRightPoint_.GetX(), x) && GreatOrEqual(bottomRightPoint_.GetY(), y);
    }

private:
    TouchRegionPoint bottomRightPoint_;
    TouchRegionPoint topLeftPoint_;
};

// The render node of slider component.
class RenderSlider : public RenderNode {
    DECLARE_ACE_TYPE(RenderSlider, RenderNode);

public:
    RenderSlider();
    ~RenderSlider() override = default;

    static RefPtr<RenderNode> Create();

    void Update(const RefPtr<Component>& component) override;

    void PerformLayout() override;

    void OnPaintFinish() override;

    bool HandleFocusEvent(const KeyEvent& keyEvent);

    double GetValue() const
    {
        return value_;
    }

    double GetMax() const
    {
        return max_;
    }

    double GetMin() const
    {
        return min_;
    }

    double GetStep() const
    {
        return step_;
    }

    SliderMode GetMode() const
    {
        return mode_;
    }

    Axis GetDirection() const
    {
        return direction_;
    }

    bool GetShowSteps() const
    {
        return showSteps_;
    }

    bool GetShowTips() const
    {
        return showTips_;
    }

    bool GetIsReverse() const
    {
        return isReverse_;
    }

    std::function<void(const std::string&)> GetOnMovedEndId() const
    {
        return onMoveEnd_;
    }

    bool GetErrorBit() const
    {
        return isError_;
    }

    bool GetFocus() const
    {
        return isFocus_;
    }

    const WeakPtr<SliderComponent>& GetSliderComponent() const
    {
        return sliderComponent_;
    }

    void SetFocus(bool isFocus)
    {
        isFocus_ = isFocus;
        MarkNeedLayout();
    }

    void SyncValueToComponent(double value)
    {
        value_ = value;
        auto slider = sliderComponent_.Upgrade();
        if (slider) {
            slider->SetCurrentValue(value);
        }
    }

    void SetTotalRatio(double ratio)
    {
        if (ratio > 1.0) {
            totalRatio_ = 1.0;
        } else {
            totalRatio_ = ratio;
        }
        if (showTips_) {
            UpdateTipText(totalRatio_);
        }
    }

    bool NeedSmoothMoving() const
    {
        return mode_ == SliderMode::INSET && GreatNotEqual(step_, DEFAULT_STEP);
    }

    double GetThickness()
    {
        return thickness_;
    }
    
protected:
    static TouchRegionPoint GetTopTouchRegion(const Vertex& center, double width, double height);
    static TouchRegionPoint GetBotTouchRegion(const Vertex& center, double width, double height);

    Size Measure();
    void HandleDragStart(const Offset& startPoint);
    void HandleDragUpdate(const Offset& startPoint);
    void HandleDragEnd();
    void OnTouchTestHit(
        const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result) override;
    bool MouseHoverTest(const Point& parentLocalPoint) override;
    void OnMouseHoverEnterTest() override {}
    void OnMouseHoverExitTest() override {}

    void SetOnChange(const std::function<void(double,int)>& value)
    {
        onChange_ = value;
    }

    void HandleClick(const Offset& clickPosition);

    void FireMoveEndEvent();
    void FireMovingEvent(SliderEvent mode);
    void UpdateAnimation();
    void StartMoveAnimation(double from, double to, bool isClick = false);
    void RestartMoveAnimation(double value, bool isClick = false);
    void ResetMoveAnimation(double from, double to);
    void CalculateTotalRadio();
    void UpdateTipText(double value);

    void Initialize();
    void RenderBlockPosition(const Offset& touchPosition);
    void UpdateBlockPosition(const Offset& touchPosition, bool isAnimation);
    Vertex FindCenterVertex(double x, double y, double objectHeight, double objectWidth);
    void UpdateTouchRegion();
    void UpdateAccessibilityAttr();
    void InitAccessibilityEventListener();
    void HandleScrollUpdate(double delta);

    bool renderWholeNode_ = true;

    // Gesture event
    RefPtr<DragRecognizer> dragDetector_;
    RefPtr<ClickRecognizer> clickDetector_;
    TouchRegion blockTouchRegion_;
    bool insideBlockRegion_ = false;
    bool blockActive_ = false;

    // The size constrain is too small set this bit to show text
    bool isError_ = false;
    bool isValueError_ = false;

    // bar length
    double trackLength_ = 1.0;
    double totalRatio_ = 0.0;
    double cachedRatio_ = 0.0;

    // the circle block radius scale
    double radiusScale_ = 1.0;
    double step_ = DEFAULT_STEP;
    double max_ = DEFAULT_MAX;
    double min_ = DEFAULT_MIN;

    double scaleValue_ = 1.0;
    double thickness_ = 0.0;

    bool showSteps_ = false;
    bool showTips_ = false;
    bool isDraging_ = false;
    bool isReverse_ = false;
    SliderMode mode_ = SliderMode::OUTSET;
    Dimension blockHotWidth_;
    Dimension blockHotHeight_;
    Dimension hotWidth_;
    Axis direction_ = Axis::HORIZONTAL;

    RefPtr<RenderNode> tip_;
    RefPtr<RenderNode> renderText_;
    RefPtr<TextComponent> tipText_;
    WeakPtr<SliderComponent> sliderComponent_;

    std::function<void(double,int)> onChange_;

private:
    // Slider render information
    double value_ = DEFAULT_VALUE;
    double preMovingValue_ = DEFAULT_VALUE;
    std::function<void(const std::string&)> onMoveEnd_;
    std::function<void(const std::string&)> onMoving_;

    // focus information
    bool isFocus_ = false;
    bool disable_ = false;

    double animationEnd_ = 0.0;

    // animation attr
    RefPtr<Animator> controller_;
    RefPtr<CurveAnimation<double>> translate_;
    RefPtr<Animator> moveController_;
    RefPtr<CurveAnimation<double>> moveAnimation_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SLIDER_RENDER_SLIDER_H
