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

#include "core/common/focus_animation_manager.h"

#include "core/components/focus_animation/render_focus_animation.h"
#include "core/components/shadow/render_shadow.h"
#include "core/components/shadow/shadow_element.h"
#include "core/components/stack/stack_element.h"

namespace OHOS::Ace {

void FocusAnimationManager::SetFocusAnimationProperties(
    const RRect& rrect, const Color& color, const Offset& offset, bool isIndented) const
{
    if (focusAnimationStack_.empty() || (useRoot_ && rootFocusAnimationStack_.empty())) {
        LOGD("focus animation stack is empty");
        return;
    }
    auto focusAnimation = useRoot_ ? rootFocusAnimationStack_.top().Upgrade() : focusAnimationStack_.top().Upgrade();
    if (!focusAnimation) {
        LOGE("focusAnimation get failed");
        return;
    }
    if (useRoot_) {
        auto renderFocusAnimation = focusAnimationStack_.top().Upgrade();
        if (renderFocusAnimation) {
            renderFocusAnimation->CancelFocusAnimation();
        }
    }
    focusAnimation->SetFocusAnimationProperties(rrect, color, offset, isIndented);
}

void FocusAnimationManager::SetAvailableRect(const Rect& paintRect)
{
    availableRect_ = paintRect;
}

void FocusAnimationManager::CancelFocusAnimation() const
{
    if (focusAnimationStack_.empty() || (useRoot_ && rootFocusAnimationStack_.empty())) {
        LOGD("focus animation stack is empty");
        return;
    }
    auto focusAnimation = useRoot_ ? rootFocusAnimationStack_.top().Upgrade() : focusAnimationStack_.top().Upgrade();
    if (!focusAnimation) {
        LOGE("focusAnimation get failed");
        return;
    }
    focusAnimation->CancelFocusAnimation();
}

void FocusAnimationManager::PushFocusAnimationElement(const RefPtr<Element>& element)
{
    auto focusElement = AceType::DynamicCast<FocusAnimationElement>(element);
    if (!focusElement) {
        LOGE("fail to get FocusAnimationElement");
        return;
    }
    auto renderFocus = AceType::DynamicCast<RenderFocusAnimation>(focusElement->GetRenderNode());
    if (!renderFocus) {
        LOGE("fail to get RenderFocusAnimation");
        return;
    }
    renderFocus->SetPaintRect(availableRect_);
    if (focusElement->IsRoot()) {
        if (!rootFocusAnimationStack_.empty()) {
            auto focusAnimation = rootFocusAnimationStack_.top().Upgrade();
            if (focusAnimation) {
                focusAnimation->CancelFocusAnimation();
            }
        }
        rootFocusAnimationStack_.push(renderFocus);
    } else {
        if (!focusAnimationStack_.empty()) {
            auto focusAnimation = focusAnimationStack_.top().Upgrade();
            if (focusAnimation) {
                focusAnimation->CancelFocusAnimation();
            }
        }
        focusAnimationStack_.push(renderFocus);
    }
}

void FocusAnimationManager::PopFocusAnimationElement()
{
    if (focusAnimationStack_.empty()) {
        LOGD("focus animation stack is empty");
        return;
    }
    focusAnimationStack_.pop();
    if (!focusAnimationStack_.empty()) {
        auto focusAnimation = focusAnimationStack_.top().Upgrade();
        if (!useRoot_ && focusAnimation) {
            focusAnimation->StartFocusAnimation();
        }
    }
}

void FocusAnimationManager::PopRootFocusAnimationElement()
{
    if (rootFocusAnimationStack_.empty()) {
        LOGD("focus animation stack is empty");
        return;
    }
    rootFocusAnimationStack_.pop();
}

void FocusAnimationManager::StartFocusAnimation() const
{
    if (focusAnimationStack_.empty()) {
        LOGD("focus animation stack is empty");
        return;
    }
    auto focusAnimation = focusAnimationStack_.top().Upgrade();
    if (!focusAnimation) {
        LOGE("focusAnimation get failed");
        return;
    }
    focusAnimation->StartFocusAnimation();
}

void FocusAnimationManager::StopFocusAnimation() const
{
    if (focusAnimationStack_.empty()) {
        LOGD("focus animation stack is empty");
        return;
    }
    auto focusAnimation = focusAnimationStack_.top().Upgrade();
    if (!focusAnimation) {
        LOGE("focusAnimation get failed");
        return;
    }
    focusAnimation->StopFocusAnimation();
}

void FocusAnimationManager::SetFocusAnimationProperties(
    const RRect& rrect, const Color& color, const Offset& offset, const Rect& clipRect) const
{
    if (focusAnimationStack_.empty()) {
        LOGD("focus animation stack is empty");
        return;
    }
    auto focusAnimation = focusAnimationStack_.top().Upgrade();
    if (!focusAnimation) {
        LOGE("focusAnimation get failed");
        return;
    }
    focusAnimation->SetFocusAnimationProperties(rrect, color, offset, clipRect);
}

void FocusAnimationManager::PushShadow(const RefPtr<Element>& element)
{
    auto shadowElement = AceType::DynamicCast<ShadowElement>(element);
    if (!shadowElement) {
        LOGE("fail to get shadowElement");
        return;
    }
    auto renderShadow = AceType::DynamicCast<RenderShadow>(shadowElement->GetRenderNode());
    if (!renderShadow) {
        LOGE("fail to get renderShadow");
        return;
    }
    shadowStack_.push(renderShadow);
}

void FocusAnimationManager::PopShadow()
{
    if (shadowStack_.empty()) {
        LOGE("shadow stack is empty");
        return;
    }
    shadowStack_.pop();
}

void FocusAnimationManager::SetShadowProperties(const RRect& rrect, const Offset& offset)
{
    if (shadowStack_.empty()) {
        LOGE("shadow stack is empty");
        return;
    }
    auto shadow = shadowStack_.top().Upgrade();
    if (!shadow) {
        LOGE("shadow get failed");
        return;
    }
    shadow->SetShadowProperties(rrect, offset);
}

void FocusAnimationManager::SetShadowProperties(const RRect& rrect, const Offset& offset, const Rect& clipRect)
{
    if (shadowStack_.empty()) {
        LOGE("shadow stack is empty");
        return;
    }
    auto shadow = shadowStack_.top().Upgrade();
    if (!shadow) {
        LOGE("shadow get failed");
        return;
    }
    shadow->SetShadowProperties(rrect, offset, clipRect);
}

void FocusAnimationManager::CancelShadow() const
{
    if (shadowStack_.empty()) {
        LOGE("shadow stack is empty");
        return;
    }
    auto shadow = shadowStack_.top().Upgrade();
    if (!shadow) {
        LOGE("shadow get failed");
        return;
    }
    shadow->CancelShadow();
}

RefPtr<RenderFocusAnimation> FocusAnimationManager::GetRenderFocusAnimation() const
{
    if (focusAnimationStack_.empty() || (useRoot_ && rootFocusAnimationStack_.empty())) {
        return nullptr;
    }
    auto focusAnimation = useRoot_ ? rootFocusAnimationStack_.top().Upgrade() : focusAnimationStack_.top().Upgrade();
    if (!focusAnimation) {
        LOGE("focusAnimation get failed");
        return nullptr;
    }
    return focusAnimation;
}

void FocusAnimationManager::SetUseRoot(bool useRoot)
{
    useRoot_ = useRoot;
    if (!useRoot_ && !rootFocusAnimationStack_.empty()) {
        auto focusAnimation = rootFocusAnimationStack_.top().Upgrade();
        if (focusAnimation) {
            focusAnimation->CancelFocusAnimation();
        }
    }
}

void FocusAnimationManager::SetIsKeyEvent(bool isKeyEvent)
{
    if (focusAnimationStack_.empty() || (useRoot_ && rootFocusAnimationStack_.empty())) {
        LOGD("focus animation stack is empty");
        return;
    }
    auto focusAnimation = useRoot_ ? rootFocusAnimationStack_.top().Upgrade() : focusAnimationStack_.top().Upgrade();
    if (!focusAnimation) {
        LOGE("focusAnimation get failed");
        return;
    }
    focusAnimation->SetIsKeyEvent(isKeyEvent);
}

} // namespace OHOS::Ace