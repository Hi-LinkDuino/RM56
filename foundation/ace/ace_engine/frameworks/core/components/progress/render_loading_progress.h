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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PROGRESS_RENDER_LOADING_PROGRESS_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PROGRESS_RENDER_LOADING_PROGRESS_H

#include <vector>

#include "core/animation/animator.h"
#include "core/animation/picture_animation.h"
#include "core/components/progress/loading_progress_component.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

constexpr int32_t TOTAL_FRAME_NUMBER = 60;
constexpr int32_t MODE_DRAG = 0;
constexpr int32_t MODE_LOOP = 1;
constexpr int32_t MODE_EXIT = 2;

class RenderLoadingProgress : public RenderNode {
    DECLARE_ACE_TYPE(RenderLoadingProgress, RenderNode);

public:
    RenderLoadingProgress();
    ~RenderLoadingProgress() override = default;

    void Update(const RefPtr<Component>& component) override;

    void SetDiameter(const Dimension& diameter)
    {
        // Refresh will set new diameter when window size changed.
        if (!NearEqual(NormalizeToPx(diameter), diameter_)) {
            diameterDimension_ = diameter;
            ringController_ = nullptr;
            cometController_ = nullptr;
            usedScale_ = 0.0;
            SetNeedLayout(true);
        }
    }

    int32_t GetLoadingMode() const
    {
        return loadingMode_;
    }

    Color GetProgressColor() const
    {
        return progressColor_;
    }

    void SetLoadingMode(int32_t mode);

    void SetDragRange(double minDistance, double maxDistance);

    void SetDragDistance(double distance);

    void PerformLayout() override;

    static RefPtr<RenderNode> Create();

protected:
    void CalculateCirclePosition();
    void OnVisibleChanged() override;
    void OnHiddenChanged(bool hidden) override;
    void AnimationChanged();

    void UpdateLoadingSize(double diameter);
    void CalculateValue(int32_t start, int32_t end, double percent = 1.0);
    void UpdateRingAnimation();
    void UpdateCometAnimation();
    void DoCometTailAnimation();
    void DoCometLoopAnimation();
    void UpdateCometParams();
    float GetCometScaleByDegree(double degree);
    float GetCometAlphaByDegree(double degree);

    Offset center_;
    double diameter_ = 0.0;
    Dimension diameterDimension_;
    Dimension ringRadiusDimension_;
    Dimension orbitRadiusDimension_;
    RefPtr<Animator> ringController_;
    RefPtr<Animator> cometController_;

    // Prop for refresh.
    int32_t loadingMode_ = MODE_LOOP;
    double minDistance_ = 0.0;
    double maxDistance_ = 0.0;
    double curDistance_ = 0.0;
    double dragScale_ = 1.0; // Scale effect for ring
    double dragAlpha_ = 1.0; // Alpha effect for ring and comet
    double exitScale_ = 1.0; // Scale effect for ring and comet when exit from refresh.
    double exitAlpha_ = 1.0; // Alpha effect for ring and comet when exit from refresh.

    double scale_ = 0.0;
    double usedScale_ = 0.0;
    double moveRatio_ = MOVE_RATIO;
    double ringWidth_ = 0.0;
    double ringRadius_ = 0.0;
    Offset ringOffset_;
    struct CometParam {
        uint8_t alpha = UINT8_MAX;
        double scale = 1.0;
        double angular = 0.0;
    };
    std::vector<CometParam> cometParams_;
    double orbitRadius_ = 0.0;
    double cometRadius_ = 0.0;
    uint8_t cometAlpha_ = UINT8_MAX;
    int32_t cometCount_ = COMET_COUNT;
    double cometCurTail_ = 0.0;
    double cometTailLen_ = COMET_TAIL_LEN;
    BaseId::IdType moveStopId_ = 0;
    BaseId::IdType tailStopId_ = 0;
    Color progressColor_ = PROGRESS_COLOR; // When Dark: Color(0xffffffff)

    // Use when Dark/night Mode.
    double ringBgWidth_ = 0.0;
    double ringBlurRadius_ = 0.0;
    double ringBgBlurRadius_ = 0.0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PROGRESS_RENDER_LOADING_PROGRESS_H
