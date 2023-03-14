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

#include "core/components/navigation_bar/navigation_bar_element.h"

#include "core/event/ace_event_helper.h"
#include "core/gestures/click_recognizer.h"

namespace OHOS::Ace {
namespace {

#ifndef WEARABLE_PRODUCT
constexpr double MENU_DEFAULT_HEIGHT = 56.0;
#endif

} // namespace

void NavigationBarElement::PerformBuild()
{
    RefPtr<NavigationBarComponent> navigationBar = AceType::DynamicCast<NavigationBarComponent>(component_);
    if (navigationBar) {
        const auto& child = children_.empty() ? nullptr : children_.front();
        auto newBar = navigationBar->Build(context_);
#ifndef WEARABLE_PRODUCT
        BindMoreButtonClickEvent(navigationBar);
        BindClickEventToOptions(navigationBar);
#endif
        UpdateChild(child, newBar);
    }
}

#ifndef WEARABLE_PRODUCT
void NavigationBarElement::BindMoreButtonClickEvent(const RefPtr<NavigationBarComponent>& navigationBar)
{
    auto moreButton = navigationBar->GetMoreButton();
    if (!moreButton) {
        return;
    }
    auto weak = AceType::WeakClaim(this);
    moreClickMarker_ = BackEndEventManager<void()>::GetInstance().GetAvailableMarker();
    BackEndEventManager<void()>::GetInstance().BindBackendEvent(
        moreClickMarker_, [weak = WeakClaim(this), navigationBar]() {
            LOGD("click collapse icon");
            auto menu = navigationBar->GetMenu();
            if (!menu) {
                LOGW("navigation bar not menu");
                return;
            }
            auto showMenuFunc = menu->GetTargetCallback();
            if (!showMenuFunc) {
                return;
            }
            auto element = weak.Upgrade();
            if (!element) {
                LOGW("weak ptr to ref ptr fail");
                return;
            }
            auto context = element->GetContext().Upgrade();
            if (!context) {
                LOGW("context is empty");
                return;
            }
            double morePopupOffsetX = 0.0;
            if (navigationBar->GetTextDirection() == TextDirection::LTR) {
                morePopupOffsetX = context->GetStageRect().Width();
            }
            showMenuFunc(navigationBar->GetId(),
                Offset(morePopupOffsetX,
                       context->NormalizeToPx(Dimension(MENU_DEFAULT_HEIGHT, DimensionUnit::VP))));
        });
    moreButton->SetClickedEventId(moreClickMarker_);
}

void NavigationBarElement::BindClickEventToOptions(const RefPtr<NavigationBarComponent>& navigationBar)
{
    auto menu = navigationBar->GetMenu();
    if (!menu) {
        LOGI("navigation bar not menu");
        return;
    }

    const auto& menuSuccessEvent = menu->GetOnSuccess();
    if (menuSuccessEvent.IsEmpty()) {
        LOGI("navigation bar menu not listen success event");
        return;
    }

    menuSelectedEvent_ = AceAsyncEvent<void(const std::string&)>::Create(menuSuccessEvent, context_);
    for (const auto& menuItem : navigationBar->GetMenuItemsInBar()) {
        menuItem.second.button->SetClickedEventId(menuItem.second.clickEvent);
        std::string eventId = menuItem.second.clickEvent.GetData().eventId;

        auto weak = AceType::WeakClaim(this);
        BackEndEventManager<void()>::GetInstance().BindBackendEvent(
            menuItem.second.clickEvent, [navigationBar, eventId, weak]() {
                LOGD("click menuItem %{public}s in bar", eventId.c_str());
                auto navigationBarElement = weak.Upgrade();
                if (!navigationBarElement) {
                    LOGE("get navigation bar element failed");
                    return;
                }

                auto menuItemsInBar = navigationBar->GetMenuItemsInBar();
                auto menuItemIter = menuItemsInBar.find(eventId);
                if (menuItemIter == menuItemsInBar.end()) {
                    LOGE("not find which item");
                    return;
                }
                std::string selected = menuItemIter->second.value;
                std::string param = std::string(R"("selected",{"value":")").append(selected.append("\"},null"));
                navigationBarElement->menuSelectedEvent_(param);
            });
    }
}
#endif

} // namespace OHOS::Ace
