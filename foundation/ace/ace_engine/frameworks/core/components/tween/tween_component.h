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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TWEEN_TWEEN_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TWEEN_TWEEN_COMPONENT_H

#include "base/utils/macros.h"
#include "core/animation/animation_pub.h"
#include "core/animation/curve.h"
#include "core/components/common/properties/tween_option.h"
#include "core/components/tween/tween_element.h"
#include "core/pipeline/base/composed_component.h"

namespace OHOS::Ace {

class ACE_EXPORT TweenComponent : public ComposedComponent {
    DECLARE_ACE_TYPE(TweenComponent, ComposedComponent);

public:
    TweenComponent(const ComposeId& id, const std::string& name, const RefPtr<Component>& child)
        : ComposedComponent(id, name, child)
    {}
    TweenComponent(const std::string& name, const RefPtr<Component>& child)
        : ComposedComponent(AllocTweenComponentId(), name, child)
    {}
    TweenComponent(const ComposeId& id, const std::string& name) : ComposedComponent(id, name) {}
    explicit TweenComponent(const std::string& name) : ComposedComponent(AllocTweenComponentId(), name) {}
    ~TweenComponent() override = default;

    void SetTweenOption(const TweenOption& option);
    void SetCustomTweenOption(const TweenOption& option);
    void SetAnimationOperation(const AnimationOperation& operation);
    void SetCustomAnimationOperation(const AnimationOperation& operation);

    const TweenOption& GetTweenOption() const;
    const TweenOption& GetCustomTweenOption() const;
    const AnimationOperation& GetAnimationOperation() const;
    const AnimationOperation& GetCustomAnimationOperation() const;

    void SetOptionCssChanged(bool change)
    {
        optionCssChanged_ = change;
    }

    void SetOptionCustomChanged(bool change)
    {
        optionCustomChanged_ = change;
    }

    bool IsOptionCssChanged() const
    {
        return optionCssChanged_;
    }

    bool IsOptionCustomChanged() const
    {
        return optionCustomChanged_;
    }

    void SetOperationCssChanged(bool change)
    {
        operationCssChanged_ = change;
    }

    void SetOperationCustomChanged(bool change)
    {
        operationCustomChanged_ = change;
    }

    bool IsOperationCssChanged() const
    {
        return operationCssChanged_;
    }

    bool IsOperationCustomChanged() const
    {
        return operationCustomChanged_;
    }

    RefPtr<Element> CreateElement() override;

    static ComposeId AllocTweenComponentId();

    void SetIsFirstFrameShow(bool isFirstFrameShow);

    bool GetIsFirstFrameShow() const;

    void SetAnimator(const RefPtr<Animator>& animator);
    RefPtr<Animator> GetAnimator() const;

    void SetLeafNode(bool isLeaf)
    {
        isLeaf_ = isLeaf;
    }

    bool IsLeafNode() const
    {
        return isLeaf_;
    }

    void SetShadow(const Shadow& shadow)
    {
        shadow_ = shadow;
    }

    const Shadow& GetShadow() const
    {
        return shadow_;
    }

    void SetLeft(const Dimension& left)
    {
        positionParam_.left.first = left;
        positionParam_.left.second = true;
    }

    void SetRight(const Dimension& right)
    {
        positionParam_.right.first = right;
        positionParam_.right.second = true;
    }

    void SetTop(const Dimension& top)
    {
        positionParam_.top.first = top;
        positionParam_.top.second = true;
    }

    void SetBottom(const Dimension& bottom)
    {
        positionParam_.bottom.first = bottom;
        positionParam_.bottom.second = true;
    }

    void SetHasLeft(bool hasLeft)
    {
        positionParam_.left.second = hasLeft;
    }

    void SetHasRight(bool hasRight)
    {
        positionParam_.right.second = hasRight;
    }

    void SetHasTop(bool hasTop)
    {
        positionParam_.top.second = hasTop;
    }

    void SetHasBottom(bool hasBottom)
    {
        positionParam_.bottom.second = hasBottom;
    }

    void SetPositionType(PositionType positionType)
    {
        positionParam_.type = positionType;
    }

    const PositionParam& GetPositionParam() const
    {
        return positionParam_;
    }

    void UpdateAnimationName(const std::string& animationName)
    {
        if (animationName_ == animationName) {
            isAnimationNameUpdated_ = false;
        } else {
            animationName_ = animationName;
            isAnimationNameUpdated_ = true;
        }
    }

    bool IsAnimationNameUpdated() const
    {
        return isAnimationNameUpdated_;
    }

    void DisableAnimationNameUpdated()
    {
        isAnimationNameUpdated_ = false;
    }

private:
    TweenOption optionCss_;
    TweenOption optionCustom_;
    AnimationOperation operationCss_ = AnimationOperation::PLAY;
    AnimationOperation operationCustom_ = AnimationOperation::PLAY;
    std::string animationName_;
    bool isAnimationNameUpdated_ = false;
    bool isFirstFrameShow_ = true;
    bool optionCssChanged_ = false;
    bool optionCustomChanged_ = false;
    bool operationCssChanged_ = false;
    bool operationCustomChanged_ = false;
    bool isLeaf_ = false;
    RefPtr<Animator> animator_;
    Shadow shadow_;
    PositionParam positionParam_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TWEEN_TWEEN_COMPONENT_H
