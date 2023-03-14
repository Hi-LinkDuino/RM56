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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_WATCH_SLIDER_WATCH_SLIDER_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_WATCH_SLIDER_WATCH_SLIDER_COMPONENT_H

#include <string>

#include "core/components/common/properties/color.h"
#include "core/components/common/rotation/rotation_controller.h"
#include "core/event/ace_event_handler.h"
#include "core/pipeline/base/element.h"
#include "core/pipeline/base/render_component.h"

namespace OHOS::Ace {

class WatchSliderComponent : public RenderComponent {
    DECLARE_ACE_TYPE(WatchSliderComponent, RenderComponent);

public:
    WatchSliderComponent() : rotationController_(AceType::MakeRefPtr<RotationController>()) {}
    ~WatchSliderComponent() override = default;

    RefPtr<Element> CreateElement() override;
    RefPtr<RenderNode> CreateRenderNode() override;

    double GetMinValue() const
    {
        return min_;
    }

    void SetMinValue(double min)
    {
        min_ = min;
    }

    double GetMaxValue() const
    {
        return max_;
    }

    void SetMaxValue(double min)
    {
        max_ = min;
    }

    double GetStep() const
    {
        return step_;
    }

    void SetStep(double step)
    {
        step_ = step;
    }

    double GetValue() const
    {
        return value_;
    }

    void SetValue(double value)
    {
        value_ = value;
    }

    const EventMarker& GetOnMoveEndEventId() const
    {
        return onMoveEndEventId_;
    }

    void SetOnMoveEndEventId(const EventMarker& methodId)
    {
        onMoveEndEventId_ = methodId;
    }

    const EventMarker& GetOnMovingEventId() const
    {
        return onMovingEventId_;
    }

    void SetOnMovingEventId(const EventMarker& methodId)
    {
        onMovingEventId_ = methodId;
    }

    const std::string& GetMinIconUrl() const
    {
        return minIconUrl_;
    }

    void SetMinIconUrl(const std::string& url)
    {
        minIconUrl_ = url;
    }

    const std::string& GetMaxIconUrl() const
    {
        return maxIconUrl_;
    }

    void SetMaxIconUrl(const std::string& url)
    {
        maxIconUrl_ = url;
    }

    const Color& GetBackgroundColor() const
    {
        return color_;
    }

    void SetBackgroundColor(const Color& color)
    {
        color_ = color;
    }

    const Color& GetSelectColor() const
    {
        return selectColor_;
    }

    void SetSelectColor(const Color& color)
    {
        selectColor_ = color;
    }

    bool GetContinuous() const
    {
        return isContinuous_;
    }

    void SetContinuous(bool flag)
    {
        isContinuous_ = flag;
    }

    const RefPtr<RotationController>& GetRotationController() const
    {
        return rotationController_;
    }

    void SetDisable(bool disable)
    {
        isDisable_ = disable;
    }

    bool GetDisable() const
    {
        return isDisable_;
    }

private:
    double max_ = 100.0;
    double min_ = 0.0;
    double step_ = 1.0;
    double value_ = 100.0;
    bool isDisable_ = false;
    std::string minIconUrl_;
    std::string maxIconUrl_;
    Color color_;
    Color selectColor_;
    EventMarker onMoveEndEventId_;
    EventMarker onMovingEventId_;
    bool isContinuous_ = true;

    RefPtr<RotationController> rotationController_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_WATCH_SLIDER_WATCH_SLIDER_COMPONENT_H
