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

#include "core/components/navigation_bar/navigation_container_element.h"

#include "core/components/navigation_bar/navigation_container_component.h"
#include "frameworks/bridge/declarative_frontend/declarative_frontend.h"
#include "frameworks/core/components/navigator/navigator_element.h"

namespace OHOS::Ace {

void NavigationContainerElement::ConnectNavigator(const RefPtr<StageElement>& targetContainer)
{
    std::stack<RefPtr<Element>> stack;
    stack.push(AceType::Claim(this));
    bool isFirstNavigator = true;
    while (!stack.empty()) {
        auto element = stack.top();
        stack.pop();
        auto navigator = AceType::DynamicCast<NavigatorElement>(element);
        if (navigator) {
            navigator->SetTargetContainer(targetContainer);
            if (isFirstNavigator) {
                auto context = GetContext().Upgrade();
                if (!context) {
                    LOGE("context is null");
                    return;
                }
                navigator->OnClick();
                isFirstNavigator = false;
            }
        } else {
            auto elementChildren = element->GetChildren();
            for (auto iter = elementChildren.rbegin(); iter != elementChildren.rend(); ++iter) {
                stack.push(*iter);
            }
        }
    }
}

void NavigationContainerElement::PerformBuild()
{
    auto navigationContainer = AceType::DynamicCast<NavigationContainerComponent>(component_);
    if (!navigationContainer) {
        return;
    }

    auto declaration = navigationContainer->GetDeclaration();
    if (!declaration) {
        ComponentGroupElement::PerformBuild();
        return;
    }
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }

    auto animationOption = context->GetExplicitAnimationOption();
    if (animationOption.GetDuration() != 0) {
        declaration->animationOption = animationOption;
    }
    navigationContainer->Build(context_, navigationContainer->GetMenuCount());
    ComponentGroupElement::PerformBuild();

    auto tabController = navigationContainer->GetTabController();
    if (tabController) {
        tabBarChangeListener_ = [declaration, weakContent = context_, tabController](int32_t index) {
            if (!declaration) {
                return;
            }
            if (index < 0 || index >= static_cast<int32_t>(declaration->toolbarItems.size())) {
                return;
            }
            auto pos = declaration->toolbarItems.begin();
            std::advance(pos, index);
            if (!(*pos)->action.IsEmpty()) {
                AceAsyncEvent<void()>::Create((*pos)->action, weakContent)();
            }

            auto pipelineContext = weakContent.Upgrade();
            if (!pipelineContext) {
                return;
            }
            pipelineContext->ScheduleUpdate(NavigationContainerComponent::BuildToolBar(declaration, tabController));
        };
        tabController->SetTabBarChangeListener(tabBarChangeListener_);
    }

    auto lastChild = GetChildren().back();
    if (lastChild) {
        auto stageElement = AceType::DynamicCast<StageElement>(lastChild->GetChildren().back());
        if (!stageElement) {
            LOGE("stage is null");
            return;
        }
        ConnectNavigator(stageElement);
    }
}

} // namespace OHOS::Ace
