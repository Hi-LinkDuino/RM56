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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PROGRESS_RENDER_PROGRESS_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PROGRESS_RENDER_PROGRESS_H

#include "core/components/progress/progress_component.h"
#include "core/pipeline/base/component.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

constexpr double PROGRESS_DEFAULT_VALUE = 0.0;
constexpr double PROGRESS_DEFAULT_MAX = 1.0;
constexpr double PROGRESS_DEFAULT_MIN = 0.0;
constexpr double PROGRESS_DEFAULT_CACHED_VALUE = 0.0;

class RenderProgress : public RenderNode {
    DECLARE_ACE_TYPE(RenderProgress, RenderNode);

public:
    static RefPtr<RenderNode> Create();
    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;
    void OnPaintFinish() override;

    bool GetErrorBit() const
    {
        return isError_;
    }
    double GetMaxValue() const
    {
        return max_;
    }
    double GetMinValue() const
    {
        return min_;
    }
    double GetValue() const
    {
        return value_;
    }
    ProgressType GetProgressType() const
    {
        return type_;
    }
    RefPtr<ProgressComponent> GetProgressComponent() const
    {
        return component_;
    }

protected:
    Size Measure();
    // TRACK length
    double totalRatio_ = 0.0;
    double cachedRatio_ = 0.0;
    double trackThickness_ = 0.0;

    // vp to logical pixel
    double scale_ = 0.0;
    ProgressType type_ = ProgressType::LINEAR;

private:
    void UpdateAccessibilityAttr();

    RefPtr<ProgressComponent> component_;
    double value_ = PROGRESS_DEFAULT_VALUE;
    double max_ = PROGRESS_DEFAULT_MAX;
    double min_ = PROGRESS_DEFAULT_MIN;
    double cachedValue_ = PROGRESS_DEFAULT_CACHED_VALUE;

    bool isError_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PROGRESS_RENDER_PROGRESS_H
