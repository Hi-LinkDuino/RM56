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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_MARQUEE_RENDER_MARQUEE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_MARQUEE_RENDER_MARQUEE_H

#include <functional>

#include "core/animation/animator.h"
#include "core/animation/curve_animation.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class ACE_EXPORT RenderMarquee : public RenderNode {
    DECLARE_ACE_TYPE(RenderMarquee, RenderNode);

public:
    static RefPtr<RenderNode> Create();
    void Update(const RefPtr<Component>& component) override;
    void GetPlayerCtr(const RefPtr<Component>& component, bool emdit, bool control);
    void GetMarqueeCallback(const RefPtr<Component>& component);
    void PerformLayout() override;
    void OnHiddenChanged(bool hidden) override;
    void Start();
    void Stop();
    std::string GetValue() const
    {
        return value_;
    }
    double GetStep() const
    {
        return scrollAmount_;
    }
    int32_t GetLoop() const
    {
        return loop_;
    }
    bool GetStart() const
    {
        return start_;
    }
    bool GetFromStart() const
    {
        if (direction_ == MarqueeDirection::LEFT) {
            return true;
        } else {
            return false;
        }
    }

protected:
    virtual TextDirection GetTextDirection(const std::string& text) const = 0;

    void SetOnStart(const std::function<void(void)>& value)
    {
        onStartEvent_ = value;
    }

    void SetOnBounce(const std::function<void(void)>& value)
    {
        onBounceEvent_ = value;
    }

    void SetOnFinish(const std::function<void(void)>& value)
    {
        onFinishEvent_ = value;
    }

private:
    void UpdateAnimation();
    void UpdateChildPosition(double position);
    void OnAnimationStart();
    void OnAnimationStop();

    RefPtr<RenderNode> childText_;
    RefPtr<Animator> controller_;
    RefPtr<CurveAnimation<double>> translate_;
    Offset childPosition_ = Offset::ErrorOffset();

    bool startAfterLayout_ = true;
    bool startAfterShowed_ = false;
    bool playerFinishControl_ = false;
    bool isHidden_ = false;
    bool start_ = true;
    double scrollAmount_ = 0.0;
    int32_t loop_ = ANIMATION_REPEAT_INFINITE;
    int32_t currentLoop_ = 0;
    std::string value_;
    MarqueeDirection direction_ = MarqueeDirection::LEFT;
    std::function<void()> bounceEvent_;
    std::function<void()> finishEvent_;
    std::function<void()> startEvent_;

    std::function<void()> onBounceEvent_;
    std::function<void()> onFinishEvent_;
    std::function<void()> onStartEvent_;
    Size lastLayoutSize_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_MARQUEE_RENDER_MARQUEE_H
