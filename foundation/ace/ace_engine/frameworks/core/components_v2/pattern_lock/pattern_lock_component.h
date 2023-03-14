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
#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_PATTERN_LOCK_PATTERN_LOCK_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_PATTERN_LOCK_PATTERN_LOCK_COMPONENT_H
#include "base/geometry/dimension.h"
#include "base/log/log.h"
#include "core/components/common/properties/color.h"
#include "core/components/common/properties/edge.h"
#include "core/components_v2/pattern_lock/pattern_lock_controller.h"
#include "core/pipeline/base/render_component.h"
#include "frameworks/core/components/common/properties/svg_paint_state.h"

namespace OHOS::Ace::V2 {
constexpr int32_t DEFAULT_SIDE_LENGTH = 300;
constexpr int32_t DEFAULT_CIRCLE_RADIUS = 14;
constexpr int32_t DEFAULT_STROKE_WIDTH = 34;

class PatternCompleteEvent : public BaseEventInfo {
    DECLARE_RELATIONSHIP_OF_CLASSES(PatternCompleteEvent, BaseEventInfo);

public:
    explicit PatternCompleteEvent(std::vector<int> input) : BaseEventInfo("patternCompleteEvent"), input_(input) {}
    ~PatternCompleteEvent() = default;
    std::vector<int> GetInput() const
    {
        return input_;
    }

private:
    std::vector<int> input_;
};
class ACE_EXPORT PatternLockComponent : public RenderComponent {
    DECLARE_ACE_TYPE(PatternLockComponent, RenderComponent);

public:
    PatternLockComponent();
    ~PatternLockComponent() override = default;
    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;
    void SetCircleRadius(const Dimension& circleRadius);
    void SetSideLength(const Dimension& sideLength);
    void SetRegularColor(const Color& regularColor);
    void SetSelectedColor(const Color& selectedColor);
    void SetActiveColor(const Color& regularColor);
    void SetPathColor(const Color& regularColor);
    void SetStrokeWidth(const Dimension& lineWidth);
    void SetAutoReset(bool isAutoReset);
    const Dimension& GetCircleRadius() const;
    const Dimension& GetSideLength() const;
    const Dimension& GetStrokeWidth() const;
    const Color& GetRegularColor() const;
    const Color& GetSelectedColor() const;
    const Color& GetActiveColor() const;
    const Color& GetPathColor() const;
    bool GetAutoReset() const;
    void SetPatternCompleteEvent(const EventMarker& event)
    {
        patternCompleteEvent_ = event;
    }
    const EventMarker& GetPatternCompleteEvent() const
    {
        return patternCompleteEvent_;
    }
    RefPtr<V2::PatternLockController> GetPatternLockController() const;

private:
    Dimension circleRadius_ { DEFAULT_CIRCLE_RADIUS, DimensionUnit::VP };
    Dimension sideLength_ { DEFAULT_SIDE_LENGTH, DimensionUnit::VP };
    Color regularColor_;
    Color selectedColor_;
    Color pathColor_;
    Color activeColor_;
    EventMarker patternCompleteEvent_;
    RefPtr<V2::PatternLockController> patternLockController_;
    Dimension strokeWidth_ { DEFAULT_STROKE_WIDTH, DimensionUnit::VP };
    bool autoReset_ = true;
};
} // namespace OHOS::Ace::V2
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_PATTERN_LOCK_PATTERN_LOCK_COMPONENT_H