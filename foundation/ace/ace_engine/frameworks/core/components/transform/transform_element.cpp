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

#include "core/components/transform/transform_element.h"

#include "core/components/page/page_element.h"

namespace OHOS::Ace {

TransformElement::~TransformElement()
{
    auto page = pageElement_.Upgrade();
    if (page) {
        page->RemoveCardTransition(GetRetakeId());
    }
}

void TransformElement::PerformBuild()
{
    RefPtr<TransformComponent> transform = AceType::DynamicCast<TransformComponent>(component_);
    const auto& child = children_.empty() ? nullptr : children_.front();
    const auto& newComponent = transform ? transform->GetChild() : nullptr;
    UpdateChild(child, newComponent);
    if (transform) {
        AddCardTransform(transform->GetTransitionEffect());
    }
}

void TransformElement::AddCardTransform(TransitionEffect option)
{
    if (option == TransitionEffect::UNFOLD) {
        auto page = SearchParentPage();
        if (!page) {
            return;
        }
        pageElement_ = page;
        page->AddCardTransition(AceType::Claim(this));
    }
}

RefPtr<PageElement> TransformElement::SearchParentPage() const
{
    auto parent = GetElementParent().Upgrade();
    while (parent) {
        auto page = AceType::DynamicCast<PageElement>(parent);
        if (page) {
            LOGD("Find parent page. page id: %{public}d", page->GetPageId());
            return page;
        }
        parent = parent->GetElementParent().Upgrade();
    }
    LOGW("No parent page found.");
    return nullptr;
}

} // namespace OHOS::Ace