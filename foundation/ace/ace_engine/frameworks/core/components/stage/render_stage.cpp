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

#include "core/components/stage/render_stage.h"

#include "base/utils/system_properties.h"
#include "core/components/common/properties/page_transition_option.h"
#include "core/components/stage/stage_element.h"

namespace OHOS::Ace {
namespace {

constexpr double DRAG_LIMIT = 200.0;
constexpr int32_t LEAST_DRAG_BACK_PAGES = 2;

RefPtr<StageElement> GetStageElement(const WeakPtr<PipelineContext>& contextWeak)
{
    auto context = contextWeak.Upgrade();
    if (!context) {
        LOGE("Notify drag back failed. pipeline context is null.");
        return nullptr;
    }
    auto stageElement = context->GetStageElement();
    return stageElement;
}

void SetPageHidden(const RefPtr<Element>& element, bool hidden)
{
    // first try with page element.
    RefPtr<PageElement> page = AceType::DynamicCast<PageElement>(element);
    if (!page) {
        return;
    }
    page->SetHidden(hidden);
}

} // namespace

RefPtr<RenderNode> RenderStage::Create()
{
    return AceType::MakeRefPtr<RenderStage>();
}

void RenderStage::Update(const RefPtr<Component>& component)
{
    RenderStack::Update(component);
    if (component) {
        isRightToLeft_ = component->GetTextDirection() == TextDirection::RTL;
    }
    WatchDragToBack();
}

void RenderStage::WatchDragToBack()
{
    dragDetector_ = AceType::MakeRefPtr<HorizontalDragRecognizer>();

    dragDetector_->SetOnDragStart([weakRenderStage = WeakClaim(this)](const DragStartInfo& info) {
        auto stage = weakRenderStage.Upgrade();
        if (!stage || stage->GetDisableTouchEvent()) {
            LOGW("touch is not allowed at this time.");
            return;
        }
        LOGI("Drag start to back.");
        stage->HandleDragStart();
    });
    dragDetector_->SetOnDragUpdate([weakRenderStage = WeakClaim(this)](const DragUpdateInfo& info) {
        auto stage = weakRenderStage.Upgrade();
        if (!stage || stage->GetDisableTouchEvent()) {
            LOGW("touch is not allowed at this time.");
            return;
        }
        stage->GetControllers();
        stage->HandleDragUpdate(info.GetMainDelta());
    });
    dragDetector_->SetOnDragEnd([weakRenderStage = WeakClaim(this)](const DragEndInfo& info) {
        auto stage = weakRenderStage.Upgrade();
        if (!stage || stage->GetDisableTouchEvent()) {
            LOGW("touch is not allowed at this time.");
            return;
        }
        stage->HandleDragEnd();
    });
    dragDetector_->SetOnDragCancel([weakRenderStage = WeakClaim(this)]() {
        auto stage = weakRenderStage.Upgrade();
        if (!stage || stage->GetDisableTouchEvent()) {
            LOGW("touch is not allowed at this time.");
            return;
        }
        // do the same as drag end when drag cancel happens.
        stage->HandleDragEnd();
    });
}

void RenderStage::HandleDragUpdate(double deltaX)
{
    if (forbidSwipeToRight_) {
        LOGE("Swipe to right is forbidden.");
        return;
    }

    auto pipelineContext = context_.Upgrade();
    if (!pipelineContext) {
        LOGE("HandleDragUpdate : context is null.");
        return;
    }
    double rootWidth = pipelineContext->GetRootWidth();
    if (NearZero(rootWidth)) {
        LOGE("root width is zero.");
        return;
    }
    if (isRightToLeft_) {
        dragOffsetX_ = std::clamp(dragOffsetX_ + deltaX, -rootWidth, 0.0);
    } else {
        dragOffsetX_ = std::clamp(dragOffsetX_ + deltaX, 0.0, rootWidth);
    }

    // calculate the time based on the drag distance
    tickTime_ = (rootWidth - fabs(dragOffsetX_)) / rootWidth * TRANSITION_WATCH_DURATION;

    auto stageElement = GetStageElement(context_);
    if (!stageElement) {
        LOGE("stageElement is null.");
        return;
    }

    auto children = stageElement->GetChildren();
    if (children.size() < LEAST_DRAG_BACK_PAGES) {
        LOGE("children size less than two.");
        return;
    }
    auto childIter = children.rbegin();
    auto topElement = *childIter++;
    auto nextTopElement = *childIter++;
    SetPageHidden(nextTopElement, NearEqual(tickTime_, TRANSITION_WATCH_DURATION));
    if (!controllerIn_ || !controllerOut_) {
        LOGE("HandleDragUpdate : controllerIn or controllerOut is null.");
        return;
    }
    controllerIn_->NotifyStartListener();
    controllerOut_->NotifyStartListener();
    controllerIn_->TriggerFrame(tickTime_);
    controllerOut_->TriggerFrame(tickTime_);
}

void RenderStage::HandleDragStart()
{
    if (forbidSwipeToRight_) {
        LOGE("Swipe to right is forbidden.");
        return;
    }

    auto stageElement = GetStageElement(context_);
    if (!stageElement) {
        LOGE("Notify drag back failed. stageElement is null.");
        return;
    }

    auto children = stageElement->GetChildren();
    if (children.size() < LEAST_DRAG_BACK_PAGES) {
        LOGE("Notify drag back failed. children size less than two.");
        return;
    }
    auto childIter = children.rbegin();
    auto topElement = *childIter++;
    auto nextTopElement = *childIter++;
    auto transitionIn = PageTransitionElement::GetTransitionElement(topElement);
    auto transitionOut = PageTransitionElement::GetTransitionElement(nextTopElement);
    if (!transitionIn || !transitionOut) {
        LOGE("transitionIn or transitionOut is null.");
        return;
    }
    if (!stageElement->InitTransition(transitionIn, transitionOut, TransitionEvent::POP_START)) {
        LOGE("Notify drag back failed. Init transition failed.");
    }
}

void RenderStage::HandleDragEnd()
{
    if (forbidSwipeToRight_) {
        LOGE("Swipe to right is forbidden.");
        return;
    }

    if (!controllerIn_ || !controllerOut_) {
        LOGE("HandleDragEnd : controllerIn or controllerOut is null.");
        return;
    }
    auto pipelineContext = context_.Upgrade();
    if (!pipelineContext) {
        LOGE("HandleDragEnd : context is null.");
        return;
    }
    controllerIn_->NotifyStopListener();
    controllerOut_->NotifyStopListener();
    SetDisableTouchEvent(true);
    auto dragLimit = DRAG_LIMIT / pipelineContext->GetViewScale();
    if (fabs(dragOffsetX_) >= dragLimit) {
        controllerIn_->UpdatePlayedTime(TRANSITION_WATCH_DURATION - tickTime_);
        controllerOut_->UpdatePlayedTime(TRANSITION_WATCH_DURATION - tickTime_);
        pipelineContext->CallRouterBackToPopPage();
    } else {
        controllerIn_->Forward();
        controllerOut_->Forward();
        controllerIn_->AddStopListener([weakRenderStage = WeakClaim(this), contextWeak = context_] {
            auto stage = weakRenderStage.Upgrade();
            if (stage) {
                stage->SetDisableTouchEvent(false);
            }
            auto stageElement = GetStageElement(contextWeak);
            if (!stageElement) {
                LOGE("Recovery touchable failed. stageElement is null.");
                return;
            }

            auto children = stageElement->GetChildren();
            if (children.size() < LEAST_DRAG_BACK_PAGES) {
                LOGE("Recovery touchable failed. children size less than two.");
                return;
            }
            auto childIter = children.rbegin();
            auto topElement = *childIter++;
            auto nextTopElement = *childIter++;
            auto transitionIn = PageTransitionElement::GetTransitionElement(topElement);
            auto transitionOut = PageTransitionElement::GetTransitionElement(nextTopElement);
            if (!transitionIn || !transitionOut) {
                LOGE("transitionIn or transitionOut is null.");
                return;
            }
            transitionIn->SetTouchable(true);
            transitionOut->SetTouchable(false);
            SetPageHidden(nextTopElement, true);
        });
    }
    // reset drag offset
    dragOffsetX_ = 0.0;
}

void RenderStage::OnTouchTestHit(
    const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    if (dragDetector_) {
        dragDetector_->SetCoordinateOffset(coordinateOffset);
        result.emplace_back(dragDetector_);
    }
}

void RenderStage::GetControllers()
{
    controllerIn_.Reset();
    controllerOut_.Reset();
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("GetControllers : context is null.");
        return;
    }
    RefPtr<StageElement> stageElement = context->GetStageElement();
    if (!stageElement) {
        LOGE("GetControllers : stageElement is null.");
        return;
    }

    auto children = stageElement->GetChildren();
    if (children.size() < LEAST_DRAG_BACK_PAGES) {
        LOGE("GetControllers : children size less than two.");
        return;
    }
    auto childIter = children.rbegin();
    auto topElement = *(childIter++);
    auto nextTopElement = *(childIter++);
    auto transitionIn = PageTransitionElement::GetTransitionElement(topElement);
    auto transitionOut = PageTransitionElement::GetTransitionElement(nextTopElement);

    if (!transitionIn || !transitionOut) {
        LOGE("transitionIn or transitionOut is null.");
        return;
    }
    controllerIn_ = transitionIn->GetTransitionController();
    controllerOut_ = transitionOut->GetTransitionController();
}

} // namespace OHOS::Ace