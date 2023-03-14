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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DATA_PANEL_RENDER_DATA_PANEL_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DATA_PANEL_RENDER_DATA_PANEL_H

#include <chrono>

#include "core/animation/animator.h"
#include "core/animation/curve_animation.h"
#include "core/components/data_panel/data_panel_component.h"
#include "core/pipeline/base/component.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

struct ArcData {
    Offset center;
    double progress = 0.0;
    double radius = 0.0;
    double thickness = 0.0;
    double maxValue = 0.0;
    Color startColor;
    Color endColor;

    double wholeAngle = 360.0;
    double startAngle = 0.0;
};

class RenderDataPanel : public RenderNode {
    DECLARE_ACE_TYPE(RenderDataPanel, RenderNode);

public:
    ~RenderDataPanel() override = default;
    void Update(const RefPtr<Component>& component) override;
    static RefPtr<RenderNode> Create();
    virtual void PlayAnimation() = 0;
    virtual void StopAnimation() = 0;
    bool GetCloseEffect() const
    {
        return !useEffect_;
    }

protected:
    RenderDataPanel();

    const Size Measure();
    void PerformLayout() override;
    virtual void PrepareAnimation() = 0;
    void OnVisibleChanged() override;
    void OnHiddenChanged(bool hidden) override;
    void AnimationChanged();

    MeasureType measureType_ = MeasureType::PARENT;
    ChartType type_ = ChartType::LINE;
    Dimension thickness_;
    Color backgroundTrack_ = Color::FromString("#08000000");
    bool autoScale_ = false;

    RefPtr<Animator> animator_;
    RefPtr<Animator> progressTransitionController_;
    double rotateAngle_ = 0.0;
    double sweepDegree_ = 0.0;
    double percent_ = 0.0;
    bool useEffect_ = false;
    bool animationInitialized_ = false;
    bool isUserSetPlay_ = false;
    double userAnimationDuration_ = -1.0;

    double previousPercentValue_ = 0.0;
    double percentChange_ = 0.0;
    std::chrono::steady_clock::time_point previousUpdateTime_ = std::chrono::steady_clock::now();
    std::chrono::duration<double> animationDuring_;

    bool needReplayAnimation_ = false;

private:
    // data panel default height and width
    Dimension height_;
    Dimension width_;
};

class RenderProgressDataPanel : public RenderDataPanel {
    DECLARE_ACE_TYPE(RenderProgressDataPanel, RenderDataPanel);

public:
    static RefPtr<RenderNode> Create();
    void Update(const RefPtr<Component>& component) override;

    const Color& GetStartColor() const
    {
        return startColor_;
    }

    const Color& GetEndColor() const
    {
        return endColor_;
    }

    bool IsRepaintBoundary() const override
    {
        return true;
    }

    void PlayAnimation() override
    {
        if (animator_) {
            animator_->Play();
            isUserSetPlay_ = true;
        }

        if (!isLoading_ && progressTransitionController_) {
            progressTransitionController_->Play();
            isUserSetPlay_ = true;
        }
    }

    void StopAnimation() override
    {
        if (animator_) {
            animator_->Stop();
            isUserSetPlay_ = false;
        }

        if (!isLoading_ && progressTransitionController_) {
            progressTransitionController_->Stop();
            isUserSetPlay_ = false;
        }
    }

protected:
    void PrepareAnimation() override;
    double GetProgress() const
    {
        return progress_;
    }

private:
    Color startColor_;
    Color endColor_;
    double progress_ = 0.0;
    bool isLoading_ = false;

    RefPtr<CurveAnimation<double>> animation_;
    RefPtr<CurveAnimation<double>> transitionAnimation_;
};

class RenderPercentageDataPanel : public RenderDataPanel {
    DECLARE_ACE_TYPE(RenderPercentageDataPanel, RenderDataPanel);

public:
    static RefPtr<RenderNode> Create();
    void Update(const RefPtr<Component>& component) override;

    void PlayAnimation() override
    {
        if (animator_) {
            animator_->Play();
            isUserSetPlay_ = true;
        }
    }

    void StopAnimation() override
    {
        if (animator_) {
            animator_->Pause();
            isUserSetPlay_ = false;
        }
    }

    const std::vector<Segment>& GetSegments() const
    {
        return segments_;
    }

    double GetMaxValue() const
    {
        return maxValue_;
    }

    ChartType GetDataPanelType() const
    {
        return panelType_;
    }

protected:
    void PrepareAnimation() override;

    double GetTotalValue() const
    {
        return totalValue_;
    }

    double GetStartDegree() const
    {
        return startDegree_;
    }

    double GetSweepDegree() const
    {
        return sweepDegree_;
    }

    double animationPercent_ = 0.0;

private:
    ChartType panelType_ = ChartType::PROGRESS;
    double startDegree_ = 0.0;
    double sweepDegree_ = 360.0;
    std::vector<Segment> segments_;
    double totalValue_ = 0.0;
    double maxValue_ = 0.0;
    RefPtr<CurveAnimation<double>> animation_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DATA_PANEL_RENDER_DATA_PANEL_H
