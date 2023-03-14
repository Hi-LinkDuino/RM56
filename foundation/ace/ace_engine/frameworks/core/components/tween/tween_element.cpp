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

#include "core/components/tween/tween_element.h"

#include "base/geometry/transform_util.h"
#include "core/animation/curve_animation.h"
#include "core/common/frontend.h"
#include "core/components/clip/render_clip.h"
#include "core/components/display/display_component.h"
#include "core/components/positioned/positioned_element.h"
#include "core/components/shared_transition/shared_transition_element.h"
#include "core/components/transform/transform_component.h"
#include "core/components/tween/tween_component.h"
#include "core/pipeline/base/render_element.h"

namespace OHOS::Ace {
namespace {
RefPtr<PropertyAnimatable> GetAnimatable(const RefPtr<Element>& contentElement)
{
    if (!contentElement) {
        LOGE("Get Animatable failed. content element is null.");
        return nullptr;
    }
    return AceType::DynamicCast<PropertyAnimatable>(contentElement->GetRenderNode());
}

void SetTranslateProperties(const RefPtr<Animation<DimensionOffset>>& translateAnimation, TweenOption& option)
{
    if (option.GetCurve()) {
        translateAnimation->SetCurve(option.GetCurve());
    }
    if (!translateAnimation->HasInitValue()) {
        DimensionOffset beginPos = DimensionOffset(Dimension(), Dimension());
        translateAnimation->SetInitValue(beginPos);
    }
}

void SetScaleProperties(const RefPtr<Animation<float>>& scaleAnimation, TweenOption& option)
{
    if (option.GetCurve()) {
        scaleAnimation->SetCurve(option.GetCurve());
    }
    if (!scaleAnimation->HasInitValue()) {
        scaleAnimation->SetInitValue(1.0f);
    }
}

void SetRotateProperties(const RefPtr<Animation<float>>& rotateAnimation, TweenOption& option)
{
    if (option.GetCurve()) {
        rotateAnimation->SetCurve(option.GetCurve());
    }
    if (!rotateAnimation->HasInitValue()) {
        rotateAnimation->SetInitValue(0.0f);
    }
}

void ResetController(RefPtr<Animator>& controller)
{
    if (!controller) {
        LOGE("Reset Controller failed. controller is null.");
        return;
    }
    if (controller->GetStatus() != Animator::Status::IDLE && controller->GetStatus() != Animator::Status::STOPPED) {
        controller->Finish();
    }
    controller->ClearInterpolators();
}

template<class T>
void SetAnimationProperties(const RefPtr<Animation<T>>& animation, TweenOption& option)
{
    if (option.GetCurve()) {
        animation->SetCurve(option.GetCurve());
    }
    if (!animation->HasInitValue()) {
        animation->SetInitValue(T {});
    }
}

void RenderTransformUpdate(WeakPtr<RenderTransform>& weakPtr, const TransformOperation& value)
{
    auto renderTransformNode = weakPtr.Upgrade();
    if (renderTransformNode) {
        switch (value.type_) {
            case TransformOperationType::TRANSLATE:
                renderTransformNode->Translate(
                    value.translateOperation_.dx, value.translateOperation_.dy, value.translateOperation_.dz);
                break;
            case TransformOperationType::SKEW:
                renderTransformNode->Skew(value.skewOperation_.skewX, value.skewOperation_.skewY);
                break;
            case TransformOperationType::ROTATE:
                renderTransformNode->Rotate(value.rotateOperation_.angle, value.rotateOperation_.dx,
                    value.rotateOperation_.dy, value.rotateOperation_.dz);
                break;
            case TransformOperationType::MATRIX:
                renderTransformNode->Matrix3D(value.matrix4_);
                break;
            case TransformOperationType::SCALE:
                renderTransformNode->Scale(
                    value.scaleOperation_.scaleX, value.scaleOperation_.scaleY, value.scaleOperation_.scaleZ);
                break;
            case TransformOperationType::PERSPECTIVE:
                renderTransformNode->Perspective(value.perspectiveOperation_.distance);
                break;
            case TransformOperationType::UNDEFINED:
                renderTransformNode->Translate(Dimension {}, Dimension {}, Dimension {});
                break;
            default:
                LOGE("unsupported transform operation");
                break;
        }
    }
}

void TransformComponentUpdate(WeakPtr<TransformComponent>& transform, const TransformOperation& value)
{
    auto transformComponent = transform.Upgrade();
    if (transformComponent) {
        transformComponent->ResetTransform();
        switch (value.type_) {
            case TransformOperationType::TRANSLATE:
                transformComponent->Translate(
                    value.translateOperation_.dx, value.translateOperation_.dy, value.translateOperation_.dz);
                break;
            case TransformOperationType::SKEW:
                transformComponent->Skew(value.skewOperation_.skewX, value.skewOperation_.skewY);
                break;
            case TransformOperationType::ROTATE:
                transformComponent->Rotate(value.rotateOperation_.dx, value.rotateOperation_.dy,
                    value.rotateOperation_.dz, value.rotateOperation_.angle);
                break;
            case TransformOperationType::MATRIX:
                transformComponent->Matrix3d(value.matrix4_);
                break;
            case TransformOperationType::SCALE:
                transformComponent->Scale(
                    value.scaleOperation_.scaleX, value.scaleOperation_.scaleY, value.scaleOperation_.scaleZ);
                break;
            case TransformOperationType::PERSPECTIVE:
                transformComponent->Perspective(value.perspectiveOperation_.distance);
                break;
            case TransformOperationType::UNDEFINED:
                transformComponent->Translate(Dimension {}, Dimension {}, Dimension {});
                break;
            default:
                LOGE("unsupported transform operation");
                break;
        }
    }
}

void RenderTransformOriginUpdate(const WeakPtr<RenderTransform>& weakPtr, const DimensionOffset& origin)
{
    auto renderTransformNode = weakPtr.Upgrade();
    if (renderTransformNode) {
        renderTransformNode->SetTransformOrigin(origin.GetX(), origin.GetY());
        renderTransformNode->MarkNeedUpdateOrigin();
    }
}

void CreateTransformAnimation(const RefPtr<RenderTransform>& renderTransformNode,
    const WeakPtr<TransformComponent>& transform, TweenOption& option)
{
    WeakPtr<RenderTransform> weak(renderTransformNode);

    for (const auto& animation : option.GetTransformAnimations()) {
        if (animation) {
            SetAnimationProperties(animation, option);
            animation->AddListener(std::bind(RenderTransformUpdate, weak, std::placeholders::_1));
            animation->AddListener(std::bind(TransformComponentUpdate, transform, std::placeholders::_1));
        }
    }
}

void OpacityAnimationListener(
    const WeakPtr<RenderDisplay>& weakRender, WeakPtr<DisplayComponent>& display, float value)
{
    auto opacity = static_cast<uint8_t>(Round(value * UINT8_MAX));
    if (value < 0.0f || value > 1.0f) {
        opacity = UINT8_MAX;
    }
    auto renderDisplayNode = weakRender.Upgrade();
    auto displayComponent = display.Upgrade();
    if (renderDisplayNode) {
        renderDisplayNode->UpdateOpacity(opacity);
        if (displayComponent) {
            displayComponent->SetOpacity((double) opacity / UINT8_MAX);
        }
    }
}

void CreateOpacityAnimation(
    const RefPtr<RenderDisplay>& renderDisplayNode, const WeakPtr<DisplayComponent>& display, TweenOption& option)
{
    auto& opacityAnimation = option.GetOpacityAnimation();
    if (!opacityAnimation) {
        LOGD("create opacity animation with null. skip it.");
        return;
    }
    if (!opacityAnimation->HasInitValue()) {
        opacityAnimation->SetInitValue(UINT8_MAX);
    }
    WeakPtr<RenderDisplay> weakRender = renderDisplayNode;
    opacityAnimation->AddListener(std::bind(OpacityAnimationListener, weakRender, display, std::placeholders::_1));

    if (option.GetCurve()) {
        opacityAnimation->SetCurve(option.GetCurve());
    }
}

bool BindingTransformAnimationToController(RefPtr<Animator>& controller, TweenOption& option)
{
    bool needAnimation = false;
    const auto& animations = option.GetTransformAnimations();
    for (auto& animation : animations) {
        if (animation) {
            needAnimation = true;
            controller->AddInterpolator(animation);
        }
    }
    return needAnimation;
}

} // namespace

const LinearEnumMapNode<AnimationType,
    void (*)(const RefPtr<Animation<float>>&, WeakPtr<RenderTransform>&, TweenOption&)>
    TweenElement::transformFloatAnimationAddMap_[] = {
        { AnimationType::SCALE,
            [](const RefPtr<Animation<float>>& scaleAnimation, WeakPtr<RenderTransform>& weakRender,
                TweenOption& option) {
                SetScaleProperties(scaleAnimation, option);
                scaleAnimation->AddListener([weakRender, scaleAnimation](float value) {
                    auto renderTransformNode = weakRender.Upgrade();
                    if (renderTransformNode) {
                        renderTransformNode->Scale(value);
                    }
                });
            } },
        { AnimationType::SCALE_X,
            [](const RefPtr<Animation<float>>& scaleXAnimation, WeakPtr<RenderTransform>& weakRender,
                TweenOption& option) {
                SetScaleProperties(scaleXAnimation, option);
                double maxScaleXY = option.GetMaxScaleXY();
                auto renderTransformNode = weakRender.Upgrade();
                if (renderTransformNode) {
                    renderTransformNode->SetMaxScaleXY(maxScaleXY);
                }
                scaleXAnimation->AddListener([weakRender, scaleXAnimation](float value) {
                    auto renderTransformNode = weakRender.Upgrade();
                    if (renderTransformNode) {
                        renderTransformNode->Scale(value, 1.0f);
                    }
                });
            } },
        { AnimationType::SCALE_Y,
            [](const RefPtr<Animation<float>>& scaleYAnimation, WeakPtr<RenderTransform>& weakRender,
                TweenOption& option) {
                SetScaleProperties(scaleYAnimation, option);
                scaleYAnimation->AddListener([weakRender, scaleYAnimation](float value) {
                    auto renderTransformNode = weakRender.Upgrade();
                    if (renderTransformNode) {
                        renderTransformNode->Scale(1.0f, value);
                    }
                });
            } },
        { AnimationType::ROTATE_Z,
            [](const RefPtr<Animation<float>>& rotateZAnimation, WeakPtr<RenderTransform>& weakRender,
                TweenOption& option) {
                SetRotateProperties(rotateZAnimation, option);
                rotateZAnimation->AddListener([weakRender, rotateZAnimation](float value) {
                    auto renderTransformNode = weakRender.Upgrade();
                    if (renderTransformNode) {
                        renderTransformNode->RotateZ(value);
                    }
                });
            } },
        { AnimationType::ROTATE_X,
            [](const RefPtr<Animation<float>>& rotateXAnimation, WeakPtr<RenderTransform>& weakRender,
                TweenOption& option) {
                SetRotateProperties(rotateXAnimation, option);
                rotateXAnimation->AddListener([weakRender, rotateXAnimation](float value) {
                    auto renderTransformNode = weakRender.Upgrade();
                    if (renderTransformNode) {
                        renderTransformNode->RotateX(value);
                    }
                });
            } },
        { AnimationType::ROTATE_Y,
            [](const RefPtr<Animation<float>>& rotateYAnimation, WeakPtr<RenderTransform>& weakRender,
                TweenOption& option) {
                SetRotateProperties(rotateYAnimation, option);
                rotateYAnimation->AddListener([weakRender, rotateYAnimation](float value) {
                    auto renderTransformNode = weakRender.Upgrade();
                    if (renderTransformNode) {
                        renderTransformNode->RotateY(value);
                    }
                });
            } }
    };

void TweenElement::Update()
{
    ComposedElement::Update();
    if (!component_) {
        return;
    }
    auto tweenComponent = AceType::DynamicCast<TweenComponent>(component_);
    if (!tweenComponent) {
        LOGE("Get TweenComponent failed.");
        return;
    }
    shadow_ = tweenComponent->GetShadow();
    positionParam_ = tweenComponent->GetPositionParam();

    if (tweenComponent->IsAnimationNameUpdated()) {
        needUpdateKeyframes_ = true;
        tweenComponent->DisableAnimationNameUpdated();
    }
    if (tweenComponent->IsOptionCssChanged()) {
        needUpdateTweenOption_ = true;
        option_ = tweenComponent->GetTweenOption();
        tweenComponent->SetOptionCssChanged(false);
    }
    if (tweenComponent->IsOptionCustomChanged()) {
        needUpdateTweenOptionCustom_ = true;
        optionCustom_ = tweenComponent->GetCustomTweenOption();
        tweenComponent->SetOptionCustomChanged(false);
    }
    if (tweenComponent->IsOperationCssChanged()) {
        operation_ = tweenComponent->GetAnimationOperation();
        tweenComponent->SetOperationCssChanged(false);
    }
    if (tweenComponent->IsOperationCustomChanged()) {
        operationCustom_ = tweenComponent->GetCustomAnimationOperation();
        tweenComponent->SetOperationCustomChanged(false);
    }
    auto pipelineContext = context_.Upgrade();
    if (pipelineContext) {
        RefPtr<Animator> controller = tweenComponent->GetAnimator();
        if (controller) {
            isComponentController_ = true;
            if (!controller->HasScheduler()) {
                controller->AttachScheduler(context_);
            }
            controllerCustom_ = controller;
        }
        if (!controller_) {
            isDelegatedController_ = false;
            controller_ = AceType::MakeRefPtr<Animator>(context_);
            LOGD("set animator to component when update.");
        }

        LOGD("add request to pipeline context.");
        if (operation_ == AnimationOperation::PAUSE || operation_ == AnimationOperation::FINISH
            || operationCustom_ == AnimationOperation::PAUSE || operationCustom_ == AnimationOperation::FINISH) {
            pipelineContext->AddPostAnimationFlushListener(AceType::Claim(this));
        }
        pipelineContext->AddPostFlushListener(AceType::Claim(this));
    }
}

TweenElement::~TweenElement()
{
    if (isComponentController_ && controllerCustom_) {
        controllerCustom_->ClearInterpolators();
        controllerCustom_->ClearAllListeners();
        controllerCustom_->Stop();
    }
}

void TweenElement::ApplyOperation(RefPtr<Animator>& controller, AnimationOperation& operation)
{
    LOGD("apply operation: %{public}d", operation);
    switch (operation) {
        case AnimationOperation::PLAY:
            controller->Play();
            break;
        case AnimationOperation::RUNNING:
            controller->Play();
            break;
        case AnimationOperation::PAUSE:
            controller->Pause();
            break;
        case AnimationOperation::CANCEL:
            controller->Cancel();
            break;
        case AnimationOperation::FINISH:
            controller->Finish();
            break;
        case AnimationOperation::REVERSE:
            controller->Reverse();
            break;
        case AnimationOperation::NONE:
        default:
            break;
    }
}

void TweenElement::OnPostFlush()
{
    if (skipPostFlush_) {
        skipPostFlush_ = false;
        return;
    }
    AddPreFlush();
}

void TweenElement::OnPostAnimationFlush()
{
    if (controller_) {
        controller_->TriggerFrame(controller_->GetPlayedTime(), true);
    }
    if (controllerCustom_) {
        controllerCustom_->TriggerFrame(controllerCustom_->GetPlayedTime(), true);
    }
}

void TweenElement::OnPreFlush()
{
    if (!controller_ && !controllerCustom_) {
        LOGD("empty controller, skip start tween.");
        return;
    }
    SetWrapHidden(false);
    if (isDelegatedController_ && !isComponentController_) {
        LOGD("controller is set from outside. skip prepare animation.");
        return;
    }

    if (needUpdateKeyframes_ || (operation_ == AnimationOperation::PLAY && needUpdateTweenOption_)) {
        ResetController(controller_);
        ApplyKeyframes();
        needUpdateKeyframes_ = false;
    }

    if (needUpdateTweenOption_) {
        ApplyOptions(controller_, option_);
        needUpdateTweenOption_ = false;
    }
    if (needUpdateTweenOptionCustom_) {
        ResetController(controllerCustom_);
        ApplyKeyframes(controllerCustom_, optionCustom_, prepareIdCustom_);
        ApplyOptions(controllerCustom_, optionCustom_);
        needUpdateTweenOptionCustom_ = false;
    }
    if (operation_ != AnimationOperation::NONE || operationCustom_ != AnimationOperation::NONE) {
        auto pipelineContext = context_.Upgrade();
        if (!pipelineContext) {
            return;
        }
    }

    LOGD("Start tween animation with operation: %{public}d, operationCustom: %{public}d", operation_, operationCustom_);
    if (controller_) {
        ApplyOperation(controller_, operation_);
    }
    if (controllerCustom_) {
        ApplyOperation(controllerCustom_, operationCustom_);
    }

    // reset operation to none.
    operation_ = AnimationOperation::NONE;
    operationCustom_ = AnimationOperation::NONE;
}

bool TweenElement::IsNeedAnimation(RefPtr<Animator>& controller, TweenOption& option)
{
    if (!controller) {
        LOGE("add interpolator failed. controller is null.");
        return false;
    }
    bool needAnimation = false;
    auto& transformOffsetAnimations = option.GetTranslateAnimations();
    for (auto&& [translate, animation] : transformOffsetAnimations) {
        if (animation) {
            needAnimation = true;
            LOGD("add translate animation.");
            controller->AddInterpolator(animation);
        }
    }
    auto& transformFloatAnimations = option.GetTransformFloatAnimation();
    for (auto&& [transformFloat, animation] : transformFloatAnimations) {
        if (animation) {
            needAnimation = true;
            LOGD("add transform float animation.");
            controller->AddInterpolator(animation);
        }
    }
    if (BindingTransformAnimationToController(controller, option)) {
        needAnimation = true;
    }
    auto& transformOriginAnimation = option.GetTransformOriginAnimation();
    if (transformOriginAnimation) {
        controller->AddInterpolator(transformOriginAnimation);
        // no need enable needAnimation, Transform Origin Animation only work when set transform animation.
    }
    auto& opacityAnimation = option.GetOpacityAnimation();
    if (opacityAnimation) {
        LOGD("add opacity animation.");
        controller->AddInterpolator(opacityAnimation);
        needAnimation = true;
    }
    auto& colorAnimation = option.GetColorAnimation();
    if (colorAnimation) {
        LOGD("add color animation.");
        controller->AddInterpolator(colorAnimation);
        needAnimation = true;
    }
    if (AddToAnimator(option.GetFloatPropertyAnimation(), controller, option)) {
        needAnimation = true;
    }
    return needAnimation;
}

RefPtr<Component> TweenElement::BuildChild()
{
    RefPtr<TweenComponent> tween = AceType::DynamicCast<TweenComponent>(component_);
    if (tween) {
        RefPtr<DisplayComponent> displayComponent = AceType::DynamicCast<DisplayComponent>(tween->GetChild());
        RefPtr<TransformComponent> transformComponent;
        if (displayComponent) {
            transformComponent = AceType::DynamicCast<TransformComponent>(displayComponent->GetChild());
            if (!transformComponent) {
                transformComponent = AceType::MakeRefPtr<TransformComponent>();
                transformComponent->SetChild(displayComponent->GetChild());
                displayComponent->SetChild(transformComponent);
            }
        } else {
            transformComponent = AceType::MakeRefPtr<TransformComponent>();
            displayComponent = AceType::MakeRefPtr<DisplayComponent>(transformComponent);
            transformComponent->SetChild(ComposedElement::BuildChild());
        }
        displayComponent->SetPositionType(positionParam_.type);
        displayComponent->SetHasLeft(positionParam_.left.second);
        displayComponent->SetHasRight(positionParam_.right.second);
        displayComponent->SetHasTop(positionParam_.top.second);
        displayComponent->SetHasBottom(positionParam_.bottom.second);
        displayComponent->SetLeft(positionParam_.left.first);
        displayComponent->SetRight(positionParam_.right.first);
        displayComponent->SetTop(positionParam_.top.first);
        displayComponent->SetBottom(positionParam_.bottom.first);
        displayComponent->DisableLayer(tween->IsLeafNode());
        transform_ = transformComponent;
        display_ = displayComponent;
        return displayComponent;
    } else {
        LOGE("no tween component found. return empty child.");
        return nullptr;
    }
}

void TweenElement::PerformBuild()
{
    ComposedElement::PerformBuild();
    auto tweenComponent = AceType::DynamicCast<TweenComponent>(component_);
    if (!tweenComponent) {
        LOGE("Get TweenComponent failed.");
        return;
    }
    if (!tweenComponent->GetIsFirstFrameShow()) {
        SetWrapHidden(true);
    }
}

bool TweenElement::CanUpdate(const RefPtr<Component>& newComponent)
{
    auto pipelineContext = context_.Upgrade();
    if (pipelineContext && pipelineContext->GetIsDeclarative()) {
        return ComposedElement::CanUpdate(newComponent);
    }
    // components of the same type are not updated.
    return Element::CanUpdate(newComponent);
}

void TweenElement::CreateTranslateAnimation(const RefPtr<RenderTransform>& renderTransformNode, TweenOption& option)
{
    if (!option.HasTransformOffsetChanged()) {
        LOGD("create translate animation with null. skip it.");
        return;
    }
    auto& transformOffsetAnimations = option.GetTranslateAnimations();
    WeakPtr<RenderTransform> weakRender = renderTransformNode;
    static const LinearEnumMapNode<AnimationType,
        void (*)(const RefPtr<Animation<DimensionOffset>>&, WeakPtr<RenderTransform>&, TweenOption&)>
        translateAnimationAddMap[] = {
            { AnimationType::TRANSLATE,
                [](const RefPtr<Animation<DimensionOffset>>& translateAnimation,
                    WeakPtr<RenderTransform>& weakRender, TweenOption& option) {
                    SetTranslateProperties(translateAnimation, option);
                    translateAnimation->AddListener([weakRender](const DimensionOffset& value) {
                        auto renderTransformNode = weakRender.Upgrade();
                        if (renderTransformNode) {
                            renderTransformNode->Translate(value.GetX(), value.GetY());
                        }
                    });
                } },
            { AnimationType::TRANSLATE_X,
                [](const RefPtr<Animation<DimensionOffset>>& translateXAnimation, WeakPtr<RenderTransform>& weakRender,
                    TweenOption& option) {
                    SetTranslateProperties(translateXAnimation, option);
                    translateXAnimation->AddListener([weakRender](const DimensionOffset& value) {
                        auto renderTransformNode = weakRender.Upgrade();
                        if (renderTransformNode) {
                            renderTransformNode->Translate(value.GetX(), 0.0_px);
                        }
                    });
                } },
            { AnimationType::TRANSLATE_Y,
                [](const RefPtr<Animation<DimensionOffset>>& translateYAnimation, WeakPtr<RenderTransform>& weakRender,
                    TweenOption& option) {
                    SetTranslateProperties(translateYAnimation, option);
                    translateYAnimation->AddListener([weakRender](const DimensionOffset& value) {
                        auto renderTransformNode = weakRender.Upgrade();
                        if (renderTransformNode) {
                            renderTransformNode->Translate(0.0_px, value.GetY());
                        }
                    });
                } }
            };
    size_t mapSize = ArraySize(translateAnimationAddMap);
    auto iterTranslateAnimation = transformOffsetAnimations.find(AnimationType::TRANSLATE);
    if (iterTranslateAnimation != transformOffsetAnimations.end()) {
        auto translateAnimationIter =
            BinarySearchFindIndex(translateAnimationAddMap, mapSize, AnimationType::TRANSLATE);
        if (translateAnimationIter != -1) {
            auto& translateAnimation = iterTranslateAnimation->second;
            translateAnimationAddMap[translateAnimationIter].value(translateAnimation, weakRender, option);
        }
    }

    auto iterTranslateXAnimation = transformOffsetAnimations.find(AnimationType::TRANSLATE_X);
    if (iterTranslateXAnimation != transformOffsetAnimations.end()) {
        auto translateXAnimationIter =
            BinarySearchFindIndex(translateAnimationAddMap, mapSize, AnimationType::TRANSLATE_X);
        if (translateXAnimationIter != -1) {
            auto& translateXAnimation = iterTranslateXAnimation->second;
            translateAnimationAddMap[translateXAnimationIter].value(translateXAnimation, weakRender, option);
        }
    }

    auto iterTranslateYAnimation = transformOffsetAnimations.find(AnimationType::TRANSLATE_Y);
    if (iterTranslateYAnimation != transformOffsetAnimations.end()) {
        auto translateYAnimationIter =
            BinarySearchFindIndex(translateAnimationAddMap, mapSize, AnimationType::TRANSLATE_Y);
        if (translateYAnimationIter != -1) {
            auto& translateYAnimation = iterTranslateYAnimation->second;
            translateAnimationAddMap[translateYAnimationIter].value(translateYAnimation, weakRender, option);
        }
    }
}

void TweenElement::CreateScaleAnimation(const RefPtr<RenderTransform>& renderTransformNode, TweenOption& option)
{
    if (!option.HasTransformFloatChanged()) {
        LOGD("create scale animation with null. skip it.");
        return;
    }
    auto& transformFloatAnimations = option.GetTransformFloatAnimation();
    WeakPtr<RenderTransform> weakRender = renderTransformNode;
    auto iterScaleAnimation = transformFloatAnimations.find(AnimationType::SCALE);
    size_t mapSize = ArraySize(transformFloatAnimationAddMap_);
    if (iterScaleAnimation != transformFloatAnimations.end()) {
        auto scaleAnimationIter = BinarySearchFindIndex(transformFloatAnimationAddMap_, mapSize, AnimationType::SCALE);
        if (scaleAnimationIter != -1) {
            auto& scaleAnimation = iterScaleAnimation->second;
            transformFloatAnimationAddMap_[scaleAnimationIter].value(scaleAnimation, weakRender, option);
        }
    }

    auto iterScaleXAnimation = transformFloatAnimations.find(AnimationType::SCALE_X);
    if (iterScaleXAnimation != transformFloatAnimations.end()) {
        auto scaleXAnimationIter =
            BinarySearchFindIndex(transformFloatAnimationAddMap_, mapSize, AnimationType::SCALE_X);
        if (scaleXAnimationIter != -1) {
            auto& scaleXAnimation = iterScaleXAnimation->second;
            transformFloatAnimationAddMap_[scaleXAnimationIter].value(scaleXAnimation, weakRender, option);
        }
    }

    auto iterScaleYAnimation = transformFloatAnimations.find(AnimationType::SCALE_Y);
    if (iterScaleYAnimation != transformFloatAnimations.end()) {
        auto scaleYAnimationIter =
            BinarySearchFindIndex(transformFloatAnimationAddMap_, mapSize, AnimationType::SCALE_Y);
        if (scaleYAnimationIter != -1) {
            auto& scaleYAnimation = iterScaleYAnimation->second;
            transformFloatAnimationAddMap_[scaleYAnimationIter].value(scaleYAnimation, weakRender, option);
        }
    }
}

void TweenElement::CreateTransformOriginAnimation(
    const RefPtr<RenderTransform>& renderTransformNode, TweenOption& option)
{
    if (option.HasTransformOriginChanged()) {
        renderTransformNode->SetTransformOrigin(option.GetTransformOriginX(), option.GetTransformOriginY());
        auto animation = option.GetTransformOriginAnimation();
        if (animation) {
            animation->AddListener([weak = AceType::WeakClaim(AceType::RawPtr(renderTransformNode))](
                                       const DimensionOffset& value) { RenderTransformOriginUpdate(weak, value); });

            if (option.GetCurve()) {
                animation->SetCurve(option.GetCurve());
            }
        }
        option.SetTransformOriginChanged(false);
    } else {
        renderTransformNode->SetTransformOrigin(HALF_PERCENT, HALF_PERCENT);
        renderTransformNode->MarkNeedUpdateOrigin();
    }
}

void TweenElement::CreateRotateAnimation(const RefPtr<RenderTransform>& renderTransformNode, TweenOption& option)
{
    if (!option.HasTransformFloatChanged()) {
        LOGD("create rotate animation with null. skip it.");
        return;
    }
    auto& transformFloatAnimations = option.GetTransformFloatAnimation();
    WeakPtr<RenderTransform> weakRender = renderTransformNode;
    auto iterRotateZAnimation = transformFloatAnimations.find(AnimationType::ROTATE_Z);
    size_t mapSize = ArraySize(transformFloatAnimationAddMap_);
    if (iterRotateZAnimation != transformFloatAnimations.end()) {
        auto rotateZAnimationIter =
            BinarySearchFindIndex(transformFloatAnimationAddMap_, mapSize, AnimationType::ROTATE_Z);
        if (rotateZAnimationIter != -1) {
            auto& rotateZAnimation = iterRotateZAnimation->second;
            transformFloatAnimationAddMap_[rotateZAnimationIter].value(rotateZAnimation, weakRender, option);
        }
    }

    auto iterRotateXAnimation = transformFloatAnimations.find(AnimationType::ROTATE_X);
    if (iterRotateXAnimation != transformFloatAnimations.end()) {
        auto rotateXAnimationIter =
            BinarySearchFindIndex(transformFloatAnimationAddMap_, mapSize, AnimationType::ROTATE_X);
        if (rotateXAnimationIter != -1) {
            auto& rotateXAnimation = iterRotateXAnimation->second;
            transformFloatAnimationAddMap_[rotateXAnimationIter].value(rotateXAnimation, weakRender, option);
        }
    }

    auto iterRotateYAnimation = transformFloatAnimations.find(AnimationType::ROTATE_Y);
    if (iterRotateYAnimation != transformFloatAnimations.end()) {
        auto rotateYAnimationIter =
            BinarySearchFindIndex(transformFloatAnimationAddMap_, mapSize, AnimationType::ROTATE_Y);
        if (rotateYAnimationIter != -1) {
            auto& rotateYAnimation = iterRotateYAnimation->second;
            transformFloatAnimationAddMap_[rotateYAnimationIter].value(rotateYAnimation, weakRender, option);
        }
    }
}

void TweenElement::CreateColorAnimation(const RefPtr<PropertyAnimatable>& animatable, TweenOption& option)
{
    if (!animatable) {
        LOGE("create color animation failed. not a animatable child.");
        return;
    }
    auto& colorAnimation = option.GetColorAnimation();
    if (!colorAnimation) {
        LOGE("create color animation with null. skip it.");
        return;
    }
    PropertyAnimatableType propertyType;
    if (option.GetIsBackground()) {
        propertyType = PropertyAnimatableType::PROPERTY_BACK_DECORATION_COLOR;
    } else {
        propertyType = PropertyAnimatableType::PROPERTY_FRONT_DECORATION_COLOR;
    }
    CreatePropertyAnimation<ColorPropertyAnimatable, Color>(animatable, propertyType, option, colorAnimation);
}

template<class U, class V>
bool TweenElement::CreatePropertyAnimation(const RefPtr<PropertyAnimatable>& propertyAnimatable,
    PropertyAnimatableType propertyType, const TweenOption& option, RefPtr<Animation<V>>& animation)
{
    if (!animation) {
        LOGE("CreatePropertyAnimation failed, animation is null.");
        return false;
    }
    typename U::Type initValue;
    bool created =
        PropertyAnimatable::AddPropertyAnimation<U, V>(propertyAnimatable, propertyType, animation, initValue);
    if (!created) {
        LOGE("create property animation failed. property: %{public}d", propertyType);
        return false;
    }
    if (option.GetCurve()) {
        animation->SetCurve(option.GetCurve());
    }
    if (!animation->HasInitValue()) {
        animation->SetInitValue(initValue);
    }
    return true;
}

template<class U>
bool TweenElement::AddToAnimator(
    const std::map<PropertyAnimatableType, U>& animations, RefPtr<Animator>& controller, TweenOption& option)
{
    bool needAnimation = false;
    for (auto&& [property, animation] : animations) {
        if (animation) {
            needAnimation = true;
            LOGD("add property animation. property: %{public}d", property);
            controller->AddInterpolator(animation);
        }
    }
    return needAnimation;
}

void TweenElement::SetController(const RefPtr<Animator>& controller)
{
    if (!controller) {
        LOGE("set controller failed. controller is empty.");
        return;
    }
    LOGD("set controller");
    if (!controller_->IsStopped()) {
        controller_->Stop();
    }
    isDelegatedController_ = true;
    controller_ = controller;
}

const TweenOption& TweenElement::GetOption() const
{
    return option_;
}

void TweenElement::SetOption(const TweenOption& option)
{
    LOGD("set tween option");
    option_ = option;
}

const RefPtr<Animator>& TweenElement::GetController() const
{
    return controller_;
}

void TweenElement::SetOpacity(uint8_t opacity)
{
    if (children_.empty()) {
        LOGE("no child when set Opacity");
        return;
    }
    const auto& child = children_.front();
    if (!child) {
        LOGE("child is null.");
        return;
    }
    auto childElement = AceType::DynamicCast<RenderElement>(child);
    if (!childElement) {
        LOGE("child element is null.");
        return;
    }
    const auto& displayRenderNode = AceType::DynamicCast<RenderDisplay>(childElement->GetRenderNode());
    if (!displayRenderNode) {
        LOGE("no display render node found.");
        return;
    }
    LOGD("set Opacity. Opacity: %{public}d", opacity);
    displayRenderNode->UpdateOpacity(opacity);
}

void TweenElement::SkipPostFlush()
{
    skipPostFlush_ = true;
}

void TweenElement::AddPreFlush()
{
    auto pipelineContext = context_.Upgrade();
    if (!pipelineContext) {
        return;
    }
    pipelineContext->AddPreFlushListener(AceType::Claim(this));
}

void TweenElement::SetWrapHidden(bool hidden)
{
    if (children_.empty()) {
        LOGE("no child when set visible");
        return;
    }
    const auto& child = children_.front();
    if (!child) {
        LOGE("child is null.");
        return;
    }
    auto childElement = AceType::DynamicCast<RenderElement>(child);
    if (!childElement) {
        LOGE("child element is null.");
        return;
    }
    const auto& displayRenderNode = AceType::DynamicCast<RenderDisplay>(childElement->GetRenderNode());
    if (!displayRenderNode) {
        LOGE("no display render node found.");
        return;
    }
    displayRenderNode->UpdateHidden(hidden);
}

void TweenElement::SetTouchable(bool enable)
{
    LOGD("set tween touchable status: %{public}d", enable);

    if (children_.empty()) {
        LOGW("get content child failed. no child yet.");
        return;
    }
    const auto& child = children_.front();
    if (!child || child->GetType() != RENDER_ELEMENT) {
        LOGW("get content child failed. null child or not render child.");
        return;
    }
    const auto& transformElement = AceType::DynamicCast<RenderElement>(child)->GetFirstChild();
    if (!transformElement) {
        LOGE("Get RenderElement failed.");
        return;
    }
    const auto& transformRenderNode = AceType::DynamicCast<RenderTransform>(transformElement->GetRenderNode());
    if (transformRenderNode) {
        transformRenderNode->SetTouchable(enable);
    }
}

RefPtr<RenderNode> TweenElement::GetContentRender() const
{
    auto contentElement = GetContentElement();
    if (!contentElement) {
        return nullptr;
    }
    return contentElement->GetRenderNode();
}

bool TweenElement::ApplyKeyframes()
{
    return ApplyKeyframes(controller_, option_, prepareId_);
}

void TweenElement::AddPrepareListener(
    RefPtr<Animator>& controller, const WeakPtr<RenderTransform>& weakTransform, BaseId::IdType& prepareId)
{
    if (!controller) {
        LOGE("Add Prepare Listener failed. controller is null.");
        return;
    }
    controller->RemovePrepareListener(prepareId);
    prepareId =
        controller->AddPrepareListener([weakTransform, weakContext = context_,
                                           weakTween = AceType::WeakClaim(this),
                                           needForceResetTransform =
                                              controller->GetAllowRunningAsynchronously()]() {
            // reset transform matrix at the start of every frame.
            auto context = weakContext.Upgrade();
            auto tween = weakTween.Upgrade();
            auto transform = weakTransform.Upgrade();
            if (context && tween && transform) {
                auto currentTimestamp = context->GetTimeFromExternalTimer();
                if (tween->currentTimestamp_ != currentTimestamp || tween->currentTimestamp_ == 0 ||
                    needForceResetTransform) {
                    transform->ResetTransform();
                    tween->currentTimestamp_ = currentTimestamp;
                }
            }
        });
}

bool TweenElement::ApplyKeyframes(RefPtr<Animator>& controller, TweenOption& option, BaseId::IdType& prepareId)
{
    if (!controller) {
        LOGW("controller is null.");
        return false;
    }
    if (children_.empty()) {
        LOGW("apply option failed. no child yet.");
        return false;
    }
    const auto& child = children_.front();
    if (!child || child->GetType() != RENDER_ELEMENT) {
        LOGW("apply option failed. null child or not render child.");
        return false;
    }
    LOGD("TweenElement: ApplyKeyframes.");

    const auto& displayRenderNode =
        AceType::DynamicCast<RenderDisplay>(AceType::DynamicCast<RenderElement>(child)->GetRenderNode());
    if (!displayRenderNode) {
        LOGE("display render node is null.");
        return false;
    }
    const auto& transformElement = AceType::DynamicCast<RenderElement>(child)->GetFirstChild();
    if (!transformElement) {
        LOGE("transform element node is null.");
        return false;
    }
    const auto& transformRenderNode = AceType::DynamicCast<RenderTransform>(transformElement->GetRenderNode());
    if (!transformRenderNode) {
        LOGE("transform render node is null.");
        return false;
    }
    if (shadow_.IsValid()) {
        displayRenderNode->SetShadow(shadow_);
        transformRenderNode->SetShadow(shadow_);
    }

    const auto& contentElement = AceType::DynamicCast<RenderElement>(transformElement)->GetFirstChild();
    auto animatable = GetAnimatable(contentElement);
    if (animatable) {
        CreateColorAnimation(animatable, option);
        CreatePropertyAnimationFloat(animatable, option);
    }
    CreateTransformAnimation(transformRenderNode, transform_, option);
    CreateTranslateAnimation(transformRenderNode, option);
    CreateScaleAnimation(transformRenderNode, option);
    CreateRotateAnimation(transformRenderNode, option);
    CreateTransformOriginAnimation(transformRenderNode, option);
    if (option.HasTransformOffsetChanged() || option.HasTransformFloatChanged() || option.HasTransformChanged()) {
        AddPrepareListener(controller, transformRenderNode, prepareId);
    }
    CreateOpacityAnimation(displayRenderNode, display_, option);
    return IsNeedAnimation(controller, option);
}

void TweenElement::ApplyOptions(RefPtr<Animator>& controller, TweenOption& option)
{
    if (!controller) {
        LOGE("Apply Options failed. Controller is null.");
        return;
    }
    LOGD("apply options.");
    controller->SetDuration(option.GetDuration());
    controller->SetIteration(option.GetIteration());
    controller->SetStartDelay(option.GetDelay());
    controller->SetFillMode(option.GetFillMode());
    controller->SetTempo(option.GetTempo());
    controller->SetAnimationDirection(option.GetAnimationDirection());
    controller->SetAllowRunningAsynchronously(option.GetAllowRunningAsynchronously());

    for (const auto& [type, animation] : option.GetAnimatables()) {
        if (option.GetCurve()) {
            animation->SetCurve(option.GetCurve());
        }
        controller->AddInterpolator(animation);
    }
}

void TweenElement::ApplyOptions()
{
    ApplyOptions(controller_, option_);
}

RefPtr<Element> TweenElement::GetContentElement() const
{
    const auto& mountParent = GetContentParent();
    if (!mountParent) {
        LOGE("Get content element failed. content parent is null.");
        return nullptr;
    }
    return mountParent->GetFirstChild();
}

RefPtr<Element> TweenElement::GetContentParent() const
{
    const auto child = GetFirstChild();
    if (!child) {
        LOGW("Get transformElement failed. null child.");
        return nullptr;
    }
    const auto& displayRenderNode =
        AceType::DynamicCast<RenderDisplay>(AceType::DynamicCast<RenderElement>(child)->GetRenderNode());
    if (!displayRenderNode) {
        LOGE("display render node is null.");
        return nullptr;
    }
    const auto& transformElement = AceType::DynamicCast<RenderElement>(child)->GetFirstChild();
    if (!transformElement) {
        LOGE("Get transformElement failed. transform element is null");
        return nullptr;
    }
    const auto& transformRenderNode = AceType::DynamicCast<RenderTransform>(transformElement->GetRenderNode());
    if (!transformRenderNode) {
        LOGE("Get transformElement failed. transform render node is null.");
        return nullptr;
    }
    return transformElement;
}

void TweenElement::CreatePropertyAnimationFloat(const RefPtr<PropertyAnimatable>& animatable, TweenOption& option)
{
    if (!animatable) {
        LOGE("Create property animation for float failed. animatable is null.");
        return;
    }
    auto& propertyFloatMap = option.GetFloatPropertyAnimation();
    if (propertyFloatMap.empty()) {
        LOGD("No property animation float found. skip it.");
        return;
    }
    for (auto&& [property, animation] : propertyFloatMap) {
        LOGD("Create animation float for property: %{public}d", property);
        CreatePropertyAnimation<FloatPropertyAnimatable, float>(animatable, property, option, animation);
    }
}

} // namespace OHOS::Ace
