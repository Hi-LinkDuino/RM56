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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_RENDER_PICKER_COLUMN_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_RENDER_PICKER_COLUMN_H

#include "core/animation/animator.h"
#include "core/animation/curve_animation.h"
#include "core/common/vibrator/vibrator_proxy.h"
#include "core/components/clip/render_clip.h"
#include "core/components/display/render_display.h"
#include "core/components/flex/render_flex.h"
#include "core/components/picker/picker_column_component.h"
#include "core/components/picker/render_picker_option.h"
#include "core/components/scroll/render_scroll.h"
#include "core/gestures/drag_recognizer.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class RenderPickerColumn : public RenderNode {
    DECLARE_ACE_TYPE(RenderPickerColumn, RenderNode);

public:
    static RefPtr<RenderNode> Create();

    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;

    bool IsUseOnly() override
    {
        return true;
    }

    void UpdateOffset(double value)
    {
        xOffset_ = value;
        MarkNeedLayout();
    }

    double GetXOffset() const
    {
        return xOffset_;
    }

    void UpdateColumnOpacity(double value)
    {
        if (display_) {
            display_->UpdateOpacity(static_cast<uint8_t>(value));
        }
    }

    bool RotationTest(const RotationEvent& event) override;

    bool HandleScroll(bool isDown);
    bool HandleFinished(bool success);
    void HandleFocus(bool focus);

    std::string GetColumnTag() const;

    uint32_t GetWidthRatio() const;

    void FlushCurrentOptions();

    void UpdateRenders();

    void SetAdjustHeight(double value)
    {
        adjustHeight_ = value;
    }

    void OnPaintFinish() override
    {
        UpdateAccessibility();
    }

    const RefPtr<RenderFlex>& GetFlexColumn() const
    {
        return column_;
    }

    void UpdateToss(double y);

    void TossStoped();

    Offset GetInnerColumnGlobalOffset() const
    {
        if (!column_) {
            return Offset();
        }
        return column_->GetGlobalOffset();
    }

protected:
    void OnTouchTestHit(
        const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result) override;

private:
    void HandleDragStart(const TouchEventInfo& info);
    void HandleDragMove(const TouchEventInfo& info);
    void HandleDragEnd(const TouchEventInfo& info);
    void GetRenders(const RefPtr<RenderNode>& render);
    void GetRenders();
    void ClearRenders();
    void ScrollOption(double delta);

    void CreateAnimation();
    RefPtr<CurveAnimation<double>> CreateAnimation(double from, double to);
    void HandleCurveStopped();
    bool InnerHandleScroll(bool isDown);

    void LayoutSplitter();

    void UpdateAccessibility();

    bool NotLoopOptions() const;
    bool CanMove(bool isDown) const;

    void UpdatePositionY(double y);

    RefPtr<RenderClip> clip_;
    RefPtr<RenderFlex> column_;
    RefPtr<RenderDisplay> display_;
    RefPtr<RenderText> splitter_;
    std::vector<RefPtr<RenderPickerOption>> options_;

    RefPtr<PickerColumnComponent> data_;

    bool focused_ = false;
    double rotateAngle_ = 0.0;
    double rotateInterval_ = 0.0;

    bool nodeHandlerSetted_ = false;

    RefPtr<RawRecognizer> rawRecognizer_;
    RefPtr<VerticalDragRecognizer> dragRecognizer_;
    double xOffset_ = 0.0;
    double yOffset_ = 0.0;
    double yLast_ = 0.0;
    bool pressed_ = false;
    Dimension jumpInterval_;
    Dimension columnMargin_;

    double adjustHeight_ = 0.0;
    double scrollDelta_ = 0.0;
    bool animationCreated_ = false;
    RefPtr<Animator> toController_;
    RefPtr<Animator> fromController_;
    RefPtr<CurveAnimation<double>> toBottomCurve_;
    RefPtr<CurveAnimation<double>> toTopCurve_;
    RefPtr<CurveAnimation<double>> fromBottomCurve_;
    RefPtr<CurveAnimation<double>> fromTopCurve_;

    RefPtr<Vibrator> vibrator_;
    bool needVibrate_ = true;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_RENDER_PICKER_COLUMN_H
