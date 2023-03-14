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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_RENDER_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_RENDER_COMPONENT_H

#include "base/geometry/animatable_dimension.h"
#include "base/geometry/dimension_rect.h"
#include "base/memory/ace_type.h"
#include "core/components/common/layout/layout_param.h"
#include "core/components/common/layout/position_param.h"
#include "core/components/common/properties/motion_path_option.h"
#include "core/event/ace_event_handler.h"
#include "core/pipeline/base/component.h"

namespace OHOS::Ace {

class RenderNode;

// RenderComponent is visible Component which can create an RenderNode.
class RenderComponent : public Component {
    DECLARE_ACE_TYPE(RenderComponent, Component);

public:
    RenderComponent() = default;
    ~RenderComponent() override = default;

    bool IsTakeBoundary() const
    {
        return takeBoundary_;
    }

    void SetTakeBoundary(bool takeBoundary)
    {
        takeBoundary_ = takeBoundary;
    }

    void SetZIndex(int32_t zIndex)
    {
        zIndex_ = zIndex;
    }

    int32_t GetZIndex() const
    {
        return zIndex_;
    }

    void SetIsPercentSize(bool isPercentSize)
    {
        isPercentSize_ = isPercentSize;
    }

    bool GetIsPercentSize() const
    {
        return isPercentSize_;
    }

    const std::string& GetAccessibilityText() const
    {
        return accessibilityText_;
    }

    void SetAccessibilityText(const std::string& accessibilityText)
    {
        accessibilityText_ = accessibilityText;
    }

    virtual void SetLeft(const Dimension& left)
    {
        positionParam_.left.first = AnimatableDimension(left);
        positionParam_.left.second = true;
    }

    virtual void SetLeft(const AnimatableDimension& left)
    {
        positionParam_.left.first = left;
        positionParam_.left.second = true;
    }

    void SetAnchorX(const Dimension& anchorX)
    {
        positionParam_.anchor.first = anchorX;
    }

    virtual void SetRight(const Dimension& right)
    {
        positionParam_.right.first = AnimatableDimension(right);
        positionParam_.right.second = true;
    }

    virtual void SetRight(const AnimatableDimension& right)
    {
        positionParam_.right.first = right;
        positionParam_.right.second = true;
    }

    void SetAnchorY(const Dimension& anchorY)
    {
        positionParam_.anchor.second = anchorY;
    }

    virtual void SetTop(const Dimension& top)
    {
        positionParam_.top.first = AnimatableDimension(top);
        positionParam_.top.second = true;
    }

    virtual void SetTop(const AnimatableDimension& top)
    {
        positionParam_.top.first = top;
        positionParam_.top.second = true;
    }

    virtual void SetBottom(const Dimension& bottom)
    {
        positionParam_.bottom.first = bottom;
        positionParam_.bottom.second = true;
    }

    virtual void SetBottom(const AnimatableDimension& bottom)
    {
        positionParam_.bottom.first = AnimatableDimension(bottom);
        positionParam_.bottom.second = true;
    }

    void SetHasLeft(bool hasLeft)
    {
        positionParam_.left.second = hasLeft;
    }

    void SetHasRight(bool hasRight)
    {
        positionParam_.right.second = hasRight;
    }

    void SetHasTop(bool hasTop)
    {
        positionParam_.top.second = hasTop;
    }

    void SetHasBottom(bool hasBottom)
    {
        positionParam_.bottom.second = hasBottom;
    }

    virtual const Dimension& GetLeft() const
    {
        return positionParam_.left.first;
    }

    virtual const Dimension& GetRight() const
    {
        return positionParam_.right.first;
    }

    virtual const Dimension& GetTop() const
    {
        return positionParam_.top.first;
    }

    virtual const Dimension& GetBottom() const
    {
        return positionParam_.bottom.first;
    }

    virtual bool HasLeft() const
    {
        return positionParam_.left.second;
    }

    virtual bool HasRight() const
    {
        return positionParam_.right.second;
    }

    virtual bool HasTop() const
    {
        return positionParam_.top.second;
    }

    virtual bool HasBottom() const
    {
        return positionParam_.bottom.second;
    }

    PositionType GetPositionType() const
    {
        return positionParam_.type;
    }

    void SetPositionType(PositionType positionType)
    {
        positionParam_.type = positionType;
    }

    const PositionParam& GetPositionParam() const
    {
        return positionParam_;
    }

    double GetFlexWeight() const
    {
        return flexWeight_;
    }

    void SetFlexWeight(double flexWeight)
    {
        flexWeight_ = flexWeight;
    }

    int32_t GetDisplayIndex() const
    {
        return displayIndex_;
    }

    void SetDisplayIndex(int32_t displayIndex)
    {
        displayIndex_ = displayIndex;
    }

    MeasureType GetMeasureType() const
    {
        return measureType_;
    }

    void SetMeasureType(MeasureType measureType)
    {
        measureType_ = measureType;
    }

    bool IsCustomComponent() const
    {
        return isCustomComponent_;
    }

    void SetIsCustomComponent(bool isCustomComponent)
    {
        isCustomComponent_ = isCustomComponent;
    }

    const EventMarker& GetOnLayoutReadyMarker() const
    {
        return onLayoutReady_;
    }

    void SetOnLayoutReadyMarker(const EventMarker& onLayoutReady)
    {
        onLayoutReady_ = onLayoutReady;
    }

    bool IsIgnored() const
    {
        return isIgnored_;
    }

    void SetIsIgnored(bool ignore)
    {
        isIgnored_ = ignore;
    }

    bool InterceptEvent() const
    {
        return interceptEvent_;
    }

    void SetInterceptEvent(bool interceptEvent)
    {
        interceptEvent_ = interceptEvent;
    }

    const MotionPathOption& GetMotionPathOption() const
    {
        return motionPathOption_;
    }

    void SetMotionPathOption(const MotionPathOption& option)
    {
        motionPathOption_ = option;
    }

    virtual RefPtr<RenderNode> CreateRenderNode() = 0;

    bool IsResponseRegion() const
    {
        return isResponseRegion_;
    }

    void MarkResponseRegion(bool isResponseRegion)
    {
        isResponseRegion_ = isResponseRegion;
    }

    const std::vector<DimensionRect>& GetResponseRegion() const
    {
        return responseRegion_;
    }

    void SetResponseRegion(const std::vector<DimensionRect>& responseRegion)
    {
        responseRegion_ = responseRegion;
    }

protected:
    bool takeBoundary_ = true;
    std::string accessibilityText_;
    PositionParam positionParam_;
    double flexWeight_ = 0.0;
    int32_t displayIndex_ = 1;
    MeasureType measureType_ = MeasureType::PARENT;
    EventMarker onLayoutReady_;
    bool isIgnored_ = false;
    bool interceptEvent_ = false;

    bool isCustomComponent_ = false;
    bool isPercentSize_ = false;
    int32_t zIndex_ = 0;
    MotionPathOption motionPathOption_;
    bool isResponseRegion_ = false;
    std::vector<DimensionRect> responseRegion_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_RENDER_COMPONENT_H
