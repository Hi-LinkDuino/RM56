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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_PROGRESS_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_PROGRESS_H

#include <vector>

#include "core/components/progress/bubble_progress_component.h"
#include "core/components/progress/loading_progress_component.h"
#include "core/components/progress/progress_component.h"
#include "frameworks/bridge/common/dom/dom_node.h"

namespace OHOS::Ace::Framework {

class DOMProgress final : public DOMNode {
    DECLARE_ACE_TYPE(DOMProgress, DOMNode);

public:
    DOMProgress(NodeId nodeId, const std::string& nodeName);
    ~DOMProgress() override = default;

    static RefPtr<ProgressComponent> CreateProgressComponent(
        double min, double percent, double cachedValue, double max, ProgressType type);

    RefPtr<Component> GetSpecializedComponent() override;

protected:
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;
    void OnSetStyleFinished() override;
    void PrepareSpecializedComponent() override;

private:
    void InitProgressIfNeed();

    double cachedValue_ = 0.0;
    double min_ = 0.0;
    double max_ = 100.0;
    double percent_ = 0.0;

    bool clockwiseDirection_ = true;
    bool showAnimationEffect_ = true;

    // All the magic numbers below are followed UX requirement.
    std::pair<Color, bool> color_ { Color(), false };

    Gradient gradient_;
    std::pair<Color, bool> cachedColor_ { Color(), false };
    std::pair<Color, bool> backgroundColor_ { Color(), false };
    std::pair<Dimension, bool> strokeWidth_ = { 32.0_px, false };
    std::pair<Dimension, bool> scaleWidth_ = { 2.0_vp, false };
    std::pair<int32_t, bool> scaleNumber_ = { 120, false };

    std::pair<double, bool> radius_ = { -1.0, false };
    std::pair<double, bool> centerX_ = { -1.0, false };
    std::pair<double, bool> centerY_ = { -1.0, false };

    double startDegree_ = -120.0;
    double sweepDegree_ = 240.0;
    bool isStartToEnd_ = true;

    Dimension diameter_;
    Dimension bubbleRadius_;

    std::vector<Color> colors_;
    std::vector<double> weights_;

    ProgressType type_ = ProgressType::LINEAR;

    RefPtr<ProgressComponent> progressChild_;
    RefPtr<BubbleProgressComponent> bubbleProgressChild_;
    RefPtr<LoadingProgressComponent> loadingProgressChild_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_PROGRESS_H
