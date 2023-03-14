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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TWEEN_TWEEN_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TWEEN_TWEEN_ELEMENT_H

#include "base/utils/linear_map.h"
#include "base/utils/utils.h"
#include "core/animation/animation.h"
#include "core/animation/animator.h"
#include "core/animation/property_animatable.h"
#include "core/components/common/layout/position_param.h"
#include "core/components/common/properties/tween_option.h"
#include "core/components/display/render_display.h"
#include "core/components/transform/render_transform.h"
#include "core/components/transform/transform_component.h"
#include "core/pipeline/base/composed_element.h"

namespace OHOS::Ace {

class TweenElement : public ComposedElement, public FlushEvent {
    DECLARE_ACE_TYPE(TweenElement, ComposedElement, FlushEvent);

public:
    explicit TweenElement(const ComposeId& id) : ComposedElement(id) {}
    ~TweenElement() override;

    void PerformBuild() override;
    void Update() override;
    bool CanUpdate(const RefPtr<Component>& newComponent) override;
    void OnPostFlush() override;
    void OnPostAnimationFlush() override;
    void OnPreFlush() override;
    bool ApplyKeyframes();
    void ApplyOptions();
    const TweenOption& GetOption() const;
    void SetOption(const TweenOption& option);
    void SetWrapHidden(bool hidden);
    void AddPreFlush();
    void SkipPostFlush();
    void SetOpacity(uint8_t opacity);
    void SetController(const RefPtr<Animator>& controller);
    RefPtr<RenderNode> GetContentRender() const;
    RefPtr<Element> GetContentElement() const;
    RefPtr<Element> GetContentParent() const;
    const RefPtr<Animator>& GetController() const;
    void SetTouchable(bool enable);

    void Dump() override {}

protected:
    RefPtr<Component> BuildChild() override;

private:
    bool ApplyKeyframes(RefPtr<Animator>& controller, TweenOption& option, BaseId::IdType& prepareId);
    void ApplyOptions(RefPtr<Animator>& controller, TweenOption& option);
    bool IsNeedAnimation(RefPtr<Animator>& controller, TweenOption& option);
    void AddPrepareListener(RefPtr<Animator>& controller, const WeakPtr<RenderTransform>& weakTransform,
        BaseId::IdType& prepareId);
    void CreateTranslateAnimation(const RefPtr<RenderTransform>& renderTransformNode, TweenOption& option);
    void CreateScaleAnimation(const RefPtr<RenderTransform>& renderTransformNode, TweenOption& option);
    void CreateRotateAnimation(const RefPtr<RenderTransform>& renderTransformNode, TweenOption& option);
    void CreateTransformOriginAnimation(const RefPtr<RenderTransform>& renderTransformNode, TweenOption& option);
    void CreateColorAnimation(const RefPtr<PropertyAnimatable>& animatable, TweenOption& option);
    void CreatePropertyAnimationFloat(const RefPtr<PropertyAnimatable>& animatable, TweenOption& option);
    void ApplyOperation(RefPtr<Animator>& controller, AnimationOperation& operation);
    template<class U, class V>
    static bool CreatePropertyAnimation(const RefPtr<PropertyAnimatable>& propertyAnimatable,
        PropertyAnimatableType propertyType, const TweenOption& option, RefPtr<Animation<V>>& animation);
    template<class U>
    static bool AddToAnimator(
        const std::map<PropertyAnimatableType, U>& animations, RefPtr<Animator>& controller, TweenOption& option);

    bool isDelegatedController_ = false;
    bool isComponentController_ = false;
    bool needUpdateTweenOption_ = false;
    bool needUpdateKeyframes_ = false;
    bool needUpdateTweenOptionCustom_ = false;
    RefPtr<Animator> controller_;
    RefPtr<Animator> controllerCustom_;
    BaseId::IdType prepareIdCustom_ = -1;
    BaseId::IdType prepareId_ = -1;
    AnimationOperation operation_ = AnimationOperation::NONE;
    AnimationOperation operationCustom_ = AnimationOperation::NONE;
    TweenOption option_;
    TweenOption optionCustom_;
    uint64_t currentTimestamp_ = 0;
    const static LinearEnumMapNode<AnimationType,
        void (*)(const RefPtr<Animation<float>>&, WeakPtr<RenderTransform>&, TweenOption&)>
        transformFloatAnimationAddMap_[];
    Shadow shadow_;
    PositionParam positionParam_;
    bool skipPostFlush_ = false;
    WeakPtr<TransformComponent> transform_;
    WeakPtr<DisplayComponent> display_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TWEEN_TWEEN_ELEMENT_H
