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

#include "core/components_v2/foreach/lazy_foreach_component.h"

#include "core/components_v2/foreach/lazy_foreach_element.h"

namespace OHOS::Ace::V2 {

RefPtr<Element> LazyForEachComponent::CreateElement()
{
    return AceType::MakeRefPtr<LazyForEachElement>(GetId());
}

size_t LazyForEachComponent::TotalCount()
{
    return expanded_ ? GetChildren().size() : OnGetTotalCount();
}

RefPtr<Component> LazyForEachComponent::GetChildByIndex(size_t index)
{
    if (!expanded_) {
        return OnGetChildByIndex(index);
    }

    const auto& children = GetChildren();
    if (index >= children.size()) {
        return nullptr;
    }

    auto it = children.begin();
    std::advance(it, index);
    return *it;
}

std::list<RefPtr<Component>>& LazyForEachComponent::ExpandChildren()
{
    if (!expanded_) {
        size_t totalCount = OnGetTotalCount();
        for (size_t idx = 0; idx < totalCount; ++idx) {
            children_.emplace_back(OnGetChildByIndex(idx));
        }
        expanded_ = true;
    }
    return children_;
}

} // namespace OHOS::Ace::V2
