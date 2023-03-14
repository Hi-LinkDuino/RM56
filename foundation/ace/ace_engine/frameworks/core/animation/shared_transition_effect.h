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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SHARED_TRANSITION_EFFECT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SHARED_TRANSITION_EFFECT_H

#include "base/memory/ace_type.h"
#include "core/animation/page_transition_listener.h"
#include "core/components/common/properties/tween_option.h"
#include "core/pipeline/base/component.h"
#include "core/pipeline/base/element.h"

namespace OHOS::Ace {

class SharedTransitionElement;
class OverlayElement;
class Animator;
class TweenElement;

enum class SharedTransitionEffectType {
    SHARED_EFFECT_STATIC,
    SHARED_EFFECT_EXCHANGE,
};

class ACE_EXPORT SharedTransitionEffect : public AceType {
    DECLARE_ACE_TYPE(SharedTransitionEffect, AceType);

public:
    explicit SharedTransitionEffect(const ShareId& shareId, SharedTransitionEffectType type);
    ~SharedTransitionEffect() override = default;
    SharedTransitionEffectType GetType() const
    {
        return type_;
    }
    void SetSharedElement(const WeakPtr<SharedTransitionElement>& src, const WeakPtr<SharedTransitionElement>& dest)
    {
        dest_ = dest;
        src_ = src;
    }
    const WeakPtr<SharedTransitionElement>& GetDestSharedElement() const
    {
        return dest_;
    }
    const WeakPtr<SharedTransitionElement>& GetSrcSharedElement() const
    {
        return src_;
    }
    const RefPtr<Animator>& GetAnimator() const
    {
        return controller_;
    }
    const std::string& GetShareId() const
    {
        return shareId_;
    }
    void setCurrentSharedElement(const WeakPtr<SharedTransitionElement>& current)
    {
        currentWorking_ = current;
    }
    const WeakPtr<SharedTransitionElement>& GetCurrentSharedElement() const
    {
        return currentWorking_;
    }
    virtual bool CreateAnimation(TweenOption& option, TransitionEvent event, bool isLazy) = 0;
    virtual bool ApplyAnimation(RefPtr<OverlayElement>& overlay, RefPtr<Animator>& controller,
        TweenOption& option, TransitionEvent event) = 0;
    virtual bool Allow(TransitionEvent event) = 0;
    static RefPtr<SharedTransitionEffect> GetSharedTransitionEffect(
        SharedTransitionEffectType effect, const ShareId& shareId);

protected:
    bool CheckIn(TransitionEvent event, WeakPtr<SharedTransitionElement>& sharedWeak, Offset& ticket);
    bool TakeOff(TransitionEvent event, RefPtr<OverlayElement>& overlay, WeakPtr<SharedTransitionElement>& sharedWeak,
        const Offset& ticket, TweenOption& option);
    bool TakeOffTween(const RefPtr<Element>& tweenElement, const RefPtr<Component>& passengerComponent,
        const RefPtr<Element>& passengerElement, TweenOption& option);

    ShareId shareId_;
    const SharedTransitionEffectType type_;
    WeakPtr<SharedTransitionElement> dest_;
    WeakPtr<SharedTransitionElement> src_;
    WeakPtr<SharedTransitionElement> currentWorking_;
    RefPtr<Animator> controller_;
    WeakPtr<TweenElement> tweenSeatElement_;
};

class SharedTransitionExchange : public SharedTransitionEffect {
    DECLARE_ACE_TYPE(SharedTransitionExchange, SharedTransitionEffect);

public:
    explicit SharedTransitionExchange(const ShareId& shareId)
        : SharedTransitionEffect(shareId, SharedTransitionEffectType::SHARED_EFFECT_EXCHANGE) {}
    ~SharedTransitionExchange() override = default;
    bool CreateAnimation(TweenOption& option, TransitionEvent event, bool isLazy) override;
    bool ApplyAnimation(RefPtr<OverlayElement>& overlay, RefPtr<Animator>& controller, TweenOption& option,
        TransitionEvent event) override;
    bool Allow(TransitionEvent event) override;

private:
    bool CreateTranslateAnimation(TweenOption& option, TransitionEvent event, bool calledByLazyLoad);
    bool CreateSizeAnimation(TweenOption& option, TransitionEvent event, bool isLazy);
    bool CreateOpacityAnimation(TweenOption& option, TransitionEvent event, bool isLazy);
    void AddLazyLoadCallback(TransitionEvent event);
    bool autoWidth_ = true;
    bool autoHeight_ = true;
    bool autoTranslate_ = true;
};

class SharedTransitionStatic : public SharedTransitionEffect {
    DECLARE_ACE_TYPE(SharedTransitionStatic, SharedTransitionEffect);

public:
    explicit SharedTransitionStatic(const ShareId& shareId)
        : SharedTransitionEffect(shareId, SharedTransitionEffectType::SHARED_EFFECT_STATIC) {}
    ~SharedTransitionStatic() override = default;
    bool CreateAnimation(TweenOption& option, TransitionEvent event, bool isLazy) override;
    // the dest page and source page elements are in effect
    bool ApplyAnimation(RefPtr<OverlayElement>& overlay, RefPtr<Animator>& controller, TweenOption& option,
        TransitionEvent event) override;
    bool Allow(TransitionEvent event) override;

private:
    void AddLazyLoadCallback();
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SHARED_TRANSITION_EFFECT_H
