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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DISPLAY_RENDER_DISPLAY_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DISPLAY_RENDER_DISPLAY_H

#include "core/animation/animator.h"
#include "core/components/display/display_component.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class RenderDisplay : public RenderNode {
    DECLARE_ACE_TYPE(RenderDisplay, RenderNode);

public:
    static RefPtr<RenderNode> Create();
    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;

    void Dump() override;

    void OnAttachContext() override
    {
        animatableOpacity_.SetContextAndCallback(
            context_, std::bind(&RenderDisplay::OnOpacityAnimationCallback, this));
    }

    void UpdateVisibleType(VisibleType type)
    {
        if (visible_ != type) {
            visible_ = type;
            SetVisible(visible_ == VisibleType::VISIBLE);
            MarkNeedLayout();
        }
    }

    void UpdateHidden(bool hidden)
    {
        if (GetHidden() != hidden) {
            SetSelfHidden(hidden);
            MarkNeedLayout();
        }
    }

    void UpdateOpacity()
    {
        auto display = displayComponent_.Upgrade();
        if (display) {
            display->SetOpacity(transitionOpacity_);
        }
    }

    void UpdateOpacity(uint8_t opacity) override
    {
        if (disableLayer_) {
            for (auto& callback : opacityCallbacks_) {
                callback(opacity);
            }
            return;
        }
        if (opacity_ != opacity) {
            opacity_ = opacity;
            MarkNeedRender();
        }
    }

    double GetTransitionOpacity() const
    {
        return transitionOpacity_;
    }

    VisibleType GetVisibleType()
    {
        return visible_;
    }

    void GetOpacityCallbacks();

    bool GetVisible() const override;

    void OnStatusStyleChanged(VisualState style) override;

    void OnTransition(TransitionType type, int32_t id) override;

    bool HasDisappearingTransition(int32_t nodeId) override;

    RefPtr<StateAttributes<DisplayStateAttribute>> GetStateAttributes()
    {
        if (stateAttributeList_ == nullptr) {
            stateAttributeList_ = MakeRefPtr<StateAttributes<DisplayStateAttribute>>();
        }
        return stateAttributeList_;
    }


    bool HasStateAttributes()
    {
        return stateAttributeList_ != nullptr;
    }

protected:
    void OnOpacityAnimationCallback();
    void OnOpacityDisappearingCallback();
    void ClearRenderObject() override;
    int32_t GetCardAppearingDuration();
    void CreateAppearingAnimation(uint8_t opacity, int32_t limit);
    void ResetAppearingAnimation();

    int32_t duration_ = 0;
    double transitionOpacity_ = 0.0;
    VisibleType visible_ = VisibleType::VISIBLE;
    AnimatableDouble animatableOpacity_ = AnimatableDouble(1.0);
    double appearingOpacity_ = 0.0;
    double disappearingOpacity_ = 0.0;
    bool hasDisappearTransition_ = false;
    bool hasAppearTransition_ = false;
    bool pendingAppearing_ = false;
    bool disableLayer_ = false;
    std::list<OpacityCallback> opacityCallbacks_;
    WeakPtr<DisplayComponent> displayComponent_;
    RefPtr<CurveAnimation<uint8_t>> appearingAnimation_;
    RefPtr<Animator> animator_;
    RefPtr<StateAttributes<DisplayStateAttribute>> stateAttributeList_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DISPLAY_RENDER_DISPLAY_H
