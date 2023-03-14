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

#include "core/animation/card_transition_controller.h"

#include "core/animation/curve_animation.h"
#include "core/components/list/list_item_element.h"
#include "core/components/list/render_list.h"
#include "core/components/page_transition/page_transition_element.h"
#include "core/components/transform/transform_element.h"

namespace OHOS::Ace {
namespace {

constexpr float OPACITY_CHANGE_END = 0.286f;
constexpr float OPACITY_CHANGE_START = 0.571f;

} // namespace

CardTransitionController::CardTransitionController(const WeakPtr<PipelineContext>& context) : context_(context)
{
    controller_ = AceType::MakeRefPtr<Animator>(context);
};

void CardTransitionController::RegisterTransitionListener()
{
    auto pipelineContext = context_.Upgrade();
    if (!pipelineContext) {
        LOGE("Register Transition listener to stage failed. pipeline context is null.");
        return;
    }
    auto weak = AceType::WeakClaim(this);
    pipelineContext->AddPageTransitionListener(
        [weak](const TransitionEvent& event, const WeakPtr<PageElement>& in, const WeakPtr<PageElement>& out) {
            if ((event != TransitionEvent::POP_START) && (event != TransitionEvent::PUSH_START)) {
                LOGE("handle event: %{public}d failed. unknown event.", event);
                return;
            }
            auto controller = weak.Upgrade();
            if (!controller) {
                LOGE("handle event: %{public}d failed. controller is null.", event);
                return;
            }
            if (event == TransitionEvent::PUSH_START) {
                controller->pageDest_ = in;
                controller->pageSrc_ = out;
            } else {
                controller->pageDest_ = out;
                controller->pageSrc_ = in;
            }
            controller->event_ = event;
            controller->PrepareTransition();
        });
}

void CardTransitionController::PrepareTransition()
{
    // finish previous transition
    controller_->Finish();
    // clear controller
    controller_->ClearAllListeners();
    controller_->ClearInterpolators();
    controller_->ClearPrepareListeners();
    auto pipelineContext = context_.Upgrade();
    if (!pipelineContext) {
        LOGE("prepare card transition failed. pipeline is null.");
        return;
    }
    auto pageDest = (event_ == TransitionEvent::PUSH_START) ? pageDest_.Upgrade() : pageSrc_.Upgrade();
    if (!pageDest || pageDest->GetCardComposeId().empty()) {
        LOGE("prepare card transition failed. page dest is null. event: %{public}d", event_);
        return;
    }
    auto composeId = pageDest->GetCardComposeId();
    auto cardComposeElement = pipelineContext->GetComposedElementById(composeId);
    if (!cardComposeElement) {
        LOGE("prepare card transition failed. cardComposeElement is null.");
        return;
    }
    auto cardRender = cardComposeElement->GetRenderNode();
    if (!cardRender) {
        LOGE("prepare card transition failed. cardRender is null.");
        return;
    }
    auto rrect = GetCardRect(composeId);
    CreateCardAnimation(cardComposeElement, cardRender->GetGlobalOffset(), rrect);
    CreateCardListAnimation(cardRender);
    CreateExternalAnimation(cardRender->GetLayoutSize().Height(), cardRender->GetGlobalOffset().GetY());
}

void CardTransitionController::CreateCardAnimation(const RefPtr<Element>& cardComposeElement,
    const Offset& globalOffset, const RRect& rrect)
{
    auto listItemElement = cardComposeElement->GetFirstChild();
    if (!listItemElement) {
        LOGE("create card animation failed. listItemElement is null.");
        return;
    }
    auto displayElement = AceType::DynamicCast<DisplayElement>(listItemElement->GetFirstChild());
    if (!displayElement) {
        LOGE("create card animation failed. displayElement is null.");
        return;
    }
    auto displayRender = displayElement->GetRenderNode();
    if (!displayRender) {
        LOGE("create card animation failed. displayRender is null.");
        return;
    }
    auto transformElement = GetCardTransformElement(cardComposeElement);
    if (!transformElement) {
        LOGE("create card animation failed. transformElement is null.");
        return;
    }
    auto transformRender = AceType::DynamicCast<RenderTransform>(transformElement->GetRenderNode());
    if (!transformRender) {
        LOGE("create card animation failed. transformRender is null.");
        return;
    }
    auto weakTransform = AceType::WeakClaim(AceType::RawPtr(transformRender));
    CreateCardScaleAnimation(weakTransform, rrect);
    CreateCardTranslateAnimation(weakTransform, globalOffset, rrect.GetRect().GetOffset() - globalOffset);
    CreateCardOpacityAnimation(displayRender);
    AddPrepareListener(weakTransform);
    AddStopListener(weakTransform);
}

void CardTransitionController::CreateCardTranslateAnimation(const WeakPtr<RenderTransform>& weakTransform,
    const Offset& globalOffset, const Offset& marginOffset)
{
    auto pipelineContext = context_.Upgrade();
    if (!pipelineContext) {
        LOGE("create card translate animation failed. pipeline is null.");
        return;
    }
    Offset startOffset = (event_ == TransitionEvent::PUSH_START) ? Offset() : Offset() - (globalOffset + marginOffset);
    Offset endOffset = (event_ == TransitionEvent::PUSH_START) ? Offset() - (globalOffset + marginOffset) : Offset();
    auto offsetAnimation = AceType::MakeRefPtr<CurveAnimation<Offset>>(startOffset, endOffset, Curves::FRICTION);
    offsetAnimation->AddListener([weakTransform](const Offset& offset) {
        auto transform = weakTransform.Upgrade();
        if (transform) {
            transform->Translate(Dimension(offset.GetX()), Dimension(offset.GetY()));
        }
    });
    controller_->AddInterpolator(offsetAnimation);
}

void CardTransitionController::CreateCardScaleAnimation(const WeakPtr<RenderTransform>& weakTransform,
    const RRect& rrect)
{
    auto pipelineContext = context_.Upgrade();
    if (!pipelineContext) {
        LOGE("create card scale animation failed. pipeline is null.");
        return;
    }
    auto transform = weakTransform.Upgrade();
    if (!transform) {
        LOGE("create card scale animation failed. transform is null.");
        return;
    }
    auto width = rrect.Width();
    if (NearEqual(width, 0.0)) {
        LOGE("create card animation failed. width is zero.");
        return;
    }
    transform->SetDisableClickEffect(true);
    transform->ResetTransform();
    transform->SetTransformOrigin(Dimension(), Dimension());
    transform->MarkNeedUpdateOrigin();

    double startScale = (event_ == TransitionEvent::PUSH_START) ? 1.0 : pipelineContext->GetRootWidth() / width;
    double endScale = (event_ == TransitionEvent::PUSH_START) ? pipelineContext->GetRootWidth() / width : 1.0;
    auto scaleAnimation = AceType::MakeRefPtr<CurveAnimation<double>>(startScale, endScale, Curves::FRICTION);
    scaleAnimation->AddListener([weakTransform](double value) {
        auto transform = weakTransform.Upgrade();
        if (transform) {
            transform->Scale(value);
        }
    });
    controller_->AddInterpolator(scaleAnimation);
}

void CardTransitionController::CreateCardOpacityAnimation(RefPtr<RenderNode>& displayRender)
{
    auto opacityAnimation = AceType::MakeRefPtr<KeyframeAnimation<uint8_t>>();
    if (event_ == TransitionEvent::PUSH_START) {
        auto opacityKeyframe1 = AceType::MakeRefPtr<Keyframe<uint8_t>>(0.0f, 255);
        auto opacityKeyframe2 = AceType::MakeRefPtr<Keyframe<uint8_t>>(OPACITY_CHANGE_END, 0);
        opacityKeyframe2->SetCurve(Curves::SHARP);
        auto opacityKeyframe3 = AceType::MakeRefPtr<Keyframe<uint8_t>>(1.0f, 0);
        opacityAnimation->AddKeyframe(opacityKeyframe1);
        opacityAnimation->AddKeyframe(opacityKeyframe2);
        opacityAnimation->AddKeyframe(opacityKeyframe3);
    } else {
        auto opacityKeyframe1 = AceType::MakeRefPtr<Keyframe<uint8_t>>(0.0f, 0);
        auto opacityKeyframe2 = AceType::MakeRefPtr<Keyframe<uint8_t>>(OPACITY_CHANGE_START, 0);
        auto opacityKeyframe3 = AceType::MakeRefPtr<Keyframe<uint8_t>>(1.0f, 255);
        opacityKeyframe3->SetCurve(Curves::SHARP);
        opacityAnimation->AddKeyframe(opacityKeyframe1);
        opacityAnimation->AddKeyframe(opacityKeyframe2);
        opacityAnimation->AddKeyframe(opacityKeyframe3);
    }
    opacityAnimation->AddListener([displayWeak = AceType::WeakClaim(AceType::RawPtr(displayRender))](uint8_t value) {
        auto display = displayWeak.Upgrade();
        if (display) {
            display->UpdateOpacity(value);
        }
    });
    controller_->AddInterpolator(opacityAnimation);
}

void CardTransitionController::CreateCardListAnimation(const RefPtr<RenderNode>& renderNode)
{
    auto renderListItem = AceType::DynamicCast<RenderListItem>(renderNode);
    if (!renderListItem) {
        LOGD("list item is null");
        return;
    }
    auto height = renderListItem->GetLayoutSize().Height();
    auto start = (event_ == TransitionEvent::PUSH_START) ? 0.0 : height;
    auto end = (event_ == TransitionEvent::PUSH_START) ? height : 0.0;
    auto heightAnimation = AceType::MakeRefPtr<CurveAnimation<double>>(start, end, Curves::FRICTION);
    auto listItemWeak = AceType::WeakClaim(AceType::RawPtr(renderListItem));
    heightAnimation->AddListener([listItemWeak](double value) {
        auto renderListItem = listItemWeak.Upgrade();
        if (renderListItem) {
            renderListItem->RunCardTransitionAnimation(value);
        }
    });
    controller_->AddInterpolator(heightAnimation);
    controller_->AddStopListener([listItemWeak] {
        auto renderListItem = listItemWeak.Upgrade();
        if (renderListItem) {
            renderListItem->StopCardTransitionAnimation();
        }
    });
}

void CardTransitionController::CreateExternalAnimation(double height, double cardOffsetY)
{
    auto pageSrc = (event_ == TransitionEvent::PUSH_START) ? pageSrc_.Upgrade() : pageDest_.Upgrade();
    if (!pageSrc) {
        return;
    }
    const auto& transformMap = pageSrc->GetCardTransitionMap();
    for (auto& item : transformMap) {
        auto& transformWeak = item.second;
        auto transformElement = transformWeak.Upgrade();
        if (!transformElement) {
            continue;
        }
        auto transformRender = AceType::DynamicCast<RenderTransform>(transformElement->GetRenderNode());
        if (!transformRender) {
            continue;
        }
        auto transformOffsetY = transformRender->GetGlobalOffset().GetY();
        if (NearEqual(transformOffsetY, cardOffsetY)) {
            continue;
        }
        auto shiftHeight = height;
        if (transformOffsetY < cardOffsetY) {
            shiftHeight = -height;
        }
        auto start = (event_ == TransitionEvent::PUSH_START) ? 0.0 : shiftHeight;
        auto end = (event_ == TransitionEvent::PUSH_START) ? shiftHeight : 0.0;
        auto positionAnimation = AceType::MakeRefPtr<CurveAnimation<double>>(start, end, Curves::FRICTION);
        auto weakTransform = AceType::WeakClaim(AceType::RawPtr(transformRender));
        positionAnimation->AddListener([weakTransform](double value) {
            auto transform = weakTransform.Upgrade();
            if (transform) {
                transform->ResetTransform();
                transform->Translate(Dimension(), Dimension(value));
            }
        });
        controller_->AddInterpolator(positionAnimation);
        controller_->AddStopListener([weakTransform, event = event_] {
            auto transform = weakTransform.Upgrade();
            if (transform && event == TransitionEvent::PUSH_START) {
                transform->ResetTransform();
            }
        });
    }
}

void CardTransitionController::AddPrepareListener(const WeakPtr<RenderTransform>& weakTransform)
{
    controller_->AddPrepareListener([weakTransform, weakContext = context_, weak = AceType::WeakClaim(this)]() {
        auto context = weakContext.Upgrade();
        auto cardTransition = weak.Upgrade();
        auto transform = weakTransform.Upgrade();
        if (context && cardTransition && transform) {
            auto currentTimestamp = context->GetTimeFromExternalTimer();
            if (cardTransition->currentTimestamp_ != currentTimestamp || cardTransition->currentTimestamp_ == 0) {
                transform->ResetTransform();
                cardTransition->currentTimestamp_ = currentTimestamp;
            }
        }
    });
}

void CardTransitionController::AddStopListener(const WeakPtr<RenderTransform>& weakTransform)
{
    controller_->AddStopListener([weakTransform] {
        auto transform = weakTransform.Upgrade();
        if (transform) {
            transform->ResetTransform();
            transform->SetTransformOrigin(Dimension(transform->GetLayoutSize().Width() * 0.5),
                Dimension(transform->GetLayoutSize().Height() * 0.5));
            transform->MarkNeedUpdateOrigin();
            transform->SetDisableClickEffect(false);
        }
    });
}

RefPtr<Element> CardTransitionController::GetCardTransformElement(const RefPtr<Element>& element)
{
    auto childElement = element->GetFirstChild();
    while (childElement) {
        auto composedElement =  AceType::DynamicCast<ComposedElement>(childElement);
        if (composedElement) {
            return nullptr;
        }
        auto transformElement =  AceType::DynamicCast<TransformElement>(childElement);
        if (transformElement) {
            return transformElement;
        }
        childElement = childElement->GetFirstChild();
    }
    return nullptr;
}

RRect CardTransitionController::GetCardRect(const ComposeId& composeId) const
{
    auto pipelineContext = context_.Upgrade();
    if (!pipelineContext) {
        LOGE("get card rect failed. pipeline is null.");
        return RRect();
    }
    auto cardComposeElement = pipelineContext->GetComposedElementById(composeId);
    if (!cardComposeElement) {
        return RRect();
    }
    auto renderListItem = AceType::DynamicCast<RenderListItem>(cardComposeElement->GetRenderNode());
    if (!renderListItem) {
        LOGD("list item is null");
        return RRect();
    }
    return renderListItem->GetRRect();
}

} // namespace OHOS::Ace
