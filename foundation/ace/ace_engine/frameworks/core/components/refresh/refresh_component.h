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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_REFRESH_REFRESH_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_REFRESH_REFRESH_COMPONENT_H

#include "base/geometry/dimension.h"
#include "core/components_v2/common/common_def.h"
#include "core/pipeline/base/sole_child_component.h"

namespace OHOS::Ace {

inline constexpr double DEFAULT_INDICATOR_OFFSET = 16.0;
inline constexpr int32_t MIN_FRICTION_RATIO = 0;
inline constexpr int32_t MAX_FRICTION_RATIO = 100;
#ifdef WEARABLE_PRODUCT
inline constexpr int32_t DEFAULT_FRICTION_RATIO = 62;
#else
inline constexpr int32_t DEFAULT_FRICTION_RATIO = 42;
#endif

class ACE_EXPORT RefreshComponent : public SoleChildComponent {
    DECLARE_ACE_TYPE(RefreshComponent, SoleChildComponent);

public:
    RefreshComponent() : SoleChildComponent() {}

    ~RefreshComponent() override = default;

    RefPtr<Element> CreateElement() override;

    RefPtr<RenderNode> CreateRenderNode() override;

    const Dimension& GetIndicatorOffset() const
    {
        return indicatorOffset_;
    }

    ACE_DEFINE_COMPONENT_EVENT(OnStateChange, void(int));

    ACE_DEFINE_COMPONENT_EVENT(OnRefreshing, void());

    void SetIndicatorOffset(const Dimension& indicatorOffset)
    {
        if (GreatOrEqual(indicatorOffset.Value(), 0.0)) {
            indicatorOffset_ = indicatorOffset;
        } else {
            LOGD("indicatorOffset value is invalid");
        }
    }

    bool IsRefreshing() const
    {
        return refreshing_;
    }

    void IsRefresh(bool isRefresh)
    {
        isRefresh_ = isRefresh;
    }

    bool GetIsRefresh() const
    {
        return isRefresh_;
    }

    void SetRefreshing(bool isRefreshing)
    {
        refreshing_ = isRefreshing;
    }

    bool IsUseOffset() const
    {
        return useOffset_;
    }

    void SetUseOffset(bool isUseOffset)
    {
        useOffset_ = isUseOffset;
    }

    RefreshType GetRefreshType() const
    {
        return refreshType_;
    }

    void SetRefreshType(RefreshType refreshType)
    {
        refreshType_ = refreshType;
    }

    bool IsShowLastTime() const
    {
        return showLastTime_;
    }

    void SetShowLastTime(bool isShowLastTime)
    {
        showLastTime_ = isShowLastTime;
    }

    void SetRefreshEventId(const EventMarker& refreshEventId)
    {
        refreshEventId_ = refreshEventId;
    }

    void SetTimeOffset(const Dimension& timeOffset)
    {
        timeOffset_ = timeOffset;
    }

    const Dimension& GetTimeOffset() const
    {
        return timeOffset_;
    }

    const EventMarker& GetRefreshEventId() const
    {
        return refreshEventId_;
    }

    void SetPulldownEventId(const EventMarker& pulldownEventId)
    {
        pullDownEventId_ = pulldownEventId;
    }

    const EventMarker& GetPulldownEventId() const
    {
        return pullDownEventId_;
    }

    const Color& GetBackgroundColor() const
    {
        return backgroundColor_;
    }

    void SetBackgroundColor(const Color& backgroundColor)
    {
        backgroundColor_ = backgroundColor;
    }

    const Color& GetProgressColor() const
    {
        return progressColor_;
    }

    void SetProgressColor(const Color& progressColor)
    {
        progressColor_ = progressColor;
    }

    const Dimension& GetLoadingDistance() const
    {
        return loadingDistance_;
    }

    void SetLoadingDistance(const Dimension& loadingDistance)
    {
        loadingDistance_ = loadingDistance;
    }

    const Dimension& GetRefreshDistance() const
    {
        return refreshDistance_;
    }

    void SetRefreshDistance(const Dimension& refreshDistance)
    {
        if (GreatOrEqual(refreshDistance.Value(), 0.0)) {
            refreshDistance_ = refreshDistance;
        } else {
            refreshDistance_ = Dimension(0.0, DimensionUnit::VP);
        }

    }

    const Dimension& GetProgressDistance() const
    {
        return progressDistance_;
    }

    void SetProgressDistance(const Dimension& progressDistance)
    {
        progressDistance_ = progressDistance;
    }

    const Dimension& GetMaxDistance() const
    {
        return maxDistance_;
    }

    void SetMaxDistance(const Dimension& maxDistance)
    {
        maxDistance_ = maxDistance;
    }

    const Dimension& GetProgressDiameter() const
    {
        return progressDiameter_;
    }

    void SetProgressDiameter(const Dimension& progressDiameter)
    {
        progressDiameter_ = progressDiameter;
    }

    const Dimension& GetShowTimeDistance() const
    {
        return showTimeDistance_;
    }

    void SetShowTimeDistance(const Dimension& showTimeDistance)
    {
        showTimeDistance_ = showTimeDistance;
    }

    void SetFriction(int32_t friction)
    {
        if (friction >= MIN_FRICTION_RATIO && friction <= MAX_FRICTION_RATIO) {
            friction_ = friction;
        } else if (friction < MIN_FRICTION_RATIO) {
            friction_ = MIN_FRICTION_RATIO;
        } else {
            friction_ = MAX_FRICTION_RATIO;
        }
    }

    int32_t GetFriction() const
    {
        return friction_;
    }

    const TextStyle& GetTextStyle() const
    {
        return textStyle_;
    }

    void SetTextStyle(const TextStyle& textStyle)
    {
        textStyle_ = textStyle;
    }

private:
    Dimension indicatorOffset_ = Dimension(DEFAULT_INDICATOR_OFFSET, DimensionUnit::VP);
    Dimension loadingDistance_;
    Dimension progressDistance_;
    Dimension showTimeDistance_;
    Dimension refreshDistance_;
    Dimension maxDistance_;
    Dimension progressDiameter_;
    Dimension timeOffset_;

    bool useOffset_ = false;
    bool showLastTime_ = false;
    bool refreshing_ = false;
    RefreshType refreshType_ = RefreshType::AUTO;
    bool isRefresh_ = false;

    int32_t friction_ = DEFAULT_FRICTION_RATIO;
    TextStyle textStyle_;

#ifdef WEARABLE_PRODUCT
    Color backgroundColor_ = Color::BLACK;
    Color progressColor_ = Color::WHITE;
#else
    Color backgroundColor_ = Color::WHITE;
    Color progressColor_ = Color::BLACK;
#endif
    EventMarker refreshEventId_;
    EventMarker pullDownEventId_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_REFRESH_REFRESH_COMPONENT_H
