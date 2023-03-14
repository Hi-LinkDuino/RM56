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

#include "core/components/select_popup/select_popup_element.h"

#include "core/components/select_popup/render_select_popup.h"
#include "core/components/select_popup/select_popup_component.h"

namespace OHOS::Ace {

void SelectPopupElement::PerformBuild()
{
    auto component = AceType::DynamicCast<SelectPopupComponent>(component_);
    auto context = context_.Upgrade();
    if (!context || !context->GetAccessibilityManager() || !component) {
        LOGE("select: select popup component is null or initialize failed.");
    } else {
        component->Initialize(context->GetAccessibilityManager());
        component->SetRefreshAnimationCallback([weak = WeakClaim(this)](const TweenOption& option, bool isIn) {
            auto element = weak.Upgrade();
            if (element) {
                element->OnRefreshAnimation(option, isIn);
            }
        });
    }
    SoleChildElement::PerformBuild();
    auto tween = GetTween(AceType::Claim(this));
    if (tween && tween->GetController() && component) {
        component->SetAnimationController(tween->GetController());
    }
    auto render = AceType::DynamicCast<RenderSelectPopup>(renderNode_);
    if (render) {
        render->UpdateRenders();
    }
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    if (context && component) {
        auto manager = context->GetAccessibilityManager();
        if (manager) {
            auto node = manager->GetAccessibilityNodeById(component->GetSelectPopupId());
            auto stackElement = component->GetStackElement();
            if (node && stackElement) {
                node->SetZIndexToChild(stackElement->GetChildrenSize());
                manager->ClearNodeRectInfo(node, false);
            }
        }
    }
#endif
}

RefPtr<TweenElement> SelectPopupElement::GetTween(const RefPtr<Element>& element) const
{
    if (!element) {
        return nullptr;
    }

    if (AceType::InstanceOf<TweenElement>(element)) {
        return AceType::DynamicCast<TweenElement>(element);
    }

    for (const auto& child : element->GetChildren()) {
        auto tween = GetTween(child);
        if (tween) {
            return tween;
        }
    }

    return nullptr;
}

void SelectPopupElement::OnRefreshAnimation(const TweenOption& option, bool isIn)
{
    auto tween = GetTween(AceType::Claim(this));
    if (!tween) {
        LOGE("the tween element is null.");
        return;
    }

    auto controller = tween->GetController();
    if (controller) {
        controller->ClearInterpolators();
        controller->ClearAllListeners();
    }
    tween->SetOption(option);
    tween->ApplyOptions();
    tween->ApplyKeyframes();
    isInAnimation_ = isIn;
}

} // namespace OHOS::Ace
