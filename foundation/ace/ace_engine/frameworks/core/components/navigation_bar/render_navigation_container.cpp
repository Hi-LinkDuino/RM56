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

#include "core/components/navigation_bar/render_navigation_container.h"

#include "core/animation/curve_animation.h"

namespace OHOS::Ace {

RefPtr<RenderNode> RenderNavigationContainer::Create()
{
    return AceType::MakeRefPtr<RenderNavigationContainer>();
}

void RenderNavigationContainer::PerformLayout()
{
    auto layoutParam = GetLayoutParam();
    double usedHeight = collapsingNavigationBar_ ? collapsingNavigationBar_->GetPositionY() : 0.0;
    double maxWidth = layoutParam.GetMaxSize().Width();
    double maxHeight = layoutParam.GetMaxSize().Height();
    for (const auto& child : GetChildren()) {
        layoutParam.SetMaxSize(Size(maxWidth, maxHeight - usedHeight));
        child->Layout(layoutParam);
        child->SetPosition(Offset(0.0, usedHeight));
        usedHeight += child->GetLayoutSize().Height();
    }
    SetLayoutSize(GetLayoutParam().GetMaxSize());
}

void RenderNavigationContainer::OnRelatedStart()
{
    if (collapsingNavigationBar_) {
        collapsingNavigationBar_->OnRelatedStart();
    }
}

void RenderNavigationContainer::OnRelatedPreScroll(const Offset& delta, Offset& consumed)
{
    if (collapsingNavigationBar_) {
        collapsingNavigationBar_->OnRelatedPreScroll(delta, consumed);
    }
}

void RenderNavigationContainer::OnRelatedScroll(const Offset& delta, Offset& consumed)
{
    if (collapsingNavigationBar_) {
        collapsingNavigationBar_->OnRelatedScroll(delta, consumed);
    }
}

void RenderNavigationContainer::OnRelatedEnd()
{
    if (collapsingNavigationBar_) {
        collapsingNavigationBar_->OnRelatedEnd();
    }
}

void RenderNavigationContainer::Update(const RefPtr<Component>& component)
{
    auto navigationContainerComponent = AceType::DynamicCast<NavigationContainerComponent>(component);
    if (!navigationContainerComponent) {
        return;
    }
    auto declaration = navigationContainerComponent->GetDeclaration();
    if (!declaration) {
        return;
    }
    title_ = declaration->title;
    subTitle_ = declaration->subTitle;
    hideBackButton_ = declaration->hideBarBackButton;
    hideNavigationBar_ = declaration->hideBar;
    hideNavigationToolBar_ = declaration->hideToolbar;
    titleMode_ = declaration->titleMode;
    toolBarItems_ = declaration->toolbarItems;
}

} // namespace OHOS::Ace
