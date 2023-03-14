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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_RENDER_SIDE_BAR_ANIMATION_CONTROLLER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_RENDER_SIDE_BAR_ANIMATION_CONTROLLER_H

#include "base/memory/ace_type.h"
#include "core/animation/animator.h"
#include "core/animation/curve_animation.h"
#include "core/components/side_bar/side_bar_container_component.h"
#include "core/pipeline/base/render_node.h"
#include "core/pipeline/pipeline_context.h"

namespace OHOS::Ace {
using AnimationCallback = std::function<void(double)>;
using StopAnimationCallback = std::function<void()>;

class SideBarAnimation : public AceType {
    DECLARE_ACE_TYPE(SideBarAnimation, AceType);

public:
    SideBarAnimation(const WeakPtr<PipelineContext>& context, double start, double end, int delay,
        int duration, const RefPtr<Curve>& curve, AnimationCallback&& callback);
    ~SideBarAnimation() override = default;

    void Play();
    void Stop();

    void AddStopListenerCallback(const StopAnimationCallback& callBack)
    {
        if (controller_) {
            controller_->AddStopListener(callBack);
        }
    }

    void ClearStopListenerCallback()
    {
        if (controller_) {
            controller_->ClearStopListeners();
        }
    }

private:
    double start_ = 0.0;
    double end_ = 0.0;
    int delay_ = 0;
    int duration_ = 0;
    WeakPtr<PipelineContext> context_;
    RefPtr<Curve> curve_;
    AnimationCallback callback_;
    RefPtr<CurveAnimation<double>> animation_;
    RefPtr<Animator> controller_;
};

class SideBarAnimationController : public AceType {
    DECLARE_ACE_TYPE(SideBarAnimationController, AceType);

public:
    explicit SideBarAnimationController(const WeakPtr<PipelineContext>& context) :
             context_(context) {}
    ~SideBarAnimationController() override = default;

    void SetAnimationStopCallback(StopAnimationCallback&& callback)
    {
        stopCallback_ = std::move(callback);
    }

    void SetRenderSideBarContainer(const WeakPtr<RenderNode>& node)
    {
        sidebar_ = node;
    }

    const WeakPtr<RenderNode>& GetRenderSideBarContainer() const
    {
        return sidebar_;
    }

    void StopAnimation();

    void PlaySideBarContainerToAnimation(SideStatus status);

private:
    void CreateAniamtion();
    void PlayHideAnimation();
    void StopHideAnimation();
    void PlayShowAnimation();
    void StopShowAnimation();

    RefPtr<SideBarAnimation> leftToRightAnimation_;
    RefPtr<SideBarAnimation> rightToLeftAnimation_;

    WeakPtr<PipelineContext> context_;
    StopAnimationCallback stopCallback_;
    bool isAnimationCreated_ = false;
    WeakPtr<RenderNode> sidebar_;
};

}

#endif