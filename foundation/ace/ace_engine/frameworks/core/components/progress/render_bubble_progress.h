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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PROGRESS_RENDER_BUBBLE_PROGRESS_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PROGRESS_RENDER_BUBBLE_PROGRESS_H

#include <vector>
#include <unordered_map>
#include <array>

#include "core/animation/animator.h"
#include "core/animation/flush_event.h"
#include "core/animation/picture_animation.h"
#include "core/components/progress/bubble_progress_component.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class RenderBubbleProgress : public RenderNode, public FlushEvent {
    DECLARE_ACE_TYPE(RenderBubbleProgress, RenderNode, FlushEvent);

public:
    RenderBubbleProgress();
    ~RenderBubbleProgress() override = default;

    void OnPostFlush() override;

    void Update(const RefPtr<Component>& component) override;

    void SetDiameter(double diameter)
    {
        diameter_ = diameter;
    }

    void PerformLayout() override;

    static RefPtr<RenderNode> Create();

    void OnAppShow() override;
    void OnAppHide() override;

protected:
    void CalculateCirclePosition();
    void OnVisibleChanged() override;
    void OnHiddenChanged(bool hidden) override;
    void AnimationChanged();

    double diameter_ = 0.0;

    using Vertex = Offset;
    Vertex center_;
    double radius_ = 0.0;
    double maxCircleRadius_ = 0.0;
    std::vector<Vertex> subCircleCenter_;

    // frame animation
    int32_t step_ = 0;
    int32_t progress_ = 0;
    Color lightToDark_;
    Color darkToLight_;
    RefPtr<PictureAnimation<int32_t>> pictureFrames_;
    RefPtr<Animator> animatorController_;

    bool simulationPrepared_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PROGRESS_RENDER_BUBBLE_PROGRESS_H
