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

#include "core/components/swiper/swiper_element.h"

#include "core/components/display/display_component.h"
#include "core/components/swiper/render_swiper.h"
#include "core/components/swiper/swiper_component.h"

namespace OHOS::Ace {
namespace {

constexpr int32_t ELEMENT_CHANGE_END_LISTENER_KEY = 1002;
constexpr int32_t INDICATOR_FOCUS_INDEX = 1;

} // namespace

void SwiperElement::PerformBuild()
{
    auto swiperComponent = AceType::DynamicCast<SwiperComponent>(component_);
    if (!swiperComponent) {
        LOGE("get swiper component failed!");
        return;
    }
    axis_ = swiperComponent->GetAxis();
    auto indicator = swiperComponent->GetIndicator();
    if (!indicator) {
        showIndicator_ = false;
    }

    if (showIndicator_ && !indicatorFocusNode_) {
        indicatorFocusNode_ = AceType::MakeRefPtr<FocusNode>();
        FocusGroup::AddChild(indicatorFocusNode_);
        registerCallBack();
    }
    ComponentGroupElement::PerformBuild();
}

void SwiperElement::registerCallBack()
{
    if (!indicatorFocusNode_) {
        return;
    }
    indicatorFocusNode_->SetOnFocusCallback([weak = WeakClaim(this)](void) {
        auto client = weak.Upgrade();
        if (!client) {
            return;
        }
        auto weakContext = client->GetContext();
        auto context = weakContext.Upgrade();
        if (context) {
            client->HandleIndicatorFocus(true && context->IsKeyEvent());
        }
    });

    indicatorFocusNode_->SetOnBlurCallback([weak = WeakClaim(this)](void) {
        auto client = weak.Upgrade();
        if (client) {
            client->HandleIndicatorFocus(false);
        }
    });
}

bool SwiperElement::IsFocusable() const
{
    auto swiper = DynamicCast<RenderSwiper>(renderNode_);
    if (!swiper) {
        LOGE("get render node failed");
        return false;
    }

    if (showIndicator_) {
        return true;
    } else {
        int32_t currentIndex = swiper->GetCurrentIndex();
        auto currentFocusNode = focusNodes_.begin();
        std::advance(currentFocusNode, currentIndex);
        if (currentFocusNode == focusNodes_.end()) {
            LOGE("target focus node is null");
            return false;
        }
        return (*currentFocusNode)->IsFocusable();
    }
}

void SwiperElement::OnFocus()
{
    auto swiper = DynamicCast<RenderSwiper>(renderNode_);
    if (!swiper) {
        LOGE("get render node failed");
        itLastFocusNode_ = focusNodes_.end();
        return;
    }
    swiper->OnStatusChanged(RenderStatus::FOCUS);

    if (showIndicator_) {
        auto currentFocusNode = focusNodes_.begin();
        if ((*currentFocusNode)->RequestFocusImmediately()) {
            itLastFocusNode_ = currentFocusNode;
            indicatorFocusNode_ = *itLastFocusNode_;
        }
    } else {
        int32_t currentIndex = swiper->GetCurrentIndex();
        auto currentFocusNode = focusNodes_.begin();
        std::advance(currentFocusNode, currentIndex);
        if (currentFocusNode != focusNodes_.end()) {
            if ((*currentFocusNode)->RequestFocusImmediately()) {
                itLastFocusNode_ = currentFocusNode;
                swiper->OnFocus();
            } else {
                // Not found any focusable node, clear focus.
                itLastFocusNode_ = focusNodes_.end();
            }
        }
    }
    swiper->RegisterChangeEndListener(ELEMENT_CHANGE_END_LISTENER_KEY, [weak = WeakClaim(this)](int32_t index) {
        auto client = weak.Upgrade();
        if (client) {
            client->RequestChildFocus(index);
        }
    });
}

void SwiperElement::RequestChildFocus(int32_t index)
{
    auto currentFocusNode = focusNodes_.begin();
    if (showIndicator_) {
        std::advance(currentFocusNode, index + INDICATOR_FOCUS_INDEX);
    } else {
        std::advance(currentFocusNode, index);
    }
    if (currentFocusNode != focusNodes_.end()) {
        if ((*currentFocusNode)->RequestFocusImmediately()) {
            itLastFocusNode_ = currentFocusNode;
        }
    }
}

void SwiperElement::OnBlur()
{
    FocusGroup::OnBlur();
    auto swiper = DynamicCast<RenderSwiper>(renderNode_);
    if (swiper) {
        swiper->OnBlur();
        swiper->UnRegisterChangeEndListener(ELEMENT_CHANGE_END_LISTENER_KEY);
        swiper->OnStatusChanged(RenderStatus::BLUR);
    }
}

void SwiperElement::HandleIndicatorFocus(bool isFocus)
{
    auto swiper = DynamicCast<RenderSwiper>(renderNode_);
    if (!swiper) {
        LOGE("get swiper render node failed");
        return;
    }
    swiper->IndicatorShowFocus(isFocus);
}

bool SwiperElement::RequestNextFocus(bool vertical, bool reverse, const Rect& rect)
{
    auto swiper = DynamicCast<RenderSwiper>(renderNode_);
    if (!swiper) {
        return false;
    }
    if (showIndicator_ && (*itLastFocusNode_ == indicatorFocusNode_)) {
        if ((axis_ == Axis::HORIZONTAL && vertical) || (axis_ != Axis::HORIZONTAL && !vertical)) {
            if (reverse) {
                return RequestCurrentItemFocus();
            }
        } else {
            swiper->UpdateIndicatorFocus(true, reverse);
            return true;
        }
    } else {
        if (showIndicator_) {
            if ((axis_ == Axis::HORIZONTAL && vertical) || (axis_ != Axis::HORIZONTAL && !vertical)) {
                if (!reverse) {
                    return RequestIndicatorFocus();
                }
            }
        } else {
            return false;
        }
    }
    return false;
}

bool SwiperElement::RequestIndicatorFocus()
{
    auto swiper = DynamicCast<RenderSwiper>(renderNode_);
    if (!swiper) {
        return false;
    }
    auto currentFocusNode = focusNodes_.begin();
    if ((*currentFocusNode)->RequestFocusImmediately()) {
        itLastFocusNode_ = currentFocusNode;
        indicatorFocusNode_ = *itLastFocusNode_;
        swiper->IndicatorShowFocus(true);
        return true;
    }
    return false;
}

bool SwiperElement::RequestCurrentItemFocus()
{
    auto swiper = DynamicCast<RenderSwiper>(renderNode_);
    if (!swiper) {
        LOGE("get swiper render node failed");
        return false;
    }
    int32_t currentIndex = swiper->GetCurrentIndex();
    auto currentFocusNode = focusNodes_.begin();
    if (showIndicator_) {
        std::advance(currentFocusNode, currentIndex + INDICATOR_FOCUS_INDEX);
    } else {
        std::advance(currentFocusNode, currentIndex);
    }
    if (currentFocusNode != focusNodes_.end()) {
        if ((*currentFocusNode)->RequestFocusImmediately()) {
            itLastFocusNode_ = currentFocusNode;
            swiper->IndicatorShowFocus(false);
            return true;
        }
    }
    return false;
}

bool SwiperElement::OnKeyEvent(const KeyEvent& keyEvent)
{
    if (!IsCurrentFocus()) {
        return false;
    }

    if (itLastFocusNode_ != focusNodes_.end() && (*itLastFocusNode_)->HandleKeyEvent(keyEvent)) {
        return true;
    }

    if (FocusNode::OnKeyEvent(keyEvent)) {
        return true;
    }

    if (keyEvent.action != KeyAction::UP) {
        return false;
    }

    switch (keyEvent.code) {
        case KeyCode::TV_CONTROL_UP:
            return RequestNextFocus(true, true, GetRect());
        case KeyCode::TV_CONTROL_DOWN:
            return RequestNextFocus(true, false, GetRect());
        case KeyCode::TV_CONTROL_LEFT:
            return RequestNextFocus(false, true, GetRect());
        case KeyCode::TV_CONTROL_RIGHT:
            return RequestNextFocus(false, false, GetRect());
        case KeyCode::KEY_TAB:
            return RequestNextFocus(false, false, GetRect()) || RequestNextFocus(true, false, GetRect());
        default:
            return false;
    }
}

} // namespace OHOS::Ace
