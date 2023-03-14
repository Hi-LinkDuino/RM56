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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_PICKER_ANIMATION_CONTROLLER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_PICKER_ANIMATION_CONTROLLER_H

#include "core/animation/animator.h"
#include "core/components/display/render_display.h"
#include "core/components/picker/picker_animation.h"
#include "core/components/picker/render_picker_column.h"
#include "core/components/triangle/render_triangle.h"
#include "core/pipeline/pipeline_context.h"

namespace OHOS::Ace {

class PickerAnimationController : public virtual AceType {
    DECLARE_ACE_TYPE(PickerAnimationController, AceType);

public:
    PickerAnimationController() = default;
    ~PickerAnimationController() = default;

    void SetPipeline(const WeakPtr<PipelineContext>& value)
    {
        pipe_ = value;
    }

    void SetTriangle(const RefPtr<RenderTriangle>& value)
    {
        triangle_ = value;
    }

    void SetLunar(const RefPtr<RenderDisplay>& value)
    {
        lunar_ = value;
    }

    void SetButtons(const RefPtr<RenderDisplay>& value)
    {
        buttons_ = value;
    }

    void SetMonthDay(const RefPtr<RenderPickerColumn>& value)
    {
        monthDay_ = value;
    }

    void SetAmPm(const RefPtr<RenderPickerColumn>& value)
    {
        amPm_ = value;
    }

    void SetHour(const RefPtr<RenderPickerColumn>& value)
    {
        hour_ = value;
    }

    void SetMinute(const RefPtr<RenderPickerColumn>& value)
    {
        minute_ = value;
    }

    void SetSecond(const RefPtr<RenderPickerColumn>& value)
    {
        second_ = value;
    }

    void SetYear(const RefPtr<RenderPickerColumn>& value)
    {
        InitColumnOpacity(year_.Upgrade(), value);
        year_ = value;
    }

    void SetMonth(const RefPtr<RenderPickerColumn>& value)
    {
        InitColumnOpacity(month_.Upgrade(), value);
        month_ = value;
    }

    void SetDay(const RefPtr<RenderPickerColumn>& value)
    {
        InitColumnOpacity(day_.Upgrade(), value);
        day_ = value;
    }

    void SetOutStopCallback(const std::function<void()>& value)
    {
        outStopCallback_ = value;
    }

    void Play(bool isIn);

private:
    // create all type of animation
    void CreateAnimation();
    void CreateTriangleAnimation();
    void CreateMovingAnimation();
    void CreateInOpacityAnimation();
    void CreateOutOpacityAnimation();
    void InitMoveRange();
    void InitColumnOpacity(const RefPtr<RenderPickerColumn>& oldColumn, const RefPtr<RenderPickerColumn>& newColumn);
    // play or stop animation
    void PlayInAnimation();
    void PlayOutAnimation();
    void StopInAnimation();
    void StopOutAnimation();

    // pipeline context
    WeakPtr<PipelineContext> pipe_;
    // common of picker base
    WeakPtr<RenderTriangle> triangle_;
    WeakPtr<RenderDisplay> lunar_;
    WeakPtr<RenderDisplay> buttons_;
    // column of datetime
    WeakPtr<RenderPickerColumn> monthDay_;
    WeakPtr<RenderPickerColumn> amPm_;
    WeakPtr<RenderPickerColumn> hour_;
    WeakPtr<RenderPickerColumn> minute_;
    WeakPtr<RenderPickerColumn> second_;
    // column of date
    WeakPtr<RenderPickerColumn> year_;
    WeakPtr<RenderPickerColumn> month_;
    WeakPtr<RenderPickerColumn> day_;
    // move range of date
    double yearStart_ = 0.0;
    double yearEnd_ = 0.0;
    double monthStart_ = 0.0;
    double monthEnd_ = 0.0;
    double dayStart_ = 0.0;
    double dayEnd_ = 0.0;
    // triangle animation
    RefPtr<PickerAnimation> inTriangle_;
    RefPtr<PickerAnimation> outTriangle_;
    // moving animation
    RefPtr<PickerAnimation> inYearMoving_;
    RefPtr<PickerAnimation> inMonthMoving_;
    RefPtr<PickerAnimation> inDayMoving_;
    RefPtr<PickerAnimation> outYearMoving_;
    RefPtr<PickerAnimation> outMonthMoving_;
    RefPtr<PickerAnimation> outDayMoving_;
    // opacity animation
    RefPtr<PickerAnimation> inOldColumnOpacity_;
    RefPtr<PickerAnimation> inMonthDayOpacity_;
    RefPtr<PickerAnimation> inYearOpacity_;
    RefPtr<PickerAnimation> inLunarButtonsOpacity_;
    RefPtr<PickerAnimation> outOldColumnOpacity_;
    RefPtr<PickerAnimation> outMonthDayOpacity_;
    RefPtr<PickerAnimation> outYearOpacity_;
    RefPtr<PickerAnimation> outLunarButtonsOpacity_;
    // callback
    std::function<void()> outStopCallback_;
    // others
    bool created_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_PICKER_ANIMATION_CONTROLLER_H
