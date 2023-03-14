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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SLIDER_FLUTTER_RENDER_SLIDER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SLIDER_FLUTTER_RENDER_SLIDER_H

#include "core/animation/animator.h"
#include "core/animation/keyframe_animation.h"
#include "core/components/slider/block_component.h"
#include "core/components/slider/render_block.h"
#include "core/components/slider/render_slider.h"
#include "core/components/track/render_track.h"
#include "frameworks/core/pipeline/layers/transform_layer.h"

namespace OHOS::Ace {

class FlutterRenderSlider final : public RenderSlider {
    DECLARE_ACE_TYPE(FlutterRenderSlider, RenderSlider);

public:
    FlutterRenderSlider() = default;
    ~FlutterRenderSlider() override = default;

    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;
    void Paint(RenderContext& context, const Offset& offset) override;
    void OnMouseHoverEnterTest() override;
    void OnMouseHoverExitTest() override;
    void OnPaintFinish() override;
    RenderLayer GetRenderLayer() override;

    bool IsRepaintBoundary() const override
    {
        return true;
    }

    RefPtr<RenderBlock> GetRenderBlock()
    {
        return AceType::DynamicCast<RenderBlock>(block_);
    }

    RefPtr<RenderTrack> GetRenderTrack()
    {
        return AceType::DynamicCast<RenderTrack>(track_);
    }

private:
    void AddTipChild();
    void HandleFocus();
    void ProcessBlock(const Offset& currentPosition);
    void ProcessTrack(const Offset& currentPosition);
    void SetTipPosition(double blockOffset);
    void StartHoverAnimation(RefPtr<Animator> controller, RefPtr<KeyframeAnimation<Color>>& colorAnimation);
    void ResetController(RefPtr<Animator>& controller);
    void CreateColorAnimation(
        RefPtr<KeyframeAnimation<Color>>& colorAnimation, const Color& beginValue, const Color& endValue, bool hover);

    RefPtr<RenderNode> block_ = AceType::MakeRefPtr<RenderBlock>();
    RefPtr<RenderNode> track_ = AceType::MakeRefPtr<RenderTrack>();
    RefPtr<Animator> controllerEnter_;
    RefPtr<Animator> controllerExit_;
    RefPtr<KeyframeAnimation<Color>> colorAnimationEnter_;
    RefPtr<KeyframeAnimation<Color>> colorAnimationExit_;
    RefPtr<Flutter::TransformLayer> layer_;
    bool initialUpdate_ = true;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SLIDER_FLUTTER_RENDER_SLIDER_H
